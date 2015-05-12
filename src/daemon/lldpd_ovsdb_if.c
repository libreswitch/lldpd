/*
 * Copyright (C) 2015 Hewlett-Packard Development Company, L.P.
 * All Rights Reserved.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License"); you may
 *   not use this file except in compliance with the License. You may obtain
 *   a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *   WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *   License for the specific language governing permissions and limitations
 *   under the License.
 *
 * File: lldpd_ovsdb_if.c
 *
 * Purpose: Main file for integrating lldpd with ovsdb and ovs poll-loop.
 *          Its purpose in life is to provide hooks to lldpd daemon to do
 *          following:
 *
 *                1. During start up, read lldpd related
 *                   configuration data and apply to lldpd.
 *
 *                2. During operations, receive administrative
 *                   configuration changes and apply to lldpd config.
 *
 *                3. Update statistics and neighbor tables periodically
 *                   to database
 *
 */

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/event_struct.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// OVS headers
#include "config.h"
#include "command-line.h"
#include "daemon.h"
#include "dirs.h"
#include "dummy.h"
#include "fatal-signal.h"
#include "poll-loop.h"
#include "stream.h"
#include "timeval.h"
#include "unixctl.h"
#include "openvswitch/vlog.h"
#include "vswitch-idl.h"
#include "coverage.h"
#include "lldpd.h"
#include "openhalon-idl.h"
#include "lldpd_ovsdb_if.h"

COVERAGE_DEFINE(lldpd_ovsdb_if);
VLOG_DEFINE_THIS_MODULE(lldpd_ovsdb_if);

static struct ovsdb_idl *idl;
static unsigned int idl_seqno;
static struct event *timeout_event;
static struct event *nbr_event;
static int system_configured = false;

static void ovs_libevent_cb(evutil_socket_t fd, short what, void *arg);
static void ovs_clear_libevents(void);
static int ovspoll_to_libevent(struct lldpd *cfg);
static void lldpd_run(struct lldpd *cfg);
static void lldpd_wait(void);
static int lldp_nbr_update(void *smap, struct lldpd_port *p_nbr);

static char *appctl_path = NULL;
static struct unixctl_server *appctl;
static unixctl_cb_func lldpd_unixctl_dump;
static unixctl_cb_func halon_lldpd_exit;
bool exiting = false;

/* Mapping of all the interfaces. */
static struct shash all_interfaces = SHASH_INITIALIZER(&all_interfaces);

static void
ovs_clear_libevents()
{
    struct poll_loop *loop = poll_loop();
    struct poll_node *node;

    /* Clear and free all the fd events. */
    HMAP_FOR_EACH (node, hmap_node, &loop->poll_nodes) {
        if(node->event) {
            event_del((struct event*) node->event);
            event_free((struct event*) node->event);
            node->event = NULL;
        }
    }

    /* Clear and free timeout event */
    if(timeout_event) {
        event_del(timeout_event);
        event_free(timeout_event);
        timeout_event = NULL;
    }

    free_poll_nodes(loop);
    loop->timeout_when = LLONG_MAX;
    loop->timeout_where = NULL;
}

static void ovs_libevent_cb(evutil_socket_t fd, short what, void *arg){
    struct lldpd *cfg = arg;
    int retval = 0;
    struct timeval tv;

    ovs_clear_libevents();

    lldpd_run(cfg);
    lldpd_wait();

    retval = ovspoll_to_libevent(cfg);
    if(retval == -1) // Nothing was scheduled, due to connection error, lets schedule cb after 1 sec
    {
        tv.tv_sec =  1;
        tv.tv_usec = 0;
        timeout_event = event_new(cfg->g_base, -1, 0,
            ovs_libevent_cb,
                cfg);
        event_add(timeout_event, &tv);
    }
    return;
}

void ovs_libevent_schedule_nbr(void *arg){
    struct lldpd *cfg = arg;

    nbr_event = event_new(cfg->g_base, -1, 0, ovs_libevent_cb, cfg);
    event_active(timeout_event, EV_TIMEOUT, 1);

    return;
}

static void lldpd_reset(struct lldpd *cfg, struct lldpd_hardware *hw)
{
    /* If hw is NULL we will reset all hw else reset only specific hw */
    if(!hw) {
        struct lldpd_hardware *hardware;
        /* h_flags is set to 0 for each port. We will cleanup all ports */
        TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries)
        hardware->h_flags = 0;
    } else {
        hw->h_flags = 0;
    }
    lldpd_cleanup(cfg);
}

#define CHANGED(x,y) (x != y)
#define CHANGED_STR(x,y) (!(x == y || (x && y && !strcmp(x,y))))
static bool lldpd_apply_tlv_configs(const struct ovsrec_open_vswitch *ovs,
                                    const char *tlv_name,
                                    u_int8_t *g_lldp_tlv_cfg)
{
    bool tlv_setting;
    bool send_update = 0;

    tlv_setting = smap_get_bool(&ovs->other_config, tlv_name,
                                OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_TLV_DEFAULT);
    if ( CHANGED(tlv_setting, *g_lldp_tlv_cfg) ) {
        *g_lldp_tlv_cfg = tlv_setting;
        VLOG_INFO("Configured %s=%d", tlv_name, *g_lldp_tlv_cfg);
        send_update = 1;
    }

    return send_update;
}

/*Check is ip is valid IPv4 or IPv6 address*/
static bool validate_ip(char *ip)
{
    struct interfaces_address *addr;

    if (ip && strpbrk(ip, "!,*?") == NULL) {
        if (inet_pton(LLDPD_AF_IPV4, ip, &addr) == 1 ||
                inet_pton(LLDPD_AF_IPV6, ip, &addr) == 1)
            return true;
    }
    return false;
}

static void
del_old_db_interface(struct shash_node *sh_node)
{
    if (sh_node) {
        struct lldp_iface *itf = sh_node->data;

        /*
         * If the lldp_hardware is also cleaned up
         * we can remove the entry else just nullify
         * ovsdb record handle
         */
        if(itf && !itf->hw) {
            free(itf->name);
            free(sh_node->data);
            shash_delete(&all_interfaces, sh_node);
        } else {
            itf->ifrow = NULL;
        }

    }
} /* del_old_interface */

static void
add_new_db_interface(const struct ovsrec_interface *ifrow)
{
    struct lldp_iface *new_itf = NULL;
    struct shash_node *sh_node = shash_find(&all_interfaces, ifrow->name);
    if(!ifrow)
        return;

    VLOG_DBG("Interface %s being added!\n", ifrow->name);

    if(!sh_node) {
        /* Allocate structure to save state information for this interface. */
        new_itf = xcalloc(1, sizeof *new_itf);

        if (!shash_add_once(&all_interfaces, ifrow->name, new_itf)) {
            VLOG_WARN("Interface %s specified twice", ifrow->name);
            free(new_itf);
        } else {
            new_itf->name = xstrdup(ifrow->name);
            new_itf->ifrow = ifrow;
            VLOG_DBG("Created local data for interface %s", ifrow->name);
        }
    } else {
        new_itf = sh_node->data;
        new_itf->ifrow = ifrow;
    }
} /* add_new_interface */

#define CHK_AND_APPLY_ITF_CONFIG_CHANGE(cfg, itf, val, cfg_changed) \
        if(itf->hw) { \
            if(CHANGED(itf->hw->h_enable_dir, val)) { \
                itf->hw->h_enable_dir = val; \
                lldpd_reset(cfg, itf->hw); \
                lldpd_update_localports(cfg); \
                cfg_changed++; \
            } \
        } \

static int
handle_interfaces_config_mods(struct shash *sh_idl_interfaces, struct lldpd *cfg)
{
    struct shash_node *sh_node;
    int rc = 0;

    /* Loop through all the current interfaces and figure out how many
     * have config changes that need action. */
    SHASH_FOR_EACH(sh_node, &all_interfaces) {
        bool cfg_changed = false;
        struct lldp_iface *itf = sh_node->data;
        const struct ovsrec_interface *ifrow =
            shash_find_data(sh_idl_interfaces, sh_node->name);

        if (ifrow && (OVSREC_IDL_IS_ROW_INSERTED(ifrow, idl_seqno) ||
                OVSREC_IDL_IS_ROW_MODIFIED(ifrow, idl_seqno))) {

            /* Check for other_config:lldp_enable_dir changes. */
            const char *ifrow_other_config_lldp_enable_dir =
                smap_get(&ifrow->other_config,
                        INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR);

            if(ifrow_other_config_lldp_enable_dir) {
                if(strcmp(ifrow_other_config_lldp_enable_dir,
                        INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_OFF) == 0) {
                    CHK_AND_APPLY_ITF_CONFIG_CHANGE(cfg, itf,
                            HARDWARE_ENABLE_DIR_OFF, cfg_changed);
                } else if(strcmp(ifrow_other_config_lldp_enable_dir,
                        INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_RX) == 0) {
                    CHK_AND_APPLY_ITF_CONFIG_CHANGE(cfg, itf,
                            HARDWARE_ENABLE_DIR_RX, cfg_changed);
                } else if(strcmp(ifrow_other_config_lldp_enable_dir,
                        INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_TX) == 0) {
                    CHK_AND_APPLY_ITF_CONFIG_CHANGE(cfg, itf,
                            HARDWARE_ENABLE_DIR_TX, cfg_changed);
                } else if(strcmp(ifrow_other_config_lldp_enable_dir,
                        INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_RXTX) == 0) {
                    CHK_AND_APPLY_ITF_CONFIG_CHANGE(cfg, itf,
                            HARDWARE_ENABLE_DIR_RXTX, cfg_changed);
                }
                VLOG_DBG("lldp status change on interface %s: %s",
                        ifrow->name, ifrow_other_config_lldp_enable_dir);
            } else {
                CHK_AND_APPLY_ITF_CONFIG_CHANGE(cfg, itf,
                        HARDWARE_ENABLE_DIR_RXTX, cfg_changed);
            }

            /* Check for link_state change. */
            bool link_state_bool = false;

            if(ifrow && ifrow->link_state &&
               !strcmp(ifrow->link_state, OVSREC_INTERFACE_LINK_STATE_UP)){
                link_state_bool = true;
            }
            if(itf->hw && CHANGED(itf->hw->h_link_state, link_state_bool)) {
                VLOG_INFO("link state change on interface%s: %s", ifrow->name, ifrow->link_state);
                itf->hw->h_link_state = link_state_bool;
                cfg_changed++;
            }
        }

        if (cfg_changed) {
            /* Update interface configuration. */
                rc++;
        }
    }

    return rc;
} /* handle_interfaces_config_mods */

static void lldpd_apply_interface_changes(struct ovsdb_idl *idl,
        struct lldpd *g_lldp_cfg, bool *send_now)
{
    int rc = 0;
    const struct ovsrec_interface *ifrow;
    struct shash sh_idl_interfaces;
    struct shash_node *sh_node, *sh_next;


    /* Collect all the interfaces in the dB. */
    shash_init(&sh_idl_interfaces);
    OVSREC_INTERFACE_FOR_EACH(ifrow, idl) {
        /*
         * Check if any table changes present.
         * If no change just return from here
         */
        if(!OVSREC_IDL_ANY_TABLE_ROWS_INSERTED(ifrow, idl_seqno) &&
                !OVSREC_IDL_ANY_TABLE_ROWS_DELETED(ifrow, idl_seqno) &&
                !OVSREC_IDL_ANY_TABLE_ROWS_MODIFIED(ifrow, idl_seqno)) {
            VLOG_DBG("No Interface cfg changes");
            /* Destroy the shash of the IDL interfaces */
            shash_destroy(&sh_idl_interfaces);
            return;
        }

        if (!shash_add_once(&sh_idl_interfaces, ifrow->name, ifrow)) {
            VLOG_WARN("interface %s specified twice", ifrow->name);
        }
    }

    /* Delete old interfaces. */
    SHASH_FOR_EACH_SAFE(sh_node, sh_next, &all_interfaces) {
        struct lldp_iface *new_itf =
                shash_find_data(&sh_idl_interfaces, sh_node->name);
        if (!new_itf) {
            del_old_db_interface(sh_node);
        }
    }
    /* Add new interfaces. */
    SHASH_FOR_EACH(sh_node, &sh_idl_interfaces) {
        struct lldp_iface *new_itf = shash_find_data(&all_interfaces,
                sh_node->name);
        if (!new_itf || !new_itf->ifrow) {
            VLOG_DBG("Found an added interface %s", sh_node->name);
            add_new_db_interface(sh_node->data);
        }
    }

    /* Check for interfaces that changed config--and need
     * handling now. */
    rc = handle_interfaces_config_mods(&sh_idl_interfaces, g_lldp_cfg);

    /* Destroy the shash of the IDL interfaces */
    shash_destroy(&sh_idl_interfaces);

    if(rc)
        *send_now = 1;
}

static void lldpd_apply_global_changes(struct ovsdb_idl *idl,
        struct lldpd *g_lldp_cfg, bool *send_now)
{
    int lldp_enabled = -1, tx_timer = 0, tx_hold_multiplier = 0;
    int i;
    bool update_now = 0;
    const struct ovsrec_open_vswitch *ovs;

    ovs = ovsrec_open_vswitch_first(idl);

    /*
     * Check if any table changes present.
     * If no change just return from here
     */
    if(!OVSREC_IDL_ANY_TABLE_ROWS_INSERTED(ovs, idl_seqno) &&
            !OVSREC_IDL_ANY_TABLE_ROWS_DELETED(ovs, idl_seqno) &&
            !OVSREC_IDL_ANY_TABLE_ROWS_MODIFIED(ovs, idl_seqno)) {
        VLOG_DBG("No Open_vSwitch cfg changes");
        return;
    }

    if(ovs) {
        tx_timer = smap_get_int(&ovs->other_config,
                           OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_TX_INTERVAL,
                           OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_TX_INTERVAL_DEFAULT);

        //Check the transmit interval and update.
        if (CHANGED(tx_timer, g_lldp_cfg->g_config.c_tx_interval) &&
                tx_timer >= OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_TX_INTERVAL_MIN &&
                tx_timer <= OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_TX_INTERVAL_MAX) {
            g_lldp_cfg->g_config.c_tx_interval = tx_timer;
            LOCAL_CHASSIS(g_lldp_cfg)->c_ttl = MIN(UINT16_MAX,
                                               (g_lldp_cfg->g_config.c_tx_interval *
                                               g_lldp_cfg->g_config.c_tx_hold) );
            VLOG_INFO("Configured lldp  tx-timer [%d]",
                                             g_lldp_cfg->g_config.c_tx_interval);
            *send_now = 1;
        }

        //Check for hold time and update.
        tx_hold_multiplier = smap_get_int(&ovs->other_config,
                                  OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_HOLD,
                                  OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_HOLD_DEFAULT);
        if (CHANGED(tx_hold_multiplier, g_lldp_cfg->g_config.c_tx_hold) &&
                tx_hold_multiplier >= OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_HOLD_MIN &&
                tx_hold_multiplier <= OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_HOLD_MAX) {
            g_lldp_cfg->g_config.c_tx_hold = tx_hold_multiplier;
            LOCAL_CHASSIS(g_lldp_cfg)->c_ttl = MIN(UINT16_MAX,
                                               (g_lldp_cfg->g_config.c_tx_interval *
                                                g_lldp_cfg->g_config.c_tx_hold) );
            VLOG_INFO("Configured lldp  tx-hold [%d]",
                                                  g_lldp_cfg->g_config.c_tx_hold);
            *send_now = 1;
        }

        const char *lldp_mgmt_pattern = smap_get(&ovs->other_config,
                OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_MGMT_ADDR);
        if(lldp_mgmt_pattern != NULL){
            if(CHANGED_STR(lldp_mgmt_pattern, g_lldp_cfg->g_config.c_mgmt_pattern)) {
                if(validate_ip((char *) lldp_mgmt_pattern)) {
                    if(g_lldp_cfg->g_config.c_mgmt_pattern != NULL)
                        free(g_lldp_cfg->g_config.c_mgmt_pattern);
                    g_lldp_cfg->g_config.c_mgmt_pattern = xstrdup(lldp_mgmt_pattern);
                    VLOG_INFO("Configured lldp  mgmt_pattern is [%s]",lldp_mgmt_pattern);
                    //We need to call update to update the local chassis
                    update_now = 1;
                    *send_now = 1;
                } else {
                    VLOG_INFO("Configured lldp  mgmt_pattern is not a valid IP address [%s]",
                            lldp_mgmt_pattern);
                }
            }
        }

        /* LLDP TLV Configuration */
        if ( lldpd_apply_tlv_configs(ovs,
                          OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_TLV_SYS_NAME_ENABLE,
                          &g_lldp_cfg->g_config.c_lldp_tlv_sys_name_enable) ) {
            *send_now=1;
        }

        if ( lldpd_apply_tlv_configs(ovs,
                          OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_TLV_SYS_DESC_ENABLE,
                          &g_lldp_cfg->g_config.c_lldp_tlv_sys_desc_enable) ) {
            *send_now=1;
        }

        if ( lldpd_apply_tlv_configs(ovs,
                          OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_TLV_SYS_CAP_ENABLE,
                          &g_lldp_cfg->g_config.c_lldp_tlv_sys_cap_enable) ) {
            *send_now=1;
        }

        if ( lldpd_apply_tlv_configs(ovs,
                          OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_TLV_MGMT_ADDR_ENABLE,
                          &g_lldp_cfg->g_config.c_lldp_tlv_mgmt_addr_enable) ) {
            *send_now=1;
        }

        if ( lldpd_apply_tlv_configs(ovs,
                          OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_TLV_PORT_DESC_ENABLE,
                          &g_lldp_cfg->g_config.c_lldp_tlv_port_desc_enable) ) {
            *send_now=1;
        }

        if ( lldpd_apply_tlv_configs(ovs,
                       OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_TLV_PORT_VLAN_ID_ENABLE,
                       &g_lldp_cfg->g_config.c_lldp_tlv_port_vlanid_enable) ) {
            *send_now=1;
        }

        if ( lldpd_apply_tlv_configs(ovs,
                  OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_TLV_PORT_PROTO_VLAN_ID_ENABLE,
                  &g_lldp_cfg->g_config.c_lldp_tlv_port_proto_vlanid_enable) ) {
            *send_now=1;
        }

        if ( lldpd_apply_tlv_configs(ovs,
                     OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_TLV_PORT_VLAN_NAME_ENABLE,
                     &g_lldp_cfg->g_config.c_lldp_tlv_port_vlan_name_enable) ) {
            *send_now=1;
        }

        if ( lldpd_apply_tlv_configs(ovs,
                     OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_TLV_PORT_PROTO_ID_ENABLE,
                     &g_lldp_cfg->g_config.c_lldp_tlv_port_proto_id_enable) ) {
            *send_now=1;
        }

        /*
        ** IMPORTANT: Keep enable/disable configuration at the end so that all
        ** other configs are configured prior to enabling.
        */
        lldp_enabled = smap_get_bool(&ovs->other_config,
                                 OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_ENABLE,
                                 OPEN_VSWITCH_OTHER_CONFIG_MAP_LLDP_ENABLE_DEFAULT);
        //Check for lldp enable and update.
        bool is_any_protocol_enabled = false;
        for (i=0; g_lldp_cfg->g_protocols[i].mode != 0; i++) {
            if (g_lldp_cfg->g_protocols[i].mode == LLDPD_MODE_LLDP){
                if (CHANGED(lldp_enabled, g_lldp_cfg->g_protocols[i].enabled)) {
                    g_lldp_cfg->g_protocols[i].enabled = lldp_enabled;
                    lldpd_reset(g_lldp_cfg, NULL);
                    lldpd_update_localports(g_lldp_cfg);
                    if(lldp_enabled) {
                        *send_now = 1;
                    }
                    VLOG_INFO("lldp %s", g_lldp_cfg->g_protocols[i].enabled ?
                              "enabled": "disabled");
                }
            }
            if(g_lldp_cfg->g_protocols[i].enabled) {
                /* Lets set is_any_protocol enabled to true */
                is_any_protocol_enabled = true;
            }
        }

        /*
         * For now we support only lldp. But in future when we support CDP
         * and EDP this code will make more sense.
         */
        if(is_any_protocol_enabled) {
            g_lldp_cfg->g_config.c_is_any_protocol_enabled = 1;
        } else {
            g_lldp_cfg->g_config.c_is_any_protocol_enabled = 0;
        }
    }

    if(update_now){
        //Update the information to local Chasis.
        levent_update_now(g_lldp_cfg);
    }
    return;
}

/******************************************************************************
*******************************************************************************
**
** START OF lldp statistics/counters reporting section
**
*******************************************************************************
******************************************************************************/

/* timer working variables */
static u_int64_t lldpd_stats_last_check_time = 0;
static u_int64_t lldpd_stats_check_interval =
    LLDP_CHECK_STATS_FREQUENCY_DFLT_MSEC;

/*
** global versions of interface counters (summed up)
*/
static u_int64_t total_h_tx_cnt = 0;
static u_int64_t total_h_rx_cnt = 0;
static u_int64_t total_h_rx_discarded_cnt = 0;
static u_int64_t total_h_rx_unrecognized_cnt = 0;
static u_int64_t total_h_ageout_cnt = 0;
static u_int64_t total_h_insert_cnt = 0;
static u_int64_t total_h_delete_cnt = 0;
static u_int64_t total_h_drop_cnt = 0;

static char *lldp_interface_statistics_keys [] = {
    INTERFACE_STATISTICS_LLDP_TX_COUNT,
    INTERFACE_STATISTICS_LLDP_RX_COUNT,
    INTERFACE_STATISTICS_LLDP_RX_DISCARDED_COUNT,
    INTERFACE_STATISTICS_LLDP_RX_UNRECOGNIZED_COUNT,
    INTERFACE_STATISTICS_LLDP_AGEOUT_COUNT,
    INTERFACE_STATISTICS_LLDP_INSERT_COUNT,
    INTERFACE_STATISTICS_LLDP_DELETE_COUNT,
    INTERFACE_STATISTICS_LLDP_DROP_COUNT
};

#define LLDPD_TOTAL_STATS_PER_INTERFACE \
    (sizeof(lldp_interface_statistics_keys) / sizeof(char*))

/*
** Report ONE interface's stats to the db
*/
static void
lldp_process_one_interface_counters (char *if_name,
    struct lldpd_hardware *hardware,
    struct ovsrec_interface *ifrow)
{
    int total = 0;
    int64_t values [LLDPD_TOTAL_STATS_PER_INTERFACE];

    /* if either pointer not found, forget it */
    if (!hardware || !ifrow) {
        VLOG_ERR("could not check stats for %s (hardware %s, dbrow %s)",
            if_name,
            hardware ? "valid" : "NULL",
            ifrow ? "valid" : "NULL");
        return;
    }

/*----------------------------------------------------------------------------*/

#define PROCESS_INTERFACE_COUNTER(COUNTER) \
    do { \
        values[total] = hardware->COUNTER; \
        total_ ## COUNTER += hardware->COUNTER; \
        total++; \
    } while (0)

/*----------------------------------------------------------------------------*/

    /* start filling them in */
    PROCESS_INTERFACE_COUNTER(h_tx_cnt);
    PROCESS_INTERFACE_COUNTER(h_rx_cnt);
    PROCESS_INTERFACE_COUNTER(h_rx_discarded_cnt);
    PROCESS_INTERFACE_COUNTER(h_rx_unrecognized_cnt);
    PROCESS_INTERFACE_COUNTER(h_ageout_cnt);
    PROCESS_INTERFACE_COUNTER(h_insert_cnt);
    PROCESS_INTERFACE_COUNTER(h_delete_cnt);
    PROCESS_INTERFACE_COUNTER(h_drop_cnt);

    ovsrec_interface_set_lldp_statistics(ifrow,
            lldp_interface_statistics_keys, values, total);
}

static void
lldp_process_all_interfaces_counters (struct lldpd *cfg)
{
    struct shash_node *sh_node;
    struct lldp_iface *dual_itf;

    /* start counting these in case anything changed */
    total_h_tx_cnt = 0;
    total_h_rx_cnt = 0;
    total_h_rx_discarded_cnt = 0;
    total_h_rx_unrecognized_cnt = 0;
    total_h_ageout_cnt = 0;
    total_h_insert_cnt = 0;
    total_h_delete_cnt = 0;
    total_h_drop_cnt = 0;

    /* for each interface, compare & update db */
    SHASH_FOR_EACH(sh_node, &all_interfaces) {
        dual_itf = sh_node->data;
        lldp_process_one_interface_counters(dual_itf->name,
            dual_itf->hw, dual_itf->ifrow);
    }
}

/*
** Report changes on the GLOBAL lldp stats.
** This is called ASSUMING that the values
** have actually changed.
*/
static void
lldp_process_global_counters (struct lldpd *cfg)
{
    struct ovsrec_open_vswitch *row;
    struct smap smap;

    smap_init(&smap);
    smap_add_format(&smap, OVSDB_STATISTICS_LLDP_TABLE_INSERTS,
        "%"PRIu64, total_h_insert_cnt);
    smap_add_format(&smap, OVSDB_STATISTICS_LLDP_TABLE_DELETES,
        "%"PRIu64, total_h_delete_cnt);
    smap_add_format(&smap, OVSDB_STATISTICS_LLDP_TABLE_DROPS,
        "%"PRIu64, total_h_drop_cnt);
    smap_add_format(&smap, OVSDB_STATISTICS_LLDP_TABLE_AGEOUTS,
        "%"PRIu64, total_h_ageout_cnt);

    VLOG_DBG("updating lldp global stats: %s=%"PRIu64" %s=%"PRIu64
        "%s=%"PRIu64" %s=%"PRIu64,
        OVSDB_STATISTICS_LLDP_TABLE_INSERTS, total_h_insert_cnt,
        OVSDB_STATISTICS_LLDP_TABLE_DELETES, total_h_delete_cnt,
        OVSDB_STATISTICS_LLDP_TABLE_DROPS, total_h_drop_cnt,
        OVSDB_STATISTICS_LLDP_TABLE_AGEOUTS, total_h_ageout_cnt);

    row = ovsrec_open_vswitch_first(idl);
    ovsrec_open_vswitch_set_lldp_statistics(row, &smap);
    smap_destroy(&smap);
}

static void
lldpd_stats_analyze (struct lldpd *cfg)
{

static struct ovsdb_idl_txn *lldp_stats_txn = NULL;

    enum ovsdb_idl_txn_status status;

    if (NULL == lldp_stats_txn) {
        lldp_stats_txn = ovsdb_idl_txn_create(idl);
        lldp_process_all_interfaces_counters(cfg);
        lldp_process_global_counters(cfg);
    }

    status = ovsdb_idl_txn_commit(lldp_stats_txn);
    if (status != TXN_INCOMPLETE) {
        ovsdb_idl_txn_destroy(lldp_stats_txn);
        lldp_stats_txn = NULL;
    }
}

/*
** This is the lldp main stat reporting scheduling function.
** It makes sure that it runs every "lldpd_stats_check_interval"
** seconds and if somehow it gets called before that time has
** elapsed, reschedules itself to be called at precisely that
** interval.  When the exact time is hit, it calls the main worker
** function "lldpd_stats_analyze".
*/
static void
lldpd_stats_run (struct lldpd *cfg)
{
    u_int64_t time_now = time_msec();
    int64_t time_elapsed_since_last_invocation =
        time_now - lldpd_stats_last_check_time;
    int64_t time_left_to_next_invocation =
        lldpd_stats_check_interval - time_elapsed_since_last_invocation;

    if (time_left_to_next_invocation < 0) {
        time_left_to_next_invocation = 0;
    }

    VLOG_DBG("lldpd_stats_run entered at time %"PRIu64, time_now);

    if (time_left_to_next_invocation > 0) {
        poll_timer_wait(time_left_to_next_invocation);
        VLOG_DBG("not our time yet, still have %"PRIu64" msecs to wait",
            time_left_to_next_invocation);
        return;
    }

    /* ok, our time has come, do the work */
    VLOG_DBG("checking lldpd stats NOW at %"PRIu64" msecs!", time_now);
    lldpd_stats_last_check_time = time_now;
    lldpd_stats_analyze(cfg);
    poll_timer_wait(lldpd_stats_check_interval);
}

/******************************************************************************
*******************************************************************************
**
** END OF lldp statistics/counters reporting section
**
*******************************************************************************
******************************************************************************/
static bool 
lldpd_ovsdb_nbrs_run(struct ovsdb_idl *idl, struct lldpd *cfg)
{
    struct lldpd_hardware *hardware;
    const struct ovsrec_interface *ifrow;
    struct lldpd_port *port;
    struct smap smap_nbr;
    const  char  *last_update_str;
    const  char  *port_ttl_str;
    time_t last_update_db;
    int    port_ttl;
    bool   nbr_change = false;
    struct shash_node *if_node;
    struct lldp_iface *itf;
    
    /* Scan all hardware interfaces in lldpd and look for updates 
     * When we find an interface that got changed, find a corrosponding
     * OVSDB nbr and update that nbr according to lldps change opcode
     */  
    SHASH_FOR_EACH(if_node, &all_interfaces) {
        itf = if_node->data;
        if  (itf && itf->hw && itf->ifrow) {
            hardware = itf->hw;
            ifrow    = itf->ifrow;
        } else {
            continue;
        }
    
        if (hardware->h_rport_change_opcode == LLDPD_AF_NBR_NOOP) {
            continue;
        } else {
                if (strcmp(ifrow->name, hardware->h_ifname) == 0) {
                    /* If no neighbor info in DB, write everything */
                    if (hardware->h_rport_change_opcode == LLDPD_AF_NBR_UPD) {
                        port_ttl_str = smap_get(&ifrow->lldp_neighbor_info, "p_chassis.c_ttl");
                        if (port_ttl_str == NULL) {
                           hardware->h_rport_change_opcode = LLDPD_AF_NBR_ADD;
                        }
                    }
                    switch (hardware->h_rport_change_opcode) {

                        case LLDPD_AF_NBR_ADD:
                        case LLDPD_AF_NBR_MOD:
                            VLOG_INFO("%s i/f %s ADD/MOD", __FUNCTION__, ifrow->name); 
                            TAILQ_FOREACH(port, &hardware->h_rports, p_entries) {
                                lldp_nbr_update(&smap_nbr, port);
                                ovsrec_interface_set_lldp_neighbor_info(ifrow, &smap_nbr);
                                smap_destroy(&smap_nbr);
                                nbr_change = true;
                                break;
                            }
                            break;

                        case LLDPD_AF_NBR_UPD:
                            VLOG_INFO("%s i/f %s UPD", __FUNCTION__, ifrow->name); 
                            TAILQ_FOREACH(port, &hardware->h_rports, p_entries) {
                                char last_update_s[10];  
        	                sprintf(last_update_s, "%llx", (long long)port->p_lastupdate); 
                                VLOG_INFO("Updating nbr time to %s", last_update_s);
                                smap_clone(&smap_nbr, &ifrow->lldp_neighbor_info);
                                smap_replace(&smap_nbr, "p_nbr.p_lastupdate", last_update_s);
                                ovsrec_interface_set_lldp_neighbor_info(ifrow, &smap_nbr);
                                smap_destroy(&smap_nbr);
                                nbr_change = true;
                                break;
                            }
                            break;

                        case LLDPD_AF_NBR_DEL:
                            VLOG_INFO("%s i/f %s DEL", __FUNCTION__, ifrow->name); 
                            ovsrec_interface_set_lldp_neighbor_info(ifrow, NULL);
                            nbr_change = true;
                            break;

                        case LLDPD_AF_NBR_NOOP:
                            VLOG_INFO("%s i/f %s NOOP", __FUNCTION__, ifrow->name); 
                            break;

                        default:
                            VLOG_INFO("%s i/f %s dafault", __FUNCTION__, ifrow->name); 
                            /* Nothing to do */
                            break;
                    } /* switch */
                    hardware->h_rport_change_opcode = LLDPD_AF_NBR_NOOP;
                } /* strcmp */
        } /* h_rport_change_opcode != NULL */
    } /* interface loop */


    /* Scan all nbr smaps in OVSDB and look for aged out entries;
     * Delete any such nbr smaps from database.
     * This will cover any corner cases, like restart, in which lldpd
     * fails to report aged out nbr entries in a timely manner.
     */
    OVSREC_INTERFACE_FOR_EACH(ifrow, idl) {
        last_update_str = smap_get(&ifrow->lldp_neighbor_info, "p_nbr.p_lastupdate");
        port_ttl_str    = smap_get(&ifrow->lldp_neighbor_info, "p_chassis.c_ttl");
        if ((last_update_str != NULL) && (port_ttl_str != NULL)) {
            last_update_db = strtoll(last_update_str, 0, 16);
            port_ttl = atoi(port_ttl_str);
            VLOG_INFO("%s timestamps: cur=%0llx last_update=%llx port_ttl=%d (sec)", 
                     __FUNCTION__, time(NULL), last_update_db, port_ttl);
            if (time(NULL) - last_update_db > port_ttl + 2) {
                VLOG_INFO("%s aging out interfcae %s", __FUNCTION__, ifrow->name); 
                ovsrec_interface_set_lldp_neighbor_info(ifrow, NULL);
                nbr_change = true;
            }
        }
    }
    return nbr_change;
}


static void 
lldpd_ovsdb_nbrs_change_all(struct ovsdb_idl *idl, struct lldpd *cfg)
{
    struct lldpd_hardware *hardware;
    const struct ovsrec_interface *ifrow;
    struct lldpd_port *port;
    struct smap smap_nbr;
    bool found = false;
    
    /* Scan all hardware interfaces in lldpd
     * Copy nbr info from lldpd to OVSDB whenever lldpd has a port entry 
     * Otherwise, delete nbr info from OVSDB
     */  
    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
            
        found = false;
        OVSREC_INTERFACE_FOR_EACH(ifrow, idl) {
            if (strcmp(ifrow->name, hardware->h_ifname) == 0) {
                TAILQ_FOREACH(port, &hardware->h_rports, p_entries) {
                    lldp_nbr_update(&smap_nbr, port);
                    ovsrec_interface_set_lldp_neighbor_info(ifrow, &smap_nbr);
                    smap_destroy(&smap_nbr);
                    found = true;
                    break;
                }
                if (!found) {
                    ovsrec_interface_set_lldp_neighbor_info(ifrow, NULL);
                }

                    hardware->h_rport_change_opcode = LLDPD_AF_NBR_NOOP;
            } /* strcmp */
        } /* ovsrec loop */ 
    } /* hardware loop */

    return;
}

static void
lldpd_reconfigure(struct ovsdb_idl *idl,struct lldpd *g_lldp_cfg)
{
    unsigned int new_idl_seqno = ovsdb_idl_get_seqno(idl);
    bool send_now = false;
    COVERAGE_INC(lldpd_ovsdb_if);

    if (new_idl_seqno == idl_seqno){
        return;
    }

    lldpd_apply_interface_changes(idl, g_lldp_cfg, &send_now);
    lldpd_apply_global_changes(idl, g_lldp_cfg, &send_now);

    if(send_now){
        //An Asnychronous send as Information has changed.
        levent_send_now(g_lldp_cfg);
    }

    idl_seqno = new_idl_seqno;
}

static void
lldpd_unixctl_dump(struct unixctl_conn *conn, int argc OVS_UNUSED,
                          const char *argv[] OVS_UNUSED, void *aux OVS_UNUSED)
{
#define BUF_LEN 4000
#define REM_BUF_LEN (BUF_LEN - 1 - strlen(buf))
    //unixctl_command_reply_error(conn, "Nothing to dump :)");
    struct shash_node *sh_node;
    char *buf = xmalloc(BUF_LEN);
    int first_row_done = 0;
    /* Loop through all the current interfaces and figure out how many
     * have config changes that need action. */
    SHASH_FOR_EACH(sh_node, &all_interfaces) {

              struct lldp_iface *itf = sh_node->data;

              if(itf->hw && first_row_done == 0) {
                  if(itf->hw->h_cfg->g_protocols[0].enabled)
                      strcpy(buf, "\nLLDP : ENABLED\n\n");
                  else strcpy(buf, "\nLLDP : DISABLED\n\n");
                 strncat(buf, "    intf name\t|   OVSDB interface\t|"
                          "   LLDPD Interface\t|    LLDP Status\t|  Link State\n", REM_BUF_LEN);
                  strncat(buf, "==============================================="
                          "===============================================\n", REM_BUF_LEN);
                  first_row_done++;
              }

              strncat(buf, itf->name, REM_BUF_LEN);
              if(itf->ifrow)
                  strncat(buf, "\t\t|    Yes", REM_BUF_LEN);
              else strncat(buf, "\t\t|    No\t", REM_BUF_LEN);
              if(itf->hw) {
                  strncat(buf, "\t\t|    Yes", REM_BUF_LEN);
                  if(itf->hw->h_enable_dir == HARDWARE_ENABLE_DIR_OFF)
                      strncat(buf, "\t\t|    off", REM_BUF_LEN);
                  if(itf->hw->h_enable_dir == HARDWARE_ENABLE_DIR_TX)
                      strncat(buf, "\t\t|    tx", REM_BUF_LEN);
                  if(itf->hw->h_enable_dir == HARDWARE_ENABLE_DIR_RX)
                      strncat(buf, "\t\t|    rx", REM_BUF_LEN);
                  if(itf->hw->h_enable_dir == HARDWARE_ENABLE_DIR_RXTX)
                      strncat(buf, "\t\t|    rxtx", REM_BUF_LEN);
              }
              else strncat(buf, "\t\t|No\t\t|", REM_BUF_LEN);

              if(itf->hw) {
                  if(itf->hw->h_link_state == INTERFACE_LINK_STATE_UP)
                      strncat(buf, "\t\t|    up", REM_BUF_LEN);
                  else
                      strncat(buf, "\t\t|    down", REM_BUF_LEN);
              }

              strncat(buf, "\n", REM_BUF_LEN);
    }
    unixctl_command_reply(conn, buf);
    free(buf);
}

static void
halon_lldpd_exit(struct unixctl_conn *conn, int argc OVS_UNUSED,
                  const char *argv[] OVS_UNUSED, void *exiting_)
{
    bool *exiting = exiting_;
    *exiting = true;
    unixctl_command_reply(conn, NULL);
}

static inline void lldpd_chk_for_system_configured(void)
{
    const struct ovsrec_open_vswitch *ovs_vsw = NULL;

    if (system_configured) {
        /* Nothing to do if we're already configured. */
        return;
    }

    ovs_vsw = ovsrec_open_vswitch_first(idl);

    if (ovs_vsw && (ovs_vsw->cur_cfg > (int64_t) 0)) {
        system_configured = true;
        VLOG_INFO("System is now configured (cur_cfg=%d).",
                 (int)ovs_vsw->cur_cfg);
    }

} /* lldpd_chk_for_system_configured */


static bool confirm_txn_try_again = false;

static void
lldpd_run(struct lldpd *cfg)
{
    bool nbr_change;
    static struct ovsdb_idl_txn *confirm_txn = NULL;

    ovsdb_idl_run(idl);
    unixctl_server_run(appctl);

    if (ovsdb_idl_is_lock_contended(idl)) {
        static struct vlog_rate_limit rl = VLOG_RATE_LIMIT_INIT(1, 1);

        VLOG_ERR_RL(&rl, "another lldpd process is running, "
                    "disabling this process until it goes away");

        return;
    } else if (!ovsdb_idl_has_lock(idl)) {
        return;
    }

    lldpd_chk_for_system_configured();

    if (system_configured) {
        lldpd_reconfigure(idl,cfg);
        lldpd_stats_run(cfg);
        daemonize_complete();
        vlog_enable_async();
        VLOG_INFO_ONCE("%s (Halon lldpd) %s", program_name, VERSION);
    }

    /* create a confirmed database transaction for nbr and config updates */
    if (!confirm_txn) {
        confirm_txn = ovsdb_idl_txn_create(idl);

        if (!confirm_txn_try_again) {
            VLOG_INFO("Invoking lldpd_ovsdb_nbrs_run");
            nbr_change = lldpd_ovsdb_nbrs_run(idl, cfg);
            if (!nbr_change) {
                ovsdb_idl_txn_destroy(confirm_txn);
                confirm_txn = NULL;
            }
        } else {
            VLOG_INFO("Invoking lldpd_ovsdb_nbrs_change_all");
            lldpd_ovsdb_nbrs_change_all(idl, cfg);
            nbr_change = true;
        }
    }

    if (confirm_txn) {
        enum ovsdb_idl_txn_status status;

        status = ovsdb_idl_txn_commit(confirm_txn);
        if (status != TXN_INCOMPLETE) {
            ovsdb_idl_txn_destroy(confirm_txn);
            confirm_txn = NULL;

            /* Sets the 'status_txn_try_again' if the transaction fails. */
            if (status == TXN_SUCCESS) {
                confirm_txn_try_again = false;
            } else {
                confirm_txn_try_again = true;
            }
        }
    }

}

static void
lldpd_wait (void)
{
    ovsdb_idl_wait(idl);
    unixctl_server_wait(appctl);
}

static int
ovspoll_to_libevent(struct lldpd *cfg)
{
    struct poll_loop *loop = poll_loop();
    long long int timeout;
    struct poll_node *node;
    struct timeval tv;
    int retval = -1;
    int events_scheduled = 0;

    /* Populate with all the fds events. */
    HMAP_FOR_EACH (node, hmap_node, &loop->poll_nodes) {
        //VLOG_INFO("in poll to event .. adding fd %d to libevent",node->pollfd.fd);
        levent_make_socket_nonblocking(node->pollfd.fd);
        node->event = event_new(cfg->g_base, node->pollfd.fd ,
            EV_READ, ovs_libevent_cb, cfg);
        retval = event_add((struct event*) node->event , NULL);
        if(!retval)
            events_scheduled ++;
        //event_base_dump_events(cfg->g_base, stdout);

    }

    /* Populate the timeout event */
    timeout = loop->timeout_when - time_msec();
    if(timeout > 0 && loop->timeout_when > 0 && loop->timeout_when < LLONG_MAX) {
        //VLOG_INFO("setting timer to callback to %d", timeout);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        if(timeout < 1000)
            tv.tv_usec = (timeout) * 1000 ; //to usecond
        else
            tv.tv_sec =  timeout/1000;
        timeout_event = event_new(cfg->g_base, -1, 0,
                                       ovs_libevent_cb,
                                                  cfg);
        retval = event_add(timeout_event, &tv);
        if(!retval)
            events_scheduled ++;
        //VLOG_INFO("timeout event added %d, fd = %d", timeout, (int)timeout_event->ev_fd);
        //event_base_dump_events(cfg->g_base, stdout);
    }

    if(events_scheduled)
        return 0;

    // Nothing was scheduled, return -1
    return -1;
}

/* Create a connection to the OVSDB at db_path and create a dB cache
 * for this daemon. */
static void
ovsdb_init(const char *db_path)
{
    /* Initialize IDL through a new connection to the dB. */
    idl = ovsdb_idl_create(db_path, &ovsrec_idl_class, false, true);
    idl_seqno = ovsdb_idl_get_seqno(idl);
    ovsdb_idl_set_lock(idl, "halon_lldpd");
    ovsdb_idl_verify_write_only(idl);

    /* Choose some OVSDB tables and columns to cache. */

    ovsdb_idl_add_table(idl, &ovsrec_table_open_vswitch);
    ovsdb_idl_add_column(idl, &ovsrec_open_vswitch_col_cur_cfg);
    ovsdb_idl_add_column(idl, &ovsrec_open_vswitch_col_other_config);
    ovsdb_idl_add_column(idl, &ovsrec_open_vswitch_col_lldp_statistics);
    ovsdb_idl_omit_alert(idl, &ovsrec_open_vswitch_col_lldp_statistics);

    ovsdb_idl_add_table(idl, &ovsrec_table_interface);
    ovsdb_idl_add_column(idl, &ovsrec_interface_col_name);
    ovsdb_idl_add_column(idl, &ovsrec_interface_col_lldp_statistics);
    ovsdb_idl_omit_alert(idl, &ovsrec_interface_col_lldp_statistics);
    ovsdb_idl_add_column(idl, &ovsrec_interface_col_other_config);
    ovsdb_idl_add_column(idl, &ovsrec_interface_col_link_state);

    /* Per interface lldp_neighbor_info */
    ovsdb_idl_add_column(idl, &ovsrec_interface_col_name);
    ovsdb_idl_add_column(idl, &ovsrec_interface_col_lldp_neighbor_info);
    ovsdb_idl_omit_alert(idl, &ovsrec_interface_col_lldp_neighbor_info);

    /* Register ovs-appctl commands for this daemon. */
    unixctl_command_register("lldpd/dump", "", 0, 0, lldpd_unixctl_dump, NULL);
} // ovsdb_init

static void
ovsdb_exit(void)
{
    ovsdb_idl_destroy(idl);
}

static void
usage(void)
{
    printf("%s: Halon lldpd daemon\n"
           "usage: %s [OPTIONS] [DATABASE]\n"
           "where DATABASE is a socket on which ovsdb-server is listening\n"
           "      (default: \"unix:%s/db.sock\").\n",
           program_name, program_name, ovs_rundir());
    stream_usage("DATABASE", true, false, true);
    daemon_usage();
    vlog_usage();
    printf("\nOther options:\n"
           "  --unixctl=SOCKET        override default control socket name\n"
           "  -h, --help              display this help message\n"
           "  -V, --version           display version information\n");
    exit(EXIT_SUCCESS);
}

static char *
lldp_ovsdb_parse_options(int argc, char *argv[], char **unixctl_pathp)
{
    enum {
        OPT_UNIXCTL = UCHAR_MAX + 1,
        VLOG_OPTION_ENUMS,
        DAEMON_OPTION_ENUMS,
        OVSDB_OPTIONS_END,
    };
    static const struct option long_options[] = {
        {"help",        no_argument, NULL, 'h'},
        {"unixctl",     required_argument, NULL, OPT_UNIXCTL},
        DAEMON_LONG_OPTIONS,
        VLOG_LONG_OPTIONS,
        {"ovsdb-options-end", optional_argument, NULL, OVSDB_OPTIONS_END},
        {NULL, 0, NULL, 0},
    };
    char *short_options = long_options_to_short_options(long_options);

    for (;;) {
        int c;
        int end_options = 0;

        c = getopt_long(argc, argv, short_options, long_options, NULL);
        if (c == -1) {
            break;
        }

        switch (c) {
        case 'h':
            usage();

        case OPT_UNIXCTL:
            *unixctl_pathp = optarg;
            break;

        VLOG_OPTION_HANDLERS
        DAEMON_OPTION_HANDLERS

        case OVSDB_OPTIONS_END:
            end_options = 1;
            break;

        case '?':
            exit(EXIT_FAILURE);

        default:
           abort();
        }
        if(end_options)
            break;
    }
    free(short_options);

    argc -= optind;
    argv += optind;

    return xasprintf("unix:%s/db.sock", ovs_rundir());
} // parse_options

void lldpd_ovsdb_init(int argc, char *argv[]){
    int retval;
    char *ovsdb_sock;
    set_program_name(argv[0]);
    proctitle_init(argc, argv);
    fatal_ignore_sigpipe();

    /* Parse commandline args and get the name of the OVSDB socket. */
    ovsdb_sock = lldp_ovsdb_parse_options(argc, argv, &appctl_path);

    /* Initialize the metadata for the IDL cache. */
    ovsrec_init();
    /* Fork and return in child process; but don't notify parent of
     * startup completion yet. */
    daemonize_start();

    /* Create UDS connection for ovs-appctl. */
    retval = unixctl_server_create(appctl_path, &appctl);
    if (retval) {
       exit(EXIT_FAILURE);
    }

    /* Register the ovs-appctl "exit" command for this daemon. */
    unixctl_command_register("exit", "", 0, 0, halon_lldpd_exit, &exiting);

   /* Create the IDL cache of the dB at ovsdb_sock. */
   ovsdb_init(ovsdb_sock);
   free(ovsdb_sock);

   /* Notify parent of startup completion. */
   daemonize_complete();

   /* Enable asynch log writes to disk. */
   vlog_enable_async();

   VLOG_INFO_ONCE("%s (Halon LLDPD Daemon) started", program_name);
   return;
}

void lldpd_ovsdb_exit(void){
    ovsdb_exit();
}

void init_ovspoll_to_libevent(struct lldpd *cfg){
    ovs_clear_libevents();
    lldpd_run(cfg);
    lldpd_wait();
    ovspoll_to_libevent(cfg);
    return;
}

void
del_lldpd_hardware_interface(struct lldpd_hardware *hw)
{
    if (hw) {
        struct shash_node *sh_node = shash_find(&all_interfaces, hw->h_ifname);
        struct lldp_iface *itf;

        if(!sh_node) {
            VLOG_ERR("Unable to delete an lldp interface %s that has no entry "
                    "in hash", hw->h_ifname);
            return;
        }

        itf = sh_node->data;

        /*
         * If the ovs rec is also cleaned up
         * we can remove the entry else just nullify
         * ovsdb record handle
         */
        if(!itf->ifrow) {
            free(itf->name);
            free(sh_node->data);
            shash_delete(&all_interfaces, sh_node);
        } else {
            itf->hw = NULL;
        }
    }
} /* del_old_interface */

void
add_lldpd_hardware_interface(struct lldpd_hardware *hw)
{
    VLOG_DBG("lldpd harware interface %s being added!\n", hw->h_ifname);
    if (hw) {
        struct shash_node *sh_node = shash_find(&all_interfaces, hw->h_ifname);
        struct lldp_iface *itf = NULL;

        if(!sh_node) {
            /* Allocate structure to save state information for this interface. */
            itf = xcalloc(1, sizeof *itf);

            if (!shash_add_once(&all_interfaces, hw->h_ifname, itf)) {
                VLOG_WARN("Interface %s specified twice", hw->h_ifname);
                free(itf);
            } else {
                itf->name = xstrdup(hw->h_ifname);
                itf->hw = hw;
                VLOG_DBG("Created local data for interface %s", hw->h_ifname);
            }
        } else {
            itf = sh_node->data;
            itf->hw = hw;

            /* Set interface lldp_enable_dir to hw from ovsrec */
            const char *ifrow_other_config_lldp_enable_dir =
                smap_get(&itf->ifrow->other_config, INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR);
            if(ifrow_other_config_lldp_enable_dir) {
                if(strcmp(ifrow_other_config_lldp_enable_dir,
                        INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_OFF) == 0) {
                    hw->h_enable_dir = HARDWARE_ENABLE_DIR_OFF;
                } else if(strcmp(ifrow_other_config_lldp_enable_dir,
                        INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_RX) == 0) {
                    hw->h_enable_dir = HARDWARE_ENABLE_DIR_RX;
                } else if(strcmp(ifrow_other_config_lldp_enable_dir,
                        INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_TX) == 0) {
                    hw->h_enable_dir = HARDWARE_ENABLE_DIR_TX;
                } else if(strcmp(ifrow_other_config_lldp_enable_dir,
                        INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_RXTX) == 0) {
                    hw->h_enable_dir = HARDWARE_ENABLE_DIR_RXTX;
                }
            } else {
                hw->h_enable_dir = HARDWARE_ENABLE_DIR_RXTX;
            }

            /* Check for link_state change. */
            bool link_state_bool = false;

            if(itf->ifrow && itf->ifrow->link_state &&
               !strcmp(itf->ifrow->link_state, OVSREC_INTERFACE_LINK_STATE_UP)){
                link_state_bool = true;
            }
            itf->hw->h_link_state = link_state_bool;
        }
    }
} /* add_new_interface */

#define MAX_LINE 132

#define key_strval_str(pbuf, poffset, key, val, key_array, val_array) \
	do { \
		*key_array = &pbuf[*poffset]; \
        	*poffset += sprintf(&pbuf[*poffset], "%s", #key ); \
		*poffset += 1; \
		*val_array = &pbuf[*poffset]; \
        	*poffset += snprintf(&pbuf[*poffset], MAX_LINE, "%s", val); \
		*poffset += 1; \
	} while(0);	

#define key_val_str(pbuf, poffset, pKey, key, key_array, val_array) \
	do { \
		*key_array = &pbuf[*poffset]; \
        	*poffset += sprintf(&pbuf[*poffset], "%s.%s", #pKey, #key ); \
		*poffset += 1; \
		*val_array = &pbuf[*poffset]; \
        	*poffset += snprintf(&pbuf[*poffset], MAX_LINE, "%s", (pKey)->key); \
		*poffset += 1; \
	} while(0);	

#define key_val_int(pbuf, poffset, pKey, key, key_array, val_array) \
	do { \
		*key_array = &pbuf[*poffset]; \
        	*poffset += sprintf(&pbuf[*poffset], "%s.%s", #pKey, #key ); \
		*poffset += 1; \
		*val_array = &pbuf[*poffset]; \
        	*poffset += snprintf(&pbuf[*poffset], MAX_LINE, "%d", (pKey)->key); \
		*poffset += 1; \
	} while(0);	

#define key_val_double(pbuf, poffset, pKey, key, key_array, val_array) \
	do { \
		*key_array = &pbuf[*poffset]; \
        	*poffset += sprintf(&pbuf[*poffset], "%s.%s", #pKey, #key ); \
		*poffset += 1; \
		*val_array = &pbuf[*poffset]; \
        	*poffset += snprintf(&pbuf[*poffset], MAX_LINE, "%llx", (long long)(pKey)->key); \
		*poffset += 1; \
	} while(0);	

#define KEY_VAL_STR_BUF_MAX 2048 
#define KEY_VAL_MAX 64 

int smap_list_set(char *svec, int *vec_cur, char *val)
{
    *vec_cur += sprintf(&svec[*vec_cur], "%s,", val);
    return 0;
}

int smap_list_get_next(char *svec, int *vec_cur, char *val)
{
    int i=0, idx = *vec_cur;

    while ((svec[idx] != 0) && (svec[idx] != ',')) {
        val[i++] = svec[idx++];
    }

    val[i] = 0;

    if (svec[idx] == 0)
        *vec_cur=-1;
    else
        *vec_cur=idx+1;

    return 0;
}

void lldp_create_vlan_list(char **vlan_list, struct lldpd_port *p_nbr)
{
    int offset_f0 = 0;
    int offset_f1 = 0;
    char vid_str[10];
    struct lldpd_vlan *vlan;

    TAILQ_FOREACH(vlan, &p_nbr->p_vlans, v_entries) {
        VLOG_INFO("lldp_create_vlan_list offset %d vlan %p", offset_f0, vlan);
        VLOG_INFO("lldp_create_vlan_list name= %s ", vlan->v_name);
        smap_list_set(vlan_list[0], &offset_f0, vlan->v_name);
        sprintf(vid_str, "%d", vlan->v_vid);
        smap_list_set(vlan_list[1], &offset_f1, vid_str);
    }
}

void lldp_fetch_vlan_list(char **vlan_list, struct lldpd_port *p_nbr)
{
    int offset_f0 = 0;
    int offset_f1 = 0;
    char vid_str[132];
    struct lldpd_vlan *vlan;

    TAILQ_FOREACH(vlan, &p_nbr->p_vlans, v_entries) {
        smap_list_get_next(vlan_list[0], &offset_f0, vid_str);
        vlan->v_name = strdup(vid_str);
        smap_list_get_next(vlan_list[1], &offset_f1, vid_str);
        vlan->v_vid = atoi(vid_str);
    }
}


void lldp_create_pi_list(char **pi_list, struct lldpd_port *p_nbr)
{
    int offset_f0 = 0;
    int offset_f1 = 0;
    char vid_str[10];
    struct lldpd_pi *p_pi;

    TAILQ_FOREACH(p_pi, &p_nbr->p_pids, p_entries) {
        smap_list_set(pi_list[0], &offset_f0, p_pi->p_pi);
        sprintf(vid_str, "%d", p_pi->p_pi_len);
        smap_list_set(pi_list[1], &offset_f1, vid_str);
    }
}

void lldp_fetch_pi_list(char **pi_list, struct lldpd_port *p_nbr)
{
    int offset_f0 = 0;
    int offset_f1 = 0;
    char vid_str[132];
    struct lldpd_pi *p_pi;

    TAILQ_FOREACH(p_pi, &p_nbr->p_pids, p_entries) {
        smap_list_get_next(pi_list[0], &offset_f0, vid_str);
        p_pi->p_pi = strdup(vid_str);
        smap_list_get_next(pi_list[1], &offset_f1, vid_str);
        p_pi->p_pi_len = atoi(vid_str);
    }
}

void lldp_create_ppvid_list(char **ppvids_list, struct lldpd_port *p_nbr)
{
    int offset_f0 = 0;
    int offset_f1 = 0;
    char vid_str[10];
    struct lldpd_ppvid *p_ppvid;

    TAILQ_FOREACH(p_ppvid, &p_nbr->p_ppvids, p_entries) {
        sprintf(vid_str, "%d", p_ppvid->p_cap_status);
        smap_list_set(ppvids_list[0], &offset_f0, vid_str);
        sprintf(vid_str, "%d", p_ppvid->p_ppvid);
        smap_list_set(ppvids_list[1], &offset_f1, vid_str);
    }
}

void lldp_fetch_ppvid_list(char **ppvids_list, struct lldpd_port *p_nbr)
{
    int offset_f0 = 0;
    int offset_f1 = 0;
    char vid_str[132];
    struct lldpd_ppvid *p_ppvid;

    TAILQ_FOREACH(p_ppvid, &p_nbr->p_ppvids, p_entries) {
        smap_list_get_next(ppvids_list[0], &offset_f0, vid_str);
        p_ppvid->p_cap_status = atoi(vid_str);
        smap_list_get_next(ppvids_list[1], &offset_f1, vid_str);
        p_ppvid->p_ppvid = atoi(vid_str);
    }
}

/*
 * The update function writes new neighbor information 
 * into OVSDB from lldpd port.
 */
int lldp_nbr_update(void *smap, struct lldpd_port *p_nbr)
{
    char *pbuf=NULL;
    char *vlan_list[2]={0,0};
    char *ppvids_list[2]={0,0};
    char *pids_list[2]={0,0};
    int offset=0;
    int idx=0;
    int i;
    char *key_array[KEY_VAL_MAX];
    char *val_array[KEY_VAL_MAX];
    struct lldpd_dot3_macphy *p_macphy;
    struct lldpd_dot3_power  *p_power;
    struct lldpd_chassis *p_chassis;

    pbuf = malloc(KEY_VAL_STR_BUF_MAX);
    if (p_nbr == NULL) {
        VLOG_ERR("%s NULL port pointer", __FUNCTION__);
        goto cleanup;
    }

    p_macphy = &p_nbr->p_macphy;
    p_power  = &p_nbr->p_power;

    p_chassis =p_nbr->p_chassis;

    /* Populate chassis key/val */
    if (p_chassis) {
        key_val_int(pbuf, &offset, p_chassis, c_refcount, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_chassis, c_index, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_chassis, c_protocol, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_chassis, c_id_subtype, &key_array[idx], &val_array[idx++]);
        key_val_str(pbuf, &offset, p_chassis, c_id, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_chassis, c_id_len, &key_array[idx], &val_array[idx++]);
        key_val_str(pbuf, &offset, p_chassis, c_name, &key_array[idx], &val_array[idx++]);
        key_val_str(pbuf, &offset, p_chassis, c_descr, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_chassis, c_cap_available, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_chassis, c_cap_enabled, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_chassis, c_ttl, &key_array[idx], &val_array[idx++]);
    }

    /* Populate port key/val */
    key_val_double(pbuf, &offset, p_nbr, p_lastchange, &key_array[idx], &val_array[idx++]);
    key_val_double(pbuf, &offset, p_nbr, p_lastupdate, &key_array[idx], &val_array[idx++]);
    key_val_int(pbuf, &offset, p_nbr, p_protocol, &key_array[idx], &val_array[idx++]);
    key_val_int(pbuf, &offset, p_nbr, p_hidden_in, &key_array[idx], &val_array[idx++]);
    key_val_int(pbuf, &offset, p_nbr, p_hidden_out, &key_array[idx], &val_array[idx++]);
    key_val_int(pbuf, &offset, p_nbr, p_id_subtype, &key_array[idx], &val_array[idx++]);
    key_val_str(pbuf, &offset, p_nbr, p_id, &key_array[idx], &val_array[idx++]);
    key_val_int(pbuf, &offset, p_nbr, p_id_len, &key_array[idx], &val_array[idx++]);
    key_val_str(pbuf, &offset, p_nbr, p_descr, &key_array[idx], &val_array[idx++]);
    key_val_int(pbuf, &offset, p_nbr, p_mfs, &key_array[idx], &val_array[idx++]);

    /* MAC PHY - DOT3 */
    if (p_macphy) {
        key_val_int(pbuf, &offset, p_macphy, autoneg_support, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_macphy, autoneg_enabled, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_macphy, autoneg_advertised, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_macphy, mau_type, &key_array[idx], &val_array[idx++]);
    }

    /* POWER mgmt - DOT3 */
    if (p_power) {
        key_val_int(pbuf, &offset, p_power, devicetype, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_power, supported, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_power, enabled, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_power, paircontrol, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_power, pairs, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_power, class, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_power, powertype, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_power, source, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_power, priority, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_power, requested, &key_array[idx], &val_array[idx++]);
        key_val_int(pbuf, &offset, p_power, allocated, &key_array[idx], &val_array[idx++]);
    }

    /* vlans */
    key_val_int(pbuf, &offset, p_nbr, p_pvid, &key_array[idx], &val_array[idx++]);


#define VLAN_LIST_STR_MAX (4096 * 160)
#define VLAN_LIST_INT_MAX (4096 * 10)

#ifdef ENABLE_DOT1
    VLOG_INFO("802.1q vlan support enabled");
    if (!TAILQ_EMPTY(&p_nbr->p_vlans)) {
        vlan_list[0] = malloc(VLAN_LIST_STR_MAX); 
        vlan_list[1] = malloc(VLAN_LIST_INT_MAX);
        if (!vlan_list[0] || !vlan_list[1]) {
            VLOG_ERR("Error allocating vlan_list");
            goto cleanup;
        }
	lldp_create_vlan_list(vlan_list, p_nbr);
        key_strval_str(pbuf, &offset, vlan_list_name, vlan_list[0], &key_array[idx], &val_array[idx++]);
        key_strval_str(pbuf, &offset, vlan_list_id,   vlan_list[1], &key_array[idx], &val_array[idx++]);
    }
    lldp_fetch_vlan_list(vlan_list, p_nbr); 

    if (!TAILQ_EMPTY(&p_nbr->p_ppvids)) {
        ppvids_list[0] = malloc(VLAN_LIST_INT_MAX);
        ppvids_list[1] = malloc(VLAN_LIST_INT_MAX);
        if (!ppvids_list[0] || !ppvids_list[1]) {
            VLOG_ERR("Error allocating ppvids_list");
            goto cleanup;
        }
	lldp_create_ppvid_list(ppvids_list, p_nbr);
        key_strval_str(pbuf, &offset, ppvids_list_cap,   ppvids_list[0], &key_array[idx], &val_array[idx++]);
        key_strval_str(pbuf, &offset, ppvids_list_ppvid, ppvids_list[1], &key_array[idx], &val_array[idx++]);
    }

    if (!TAILQ_EMPTY(&p_nbr->p_pids)) {
        pids_list[0] = malloc(VLAN_LIST_STR_MAX);
        pids_list[1] = malloc(VLAN_LIST_INT_MAX);
        if (!pids_list[0] || !pids_list[1]) {
            VLOG_ERR("Error allocating pids_list");
            goto cleanup;
        }
	lldp_create_pi_list(pids_list, p_nbr);
        key_strval_str(pbuf, &offset, pids_list_name, pids_list[0], &key_array[idx], &val_array[idx++]);
        key_strval_str(pbuf, &offset, pids_list_len,  pids_list[1], &key_array[idx], &val_array[idx++]);
    }
#endif

    /* debug - print key/val arrays */
    VLOG_INFO("log: key_array[] val_array[]");
    for (i=0; i < idx; i++) {
        VLOG_INFO("%s - %s \n", key_array[i], val_array[i]);
    }

    VLOG_INFO("buffer offset %d\n", offset);

    /* 
     *  Invoke smap_set to save in OVSDB
     */

    smap_init(smap);
    for (i=0; i < idx; i++) {
        smap_add(smap, key_array[i], val_array[i]);
    }

cleanup:

    if (vlan_list[0]) {
	free(vlan_list[0]);
    }
    if (vlan_list[1]) {
	free(vlan_list[1]);
    }
    if (ppvids_list[0]) {
        free(ppvids_list[0]);
    }
    if (ppvids_list[1]) {
        free(ppvids_list[1]);
    }
    if (pids_list[0]) {
        free(pids_list[0]);
    }
    if (pids_list[1]) {
        free(pids_list[1]);
    }
    if (pbuf) {
        free(pbuf);
    }

    return 0;
}

/*
 * The restore function construct lldpd port from smap
 * It's not currently used, except for debugging.
 */
int lldp_nbr_restore_key_val_smap(void *smap, struct lldpd_port *p_nbr)
{
    const char *val;
    struct lldpd_dot3_macphy *p_macphy;
    struct lldpd_dot3_power  *p_power;
    struct lldpd_chassis *p_chassis;
    p_nbr = (struct lldpd_port *) malloc(sizeof (struct lldpd_port));

    p_macphy = &p_nbr->p_macphy;
    p_power  = &p_nbr->p_power;
    p_nbr->p_chassis = (struct lldpd_chassis *)malloc(sizeof (struct lldpd_chassis));
    p_chassis = p_nbr->p_chassis;

    val = smap_get(smap, "p_chassis.c_refcount");
    if (val != NULL) {
        p_chassis->c_refcount = atoi(val);
    }
    val = smap_get(smap, "p_chassis.c_index");
    if (val != NULL) {
        p_chassis->c_index = atoi(val);
    }
    val = smap_get(smap, "p_chassis.c_protocol");
    if (val != NULL) {
        p_chassis->c_protocol = atoi(val);
    }
    val = smap_get(smap, "p_chassis.c_id_subtype");
    if (val != NULL) {
        p_chassis->c_id_subtype = atoi(val);
    }
    val = smap_get(smap, "p_chassis.c_id");
    if (val != NULL) {
        p_chassis->c_id = strdup(val);
    }
    val = smap_get(smap, "p_chassis.c_id_len");
    if (val != NULL) {
        p_chassis->c_id_len = atoi(val);
    }
    val = smap_get(smap, "p_chassis.c_name");
    if (val != NULL) {
        p_chassis->c_name = strdup(val);
    }
    val = smap_get(smap, "p_chassis.c_descr");
    if (val != NULL) {
        p_chassis->c_descr = strdup(val);
    }
    val = smap_get(smap, "p_chassis.c_cap_available");
    if (val != NULL) {
        p_chassis->c_cap_available = atoi(val);
    }
    val = smap_get(smap, "p_chassis.c_cap_enabled");
    if (val != NULL) {
        p_chassis->c_cap_enabled = atoi(val);
    }
    val = smap_get(smap, "p_chassis.c_ttl");
    if (val != NULL) {
        p_chassis->c_ttl = atoi(val);
    }
    val = smap_get(smap, "p_nbr.p_lastchange");
    if (val != NULL) {
        p_nbr->p_lastchange = (time_t) strtoll(val, 0, 10);
    }
    val = smap_get(smap, "p_nbr.p_lastupdate");
    if (val != NULL) {
        p_nbr->p_lastupdate = (time_t) strtoll(val, 0, 10);
    }
    val = smap_get(smap, "p_nbr.p_protocol");
    if (val != NULL) {
        p_nbr->p_protocol = atoi(val);
    }
    val = smap_get(smap, "p_nbr.p_hidden_in");
    if (val != NULL) {
        p_nbr->p_hidden_in = atoi(val);
    }
    val = smap_get(smap, "p_nbr.p_hidden_out");
    if (val != NULL) {
        p_nbr->p_hidden_out = atoi(val);
    }
    val = smap_get(smap, "p_nbr.p_id_subtype");
    if (val != NULL) {
        p_nbr->p_id_subtype = atoi(val);
    }
    val = smap_get(smap, "p_nbr.p_id");
    if (val != NULL) {
        p_nbr->p_id = strdup(val);
    }
    val = smap_get(smap, "p_nbr.p_id_len");
    if (val != NULL) {
        p_nbr->p_id_len = atoi(val);
    }
    val = smap_get(smap, "p_nbr.p_descr");
    if (val != NULL) {
        p_nbr->p_descr = strdup(val);
    }
    val = smap_get(smap, "p_nbr.p_mfs");
    if (val != NULL) {
        p_nbr->p_mfs = atoi(val);
    }
    val = smap_get(smap, "p_macphy.autoneg_support");
    if (val != NULL) {
        p_macphy->autoneg_support = atoi(val);
    }
    val = smap_get(smap, "p_macphy.autoneg_enabled");
    if (val != NULL) {
        p_macphy->autoneg_enabled = atoi(val);
    }
    val = smap_get(smap, "p_macphy.autoneg_advertised");
    if (val != NULL) {
        p_macphy->autoneg_advertised = atoi(val);
    }
    val = smap_get(smap, "p_macphy.mau_type");
    if (val != NULL) {
        p_macphy->mau_type = atoi(val);
    }
    val = smap_get(smap, "p_power.devicetype");
    if (val != NULL) {
        p_power->devicetype = atoi(val);
    }
    val = smap_get(smap, "p_power.supported");
    if (val != NULL) {
        p_power->supported = atoi(val);
    }
    val = smap_get(smap, "p_power.enabled");
    if (val != NULL) {
        p_power->enabled = atoi(val);
    }
    val = smap_get(smap, "p_power.paircontrol");
    if (val != NULL) {
        p_power->paircontrol = atoi(val);
    }
    val = smap_get(smap, "p_power.pairs");
    if (val != NULL) {
        p_power->pairs = atoi(val);
    }
    val = smap_get(smap, "p_power.class");
    if (val != NULL) {
        p_power->class = atoi(val);
    }
    val = smap_get(smap, "p_power.powertype");
    if (val != NULL) {
        p_power->powertype = atoi(val);
    }
    val = smap_get(smap, "p_power.source");
    if (val != NULL) {
        p_power->source = atoi(val);
    }
    val = smap_get(smap, "p_power.priority");
    if (val != NULL) {
        p_power->priority = atoi(val);
    }
    val = smap_get(smap, "p_power.requested");
    if (val != NULL) {
        p_power->requested = atoi(val);
    }
    val = smap_get(smap, "p_power.allocated");
    if (val != NULL) {
        p_power->allocated = atoi(val);
    }
    val = smap_get(smap, "p_nbr.p_pvid");
    if (val != NULL) {
        p_nbr->p_pvid = atoi(val);
    }
    val = smap_get(smap, "nbr_vlan_list");
    if (val != NULL) {
    }
    val = smap_get(smap, "nbr_ppvids_list");
    if (val != NULL) {
    }
    val = smap_get(smap, "nbr_pids_list");
    if (val != NULL) {
    }

    return 0;
}


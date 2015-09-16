/*
 * (c) Copyright 2015 Hewlett Packard Enterprise Development LP.
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
 * File: lldpd_ovsdb_if.h
 *
 * Purpose: This file includes all public interface defines needed by
 *          the new lldpd_ovsdb.c for lldpd - ovsdb integration
 */

#ifndef LLDPD_OVSDB_H
#define LLDPD_OVSDB_H 1

#include "lldpd.h"

enum {
    LLDP_DECODE_LIST,
    LLDP_DECODE_BITMAP
};

enum {
    LLDP_CHASSISID_SUBTYPE_INDEX,
    LLDP_PORTID_SUBTYPE_INDEX,
    LLDP_DOT3_MAU_INDEX,
    LLDP_DOT3_POWER_INDEX,
    LLDP_DOT3_POWERPAIRS_INDEX,
    LLDP_DOT3_POWER_8023AT_INDEX,
    LLDP_DOT3_POWER_SOURCE_INDEX,
    LLDP_DOT3_POWER_PRIO_INDEX,
    LLDP_DOT3_LINK_AUTONEG_INDEX,
    LLDP_CAP_INDEX,
    LLDP_PPVID_CAP_INDEX,
    LLDP_MGMT_ADDR_INDEX,
    LLDP_MGMT_IFACE_INDEX,
    LLDP_MED_CLASS_INDEX,
    LLDP_MED_APPTYPE_INDEX,
    LLDP_MED_LOCFORMAT_INDEX,
    LLDP_MED_LOCATION_GEOID_INDEX,
    LLDP_MED_LOCATION_ALTITUDE_INDEX,
    LLDP_MED_POW_TYPE_INDEX,
    LLDP_MED_POW_SOURCE_INDEX,
    LLDP_MED_POW_PRIO_INDEX,
    LLDP_MED_CAP_INDEX,
    LLDP_MODE_INDEX,
    LLDP_BOND_SLAVE_SRC_MAC_TYPE_INDEX,
    LLDP_INDEX_MAX
};

#define LLDP_CHASSISID_SUBTYPE_DECODE       LLDP_DECODE_LIST
#define LLDP_PORTID_SUBTYPE_DECODE          LLDP_DECODE_LIST
#define LLDP_DOT3_MAU_DECODE                LLDP_DECODE_LIST
#define LLDP_DOT3_POWER_DECODE              LLDP_DECODE_LIST
#define LLDP_DOT3_POWERPAIRS_DECODE         LLDP_DECODE_LIST
#define LLDP_DOT3_POWER_8023AT_DECODE       LLDP_DECODE_LIST
#define LLDP_DOT3_POWER_SOURCE_DECODE       LLDP_DECODE_LIST
#define LLDP_DOT3_POWER_PRIO_DECODE         LLDP_DECODE_LIST
#define LLDP_DOT3_LINK_AUTONEG_DECODE       LLDP_DECODE_BITMAP
#define LLDP_CAP_DECODE                     LLDP_DECODE_BITMAP
#define LLDP_PPVID_CAP_DECODE               LLDP_DECODE_BITMAP
#define LLDP_MGMT_ADDR_DECODE               LLDP_DECODE_LIST
#define LLDP_MGMT_IFACE_DECODE              LLDP_DECODE_LIST
#define LLDP_MED_CLASS_DECODE               LLDP_DECODE_LIST
#define LLDP_MED_APPTYPE_DECODE             LLDP_DECODE_LIST
#define LLDP_MED_LOCFORMAT_DECODE           LLDP_DECODE_LIST
#define LLDP_MED_LOCATION_GEOID_DECODE      LLDP_DECODE_LIST
#define LLDP_MED_LOCATION_ALTITUDE_DECODE   LLDP_DECODE_LIST
#define LLDP_MED_POW_TYPE_DECODE            LLDP_DECODE_LIST
#define LLDP_MED_POW_SOURCE_DECODE          LLDP_DECODE_LIST
#define LLDP_MED_POW_PRIO_DECODE            LLDP_DECODE_LIST
#define LLDP_MED_CAP_DECODE                 LLDP_DECODE_BITMAP
#define LLDPD_MODE_DECODE                   LLDP_DECODE_LIST
#define LLDP_BOND_SLAVE_SRC_MAC_TYPE_DECODE LLDP_DECODE_LIST

enum {
p_chassis_c_cap_available,
p_chassis_c_cap_enabled,
p_chassis_c_descr,
p_chassis_c_id,
p_chassis_c_id_len,
p_chassis_c_id_subtype,
p_chassis_c_index,
p_chassis_c_name,
p_chassis_c_protocol,
p_chassis_c_refcount,
p_chassis_c_ttl,
p_macphy_autoneg_advertised,
p_macphy_autoneg_enabled,
p_macphy_autoneg_support,
p_macphy_mau_type,
p_nbr_p_descr,
p_nbr_p_hidden_in,
p_nbr_p_hidden_out,
p_nbr_p_id,
p_nbr_p_id_len,
p_nbr_p_id_subtype,
p_nbr_p_lastchange,
p_nbr_p_lastupdate,
p_nbr_p_mfs,
p_nbr_p_protocol,
p_nbr_p_pvid,
p_power_allocated,
p_power_class,
p_power_devicetype,
p_power_enabled,
p_power_paircontrol,
p_power_pairs,
p_power_powertype,
p_power_priority,
p_power_requested,
p_power_source,
p_power_supported,
lldp_tlv_map_last
};

/* check any stat/counter changes every so often and report if changed */
#define LLDP_CHECK_STATS_FREQUENCY_DFLT             5 /* seconds */
#define LLDP_CHECK_STATS_FREQUENCY_DFLT_MSEC \
    (LLDP_CHECK_STATS_FREQUENCY_DFLT * 1000)        /* milliseconds */

void lldpd_ovsdb_init(int argc, char *argv[]);
void init_ovspoll_to_libevent(struct lldpd *cfg);
void lldpd_ovsdb_exit(void);
void add_lldpd_hardware_interface(struct lldpd_hardware *hw);
void del_lldpd_hardware_interface(struct lldpd_hardware *hw);
void ovs_libevent_schedule_nbr(void *arg);
void add_vlans_from_ovsdb(char *hw_name);
#endif //lldpd_ovsdb_if.h

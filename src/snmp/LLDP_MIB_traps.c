#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>
#include <sched.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"
#include "openswitch-idl.h"
#include "openvswitch/vlog.h"
#include "smap.h"

VLOG_DEFINE_THIS_MODULE(LLDP_MIB_snmp_traps);

#define MAX_PORT_STRING_LEN 10
#define MAX_UPTIME_STRING_LEN 20
#define MAX_COMMUNITY_STR_LEN 64

oid objid_enterprise[] = {1, 3, 6, 1, 4, 1, 3, 1, 1};
oid objid_sysdescr[] = {1, 3, 6, 1, 2, 1, 1, 1, 0};
oid objid_sysuptime[] = {1, 3, 6, 1, 2, 1, 1, 3, 0};
oid objid_snmptrap[] = {1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0};

static int status;

void init_ovsdb_snmp_notifications(struct ovsdb_idl *idl) {
    ovsdb_idl_add_table(idl, &ovsrec_table_snmp_trap);
    ovsdb_idl_add_column(idl, &ovsrec_snmp_trap_col_community_name);
    ovsdb_idl_add_column(idl, &ovsrec_snmp_trap_col_receiver_address);
    ovsdb_idl_add_column(idl, &ovsrec_snmp_trap_col_receiver_udp_port);
    ovsdb_idl_add_column(idl, &ovsrec_snmp_trap_col_type);
    ovsdb_idl_add_column(idl, &ovsrec_snmp_trap_col_version);
}

static oid objid_lldpStatsRemTablesInserts[] = {1, 0, 8802, 1, 1, 2, 1, 2, 2};
static oid objid_lldpStatsRemTablesDeletes[] = {1, 0, 8802, 1, 1, 2, 1, 2, 3};
static oid objid_lldpStatsRemTablesDrops[] = {1, 0, 8802, 1, 1, 2, 1, 2, 4};
static oid objid_lldpStatsRemTablesAgeouts[] = {1, 0, 8802, 1, 1, 2, 1, 2, 5};
void send_lldpRemTablesChange(struct ovsdb_idl *idl,
                              const char *lldpStatsRemTablesInserts_value,
                              const char *lldpStatsRemTablesDeletes_value,
                              const char *lldpStatsRemTablesDrops_value,
                              const char *lldpStatsRemTablesAgeouts_value) {
    int globalns_fd = 0, swns_fd = 0;
    globalns_fd = open("/proc/1/ns/net", O_RDONLY);
    if (globalns_fd == -1) {
        VLOG_ERR("Failed to open global namespace fd");
        return;
    }
    swns_fd = open("/var/run/netns/swns", O_RDONLY);
    if (swns_fd == -1) {
        VLOG_ERR("Failed to open swns namespace fd");
        return;
    }
    if (setns(globalns_fd, 0) == -1) {
        VLOG_ERR("Failed to switch to global namespace");
        return;
    }

    const struct ovsrec_snmp_trap *trap_row = ovsrec_snmp_trap_first(idl);
    if (trap_row == NULL) {
        VLOG_ERR("ovsrec_snmp_trap_first failed to return trap row in "
                 "send_trap_pdu");
        goto namespace_cleanup;
    }

    OVSREC_SNMP_TRAP_FOR_EACH(trap_row, idl) {
        netsnmp_session session, *ss = NULL;
        netsnmp_pdu *pdu = NULL, *response = NULL;
        char *temp_peername = NULL;
        int inform = 0;
        SOCK_STARTUP;
        snmp_sess_init(&session);

        char trap_community[MAX_COMMUNITY_STR_LEN] = {0};
        strncpy(trap_community, trap_row->community_name,
                MAX_COMMUNITY_STR_LEN);
        if (trap_community == NULL) {
            strncpy(trap_community, "public", MAX_COMMUNITY_STR_LEN);
        }
        session.community = (u_char *)trap_community;
        session.community_len = strlen(trap_community);

        const char *trap_type = trap_row->type;
        const char *trap_version = trap_row->version;
        if (strcmp(trap_type, OVSREC_SNMP_TRAP_TYPE_INFORM) == 0) {
            inform = 1;
            pdu = snmp_pdu_create(SNMP_MSG_INFORM);
        }

        if (strcmp(trap_version, OVSREC_SNMP_TRAP_VERSION_V1) == 0) {
            session.version = SNMP_VERSION_1;
            pdu = snmp_pdu_create(SNMP_MSG_TRAP);
        } else if (strcmp(trap_version, OVSREC_SNMP_TRAP_VERSION_V2C) == 0) {
            session.version = SNMP_VERSION_2c;
            if (inform != 1) {
                pdu = snmp_pdu_create(SNMP_MSG_TRAP2);
            }
        } else if (strcmp(trap_version, OVSREC_SNMP_TRAP_VERSION_V3) == 0) {
            session.version = SNMP_VERSION_3;
            if (inform != 1) {
                pdu = snmp_pdu_create(SNMP_MSG_TRAP2);
            }
        }

        if (pdu == NULL) {
            VLOG_ERR("Failed to create notification PDUn");
            SOCK_CLEANUP;
            goto namespace_cleanup;
        }

        char temp_port[MAX_PORT_STRING_LEN];
        snprintf(temp_port, 10, "%d", (int)trap_row->receiver_udp_port);
        temp_peername = (char *)calloc(strlen(trap_row->receiver_address) +
                                           strlen(temp_port) + 2,
                                       sizeof(char));
        strncpy(temp_peername, trap_row->receiver_address,
                strlen(trap_row->receiver_address));
        strcat(temp_peername, ":");
        strcat(temp_peername, temp_port);
        session.peername = temp_peername;
        ss = snmp_add(&session, netsnmp_transport_open_client("snmptrap",
                                                              session.peername),
                      NULL, NULL);
        if (ss == NULL) {
            snmp_sess_perror("snmptrap", &session);
            VLOG_ERR("Failed in snmp_add for %s", session.peername);
            goto namespace_cleanup;
        }

        long sysuptime;
        char csysuptime[MAX_UPTIME_STRING_LEN];

        sysuptime = get_uptime();
        sprintf(csysuptime, "%ld", sysuptime);
        snmp_add_var(pdu, objid_sysuptime,
                     sizeof(objid_sysuptime) / sizeof(oid), 't', csysuptime);
        if (snmp_add_var(pdu, objid_snmptrap,
                         sizeof(objid_snmptrap) / sizeof(oid), 'o',
                         "1.0.8802.1.1.2.0.0.1") != 0) {
            goto cleanup;
        }

        if (snmp_add_var(pdu, objid_lldpStatsRemTablesInserts,
                         sizeof(objid_lldpStatsRemTablesInserts) / sizeof(oid),
                         'i', lldpStatsRemTablesInserts_value) != 0) {
            goto cleanup;
        }

        if (snmp_add_var(pdu, objid_lldpStatsRemTablesDeletes,
                         sizeof(objid_lldpStatsRemTablesDeletes) / sizeof(oid),
                         'i', lldpStatsRemTablesDeletes_value) != 0) {
            goto cleanup;
        }

        if (snmp_add_var(pdu, objid_lldpStatsRemTablesDrops,
                         sizeof(objid_lldpStatsRemTablesDrops) / sizeof(oid),
                         'i', lldpStatsRemTablesDrops_value) != 0) {
            goto cleanup;
        }

        if (snmp_add_var(pdu, objid_lldpStatsRemTablesAgeouts,
                         sizeof(objid_lldpStatsRemTablesAgeouts) / sizeof(oid),
                         'i', lldpStatsRemTablesAgeouts_value) != 0) {
            goto cleanup;
        }

        if (inform) {
            status = snmp_synch_response(ss, pdu, &response);
        } else {
            status = snmp_send(ss, pdu) == 0;
        }

    cleanup:
        if (!temp_peername) {
            free(temp_peername);
        }

        if (status) {
            snmp_sess_perror(inform ? "snmpinform" : "snmptrap", ss);
            if (!inform && pdu != NULL) {
                snmp_free_pdu(pdu);
            }
        } else if (inform && response != NULL) {
            snmp_free_pdu(response);
        }

        if (ss != NULL) {
            snmp_close(ss);
        }
        SOCK_CLEANUP;
    }

namespace_cleanup:
    if (setns(swns_fd, 0) == -1) {
        VLOG_ERR("Failed to switch back to swns");
    }

    snmp_shutdown("snmpapp");
}

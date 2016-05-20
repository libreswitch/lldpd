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
#include "snmptrap_lib.h"

VLOG_DEFINE_THIS_MODULE(LLDP_MIB_snmp_traps);

oid objid_enterprise[] = {1, 3, 6, 1, 4, 1, 3, 1, 1};
oid objid_sysdescr[] = {1, 3, 6, 1, 2, 1, 1, 1, 0};
oid objid_sysuptime[] = {1, 3, 6, 1, 2, 1, 1, 3, 0};
oid objid_snmptrap[] = {1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0};

void init_ovsdb_snmp_notifications(struct ovsdb_idl *idl) {
    ovsdb_idl_add_column(idl, &ovsrec_system_col_system_mac);

    ovsdb_idl_add_table(idl, &ovsrec_table_snmp_trap);
    ovsdb_idl_add_column(idl, &ovsrec_snmp_trap_col_community_name);
    ovsdb_idl_add_column(idl, &ovsrec_snmp_trap_col_receiver_address);
    ovsdb_idl_add_column(idl, &ovsrec_snmp_trap_col_receiver_udp_port);
    ovsdb_idl_add_column(idl, &ovsrec_snmp_trap_col_type);
    ovsdb_idl_add_column(idl, &ovsrec_snmp_trap_col_version);

    ovsdb_idl_add_table(idl, &ovsrec_table_snmpv3_user);
    ovsdb_idl_add_column(idl, &ovsrec_snmpv3_user_col_auth_protocol);
    ovsdb_idl_add_column(idl, &ovsrec_snmpv3_user_col_auth_pass_phrase);
    ovsdb_idl_add_column(idl, &ovsrec_snmpv3_user_col_priv_pass_phrase);
    ovsdb_idl_add_column(idl, &ovsrec_snmpv3_user_col_user_name);
    ovsdb_idl_add_column(idl, &ovsrec_snmpv3_user_col_priv_protocol);
}

static oid objid_lldpStatsRemTablesInserts[] = {1, 0, 8802, 1, 1, 2, 1, 2, 2};
static oid objid_lldpStatsRemTablesDeletes[] = {1, 0, 8802, 1, 1, 2, 1, 2, 3};
static oid objid_lldpStatsRemTablesDrops[] = {1, 0, 8802, 1, 1, 2, 1, 2, 4};
static oid objid_lldpStatsRemTablesAgeouts[] = {1, 0, 8802, 1, 1, 2, 1, 2, 5};

int send_lldpRemTablesChange(const namespace_type nm_type,
                             struct ovsdb_idl *idl,
                             const char *lldpStatsRemTablesInserts_value,
                             const char *lldpStatsRemTablesDeletes_value,
                             const char *lldpStatsRemTablesDrops_value,
                             const char *lldpStatsRemTablesAgeouts_value) {
    const struct ovsrec_snmp_trap *trap_row = ovsrec_snmp_trap_first(idl);
    if (trap_row == NULL) {
        VLOG_DBG("ovsrec_snmp_trap_first failed to return trap row");
        return -1;
    }

    OVSREC_SNMP_TRAP_FOR_EACH(trap_row, idl) {
        init_snmp("snmpapp");
        netsnmp_session session, *ss = NULL;
        netsnmp_pdu *pdu = NULL, *response = NULL;
        int status = 0;
        int inform = 0;
        SOCK_STARTUP;
        snmp_sess_init(&session);

        const char *trap_type = trap_row->type;
        const char *trap_version = trap_row->version;
        if (strcmp(trap_version, OVSREC_SNMP_TRAP_VERSION_V1) == 0) {
            session.version = SNMP_VERSION_1;
            pdu = snmp_pdu_create(SNMP_MSG_TRAP);
            if (ops_add_snmp_trap_community(&session, trap_row) < 0) {
                VLOG_ERR("Failed in ops_add_snmp_trap_community");
                goto loop_cleanup;
            }
        } else if (strcmp(trap_version, OVSREC_SNMP_TRAP_VERSION_V2C) == 0) {
            session.version = SNMP_VERSION_2c;
            if (ops_add_snmp_trap_community(&session, trap_row) < 0) {
                VLOG_ERR("Failed in ops_add_snmp_trap_community");
                goto loop_cleanup;
            }
            if (strcmp(trap_type, OVSREC_SNMP_TRAP_TYPE_INFORM) == 0) {
                inform = 1;
                pdu = snmp_pdu_create(SNMP_MSG_INFORM);
            } else {
                pdu = snmp_pdu_create(SNMP_MSG_TRAP2);
            }
        } else if (strcmp(trap_version, OVSREC_SNMP_TRAP_VERSION_V3) == 0) {
            session.version = SNMP_VERSION_3;
            if (ops_add_snmpv3_user(idl, &session, trap_row) < 0) {
                VLOG_ERR("Failed in adding ops_add_snmpv3_user");
                goto loop_cleanup;
            }
            if (strcmp(trap_type, OVSREC_SNMP_TRAP_TYPE_INFORM) == 0) {
                inform = 1;
                pdu = snmp_pdu_create(SNMP_MSG_INFORM);
            } else {
                pdu = snmp_pdu_create(SNMP_MSG_TRAP2);
            }
        }

        if (pdu == NULL) {
            VLOG_ERR("Failed to create notification PDU");
            goto loop_cleanup;
        }

        long sysuptime;
        char csysuptime[MAX_UPTIME_STR_LEN];

        sysuptime = get_uptime();
        sprintf(csysuptime, "%ld", sysuptime);
        status = snmp_add_var(pdu, objid_sysuptime,
                              sizeof(objid_sysuptime) / sizeof(oid), 't',
                              csysuptime);
        if (status != 0) {
            VLOG_ERR("Failed to add var uptime to pdu: %d", status);
            goto loop_cleanup;
        }

        status = snmp_add_var(pdu, objid_snmptrap,
                              sizeof(objid_snmptrap) / sizeof(oid), 'o',
                              "1.0.8802.1.1.2.0.0.1");
        if (status != 0) {
            VLOG_ERR("Failed to add var snmptrap to pdu %d", status);
            goto loop_cleanup;
        }

        status =
            snmp_add_var(pdu, objid_lldpStatsRemTablesInserts,
                         sizeof(objid_lldpStatsRemTablesInserts) / sizeof(oid),
                         'u', lldpStatsRemTablesInserts_value);
        if (status != 0) {
            VLOG_ERR("Failed to add var lldpStatsRemTablesInserts to pdu %d",
                     status);
            goto loop_cleanup;
        }

        status =
            snmp_add_var(pdu, objid_lldpStatsRemTablesDeletes,
                         sizeof(objid_lldpStatsRemTablesDeletes) / sizeof(oid),
                         'u', lldpStatsRemTablesDeletes_value);
        if (status != 0) {
            VLOG_ERR("Failed to add var lldpStatsRemTablesDeletes to pdu %d",
                     status);
            goto loop_cleanup;
        }

        status =
            snmp_add_var(pdu, objid_lldpStatsRemTablesDrops,
                         sizeof(objid_lldpStatsRemTablesDrops) / sizeof(oid),
                         'u', lldpStatsRemTablesDrops_value);
        if (status != 0) {
            VLOG_ERR("Failed to add var lldpStatsRemTablesDrops to pdu %d",
                     status);
            goto loop_cleanup;
        }

        status =
            snmp_add_var(pdu, objid_lldpStatsRemTablesAgeouts,
                         sizeof(objid_lldpStatsRemTablesAgeouts) / sizeof(oid),
                         'u', lldpStatsRemTablesAgeouts_value);
        if (status != 0) {
            VLOG_ERR("Failed to add var lldpStatsRemTablesAgeouts to pdu %d",
                     status);
            goto loop_cleanup;
        }

        status = ops_snmp_send_trap(nm_type, trap_row, &session, ss, pdu,
                                    response, inform);
        if (status < 0) {
            VLOG_ERR("Failed in ops_snmp_send_trap");
            goto loop_cleanup;
        }

    loop_cleanup:
        if (status) {
            VLOG_ERR(inform ? "snmpinform failed with status: %d"
                            : "snmptrap failed with status: %d",
                     status);
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
        snmp_shutdown("snmptrap");
    }
    return 0;
}

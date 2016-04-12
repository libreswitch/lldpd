#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/mib_modules.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"

#include "lldpStatsRxPortTable.h"
#include "lldpStatsRxPortTable_interface.h"

const oid lldpStatsRxPortTable_oid[] = {LLDPSTATSRXPORTTABLE_OID};
const int lldpStatsRxPortTable_oid_size = OID_LENGTH(lldpStatsRxPortTable_oid);

lldpStatsRxPortTable_registration lldpStatsRxPortTable_user_context;
void initialize_table_lldpStatsRxPortTable(void);
void shutdown_table_lldpStatsRxPortTable(void);

void init_lldpStatsRxPortTable(void) {
    DEBUGMSGTL(
        ("verbose:lldpStatsRxPortTable:init_lldpStatsRxPortTable", "called\n"));

    lldpStatsRxPortTable_registration *user_context;
    u_long flags;

    user_context = netsnmp_create_data_list("lldpStatsRxPortTable", NULL, NULL);
    flags = 0;

    _lldpStatsRxPortTable_initialize_interface(user_context, flags);

    ovsdb_idl_add_column(idl, &ovsrec_interface_col_lldp_statistics);
    ovsdb_idl_add_column(idl, &ovsrec_interface_col_name);
}

void shutdown_lldpStatsRxPortTable(void) {
    _lldpStatsRxPortTable_shutdown_interface(
        &lldpStatsRxPortTable_user_context);
}

int lldpStatsRxPortTable_rowreq_ctx_init(
    lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx, void *user_init_ctx) {
    DEBUGMSGTL(
        ("verbose:lldpStatsRxPortTable:lldpStatsRxPortTable_rowreq_ctx_init",
         "called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    return MFD_SUCCESS;
}

void lldpStatsRxPortTable_rowreq_ctx_cleanup(
    lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(
        ("verbose:lldpStatsRxPortTable:lldpStatsRxPortTable_rowreq_ctx_cleanup",
         "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
}

int lldpStatsRxPortTable_pre_request(
    lldpStatsRxPortTable_registration *user_context) {
    DEBUGMSGTL(("verbose:lldpStatsRxPortTable:lldpStatsRxPortTable_pre_request",
                "called\n"));
    return MFD_SUCCESS;
}

int lldpStatsRxPortTable_post_request(
    lldpStatsRxPortTable_registration *user_context, int rc) {
    DEBUGMSGTL(
        ("verbose:lldpStatsRxPortTable:lldpStatsRxPortTable_post_request",
         "called\n"));
    return MFD_SUCCESS;
}

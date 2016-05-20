#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/mib_modules.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"

#include "lldpStatsTxPortTable.h"
#include "lldpStatsTxPortTable_interface.h"

const oid lldpStatsTxPortTable_oid[] = {LLDPSTATSTXPORTTABLE_OID};
const int lldpStatsTxPortTable_oid_size = OID_LENGTH(lldpStatsTxPortTable_oid);

lldpStatsTxPortTable_registration lldpStatsTxPortTable_user_context;
void initialize_table_lldpStatsTxPortTable(void);
void shutdown_table_lldpStatsTxPortTable(void);

void init_lldpStatsTxPortTable(void) {
    DEBUGMSGTL(
        ("verbose:lldpStatsTxPortTable:init_lldpStatsTxPortTable", "called\n"));

    lldpStatsTxPortTable_registration *user_context;
    u_long flags;

    user_context = netsnmp_create_data_list("lldpStatsTxPortTable", NULL, NULL);
    flags = 0;

    _lldpStatsTxPortTable_initialize_interface(user_context, flags);

    ovsdb_idl_add_column(idl, &ovsrec_interface_col_lldp_statistics);
    ovsdb_idl_add_column(idl, &ovsrec_interface_col_name);
}

void shutdown_lldpStatsTxPortTable(void) {
    _lldpStatsTxPortTable_shutdown_interface(
        &lldpStatsTxPortTable_user_context);
}

int lldpStatsTxPortTable_rowreq_ctx_init(
    lldpStatsTxPortTable_rowreq_ctx *rowreq_ctx, void *user_init_ctx) {
    DEBUGMSGTL(
        ("verbose:lldpStatsTxPortTable:lldpStatsTxPortTable_rowreq_ctx_init",
         "called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    return MFD_SUCCESS;
}

void lldpStatsTxPortTable_rowreq_ctx_cleanup(
    lldpStatsTxPortTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(
        ("verbose:lldpStatsTxPortTable:lldpStatsTxPortTable_rowreq_ctx_cleanup",
         "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
}

int lldpStatsTxPortTable_pre_request(
    lldpStatsTxPortTable_registration *user_context) {
    DEBUGMSGTL(("verbose:lldpStatsTxPortTable:lldpStatsTxPortTable_pre_request",
                "called\n"));
    return MFD_SUCCESS;
}

int lldpStatsTxPortTable_post_request(
    lldpStatsTxPortTable_registration *user_context, int rc) {
    DEBUGMSGTL(
        ("verbose:lldpStatsTxPortTable:lldpStatsTxPortTable_post_request",
         "called\n"));
    return MFD_SUCCESS;
}

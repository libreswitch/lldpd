#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/mib_modules.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"

#include "lldpConfigManAddrTable.h"
#include "lldpConfigManAddrTable_interface.h"

const oid lldpConfigManAddrTable_oid[] = {LLDPCONFIGMANADDRTABLE_OID};
const int lldpConfigManAddrTable_oid_size =
    OID_LENGTH(lldpConfigManAddrTable_oid);

lldpConfigManAddrTable_registration lldpConfigManAddrTable_user_context;
void initialize_table_lldpConfigManAddrTable(void);
void shutdown_table_lldpConfigManAddrTable(void);

void init_lldpConfigManAddrTable(void) {
    DEBUGMSGTL(("verbose:lldpConfigManAddrTable:init_lldpConfigManAddrTable",
                "called\n"));

    lldpConfigManAddrTable_registration *user_context;
    u_long flags;

    user_context =
        netsnmp_create_data_list("lldpConfigManAddrTable", NULL, NULL);
    flags = 0;

    _lldpConfigManAddrTable_initialize_interface(user_context, flags);

    ovsdb_idl_add_column(idl, &ovsrec_system_col_other_config);
}

void shutdown_lldpConfigManAddrTable(void) {
    _lldpConfigManAddrTable_shutdown_interface(
        &lldpConfigManAddrTable_user_context);
}

int lldpConfigManAddrTable_rowreq_ctx_init(
    lldpConfigManAddrTable_rowreq_ctx *rowreq_ctx, void *user_init_ctx) {
    DEBUGMSGTL((
        "verbose:lldpConfigManAddrTable:lldpConfigManAddrTable_rowreq_ctx_init",
        "called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    return MFD_SUCCESS;
}

void lldpConfigManAddrTable_rowreq_ctx_cleanup(
    lldpConfigManAddrTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(("verbose:lldpConfigManAddrTable:lldpConfigManAddrTable_rowreq_"
                "ctx_cleanup",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
}

int lldpConfigManAddrTable_pre_request(
    lldpConfigManAddrTable_registration *user_context) {
    DEBUGMSGTL(
        ("verbose:lldpConfigManAddrTable:lldpConfigManAddrTable_pre_request",
         "called\n"));
    return MFD_SUCCESS;
}

int lldpConfigManAddrTable_post_request(
    lldpConfigManAddrTable_registration *user_context, int rc) {
    DEBUGMSGTL(
        ("verbose:lldpConfigManAddrTable:lldpConfigManAddrTable_post_request",
         "called\n"));
    return MFD_SUCCESS;
}

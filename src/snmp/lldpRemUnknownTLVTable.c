#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/mib_modules.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"

#include "lldpRemUnknownTLVTable.h"
#include "lldpRemUnknownTLVTable_interface.h"

const oid lldpRemUnknownTLVTable_oid[] = {LLDPREMUNKNOWNTLVTABLE_OID};
const int lldpRemUnknownTLVTable_oid_size =
    OID_LENGTH(lldpRemUnknownTLVTable_oid);

lldpRemUnknownTLVTable_registration lldpRemUnknownTLVTable_user_context;
void initialize_table_lldpRemUnknownTLVTable(void);
void shutdown_table_lldpRemUnknownTLVTable(void);

void init_lldpRemUnknownTLVTable(void) {
    DEBUGMSGTL(("verbose:lldpRemUnknownTLVTable:init_lldpRemUnknownTLVTable",
                "called\n"));

    lldpRemUnknownTLVTable_registration *user_context;
    u_long flags;

    user_context =
        netsnmp_create_data_list("lldpRemUnknownTLVTable", NULL, NULL);
    flags = 0;

    _lldpRemUnknownTLVTable_initialize_interface(user_context, flags);

    ovsdb_idl_add_column(idl, &ovsrec_interface_col_name);
}

void shutdown_lldpRemUnknownTLVTable(void) {
    _lldpRemUnknownTLVTable_shutdown_interface(
        &lldpRemUnknownTLVTable_user_context);
}

int lldpRemUnknownTLVTable_rowreq_ctx_init(
    lldpRemUnknownTLVTable_rowreq_ctx *rowreq_ctx, void *user_init_ctx) {
    DEBUGMSGTL((
        "verbose:lldpRemUnknownTLVTable:lldpRemUnknownTLVTable_rowreq_ctx_init",
        "called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    return MFD_SUCCESS;
}

void lldpRemUnknownTLVTable_rowreq_ctx_cleanup(
    lldpRemUnknownTLVTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(("verbose:lldpRemUnknownTLVTable:lldpRemUnknownTLVTable_rowreq_"
                "ctx_cleanup",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
}

int lldpRemUnknownTLVTable_pre_request(
    lldpRemUnknownTLVTable_registration *user_context) {
    DEBUGMSGTL(
        ("verbose:lldpRemUnknownTLVTable:lldpRemUnknownTLVTable_pre_request",
         "called\n"));
    return MFD_SUCCESS;
}

int lldpRemUnknownTLVTable_post_request(
    lldpRemUnknownTLVTable_registration *user_context, int rc) {
    DEBUGMSGTL(
        ("verbose:lldpRemUnknownTLVTable:lldpRemUnknownTLVTable_post_request",
         "called\n"));
    return MFD_SUCCESS;
}

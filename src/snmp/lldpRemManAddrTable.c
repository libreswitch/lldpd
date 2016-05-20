#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/mib_modules.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"

#include "lldpRemManAddrTable.h"
#include "lldpRemManAddrTable_interface.h"

const oid lldpRemManAddrTable_oid[] = {LLDPREMMANADDRTABLE_OID};
const int lldpRemManAddrTable_oid_size = OID_LENGTH(lldpRemManAddrTable_oid);

lldpRemManAddrTable_registration lldpRemManAddrTable_user_context;
void initialize_table_lldpRemManAddrTable(void);
void shutdown_table_lldpRemManAddrTable(void);

void init_lldpRemManAddrTable(void) {
    DEBUGMSGTL(
        ("verbose:lldpRemManAddrTable:init_lldpRemManAddrTable", "called\n"));

    lldpRemManAddrTable_registration *user_context;
    u_long flags;

    user_context = netsnmp_create_data_list("lldpRemManAddrTable", NULL, NULL);
    flags = 0;

    _lldpRemManAddrTable_initialize_interface(user_context, flags);

    ovsdb_idl_add_column(idl, &ovsrec_system_col_other_config);
    ovsdb_idl_add_column(idl, &ovsrec_interface_col_name);
}

void shutdown_lldpRemManAddrTable(void) {
    _lldpRemManAddrTable_shutdown_interface(&lldpRemManAddrTable_user_context);
}

int lldpRemManAddrTable_rowreq_ctx_init(
    lldpRemManAddrTable_rowreq_ctx *rowreq_ctx, void *user_init_ctx) {
    DEBUGMSGTL(
        ("verbose:lldpRemManAddrTable:lldpRemManAddrTable_rowreq_ctx_init",
         "called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    return MFD_SUCCESS;
}

void lldpRemManAddrTable_rowreq_ctx_cleanup(
    lldpRemManAddrTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(
        ("verbose:lldpRemManAddrTable:lldpRemManAddrTable_rowreq_ctx_cleanup",
         "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
}

int lldpRemManAddrTable_pre_request(
    lldpRemManAddrTable_registration *user_context) {
    DEBUGMSGTL(("verbose:lldpRemManAddrTable:lldpRemManAddrTable_pre_request",
                "called\n"));
    return MFD_SUCCESS;
}

int lldpRemManAddrTable_post_request(
    lldpRemManAddrTable_registration *user_context, int rc) {
    DEBUGMSGTL(("verbose:lldpRemManAddrTable:lldpRemManAddrTable_post_request",
                "called\n"));
    return MFD_SUCCESS;
}

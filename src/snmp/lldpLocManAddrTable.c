#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/mib_modules.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"

#include "lldpLocManAddrTable.h"
#include "lldpLocManAddrTable_interface.h"

const oid lldpLocManAddrTable_oid[] = {LLDPLOCMANADDRTABLE_OID};
const int lldpLocManAddrTable_oid_size = OID_LENGTH(lldpLocManAddrTable_oid);

lldpLocManAddrTable_registration lldpLocManAddrTable_user_context;
void initialize_table_lldpLocManAddrTable(void);
void shutdown_table_lldpLocManAddrTable(void);

void init_lldpLocManAddrTable(void) {
    DEBUGMSGTL(
        ("verbose:lldpLocManAddrTable:init_lldpLocManAddrTable", "called\n"));

    lldpLocManAddrTable_registration *user_context;
    u_long flags;

    user_context = netsnmp_create_data_list("lldpLocManAddrTable", NULL, NULL);
    flags = 0;

    _lldpLocManAddrTable_initialize_interface(user_context, flags);

    ovsdb_idl_add_column(idl, &ovsrec_system_col_other_config);
}

void shutdown_lldpLocManAddrTable(void) {
    _lldpLocManAddrTable_shutdown_interface(&lldpLocManAddrTable_user_context);
}

int lldpLocManAddrTable_rowreq_ctx_init(
    lldpLocManAddrTable_rowreq_ctx *rowreq_ctx, void *user_init_ctx) {
    DEBUGMSGTL(
        ("verbose:lldpLocManAddrTable:lldpLocManAddrTable_rowreq_ctx_init",
         "called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    return MFD_SUCCESS;
}

void lldpLocManAddrTable_rowreq_ctx_cleanup(
    lldpLocManAddrTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(
        ("verbose:lldpLocManAddrTable:lldpLocManAddrTable_rowreq_ctx_cleanup",
         "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
}

int lldpLocManAddrTable_pre_request(
    lldpLocManAddrTable_registration *user_context) {
    DEBUGMSGTL(("verbose:lldpLocManAddrTable:lldpLocManAddrTable_pre_request",
                "called\n"));
    return MFD_SUCCESS;
}

int lldpLocManAddrTable_post_request(
    lldpLocManAddrTable_registration *user_context, int rc) {
    DEBUGMSGTL(("verbose:lldpLocManAddrTable:lldpLocManAddrTable_post_request",
                "called\n"));
    return MFD_SUCCESS;
}

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/mib_modules.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"

#include "lldpRemOrgDefInfoTable.h"
#include "lldpRemOrgDefInfoTable_interface.h"

const oid lldpRemOrgDefInfoTable_oid[] = {LLDPREMORGDEFINFOTABLE_OID};
const int lldpRemOrgDefInfoTable_oid_size =
    OID_LENGTH(lldpRemOrgDefInfoTable_oid);

lldpRemOrgDefInfoTable_registration lldpRemOrgDefInfoTable_user_context;
void initialize_table_lldpRemOrgDefInfoTable(void);
void shutdown_table_lldpRemOrgDefInfoTable(void);

void init_lldpRemOrgDefInfoTable(void) {
    DEBUGMSGTL(("verbose:lldpRemOrgDefInfoTable:init_lldpRemOrgDefInfoTable",
                "called\n"));

    lldpRemOrgDefInfoTable_registration *user_context;
    u_long flags;

    user_context =
        netsnmp_create_data_list("lldpRemOrgDefInfoTable", NULL, NULL);
    flags = 0;

    _lldpRemOrgDefInfoTable_initialize_interface(user_context, flags);

    ovsdb_idl_add_column(idl, &ovsrec_interface_col_name);
}

void shutdown_lldpRemOrgDefInfoTable(void) {
    _lldpRemOrgDefInfoTable_shutdown_interface(
        &lldpRemOrgDefInfoTable_user_context);
}

int lldpRemOrgDefInfoTable_rowreq_ctx_init(
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx, void *user_init_ctx) {
    DEBUGMSGTL((
        "verbose:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_rowreq_ctx_init",
        "called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    return MFD_SUCCESS;
}

void lldpRemOrgDefInfoTable_rowreq_ctx_cleanup(
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(("verbose:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_rowreq_"
                "ctx_cleanup",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
}

int lldpRemOrgDefInfoTable_pre_request(
    lldpRemOrgDefInfoTable_registration *user_context) {
    DEBUGMSGTL(
        ("verbose:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_pre_request",
         "called\n"));
    return MFD_SUCCESS;
}

int lldpRemOrgDefInfoTable_post_request(
    lldpRemOrgDefInfoTable_registration *user_context, int rc) {
    DEBUGMSGTL(
        ("verbose:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_post_request",
         "called\n"));
    return MFD_SUCCESS;
}

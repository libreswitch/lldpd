#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/mib_modules.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"

#include "lldpPortConfigTable.h"
#include "lldpPortConfigTable_interface.h"

const oid lldpPortConfigTable_oid[] = {LLDPPORTCONFIGTABLE_OID};
const int lldpPortConfigTable_oid_size = OID_LENGTH(lldpPortConfigTable_oid);

lldpPortConfigTable_registration lldpPortConfigTable_user_context;
void initialize_table_lldpPortConfigTable(void);
void shutdown_table_lldpPortConfigTable(void);

void init_lldpPortConfigTable(void) {
    DEBUGMSGTL(
        ("verbose:lldpPortConfigTable:init_lldpPortConfigTable", "called\n"));

    lldpPortConfigTable_registration *user_context;
    u_long flags;

    user_context = netsnmp_create_data_list("lldpPortConfigTable", NULL, NULL);
    flags = 0;

    _lldpPortConfigTable_initialize_interface(user_context, flags);

    ovsdb_idl_add_column(idl, &ovsrec_system_col_other_config);
    ovsdb_idl_add_column(idl, &ovsrec_interface_col_other_config);
    ovsdb_idl_add_column(idl, &ovsrec_interface_col_name);
}

void shutdown_lldpPortConfigTable(void) {
    _lldpPortConfigTable_shutdown_interface(&lldpPortConfigTable_user_context);
}

int lldpPortConfigTable_rowreq_ctx_init(
    lldpPortConfigTable_rowreq_ctx *rowreq_ctx, void *user_init_ctx) {
    DEBUGMSGTL(
        ("verbose:lldpPortConfigTable:lldpPortConfigTable_rowreq_ctx_init",
         "called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    return MFD_SUCCESS;
}

void lldpPortConfigTable_rowreq_ctx_cleanup(
    lldpPortConfigTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(
        ("verbose:lldpPortConfigTable:lldpPortConfigTable_rowreq_ctx_cleanup",
         "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
}

int lldpPortConfigTable_pre_request(
    lldpPortConfigTable_registration *user_context) {
    DEBUGMSGTL(("verbose:lldpPortConfigTable:lldpPortConfigTable_pre_request",
                "called\n"));
    return MFD_SUCCESS;
}

int lldpPortConfigTable_post_request(
    lldpPortConfigTable_registration *user_context, int rc) {
    DEBUGMSGTL(("verbose:lldpPortConfigTable:lldpPortConfigTable_post_request",
                "called\n"));
    return MFD_SUCCESS;
}

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/mib_modules.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"

#include "lldpRemTable.h"
#include "lldpRemTable_interface.h"

const oid lldpRemTable_oid[] = {LLDPREMTABLE_OID};
const int lldpRemTable_oid_size = OID_LENGTH(lldpRemTable_oid);

lldpRemTable_registration lldpRemTable_user_context;
void initialize_table_lldpRemTable(void);
void shutdown_table_lldpRemTable(void);

void init_lldpRemTable(void) {
    DEBUGMSGTL(("verbose:lldpRemTable:init_lldpRemTable", "called\n"));

    lldpRemTable_registration *user_context;
    u_long flags;

    user_context = netsnmp_create_data_list("lldpRemTable", NULL, NULL);
    flags = 0;

    _lldpRemTable_initialize_interface(user_context, flags);

    ovsdb_idl_add_column(idl, &ovsrec_system_col_other_config);
    ovsdb_idl_add_column(idl, &ovsrec_interface_col_lldp_neighbor_info);
    ovsdb_idl_add_column(idl, &ovsrec_system_col_hostname);
    ovsdb_idl_add_column(idl, &ovsrec_interface_col_name);
}

void shutdown_lldpRemTable(void) {
    _lldpRemTable_shutdown_interface(&lldpRemTable_user_context);
}

int lldpRemTable_rowreq_ctx_init(lldpRemTable_rowreq_ctx *rowreq_ctx,
                                 void *user_init_ctx) {
    DEBUGMSGTL(
        ("verbose:lldpRemTable:lldpRemTable_rowreq_ctx_init", "called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    return MFD_SUCCESS;
}

void lldpRemTable_rowreq_ctx_cleanup(lldpRemTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(
        ("verbose:lldpRemTable:lldpRemTable_rowreq_ctx_cleanup", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
}

int lldpRemTable_pre_request(lldpRemTable_registration *user_context) {
    DEBUGMSGTL(("verbose:lldpRemTable:lldpRemTable_pre_request", "called\n"));
    return MFD_SUCCESS;
}

int lldpRemTable_post_request(lldpRemTable_registration *user_context, int rc) {
    DEBUGMSGTL(("verbose:lldpRemTable:lldpRemTable_post_request", "called\n"));
    return MFD_SUCCESS;
}

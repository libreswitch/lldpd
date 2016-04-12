#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/mib_modules.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"

#include "lldpLocPortTable.h"
#include "lldpLocPortTable_interface.h"

const oid lldpLocPortTable_oid[] = {LLDPLOCPORTTABLE_OID};
const int lldpLocPortTable_oid_size = OID_LENGTH(lldpLocPortTable_oid);

lldpLocPortTable_registration lldpLocPortTable_user_context;
void initialize_table_lldpLocPortTable(void);
void shutdown_table_lldpLocPortTable(void);

void init_lldpLocPortTable(void) {
    DEBUGMSGTL(("verbose:lldpLocPortTable:init_lldpLocPortTable", "called\n"));

    lldpLocPortTable_registration *user_context;
    u_long flags;

    user_context = netsnmp_create_data_list("lldpLocPortTable", NULL, NULL);
    flags = 0;

    _lldpLocPortTable_initialize_interface(user_context, flags);

    ovsdb_idl_add_column(idl, &ovsrec_interface_col_name);
}

void shutdown_lldpLocPortTable(void) {
    _lldpLocPortTable_shutdown_interface(&lldpLocPortTable_user_context);
}

int lldpLocPortTable_rowreq_ctx_init(lldpLocPortTable_rowreq_ctx *rowreq_ctx,
                                     void *user_init_ctx) {
    DEBUGMSGTL(("verbose:lldpLocPortTable:lldpLocPortTable_rowreq_ctx_init",
                "called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    return MFD_SUCCESS;
}

void lldpLocPortTable_rowreq_ctx_cleanup(
    lldpLocPortTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(("verbose:lldpLocPortTable:lldpLocPortTable_rowreq_ctx_cleanup",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
}

int lldpLocPortTable_pre_request(lldpLocPortTable_registration *user_context) {
    DEBUGMSGTL(
        ("verbose:lldpLocPortTable:lldpLocPortTable_pre_request", "called\n"));
    return MFD_SUCCESS;
}

int lldpLocPortTable_post_request(lldpLocPortTable_registration *user_context,
                                  int rc) {
    DEBUGMSGTL(
        ("verbose:lldpLocPortTable:lldpLocPortTable_post_request", "called\n"));
    return MFD_SUCCESS;
}

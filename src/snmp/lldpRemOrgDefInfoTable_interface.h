#ifndef LLDPREMORGDEFINFOTABLE_INTERFACE_H
#define LLDPREMORGDEFINFOTABLE_INTERFACE_H
#include "lldpRemOrgDefInfoTable.h"

void _lldpRemOrgDefInfoTable_initialize_interface(
    lldpRemOrgDefInfoTable_registration *user_ctx, u_long flags);
void _lldpRemOrgDefInfoTable_shutdown_interface(
    lldpRemOrgDefInfoTable_registration *user_ctx);
lldpRemOrgDefInfoTable_registration *
lldpRemOrgDefInfoTable_registration_get(void);
lldpRemOrgDefInfoTable_registration *lldpRemOrgDefInfoTable_registration_set(
    lldpRemOrgDefInfoTable_registration *newreg);
netsnmp_container *lldpRemOrgDefInfoTable_container_get(void);
int lldpRemOrgDefInfoTable_container_size(void);
lldpRemOrgDefInfoTable_rowreq_ctx *
lldpRemOrgDefInfoTable_allocate_rowreq_ctx(void *);
void lldpRemOrgDefInfoTable_release_rowreq_ctx(
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx);
int lldpRemOrgDefInfoTable_index_to_oid(
    netsnmp_index *oid_idx, lldpRemOrgDefInfoTable_mib_index *mib_idx);
int lldpRemOrgDefInfoTable_index_from_oid(
    netsnmp_index *oid_idx, lldpRemOrgDefInfoTable_mib_index *mib_idx);
void lldpRemOrgDefInfoTable_valid_columns_set(netsnmp_column_info *vc);
#endif
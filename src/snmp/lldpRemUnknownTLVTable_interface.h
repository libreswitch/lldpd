#ifndef LLDPREMUNKNOWNTLVTABLE_INTERFACE_H
#define LLDPREMUNKNOWNTLVTABLE_INTERFACE_H
#include "lldpRemUnknownTLVTable.h"

void _lldpRemUnknownTLVTable_initialize_interface(
    lldpRemUnknownTLVTable_registration *user_ctx, u_long flags);
void _lldpRemUnknownTLVTable_shutdown_interface(
    lldpRemUnknownTLVTable_registration *user_ctx);
lldpRemUnknownTLVTable_registration *
lldpRemUnknownTLVTable_registration_get(void);
lldpRemUnknownTLVTable_registration *lldpRemUnknownTLVTable_registration_set(
    lldpRemUnknownTLVTable_registration *newreg);
netsnmp_container *lldpRemUnknownTLVTable_container_get(void);
int lldpRemUnknownTLVTable_container_size(void);
lldpRemUnknownTLVTable_rowreq_ctx *
lldpRemUnknownTLVTable_allocate_rowreq_ctx(void *);
void lldpRemUnknownTLVTable_release_rowreq_ctx(
    lldpRemUnknownTLVTable_rowreq_ctx *rowreq_ctx);
int lldpRemUnknownTLVTable_index_to_oid(
    netsnmp_index *oid_idx, lldpRemUnknownTLVTable_mib_index *mib_idx);
int lldpRemUnknownTLVTable_index_from_oid(
    netsnmp_index *oid_idx, lldpRemUnknownTLVTable_mib_index *mib_idx);
void lldpRemUnknownTLVTable_valid_columns_set(netsnmp_column_info *vc);
#endif
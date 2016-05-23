#ifndef LLDPREMMANADDRTABLE_INTERFACE_H
#define LLDPREMMANADDRTABLE_INTERFACE_H
#include "lldpRemManAddrTable.h"

void _lldpRemManAddrTable_initialize_interface(
    lldpRemManAddrTable_registration *user_ctx, u_long flags);
void _lldpRemManAddrTable_shutdown_interface(
    lldpRemManAddrTable_registration *user_ctx);
lldpRemManAddrTable_registration *lldpRemManAddrTable_registration_get(void);
lldpRemManAddrTable_registration *
lldpRemManAddrTable_registration_set(lldpRemManAddrTable_registration *newreg);
netsnmp_container *lldpRemManAddrTable_container_get(void);
int lldpRemManAddrTable_container_size(void);
lldpRemManAddrTable_rowreq_ctx *lldpRemManAddrTable_allocate_rowreq_ctx(void *);
void lldpRemManAddrTable_release_rowreq_ctx(
    lldpRemManAddrTable_rowreq_ctx *rowreq_ctx);
int lldpRemManAddrTable_index_to_oid(netsnmp_index *oid_idx,
                                     lldpRemManAddrTable_mib_index *mib_idx);
int lldpRemManAddrTable_index_from_oid(netsnmp_index *oid_idx,
                                       lldpRemManAddrTable_mib_index *mib_idx);
void lldpRemManAddrTable_valid_columns_set(netsnmp_column_info *vc);
#endif
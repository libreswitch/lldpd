#ifndef LLDPCONFIGMANADDRTABLE_INTERFACE_H
#define LLDPCONFIGMANADDRTABLE_INTERFACE_H
#include "lldpConfigManAddrTable.h"

void _lldpConfigManAddrTable_initialize_interface(
    lldpConfigManAddrTable_registration *user_ctx, u_long flags);
void _lldpConfigManAddrTable_shutdown_interface(
    lldpConfigManAddrTable_registration *user_ctx);
lldpConfigManAddrTable_registration *
lldpConfigManAddrTable_registration_get(void);
lldpConfigManAddrTable_registration *lldpConfigManAddrTable_registration_set(
    lldpConfigManAddrTable_registration *newreg);
netsnmp_container *lldpConfigManAddrTable_container_get(void);
int lldpConfigManAddrTable_container_size(void);
lldpConfigManAddrTable_rowreq_ctx *
lldpConfigManAddrTable_allocate_rowreq_ctx(void *);
void lldpConfigManAddrTable_release_rowreq_ctx(
    lldpConfigManAddrTable_rowreq_ctx *rowreq_ctx);
int lldpConfigManAddrTable_index_to_oid(
    netsnmp_index *oid_idx, lldpConfigManAddrTable_mib_index *mib_idx);
int lldpConfigManAddrTable_index_from_oid(
    netsnmp_index *oid_idx, lldpConfigManAddrTable_mib_index *mib_idx);
void lldpConfigManAddrTable_valid_columns_set(netsnmp_column_info *vc);
#endif
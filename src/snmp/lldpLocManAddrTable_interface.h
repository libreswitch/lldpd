#ifndef LLDPLOCMANADDRTABLE_INTERFACE_H
#define LLDPLOCMANADDRTABLE_INTERFACE_H
#include "lldpLocManAddrTable.h"

void _lldpLocManAddrTable_initialize_interface(
    lldpLocManAddrTable_registration *user_ctx, u_long flags);
void _lldpLocManAddrTable_shutdown_interface(
    lldpLocManAddrTable_registration *user_ctx);
lldpLocManAddrTable_registration *lldpLocManAddrTable_registration_get(void);
lldpLocManAddrTable_registration *
lldpLocManAddrTable_registration_set(lldpLocManAddrTable_registration *newreg);
netsnmp_container *lldpLocManAddrTable_container_get(void);
int lldpLocManAddrTable_container_size(void);
lldpLocManAddrTable_rowreq_ctx *lldpLocManAddrTable_allocate_rowreq_ctx(void *);
void lldpLocManAddrTable_release_rowreq_ctx(
    lldpLocManAddrTable_rowreq_ctx *rowreq_ctx);
int lldpLocManAddrTable_index_to_oid(netsnmp_index *oid_idx,
                                     lldpLocManAddrTable_mib_index *mib_idx);
int lldpLocManAddrTable_index_from_oid(netsnmp_index *oid_idx,
                                       lldpLocManAddrTable_mib_index *mib_idx);
void lldpLocManAddrTable_valid_columns_set(netsnmp_column_info *vc);
#endif
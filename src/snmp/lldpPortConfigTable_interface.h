#ifndef LLDPPORTCONFIGTABLE_INTERFACE_H
#define LLDPPORTCONFIGTABLE_INTERFACE_H
#include "lldpPortConfigTable.h"

void _lldpPortConfigTable_initialize_interface(
    lldpPortConfigTable_registration *user_ctx, u_long flags);
void _lldpPortConfigTable_shutdown_interface(
    lldpPortConfigTable_registration *user_ctx);
lldpPortConfigTable_registration *lldpPortConfigTable_registration_get(void);
lldpPortConfigTable_registration *
lldpPortConfigTable_registration_set(lldpPortConfigTable_registration *newreg);
netsnmp_container *lldpPortConfigTable_container_get(void);
int lldpPortConfigTable_container_size(void);
lldpPortConfigTable_rowreq_ctx *lldpPortConfigTable_allocate_rowreq_ctx(void *);
void lldpPortConfigTable_release_rowreq_ctx(
    lldpPortConfigTable_rowreq_ctx *rowreq_ctx);
int lldpPortConfigTable_index_to_oid(netsnmp_index *oid_idx,
                                     lldpPortConfigTable_mib_index *mib_idx);
int lldpPortConfigTable_index_from_oid(netsnmp_index *oid_idx,
                                       lldpPortConfigTable_mib_index *mib_idx);
void lldpPortConfigTable_valid_columns_set(netsnmp_column_info *vc);
#endif
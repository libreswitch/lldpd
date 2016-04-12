#ifndef LLDPREMTABLE_INTERFACE_H
#define LLDPREMTABLE_INTERFACE_H
#include "lldpRemTable.h"

void _lldpRemTable_initialize_interface(lldpRemTable_registration *user_ctx,
                                        u_long flags);
void _lldpRemTable_shutdown_interface(lldpRemTable_registration *user_ctx);
lldpRemTable_registration *lldpRemTable_registration_get(void);
lldpRemTable_registration *
lldpRemTable_registration_set(lldpRemTable_registration *newreg);
netsnmp_container *lldpRemTable_container_get(void);
int lldpRemTable_container_size(void);
lldpRemTable_rowreq_ctx *lldpRemTable_allocate_rowreq_ctx(void *);
void lldpRemTable_release_rowreq_ctx(lldpRemTable_rowreq_ctx *rowreq_ctx);
int lldpRemTable_index_to_oid(netsnmp_index *oid_idx,
                              lldpRemTable_mib_index *mib_idx);
int lldpRemTable_index_from_oid(netsnmp_index *oid_idx,
                                lldpRemTable_mib_index *mib_idx);
void lldpRemTable_valid_columns_set(netsnmp_column_info *vc);
#endif
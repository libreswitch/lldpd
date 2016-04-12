#ifndef LLDPSTATSTXPORTTABLE_INTERFACE_H
#define LLDPSTATSTXPORTTABLE_INTERFACE_H
#include "lldpStatsTxPortTable.h"

void _lldpStatsTxPortTable_initialize_interface(
    lldpStatsTxPortTable_registration *user_ctx, u_long flags);
void _lldpStatsTxPortTable_shutdown_interface(
    lldpStatsTxPortTable_registration *user_ctx);
lldpStatsTxPortTable_registration *lldpStatsTxPortTable_registration_get(void);
lldpStatsTxPortTable_registration *lldpStatsTxPortTable_registration_set(
    lldpStatsTxPortTable_registration *newreg);
netsnmp_container *lldpStatsTxPortTable_container_get(void);
int lldpStatsTxPortTable_container_size(void);
lldpStatsTxPortTable_rowreq_ctx *
lldpStatsTxPortTable_allocate_rowreq_ctx(void *);
void lldpStatsTxPortTable_release_rowreq_ctx(
    lldpStatsTxPortTable_rowreq_ctx *rowreq_ctx);
int lldpStatsTxPortTable_index_to_oid(netsnmp_index *oid_idx,
                                      lldpStatsTxPortTable_mib_index *mib_idx);
int lldpStatsTxPortTable_index_from_oid(
    netsnmp_index *oid_idx, lldpStatsTxPortTable_mib_index *mib_idx);
void lldpStatsTxPortTable_valid_columns_set(netsnmp_column_info *vc);
#endif
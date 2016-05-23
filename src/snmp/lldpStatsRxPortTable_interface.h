#ifndef LLDPSTATSRXPORTTABLE_INTERFACE_H
#define LLDPSTATSRXPORTTABLE_INTERFACE_H
#include "lldpStatsRxPortTable.h"

void _lldpStatsRxPortTable_initialize_interface(
    lldpStatsRxPortTable_registration *user_ctx, u_long flags);
void _lldpStatsRxPortTable_shutdown_interface(
    lldpStatsRxPortTable_registration *user_ctx);
lldpStatsRxPortTable_registration *lldpStatsRxPortTable_registration_get(void);
lldpStatsRxPortTable_registration *lldpStatsRxPortTable_registration_set(
    lldpStatsRxPortTable_registration *newreg);
netsnmp_container *lldpStatsRxPortTable_container_get(void);
int lldpStatsRxPortTable_container_size(void);
lldpStatsRxPortTable_rowreq_ctx *
lldpStatsRxPortTable_allocate_rowreq_ctx(void *);
void lldpStatsRxPortTable_release_rowreq_ctx(
    lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx);
int lldpStatsRxPortTable_index_to_oid(netsnmp_index *oid_idx,
                                      lldpStatsRxPortTable_mib_index *mib_idx);
int lldpStatsRxPortTable_index_from_oid(
    netsnmp_index *oid_idx, lldpStatsRxPortTable_mib_index *mib_idx);
void lldpStatsRxPortTable_valid_columns_set(netsnmp_column_info *vc);
#endif
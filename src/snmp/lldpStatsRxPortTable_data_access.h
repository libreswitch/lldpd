#ifndef LLDPSTATSRXPORTTABLE_DATA_ACCESS_H
#define LLDPSTATSRXPORTTABLE_DATA_ACCESS_H

extern struct ovsdb_idl *idl;

int lldpStatsRxPortTable_init_data(
    lldpStatsRxPortTable_registration *lldpStatsRxPortTable_reg);
#define LLDPSTATSRXPORTTABLE_CACHE_TIMEOUT 30
void lldpStatsRxPortTable_container_init(netsnmp_container **container_ptr_ptr,
                                         netsnmp_cache *cache);
void lldpStatsRxPortTable_container_shutdown(netsnmp_container *container_ptr);
int lldpStatsRxPortTable_container_load(netsnmp_container *container);
void lldpStatsRxPortTable_container_free(netsnmp_container *container);
int lldpStatsRxPortTable_cache_load(netsnmp_container *container);
void lldpStatsRxPortTable_cache_free(netsnmp_container *container);
int lldpStatsRxPortTable_row_prep(lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx);
#endif
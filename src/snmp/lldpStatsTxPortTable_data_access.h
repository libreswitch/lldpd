#ifndef LLDPSTATSTXPORTTABLE_DATA_ACCESS_H
#define LLDPSTATSTXPORTTABLE_DATA_ACCESS_H

extern struct ovsdb_idl *idl;

int lldpStatsTxPortTable_init_data(
    lldpStatsTxPortTable_registration *lldpStatsTxPortTable_reg);
#define LLDPSTATSTXPORTTABLE_CACHE_TIMEOUT 30
void lldpStatsTxPortTable_container_init(netsnmp_container **container_ptr_ptr,
                                         netsnmp_cache *cache);
void lldpStatsTxPortTable_container_shutdown(netsnmp_container *container_ptr);
int lldpStatsTxPortTable_container_load(netsnmp_container *container);
void lldpStatsTxPortTable_container_free(netsnmp_container *container);
int lldpStatsTxPortTable_cache_load(netsnmp_container *container);
void lldpStatsTxPortTable_cache_free(netsnmp_container *container);
int lldpStatsTxPortTable_row_prep(lldpStatsTxPortTable_rowreq_ctx *rowreq_ctx);
#endif
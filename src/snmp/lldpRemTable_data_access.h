#ifndef LLDPREMTABLE_DATA_ACCESS_H
#define LLDPREMTABLE_DATA_ACCESS_H

extern struct ovsdb_idl *idl;

int lldpRemTable_init_data(lldpRemTable_registration *lldpRemTable_reg);
#define LLDPREMTABLE_CACHE_TIMEOUT 30
void lldpRemTable_container_init(netsnmp_container **container_ptr_ptr,
                                 netsnmp_cache *cache);
void lldpRemTable_container_shutdown(netsnmp_container *container_ptr);
int lldpRemTable_container_load(netsnmp_container *container);
void lldpRemTable_container_free(netsnmp_container *container);
int lldpRemTable_cache_load(netsnmp_container *container);
void lldpRemTable_cache_free(netsnmp_container *container);
int lldpRemTable_row_prep(lldpRemTable_rowreq_ctx *rowreq_ctx);
#endif
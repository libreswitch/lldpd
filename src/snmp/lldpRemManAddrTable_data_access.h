#ifndef LLDPREMMANADDRTABLE_DATA_ACCESS_H
#define LLDPREMMANADDRTABLE_DATA_ACCESS_H

extern struct ovsdb_idl *idl;

int lldpRemManAddrTable_init_data(
    lldpRemManAddrTable_registration *lldpRemManAddrTable_reg);
#define LLDPREMMANADDRTABLE_CACHE_TIMEOUT 30
void lldpRemManAddrTable_container_init(netsnmp_container **container_ptr_ptr,
                                        netsnmp_cache *cache);
void lldpRemManAddrTable_container_shutdown(netsnmp_container *container_ptr);
int lldpRemManAddrTable_container_load(netsnmp_container *container);
void lldpRemManAddrTable_container_free(netsnmp_container *container);
int lldpRemManAddrTable_cache_load(netsnmp_container *container);
void lldpRemManAddrTable_cache_free(netsnmp_container *container);
int lldpRemManAddrTable_row_prep(lldpRemManAddrTable_rowreq_ctx *rowreq_ctx);
#endif
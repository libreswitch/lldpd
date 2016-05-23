#ifndef LLDPCONFIGMANADDRTABLE_DATA_ACCESS_H
#define LLDPCONFIGMANADDRTABLE_DATA_ACCESS_H

extern struct ovsdb_idl *idl;

int lldpConfigManAddrTable_init_data(
    lldpConfigManAddrTable_registration *lldpConfigManAddrTable_reg);
#define LLDPCONFIGMANADDRTABLE_CACHE_TIMEOUT 30
void lldpConfigManAddrTable_container_init(
    netsnmp_container **container_ptr_ptr, netsnmp_cache *cache);
void lldpConfigManAddrTable_container_shutdown(
    netsnmp_container *container_ptr);
int lldpConfigManAddrTable_container_load(netsnmp_container *container);
void lldpConfigManAddrTable_container_free(netsnmp_container *container);
int lldpConfigManAddrTable_cache_load(netsnmp_container *container);
void lldpConfigManAddrTable_cache_free(netsnmp_container *container);
int lldpConfigManAddrTable_row_prep(
    lldpConfigManAddrTable_rowreq_ctx *rowreq_ctx);
#endif
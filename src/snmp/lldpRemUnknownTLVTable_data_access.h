#ifndef LLDPREMUNKNOWNTLVTABLE_DATA_ACCESS_H
#define LLDPREMUNKNOWNTLVTABLE_DATA_ACCESS_H

extern struct ovsdb_idl *idl;

int lldpRemUnknownTLVTable_init_data(
    lldpRemUnknownTLVTable_registration *lldpRemUnknownTLVTable_reg);
#define LLDPREMUNKNOWNTLVTABLE_CACHE_TIMEOUT 30
void lldpRemUnknownTLVTable_container_init(
    netsnmp_container **container_ptr_ptr, netsnmp_cache *cache);
void lldpRemUnknownTLVTable_container_shutdown(
    netsnmp_container *container_ptr);
int lldpRemUnknownTLVTable_container_load(netsnmp_container *container);
void lldpRemUnknownTLVTable_container_free(netsnmp_container *container);
int lldpRemUnknownTLVTable_cache_load(netsnmp_container *container);
void lldpRemUnknownTLVTable_cache_free(netsnmp_container *container);
int lldpRemUnknownTLVTable_row_prep(
    lldpRemUnknownTLVTable_rowreq_ctx *rowreq_ctx);
#endif
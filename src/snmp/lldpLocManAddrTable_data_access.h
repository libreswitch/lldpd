#ifndef LLDPLOCMANADDRTABLE_DATA_ACCESS_H
#define LLDPLOCMANADDRTABLE_DATA_ACCESS_H

extern struct ovsdb_idl *idl;

int lldpLocManAddrTable_init_data(
    lldpLocManAddrTable_registration *lldpLocManAddrTable_reg);
#define LLDPLOCMANADDRTABLE_CACHE_TIMEOUT 30
void lldpLocManAddrTable_container_init(netsnmp_container **container_ptr_ptr,
                                        netsnmp_cache *cache);
void lldpLocManAddrTable_container_shutdown(netsnmp_container *container_ptr);
int lldpLocManAddrTable_container_load(netsnmp_container *container);
void lldpLocManAddrTable_container_free(netsnmp_container *container);
int lldpLocManAddrTable_cache_load(netsnmp_container *container);
void lldpLocManAddrTable_cache_free(netsnmp_container *container);
int lldpLocManAddrTable_row_prep(lldpLocManAddrTable_rowreq_ctx *rowreq_ctx);
#endif
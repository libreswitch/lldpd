#ifndef LLDPPORTCONFIGTABLE_DATA_ACCESS_H
#define LLDPPORTCONFIGTABLE_DATA_ACCESS_H

extern struct ovsdb_idl *idl;

int lldpPortConfigTable_init_data(
    lldpPortConfigTable_registration *lldpPortConfigTable_reg);
#define LLDPPORTCONFIGTABLE_CACHE_TIMEOUT 30
void lldpPortConfigTable_container_init(netsnmp_container **container_ptr_ptr,
                                        netsnmp_cache *cache);
void lldpPortConfigTable_container_shutdown(netsnmp_container *container_ptr);
int lldpPortConfigTable_container_load(netsnmp_container *container);
void lldpPortConfigTable_container_free(netsnmp_container *container);
int lldpPortConfigTable_cache_load(netsnmp_container *container);
void lldpPortConfigTable_cache_free(netsnmp_container *container);
int lldpPortConfigTable_row_prep(lldpPortConfigTable_rowreq_ctx *rowreq_ctx);
#endif
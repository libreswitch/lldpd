#ifndef LLDPLOCPORTTABLE_DATA_ACCESS_H
#define LLDPLOCPORTTABLE_DATA_ACCESS_H

extern struct ovsdb_idl *idl;

int lldpLocPortTable_init_data(
    lldpLocPortTable_registration *lldpLocPortTable_reg);
#define LLDPLOCPORTTABLE_CACHE_TIMEOUT 30
void lldpLocPortTable_container_init(netsnmp_container **container_ptr_ptr,
                                     netsnmp_cache *cache);
void lldpLocPortTable_container_shutdown(netsnmp_container *container_ptr);
int lldpLocPortTable_container_load(netsnmp_container *container);
void lldpLocPortTable_container_free(netsnmp_container *container);
int lldpLocPortTable_cache_load(netsnmp_container *container);
void lldpLocPortTable_cache_free(netsnmp_container *container);
int lldpLocPortTable_row_prep(lldpLocPortTable_rowreq_ctx *rowreq_ctx);
#endif
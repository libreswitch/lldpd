#ifndef LLDPREMORGDEFINFOTABLE_DATA_ACCESS_H
#define LLDPREMORGDEFINFOTABLE_DATA_ACCESS_H

extern struct ovsdb_idl *idl;

int lldpRemOrgDefInfoTable_init_data(
    lldpRemOrgDefInfoTable_registration *lldpRemOrgDefInfoTable_reg);
#define LLDPREMORGDEFINFOTABLE_CACHE_TIMEOUT 30
void lldpRemOrgDefInfoTable_container_init(
    netsnmp_container **container_ptr_ptr, netsnmp_cache *cache);
void lldpRemOrgDefInfoTable_container_shutdown(
    netsnmp_container *container_ptr);
int lldpRemOrgDefInfoTable_container_load(netsnmp_container *container);
void lldpRemOrgDefInfoTable_container_free(netsnmp_container *container);
int lldpRemOrgDefInfoTable_cache_load(netsnmp_container *container);
void lldpRemOrgDefInfoTable_cache_free(netsnmp_container *container);
int lldpRemOrgDefInfoTable_row_prep(
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx);
#endif
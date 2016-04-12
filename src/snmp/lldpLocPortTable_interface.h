#ifndef LLDPLOCPORTTABLE_INTERFACE_H
#define LLDPLOCPORTTABLE_INTERFACE_H
#include "lldpLocPortTable.h"

void _lldpLocPortTable_initialize_interface(
    lldpLocPortTable_registration *user_ctx, u_long flags);
void _lldpLocPortTable_shutdown_interface(
    lldpLocPortTable_registration *user_ctx);
lldpLocPortTable_registration *lldpLocPortTable_registration_get(void);
lldpLocPortTable_registration *
lldpLocPortTable_registration_set(lldpLocPortTable_registration *newreg);
netsnmp_container *lldpLocPortTable_container_get(void);
int lldpLocPortTable_container_size(void);
lldpLocPortTable_rowreq_ctx *lldpLocPortTable_allocate_rowreq_ctx(void *);
void lldpLocPortTable_release_rowreq_ctx(
    lldpLocPortTable_rowreq_ctx *rowreq_ctx);
int lldpLocPortTable_index_to_oid(netsnmp_index *oid_idx,
                                  lldpLocPortTable_mib_index *mib_idx);
int lldpLocPortTable_index_from_oid(netsnmp_index *oid_idx,
                                    lldpLocPortTable_mib_index *mib_idx);
void lldpLocPortTable_valid_columns_set(netsnmp_column_info *vc);
#endif
#ifndef LLDPPORTCONFIGTABLE_DATA_GET_H
#define LLDPPORTCONFIGTABLE_DATA_GET_H
int lldpPortConfigAdminStatus_get(lldpPortConfigTable_rowreq_ctx *rowreq_ctx,
                                  long *lldpPortConfigAdminStatus_val_ptr);

int lldpPortConfigNotificationEnable_get(
    lldpPortConfigTable_rowreq_ctx *rowreq_ctx,
    long *lldpPortConfigNotificationEnable_val_ptr);

int lldpPortConfigTLVsTxEnable_get(lldpPortConfigTable_rowreq_ctx *rowreq_ctx,
                                   u_long *lldpPortConfigTLVsTxEnable_val_ptr);

int lldpPortConfigTable_indexes_set_tbl_idx(
    lldpPortConfigTable_mib_index *tbl_idx, long lldpPortConfigPortNum_val);

int lldpPortConfigTable_indexes_set(lldpPortConfigTable_rowreq_ctx *rowreq_ctx,
                                    long lldpPortConfigPortNum_val);
#endif

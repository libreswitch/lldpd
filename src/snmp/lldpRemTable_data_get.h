#ifndef LLDPREMTABLE_DATA_GET_H
#define LLDPREMTABLE_DATA_GET_H
int lldpRemChassisIdSubtype_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                                long *lldpRemChassisIdSubtype_val_ptr);

int lldpRemChassisId_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                         char **lldpRemChassisId_val_ptr_ptr,
                         size_t *lldpRemChassisId_val_ptr_len_ptr);

int lldpRemPortIdSubtype_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                             long *lldpRemPortIdSubtype_val_ptr);

int lldpRemPortId_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                      char **lldpRemPortId_val_ptr_ptr,
                      size_t *lldpRemPortId_val_ptr_len_ptr);

int lldpRemPortDesc_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                        char **lldpRemPortDesc_val_ptr_ptr,
                        size_t *lldpRemPortDesc_val_ptr_len_ptr);

int lldpRemSysName_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                       char **lldpRemSysName_val_ptr_ptr,
                       size_t *lldpRemSysName_val_ptr_len_ptr);

int lldpRemSysDesc_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                       char **lldpRemSysDesc_val_ptr_ptr,
                       size_t *lldpRemSysDesc_val_ptr_len_ptr);

int lldpRemSysCapSupported_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                               u_long *lldpRemSysCapSupported_val_ptr);

int lldpRemSysCapEnabled_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                             u_long *lldpRemSysCapEnabled_val_ptr);

int lldpRemTable_indexes_set_tbl_idx(lldpRemTable_mib_index *tbl_idx,
                                     long lldpRemTimeMark_val,
                                     long lldpRemLocalPortNum_val,
                                     long lldpRemIndex_val);

int lldpRemTable_indexes_set(lldpRemTable_rowreq_ctx *rowreq_ctx,
                             long lldpRemTimeMark_val,
                             long lldpRemLocalPortNum_val,
                             long lldpRemIndex_val);
#endif

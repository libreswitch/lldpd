#ifndef LLDPREMUNKNOWNTLVTABLE_DATA_GET_H
#define LLDPREMUNKNOWNTLVTABLE_DATA_GET_H
int lldpRemUnknownTLVInfo_get(lldpRemUnknownTLVTable_rowreq_ctx *rowreq_ctx,
                              char **lldpRemUnknownTLVInfo_val_ptr_ptr,
                              size_t *lldpRemUnknownTLVInfo_val_ptr_len_ptr);

int lldpRemUnknownTLVTable_indexes_set_tbl_idx(
    lldpRemUnknownTLVTable_mib_index *tbl_idx, long lldpRemTimeMark_val,
    long lldpRemLocalPortNum_val, long lldpRemIndex_val,
    long lldpRemUnknownTLVType_val);

int lldpRemUnknownTLVTable_indexes_set(
    lldpRemUnknownTLVTable_rowreq_ctx *rowreq_ctx, long lldpRemTimeMark_val,
    long lldpRemLocalPortNum_val, long lldpRemIndex_val,
    long lldpRemUnknownTLVType_val);
#endif

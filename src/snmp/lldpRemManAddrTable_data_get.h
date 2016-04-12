#ifndef LLDPREMMANADDRTABLE_DATA_GET_H
#define LLDPREMMANADDRTABLE_DATA_GET_H
int lldpRemManAddrIfSubtype_get(lldpRemManAddrTable_rowreq_ctx *rowreq_ctx,
                                long *lldpRemManAddrIfSubtype_val_ptr);

int lldpRemManAddrIfId_get(lldpRemManAddrTable_rowreq_ctx *rowreq_ctx,
                           long *lldpRemManAddrIfId_val_ptr);

int lldpRemManAddrOID_get(lldpRemManAddrTable_rowreq_ctx *rowreq_ctx,
                          oid **lldpRemManAddrOID_val_ptr_ptr,
                          size_t *lldpRemManAddrOID_val_ptr_len_ptr);

int lldpRemManAddrTable_indexes_set_tbl_idx(
    lldpRemManAddrTable_mib_index *tbl_idx, long lldpRemTimeMark_val,
    long lldpRemLocalPortNum_val, long lldpRemIndex_val,
    long lldpRemManAddrSubtype_val, char *lldpRemManAddr_val_ptr,
    size_t lldpRemManAddr_val_ptr_len);

int lldpRemManAddrTable_indexes_set(lldpRemManAddrTable_rowreq_ctx *rowreq_ctx,
                                    long lldpRemTimeMark_val,
                                    long lldpRemLocalPortNum_val,
                                    long lldpRemIndex_val,
                                    long lldpRemManAddrSubtype_val,
                                    char *lldpRemManAddr_val_ptr,
                                    size_t lldpRemManAddr_val);
#endif

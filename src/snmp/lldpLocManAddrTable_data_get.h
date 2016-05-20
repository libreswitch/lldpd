#ifndef LLDPLOCMANADDRTABLE_DATA_GET_H
#define LLDPLOCMANADDRTABLE_DATA_GET_H
int lldpLocManAddrLen_get(lldpLocManAddrTable_rowreq_ctx *rowreq_ctx,
                          long *lldpLocManAddrLen_val_ptr);

int lldpLocManAddrIfSubtype_get(lldpLocManAddrTable_rowreq_ctx *rowreq_ctx,
                                long *lldpLocManAddrIfSubtype_val_ptr);

int lldpLocManAddrIfId_get(lldpLocManAddrTable_rowreq_ctx *rowreq_ctx,
                           long *lldpLocManAddrIfId_val_ptr);

int lldpLocManAddrOID_get(lldpLocManAddrTable_rowreq_ctx *rowreq_ctx,
                          oid **lldpLocManAddrOID_val_ptr_ptr,
                          size_t *lldpLocManAddrOID_val_ptr_len_ptr);

int lldpLocManAddrTable_indexes_set_tbl_idx(
    lldpLocManAddrTable_mib_index *tbl_idx, long lldpLocManAddrSubtype_val,
    char *lldpLocManAddr_val_ptr, size_t lldpLocManAddr_val_ptr_len);

int lldpLocManAddrTable_indexes_set(lldpLocManAddrTable_rowreq_ctx *rowreq_ctx,
                                    long lldpLocManAddrSubtype_val,
                                    char *lldpLocManAddr_val_ptr,
                                    size_t lldpLocManAddr_val);
#endif

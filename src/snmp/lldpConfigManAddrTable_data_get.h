#ifndef LLDPCONFIGMANADDRTABLE_DATA_GET_H
#define LLDPCONFIGMANADDRTABLE_DATA_GET_H
int lldpConfigManAddrPortsTxEnable_get(
    lldpConfigManAddrTable_rowreq_ctx *rowreq_ctx,
    char **lldpConfigManAddrPortsTxEnable_val_ptr_ptr,
    size_t *lldpConfigManAddrPortsTxEnable_val_ptr_len_ptr);

int lldpConfigManAddrTable_indexes_set_tbl_idx(
    lldpConfigManAddrTable_mib_index *tbl_idx, long lldpLocManAddrSubtype_val,
    char *lldpLocManAddr_val_ptr, size_t lldpLocManAddr_val_ptr_len);

int lldpConfigManAddrTable_indexes_set(
    lldpConfigManAddrTable_rowreq_ctx *rowreq_ctx,
    long lldpLocManAddrSubtype_val, char *lldpLocManAddr_val_ptr,
    size_t lldpLocManAddr_val);
#endif

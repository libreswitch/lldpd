#ifndef LLDPREMORGDEFINFOTABLE_DATA_GET_H
#define LLDPREMORGDEFINFOTABLE_DATA_GET_H
int lldpRemOrgDefInfo_get(lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx,
                          char **lldpRemOrgDefInfo_val_ptr_ptr,
                          size_t *lldpRemOrgDefInfo_val_ptr_len_ptr);

int lldpRemOrgDefInfoTable_indexes_set_tbl_idx(
    lldpRemOrgDefInfoTable_mib_index *tbl_idx, long lldpRemTimeMark_val,
    long lldpRemLocalPortNum_val, long lldpRemIndex_val,
    char *lldpRemOrgDefInfoOUI_val_ptr, size_t lldpRemOrgDefInfoOUI_val_ptr_len,
    long lldpRemOrgDefInfoSubtype_val, long lldpRemOrgDefInfoIndex_val);

int lldpRemOrgDefInfoTable_indexes_set(
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx, long lldpRemTimeMark_val,
    long lldpRemLocalPortNum_val, long lldpRemIndex_val,
    char *lldpRemOrgDefInfoOUI_val_ptr, size_t lldpRemOrgDefInfoOUI_val,
    long lldpRemOrgDefInfoSubtype_val, long lldpRemOrgDefInfoIndex_val);
#endif

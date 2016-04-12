#ifndef LLDPLOCPORTTABLE_DATA_GET_H
#define LLDPLOCPORTTABLE_DATA_GET_H
int lldpLocPortIdSubtype_get(lldpLocPortTable_rowreq_ctx *rowreq_ctx,
                             long *lldpLocPortIdSubtype_val_ptr);

int lldpLocPortId_get(lldpLocPortTable_rowreq_ctx *rowreq_ctx,
                      char **lldpLocPortId_val_ptr_ptr,
                      size_t *lldpLocPortId_val_ptr_len_ptr);

int lldpLocPortDesc_get(lldpLocPortTable_rowreq_ctx *rowreq_ctx,
                        char **lldpLocPortDesc_val_ptr_ptr,
                        size_t *lldpLocPortDesc_val_ptr_len_ptr);

int lldpLocPortTable_indexes_set_tbl_idx(lldpLocPortTable_mib_index *tbl_idx,
                                         long lldpLocPortNum_val);

int lldpLocPortTable_indexes_set(lldpLocPortTable_rowreq_ctx *rowreq_ctx,
                                 long lldpLocPortNum_val);
#endif

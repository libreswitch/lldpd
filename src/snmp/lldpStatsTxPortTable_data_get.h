#ifndef LLDPSTATSTXPORTTABLE_DATA_GET_H
#define LLDPSTATSTXPORTTABLE_DATA_GET_H
int lldpStatsTxPortFramesTotal_get(lldpStatsTxPortTable_rowreq_ctx *rowreq_ctx,
                                   long *lldpStatsTxPortFramesTotal_val_ptr);

int lldpStatsTxPortTable_indexes_set_tbl_idx(
    lldpStatsTxPortTable_mib_index *tbl_idx, long lldpStatsTxPortNum_val);

int lldpStatsTxPortTable_indexes_set(
    lldpStatsTxPortTable_rowreq_ctx *rowreq_ctx, long lldpStatsTxPortNum_val);
#endif

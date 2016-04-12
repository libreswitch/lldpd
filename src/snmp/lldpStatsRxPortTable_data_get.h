#ifndef LLDPSTATSRXPORTTABLE_DATA_GET_H
#define LLDPSTATSRXPORTTABLE_DATA_GET_H
int lldpStatsRxPortFramesDiscardedTotal_get(
    lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx,
    long *lldpStatsRxPortFramesDiscardedTotal_val_ptr);

int lldpStatsRxPortFramesErrors_get(lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx,
                                    long *lldpStatsRxPortFramesErrors_val_ptr);

int lldpStatsRxPortFramesTotal_get(lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx,
                                   long *lldpStatsRxPortFramesTotal_val_ptr);

int lldpStatsRxPortTLVsDiscardedTotal_get(
    lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx,
    long *lldpStatsRxPortTLVsDiscardedTotal_val_ptr);

int lldpStatsRxPortTLVsUnrecognizedTotal_get(
    lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx,
    long *lldpStatsRxPortTLVsUnrecognizedTotal_val_ptr);

int lldpStatsRxPortAgeoutsTotal_get(lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx,
                                    long *lldpStatsRxPortAgeoutsTotal_val_ptr);

int lldpStatsRxPortTable_indexes_set_tbl_idx(
    lldpStatsRxPortTable_mib_index *tbl_idx, long lldpStatsRxPortNum_val);

int lldpStatsRxPortTable_indexes_set(
    lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx, long lldpStatsRxPortNum_val);
#endif

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "lldpStatsRxPortTable.h"

int lldpStatsRxPortTable_indexes_set_tbl_idx(
    lldpStatsRxPortTable_mib_index *tbl_idx, long lldpStatsRxPortNum_val) {
    DEBUGMSGTL((
        "verbose:lldpStatsRxPortTable:lldpStatsRxPortTable_indexes_set_tbl_idx",
        "called\n"));

    tbl_idx->lldpStatsRxPortNum = lldpStatsRxPortNum_val;
    return MFD_SUCCESS;
}

int lldpStatsRxPortTable_indexes_set(
    lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx, long lldpStatsRxPortNum_val) {
    DEBUGMSGTL(("verbose:lldpStatsRxPortTable:lldpStatsRxPortTable_indexes_set",
                "called\n"));
    if (MFD_SUCCESS != lldpStatsRxPortTable_indexes_set_tbl_idx(
                           &rowreq_ctx->tbl_idx, lldpStatsRxPortNum_val)) {
        return MFD_ERROR;
    }
    rowreq_ctx->oid_idx.len = sizeof(rowreq_ctx->oid_tmp) / sizeof(oid);
    if (0 != lldpStatsRxPortTable_index_to_oid(&rowreq_ctx->oid_idx,
                                               &rowreq_ctx->tbl_idx)) {
        return MFD_ERROR;
    }
    return MFD_SUCCESS;
}

int lldpStatsRxPortFramesDiscardedTotal_get(
    lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx,
    long *lldpStatsRxPortFramesDiscardedTotal_val_ptr) {
    netsnmp_assert(NULL != lldpStatsRxPortFramesDiscardedTotal_val_ptr);
    DEBUGMSGTL(
        ("verbose:lldpStatsRxPortTable:lldpStatsRxPortFramesDiscardedTotal_get",
         "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpStatsRxPortFramesDiscardedTotal_val_ptr) =
        rowreq_ctx->data.lldpStatsRxPortFramesDiscardedTotal;
    return MFD_SUCCESS;
}

int lldpStatsRxPortFramesErrors_get(lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx,
                                    long *lldpStatsRxPortFramesErrors_val_ptr) {
    netsnmp_assert(NULL != lldpStatsRxPortFramesErrors_val_ptr);
    DEBUGMSGTL(("verbose:lldpStatsRxPortTable:lldpStatsRxPortFramesErrors_get",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpStatsRxPortFramesErrors_val_ptr) =
        rowreq_ctx->data.lldpStatsRxPortFramesErrors;
    return MFD_SUCCESS;
}

int lldpStatsRxPortFramesTotal_get(lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx,
                                   long *lldpStatsRxPortFramesTotal_val_ptr) {
    netsnmp_assert(NULL != lldpStatsRxPortFramesTotal_val_ptr);
    DEBUGMSGTL(("verbose:lldpStatsRxPortTable:lldpStatsRxPortFramesTotal_get",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpStatsRxPortFramesTotal_val_ptr) =
        rowreq_ctx->data.lldpStatsRxPortFramesTotal;
    return MFD_SUCCESS;
}

int lldpStatsRxPortTLVsDiscardedTotal_get(
    lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx,
    long *lldpStatsRxPortTLVsDiscardedTotal_val_ptr) {
    netsnmp_assert(NULL != lldpStatsRxPortTLVsDiscardedTotal_val_ptr);
    DEBUGMSGTL(
        ("verbose:lldpStatsRxPortTable:lldpStatsRxPortTLVsDiscardedTotal_get",
         "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpStatsRxPortTLVsDiscardedTotal_val_ptr) =
        rowreq_ctx->data.lldpStatsRxPortTLVsDiscardedTotal;
    return MFD_SUCCESS;
}

int lldpStatsRxPortTLVsUnrecognizedTotal_get(
    lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx,
    long *lldpStatsRxPortTLVsUnrecognizedTotal_val_ptr) {
    netsnmp_assert(NULL != lldpStatsRxPortTLVsUnrecognizedTotal_val_ptr);
    DEBUGMSGTL((
        "verbose:lldpStatsRxPortTable:lldpStatsRxPortTLVsUnrecognizedTotal_get",
        "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpStatsRxPortTLVsUnrecognizedTotal_val_ptr) =
        rowreq_ctx->data.lldpStatsRxPortTLVsUnrecognizedTotal;
    return MFD_SUCCESS;
}

int lldpStatsRxPortAgeoutsTotal_get(lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx,
                                    long *lldpStatsRxPortAgeoutsTotal_val_ptr) {
    netsnmp_assert(NULL != lldpStatsRxPortAgeoutsTotal_val_ptr);
    DEBUGMSGTL(("verbose:lldpStatsRxPortTable:lldpStatsRxPortAgeoutsTotal_get",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpStatsRxPortAgeoutsTotal_val_ptr) =
        rowreq_ctx->data.lldpStatsRxPortAgeoutsTotal;
    return MFD_SUCCESS;
}

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "lldpStatsTxPortTable.h"

int lldpStatsTxPortTable_indexes_set_tbl_idx(
    lldpStatsTxPortTable_mib_index *tbl_idx, long lldpStatsTxPortNum_val) {
    DEBUGMSGTL((
        "verbose:lldpStatsTxPortTable:lldpStatsTxPortTable_indexes_set_tbl_idx",
        "called\n"));

    tbl_idx->lldpStatsTxPortNum = lldpStatsTxPortNum_val;
    return MFD_SUCCESS;
}

int lldpStatsTxPortTable_indexes_set(
    lldpStatsTxPortTable_rowreq_ctx *rowreq_ctx, long lldpStatsTxPortNum_val) {
    DEBUGMSGTL(("verbose:lldpStatsTxPortTable:lldpStatsTxPortTable_indexes_set",
                "called\n"));
    if (MFD_SUCCESS != lldpStatsTxPortTable_indexes_set_tbl_idx(
                           &rowreq_ctx->tbl_idx, lldpStatsTxPortNum_val)) {
        return MFD_ERROR;
    }
    rowreq_ctx->oid_idx.len = sizeof(rowreq_ctx->oid_tmp) / sizeof(oid);
    if (0 != lldpStatsTxPortTable_index_to_oid(&rowreq_ctx->oid_idx,
                                               &rowreq_ctx->tbl_idx)) {
        return MFD_ERROR;
    }
    return MFD_SUCCESS;
}

int lldpStatsTxPortFramesTotal_get(lldpStatsTxPortTable_rowreq_ctx *rowreq_ctx,
                                   long *lldpStatsTxPortFramesTotal_val_ptr) {
    netsnmp_assert(NULL != lldpStatsTxPortFramesTotal_val_ptr);
    DEBUGMSGTL(("verbose:lldpStatsTxPortTable:lldpStatsTxPortFramesTotal_get",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpStatsTxPortFramesTotal_val_ptr) =
        rowreq_ctx->data.lldpStatsTxPortFramesTotal;
    return MFD_SUCCESS;
}

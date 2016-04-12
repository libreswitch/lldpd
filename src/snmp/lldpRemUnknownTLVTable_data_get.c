#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "lldpRemUnknownTLVTable.h"

int lldpRemUnknownTLVTable_indexes_set_tbl_idx(
    lldpRemUnknownTLVTable_mib_index *tbl_idx, long lldpRemTimeMark_val,
    long lldpRemLocalPortNum_val, long lldpRemIndex_val,
    long lldpRemUnknownTLVType_val) {
    DEBUGMSGTL(("verbose:lldpRemUnknownTLVTable:lldpRemUnknownTLVTable_indexes_"
                "set_tbl_idx",
                "called\n"));

    tbl_idx->lldpRemTimeMark = lldpRemTimeMark_val;
    tbl_idx->lldpRemLocalPortNum = lldpRemLocalPortNum_val;
    tbl_idx->lldpRemIndex = lldpRemIndex_val;
    tbl_idx->lldpRemUnknownTLVType = lldpRemUnknownTLVType_val;
    return MFD_SUCCESS;
}

int lldpRemUnknownTLVTable_indexes_set(
    lldpRemUnknownTLVTable_rowreq_ctx *rowreq_ctx, long lldpRemTimeMark_val,
    long lldpRemLocalPortNum_val, long lldpRemIndex_val,
    long lldpRemUnknownTLVType_val) {
    DEBUGMSGTL(
        ("verbose:lldpRemUnknownTLVTable:lldpRemUnknownTLVTable_indexes_set",
         "called\n"));
    if (MFD_SUCCESS != lldpRemUnknownTLVTable_indexes_set_tbl_idx(
                           &rowreq_ctx->tbl_idx, lldpRemTimeMark_val,
                           lldpRemLocalPortNum_val, lldpRemIndex_val,
                           lldpRemUnknownTLVType_val)) {
        return MFD_ERROR;
    }
    rowreq_ctx->oid_idx.len = sizeof(rowreq_ctx->oid_tmp) / sizeof(oid);
    if (0 != lldpRemUnknownTLVTable_index_to_oid(&rowreq_ctx->oid_idx,
                                                 &rowreq_ctx->tbl_idx)) {
        return MFD_ERROR;
    }
    return MFD_SUCCESS;
}

int lldpRemUnknownTLVInfo_get(lldpRemUnknownTLVTable_rowreq_ctx *rowreq_ctx,
                              char **lldpRemUnknownTLVInfo_val_ptr_ptr,
                              size_t *lldpRemUnknownTLVInfo_val_ptr_len_ptr) {
    netsnmp_assert((NULL != lldpRemUnknownTLVInfo_val_ptr_ptr) &&
                   (NULL != *lldpRemUnknownTLVInfo_val_ptr_ptr));
    netsnmp_assert(NULL != lldpRemUnknownTLVInfo_val_ptr_len_ptr);
    DEBUGMSGTL(("verbose:lldpRemUnknownTLVTable:lldpRemUnknownTLVInfo_get",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    if ((NULL == (*lldpRemUnknownTLVInfo_val_ptr_ptr)) ||
        ((*lldpRemUnknownTLVInfo_val_ptr_len_ptr) <
         (rowreq_ctx->data.lldpRemUnknownTLVInfo_len *
          sizeof(rowreq_ctx->data.lldpRemUnknownTLVInfo[0])))) {
        (*lldpRemUnknownTLVInfo_val_ptr_ptr) =
            malloc(rowreq_ctx->data.lldpRemUnknownTLVInfo_len *
                   sizeof(rowreq_ctx->data.lldpRemUnknownTLVInfo[0]));
        if (NULL == (*lldpRemUnknownTLVInfo_val_ptr_ptr)) {
            snmp_log(LOG_ERR, "could not allocate memory "
                              "(rowreq_ctx->data.lldpRemUnknownTLVInfo)\n");
            return MFD_ERROR;
        }
    }
    (*lldpRemUnknownTLVInfo_val_ptr_len_ptr) =
        rowreq_ctx->data.lldpRemUnknownTLVInfo_len *
        sizeof(rowreq_ctx->data.lldpRemUnknownTLVInfo[0]);
    memcpy((*lldpRemUnknownTLVInfo_val_ptr_ptr),
           rowreq_ctx->data.lldpRemUnknownTLVInfo,
           rowreq_ctx->data.lldpRemUnknownTLVInfo_len *
               sizeof(rowreq_ctx->data.lldpRemUnknownTLVInfo[0]));
    return MFD_SUCCESS;
}

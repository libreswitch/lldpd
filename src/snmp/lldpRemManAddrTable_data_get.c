#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "lldpRemManAddrTable.h"

int lldpRemManAddrTable_indexes_set_tbl_idx(
    lldpRemManAddrTable_mib_index *tbl_idx, long lldpRemTimeMark_val,
    long lldpRemLocalPortNum_val, long lldpRemIndex_val,
    long lldpRemManAddrSubtype_val, char *lldpRemManAddr_val_ptr,
    size_t lldpRemManAddr_val_ptr_len) {
    DEBUGMSGTL(
        ("verbose:lldpRemManAddrTable:lldpRemManAddrTable_indexes_set_tbl_idx",
         "called\n"));

    tbl_idx->lldpRemTimeMark = lldpRemTimeMark_val;
    tbl_idx->lldpRemLocalPortNum = lldpRemLocalPortNum_val;
    tbl_idx->lldpRemIndex = lldpRemIndex_val;
    tbl_idx->lldpRemManAddrSubtype = lldpRemManAddrSubtype_val;

    tbl_idx->lldpRemManAddr_len =
        sizeof(tbl_idx->lldpRemManAddr) / sizeof(tbl_idx->lldpRemManAddr[0]);
    if ((NULL == tbl_idx->lldpRemManAddr) ||
        (tbl_idx->lldpRemManAddr_len < (lldpRemManAddr_val_ptr_len))) {
        snmp_log(LOG_ERR,
                 "not enough space for value (lldpRemManAddr_val_ptr)\n");
        return MFD_ERROR;
    }
    tbl_idx->lldpRemManAddr_len = lldpRemManAddr_val_ptr_len;
    memcpy(tbl_idx->lldpRemManAddr, lldpRemManAddr_val_ptr,
           lldpRemManAddr_val_ptr_len * sizeof(lldpRemManAddr_val_ptr[0]));
    return MFD_SUCCESS;
}

int lldpRemManAddrTable_indexes_set(lldpRemManAddrTable_rowreq_ctx *rowreq_ctx,
                                    long lldpRemTimeMark_val,
                                    long lldpRemLocalPortNum_val,
                                    long lldpRemIndex_val,
                                    long lldpRemManAddrSubtype_val,
                                    char *lldpRemManAddr_val_ptr,
                                    size_t lldpRemManAddr_val_ptr_len) {
    DEBUGMSGTL(("verbose:lldpRemManAddrTable:lldpRemManAddrTable_indexes_set",
                "called\n"));
    if (MFD_SUCCESS != lldpRemManAddrTable_indexes_set_tbl_idx(
                           &rowreq_ctx->tbl_idx, lldpRemTimeMark_val,
                           lldpRemLocalPortNum_val, lldpRemIndex_val,
                           lldpRemManAddrSubtype_val, lldpRemManAddr_val_ptr,
                           lldpRemManAddr_val_ptr_len)) {
        return MFD_ERROR;
    }
    rowreq_ctx->oid_idx.len = sizeof(rowreq_ctx->oid_tmp) / sizeof(oid);
    if (0 != lldpRemManAddrTable_index_to_oid(&rowreq_ctx->oid_idx,
                                              &rowreq_ctx->tbl_idx)) {
        return MFD_ERROR;
    }
    return MFD_SUCCESS;
}

int lldpRemManAddrIfSubtype_get(lldpRemManAddrTable_rowreq_ctx *rowreq_ctx,
                                long *lldpRemManAddrIfSubtype_val_ptr) {
    netsnmp_assert(NULL != lldpRemManAddrIfSubtype_val_ptr);
    DEBUGMSGTL(("verbose:lldpRemManAddrTable:lldpRemManAddrIfSubtype_get",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpRemManAddrIfSubtype_val_ptr) =
        rowreq_ctx->data.lldpRemManAddrIfSubtype;
    return MFD_SUCCESS;
}

int lldpRemManAddrIfId_get(lldpRemManAddrTable_rowreq_ctx *rowreq_ctx,
                           long *lldpRemManAddrIfId_val_ptr) {
    netsnmp_assert(NULL != lldpRemManAddrIfId_val_ptr);
    DEBUGMSGTL(
        ("verbose:lldpRemManAddrTable:lldpRemManAddrIfId_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpRemManAddrIfId_val_ptr) = rowreq_ctx->data.lldpRemManAddrIfId;
    return MFD_SUCCESS;
}

int lldpRemManAddrOID_get(lldpRemManAddrTable_rowreq_ctx *rowreq_ctx,
                          oid **lldpRemManAddrOID_val_ptr_ptr,
                          size_t *lldpRemManAddrOID_val_ptr_len_ptr) {
    netsnmp_assert((NULL != lldpRemManAddrOID_val_ptr_ptr) &&
                   (NULL != *lldpRemManAddrOID_val_ptr_ptr));
    netsnmp_assert(NULL != lldpRemManAddrOID_val_ptr_len_ptr);
    DEBUGMSGTL(
        ("verbose:lldpRemManAddrTable:lldpRemManAddrOID_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    if ((NULL == (*lldpRemManAddrOID_val_ptr_ptr)) ||
        ((*lldpRemManAddrOID_val_ptr_len_ptr) <
         (rowreq_ctx->data.lldpRemManAddrOID_len *
          sizeof(rowreq_ctx->data.lldpRemManAddrOID[0])))) {
        (*lldpRemManAddrOID_val_ptr_ptr) =
            malloc(rowreq_ctx->data.lldpRemManAddrOID_len *
                   sizeof(rowreq_ctx->data.lldpRemManAddrOID[0]));
        if (NULL == (*lldpRemManAddrOID_val_ptr_ptr)) {
            snmp_log(LOG_ERR, "could not allocate memory "
                              "(rowreq_ctx->data.lldpRemManAddrOID)\n");
            return MFD_ERROR;
        }
    }
    (*lldpRemManAddrOID_val_ptr_len_ptr) =
        rowreq_ctx->data.lldpRemManAddrOID_len *
        sizeof(rowreq_ctx->data.lldpRemManAddrOID[0]);
    memcpy((*lldpRemManAddrOID_val_ptr_ptr), rowreq_ctx->data.lldpRemManAddrOID,
           rowreq_ctx->data.lldpRemManAddrOID_len *
               sizeof(rowreq_ctx->data.lldpRemManAddrOID[0]));
    return MFD_SUCCESS;
}

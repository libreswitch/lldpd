#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "lldpLocManAddrTable.h"

int lldpLocManAddrTable_indexes_set_tbl_idx(
    lldpLocManAddrTable_mib_index *tbl_idx, long lldpLocManAddrSubtype_val,
    char *lldpLocManAddr_val_ptr, size_t lldpLocManAddr_val_ptr_len) {
    DEBUGMSGTL(
        ("verbose:lldpLocManAddrTable:lldpLocManAddrTable_indexes_set_tbl_idx",
         "called\n"));

    tbl_idx->lldpLocManAddrSubtype = lldpLocManAddrSubtype_val;

    tbl_idx->lldpLocManAddr_len =
        sizeof(tbl_idx->lldpLocManAddr) / sizeof(tbl_idx->lldpLocManAddr[0]);
    if ((NULL == tbl_idx->lldpLocManAddr) ||
        (tbl_idx->lldpLocManAddr_len < (lldpLocManAddr_val_ptr_len))) {
        snmp_log(LOG_ERR,
                 "not enough space for value (lldpLocManAddr_val_ptr)\n");
        return MFD_ERROR;
    }
    tbl_idx->lldpLocManAddr_len = lldpLocManAddr_val_ptr_len;
    memcpy(tbl_idx->lldpLocManAddr, lldpLocManAddr_val_ptr,
           lldpLocManAddr_val_ptr_len * sizeof(lldpLocManAddr_val_ptr[0]));
    return MFD_SUCCESS;
}

int lldpLocManAddrTable_indexes_set(lldpLocManAddrTable_rowreq_ctx *rowreq_ctx,
                                    long lldpLocManAddrSubtype_val,
                                    char *lldpLocManAddr_val_ptr,
                                    size_t lldpLocManAddr_val_ptr_len) {
    DEBUGMSGTL(("verbose:lldpLocManAddrTable:lldpLocManAddrTable_indexes_set",
                "called\n"));
    if (MFD_SUCCESS != lldpLocManAddrTable_indexes_set_tbl_idx(
                           &rowreq_ctx->tbl_idx, lldpLocManAddrSubtype_val,
                           lldpLocManAddr_val_ptr,
                           lldpLocManAddr_val_ptr_len)) {
        return MFD_ERROR;
    }
    rowreq_ctx->oid_idx.len = sizeof(rowreq_ctx->oid_tmp) / sizeof(oid);
    if (0 != lldpLocManAddrTable_index_to_oid(&rowreq_ctx->oid_idx,
                                              &rowreq_ctx->tbl_idx)) {
        return MFD_ERROR;
    }
    return MFD_SUCCESS;
}

int lldpLocManAddrLen_get(lldpLocManAddrTable_rowreq_ctx *rowreq_ctx,
                          long *lldpLocManAddrLen_val_ptr) {
    netsnmp_assert(NULL != lldpLocManAddrLen_val_ptr);
    DEBUGMSGTL(
        ("verbose:lldpLocManAddrTable:lldpLocManAddrLen_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpLocManAddrLen_val_ptr) = rowreq_ctx->data.lldpLocManAddrLen;
    return MFD_SUCCESS;
}

int lldpLocManAddrIfSubtype_get(lldpLocManAddrTable_rowreq_ctx *rowreq_ctx,
                                long *lldpLocManAddrIfSubtype_val_ptr) {
    netsnmp_assert(NULL != lldpLocManAddrIfSubtype_val_ptr);
    DEBUGMSGTL(("verbose:lldpLocManAddrTable:lldpLocManAddrIfSubtype_get",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpLocManAddrIfSubtype_val_ptr) =
        rowreq_ctx->data.lldpLocManAddrIfSubtype;
    return MFD_SUCCESS;
}

int lldpLocManAddrIfId_get(lldpLocManAddrTable_rowreq_ctx *rowreq_ctx,
                           long *lldpLocManAddrIfId_val_ptr) {
    netsnmp_assert(NULL != lldpLocManAddrIfId_val_ptr);
    DEBUGMSGTL(
        ("verbose:lldpLocManAddrTable:lldpLocManAddrIfId_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpLocManAddrIfId_val_ptr) = rowreq_ctx->data.lldpLocManAddrIfId;
    return MFD_SUCCESS;
}

int lldpLocManAddrOID_get(lldpLocManAddrTable_rowreq_ctx *rowreq_ctx,
                          oid **lldpLocManAddrOID_val_ptr_ptr,
                          size_t *lldpLocManAddrOID_val_ptr_len_ptr) {
    netsnmp_assert((NULL != lldpLocManAddrOID_val_ptr_ptr) &&
                   (NULL != *lldpLocManAddrOID_val_ptr_ptr));
    netsnmp_assert(NULL != lldpLocManAddrOID_val_ptr_len_ptr);
    DEBUGMSGTL(
        ("verbose:lldpLocManAddrTable:lldpLocManAddrOID_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    if ((NULL == (*lldpLocManAddrOID_val_ptr_ptr)) ||
        ((*lldpLocManAddrOID_val_ptr_len_ptr) <
         (rowreq_ctx->data.lldpLocManAddrOID_len *
          sizeof(rowreq_ctx->data.lldpLocManAddrOID[0])))) {
        (*lldpLocManAddrOID_val_ptr_ptr) =
            malloc(rowreq_ctx->data.lldpLocManAddrOID_len *
                   sizeof(rowreq_ctx->data.lldpLocManAddrOID[0]));
        if (NULL == (*lldpLocManAddrOID_val_ptr_ptr)) {
            snmp_log(LOG_ERR, "could not allocate memory "
                              "(rowreq_ctx->data.lldpLocManAddrOID)\n");
            return MFD_ERROR;
        }
    }
    (*lldpLocManAddrOID_val_ptr_len_ptr) =
        rowreq_ctx->data.lldpLocManAddrOID_len *
        sizeof(rowreq_ctx->data.lldpLocManAddrOID[0]);
    memcpy((*lldpLocManAddrOID_val_ptr_ptr), rowreq_ctx->data.lldpLocManAddrOID,
           rowreq_ctx->data.lldpLocManAddrOID_len *
               sizeof(rowreq_ctx->data.lldpLocManAddrOID[0]));
    return MFD_SUCCESS;
}

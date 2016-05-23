#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "lldpConfigManAddrTable.h"

int lldpConfigManAddrTable_indexes_set_tbl_idx(
    lldpConfigManAddrTable_mib_index *tbl_idx, long lldpLocManAddrSubtype_val,
    char *lldpLocManAddr_val_ptr, size_t lldpLocManAddr_val_ptr_len) {
    DEBUGMSGTL(("verbose:lldpConfigManAddrTable:lldpConfigManAddrTable_indexes_"
                "set_tbl_idx",
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

int lldpConfigManAddrTable_indexes_set(
    lldpConfigManAddrTable_rowreq_ctx *rowreq_ctx,
    long lldpLocManAddrSubtype_val, char *lldpLocManAddr_val_ptr,
    size_t lldpLocManAddr_val_ptr_len) {
    DEBUGMSGTL(
        ("verbose:lldpConfigManAddrTable:lldpConfigManAddrTable_indexes_set",
         "called\n"));
    if (MFD_SUCCESS != lldpConfigManAddrTable_indexes_set_tbl_idx(
                           &rowreq_ctx->tbl_idx, lldpLocManAddrSubtype_val,
                           lldpLocManAddr_val_ptr,
                           lldpLocManAddr_val_ptr_len)) {
        return MFD_ERROR;
    }
    rowreq_ctx->oid_idx.len = sizeof(rowreq_ctx->oid_tmp) / sizeof(oid);
    if (0 != lldpConfigManAddrTable_index_to_oid(&rowreq_ctx->oid_idx,
                                                 &rowreq_ctx->tbl_idx)) {
        return MFD_ERROR;
    }
    return MFD_SUCCESS;
}

int lldpConfigManAddrPortsTxEnable_get(
    lldpConfigManAddrTable_rowreq_ctx *rowreq_ctx,
    char **lldpConfigManAddrPortsTxEnable_val_ptr_ptr,
    size_t *lldpConfigManAddrPortsTxEnable_val_ptr_len_ptr) {
    netsnmp_assert((NULL != lldpConfigManAddrPortsTxEnable_val_ptr_ptr) &&
                   (NULL != *lldpConfigManAddrPortsTxEnable_val_ptr_ptr));
    netsnmp_assert(NULL != lldpConfigManAddrPortsTxEnable_val_ptr_len_ptr);
    DEBUGMSGTL(
        ("verbose:lldpConfigManAddrTable:lldpConfigManAddrPortsTxEnable_get",
         "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    if ((NULL == (*lldpConfigManAddrPortsTxEnable_val_ptr_ptr)) ||
        ((*lldpConfigManAddrPortsTxEnable_val_ptr_len_ptr) <
         (rowreq_ctx->data.lldpConfigManAddrPortsTxEnable_len *
          sizeof(rowreq_ctx->data.lldpConfigManAddrPortsTxEnable[0])))) {
        (*lldpConfigManAddrPortsTxEnable_val_ptr_ptr) =
            malloc(rowreq_ctx->data.lldpConfigManAddrPortsTxEnable_len *
                   sizeof(rowreq_ctx->data.lldpConfigManAddrPortsTxEnable[0]));
        if (NULL == (*lldpConfigManAddrPortsTxEnable_val_ptr_ptr)) {
            snmp_log(LOG_ERR, "could not allocate memory "
                              "(rowreq_ctx->data."
                              "lldpConfigManAddrPortsTxEnable)\n");
            return MFD_ERROR;
        }
    }
    (*lldpConfigManAddrPortsTxEnable_val_ptr_len_ptr) =
        rowreq_ctx->data.lldpConfigManAddrPortsTxEnable_len *
        sizeof(rowreq_ctx->data.lldpConfigManAddrPortsTxEnable[0]);
    memcpy((*lldpConfigManAddrPortsTxEnable_val_ptr_ptr),
           rowreq_ctx->data.lldpConfigManAddrPortsTxEnable,
           rowreq_ctx->data.lldpConfigManAddrPortsTxEnable_len *
               sizeof(rowreq_ctx->data.lldpConfigManAddrPortsTxEnable[0]));
    return MFD_SUCCESS;
}

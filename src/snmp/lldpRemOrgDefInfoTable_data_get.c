#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "lldpRemOrgDefInfoTable.h"

int lldpRemOrgDefInfoTable_indexes_set_tbl_idx(
    lldpRemOrgDefInfoTable_mib_index *tbl_idx, long lldpRemTimeMark_val,
    long lldpRemLocalPortNum_val, long lldpRemIndex_val,
    char *lldpRemOrgDefInfoOUI_val_ptr, size_t lldpRemOrgDefInfoOUI_val_ptr_len,
    long lldpRemOrgDefInfoSubtype_val, long lldpRemOrgDefInfoIndex_val) {
    DEBUGMSGTL(("verbose:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_indexes_"
                "set_tbl_idx",
                "called\n"));

    tbl_idx->lldpRemTimeMark = lldpRemTimeMark_val;
    tbl_idx->lldpRemLocalPortNum = lldpRemLocalPortNum_val;
    tbl_idx->lldpRemIndex = lldpRemIndex_val;

    tbl_idx->lldpRemOrgDefInfoOUI_len =
        sizeof(tbl_idx->lldpRemOrgDefInfoOUI) /
        sizeof(tbl_idx->lldpRemOrgDefInfoOUI[0]);
    if ((NULL == tbl_idx->lldpRemOrgDefInfoOUI) ||
        (tbl_idx->lldpRemOrgDefInfoOUI_len <
         (lldpRemOrgDefInfoOUI_val_ptr_len))) {
        snmp_log(LOG_ERR,
                 "not enough space for value (lldpRemOrgDefInfoOUI_val_ptr)\n");
        return MFD_ERROR;
    }
    tbl_idx->lldpRemOrgDefInfoOUI_len = lldpRemOrgDefInfoOUI_val_ptr_len;
    memcpy(tbl_idx->lldpRemOrgDefInfoOUI, lldpRemOrgDefInfoOUI_val_ptr,
           lldpRemOrgDefInfoOUI_val_ptr_len *
               sizeof(lldpRemOrgDefInfoOUI_val_ptr[0]));
    tbl_idx->lldpRemOrgDefInfoSubtype = lldpRemOrgDefInfoSubtype_val;
    tbl_idx->lldpRemOrgDefInfoIndex = lldpRemOrgDefInfoIndex_val;
    return MFD_SUCCESS;
}

int lldpRemOrgDefInfoTable_indexes_set(
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx, long lldpRemTimeMark_val,
    long lldpRemLocalPortNum_val, long lldpRemIndex_val,
    char *lldpRemOrgDefInfoOUI_val_ptr, size_t lldpRemOrgDefInfoOUI_val_ptr_len,
    long lldpRemOrgDefInfoSubtype_val, long lldpRemOrgDefInfoIndex_val) {
    DEBUGMSGTL(
        ("verbose:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_indexes_set",
         "called\n"));
    if (MFD_SUCCESS !=
        lldpRemOrgDefInfoTable_indexes_set_tbl_idx(
            &rowreq_ctx->tbl_idx, lldpRemTimeMark_val, lldpRemLocalPortNum_val,
            lldpRemIndex_val, lldpRemOrgDefInfoOUI_val_ptr,
            lldpRemOrgDefInfoOUI_val_ptr_len, lldpRemOrgDefInfoSubtype_val,
            lldpRemOrgDefInfoIndex_val)) {
        return MFD_ERROR;
    }
    rowreq_ctx->oid_idx.len = sizeof(rowreq_ctx->oid_tmp) / sizeof(oid);
    if (0 != lldpRemOrgDefInfoTable_index_to_oid(&rowreq_ctx->oid_idx,
                                                 &rowreq_ctx->tbl_idx)) {
        return MFD_ERROR;
    }
    return MFD_SUCCESS;
}

int lldpRemOrgDefInfo_get(lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx,
                          char **lldpRemOrgDefInfo_val_ptr_ptr,
                          size_t *lldpRemOrgDefInfo_val_ptr_len_ptr) {
    netsnmp_assert((NULL != lldpRemOrgDefInfo_val_ptr_ptr) &&
                   (NULL != *lldpRemOrgDefInfo_val_ptr_ptr));
    netsnmp_assert(NULL != lldpRemOrgDefInfo_val_ptr_len_ptr);
    DEBUGMSGTL(
        ("verbose:lldpRemOrgDefInfoTable:lldpRemOrgDefInfo_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    if ((NULL == (*lldpRemOrgDefInfo_val_ptr_ptr)) ||
        ((*lldpRemOrgDefInfo_val_ptr_len_ptr) <
         (rowreq_ctx->data.lldpRemOrgDefInfo_len *
          sizeof(rowreq_ctx->data.lldpRemOrgDefInfo[0])))) {
        (*lldpRemOrgDefInfo_val_ptr_ptr) =
            malloc(rowreq_ctx->data.lldpRemOrgDefInfo_len *
                   sizeof(rowreq_ctx->data.lldpRemOrgDefInfo[0]));
        if (NULL == (*lldpRemOrgDefInfo_val_ptr_ptr)) {
            snmp_log(LOG_ERR, "could not allocate memory "
                              "(rowreq_ctx->data.lldpRemOrgDefInfo)\n");
            return MFD_ERROR;
        }
    }
    (*lldpRemOrgDefInfo_val_ptr_len_ptr) =
        rowreq_ctx->data.lldpRemOrgDefInfo_len *
        sizeof(rowreq_ctx->data.lldpRemOrgDefInfo[0]);
    memcpy((*lldpRemOrgDefInfo_val_ptr_ptr), rowreq_ctx->data.lldpRemOrgDefInfo,
           rowreq_ctx->data.lldpRemOrgDefInfo_len *
               sizeof(rowreq_ctx->data.lldpRemOrgDefInfo[0]));
    return MFD_SUCCESS;
}

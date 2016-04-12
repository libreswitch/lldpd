#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "lldpLocPortTable.h"

int lldpLocPortTable_indexes_set_tbl_idx(lldpLocPortTable_mib_index *tbl_idx,
                                         long lldpLocPortNum_val) {
    DEBUGMSGTL(("verbose:lldpLocPortTable:lldpLocPortTable_indexes_set_tbl_idx",
                "called\n"));

    tbl_idx->lldpLocPortNum = lldpLocPortNum_val;
    return MFD_SUCCESS;
}

int lldpLocPortTable_indexes_set(lldpLocPortTable_rowreq_ctx *rowreq_ctx,
                                 long lldpLocPortNum_val) {
    DEBUGMSGTL(
        ("verbose:lldpLocPortTable:lldpLocPortTable_indexes_set", "called\n"));
    if (MFD_SUCCESS != lldpLocPortTable_indexes_set_tbl_idx(
                           &rowreq_ctx->tbl_idx, lldpLocPortNum_val)) {
        return MFD_ERROR;
    }
    rowreq_ctx->oid_idx.len = sizeof(rowreq_ctx->oid_tmp) / sizeof(oid);
    if (0 != lldpLocPortTable_index_to_oid(&rowreq_ctx->oid_idx,
                                           &rowreq_ctx->tbl_idx)) {
        return MFD_ERROR;
    }
    return MFD_SUCCESS;
}

int lldpLocPortIdSubtype_get(lldpLocPortTable_rowreq_ctx *rowreq_ctx,
                             long *lldpLocPortIdSubtype_val_ptr) {
    netsnmp_assert(NULL != lldpLocPortIdSubtype_val_ptr);
    DEBUGMSGTL(
        ("verbose:lldpLocPortTable:lldpLocPortIdSubtype_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpLocPortIdSubtype_val_ptr) = rowreq_ctx->data.lldpLocPortIdSubtype;
    return MFD_SUCCESS;
}

int lldpLocPortId_get(lldpLocPortTable_rowreq_ctx *rowreq_ctx,
                      char **lldpLocPortId_val_ptr_ptr,
                      size_t *lldpLocPortId_val_ptr_len_ptr) {
    netsnmp_assert((NULL != lldpLocPortId_val_ptr_ptr) &&
                   (NULL != *lldpLocPortId_val_ptr_ptr));
    netsnmp_assert(NULL != lldpLocPortId_val_ptr_len_ptr);
    DEBUGMSGTL(("verbose:lldpLocPortTable:lldpLocPortId_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    if ((NULL == (*lldpLocPortId_val_ptr_ptr)) ||
        ((*lldpLocPortId_val_ptr_len_ptr) <
         (rowreq_ctx->data.lldpLocPortId_len *
          sizeof(rowreq_ctx->data.lldpLocPortId[0])))) {
        (*lldpLocPortId_val_ptr_ptr) =
            malloc(rowreq_ctx->data.lldpLocPortId_len *
                   sizeof(rowreq_ctx->data.lldpLocPortId[0]));
        if (NULL == (*lldpLocPortId_val_ptr_ptr)) {
            snmp_log(
                LOG_ERR,
                "could not allocate memory (rowreq_ctx->data.lldpLocPortId)\n");
            return MFD_ERROR;
        }
    }
    (*lldpLocPortId_val_ptr_len_ptr) =
        rowreq_ctx->data.lldpLocPortId_len *
        sizeof(rowreq_ctx->data.lldpLocPortId[0]);
    memcpy((*lldpLocPortId_val_ptr_ptr), rowreq_ctx->data.lldpLocPortId,
           rowreq_ctx->data.lldpLocPortId_len *
               sizeof(rowreq_ctx->data.lldpLocPortId[0]));
    return MFD_SUCCESS;
}

int lldpLocPortDesc_get(lldpLocPortTable_rowreq_ctx *rowreq_ctx,
                        char **lldpLocPortDesc_val_ptr_ptr,
                        size_t *lldpLocPortDesc_val_ptr_len_ptr) {
    netsnmp_assert((NULL != lldpLocPortDesc_val_ptr_ptr) &&
                   (NULL != *lldpLocPortDesc_val_ptr_ptr));
    netsnmp_assert(NULL != lldpLocPortDesc_val_ptr_len_ptr);
    DEBUGMSGTL(("verbose:lldpLocPortTable:lldpLocPortDesc_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    if ((NULL == (*lldpLocPortDesc_val_ptr_ptr)) ||
        ((*lldpLocPortDesc_val_ptr_len_ptr) <
         (rowreq_ctx->data.lldpLocPortDesc_len *
          sizeof(rowreq_ctx->data.lldpLocPortDesc[0])))) {
        (*lldpLocPortDesc_val_ptr_ptr) =
            malloc(rowreq_ctx->data.lldpLocPortDesc_len *
                   sizeof(rowreq_ctx->data.lldpLocPortDesc[0]));
        if (NULL == (*lldpLocPortDesc_val_ptr_ptr)) {
            snmp_log(LOG_ERR, "could not allocate memory "
                              "(rowreq_ctx->data.lldpLocPortDesc)\n");
            return MFD_ERROR;
        }
    }
    (*lldpLocPortDesc_val_ptr_len_ptr) =
        rowreq_ctx->data.lldpLocPortDesc_len *
        sizeof(rowreq_ctx->data.lldpLocPortDesc[0]);
    memcpy((*lldpLocPortDesc_val_ptr_ptr), rowreq_ctx->data.lldpLocPortDesc,
           rowreq_ctx->data.lldpLocPortDesc_len *
               sizeof(rowreq_ctx->data.lldpLocPortDesc[0]));
    return MFD_SUCCESS;
}

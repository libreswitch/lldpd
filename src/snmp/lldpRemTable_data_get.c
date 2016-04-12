#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "lldpRemTable.h"

int lldpRemTable_indexes_set_tbl_idx(lldpRemTable_mib_index *tbl_idx,
                                     long lldpRemTimeMark_val,
                                     long lldpRemLocalPortNum_val,
                                     long lldpRemIndex_val) {
    DEBUGMSGTL(
        ("verbose:lldpRemTable:lldpRemTable_indexes_set_tbl_idx", "called\n"));

    tbl_idx->lldpRemTimeMark = lldpRemTimeMark_val;
    tbl_idx->lldpRemLocalPortNum = lldpRemLocalPortNum_val;
    tbl_idx->lldpRemIndex = lldpRemIndex_val;
    return MFD_SUCCESS;
}

int lldpRemTable_indexes_set(lldpRemTable_rowreq_ctx *rowreq_ctx,
                             long lldpRemTimeMark_val,
                             long lldpRemLocalPortNum_val,
                             long lldpRemIndex_val) {
    DEBUGMSGTL(("verbose:lldpRemTable:lldpRemTable_indexes_set", "called\n"));
    if (MFD_SUCCESS != lldpRemTable_indexes_set_tbl_idx(
                           &rowreq_ctx->tbl_idx, lldpRemTimeMark_val,
                           lldpRemLocalPortNum_val, lldpRemIndex_val)) {
        return MFD_ERROR;
    }
    rowreq_ctx->oid_idx.len = sizeof(rowreq_ctx->oid_tmp) / sizeof(oid);
    if (0 !=
        lldpRemTable_index_to_oid(&rowreq_ctx->oid_idx, &rowreq_ctx->tbl_idx)) {
        return MFD_ERROR;
    }
    return MFD_SUCCESS;
}

int lldpRemChassisIdSubtype_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                                long *lldpRemChassisIdSubtype_val_ptr) {
    netsnmp_assert(NULL != lldpRemChassisIdSubtype_val_ptr);
    DEBUGMSGTL(
        ("verbose:lldpRemTable:lldpRemChassisIdSubtype_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpRemChassisIdSubtype_val_ptr) =
        rowreq_ctx->data.lldpRemChassisIdSubtype;
    return MFD_SUCCESS;
}

int lldpRemChassisId_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                         char **lldpRemChassisId_val_ptr_ptr,
                         size_t *lldpRemChassisId_val_ptr_len_ptr) {
    netsnmp_assert((NULL != lldpRemChassisId_val_ptr_ptr) &&
                   (NULL != *lldpRemChassisId_val_ptr_ptr));
    netsnmp_assert(NULL != lldpRemChassisId_val_ptr_len_ptr);
    DEBUGMSGTL(("verbose:lldpRemTable:lldpRemChassisId_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    if ((NULL == (*lldpRemChassisId_val_ptr_ptr)) ||
        ((*lldpRemChassisId_val_ptr_len_ptr) <
         (rowreq_ctx->data.lldpRemChassisId_len *
          sizeof(rowreq_ctx->data.lldpRemChassisId[0])))) {
        (*lldpRemChassisId_val_ptr_ptr) =
            malloc(rowreq_ctx->data.lldpRemChassisId_len *
                   sizeof(rowreq_ctx->data.lldpRemChassisId[0]));
        if (NULL == (*lldpRemChassisId_val_ptr_ptr)) {
            snmp_log(LOG_ERR, "could not allocate memory "
                              "(rowreq_ctx->data.lldpRemChassisId)\n");
            return MFD_ERROR;
        }
    }
    (*lldpRemChassisId_val_ptr_len_ptr) =
        rowreq_ctx->data.lldpRemChassisId_len *
        sizeof(rowreq_ctx->data.lldpRemChassisId[0]);
    memcpy((*lldpRemChassisId_val_ptr_ptr), rowreq_ctx->data.lldpRemChassisId,
           rowreq_ctx->data.lldpRemChassisId_len *
               sizeof(rowreq_ctx->data.lldpRemChassisId[0]));
    return MFD_SUCCESS;
}

int lldpRemPortIdSubtype_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                             long *lldpRemPortIdSubtype_val_ptr) {
    netsnmp_assert(NULL != lldpRemPortIdSubtype_val_ptr);
    DEBUGMSGTL(("verbose:lldpRemTable:lldpRemPortIdSubtype_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpRemPortIdSubtype_val_ptr) = rowreq_ctx->data.lldpRemPortIdSubtype;
    return MFD_SUCCESS;
}

int lldpRemPortId_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                      char **lldpRemPortId_val_ptr_ptr,
                      size_t *lldpRemPortId_val_ptr_len_ptr) {
    netsnmp_assert((NULL != lldpRemPortId_val_ptr_ptr) &&
                   (NULL != *lldpRemPortId_val_ptr_ptr));
    netsnmp_assert(NULL != lldpRemPortId_val_ptr_len_ptr);
    DEBUGMSGTL(("verbose:lldpRemTable:lldpRemPortId_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    if ((NULL == (*lldpRemPortId_val_ptr_ptr)) ||
        ((*lldpRemPortId_val_ptr_len_ptr) <
         (rowreq_ctx->data.lldpRemPortId_len *
          sizeof(rowreq_ctx->data.lldpRemPortId[0])))) {
        (*lldpRemPortId_val_ptr_ptr) =
            malloc(rowreq_ctx->data.lldpRemPortId_len *
                   sizeof(rowreq_ctx->data.lldpRemPortId[0]));
        if (NULL == (*lldpRemPortId_val_ptr_ptr)) {
            snmp_log(
                LOG_ERR,
                "could not allocate memory (rowreq_ctx->data.lldpRemPortId)\n");
            return MFD_ERROR;
        }
    }
    (*lldpRemPortId_val_ptr_len_ptr) =
        rowreq_ctx->data.lldpRemPortId_len *
        sizeof(rowreq_ctx->data.lldpRemPortId[0]);
    memcpy((*lldpRemPortId_val_ptr_ptr), rowreq_ctx->data.lldpRemPortId,
           rowreq_ctx->data.lldpRemPortId_len *
               sizeof(rowreq_ctx->data.lldpRemPortId[0]));
    return MFD_SUCCESS;
}

int lldpRemPortDesc_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                        char **lldpRemPortDesc_val_ptr_ptr,
                        size_t *lldpRemPortDesc_val_ptr_len_ptr) {
    netsnmp_assert((NULL != lldpRemPortDesc_val_ptr_ptr) &&
                   (NULL != *lldpRemPortDesc_val_ptr_ptr));
    netsnmp_assert(NULL != lldpRemPortDesc_val_ptr_len_ptr);
    DEBUGMSGTL(("verbose:lldpRemTable:lldpRemPortDesc_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    if ((NULL == (*lldpRemPortDesc_val_ptr_ptr)) ||
        ((*lldpRemPortDesc_val_ptr_len_ptr) <
         (rowreq_ctx->data.lldpRemPortDesc_len *
          sizeof(rowreq_ctx->data.lldpRemPortDesc[0])))) {
        (*lldpRemPortDesc_val_ptr_ptr) =
            malloc(rowreq_ctx->data.lldpRemPortDesc_len *
                   sizeof(rowreq_ctx->data.lldpRemPortDesc[0]));
        if (NULL == (*lldpRemPortDesc_val_ptr_ptr)) {
            snmp_log(LOG_ERR, "could not allocate memory "
                              "(rowreq_ctx->data.lldpRemPortDesc)\n");
            return MFD_ERROR;
        }
    }
    (*lldpRemPortDesc_val_ptr_len_ptr) =
        rowreq_ctx->data.lldpRemPortDesc_len *
        sizeof(rowreq_ctx->data.lldpRemPortDesc[0]);
    memcpy((*lldpRemPortDesc_val_ptr_ptr), rowreq_ctx->data.lldpRemPortDesc,
           rowreq_ctx->data.lldpRemPortDesc_len *
               sizeof(rowreq_ctx->data.lldpRemPortDesc[0]));
    return MFD_SUCCESS;
}

int lldpRemSysName_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                       char **lldpRemSysName_val_ptr_ptr,
                       size_t *lldpRemSysName_val_ptr_len_ptr) {
    netsnmp_assert((NULL != lldpRemSysName_val_ptr_ptr) &&
                   (NULL != *lldpRemSysName_val_ptr_ptr));
    netsnmp_assert(NULL != lldpRemSysName_val_ptr_len_ptr);
    DEBUGMSGTL(("verbose:lldpRemTable:lldpRemSysName_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    if ((NULL == (*lldpRemSysName_val_ptr_ptr)) ||
        ((*lldpRemSysName_val_ptr_len_ptr) <
         (rowreq_ctx->data.lldpRemSysName_len *
          sizeof(rowreq_ctx->data.lldpRemSysName[0])))) {
        (*lldpRemSysName_val_ptr_ptr) =
            malloc(rowreq_ctx->data.lldpRemSysName_len *
                   sizeof(rowreq_ctx->data.lldpRemSysName[0]));
        if (NULL == (*lldpRemSysName_val_ptr_ptr)) {
            snmp_log(LOG_ERR, "could not allocate memory "
                              "(rowreq_ctx->data.lldpRemSysName)\n");
            return MFD_ERROR;
        }
    }
    (*lldpRemSysName_val_ptr_len_ptr) =
        rowreq_ctx->data.lldpRemSysName_len *
        sizeof(rowreq_ctx->data.lldpRemSysName[0]);
    memcpy((*lldpRemSysName_val_ptr_ptr), rowreq_ctx->data.lldpRemSysName,
           rowreq_ctx->data.lldpRemSysName_len *
               sizeof(rowreq_ctx->data.lldpRemSysName[0]));
    return MFD_SUCCESS;
}

int lldpRemSysDesc_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                       char **lldpRemSysDesc_val_ptr_ptr,
                       size_t *lldpRemSysDesc_val_ptr_len_ptr) {
    netsnmp_assert((NULL != lldpRemSysDesc_val_ptr_ptr) &&
                   (NULL != *lldpRemSysDesc_val_ptr_ptr));
    netsnmp_assert(NULL != lldpRemSysDesc_val_ptr_len_ptr);
    DEBUGMSGTL(("verbose:lldpRemTable:lldpRemSysDesc_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    if ((NULL == (*lldpRemSysDesc_val_ptr_ptr)) ||
        ((*lldpRemSysDesc_val_ptr_len_ptr) <
         (rowreq_ctx->data.lldpRemSysDesc_len *
          sizeof(rowreq_ctx->data.lldpRemSysDesc[0])))) {
        (*lldpRemSysDesc_val_ptr_ptr) =
            malloc(rowreq_ctx->data.lldpRemSysDesc_len *
                   sizeof(rowreq_ctx->data.lldpRemSysDesc[0]));
        if (NULL == (*lldpRemSysDesc_val_ptr_ptr)) {
            snmp_log(LOG_ERR, "could not allocate memory "
                              "(rowreq_ctx->data.lldpRemSysDesc)\n");
            return MFD_ERROR;
        }
    }
    (*lldpRemSysDesc_val_ptr_len_ptr) =
        rowreq_ctx->data.lldpRemSysDesc_len *
        sizeof(rowreq_ctx->data.lldpRemSysDesc[0]);
    memcpy((*lldpRemSysDesc_val_ptr_ptr), rowreq_ctx->data.lldpRemSysDesc,
           rowreq_ctx->data.lldpRemSysDesc_len *
               sizeof(rowreq_ctx->data.lldpRemSysDesc[0]));
    return MFD_SUCCESS;
}

int lldpRemSysCapSupported_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                               u_long *lldpRemSysCapSupported_val_ptr) {
    netsnmp_assert(NULL != lldpRemSysCapSupported_val_ptr);
    DEBUGMSGTL(("verbose:lldpRemTable:lldpRemSysCapSupported_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpRemSysCapSupported_val_ptr) = rowreq_ctx->data.lldpRemSysCapSupported;
    return MFD_SUCCESS;
}

int lldpRemSysCapEnabled_get(lldpRemTable_rowreq_ctx *rowreq_ctx,
                             u_long *lldpRemSysCapEnabled_val_ptr) {
    netsnmp_assert(NULL != lldpRemSysCapEnabled_val_ptr);
    DEBUGMSGTL(("verbose:lldpRemTable:lldpRemSysCapEnabled_get", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpRemSysCapEnabled_val_ptr) = rowreq_ctx->data.lldpRemSysCapEnabled;
    return MFD_SUCCESS;
}

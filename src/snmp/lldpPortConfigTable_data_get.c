#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "lldpPortConfigTable.h"

int lldpPortConfigTable_indexes_set_tbl_idx(
    lldpPortConfigTable_mib_index *tbl_idx, long lldpPortConfigPortNum_val) {
    DEBUGMSGTL(
        ("verbose:lldpPortConfigTable:lldpPortConfigTable_indexes_set_tbl_idx",
         "called\n"));

    tbl_idx->lldpPortConfigPortNum = lldpPortConfigPortNum_val;
    return MFD_SUCCESS;
}

int lldpPortConfigTable_indexes_set(lldpPortConfigTable_rowreq_ctx *rowreq_ctx,
                                    long lldpPortConfigPortNum_val) {
    DEBUGMSGTL(("verbose:lldpPortConfigTable:lldpPortConfigTable_indexes_set",
                "called\n"));
    if (MFD_SUCCESS != lldpPortConfigTable_indexes_set_tbl_idx(
                           &rowreq_ctx->tbl_idx, lldpPortConfigPortNum_val)) {
        return MFD_ERROR;
    }
    rowreq_ctx->oid_idx.len = sizeof(rowreq_ctx->oid_tmp) / sizeof(oid);
    if (0 != lldpPortConfigTable_index_to_oid(&rowreq_ctx->oid_idx,
                                              &rowreq_ctx->tbl_idx)) {
        return MFD_ERROR;
    }
    return MFD_SUCCESS;
}

int lldpPortConfigAdminStatus_get(lldpPortConfigTable_rowreq_ctx *rowreq_ctx,
                                  long *lldpPortConfigAdminStatus_val_ptr) {
    netsnmp_assert(NULL != lldpPortConfigAdminStatus_val_ptr);
    DEBUGMSGTL(("verbose:lldpPortConfigTable:lldpPortConfigAdminStatus_get",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpPortConfigAdminStatus_val_ptr) =
        rowreq_ctx->data.lldpPortConfigAdminStatus;
    return MFD_SUCCESS;
}

int lldpPortConfigNotificationEnable_get(
    lldpPortConfigTable_rowreq_ctx *rowreq_ctx,
    long *lldpPortConfigNotificationEnable_val_ptr) {
    netsnmp_assert(NULL != lldpPortConfigNotificationEnable_val_ptr);
    DEBUGMSGTL(
        ("verbose:lldpPortConfigTable:lldpPortConfigNotificationEnable_get",
         "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpPortConfigNotificationEnable_val_ptr) =
        rowreq_ctx->data.lldpPortConfigNotificationEnable;
    return MFD_SUCCESS;
}

int lldpPortConfigTLVsTxEnable_get(lldpPortConfigTable_rowreq_ctx *rowreq_ctx,
                                   u_long *lldpPortConfigTLVsTxEnable_val_ptr) {
    netsnmp_assert(NULL != lldpPortConfigTLVsTxEnable_val_ptr);
    DEBUGMSGTL(("verbose:lldpPortConfigTable:lldpPortConfigTLVsTxEnable_get",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);

    (*lldpPortConfigTLVsTxEnable_val_ptr) =
        rowreq_ctx->data.lldpPortConfigTLVsTxEnable;
    return MFD_SUCCESS;
}

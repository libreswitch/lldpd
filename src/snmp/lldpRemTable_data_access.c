#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "LLDP_MIB_custom.h"
#include "lldpRemTable.h"
#include "lldpRemTable_data_access.h"
#include "lldpRemTable_ovsdb_get.h"

#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"

int lldpRemTable_init_data(lldpRemTable_registration *lldpRemTable_reg) {
    DEBUGMSGTL(("verbose:lldpRemTable:lldpRemTable_init_data", "called\n"));
    return MFD_SUCCESS;
}

void lldpRemTable_container_init(netsnmp_container **container_ptr_ptr,
                                 netsnmp_cache *cache) {
    DEBUGMSGTL(
        ("verbose:lldpRemTable:lldpRemTable_container_init", "called\n"));
    if (NULL == container_ptr_ptr) {
        snmp_log(LOG_ERR,
                 "bad container param to lldpRemTable_container_init\n");
        return;
    }
    *container_ptr_ptr = NULL;
    if (NULL == cache) {
        snmp_log(LOG_ERR, "bad cache param to lldpRemTable_container_init\n");
        return;
    }
    cache->timeout = LLDPREMTABLE_CACHE_TIMEOUT;
}

void lldpRemTable_container_shutdown(netsnmp_container *container_ptr) {
    DEBUGMSGTL(
        ("verbose:lldpRemTable:lldpRemTable_container_shutdown", "called\n"));
    if (NULL == container_ptr) {
        snmp_log(LOG_ERR, "bad params to lldpRemTable_container_shutdown\n");
        return;
    }
}

int lldpRemTable_container_load(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpRemTable:lldpRemTable_container_load", "called\n"));
    lldpRemTable_rowreq_ctx *rowreq_ctx;
    size_t count = 0;

    const struct ovsrec_interface *interface_row = NULL;
    const struct ovsrec_system *system_row = NULL;

    long lldpRemTimeMark;
    long lldpRemLocalPortNum;
    long lldpRemIndex;

    long lldpRemChassisIdSubtype;
    char lldpRemChassisId[255];
    size_t lldpRemChassisId_len;
    long lldpRemPortIdSubtype;
    char lldpRemPortId[255];
    size_t lldpRemPortId_len;
    char lldpRemPortDesc[255];
    size_t lldpRemPortDesc_len;
    char lldpRemSysName[255];
    size_t lldpRemSysName_len;
    char lldpRemSysDesc[255];
    size_t lldpRemSysDesc_len;
    u_long lldpRemSysCapSupported;
    u_long lldpRemSysCapEnabled;

    interface_row = ovsrec_interface_first(idl);
    if (!interface_row) {
        snmp_log(LOG_ERR, "not able to fetch interface row");
        return -1;
    }

    system_row = ovsrec_system_first(idl);
    if (!system_row) {
        snmp_log(LOG_ERR, "not able to fetch system row");
        return -1;
    }

    OVSREC_INTERFACE_FOR_EACH(interface_row, idl) {
        if (lldpPortConfigTable_skip_function(idl, interface_row)) {
            continue;
        }
        ovsdb_get_lldpRemTimeMark(idl, interface_row, &lldpRemTimeMark);
        ovsdb_get_lldpRemLocalPortNum(idl, interface_row, &lldpRemLocalPortNum);
        ovsdb_get_lldpRemIndex(idl, interface_row, &lldpRemIndex);

        ovsdb_get_lldpRemChassisIdSubtype(idl, interface_row,
                                          &lldpRemChassisIdSubtype);
        ovsdb_get_lldpRemChassisId(idl, interface_row, lldpRemChassisId,
                                   &lldpRemChassisId_len);
        ovsdb_get_lldpRemPortIdSubtype(idl, interface_row,
                                       &lldpRemPortIdSubtype);
        ovsdb_get_lldpRemPortId(idl, interface_row, lldpRemPortId,
                                &lldpRemPortId_len);
        ovsdb_get_lldpRemPortDesc(idl, interface_row, lldpRemPortDesc,
                                  &lldpRemPortDesc_len);
        ovsdb_get_lldpRemSysName(idl, interface_row, system_row, lldpRemSysName,
                                 &lldpRemSysName_len);
        ovsdb_get_lldpRemSysDesc(idl, interface_row, lldpRemSysDesc,
                                 &lldpRemSysDesc_len);
        ovsdb_get_lldpRemSysCapSupported(idl, interface_row,
                                         &lldpRemSysCapSupported);
        ovsdb_get_lldpRemSysCapEnabled(idl, interface_row, system_row,
                                       &lldpRemSysCapEnabled);

        rowreq_ctx = lldpRemTable_allocate_rowreq_ctx(NULL);
        if (rowreq_ctx == NULL) {
            snmp_log(LOG_ERR, "memory allocation failed");
            return MFD_RESOURCE_UNAVAILABLE;
        }
        if (MFD_SUCCESS != lldpRemTable_indexes_set(rowreq_ctx, lldpRemTimeMark,
                                                    lldpRemLocalPortNum,
                                                    lldpRemIndex)) {
            snmp_log(LOG_ERR, "error setting indexes while loading");
            lldpRemTable_release_rowreq_ctx(rowreq_ctx);
            continue;
        }

        rowreq_ctx->data.lldpRemChassisIdSubtype = lldpRemChassisIdSubtype;
        rowreq_ctx->data.lldpRemChassisId_len =
            lldpRemChassisId_len * sizeof(lldpRemChassisId[0]);
        memcpy(rowreq_ctx->data.lldpRemChassisId, lldpRemChassisId,
               lldpRemChassisId_len * sizeof(lldpRemChassisId[0]));
        rowreq_ctx->data.lldpRemPortIdSubtype = lldpRemPortIdSubtype;
        rowreq_ctx->data.lldpRemPortId_len =
            lldpRemPortId_len * sizeof(lldpRemPortId[0]);
        memcpy(rowreq_ctx->data.lldpRemPortId, lldpRemPortId,
               lldpRemPortId_len * sizeof(lldpRemPortId[0]));
        rowreq_ctx->data.lldpRemPortDesc_len =
            lldpRemPortDesc_len * sizeof(lldpRemPortDesc[0]);
        memcpy(rowreq_ctx->data.lldpRemPortDesc, lldpRemPortDesc,
               lldpRemPortDesc_len * sizeof(lldpRemPortDesc[0]));
        rowreq_ctx->data.lldpRemSysName_len =
            lldpRemSysName_len * sizeof(lldpRemSysName[0]);
        memcpy(rowreq_ctx->data.lldpRemSysName, lldpRemSysName,
               lldpRemSysName_len * sizeof(lldpRemSysName[0]));
        rowreq_ctx->data.lldpRemSysDesc_len =
            lldpRemSysDesc_len * sizeof(lldpRemSysDesc[0]);
        memcpy(rowreq_ctx->data.lldpRemSysDesc, lldpRemSysDesc,
               lldpRemSysDesc_len * sizeof(lldpRemSysDesc[0]));
        rowreq_ctx->data.lldpRemSysCapSupported = lldpRemSysCapSupported;
        rowreq_ctx->data.lldpRemSysCapEnabled = lldpRemSysCapEnabled;
        CONTAINER_INSERT(container, rowreq_ctx);
        ++count;
    }
    DEBUGMSGTL(("verbose:lldpRemTable:lldpRemTable_container_load",
                "inserted %d records\n", (int)count));
    return MFD_SUCCESS;
}

void lldpRemTable_container_free(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpRemTable:lldpRemTable_container_free", "called\n"));
}

int lldpRemTable_row_prep(lldpRemTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(("verbose:lldpRemTable:lldpRemTable_row_prep", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
    return MFD_SUCCESS;
}

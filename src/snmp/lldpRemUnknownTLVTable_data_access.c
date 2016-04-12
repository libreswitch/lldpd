#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "LLDP_MIB_custom.h"
#include "lldpRemUnknownTLVTable.h"
#include "lldpRemUnknownTLVTable_data_access.h"
#include "lldpRemUnknownTLVTable_ovsdb_get.h"

#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"

int lldpRemUnknownTLVTable_init_data(
    lldpRemUnknownTLVTable_registration *lldpRemUnknownTLVTable_reg) {
    DEBUGMSGTL(
        ("verbose:lldpRemUnknownTLVTable:lldpRemUnknownTLVTable_init_data",
         "called\n"));
    return MFD_SUCCESS;
}

void lldpRemUnknownTLVTable_container_init(
    netsnmp_container **container_ptr_ptr, netsnmp_cache *cache) {
    DEBUGMSGTL(
        ("verbose:lldpRemUnknownTLVTable:lldpRemUnknownTLVTable_container_init",
         "called\n"));
    if (NULL == container_ptr_ptr) {
        snmp_log(
            LOG_ERR,
            "bad container param to lldpRemUnknownTLVTable_container_init\n");
        return;
    }
    *container_ptr_ptr = NULL;
    if (NULL == cache) {
        snmp_log(LOG_ERR,
                 "bad cache param to lldpRemUnknownTLVTable_container_init\n");
        return;
    }
    cache->timeout = LLDPREMUNKNOWNTLVTABLE_CACHE_TIMEOUT;
}

void lldpRemUnknownTLVTable_container_shutdown(
    netsnmp_container *container_ptr) {
    DEBUGMSGTL(("verbose:lldpRemUnknownTLVTable:lldpRemUnknownTLVTable_"
                "container_shutdown",
                "called\n"));
    if (NULL == container_ptr) {
        snmp_log(LOG_ERR,
                 "bad params to lldpRemUnknownTLVTable_container_shutdown\n");
        return;
    }
}

int lldpRemUnknownTLVTable_container_load(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpRemUnknownTLVTable:lldpRemUnknownTLVTable_container_load",
         "called\n"));
    lldpRemUnknownTLVTable_rowreq_ctx *rowreq_ctx;
    size_t count = 0;

    const struct ovsrec_interface *interface_row = NULL;

    long lldpRemTimeMark;
    long lldpRemLocalPortNum;
    long lldpRemIndex;
    long lldpRemUnknownTLVType;

    char lldpRemUnknownTLVInfo[511];
    size_t lldpRemUnknownTLVInfo_len;

    interface_row = ovsrec_interface_first(idl);
    if (!interface_row) {
        snmp_log(LOG_ERR, "not able to fetch interface row");
        return -1;
    }

    OVSREC_INTERFACE_FOR_EACH(interface_row, idl) {
        if (lldpPortConfigTable_skip_function(idl, interface_row)) {
            continue;
        }
        ovsdb_get_lldpRemTimeMark(idl, interface_row, &lldpRemTimeMark);
        ovsdb_get_lldpRemLocalPortNum(idl, interface_row, &lldpRemLocalPortNum);
        ovsdb_get_lldpRemIndex(idl, interface_row, &lldpRemIndex);
        ovsdb_get_lldpRemUnknownTLVType(idl, interface_row,
                                        &lldpRemUnknownTLVType);

        ovsdb_get_lldpRemUnknownTLVInfo(idl, interface_row,
                                        lldpRemUnknownTLVInfo,
                                        &lldpRemUnknownTLVInfo_len);

        rowreq_ctx = lldpRemUnknownTLVTable_allocate_rowreq_ctx(NULL);
        if (rowreq_ctx == NULL) {
            snmp_log(LOG_ERR, "memory allocation failed");
            return MFD_RESOURCE_UNAVAILABLE;
        }
        if (MFD_SUCCESS != lldpRemUnknownTLVTable_indexes_set(
                               rowreq_ctx, lldpRemTimeMark, lldpRemLocalPortNum,
                               lldpRemIndex, lldpRemUnknownTLVType)) {
            snmp_log(LOG_ERR, "error setting indexes while loading");
            lldpRemUnknownTLVTable_release_rowreq_ctx(rowreq_ctx);
            continue;
        }

        rowreq_ctx->data.lldpRemUnknownTLVInfo_len =
            lldpRemUnknownTLVInfo_len * sizeof(lldpRemUnknownTLVInfo[0]);
        memcpy(rowreq_ctx->data.lldpRemUnknownTLVInfo, lldpRemUnknownTLVInfo,
               lldpRemUnknownTLVInfo_len * sizeof(lldpRemUnknownTLVInfo[0]));
        CONTAINER_INSERT(container, rowreq_ctx);
        ++count;
    }
    DEBUGMSGTL(
        ("verbose:lldpRemUnknownTLVTable:lldpRemUnknownTLVTable_container_load",
         "inserted %d records\n", (int)count));
    return MFD_SUCCESS;
}

void lldpRemUnknownTLVTable_container_free(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpRemUnknownTLVTable:lldpRemUnknownTLVTable_container_free",
         "called\n"));
}

int lldpRemUnknownTLVTable_row_prep(
    lldpRemUnknownTLVTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(
        ("verbose:lldpRemUnknownTLVTable:lldpRemUnknownTLVTable_row_prep",
         "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
    return MFD_SUCCESS;
}

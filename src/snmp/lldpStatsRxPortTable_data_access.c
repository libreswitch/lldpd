#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "LLDP_MIB_custom.h"
#include "lldpStatsRxPortTable.h"
#include "lldpStatsRxPortTable_data_access.h"
#include "lldpStatsRxPortTable_ovsdb_get.h"

#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"

int lldpStatsRxPortTable_init_data(
    lldpStatsRxPortTable_registration *lldpStatsRxPortTable_reg) {
    DEBUGMSGTL(("verbose:lldpStatsRxPortTable:lldpStatsRxPortTable_init_data",
                "called\n"));
    return MFD_SUCCESS;
}

void lldpStatsRxPortTable_container_init(netsnmp_container **container_ptr_ptr,
                                         netsnmp_cache *cache) {
    DEBUGMSGTL(
        ("verbose:lldpStatsRxPortTable:lldpStatsRxPortTable_container_init",
         "called\n"));
    if (NULL == container_ptr_ptr) {
        snmp_log(
            LOG_ERR,
            "bad container param to lldpStatsRxPortTable_container_init\n");
        return;
    }
    *container_ptr_ptr = NULL;
    if (NULL == cache) {
        snmp_log(LOG_ERR,
                 "bad cache param to lldpStatsRxPortTable_container_init\n");
        return;
    }
    cache->timeout = LLDPSTATSRXPORTTABLE_CACHE_TIMEOUT;
}

void lldpStatsRxPortTable_container_shutdown(netsnmp_container *container_ptr) {
    DEBUGMSGTL(
        ("verbose:lldpStatsRxPortTable:lldpStatsRxPortTable_container_shutdown",
         "called\n"));
    if (NULL == container_ptr) {
        snmp_log(LOG_ERR,
                 "bad params to lldpStatsRxPortTable_container_shutdown\n");
        return;
    }
}

int lldpStatsRxPortTable_container_load(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpStatsRxPortTable:lldpStatsRxPortTable_container_load",
         "called\n"));
    lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx;
    size_t count = 0;

    const struct ovsrec_interface *interface_row = NULL;

    long lldpStatsRxPortNum;

    long lldpStatsRxPortFramesDiscardedTotal;
    long lldpStatsRxPortFramesErrors;
    long lldpStatsRxPortFramesTotal;
    long lldpStatsRxPortTLVsDiscardedTotal;
    long lldpStatsRxPortTLVsUnrecognizedTotal;
    long lldpStatsRxPortAgeoutsTotal;

    interface_row = ovsrec_interface_first(idl);
    if (!interface_row) {
        snmp_log(LOG_ERR, "not able to fetch interface row");
        return -1;
    }

    OVSREC_INTERFACE_FOR_EACH(interface_row, idl) {
        if (lldpPortConfigTable_skip_function(idl, interface_row)) {
            continue;
        }
        ovsdb_get_lldpStatsRxPortNum(idl, interface_row, &lldpStatsRxPortNum);

        ovsdb_get_lldpStatsRxPortFramesDiscardedTotal(
            idl, interface_row, &lldpStatsRxPortFramesDiscardedTotal);
        ovsdb_get_lldpStatsRxPortFramesErrors(idl, interface_row,
                                              &lldpStatsRxPortFramesErrors);
        ovsdb_get_lldpStatsRxPortFramesTotal(idl, interface_row,
                                             &lldpStatsRxPortFramesTotal);
        ovsdb_get_lldpStatsRxPortTLVsDiscardedTotal(
            idl, interface_row, &lldpStatsRxPortTLVsDiscardedTotal);
        ovsdb_get_lldpStatsRxPortTLVsUnrecognizedTotal(
            idl, interface_row, &lldpStatsRxPortTLVsUnrecognizedTotal);
        ovsdb_get_lldpStatsRxPortAgeoutsTotal(idl, interface_row,
                                              &lldpStatsRxPortAgeoutsTotal);

        rowreq_ctx = lldpStatsRxPortTable_allocate_rowreq_ctx(NULL);
        if (rowreq_ctx == NULL) {
            snmp_log(LOG_ERR, "memory allocation failed");
            return MFD_RESOURCE_UNAVAILABLE;
        }
        if (MFD_SUCCESS !=
            lldpStatsRxPortTable_indexes_set(rowreq_ctx, lldpStatsRxPortNum)) {
            snmp_log(LOG_ERR, "error setting indexes while loading");
            lldpStatsRxPortTable_release_rowreq_ctx(rowreq_ctx);
            continue;
        }

        rowreq_ctx->data.lldpStatsRxPortFramesDiscardedTotal =
            lldpStatsRxPortFramesDiscardedTotal;
        rowreq_ctx->data.lldpStatsRxPortFramesErrors =
            lldpStatsRxPortFramesErrors;
        rowreq_ctx->data.lldpStatsRxPortFramesTotal =
            lldpStatsRxPortFramesTotal;
        rowreq_ctx->data.lldpStatsRxPortTLVsDiscardedTotal =
            lldpStatsRxPortTLVsDiscardedTotal;
        rowreq_ctx->data.lldpStatsRxPortTLVsUnrecognizedTotal =
            lldpStatsRxPortTLVsUnrecognizedTotal;
        rowreq_ctx->data.lldpStatsRxPortAgeoutsTotal =
            lldpStatsRxPortAgeoutsTotal;
        CONTAINER_INSERT(container, rowreq_ctx);
        ++count;
    }
    DEBUGMSGTL(
        ("verbose:lldpStatsRxPortTable:lldpStatsRxPortTable_container_load",
         "inserted %d records\n", (int)count));
    return MFD_SUCCESS;
}

void lldpStatsRxPortTable_container_free(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpStatsRxPortTable:lldpStatsRxPortTable_container_free",
         "called\n"));
}

int lldpStatsRxPortTable_row_prep(lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(("verbose:lldpStatsRxPortTable:lldpStatsRxPortTable_row_prep",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
    return MFD_SUCCESS;
}

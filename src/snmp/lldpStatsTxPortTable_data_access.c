#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "LLDP_MIB_custom.h"
#include "lldpStatsTxPortTable.h"
#include "lldpStatsTxPortTable_data_access.h"
#include "lldpStatsTxPortTable_ovsdb_get.h"

#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"

int lldpStatsTxPortTable_init_data(
    lldpStatsTxPortTable_registration *lldpStatsTxPortTable_reg) {
    DEBUGMSGTL(("verbose:lldpStatsTxPortTable:lldpStatsTxPortTable_init_data",
                "called\n"));
    return MFD_SUCCESS;
}

void lldpStatsTxPortTable_container_init(netsnmp_container **container_ptr_ptr,
                                         netsnmp_cache *cache) {
    DEBUGMSGTL(
        ("verbose:lldpStatsTxPortTable:lldpStatsTxPortTable_container_init",
         "called\n"));
    if (NULL == container_ptr_ptr) {
        snmp_log(
            LOG_ERR,
            "bad container param to lldpStatsTxPortTable_container_init\n");
        return;
    }
    *container_ptr_ptr = NULL;
    if (NULL == cache) {
        snmp_log(LOG_ERR,
                 "bad cache param to lldpStatsTxPortTable_container_init\n");
        return;
    }
    cache->timeout = LLDPSTATSTXPORTTABLE_CACHE_TIMEOUT;
}

void lldpStatsTxPortTable_container_shutdown(netsnmp_container *container_ptr) {
    DEBUGMSGTL(
        ("verbose:lldpStatsTxPortTable:lldpStatsTxPortTable_container_shutdown",
         "called\n"));
    if (NULL == container_ptr) {
        snmp_log(LOG_ERR,
                 "bad params to lldpStatsTxPortTable_container_shutdown\n");
        return;
    }
}

int lldpStatsTxPortTable_container_load(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpStatsTxPortTable:lldpStatsTxPortTable_container_load",
         "called\n"));
    lldpStatsTxPortTable_rowreq_ctx *rowreq_ctx;
    size_t count = 0;

    const struct ovsrec_interface *interface_row = NULL;

    long lldpStatsTxPortNum;

    long lldpStatsTxPortFramesTotal;

    interface_row = ovsrec_interface_first(idl);
    if (!interface_row) {
        snmp_log(LOG_ERR, "not able to fetch interface row");
        return -1;
    }

    OVSREC_INTERFACE_FOR_EACH(interface_row, idl) {
        if (lldpPortConfigTable_skip_function(idl, interface_row)) {
            continue;
        }
        ovsdb_get_lldpStatsTxPortNum(idl, interface_row, &lldpStatsTxPortNum);

        ovsdb_get_lldpStatsTxPortFramesTotal(idl, interface_row,
                                             &lldpStatsTxPortFramesTotal);

        rowreq_ctx = lldpStatsTxPortTable_allocate_rowreq_ctx(NULL);
        if (rowreq_ctx == NULL) {
            snmp_log(LOG_ERR, "memory allocation failed");
            return MFD_RESOURCE_UNAVAILABLE;
        }
        if (MFD_SUCCESS !=
            lldpStatsTxPortTable_indexes_set(rowreq_ctx, lldpStatsTxPortNum)) {
            snmp_log(LOG_ERR, "error setting indexes while loading");
            lldpStatsTxPortTable_release_rowreq_ctx(rowreq_ctx);
            continue;
        }

        rowreq_ctx->data.lldpStatsTxPortFramesTotal =
            lldpStatsTxPortFramesTotal;
        CONTAINER_INSERT(container, rowreq_ctx);
        ++count;
    }
    DEBUGMSGTL(
        ("verbose:lldpStatsTxPortTable:lldpStatsTxPortTable_container_load",
         "inserted %d records\n", (int)count));
    return MFD_SUCCESS;
}

void lldpStatsTxPortTable_container_free(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpStatsTxPortTable:lldpStatsTxPortTable_container_free",
         "called\n"));
}

int lldpStatsTxPortTable_row_prep(lldpStatsTxPortTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(("verbose:lldpStatsTxPortTable:lldpStatsTxPortTable_row_prep",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
    return MFD_SUCCESS;
}

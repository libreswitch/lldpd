#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "LLDP_MIB_custom.h"
#include "lldpConfigManAddrTable.h"
#include "lldpConfigManAddrTable_data_access.h"
#include "lldpConfigManAddrTable_ovsdb_get.h"

#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"

int lldpConfigManAddrTable_init_data(
    lldpConfigManAddrTable_registration *lldpConfigManAddrTable_reg) {
    DEBUGMSGTL(
        ("verbose:lldpConfigManAddrTable:lldpConfigManAddrTable_init_data",
         "called\n"));
    return MFD_SUCCESS;
}

void lldpConfigManAddrTable_container_init(
    netsnmp_container **container_ptr_ptr, netsnmp_cache *cache) {
    DEBUGMSGTL(
        ("verbose:lldpConfigManAddrTable:lldpConfigManAddrTable_container_init",
         "called\n"));
    if (NULL == container_ptr_ptr) {
        snmp_log(
            LOG_ERR,
            "bad container param to lldpConfigManAddrTable_container_init\n");
        return;
    }
    *container_ptr_ptr = NULL;
    if (NULL == cache) {
        snmp_log(LOG_ERR,
                 "bad cache param to lldpConfigManAddrTable_container_init\n");
        return;
    }
    cache->timeout = LLDPCONFIGMANADDRTABLE_CACHE_TIMEOUT;
}

void lldpConfigManAddrTable_container_shutdown(
    netsnmp_container *container_ptr) {
    DEBUGMSGTL(("verbose:lldpConfigManAddrTable:lldpConfigManAddrTable_"
                "container_shutdown",
                "called\n"));
    if (NULL == container_ptr) {
        snmp_log(LOG_ERR,
                 "bad params to lldpConfigManAddrTable_container_shutdown\n");
        return;
    }
}

int lldpConfigManAddrTable_container_load(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpConfigManAddrTable:lldpConfigManAddrTable_container_load",
         "called\n"));
    lldpConfigManAddrTable_rowreq_ctx *rowreq_ctx;
    size_t count = 0;

    const struct ovsrec_system *system_row = NULL;

    long lldpLocManAddrSubtype;
    char lldpLocManAddr[31];
    size_t lldpLocManAddr_len;

    char lldpConfigManAddrPortsTxEnable[512];
    size_t lldpConfigManAddrPortsTxEnable_len;

    system_row = ovsrec_system_first(idl);
    if (!system_row) {
        snmp_log(LOG_ERR, "not able to fetch system row");
        return -1;
    }

    OVSREC_SYSTEM_FOR_EACH(system_row, idl) {
        ovsdb_get_lldpLocManAddrSubtype(idl, system_row,
                                        &lldpLocManAddrSubtype);
        ovsdb_get_lldpLocManAddr(idl, system_row, lldpLocManAddr,
                                 &lldpLocManAddr_len);

        ovsdb_get_lldpConfigManAddrPortsTxEnable(
            idl, system_row, lldpConfigManAddrPortsTxEnable,
            &lldpConfigManAddrPortsTxEnable_len);

        rowreq_ctx = lldpConfigManAddrTable_allocate_rowreq_ctx(NULL);
        if (rowreq_ctx == NULL) {
            snmp_log(LOG_ERR, "memory allocation failed");
            return MFD_RESOURCE_UNAVAILABLE;
        }
        if (MFD_SUCCESS != lldpConfigManAddrTable_indexes_set(
                               rowreq_ctx, lldpLocManAddrSubtype,
                               lldpLocManAddr, lldpLocManAddr_len)) {
            snmp_log(LOG_ERR, "error setting indexes while loading");
            lldpConfigManAddrTable_release_rowreq_ctx(rowreq_ctx);
            continue;
        }

        rowreq_ctx->data.lldpConfigManAddrPortsTxEnable_len =
            lldpConfigManAddrPortsTxEnable_len *
            sizeof(lldpConfigManAddrPortsTxEnable[0]);
        memcpy(rowreq_ctx->data.lldpConfigManAddrPortsTxEnable,
               lldpConfigManAddrPortsTxEnable,
               lldpConfigManAddrPortsTxEnable_len *
                   sizeof(lldpConfigManAddrPortsTxEnable[0]));
        CONTAINER_INSERT(container, rowreq_ctx);
        ++count;
    }
    DEBUGMSGTL(
        ("verbose:lldpConfigManAddrTable:lldpConfigManAddrTable_container_load",
         "inserted %d records\n", (int)count));
    return MFD_SUCCESS;
}

void lldpConfigManAddrTable_container_free(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpConfigManAddrTable:lldpConfigManAddrTable_container_free",
         "called\n"));
}

int lldpConfigManAddrTable_row_prep(
    lldpConfigManAddrTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(
        ("verbose:lldpConfigManAddrTable:lldpConfigManAddrTable_row_prep",
         "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
    return MFD_SUCCESS;
}

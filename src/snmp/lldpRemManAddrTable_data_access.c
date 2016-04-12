#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "LLDP_MIB_custom.h"
#include "lldpRemManAddrTable.h"
#include "lldpRemManAddrTable_data_access.h"
#include "lldpRemManAddrTable_ovsdb_get.h"

#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"

int lldpRemManAddrTable_init_data(
    lldpRemManAddrTable_registration *lldpRemManAddrTable_reg) {
    DEBUGMSGTL(("verbose:lldpRemManAddrTable:lldpRemManAddrTable_init_data",
                "called\n"));
    return MFD_SUCCESS;
}

void lldpRemManAddrTable_container_init(netsnmp_container **container_ptr_ptr,
                                        netsnmp_cache *cache) {
    DEBUGMSGTL(
        ("verbose:lldpRemManAddrTable:lldpRemManAddrTable_container_init",
         "called\n"));
    if (NULL == container_ptr_ptr) {
        snmp_log(LOG_ERR,
                 "bad container param to lldpRemManAddrTable_container_init\n");
        return;
    }
    *container_ptr_ptr = NULL;
    if (NULL == cache) {
        snmp_log(LOG_ERR,
                 "bad cache param to lldpRemManAddrTable_container_init\n");
        return;
    }
    cache->timeout = LLDPREMMANADDRTABLE_CACHE_TIMEOUT;
}

void lldpRemManAddrTable_container_shutdown(netsnmp_container *container_ptr) {
    DEBUGMSGTL(
        ("verbose:lldpRemManAddrTable:lldpRemManAddrTable_container_shutdown",
         "called\n"));
    if (NULL == container_ptr) {
        snmp_log(LOG_ERR,
                 "bad params to lldpRemManAddrTable_container_shutdown\n");
        return;
    }
}

int lldpRemManAddrTable_container_load(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpRemManAddrTable:lldpRemManAddrTable_container_load",
         "called\n"));
    lldpRemManAddrTable_rowreq_ctx *rowreq_ctx;
    size_t count = 0;

    const struct ovsrec_interface *interface_row = NULL;
    const struct ovsrec_system *system_row = NULL;

    long lldpRemTimeMark;
    long lldpRemLocalPortNum;
    long lldpRemIndex;
    long lldpRemManAddrSubtype;
    char lldpRemManAddr[31];
    size_t lldpRemManAddr_len;

    long lldpRemManAddrIfSubtype;
    long lldpRemManAddrIfId;
    oid lldpRemManAddrOID[MAX_OID_LEN];
    size_t lldpRemManAddrOID_len;

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
        ovsdb_get_lldpRemManAddrSubtype(idl, interface_row,
                                        &lldpRemManAddrSubtype);
        ovsdb_get_lldpRemManAddr(idl, interface_row, system_row, lldpRemManAddr,
                                 &lldpRemManAddr_len);

        ovsdb_get_lldpRemManAddrIfSubtype(idl, interface_row,
                                          &lldpRemManAddrIfSubtype);
        ovsdb_get_lldpRemManAddrIfId(idl, interface_row, &lldpRemManAddrIfId);
        ovsdb_get_lldpRemManAddrOID(idl, interface_row, lldpRemManAddrOID,
                                    &lldpRemManAddrOID_len);

        rowreq_ctx = lldpRemManAddrTable_allocate_rowreq_ctx(NULL);
        if (rowreq_ctx == NULL) {
            snmp_log(LOG_ERR, "memory allocation failed");
            return MFD_RESOURCE_UNAVAILABLE;
        }
        if (MFD_SUCCESS != lldpRemManAddrTable_indexes_set(
                               rowreq_ctx, lldpRemTimeMark, lldpRemLocalPortNum,
                               lldpRemIndex, lldpRemManAddrSubtype,
                               lldpRemManAddr, lldpRemManAddr_len)) {
            snmp_log(LOG_ERR, "error setting indexes while loading");
            lldpRemManAddrTable_release_rowreq_ctx(rowreq_ctx);
            continue;
        }

        rowreq_ctx->data.lldpRemManAddrIfSubtype = lldpRemManAddrIfSubtype;
        rowreq_ctx->data.lldpRemManAddrIfId = lldpRemManAddrIfId;
        rowreq_ctx->data.lldpRemManAddrOID_len =
            lldpRemManAddrOID_len * sizeof(lldpRemManAddrOID[0]);
        memcpy(rowreq_ctx->data.lldpRemManAddrOID, lldpRemManAddrOID,
               lldpRemManAddrOID_len * sizeof(lldpRemManAddrOID[0]));
        CONTAINER_INSERT(container, rowreq_ctx);
        ++count;
    }
    DEBUGMSGTL(
        ("verbose:lldpRemManAddrTable:lldpRemManAddrTable_container_load",
         "inserted %d records\n", (int)count));
    return MFD_SUCCESS;
}

void lldpRemManAddrTable_container_free(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpRemManAddrTable:lldpRemManAddrTable_container_free",
         "called\n"));
}

int lldpRemManAddrTable_row_prep(lldpRemManAddrTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(("verbose:lldpRemManAddrTable:lldpRemManAddrTable_row_prep",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
    return MFD_SUCCESS;
}

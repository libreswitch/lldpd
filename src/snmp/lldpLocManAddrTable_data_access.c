#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "LLDP_MIB_custom.h"
#include "lldpLocManAddrTable.h"
#include "lldpLocManAddrTable_data_access.h"
#include "lldpLocManAddrTable_ovsdb_get.h"

#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"

int lldpLocManAddrTable_init_data(
    lldpLocManAddrTable_registration *lldpLocManAddrTable_reg) {
    DEBUGMSGTL(("verbose:lldpLocManAddrTable:lldpLocManAddrTable_init_data",
                "called\n"));
    return MFD_SUCCESS;
}

void lldpLocManAddrTable_container_init(netsnmp_container **container_ptr_ptr,
                                        netsnmp_cache *cache) {
    DEBUGMSGTL(
        ("verbose:lldpLocManAddrTable:lldpLocManAddrTable_container_init",
         "called\n"));
    if (NULL == container_ptr_ptr) {
        snmp_log(LOG_ERR,
                 "bad container param to lldpLocManAddrTable_container_init\n");
        return;
    }
    *container_ptr_ptr = NULL;
    if (NULL == cache) {
        snmp_log(LOG_ERR,
                 "bad cache param to lldpLocManAddrTable_container_init\n");
        return;
    }
    cache->timeout = LLDPLOCMANADDRTABLE_CACHE_TIMEOUT;
}

void lldpLocManAddrTable_container_shutdown(netsnmp_container *container_ptr) {
    DEBUGMSGTL(
        ("verbose:lldpLocManAddrTable:lldpLocManAddrTable_container_shutdown",
         "called\n"));
    if (NULL == container_ptr) {
        snmp_log(LOG_ERR,
                 "bad params to lldpLocManAddrTable_container_shutdown\n");
        return;
    }
}

int lldpLocManAddrTable_container_load(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpLocManAddrTable:lldpLocManAddrTable_container_load",
         "called\n"));
    lldpLocManAddrTable_rowreq_ctx *rowreq_ctx;
    size_t count = 0;

    const struct ovsrec_system *system_row = NULL;

    long lldpLocManAddrSubtype;
    char lldpLocManAddr[31];
    size_t lldpLocManAddr_len;

    long lldpLocManAddrLen;
    long lldpLocManAddrIfSubtype;
    long lldpLocManAddrIfId;
    oid lldpLocManAddrOID[MAX_OID_LEN];
    size_t lldpLocManAddrOID_len;

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

        ovsdb_get_lldpLocManAddrLen(idl, system_row, &lldpLocManAddrLen);
        ovsdb_get_lldpLocManAddrIfSubtype(idl, system_row,
                                          &lldpLocManAddrIfSubtype);
        ovsdb_get_lldpLocManAddrIfId(idl, system_row, &lldpLocManAddrIfId);
        ovsdb_get_lldpLocManAddrOID(idl, system_row, lldpLocManAddrOID,
                                    &lldpLocManAddrOID_len);

        rowreq_ctx = lldpLocManAddrTable_allocate_rowreq_ctx(NULL);
        if (rowreq_ctx == NULL) {
            snmp_log(LOG_ERR, "memory allocation failed");
            return MFD_RESOURCE_UNAVAILABLE;
        }
        if (MFD_SUCCESS != lldpLocManAddrTable_indexes_set(
                               rowreq_ctx, lldpLocManAddrSubtype,
                               lldpLocManAddr, lldpLocManAddr_len)) {
            snmp_log(LOG_ERR, "error setting indexes while loading");
            lldpLocManAddrTable_release_rowreq_ctx(rowreq_ctx);
            continue;
        }

        rowreq_ctx->data.lldpLocManAddrLen = lldpLocManAddrLen;
        rowreq_ctx->data.lldpLocManAddrIfSubtype = lldpLocManAddrIfSubtype;
        rowreq_ctx->data.lldpLocManAddrIfId = lldpLocManAddrIfId;
        rowreq_ctx->data.lldpLocManAddrOID_len =
            lldpLocManAddrOID_len * sizeof(lldpLocManAddrOID[0]);
        memcpy(rowreq_ctx->data.lldpLocManAddrOID, lldpLocManAddrOID,
               lldpLocManAddrOID_len * sizeof(lldpLocManAddrOID[0]));
        CONTAINER_INSERT(container, rowreq_ctx);
        ++count;
    }
    DEBUGMSGTL(
        ("verbose:lldpLocManAddrTable:lldpLocManAddrTable_container_load",
         "inserted %d records\n", (int)count));
    return MFD_SUCCESS;
}

void lldpLocManAddrTable_container_free(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpLocManAddrTable:lldpLocManAddrTable_container_free",
         "called\n"));
}

int lldpLocManAddrTable_row_prep(lldpLocManAddrTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(("verbose:lldpLocManAddrTable:lldpLocManAddrTable_row_prep",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
    return MFD_SUCCESS;
}

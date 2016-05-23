#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "LLDP_MIB_custom.h"
#include "lldpRemOrgDefInfoTable.h"
#include "lldpRemOrgDefInfoTable_data_access.h"
#include "lldpRemOrgDefInfoTable_ovsdb_get.h"

#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"

int lldpRemOrgDefInfoTable_init_data(
    lldpRemOrgDefInfoTable_registration *lldpRemOrgDefInfoTable_reg) {
    DEBUGMSGTL(
        ("verbose:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_init_data",
         "called\n"));
    return MFD_SUCCESS;
}

void lldpRemOrgDefInfoTable_container_init(
    netsnmp_container **container_ptr_ptr, netsnmp_cache *cache) {
    DEBUGMSGTL(
        ("verbose:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_container_init",
         "called\n"));
    if (NULL == container_ptr_ptr) {
        snmp_log(
            LOG_ERR,
            "bad container param to lldpRemOrgDefInfoTable_container_init\n");
        return;
    }
    *container_ptr_ptr = NULL;
    if (NULL == cache) {
        snmp_log(LOG_ERR,
                 "bad cache param to lldpRemOrgDefInfoTable_container_init\n");
        return;
    }
    cache->timeout = LLDPREMORGDEFINFOTABLE_CACHE_TIMEOUT;
}

void lldpRemOrgDefInfoTable_container_shutdown(
    netsnmp_container *container_ptr) {
    DEBUGMSGTL(("verbose:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_"
                "container_shutdown",
                "called\n"));
    if (NULL == container_ptr) {
        snmp_log(LOG_ERR,
                 "bad params to lldpRemOrgDefInfoTable_container_shutdown\n");
        return;
    }
}

int lldpRemOrgDefInfoTable_container_load(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_container_load",
         "called\n"));
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx;
    size_t count = 0;

    const struct ovsrec_interface *interface_row = NULL;

    long lldpRemTimeMark;
    long lldpRemLocalPortNum;
    long lldpRemIndex;
    char lldpRemOrgDefInfoOUI[3];
    size_t lldpRemOrgDefInfoOUI_len;
    long lldpRemOrgDefInfoSubtype;
    long lldpRemOrgDefInfoIndex;

    char lldpRemOrgDefInfo[507];
    size_t lldpRemOrgDefInfo_len;

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
        ovsdb_get_lldpRemOrgDefInfoOUI(idl, interface_row, lldpRemOrgDefInfoOUI,
                                       &lldpRemOrgDefInfoOUI_len);
        ovsdb_get_lldpRemOrgDefInfoSubtype(idl, interface_row,
                                           &lldpRemOrgDefInfoSubtype);
        ovsdb_get_lldpRemOrgDefInfoIndex(idl, interface_row,
                                         &lldpRemOrgDefInfoIndex);

        ovsdb_get_lldpRemOrgDefInfo(idl, interface_row, lldpRemOrgDefInfo,
                                    &lldpRemOrgDefInfo_len);

        rowreq_ctx = lldpRemOrgDefInfoTable_allocate_rowreq_ctx(NULL);
        if (rowreq_ctx == NULL) {
            snmp_log(LOG_ERR, "memory allocation failed");
            return MFD_RESOURCE_UNAVAILABLE;
        }
        if (MFD_SUCCESS !=
            lldpRemOrgDefInfoTable_indexes_set(
                rowreq_ctx, lldpRemTimeMark, lldpRemLocalPortNum, lldpRemIndex,
                lldpRemOrgDefInfoOUI, lldpRemOrgDefInfoOUI_len,
                lldpRemOrgDefInfoSubtype, lldpRemOrgDefInfoIndex)) {
            snmp_log(LOG_ERR, "error setting indexes while loading");
            lldpRemOrgDefInfoTable_release_rowreq_ctx(rowreq_ctx);
            continue;
        }

        rowreq_ctx->data.lldpRemOrgDefInfo_len =
            lldpRemOrgDefInfo_len * sizeof(lldpRemOrgDefInfo[0]);
        memcpy(rowreq_ctx->data.lldpRemOrgDefInfo, lldpRemOrgDefInfo,
               lldpRemOrgDefInfo_len * sizeof(lldpRemOrgDefInfo[0]));
        CONTAINER_INSERT(container, rowreq_ctx);
        ++count;
    }
    DEBUGMSGTL(
        ("verbose:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_container_load",
         "inserted %d records\n", (int)count));
    return MFD_SUCCESS;
}

void lldpRemOrgDefInfoTable_container_free(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_container_free",
         "called\n"));
}

int lldpRemOrgDefInfoTable_row_prep(
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(
        ("verbose:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_row_prep",
         "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
    return MFD_SUCCESS;
}

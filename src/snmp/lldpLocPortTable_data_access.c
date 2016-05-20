#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "LLDP_MIB_custom.h"
#include "lldpLocPortTable.h"
#include "lldpLocPortTable_data_access.h"
#include "lldpLocPortTable_ovsdb_get.h"

#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"

int lldpLocPortTable_init_data(
    lldpLocPortTable_registration *lldpLocPortTable_reg) {
    DEBUGMSGTL(
        ("verbose:lldpLocPortTable:lldpLocPortTable_init_data", "called\n"));
    return MFD_SUCCESS;
}

void lldpLocPortTable_container_init(netsnmp_container **container_ptr_ptr,
                                     netsnmp_cache *cache) {
    DEBUGMSGTL(("verbose:lldpLocPortTable:lldpLocPortTable_container_init",
                "called\n"));
    if (NULL == container_ptr_ptr) {
        snmp_log(LOG_ERR,
                 "bad container param to lldpLocPortTable_container_init\n");
        return;
    }
    *container_ptr_ptr = NULL;
    if (NULL == cache) {
        snmp_log(LOG_ERR,
                 "bad cache param to lldpLocPortTable_container_init\n");
        return;
    }
    cache->timeout = LLDPLOCPORTTABLE_CACHE_TIMEOUT;
}

void lldpLocPortTable_container_shutdown(netsnmp_container *container_ptr) {
    DEBUGMSGTL(("verbose:lldpLocPortTable:lldpLocPortTable_container_shutdown",
                "called\n"));
    if (NULL == container_ptr) {
        snmp_log(LOG_ERR,
                 "bad params to lldpLocPortTable_container_shutdown\n");
        return;
    }
}

int lldpLocPortTable_container_load(netsnmp_container *container) {
    DEBUGMSGTL(("verbose:lldpLocPortTable:lldpLocPortTable_container_load",
                "called\n"));
    lldpLocPortTable_rowreq_ctx *rowreq_ctx;
    size_t count = 0;

    const struct ovsrec_interface *interface_row = NULL;

    long lldpLocPortNum;

    long lldpLocPortIdSubtype;
    char lldpLocPortId[255];
    size_t lldpLocPortId_len;
    char lldpLocPortDesc[255];
    size_t lldpLocPortDesc_len;

    interface_row = ovsrec_interface_first(idl);
    if (!interface_row) {
        snmp_log(LOG_ERR, "not able to fetch interface row");
        return -1;
    }

    OVSREC_INTERFACE_FOR_EACH(interface_row, idl) {
        if (lldpPortConfigTable_skip_function(idl, interface_row)) {
            continue;
        }
        ovsdb_get_lldpLocPortNum(idl, interface_row, &lldpLocPortNum);

        ovsdb_get_lldpLocPortIdSubtype(idl, interface_row,
                                       &lldpLocPortIdSubtype);
        ovsdb_get_lldpLocPortId(idl, interface_row, lldpLocPortId,
                                &lldpLocPortId_len);
        ovsdb_get_lldpLocPortDesc(idl, interface_row, lldpLocPortDesc,
                                  &lldpLocPortDesc_len);

        rowreq_ctx = lldpLocPortTable_allocate_rowreq_ctx(NULL);
        if (rowreq_ctx == NULL) {
            snmp_log(LOG_ERR, "memory allocation failed");
            return MFD_RESOURCE_UNAVAILABLE;
        }
        if (MFD_SUCCESS !=
            lldpLocPortTable_indexes_set(rowreq_ctx, lldpLocPortNum)) {
            snmp_log(LOG_ERR, "error setting indexes while loading");
            lldpLocPortTable_release_rowreq_ctx(rowreq_ctx);
            continue;
        }

        rowreq_ctx->data.lldpLocPortIdSubtype = lldpLocPortIdSubtype;
        rowreq_ctx->data.lldpLocPortId_len =
            lldpLocPortId_len * sizeof(lldpLocPortId[0]);
        memcpy(rowreq_ctx->data.lldpLocPortId, lldpLocPortId,
               lldpLocPortId_len * sizeof(lldpLocPortId[0]));
        rowreq_ctx->data.lldpLocPortDesc_len =
            lldpLocPortDesc_len * sizeof(lldpLocPortDesc[0]);
        memcpy(rowreq_ctx->data.lldpLocPortDesc, lldpLocPortDesc,
               lldpLocPortDesc_len * sizeof(lldpLocPortDesc[0]));
        CONTAINER_INSERT(container, rowreq_ctx);
        ++count;
    }
    DEBUGMSGTL(("verbose:lldpLocPortTable:lldpLocPortTable_container_load",
                "inserted %d records\n", (int)count));
    return MFD_SUCCESS;
}

void lldpLocPortTable_container_free(netsnmp_container *container) {
    DEBUGMSGTL(("verbose:lldpLocPortTable:lldpLocPortTable_container_free",
                "called\n"));
}

int lldpLocPortTable_row_prep(lldpLocPortTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(
        ("verbose:lldpLocPortTable:lldpLocPortTable_row_prep", "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
    return MFD_SUCCESS;
}

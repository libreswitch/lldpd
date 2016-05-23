#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "LLDP_MIB_custom.h"
#include "lldpPortConfigTable.h"
#include "lldpPortConfigTable_data_access.h"
#include "lldpPortConfigTable_ovsdb_get.h"

#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"

int lldpPortConfigTable_init_data(
    lldpPortConfigTable_registration *lldpPortConfigTable_reg) {
    DEBUGMSGTL(("verbose:lldpPortConfigTable:lldpPortConfigTable_init_data",
                "called\n"));
    return MFD_SUCCESS;
}

void lldpPortConfigTable_container_init(netsnmp_container **container_ptr_ptr,
                                        netsnmp_cache *cache) {
    DEBUGMSGTL(
        ("verbose:lldpPortConfigTable:lldpPortConfigTable_container_init",
         "called\n"));
    if (NULL == container_ptr_ptr) {
        snmp_log(LOG_ERR,
                 "bad container param to lldpPortConfigTable_container_init\n");
        return;
    }
    *container_ptr_ptr = NULL;
    if (NULL == cache) {
        snmp_log(LOG_ERR,
                 "bad cache param to lldpPortConfigTable_container_init\n");
        return;
    }
    cache->timeout = LLDPPORTCONFIGTABLE_CACHE_TIMEOUT;
}

void lldpPortConfigTable_container_shutdown(netsnmp_container *container_ptr) {
    DEBUGMSGTL(
        ("verbose:lldpPortConfigTable:lldpPortConfigTable_container_shutdown",
         "called\n"));
    if (NULL == container_ptr) {
        snmp_log(LOG_ERR,
                 "bad params to lldpPortConfigTable_container_shutdown\n");
        return;
    }
}

int lldpPortConfigTable_container_load(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpPortConfigTable:lldpPortConfigTable_container_load",
         "called\n"));
    lldpPortConfigTable_rowreq_ctx *rowreq_ctx;
    size_t count = 0;

    const struct ovsrec_interface *interface_row = NULL;
    const struct ovsrec_system *system_row = NULL;

    long lldpPortConfigPortNum;

    long lldpPortConfigAdminStatus;
    long lldpPortConfigNotificationEnable;
    u_long lldpPortConfigTLVsTxEnable;

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
        ovsdb_get_lldpPortConfigPortNum(idl, interface_row,
                                        &lldpPortConfigPortNum);

        ovsdb_get_lldpPortConfigAdminStatus(idl, interface_row,
                                            &lldpPortConfigAdminStatus);
        ovsdb_get_lldpPortConfigNotificationEnable(
            idl, interface_row, &lldpPortConfigNotificationEnable);
        ovsdb_get_lldpPortConfigTLVsTxEnable(idl, interface_row, system_row,
                                             &lldpPortConfigTLVsTxEnable);

        rowreq_ctx = lldpPortConfigTable_allocate_rowreq_ctx(NULL);
        if (rowreq_ctx == NULL) {
            snmp_log(LOG_ERR, "memory allocation failed");
            return MFD_RESOURCE_UNAVAILABLE;
        }
        if (MFD_SUCCESS != lldpPortConfigTable_indexes_set(
                               rowreq_ctx, lldpPortConfigPortNum)) {
            snmp_log(LOG_ERR, "error setting indexes while loading");
            lldpPortConfigTable_release_rowreq_ctx(rowreq_ctx);
            continue;
        }

        rowreq_ctx->data.lldpPortConfigAdminStatus = lldpPortConfigAdminStatus;
        rowreq_ctx->data.lldpPortConfigNotificationEnable =
            lldpPortConfigNotificationEnable;
        rowreq_ctx->data.lldpPortConfigTLVsTxEnable =
            lldpPortConfigTLVsTxEnable;
        CONTAINER_INSERT(container, rowreq_ctx);
        ++count;
    }
    DEBUGMSGTL(
        ("verbose:lldpPortConfigTable:lldpPortConfigTable_container_load",
         "inserted %d records\n", (int)count));
    return MFD_SUCCESS;
}

void lldpPortConfigTable_container_free(netsnmp_container *container) {
    DEBUGMSGTL(
        ("verbose:lldpPortConfigTable:lldpPortConfigTable_container_free",
         "called\n"));
}

int lldpPortConfigTable_row_prep(lldpPortConfigTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(("verbose:lldpPortConfigTable:lldpPortConfigTable_row_prep",
                "called\n"));
    netsnmp_assert(NULL != rowreq_ctx);
    return MFD_SUCCESS;
}

#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"
#include "LLDP_MIB_custom.h"
#include "lldpPortConfigTable_ovsdb_get.h"

void ovsdb_get_lldpPortConfigPortNum(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpPortConfigPortNum_val_ptr) {
    lldpPortConfigPortNum_custom_function(idl, interface_row,
                                          lldpPortConfigPortNum_val_ptr);
}

void ovsdb_get_lldpPortConfigAdminStatus(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpPortConfigAdminStatus_val_ptr) {
    lldpPortConfigAdminStatus_custom_function(
        idl, interface_row, lldpPortConfigAdminStatus_val_ptr);
}

void ovsdb_get_lldpPortConfigNotificationEnable(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpPortConfigNotificationEnable_val_ptr) {
    char *temp = (char *)smap_get(&interface_row->other_config, "lldp_enable");
    if (temp == NULL) {
        *lldpPortConfigNotificationEnable_val_ptr = 0;
    } else {
        *lldpPortConfigNotificationEnable_val_ptr = (long)atoi(temp);
    }
}

void ovsdb_get_lldpPortConfigTLVsTxEnable(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    const struct ovsrec_system *system_row,
    u_long *lldpPortConfigTLVsTxEnable_val_ptr) {
    lldpPortConfigTLVsTxEnable_custom_function(
        idl, interface_row, system_row, lldpPortConfigTLVsTxEnable_val_ptr);
}

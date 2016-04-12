#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"
#include "LLDP_MIB_custom.h"
#include "lldpRemTable_ovsdb_get.h"

void ovsdb_get_lldpRemTimeMark(struct ovsdb_idl *idl,
                               const struct ovsrec_interface *interface_row,
                               long *lldpRemTimeMark_val_ptr) {
    *lldpRemTimeMark_val_ptr = (long)NULL;
}

void ovsdb_get_lldpRemLocalPortNum(struct ovsdb_idl *idl,
                                   const struct ovsrec_interface *interface_row,
                                   long *lldpRemLocalPortNum_val_ptr) {
    lldpPortConfigPortNum_custom_function(idl, interface_row,
                                          lldpRemLocalPortNum_val_ptr);
}

void ovsdb_get_lldpRemIndex(struct ovsdb_idl *idl,
                            const struct ovsrec_interface *interface_row,
                            long *lldpRemIndex_val_ptr) {
    *lldpRemIndex_val_ptr = (long)NULL;
}

void ovsdb_get_lldpRemChassisIdSubtype(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpRemChassisIdSubtype_val_ptr) {
    char *temp = (char *)smap_get(&interface_row->lldp_neighbor_info,
                                  "chassis_id_subtype");
    if (temp == NULL) {
        *lldpRemChassisIdSubtype_val_ptr = 0;
    } else {
        *lldpRemChassisIdSubtype_val_ptr = (long)atoi(temp);
    }
}

void ovsdb_get_lldpRemChassisId(struct ovsdb_idl *idl,
                                const struct ovsrec_interface *interface_row,
                                char *lldpRemChassisId_val_ptr,
                                size_t *lldpRemChassisId_val_ptr_len) {
    char *temp =
        (char *)smap_get(&interface_row->lldp_neighbor_info, "chassis_id");
    *lldpRemChassisId_val_ptr_len = temp != NULL ? strlen(temp) : 0;
    memcpy(lldpRemChassisId_val_ptr, temp, *lldpRemChassisId_val_ptr_len);
}

void ovsdb_get_lldpRemPortIdSubtype(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpRemPortIdSubtype_val_ptr) {
    char *temp =
        (char *)smap_get(&interface_row->lldp_neighbor_info, "port_id_subtype");
    if (temp == NULL) {
        *lldpRemPortIdSubtype_val_ptr = 0;
    } else {
        *lldpRemPortIdSubtype_val_ptr = (long)atoi(temp);
    }
}

void ovsdb_get_lldpRemPortId(struct ovsdb_idl *idl,
                             const struct ovsrec_interface *interface_row,
                             char *lldpRemPortId_val_ptr,
                             size_t *lldpRemPortId_val_ptr_len) {
    char *temp = (char *)interface_row->name;
    *lldpRemPortId_val_ptr_len = temp != NULL ? strlen(temp) : 0;
    memcpy(lldpRemPortId_val_ptr, temp, *lldpRemPortId_val_ptr_len);
}

void ovsdb_get_lldpRemPortDesc(struct ovsdb_idl *idl,
                               const struct ovsrec_interface *interface_row,
                               char *lldpRemPortDesc_val_ptr,
                               size_t *lldpRemPortDesc_val_ptr_len) {
    char *temp = (char *)interface_row->name;
    *lldpRemPortDesc_val_ptr_len = temp != NULL ? strlen(temp) : 0;
    memcpy(lldpRemPortDesc_val_ptr, temp, *lldpRemPortDesc_val_ptr_len);
}

void ovsdb_get_lldpRemSysName(struct ovsdb_idl *idl,
                              const struct ovsrec_interface *interface_row,
                              const struct ovsrec_system *system_row,
                              char *lldpRemSysName_val_ptr,
                              size_t *lldpRemSysName_val_ptr_len) {
    char *temp = (char *)system_row->hostname;
    *lldpRemSysName_val_ptr_len = temp != NULL ? strlen(temp) : 0;
    memcpy(lldpRemSysName_val_ptr, temp, *lldpRemSysName_val_ptr_len);
}

void ovsdb_get_lldpRemSysDesc(struct ovsdb_idl *idl,
                              const struct ovsrec_interface *interface_row,
                              char *lldpRemSysDesc_val_ptr,
                              size_t *lldpRemSysDesc_val_ptr_len) {
    *lldpRemSysDesc_val_ptr = '\0';
    *lldpRemSysDesc_val_ptr_len = 0;
}

void ovsdb_get_lldpRemSysCapSupported(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    u_long *lldpRemSysCapSupported_val_ptr) {
    *lldpRemSysCapSupported_val_ptr = (u_long)NULL;
}

void ovsdb_get_lldpRemSysCapEnabled(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    const struct ovsrec_system *system_row,
    u_long *lldpRemSysCapEnabled_val_ptr) {
    lldpPortConfigTLVsTxEnable_custom_function(idl, interface_row, system_row,
                                               lldpRemSysCapEnabled_val_ptr);
}

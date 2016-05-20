#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"
#include "LLDP_MIB_custom.h"
#include "lldpLocPortTable_ovsdb_get.h"

void ovsdb_get_lldpLocPortNum(struct ovsdb_idl *idl,
                              const struct ovsrec_interface *interface_row,
                              long *lldpLocPortNum_val_ptr) {
    lldpPortConfigPortNum_custom_function(idl, interface_row,
                                          lldpLocPortNum_val_ptr);
}

void ovsdb_get_lldpLocPortIdSubtype(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpLocPortIdSubtype_val_ptr) {
    *lldpLocPortIdSubtype_val_ptr = (long)NULL;
}

void ovsdb_get_lldpLocPortId(struct ovsdb_idl *idl,
                             const struct ovsrec_interface *interface_row,
                             char *lldpLocPortId_val_ptr,
                             size_t *lldpLocPortId_val_ptr_len) {
    char *temp = (char *)interface_row->name;
    *lldpLocPortId_val_ptr_len = temp != NULL ? strlen(temp) : 0;
    memcpy(lldpLocPortId_val_ptr, temp, *lldpLocPortId_val_ptr_len);
}

void ovsdb_get_lldpLocPortDesc(struct ovsdb_idl *idl,
                               const struct ovsrec_interface *interface_row,
                               char *lldpLocPortDesc_val_ptr,
                               size_t *lldpLocPortDesc_val_ptr_len) {
    *lldpLocPortDesc_val_ptr = '\0';
    *lldpLocPortDesc_val_ptr_len = 0;
}

#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"
#include "LLDP_MIB_custom.h"
#include "lldpRemManAddrTable_ovsdb_get.h"

void ovsdb_get_lldpRemManAddrSubtype(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpRemManAddrSubtype_val_ptr) {
    *lldpRemManAddrSubtype_val_ptr = (long)NULL;
}

void ovsdb_get_lldpRemManAddr(struct ovsdb_idl *idl,
                              const struct ovsrec_interface *interface_row,
                              const struct ovsrec_system *system_row,
                              char *lldpRemManAddr_val_ptr,
                              size_t *lldpRemManAddr_val_ptr_len) {
    char *temp = (char *)smap_get(&system_row->other_config, "lldp_mgmt_add");
    *lldpRemManAddr_val_ptr_len = temp != NULL ? strlen(temp) : 0;
    memcpy(lldpRemManAddr_val_ptr, temp, *lldpRemManAddr_val_ptr_len);
}

void ovsdb_get_lldpRemManAddrIfSubtype(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpRemManAddrIfSubtype_val_ptr) {
    *lldpRemManAddrIfSubtype_val_ptr = (long)NULL;
}

void ovsdb_get_lldpRemManAddrIfId(struct ovsdb_idl *idl,
                                  const struct ovsrec_interface *interface_row,
                                  long *lldpRemManAddrIfId_val_ptr) {
    *lldpRemManAddrIfId_val_ptr = (long)NULL;
}

void ovsdb_get_lldpRemManAddrOID(struct ovsdb_idl *idl,
                                 const struct ovsrec_interface *interface_row,
                                 oid *lldpRemManAddrOID_val_ptr,
                                 size_t *lldpRemManAddrOID_val_ptr_len) {
    *lldpRemManAddrOID_val_ptr = (oid)NULL;
    *lldpRemManAddrOID_val_ptr_len = 0;
}

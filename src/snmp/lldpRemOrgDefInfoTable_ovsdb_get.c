#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"
#include "LLDP_MIB_custom.h"
#include "lldpRemOrgDefInfoTable_ovsdb_get.h"

void ovsdb_get_lldpRemOrgDefInfoOUI(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    char *lldpRemOrgDefInfoOUI_val_ptr,
    size_t *lldpRemOrgDefInfoOUI_val_ptr_len) {
    *lldpRemOrgDefInfoOUI_val_ptr = '\0';
    *lldpRemOrgDefInfoOUI_val_ptr_len = 0;
}

void ovsdb_get_lldpRemOrgDefInfoSubtype(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpRemOrgDefInfoSubtype_val_ptr) {
    *lldpRemOrgDefInfoSubtype_val_ptr = (long)NULL;
}

void ovsdb_get_lldpRemOrgDefInfoIndex(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpRemOrgDefInfoIndex_val_ptr) {
    *lldpRemOrgDefInfoIndex_val_ptr = (long)NULL;
}

void ovsdb_get_lldpRemOrgDefInfo(struct ovsdb_idl *idl,
                                 const struct ovsrec_interface *interface_row,
                                 char *lldpRemOrgDefInfo_val_ptr,
                                 size_t *lldpRemOrgDefInfo_val_ptr_len) {
    *lldpRemOrgDefInfo_val_ptr = '\0';
    *lldpRemOrgDefInfo_val_ptr_len = 0;
}

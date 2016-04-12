#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"
#include "LLDP_MIB_custom.h"
#include "lldpRemUnknownTLVTable_ovsdb_get.h"

void ovsdb_get_lldpRemUnknownTLVType(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpRemUnknownTLVType_val_ptr) {
    *lldpRemUnknownTLVType_val_ptr = (long)NULL;
}

void ovsdb_get_lldpRemUnknownTLVInfo(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    char *lldpRemUnknownTLVInfo_val_ptr,
    size_t *lldpRemUnknownTLVInfo_val_ptr_len) {
    *lldpRemUnknownTLVInfo_val_ptr = '\0';
    *lldpRemUnknownTLVInfo_val_ptr_len = 0;
}

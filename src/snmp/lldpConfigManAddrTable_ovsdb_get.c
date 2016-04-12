#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"
#include "LLDP_MIB_custom.h"
#include "lldpConfigManAddrTable_ovsdb_get.h"

void ovsdb_get_lldpLocManAddrSubtype(struct ovsdb_idl *idl,
                                     const struct ovsrec_system *system_row,
                                     long *lldpLocManAddrSubtype_val_ptr) {
    *lldpLocManAddrSubtype_val_ptr = (long)NULL;
}

void ovsdb_get_lldpLocManAddr(struct ovsdb_idl *idl,
                              const struct ovsrec_system *system_row,
                              char *lldpLocManAddr_val_ptr,
                              size_t *lldpLocManAddr_val_ptr_len) {
    char *temp = (char *)smap_get(&system_row->other_config, "lldp_mgmt_addr");
    *lldpLocManAddr_val_ptr_len = temp != NULL ? strlen(temp) : 0;
    memcpy(lldpLocManAddr_val_ptr, temp, *lldpLocManAddr_val_ptr_len);
}

void ovsdb_get_lldpConfigManAddrPortsTxEnable(
    struct ovsdb_idl *idl, const struct ovsrec_system *system_row,
    char *lldpConfigManAddrPortsTxEnable_val_ptr,
    size_t *lldpConfigManAddrPortsTxEnable_val_ptr_len) {
    *lldpConfigManAddrPortsTxEnable_val_ptr = '\0';
    *lldpConfigManAddrPortsTxEnable_val_ptr_len = 0;
}

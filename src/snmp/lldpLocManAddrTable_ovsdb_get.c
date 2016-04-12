#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"
#include "LLDP_MIB_custom.h"
#include "lldpLocManAddrTable_ovsdb_get.h"

void ovsdb_get_lldpLocManAddrLen(struct ovsdb_idl *idl,
                                 const struct ovsrec_system *system_row,
                                 long *lldpLocManAddrLen_val_ptr) {
    *lldpLocManAddrLen_val_ptr = (long)NULL;
}

void ovsdb_get_lldpLocManAddrIfSubtype(struct ovsdb_idl *idl,
                                       const struct ovsrec_system *system_row,
                                       long *lldpLocManAddrIfSubtype_val_ptr) {
    *lldpLocManAddrIfSubtype_val_ptr = (long)NULL;
}

void ovsdb_get_lldpLocManAddrIfId(struct ovsdb_idl *idl,
                                  const struct ovsrec_system *system_row,
                                  long *lldpLocManAddrIfId_val_ptr) {
    *lldpLocManAddrIfId_val_ptr = (long)NULL;
}

void ovsdb_get_lldpLocManAddrOID(struct ovsdb_idl *idl,
                                 const struct ovsrec_system *system_row,
                                 oid *lldpLocManAddrOID_val_ptr,
                                 size_t *lldpLocManAddrOID_val_ptr_len) {
    *lldpLocManAddrOID_val_ptr = (oid)NULL;
    *lldpLocManAddrOID_val_ptr_len = 0;
}

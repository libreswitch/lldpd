#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"
#include "LLDP_MIB_custom.h"
#include "lldpStatsTxPortTable_ovsdb_get.h"

void ovsdb_get_lldpStatsTxPortNum(struct ovsdb_idl *idl,
                                  const struct ovsrec_interface *interface_row,
                                  long *lldpStatsTxPortNum_val_ptr) {
    lldpPortConfigPortNum_custom_function(idl, interface_row,
                                          lldpStatsTxPortNum_val_ptr);
}

void ovsdb_get_lldpStatsTxPortFramesTotal(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsTxPortFramesTotal_val_ptr) {
    lldpStatsTxPortFramesTotal_custom_function(
        idl, interface_row, lldpStatsTxPortFramesTotal_val_ptr);
}

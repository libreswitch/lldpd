#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"
#include "LLDP_MIB_custom.h"
#include "lldpStatsRxPortTable_ovsdb_get.h"

void ovsdb_get_lldpStatsRxPortNum(struct ovsdb_idl *idl,
                                  const struct ovsrec_interface *interface_row,
                                  long *lldpStatsRxPortNum_val_ptr) {
    lldpPortConfigPortNum_custom_function(idl, interface_row,
                                          lldpStatsRxPortNum_val_ptr);
}

void ovsdb_get_lldpStatsRxPortFramesDiscardedTotal(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsRxPortFramesDiscardedTotal_val_ptr) {
    lldpStatsRxPortFramesDiscardedTotal_custom_function(
        idl, interface_row, lldpStatsRxPortFramesDiscardedTotal_val_ptr);
}

void ovsdb_get_lldpStatsRxPortFramesErrors(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsRxPortFramesErrors_val_ptr) {
    lldpStatsRxPortFramesErrors_custom_function(
        idl, interface_row, lldpStatsRxPortFramesErrors_val_ptr);
}

void ovsdb_get_lldpStatsRxPortFramesTotal(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsRxPortFramesTotal_val_ptr) {
    lldpStatsRxPortFramesTotal_custom_function(
        idl, interface_row, lldpStatsRxPortFramesTotal_val_ptr);
}

void ovsdb_get_lldpStatsRxPortTLVsDiscardedTotal(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsRxPortTLVsDiscardedTotal_val_ptr) {
    lldpStatsRxPortTLVsDiscardedTotal_custom_function(
        idl, interface_row, lldpStatsRxPortTLVsDiscardedTotal_val_ptr);
}

void ovsdb_get_lldpStatsRxPortTLVsUnrecognizedTotal(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsRxPortTLVsUnrecognizedTotal_val_ptr) {
    lldpStatsRxPortTLVsUnrecognizedTotal_custom_function(
        idl, interface_row, lldpStatsRxPortTLVsUnrecognizedTotal_val_ptr);
}

void ovsdb_get_lldpStatsRxPortAgeoutsTotal(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsRxPortAgeoutsTotal_val_ptr) {
    *lldpStatsRxPortAgeoutsTotal_val_ptr = (long)NULL;
}

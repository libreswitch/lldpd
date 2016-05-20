#include "vswitch-idl.h"
#include "ovsdb-idl.h"

int lldpPortConfigTable_skip_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row);

void lldpPortConfigPortNum_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, long *lldpPortConfigPortNum_val_ptr);

void lldpPortConfigAdminStatus_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, long *lldpPortConfigAdminStatus_val_ptr);

void lldpPortConfigTLVsTxEnable_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, const struct ovsrec_system *system_row, u_long *lldpPortConfigTLVsTxEnable_val_ptr);

void lldpStatsRxPortFramesErrors_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, long *lldpStatsRxPortFramesError_val_ptr);

void lldpStatsRxPortFramesDiscardedTotal_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, long *lldpStatsRxPortFramesDiscardedTotal_val_ptr);

void lldpStatsRxPortFramesTotal_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, long *lldpStatsRxPortFramesTotal_val_ptr);

void lldpStatsTxPortFramesTotal_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, long *lldpStatsTxPortFramesTotal_val_ptr);

void lldpStatsRxPortTLVsUnrecognizedTotal_custom_function(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsRxPortTLVsUnrecognizedTotal_val_ptr);

void lldpStatsRxPortTLVsDiscardedTotal_custom_function(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsRxPortTLVsDiscardedTotal_val_ptr);

void lldpPortConfigPortNum_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, long *lldpPortConfigPortNum_val_ptr);

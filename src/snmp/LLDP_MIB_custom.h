#include "vswitch-idl.h"
#include "ovsdb-idl.h"

int lldpPortConfigTable_skip_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row);

void lldpPortConfigPortNum_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, long *lldpPortConfigPortNum_val_ptr);

void lldpPortConfigAdminStatus_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, long *lldpPortConfigAdminStatus_val_ptr);

void lldpPortConfigTLVsTxEnable_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, const struct ovsrec_system *system_row, u_long *lldpPortConfigTLVsTxEnable_val_ptr);

void lldpMessageTxInterval_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_system *system_row, long *lldpMessageTxInterval_val_ptr);

void lldpMessageTxHoldMultiplier_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_system *system_row, long *lldpMessageTxHoldMultiplier_val_ptr);

void lldpStatsRemTablesInserts_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_system *system_row, long *lldpStatsRemTablesInserts_val_ptr);

void lldpStatsRemTablesDeletes_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_system *system_row, long *lldpStatsRemTablesDeletes_val_ptr);

void lldpStatsRemTablesDrops_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_system *system_row, long *lldpStatsRemTablesDrops_val_ptr);

void lldpStatsRemTablesAgeouts_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_system *system_row, long *lldpStatsRemTablesAgeouts_val_ptr);

void smap_to_long(const char* in, long *out);

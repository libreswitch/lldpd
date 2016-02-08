#ifndef LLDP_MIB_SCALARS_OVSDB_GET_H
#define LLDP_MIB_SCALARS_OVSDB_GET_H

#include "vswitch-idl.h"
#include "ovsdb-idl.h"
extern struct ovsdb_idl *idl;

void ovsdb_get_lldpMessageTxInterval(struct ovsdb_idl *idl,
                                     const struct ovsrec_system *system_row,
                                     long *lldpMessageTxInterval_val_ptr);
void ovsdb_get_lldpMessageTxHoldMultiplier(
    struct ovsdb_idl *idl, const struct ovsrec_system *system_row,
    long *lldpMessageTxHoldMultiplier_val_ptr);
void ovsdb_get_lldpReinitDelay(struct ovsdb_idl *idl,
                               long *lldpReinitDelay_val_ptr);
void ovsdb_get_lldpTxDelay(struct ovsdb_idl *idl, long *lldpTxDelay_val_ptr);
void ovsdb_get_lldpNotificationInterval(struct ovsdb_idl *idl,
                                        long *lldpNotificationInterval_val_ptr);
void ovsdb_get_lldpStatsRemTablesLastChangeTime(
    struct ovsdb_idl *idl, long *lldpStatsRemTablesLastChangeTime_val_ptr);
void ovsdb_get_lldpStatsRemTablesInserts(
    struct ovsdb_idl *idl, const struct ovsrec_system *system_row,
    long *lldpStatsRemTablesInserts_val_ptr);
void ovsdb_get_lldpStatsRemTablesDeletes(
    struct ovsdb_idl *idl, const struct ovsrec_system *system_row,
    long *lldpStatsRemTablesDeletes_val_ptr);
void ovsdb_get_lldpStatsRemTablesDrops(struct ovsdb_idl *idl,
                                       const struct ovsrec_system *system_row,
                                       long *lldpStatsRemTablesDrops_val_ptr);
void ovsdb_get_lldpStatsRemTablesAgeouts(
    struct ovsdb_idl *idl, const struct ovsrec_system *system_row,
    long *lldpStatsRemTablesAgeouts_val_ptr);
void ovsdb_get_lldpLocChassisIdSubtype(struct ovsdb_idl *idl,
                                       long *lldpLocChassisIdSubtype_val_ptr);
void ovsdb_get_lldpLocChassisId(struct ovsdb_idl *idl,
                                char *lldpLocChassisId_val_ptr,
                                size_t *lldpLocChassisId_val_ptr_len);
void ovsdb_get_lldpLocSysName(struct ovsdb_idl *idl,
                              const struct ovsrec_system *system_row,
                              char *lldpLocSysName_val_ptr,
                              size_t *lldpLocSysName_val_ptr_len);
void ovsdb_get_lldpLocSysDesc(struct ovsdb_idl *idl,
                              char *lldpLocSysDesc_val_ptr,
                              size_t *lldpLocSysDesc_val_ptr_len);
void ovsdb_get_lldpLocSysCapSupported(struct ovsdb_idl *idl,
                                      u_long *lldpLocSysCapSupported_val_ptr);
void ovsdb_get_lldpLocSysCapEnabled(struct ovsdb_idl *idl,
                                    u_long *lldpLocSysCapEnabled_val_ptr);

#endif
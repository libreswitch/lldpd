#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"
#include "LLDP_MIB_custom.h"
#include "LLDP_MIB_scalars_ovsdb_get.h"

void ovsdb_get_lldpMessageTxInterval(struct ovsdb_idl *idl,
                                     const struct ovsrec_system *system_row,
                                     long *lldpMessageTxInterval_val_ptr) {
    char *temp =
        (char *)smap_get(&system_row->other_config, "lldp_tx_interval");
    if (temp == NULL) {
        *lldpMessageTxInterval_val_ptr = 0;
    } else {
        *lldpMessageTxInterval_val_ptr = (long)atoi(temp);
    }
}

void ovsdb_get_lldpMessageTxHoldMultiplier(
    struct ovsdb_idl *idl, const struct ovsrec_system *system_row,
    long *lldpMessageTxHoldMultiplier_val_ptr) {
    char *temp = (char *)smap_get(&system_row->other_config, "lldp_hold");
    if (temp == NULL) {
        *lldpMessageTxHoldMultiplier_val_ptr = 0;
    } else {
        *lldpMessageTxHoldMultiplier_val_ptr = (long)atoi(temp);
    }
}

void ovsdb_get_lldpReinitDelay(struct ovsdb_idl *idl,
                               const struct ovsrec_system *system_row,
                               long *lldpReinitDelay_val_ptr) {
    char *temp = (char *)smap_get(&system_row->other_config, "lldp_reinit");
    if (temp == NULL) {
        *lldpReinitDelay_val_ptr = 0;
    } else {
        *lldpReinitDelay_val_ptr = (long)atoi(temp);
    }
}

void ovsdb_get_lldpTxDelay(struct ovsdb_idl *idl, long *lldpTxDelay_val_ptr) {
    *lldpTxDelay_val_ptr = (long)NULL;
}

void ovsdb_get_lldpNotificationInterval(
    struct ovsdb_idl *idl, long *lldpNotificationInterval_val_ptr) {
    *lldpNotificationInterval_val_ptr = (long)NULL;
}

void ovsdb_get_lldpStatsRemTablesLastChangeTime(
    struct ovsdb_idl *idl, long *lldpStatsRemTablesLastChangeTime_val_ptr) {
    *lldpStatsRemTablesLastChangeTime_val_ptr = (long)NULL;
}

void ovsdb_get_lldpStatsRemTablesInserts(
    struct ovsdb_idl *idl, const struct ovsrec_system *system_row,
    long *lldpStatsRemTablesInserts_val_ptr) {
    char *temp =
        (char *)smap_get(&system_row->lldp_statistics, "lldp_table_inserts");
    if (temp == NULL) {
        *lldpStatsRemTablesInserts_val_ptr = 0;
    } else {
        *lldpStatsRemTablesInserts_val_ptr = (long)atoi(temp);
    }
}

void ovsdb_get_lldpStatsRemTablesDeletes(
    struct ovsdb_idl *idl, const struct ovsrec_system *system_row,
    long *lldpStatsRemTablesDeletes_val_ptr) {
    char *temp =
        (char *)smap_get(&system_row->lldp_statistics, "lldp_table_deletes");
    if (temp == NULL) {
        *lldpStatsRemTablesDeletes_val_ptr = 0;
    } else {
        *lldpStatsRemTablesDeletes_val_ptr = (long)atoi(temp);
    }
}

void ovsdb_get_lldpStatsRemTablesDrops(struct ovsdb_idl *idl,
                                       const struct ovsrec_system *system_row,
                                       long *lldpStatsRemTablesDrops_val_ptr) {
    char *temp =
        (char *)smap_get(&system_row->lldp_statistics, "lldp_table_drops");
    if (temp == NULL) {
        *lldpStatsRemTablesDrops_val_ptr = 0;
    } else {
        *lldpStatsRemTablesDrops_val_ptr = (long)atoi(temp);
    }
}

void ovsdb_get_lldpStatsRemTablesAgeouts(
    struct ovsdb_idl *idl, const struct ovsrec_system *system_row,
    long *lldpStatsRemTablesAgeouts_val_ptr) {
    char *temp =
        (char *)smap_get(&system_row->lldp_statistics, "lldp_table_ageouts");
    if (temp == NULL) {
        *lldpStatsRemTablesAgeouts_val_ptr = 0;
    } else {
        *lldpStatsRemTablesAgeouts_val_ptr = (long)atoi(temp);
    }
}

void ovsdb_get_lldpLocChassisIdSubtype(struct ovsdb_idl *idl,
                                       long *lldpLocChassisIdSubtype_val_ptr) {
    *lldpLocChassisIdSubtype_val_ptr = (long)NULL;
}

void ovsdb_get_lldpLocChassisId(struct ovsdb_idl *idl,
                                char *lldpLocChassisId_val_ptr,
                                size_t *lldpLocChassisId_val_ptr_len) {
    *lldpLocChassisId_val_ptr = '\0';
    *lldpLocChassisId_val_ptr_len = 0;
}

void ovsdb_get_lldpLocSysName(struct ovsdb_idl *idl,
                              const struct ovsrec_system *system_row,
                              char *lldpLocSysName_val_ptr,
                              size_t *lldpLocSysName_val_ptr_len) {
    char *temp = (char *)system_row->hostname;
    *lldpLocSysName_val_ptr_len = temp != NULL ? strlen(temp) : 0;
    memcpy(lldpLocSysName_val_ptr, temp, *lldpLocSysName_val_ptr_len);
}

void ovsdb_get_lldpLocSysDesc(struct ovsdb_idl *idl,
                              char *lldpLocSysDesc_val_ptr,
                              size_t *lldpLocSysDesc_val_ptr_len) {
    *lldpLocSysDesc_val_ptr = '\0';
    *lldpLocSysDesc_val_ptr_len = 0;
}

void ovsdb_get_lldpLocSysCapSupported(struct ovsdb_idl *idl,
                                      u_long *lldpLocSysCapSupported_val_ptr) {
    *lldpLocSysCapSupported_val_ptr = (u_long)NULL;
}

void ovsdb_get_lldpLocSysCapEnabled(struct ovsdb_idl *idl,
                                    u_long *lldpLocSysCapEnabled_val_ptr) {
    *lldpLocSysCapEnabled_val_ptr = (u_long)NULL;
}

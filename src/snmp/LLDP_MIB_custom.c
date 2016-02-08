// Define Custom Functions for LLDP_MIB MIB in this fileName
#include <stdlib.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"
#include "LLDP_MIB_custom.h"

int lldpPortConfigTable_skip_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row) {
    if(strchr(interface_row->name, '-') != NULL)
        return 1;
    if (atoi(interface_row->name) == 0)
        return 1;
    return 0;
}

void lldpPortConfigPortNum_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, long *lldpPortConfigPortNum_val_ptr) {
    char * temp = interface_row->name;
    *lldpPortConfigPortNum_val_ptr = atoi(temp);
}

void lldpPortConfigAdminStatus_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, long *lldpPortConfigAdminStatus_val_ptr) {
    char *temp = (char*)smap_get(&interface_row->other_config, "lldp_enable_dir");
    if(temp != NULL) {
        if(!strcmp(temp,"off")) {
            *lldpPortConfigAdminStatus_val_ptr = 4;
        }
        if(!strcmp(temp,"tx")) {
            *lldpPortConfigAdminStatus_val_ptr = 1;
        }
        if(!strcmp(temp,"rx")) {
            *lldpPortConfigAdminStatus_val_ptr = 2;
        }
    }
    else {
        *lldpPortConfigAdminStatus_val_ptr = 3;
    }
}

void lldpPortConfigTLVsTxEnable_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, const struct ovsrec_system *system_row, u_long *lldpPortConfigTLVsTxEnable_val_ptr) {
    u_long mask = 0, ret = 0;
    char *temp = NULL;

    temp = (char*)smap_get(&system_row->other_config, "lldp_tlv_port_desc_enable");
    if (temp != NULL) {
        if(!strcmp(temp, "false")) {
            mask = ((~0UL) ^ (1UL));
            ret = ret & mask;
        }
        else {
            mask = 1UL;
            ret = ret | mask;
        }
        temp = NULL;
    }
    else {
        mask = 1UL;
        ret = ret | mask;
    }

    temp = (char*)smap_get(&system_row->other_config, "lldp_tlv_sys_name_enable");
    if (temp != NULL) {
        if(!strcmp(temp, "false")) {
            mask = ((~0UL) ^(2UL));
            ret = ret & mask;
        }
        else {
            mask = 2UL;
            ret = ret | mask;
        }
        temp = NULL;
    }
    else{
        mask = 2UL;
        ret = ret | mask;
    }

    temp = (char*)smap_get(&system_row->other_config, "lldp_tlv_sys_desc_enable");
    if(temp != NULL) {
        if(!strcmp(temp, "false")) {
            mask = ((~0UL) ^ (4UL));
            ret = ret & mask;
        }
        else {
            mask = 4UL;
            ret = ret | mask;
        }
        temp = NULL;
    }
    else {
         mask = 4UL;
         ret  = ret | mask;
    }

    temp = (char*)smap_get(&system_row->other_config, "lldp_tlv_sys_cap_enable");
    if(temp != NULL) {
        if(!strcmp(temp, "false")) {
            mask = ((~0UL) ^ (8UL));
            ret = ret & mask;
        }
        else {
            mask = 8UL;
            ret = ret | mask;
        }
        temp = NULL;
    }
    else {
        mask = 8UL;
        ret = ret | mask;
    }
    *lldpPortConfigTLVsTxEnable_val_ptr = ret;
}

void lldpMessageTxInterval_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_system *system_row, long *lldpMessageTxInterval_val_ptr) {
    char *temp = (char*)smap_get(&system_row->other_config, "lldp_tx_interval");
    smap_to_long(temp, lldpMessageTxInterval_val_ptr);
}

void lldpMessageTxHoldMultiplier_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_system *system_row, long *lldpMessageTxHoldMultiplier_val_ptr) {
    char *temp = (char*)smap_get(&system_row->other_config, "lldp_hold");
    smap_to_long(temp, lldpMessageTxHoldMultiplier_val_ptr);
}

void lldpStatsRemTablesInserts_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_system *system_row, long *lldpStatsRemTablesInserts_val_ptr) {
    char *temp = (char*)smap_get(&system_row->lldp_statistics, "lldp_table_inserts");
    smap_to_long(temp, lldpStatsRemTablesInserts_val_ptr);
}

void lldpStatsRemTablesDeletes_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_system *system_row, long *lldpStatsRemTablesDeletes_val_ptr) {
    char *temp = (char*)smap_get(&system_row->lldp_statistics, "lldp_table_deletes");
    smap_to_long(temp, lldpStatsRemTablesDeletes_val_ptr);
}

void lldpStatsRemTablesDrops_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_system *system_row, long *lldpStatsRemTablesDrops_val_ptr) {
    char *temp = (char*)smap_get(&system_row->lldp_statistics, "lldp_table_drops");
    smap_to_long(temp, lldpStatsRemTablesDrops_val_ptr);
}

void lldpStatsRemTablesAgeouts_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_system *system_row, long *lldpStatsRemTablesAgeouts_val_ptr) {
    char *temp = (char*)smap_get(&system_row->lldp_statistics, "lldp_table_ageouts");
    smap_to_long(temp, lldpStatsRemTablesAgeouts_val_ptr);
}

void smap_to_long(const char* in, long *out) {
    if(in == NULL) {
        *out = 0;
    }
    else {
        *out = atoi(in);
    }
}

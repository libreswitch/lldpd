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

void lldpStatsTxPortFramesTotal_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, long *lldpStatsTxPortFramesTotal_val_ptr){
    union ovsdb_atom atom;
    int tx_packets;
    unsigned int index;
    const struct ovsdb_datum *datum = NULL;

    datum = ovsrec_interface_get_lldp_statistics(interface_row, OVSDB_TYPE_STRING, OVSDB_TYPE_INTEGER);
    atom.string = "lldp_tx";
    index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
    tx_packets = (index == UINT_MAX)? 0 : datum->values[index].integer ;
    *lldpStatsTxPortFramesTotal_val_ptr = tx_packets;
}

void lldpStatsRxPortFramesTotal_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, long *lldpStatsRxPortFramesTotal_val_ptr){
    union ovsdb_atom atom;
    int rx_packets;
    unsigned int index;
    const struct ovsdb_datum *datum = NULL;

    datum = ovsrec_interface_get_lldp_statistics(interface_row, OVSDB_TYPE_STRING, OVSDB_TYPE_INTEGER);
    atom.string = "lldp_rx";
    index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
    rx_packets = (index == UINT_MAX)? 0 : datum->values[index].integer ;
    *lldpStatsRxPortFramesTotal_val_ptr = rx_packets;
}

void lldpStatsRxPortFramesDiscardedTotal_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, long *lldpStatsRxPortFramesDiscardedTotal_val_ptr){
    union ovsdb_atom atom;
    int rx_discarded;
    unsigned int index;
    const struct ovsdb_datum *datum = NULL;

    datum = ovsrec_interface_get_lldp_statistics(interface_row, OVSDB_TYPE_STRING, OVSDB_TYPE_INTEGER);
    atom.string = "lldp_rx_discared";
    index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
    rx_discarded = (index == UINT_MAX)? 0 : datum->values[index].integer ;
    *lldpStatsRxPortFramesDiscardedTotal_val_ptr = rx_discarded;
}

void lldpStatsRxPortFramesErrors_custom_function(const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row, long *lldpStatsRxPortFramesError_val_ptr){
    union ovsdb_atom atom;
    int rx_error;
    unsigned int index;
    const struct ovsdb_datum *datum = NULL;

    datum = ovsrec_interface_get_lldp_statistics(interface_row, OVSDB_TYPE_STRING, OVSDB_TYPE_INTEGER);
    atom.string = "lldp_rx_err";
    index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
    rx_error = (index == UINT_MAX)? 0 : datum->values[index].integer ;
    *lldpStatsRxPortFramesError_val_ptr = rx_error;
}

void lldpStatsRxPortTLVsUnrecognizedTotal_custom_function(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsRxPortTLVsUnrecognizedTotal_val_ptr){

    union ovsdb_atom atom;
    int rx_tlv_unknown;
    unsigned int index;
    const struct ovsdb_datum *datum = NULL;
    datum = ovsrec_interface_get_lldp_statistics(interface_row, OVSDB_TYPE_STRING, OVSDB_TYPE_INTEGER);
    atom.string = "lldp_rx_tlv_unknown";
    index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
    rx_tlv_unknown = (index == UINT_MAX)? 0 : datum->values[index].integer ;
    *lldpStatsRxPortTLVsUnrecognizedTotal_val_ptr = rx_tlv_unknown;
}

void lldpStatsRxPortTLVsDiscardedTotal_custom_function(struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsRxPortTLVsDiscardedTotal_val_ptr){
    union ovsdb_atom atom;
    int rx_tlv_discard;
    unsigned int index;
    const struct ovsdb_datum *datum = NULL;

    datum = ovsrec_interface_get_lldp_statistics(interface_row, OVSDB_TYPE_STRING, OVSDB_TYPE_INTEGER);
    atom.string = "lldp_rx_tlv_discard";
    index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
    rx_tlv_discard = (index == UINT_MAX)? 0 : datum->values[index].integer ;
    *lldpStatsRxPortTLVsDiscardedTotal_val_ptr = rx_tlv_discard;
}

#ifndef LLDPPORTCONFIGTABLE_OVSDB_GET_H
#define LLDPPORTCONFIGTABLE_OVSDB_GET_H

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"
void ovsdb_get_lldpPortConfigPortNum(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpPortConfigPortNum_val_ptr);

void ovsdb_get_lldpPortConfigAdminStatus(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpPortConfigAdminStatus_val_ptr);
void ovsdb_get_lldpPortConfigNotificationEnable(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpPortConfigNotificationEnable_val_ptr);
void ovsdb_get_lldpPortConfigTLVsTxEnable(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    const struct ovsrec_system *system_row,
    u_long *lldpPortConfigTLVsTxEnable_val_ptr);
#endif
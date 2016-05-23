#ifndef LLDPREMUNKNOWNTLVTABLE_OVSDB_GET_H
#define LLDPREMUNKNOWNTLVTABLE_OVSDB_GET_H

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"
void ovsdb_get_lldpRemTimeMark(struct ovsdb_idl *idl,
                               const struct ovsrec_interface *interface_row,
                               long *lldpRemTimeMark_val_ptr);
void ovsdb_get_lldpRemLocalPortNum(struct ovsdb_idl *idl,
                                   const struct ovsrec_interface *interface_row,
                                   long *lldpRemLocalPortNum_val_ptr);
void ovsdb_get_lldpRemIndex(struct ovsdb_idl *idl,
                            const struct ovsrec_interface *interface_row,
                            long *lldpRemIndex_val_ptr);
void ovsdb_get_lldpRemUnknownTLVType(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpRemUnknownTLVType_val_ptr);

void ovsdb_get_lldpRemUnknownTLVInfo(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    char *lldpRemUnknownTLVInfo_val_ptr,
    size_t *lldpRemUnknownTLVInfo_val_ptr_len);
#endif
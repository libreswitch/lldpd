#ifndef LLDPREMMANADDRTABLE_OVSDB_GET_H
#define LLDPREMMANADDRTABLE_OVSDB_GET_H

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
void ovsdb_get_lldpRemManAddrSubtype(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpRemManAddrSubtype_val_ptr);
void ovsdb_get_lldpRemManAddr(struct ovsdb_idl *idl,
                              const struct ovsrec_interface *interface_row,
                              const struct ovsrec_system *system_row,
                              char *lldpRemManAddr_val_ptr,
                              size_t *lldpRemManAddr_val_ptr_len);

void ovsdb_get_lldpRemManAddrIfSubtype(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpRemManAddrIfSubtype_val_ptr);
void ovsdb_get_lldpRemManAddrIfId(struct ovsdb_idl *idl,
                                  const struct ovsrec_interface *interface_row,
                                  long *lldpRemManAddrIfId_val_ptr);
void ovsdb_get_lldpRemManAddrOID(struct ovsdb_idl *idl,
                                 const struct ovsrec_interface *interface_row,
                                 oid *lldpRemManAddrOID_val_ptr,
                                 size_t *lldpRemManAddrOID_val_ptr_len);
#endif
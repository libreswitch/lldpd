#ifndef LLDPLOCMANADDRTABLE_OVSDB_GET_H
#define LLDPLOCMANADDRTABLE_OVSDB_GET_H

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"
void ovsdb_get_lldpLocManAddrSubtype(struct ovsdb_idl *idl,
                                     const struct ovsrec_system *system_row,
                                     long *lldpLocManAddrSubtype_val_ptr);
void ovsdb_get_lldpLocManAddr(struct ovsdb_idl *idl,
                              const struct ovsrec_system *system_row,
                              char *lldpLocManAddr_val_ptr,
                              size_t *lldpLocManAddr_val_ptr_len);

void ovsdb_get_lldpLocManAddrLen(struct ovsdb_idl *idl,
                                 const struct ovsrec_system *system_row,
                                 long *lldpLocManAddrLen_val_ptr);
void ovsdb_get_lldpLocManAddrIfSubtype(struct ovsdb_idl *idl,
                                       const struct ovsrec_system *system_row,
                                       long *lldpLocManAddrIfSubtype_val_ptr);
void ovsdb_get_lldpLocManAddrIfId(struct ovsdb_idl *idl,
                                  const struct ovsrec_system *system_row,
                                  long *lldpLocManAddrIfId_val_ptr);
void ovsdb_get_lldpLocManAddrOID(struct ovsdb_idl *idl,
                                 const struct ovsrec_system *system_row,
                                 oid *lldpLocManAddrOID_val_ptr,
                                 size_t *lldpLocManAddrOID_val_ptr_len);
#endif
#ifndef LLDPLOCPORTTABLE_OVSDB_GET_H
#define LLDPLOCPORTTABLE_OVSDB_GET_H

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"
void ovsdb_get_lldpLocPortNum(struct ovsdb_idl *idl,
                              const struct ovsrec_interface *interface_row,
                              long *lldpLocPortNum_val_ptr);

void ovsdb_get_lldpLocPortIdSubtype(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpLocPortIdSubtype_val_ptr);
void ovsdb_get_lldpLocPortId(struct ovsdb_idl *idl,
                             const struct ovsrec_interface *interface_row,
                             char *lldpLocPortId_val_ptr,
                             size_t *lldpLocPortId_val_ptr_len);
void ovsdb_get_lldpLocPortDesc(struct ovsdb_idl *idl,
                               const struct ovsrec_interface *interface_row,
                               char *lldpLocPortDesc_val_ptr,
                               size_t *lldpLocPortDesc_val_ptr_len);
#endif
#ifndef LLDPREMORGDEFINFOTABLE_OVSDB_GET_H
#define LLDPREMORGDEFINFOTABLE_OVSDB_GET_H

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
void ovsdb_get_lldpRemOrgDefInfoOUI(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    char *lldpRemOrgDefInfoOUI_val_ptr,
    size_t *lldpRemOrgDefInfoOUI_val_ptr_len);
void ovsdb_get_lldpRemOrgDefInfoSubtype(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpRemOrgDefInfoSubtype_val_ptr);
void ovsdb_get_lldpRemOrgDefInfoIndex(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpRemOrgDefInfoIndex_val_ptr);

void ovsdb_get_lldpRemOrgDefInfo(struct ovsdb_idl *idl,
                                 const struct ovsrec_interface *interface_row,
                                 char *lldpRemOrgDefInfo_val_ptr,
                                 size_t *lldpRemOrgDefInfo_val_ptr_len);
#endif
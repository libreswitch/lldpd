#ifndef LLDPREMTABLE_OVSDB_GET_H
#define LLDPREMTABLE_OVSDB_GET_H

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

void ovsdb_get_lldpRemChassisIdSubtype(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpRemChassisIdSubtype_val_ptr);
void ovsdb_get_lldpRemChassisId(struct ovsdb_idl *idl,
                                const struct ovsrec_interface *interface_row,
                                char *lldpRemChassisId_val_ptr,
                                size_t *lldpRemChassisId_val_ptr_len);
void ovsdb_get_lldpRemPortIdSubtype(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpRemPortIdSubtype_val_ptr);
void ovsdb_get_lldpRemPortId(struct ovsdb_idl *idl,
                             const struct ovsrec_interface *interface_row,
                             char *lldpRemPortId_val_ptr,
                             size_t *lldpRemPortId_val_ptr_len);
void ovsdb_get_lldpRemPortDesc(struct ovsdb_idl *idl,
                               const struct ovsrec_interface *interface_row,
                               char *lldpRemPortDesc_val_ptr,
                               size_t *lldpRemPortDesc_val_ptr_len);
void ovsdb_get_lldpRemSysName(struct ovsdb_idl *idl,
                              const struct ovsrec_interface *interface_row,
                              const struct ovsrec_system *system_row,
                              char *lldpRemSysName_val_ptr,
                              size_t *lldpRemSysName_val_ptr_len);
void ovsdb_get_lldpRemSysDesc(struct ovsdb_idl *idl,
                              const struct ovsrec_interface *interface_row,
                              char *lldpRemSysDesc_val_ptr,
                              size_t *lldpRemSysDesc_val_ptr_len);
void ovsdb_get_lldpRemSysCapSupported(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    u_long *lldpRemSysCapSupported_val_ptr);
void ovsdb_get_lldpRemSysCapEnabled(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    const struct ovsrec_system *system_row,
    u_long *lldpRemSysCapEnabled_val_ptr);
#endif
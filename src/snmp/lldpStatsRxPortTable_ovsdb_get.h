#ifndef LLDPSTATSRXPORTTABLE_OVSDB_GET_H
#define LLDPSTATSRXPORTTABLE_OVSDB_GET_H

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"
void ovsdb_get_lldpStatsRxPortNum(struct ovsdb_idl *idl,
                                  const struct ovsrec_interface *interface_row,
                                  long *lldpStatsRxPortNum_val_ptr);

void ovsdb_get_lldpStatsRxPortFramesDiscardedTotal(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsRxPortFramesDiscardedTotal_val_ptr);
void ovsdb_get_lldpStatsRxPortFramesErrors(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsRxPortFramesErrors_val_ptr);
void ovsdb_get_lldpStatsRxPortFramesTotal(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsRxPortFramesTotal_val_ptr);
void ovsdb_get_lldpStatsRxPortTLVsDiscardedTotal(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsRxPortTLVsDiscardedTotal_val_ptr);
void ovsdb_get_lldpStatsRxPortTLVsUnrecognizedTotal(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsRxPortTLVsUnrecognizedTotal_val_ptr);
void ovsdb_get_lldpStatsRxPortAgeoutsTotal(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsRxPortAgeoutsTotal_val_ptr);
#endif
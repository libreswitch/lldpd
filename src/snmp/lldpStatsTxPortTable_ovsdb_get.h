#ifndef LLDPSTATSTXPORTTABLE_OVSDB_GET_H
#define LLDPSTATSTXPORTTABLE_OVSDB_GET_H

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"
void ovsdb_get_lldpStatsTxPortNum(struct ovsdb_idl *idl,
                                  const struct ovsrec_interface *interface_row,
                                  long *lldpStatsTxPortNum_val_ptr);

void ovsdb_get_lldpStatsTxPortFramesTotal(
    struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    long *lldpStatsTxPortFramesTotal_val_ptr);
#endif
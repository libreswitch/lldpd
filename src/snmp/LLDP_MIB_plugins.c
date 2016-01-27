#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "LLDP_MIB_plugins.h"
#include "LLDP_MIB_scalars.h"
#include "lldpPortConfigTable.h"

void ops_snmp_init(void) {
  init_lldpMessageTxInterval();
  init_lldpMessageTxHoldMultiplier();
  init_lldpReinitDelay();
  init_lldpTxDelay();
  init_lldpNotificationInterval();
  init_lldpStatsRemTablesLastChangeTime();
  init_lldpStatsRemTablesInserts();
  init_lldpStatsRemTablesDeletes();
  init_lldpStatsRemTablesDrops();
  init_lldpStatsRemTablesAgeouts();
  init_lldpLocChassisIdSubtype();
  init_lldpLocChassisId();
  init_lldpLocSysName();
  init_lldpLocSysDesc();
  init_lldpLocSysCapSupported();
  init_lldpLocSysCapEnabled();

  init_lldpPortConfigTable();
}

void ops_snmp_run(void) {}
void ops_snmp_wait(void) {}
void ops_snmp_destroy(void) { shutdown_lldpPortConfigTable(); }

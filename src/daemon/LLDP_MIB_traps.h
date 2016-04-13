#include "vswitch-idl.h"
#include "ovsdb-idl.h"
#include "snmptrap_lib.h"

void init_ovsdb_snmp_notifications(const struct ovsdb_idl *idl);

int send_lldpRemTablesChange(const namespace_type nm_type, const struct ovsdb_idl *idl, const char *lldpStatsRemTablesInserts_value, const char *lldpStatsRemTablesDeletes_value, const char *lldpStatsRemTablesDrops_value, const char *lldpStatsRemTablesAgeouts_value);

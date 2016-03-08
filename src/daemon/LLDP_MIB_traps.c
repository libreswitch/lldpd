#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "vswitch-idl.h"
#include "ovsdb-idl.h"
#include <sys/types.h>


oid objid_enterprise[] = {1, 3, 6, 1, 4, 1, 3, 1, 1};
oid objid_sysdescr[] = {1, 3, 6, 1, 2, 1, 1, 1, 0};
oid objid_sysuptime[] = {1, 3, 6, 1, 2, 1, 1, 3, 0};
oid objid_snmptrap[] = {1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0};

static int status;

void init_ovsdb_snmp_notifications(const struct ovsdb_idl *idl) {
}

static oid objid_lldpStatsRemTablesInserts[] = {1, 0, 8802, 1, 1, 2, 1, 2, 2};
static oid objid_lldpStatsRemTablesDeletes[] = {1, 0, 8802, 1, 1, 2, 1, 2, 3};
static oid objid_lldpStatsRemTablesDrops[] = {1, 0, 8802, 1, 1, 2, 1, 2, 4};
static oid objid_lldpStatsRemTablesAgeouts[] = {1, 0, 8802, 1, 1, 2, 1, 2, 5};

void send_lldpRemTablesChange(const struct ovsdb_idl *idl, const char *lldpStatsRemTablesInserts_value,
                              const char *lldpStatsRemTablesDeletes_value,
                              const char *lldpStatsRemTablesDrops_value,
                              const char *lldpStatsRemTablesAgeouts_value) {
    netsnmp_session session, *ss;
    netsnmp_pdu *pdu, *response;
    int inform = 0;
    SOCK_STARTUP;
    snmp_sess_init(&session);
    u_char comm[] = "public";
    session.community = comm;
    session.community_len = strlen((char*)session.community);
    session.version = SNMP_VERSION_2c;
    pdu = snmp_pdu_create(SNMP_MSG_TRAP2);
    if (pdu == NULL) {
      fprintf(stderr, "Failed to create notification PDUn");
      SOCK_CLEANUP;
      return;
    }

    session.peername = "localhost";
    ss = snmp_add(&session,
                  netsnmp_transport_open_client("snmptrap", session.peername),
                  NULL, NULL);
    if (ss == NULL) {
      snmp_sess_perror("snmptrap", &session);
      return;
    }

    long sysuptime;
    char csysuptime[20];

    sysuptime = get_uptime();
    sprintf(csysuptime, "%ld", sysuptime);
    snmp_add_var(pdu, objid_sysuptime, sizeof(objid_sysuptime) / sizeof(oid),
                 't', csysuptime);
    if (snmp_add_var(pdu, objid_snmptrap, sizeof(objid_snmptrap) / sizeof(oid),
                     'o', "1.0.8802.1.1.2.0.0.1") != 0) {
      SOCK_CLEANUP;
      return;
    }

    if (snmp_add_var(pdu, objid_lldpStatsRemTablesInserts,
                     sizeof(objid_lldpStatsRemTablesInserts) / sizeof(oid), 'i',
                     lldpStatsRemTablesInserts_value) != 0) {
      SOCK_CLEANUP;
      return;
    }

    if (snmp_add_var(pdu, objid_lldpStatsRemTablesDeletes,
                     sizeof(objid_lldpStatsRemTablesDeletes) / sizeof(oid), 'i',
                     lldpStatsRemTablesDeletes_value) != 0) {
      SOCK_CLEANUP;
      return;
    }

    if (snmp_add_var(pdu, objid_lldpStatsRemTablesDrops,
                     sizeof(objid_lldpStatsRemTablesDrops) / sizeof(oid), 'i',
                     lldpStatsRemTablesDrops_value) != 0) {
      SOCK_CLEANUP;
      return;
    }

    if (snmp_add_var(pdu, objid_lldpStatsRemTablesAgeouts,
                     sizeof(objid_lldpStatsRemTablesAgeouts) / sizeof(oid), 'i',
                     lldpStatsRemTablesAgeouts_value) != 0) {
      SOCK_CLEANUP;
      return;
    }

    if (inform) {
      status = snmp_synch_response(ss, pdu, &response);
    } else {
      status = snmp_send(ss, pdu) == 0;
    }
    if (status) {
      snmp_sess_perror(inform ? "snmpinform" : "snmptrap", ss);
      if (!inform) {
        snmp_free_pdu(pdu);
      }
    } else if (inform) {
      snmp_free_pdu(response);
    }

    snmp_close(ss);

    snmp_shutdown("snmpapp");
    SOCK_CLEANUP;
}

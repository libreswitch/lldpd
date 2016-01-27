//
// Net-SNMP MIB module LLDP-MIB (http://pysnmp.sf.net)
// ASN.1 source file://\usr\share\snmp\mibs\Avi\LLDP-MIB.txt
// Produced by pysmi-0.0.6 at Tue Jan 26 18:26:26 2016
// On host ? platform ? version ? by user ?
// Using Python version 2.7.11 (v2.7.11:6d1b6a68f775, Dec  5 2015, 20:32:19)
// [MSC v.1500 32 bit (Intel)]
//
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "LLDP_MIB_custom.h"
#include "LLDP_MIB_scalars.h"
#include "LLDP_MIB_scalars_ovsdb_get.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
static long netsnmp_lldpMessageTxInterval;
int handler_lldpMessageTxInterval(netsnmp_mib_handler *handler,
                                  netsnmp_handler_registration *reginfo,
                                  netsnmp_agent_request_info *reqinfo,
                                  netsnmp_request_info *requests);

void init_lldpMessageTxInterval(void) {
  const oid lldpMessageTxInterval_oid[] = {1, 0, 8802, 1, 1, 2, 1, 1, 1};
  netsnmp_register_scalar(netsnmp_create_handler_registration(
      "lldpMessageTxInterval", handler_lldpMessageTxInterval,
      lldpMessageTxInterval_oid, OID_LENGTH(lldpMessageTxInterval_oid),
      HANDLER_CAN_RWRITE));

  ovsdb_idl_add_column(idl, &ovsrec_system_col_other_config);
}

int handler_lldpMessageTxInterval(netsnmp_mib_handler *handler,
                                  netsnmp_handler_registration *reginfo,
                                  netsnmp_agent_request_info *reqinfo,
                                  netsnmp_request_info *requests) {
  if (reqinfo->mode == MODE_GET) {
    const struct ovsrec_system *system_row = ovsrec_system_first(idl);
    ovsdb_get_lldpMessageTxInterval(idl, system_row,
                                    &netsnmp_lldpMessageTxInterval);
    snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                             &netsnmp_lldpMessageTxInterval,
                             sizeof(netsnmp_lldpMessageTxInterval));
  }
  return SNMP_ERR_NOERROR;
}

static long netsnmp_lldpMessageTxHoldMultiplier;
int handler_lldpMessageTxHoldMultiplier(netsnmp_mib_handler *handler,
                                        netsnmp_handler_registration *reginfo,
                                        netsnmp_agent_request_info *reqinfo,
                                        netsnmp_request_info *requests);

void init_lldpMessageTxHoldMultiplier(void) {
  const oid lldpMessageTxHoldMultiplier_oid[] = {1, 0, 8802, 1, 1, 2, 1, 1, 2};
  netsnmp_register_scalar(netsnmp_create_handler_registration(
      "lldpMessageTxHoldMultiplier", handler_lldpMessageTxHoldMultiplier,
      lldpMessageTxHoldMultiplier_oid,
      OID_LENGTH(lldpMessageTxHoldMultiplier_oid), HANDLER_CAN_RWRITE));

  ovsdb_idl_add_column(idl, &ovsrec_system_col_other_config);
}

int handler_lldpMessageTxHoldMultiplier(netsnmp_mib_handler *handler,
                                        netsnmp_handler_registration *reginfo,
                                        netsnmp_agent_request_info *reqinfo,
                                        netsnmp_request_info *requests) {
  if (reqinfo->mode == MODE_GET) {
    const struct ovsrec_system *system_row = ovsrec_system_first(idl);
    ovsdb_get_lldpMessageTxHoldMultiplier(idl, system_row,
                                          &netsnmp_lldpMessageTxHoldMultiplier);
    snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                             &netsnmp_lldpMessageTxHoldMultiplier,
                             sizeof(netsnmp_lldpMessageTxHoldMultiplier));
  }
  return SNMP_ERR_NOERROR;
}

static long netsnmp_lldpReinitDelay;
int handler_lldpReinitDelay(netsnmp_mib_handler *handler,
                            netsnmp_handler_registration *reginfo,
                            netsnmp_agent_request_info *reqinfo,
                            netsnmp_request_info *requests);

void init_lldpReinitDelay(void) {
  const oid lldpReinitDelay_oid[] = {1, 0, 8802, 1, 1, 2, 1, 1, 3};
  netsnmp_register_scalar(netsnmp_create_handler_registration(
      "lldpReinitDelay", handler_lldpReinitDelay, lldpReinitDelay_oid,
      OID_LENGTH(lldpReinitDelay_oid), HANDLER_CAN_RWRITE));
}

int handler_lldpReinitDelay(netsnmp_mib_handler *handler,
                            netsnmp_handler_registration *reginfo,
                            netsnmp_agent_request_info *reqinfo,
                            netsnmp_request_info *requests) {
  if (reqinfo->mode == MODE_GET) {
    ovsdb_get_lldpReinitDelay(idl, &netsnmp_lldpReinitDelay);
    snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                             &netsnmp_lldpReinitDelay,
                             sizeof(netsnmp_lldpReinitDelay));
  }
  return SNMP_ERR_NOERROR;
}

static long netsnmp_lldpTxDelay;
int handler_lldpTxDelay(netsnmp_mib_handler *handler,
                        netsnmp_handler_registration *reginfo,
                        netsnmp_agent_request_info *reqinfo,
                        netsnmp_request_info *requests);

void init_lldpTxDelay(void) {
  const oid lldpTxDelay_oid[] = {1, 0, 8802, 1, 1, 2, 1, 1, 4};
  netsnmp_register_scalar(netsnmp_create_handler_registration(
      "lldpTxDelay", handler_lldpTxDelay, lldpTxDelay_oid,
      OID_LENGTH(lldpTxDelay_oid), HANDLER_CAN_RWRITE));
}

int handler_lldpTxDelay(netsnmp_mib_handler *handler,
                        netsnmp_handler_registration *reginfo,
                        netsnmp_agent_request_info *reqinfo,
                        netsnmp_request_info *requests) {
  if (reqinfo->mode == MODE_GET) {
    ovsdb_get_lldpTxDelay(idl, &netsnmp_lldpTxDelay);
    snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                             &netsnmp_lldpTxDelay, sizeof(netsnmp_lldpTxDelay));
  }
  return SNMP_ERR_NOERROR;
}

static long netsnmp_lldpNotificationInterval;
int handler_lldpNotificationInterval(netsnmp_mib_handler *handler,
                                     netsnmp_handler_registration *reginfo,
                                     netsnmp_agent_request_info *reqinfo,
                                     netsnmp_request_info *requests);

void init_lldpNotificationInterval(void) {
  const oid lldpNotificationInterval_oid[] = {1, 0, 8802, 1, 1, 2, 1, 1, 5};
  netsnmp_register_scalar(netsnmp_create_handler_registration(
      "lldpNotificationInterval", handler_lldpNotificationInterval,
      lldpNotificationInterval_oid, OID_LENGTH(lldpNotificationInterval_oid),
      HANDLER_CAN_RWRITE));
}

int handler_lldpNotificationInterval(netsnmp_mib_handler *handler,
                                     netsnmp_handler_registration *reginfo,
                                     netsnmp_agent_request_info *reqinfo,
                                     netsnmp_request_info *requests) {
  if (reqinfo->mode == MODE_GET) {
    ovsdb_get_lldpNotificationInterval(idl, &netsnmp_lldpNotificationInterval);
    snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                             &netsnmp_lldpNotificationInterval,
                             sizeof(netsnmp_lldpNotificationInterval));
  }
  return SNMP_ERR_NOERROR;
}

static long netsnmp_lldpStatsRemTablesLastChangeTime;
int handler_lldpStatsRemTablesLastChangeTime(
    netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
    netsnmp_agent_request_info *reqinfo, netsnmp_request_info *requests);

void init_lldpStatsRemTablesLastChangeTime(void) {
  const oid lldpStatsRemTablesLastChangeTime_oid[] = {1, 0, 8802, 1, 1,
                                                      2, 1, 2,    1};
  netsnmp_register_scalar(netsnmp_create_handler_registration(
      "lldpStatsRemTablesLastChangeTime",
      handler_lldpStatsRemTablesLastChangeTime,
      lldpStatsRemTablesLastChangeTime_oid,
      OID_LENGTH(lldpStatsRemTablesLastChangeTime_oid), HANDLER_CAN_RWRITE));
}

int handler_lldpStatsRemTablesLastChangeTime(
    netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
    netsnmp_agent_request_info *reqinfo, netsnmp_request_info *requests) {
  if (reqinfo->mode == MODE_GET) {
    ovsdb_get_lldpStatsRemTablesLastChangeTime(
        idl, &netsnmp_lldpStatsRemTablesLastChangeTime);
    snmp_set_var_typed_value(requests->requestvb, ASN_TIMETICKS,
                             &netsnmp_lldpStatsRemTablesLastChangeTime,
                             sizeof(netsnmp_lldpStatsRemTablesLastChangeTime));
  }
  return SNMP_ERR_NOERROR;
}

static long netsnmp_lldpStatsRemTablesInserts;
int handler_lldpStatsRemTablesInserts(netsnmp_mib_handler *handler,
                                      netsnmp_handler_registration *reginfo,
                                      netsnmp_agent_request_info *reqinfo,
                                      netsnmp_request_info *requests);

void init_lldpStatsRemTablesInserts(void) {
  const oid lldpStatsRemTablesInserts_oid[] = {1, 0, 8802, 1, 1, 2, 1, 2, 2};
  netsnmp_register_scalar(netsnmp_create_handler_registration(
      "lldpStatsRemTablesInserts", handler_lldpStatsRemTablesInserts,
      lldpStatsRemTablesInserts_oid, OID_LENGTH(lldpStatsRemTablesInserts_oid),
      HANDLER_CAN_RWRITE));

  ovsdb_idl_add_column(idl, &ovsrec_system_col_lldp_statistics);
}

int handler_lldpStatsRemTablesInserts(netsnmp_mib_handler *handler,
                                      netsnmp_handler_registration *reginfo,
                                      netsnmp_agent_request_info *reqinfo,
                                      netsnmp_request_info *requests) {
  if (reqinfo->mode == MODE_GET) {
    const struct ovsrec_system *system_row = ovsrec_system_first(idl);
    ovsdb_get_lldpStatsRemTablesInserts(idl, system_row,
                                        &netsnmp_lldpStatsRemTablesInserts);
    snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                             &netsnmp_lldpStatsRemTablesInserts,
                             sizeof(netsnmp_lldpStatsRemTablesInserts));
  }
  return SNMP_ERR_NOERROR;
}

static long netsnmp_lldpStatsRemTablesDeletes;
int handler_lldpStatsRemTablesDeletes(netsnmp_mib_handler *handler,
                                      netsnmp_handler_registration *reginfo,
                                      netsnmp_agent_request_info *reqinfo,
                                      netsnmp_request_info *requests);

void init_lldpStatsRemTablesDeletes(void) {
  const oid lldpStatsRemTablesDeletes_oid[] = {1, 0, 8802, 1, 1, 2, 1, 2, 3};
  netsnmp_register_scalar(netsnmp_create_handler_registration(
      "lldpStatsRemTablesDeletes", handler_lldpStatsRemTablesDeletes,
      lldpStatsRemTablesDeletes_oid, OID_LENGTH(lldpStatsRemTablesDeletes_oid),
      HANDLER_CAN_RWRITE));

  ovsdb_idl_add_column(idl, &ovsrec_system_col_lldp_statistics);
}

int handler_lldpStatsRemTablesDeletes(netsnmp_mib_handler *handler,
                                      netsnmp_handler_registration *reginfo,
                                      netsnmp_agent_request_info *reqinfo,
                                      netsnmp_request_info *requests) {
  if (reqinfo->mode == MODE_GET) {
    const struct ovsrec_system *system_row = ovsrec_system_first(idl);
    ovsdb_get_lldpStatsRemTablesDeletes(idl, system_row,
                                        &netsnmp_lldpStatsRemTablesDeletes);
    snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                             &netsnmp_lldpStatsRemTablesDeletes,
                             sizeof(netsnmp_lldpStatsRemTablesDeletes));
  }
  return SNMP_ERR_NOERROR;
}

static long netsnmp_lldpStatsRemTablesDrops;
int handler_lldpStatsRemTablesDrops(netsnmp_mib_handler *handler,
                                    netsnmp_handler_registration *reginfo,
                                    netsnmp_agent_request_info *reqinfo,
                                    netsnmp_request_info *requests);

void init_lldpStatsRemTablesDrops(void) {
  const oid lldpStatsRemTablesDrops_oid[] = {1, 0, 8802, 1, 1, 2, 1, 2, 4};
  netsnmp_register_scalar(netsnmp_create_handler_registration(
      "lldpStatsRemTablesDrops", handler_lldpStatsRemTablesDrops,
      lldpStatsRemTablesDrops_oid, OID_LENGTH(lldpStatsRemTablesDrops_oid),
      HANDLER_CAN_RWRITE));

  ovsdb_idl_add_column(idl, &ovsrec_system_col_lldp_statistics);
}

int handler_lldpStatsRemTablesDrops(netsnmp_mib_handler *handler,
                                    netsnmp_handler_registration *reginfo,
                                    netsnmp_agent_request_info *reqinfo,
                                    netsnmp_request_info *requests) {
  if (reqinfo->mode == MODE_GET) {
    const struct ovsrec_system *system_row = ovsrec_system_first(idl);
    ovsdb_get_lldpStatsRemTablesDrops(idl, system_row,
                                      &netsnmp_lldpStatsRemTablesDrops);
    snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                             &netsnmp_lldpStatsRemTablesDrops,
                             sizeof(netsnmp_lldpStatsRemTablesDrops));
  }
  return SNMP_ERR_NOERROR;
}

static long netsnmp_lldpStatsRemTablesAgeouts;
int handler_lldpStatsRemTablesAgeouts(netsnmp_mib_handler *handler,
                                      netsnmp_handler_registration *reginfo,
                                      netsnmp_agent_request_info *reqinfo,
                                      netsnmp_request_info *requests);

void init_lldpStatsRemTablesAgeouts(void) {
  const oid lldpStatsRemTablesAgeouts_oid[] = {1, 0, 8802, 1, 1, 2, 1, 2, 5};
  netsnmp_register_scalar(netsnmp_create_handler_registration(
      "lldpStatsRemTablesAgeouts", handler_lldpStatsRemTablesAgeouts,
      lldpStatsRemTablesAgeouts_oid, OID_LENGTH(lldpStatsRemTablesAgeouts_oid),
      HANDLER_CAN_RWRITE));

  ovsdb_idl_add_column(idl, &ovsrec_system_col_lldp_statistics);
}

int handler_lldpStatsRemTablesAgeouts(netsnmp_mib_handler *handler,
                                      netsnmp_handler_registration *reginfo,
                                      netsnmp_agent_request_info *reqinfo,
                                      netsnmp_request_info *requests) {
  if (reqinfo->mode == MODE_GET) {
    const struct ovsrec_system *system_row = ovsrec_system_first(idl);
    ovsdb_get_lldpStatsRemTablesAgeouts(idl, system_row,
                                        &netsnmp_lldpStatsRemTablesAgeouts);
    snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                             &netsnmp_lldpStatsRemTablesAgeouts,
                             sizeof(netsnmp_lldpStatsRemTablesAgeouts));
  }
  return SNMP_ERR_NOERROR;
}

static long netsnmp_lldpLocChassisIdSubtype;
int handler_lldpLocChassisIdSubtype(netsnmp_mib_handler *handler,
                                    netsnmp_handler_registration *reginfo,
                                    netsnmp_agent_request_info *reqinfo,
                                    netsnmp_request_info *requests);

void init_lldpLocChassisIdSubtype(void) {
  const oid lldpLocChassisIdSubtype_oid[] = {1, 0, 8802, 1, 1, 2, 1, 3, 1};
  netsnmp_register_scalar(netsnmp_create_handler_registration(
      "lldpLocChassisIdSubtype", handler_lldpLocChassisIdSubtype,
      lldpLocChassisIdSubtype_oid, OID_LENGTH(lldpLocChassisIdSubtype_oid),
      HANDLER_CAN_RWRITE));
}

int handler_lldpLocChassisIdSubtype(netsnmp_mib_handler *handler,
                                    netsnmp_handler_registration *reginfo,
                                    netsnmp_agent_request_info *reqinfo,
                                    netsnmp_request_info *requests) {
  if (reqinfo->mode == MODE_GET) {
    ovsdb_get_lldpLocChassisIdSubtype(idl, &netsnmp_lldpLocChassisIdSubtype);
    snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                             &netsnmp_lldpLocChassisIdSubtype,
                             sizeof(netsnmp_lldpLocChassisIdSubtype));
  }
  return SNMP_ERR_NOERROR;
}

static char netsnmp_lldpLocChassisId[256];
static size_t netsnmp_lldpLocChassisId_len = 0;
int handler_lldpLocChassisId(netsnmp_mib_handler *handler,
                             netsnmp_handler_registration *reginfo,
                             netsnmp_agent_request_info *reqinfo,
                             netsnmp_request_info *requests);

void init_lldpLocChassisId(void) {
  const oid lldpLocChassisId_oid[] = {1, 0, 8802, 1, 1, 2, 1, 3, 2};
  netsnmp_register_scalar(netsnmp_create_handler_registration(
      "lldpLocChassisId", handler_lldpLocChassisId, lldpLocChassisId_oid,
      OID_LENGTH(lldpLocChassisId_oid), HANDLER_CAN_RWRITE));
}

int handler_lldpLocChassisId(netsnmp_mib_handler *handler,
                             netsnmp_handler_registration *reginfo,
                             netsnmp_agent_request_info *reqinfo,
                             netsnmp_request_info *requests) {
  if (reqinfo->mode == MODE_GET) {
    ovsdb_get_lldpLocChassisId(idl, netsnmp_lldpLocChassisId,
                               &netsnmp_lldpLocChassisId_len);
    snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             &netsnmp_lldpLocChassisId,
                             netsnmp_lldpLocChassisId_len);
  }
  return SNMP_ERR_NOERROR;
}

static char netsnmp_lldpLocSysName[255];
static size_t netsnmp_lldpLocSysName_len = 0;
int handler_lldpLocSysName(netsnmp_mib_handler *handler,
                           netsnmp_handler_registration *reginfo,
                           netsnmp_agent_request_info *reqinfo,
                           netsnmp_request_info *requests);

void init_lldpLocSysName(void) {
  const oid lldpLocSysName_oid[] = {1, 0, 8802, 1, 1, 2, 1, 3, 3};
  netsnmp_register_scalar(netsnmp_create_handler_registration(
      "lldpLocSysName", handler_lldpLocSysName, lldpLocSysName_oid,
      OID_LENGTH(lldpLocSysName_oid), HANDLER_CAN_RWRITE));

  ovsdb_idl_add_column(idl, &ovsrec_system_col_hostname);
}

int handler_lldpLocSysName(netsnmp_mib_handler *handler,
                           netsnmp_handler_registration *reginfo,
                           netsnmp_agent_request_info *reqinfo,
                           netsnmp_request_info *requests) {
  if (reqinfo->mode == MODE_GET) {
    const struct ovsrec_system *system_row = ovsrec_system_first(idl);
    ovsdb_get_lldpLocSysName(idl, system_row, netsnmp_lldpLocSysName,
                             &netsnmp_lldpLocSysName_len);
    snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             &netsnmp_lldpLocSysName,
                             netsnmp_lldpLocSysName_len);
  }
  return SNMP_ERR_NOERROR;
}

static char netsnmp_lldpLocSysDesc[255];
static size_t netsnmp_lldpLocSysDesc_len = 0;
int handler_lldpLocSysDesc(netsnmp_mib_handler *handler,
                           netsnmp_handler_registration *reginfo,
                           netsnmp_agent_request_info *reqinfo,
                           netsnmp_request_info *requests);

void init_lldpLocSysDesc(void) {
  const oid lldpLocSysDesc_oid[] = {1, 0, 8802, 1, 1, 2, 1, 3, 4};
  netsnmp_register_scalar(netsnmp_create_handler_registration(
      "lldpLocSysDesc", handler_lldpLocSysDesc, lldpLocSysDesc_oid,
      OID_LENGTH(lldpLocSysDesc_oid), HANDLER_CAN_RWRITE));
}

int handler_lldpLocSysDesc(netsnmp_mib_handler *handler,
                           netsnmp_handler_registration *reginfo,
                           netsnmp_agent_request_info *reqinfo,
                           netsnmp_request_info *requests) {
  if (reqinfo->mode == MODE_GET) {
    ovsdb_get_lldpLocSysDesc(idl, netsnmp_lldpLocSysDesc,
                             &netsnmp_lldpLocSysDesc_len);
    snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             &netsnmp_lldpLocSysDesc,
                             netsnmp_lldpLocSysDesc_len);
  }
  return SNMP_ERR_NOERROR;
}

static u_long netsnmp_lldpLocSysCapSupported;
int handler_lldpLocSysCapSupported(netsnmp_mib_handler *handler,
                                   netsnmp_handler_registration *reginfo,
                                   netsnmp_agent_request_info *reqinfo,
                                   netsnmp_request_info *requests);

void init_lldpLocSysCapSupported(void) {
  const oid lldpLocSysCapSupported_oid[] = {1, 0, 8802, 1, 1, 2, 1, 3, 5};
  netsnmp_register_scalar(netsnmp_create_handler_registration(
      "lldpLocSysCapSupported", handler_lldpLocSysCapSupported,
      lldpLocSysCapSupported_oid, OID_LENGTH(lldpLocSysCapSupported_oid),
      HANDLER_CAN_RWRITE));
}

int handler_lldpLocSysCapSupported(netsnmp_mib_handler *handler,
                                   netsnmp_handler_registration *reginfo,
                                   netsnmp_agent_request_info *reqinfo,
                                   netsnmp_request_info *requests) {
  if (reqinfo->mode == MODE_GET) {
    ovsdb_get_lldpLocSysCapSupported(idl, &netsnmp_lldpLocSysCapSupported);
    snmp_set_var_typed_value(requests->requestvb, ASN_BIT8,
                             &netsnmp_lldpLocSysCapSupported,
                             sizeof(netsnmp_lldpLocSysCapSupported));
  }
  return SNMP_ERR_NOERROR;
}

static u_long netsnmp_lldpLocSysCapEnabled;
int handler_lldpLocSysCapEnabled(netsnmp_mib_handler *handler,
                                 netsnmp_handler_registration *reginfo,
                                 netsnmp_agent_request_info *reqinfo,
                                 netsnmp_request_info *requests);

void init_lldpLocSysCapEnabled(void) {
  const oid lldpLocSysCapEnabled_oid[] = {1, 0, 8802, 1, 1, 2, 1, 3, 6};
  netsnmp_register_scalar(netsnmp_create_handler_registration(
      "lldpLocSysCapEnabled", handler_lldpLocSysCapEnabled,
      lldpLocSysCapEnabled_oid, OID_LENGTH(lldpLocSysCapEnabled_oid),
      HANDLER_CAN_RWRITE));
}

int handler_lldpLocSysCapEnabled(netsnmp_mib_handler *handler,
                                 netsnmp_handler_registration *reginfo,
                                 netsnmp_agent_request_info *reqinfo,
                                 netsnmp_request_info *requests) {
  if (reqinfo->mode == MODE_GET) {
    ovsdb_get_lldpLocSysCapEnabled(idl, &netsnmp_lldpLocSysCapEnabled);
    snmp_set_var_typed_value(requests->requestvb, ASN_BIT8,
                             &netsnmp_lldpLocSysCapEnabled,
                             sizeof(netsnmp_lldpLocSysCapEnabled));
  }
  return SNMP_ERR_NOERROR;
}

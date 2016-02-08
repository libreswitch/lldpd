#ifndef LLDPPORTCONFIGTABLE_H
#define LLDPPORTCONFIGTABLE_H
#include <net-snmp/library/asn1.h>
#include "lldpPortConfigTable_oids.h"
#include "lldpPortConfigTable_enums.h"

void init_lldpPortConfigTable(void);
void shutdown_lldpPortConfigTable(void);

typedef netsnmp_data_list lldpPortConfigTable_registration;

typedef struct lldpPortConfigTable_data_s {
  long lldpPortConfigAdminStatus;
  long lldpPortConfigNotificationEnable;
  u_long lldpPortConfigTLVsTxEnable;
} lldpPortConfigTable_data;

typedef struct lldpPortConfigTable_mib_index_s {
  long lldpPortConfigPortNum;
} lldpPortConfigTable_mib_index;

typedef struct lldpPortConfigTable_rowreq_ctx_s {
  netsnmp_index oid_idx;
  oid oid_tmp[MAX_OID_LEN];
  lldpPortConfigTable_mib_index tbl_idx;
  lldpPortConfigTable_data data;
  u_int rowreq_flags;
  netsnmp_data_list *lldpPortConfigTable_data_list;
} lldpPortConfigTable_rowreq_ctx;

typedef struct lldpPortConfigTable_ref_rowreq_ctx_s {
  lldpPortConfigTable_rowreq_ctx *rowreq_ctx;
} lldpPortConfigTable_ref_rowreq_ctx;

int lldpPortConfigTable_pre_request(
    lldpPortConfigTable_registration *user_context);
int lldpPortConfigTable_post_request(
    lldpPortConfigTable_registration *user_context, int rc);
int lldpPortConfigTable_rowreq_ctx_init(
    lldpPortConfigTable_rowreq_ctx *rowreq_ctx, void *user_init_ctx);
void lldpPortConfigTable_rowreq_ctx_cleanup(
    lldpPortConfigTable_rowreq_ctx *rowreq_ctx);
lldpPortConfigTable_rowreq_ctx *lldpPortConfigTable_row_find_by_mib_index(
    lldpPortConfigTable_mib_index *mib_idx);
extern const oid lldpPortConfigTable_oid[];
extern const int lldpPortConfigTable_oid_size;
#include "lldpPortConfigTable_interface.h"
#include "lldpPortConfigTable_data_access.h"
#include "lldpPortConfigTable_data_get.h"
#include "lldpPortConfigTable_data_set.h"

#endif
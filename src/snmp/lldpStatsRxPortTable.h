#ifndef LLDPSTATSRXPORTTABLE_H
#define LLDPSTATSRXPORTTABLE_H
#include <net-snmp/library/asn1.h>
#include "lldpStatsRxPortTable_oids.h"
#include "lldpStatsRxPortTable_enums.h"

void init_lldpStatsRxPortTable(void);
void shutdown_lldpStatsRxPortTable(void);

typedef netsnmp_data_list lldpStatsRxPortTable_registration;

typedef struct lldpStatsRxPortTable_data_s {
    long lldpStatsRxPortFramesDiscardedTotal;
    long lldpStatsRxPortFramesErrors;
    long lldpStatsRxPortFramesTotal;
    long lldpStatsRxPortTLVsDiscardedTotal;
    long lldpStatsRxPortTLVsUnrecognizedTotal;
    long lldpStatsRxPortAgeoutsTotal;
} lldpStatsRxPortTable_data;

typedef struct lldpStatsRxPortTable_mib_index_s {
    long lldpStatsRxPortNum;
} lldpStatsRxPortTable_mib_index;

typedef struct lldpStatsRxPortTable_rowreq_ctx_s {
    netsnmp_index oid_idx;
    oid oid_tmp[MAX_OID_LEN];
    lldpStatsRxPortTable_mib_index tbl_idx;
    lldpStatsRxPortTable_data data;
    u_int rowreq_flags;
    netsnmp_data_list *lldpStatsRxPortTable_data_list;
} lldpStatsRxPortTable_rowreq_ctx;

typedef struct lldpStatsRxPortTable_ref_rowreq_ctx_s {
    lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx;
} lldpStatsRxPortTable_ref_rowreq_ctx;

int lldpStatsRxPortTable_pre_request(
    lldpStatsRxPortTable_registration *user_context);
int lldpStatsRxPortTable_post_request(
    lldpStatsRxPortTable_registration *user_context, int rc);
int lldpStatsRxPortTable_rowreq_ctx_init(
    lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx, void *user_init_ctx);
void lldpStatsRxPortTable_rowreq_ctx_cleanup(
    lldpStatsRxPortTable_rowreq_ctx *rowreq_ctx);
lldpStatsRxPortTable_rowreq_ctx *lldpStatsRxPortTable_row_find_by_mib_index(
    lldpStatsRxPortTable_mib_index *mib_idx);
extern const oid lldpStatsRxPortTable_oid[];
extern const int lldpStatsRxPortTable_oid_size;
#include "lldpStatsRxPortTable_interface.h"
#include "lldpStatsRxPortTable_data_access.h"
#include "lldpStatsRxPortTable_data_get.h"
#include "lldpStatsRxPortTable_data_set.h"

#endif
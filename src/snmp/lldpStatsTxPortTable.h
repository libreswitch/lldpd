#ifndef LLDPSTATSTXPORTTABLE_H
#define LLDPSTATSTXPORTTABLE_H
#include <net-snmp/library/asn1.h>
#include "lldpStatsTxPortTable_oids.h"
#include "lldpStatsTxPortTable_enums.h"

void init_lldpStatsTxPortTable(void);
void shutdown_lldpStatsTxPortTable(void);

typedef netsnmp_data_list lldpStatsTxPortTable_registration;

typedef struct lldpStatsTxPortTable_data_s {
    long lldpStatsTxPortFramesTotal;
} lldpStatsTxPortTable_data;

typedef struct lldpStatsTxPortTable_mib_index_s {
    long lldpStatsTxPortNum;
} lldpStatsTxPortTable_mib_index;

typedef struct lldpStatsTxPortTable_rowreq_ctx_s {
    netsnmp_index oid_idx;
    oid oid_tmp[MAX_OID_LEN];
    lldpStatsTxPortTable_mib_index tbl_idx;
    lldpStatsTxPortTable_data data;
    u_int rowreq_flags;
    netsnmp_data_list *lldpStatsTxPortTable_data_list;
} lldpStatsTxPortTable_rowreq_ctx;

typedef struct lldpStatsTxPortTable_ref_rowreq_ctx_s {
    lldpStatsTxPortTable_rowreq_ctx *rowreq_ctx;
} lldpStatsTxPortTable_ref_rowreq_ctx;

int lldpStatsTxPortTable_pre_request(
    lldpStatsTxPortTable_registration *user_context);
int lldpStatsTxPortTable_post_request(
    lldpStatsTxPortTable_registration *user_context, int rc);
int lldpStatsTxPortTable_rowreq_ctx_init(
    lldpStatsTxPortTable_rowreq_ctx *rowreq_ctx, void *user_init_ctx);
void lldpStatsTxPortTable_rowreq_ctx_cleanup(
    lldpStatsTxPortTable_rowreq_ctx *rowreq_ctx);
lldpStatsTxPortTable_rowreq_ctx *lldpStatsTxPortTable_row_find_by_mib_index(
    lldpStatsTxPortTable_mib_index *mib_idx);
extern const oid lldpStatsTxPortTable_oid[];
extern const int lldpStatsTxPortTable_oid_size;
#include "lldpStatsTxPortTable_interface.h"
#include "lldpStatsTxPortTable_data_access.h"
#include "lldpStatsTxPortTable_data_get.h"
#include "lldpStatsTxPortTable_data_set.h"

#endif
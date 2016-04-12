#ifndef LLDPCONFIGMANADDRTABLE_H
#define LLDPCONFIGMANADDRTABLE_H
#include <net-snmp/library/asn1.h>
#include "lldpConfigManAddrTable_oids.h"
#include "lldpConfigManAddrTable_enums.h"

void init_lldpConfigManAddrTable(void);
void shutdown_lldpConfigManAddrTable(void);

typedef netsnmp_data_list lldpConfigManAddrTable_registration;

typedef struct lldpConfigManAddrTable_data_s {
    char lldpConfigManAddrPortsTxEnable[512];
    size_t lldpConfigManAddrPortsTxEnable_len;
} lldpConfigManAddrTable_data;

typedef struct lldpConfigManAddrTable_mib_index_s {
    long lldpLocManAddrSubtype;
    char lldpLocManAddr[31];
    size_t lldpLocManAddr_len;
} lldpConfigManAddrTable_mib_index;

typedef struct lldpConfigManAddrTable_rowreq_ctx_s {
    netsnmp_index oid_idx;
    oid oid_tmp[MAX_OID_LEN];
    lldpConfigManAddrTable_mib_index tbl_idx;
    lldpConfigManAddrTable_data data;
    u_int rowreq_flags;
    netsnmp_data_list *lldpConfigManAddrTable_data_list;
} lldpConfigManAddrTable_rowreq_ctx;

typedef struct lldpConfigManAddrTable_ref_rowreq_ctx_s {
    lldpConfigManAddrTable_rowreq_ctx *rowreq_ctx;
} lldpConfigManAddrTable_ref_rowreq_ctx;

int lldpConfigManAddrTable_pre_request(
    lldpConfigManAddrTable_registration *user_context);
int lldpConfigManAddrTable_post_request(
    lldpConfigManAddrTable_registration *user_context, int rc);
int lldpConfigManAddrTable_rowreq_ctx_init(
    lldpConfigManAddrTable_rowreq_ctx *rowreq_ctx, void *user_init_ctx);
void lldpConfigManAddrTable_rowreq_ctx_cleanup(
    lldpConfigManAddrTable_rowreq_ctx *rowreq_ctx);
lldpConfigManAddrTable_rowreq_ctx *lldpConfigManAddrTable_row_find_by_mib_index(
    lldpConfigManAddrTable_mib_index *mib_idx);
extern const oid lldpConfigManAddrTable_oid[];
extern const int lldpConfigManAddrTable_oid_size;
#include "lldpConfigManAddrTable_interface.h"
#include "lldpConfigManAddrTable_data_access.h"
#include "lldpConfigManAddrTable_data_get.h"
#include "lldpConfigManAddrTable_data_set.h"

#endif
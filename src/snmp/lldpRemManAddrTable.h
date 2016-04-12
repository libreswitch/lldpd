#ifndef LLDPREMMANADDRTABLE_H
#define LLDPREMMANADDRTABLE_H
#include <net-snmp/library/asn1.h>
#include "lldpRemManAddrTable_oids.h"
#include "lldpRemManAddrTable_enums.h"

void init_lldpRemManAddrTable(void);
void shutdown_lldpRemManAddrTable(void);

typedef netsnmp_data_list lldpRemManAddrTable_registration;

typedef struct lldpRemManAddrTable_data_s {
    long lldpRemManAddrIfSubtype;
    long lldpRemManAddrIfId;
    oid lldpRemManAddrOID[MAX_OID_LEN];
    size_t lldpRemManAddrOID_len;
} lldpRemManAddrTable_data;

typedef struct lldpRemManAddrTable_mib_index_s {
    long lldpRemTimeMark;
    long lldpRemLocalPortNum;
    long lldpRemIndex;
    long lldpRemManAddrSubtype;
    char lldpRemManAddr[31];
    size_t lldpRemManAddr_len;
} lldpRemManAddrTable_mib_index;

typedef struct lldpRemManAddrTable_rowreq_ctx_s {
    netsnmp_index oid_idx;
    oid oid_tmp[MAX_OID_LEN];
    lldpRemManAddrTable_mib_index tbl_idx;
    lldpRemManAddrTable_data data;
    u_int rowreq_flags;
    netsnmp_data_list *lldpRemManAddrTable_data_list;
} lldpRemManAddrTable_rowreq_ctx;

typedef struct lldpRemManAddrTable_ref_rowreq_ctx_s {
    lldpRemManAddrTable_rowreq_ctx *rowreq_ctx;
} lldpRemManAddrTable_ref_rowreq_ctx;

int lldpRemManAddrTable_pre_request(
    lldpRemManAddrTable_registration *user_context);
int lldpRemManAddrTable_post_request(
    lldpRemManAddrTable_registration *user_context, int rc);
int lldpRemManAddrTable_rowreq_ctx_init(
    lldpRemManAddrTable_rowreq_ctx *rowreq_ctx, void *user_init_ctx);
void lldpRemManAddrTable_rowreq_ctx_cleanup(
    lldpRemManAddrTable_rowreq_ctx *rowreq_ctx);
lldpRemManAddrTable_rowreq_ctx *lldpRemManAddrTable_row_find_by_mib_index(
    lldpRemManAddrTable_mib_index *mib_idx);
extern const oid lldpRemManAddrTable_oid[];
extern const int lldpRemManAddrTable_oid_size;
#include "lldpRemManAddrTable_interface.h"
#include "lldpRemManAddrTable_data_access.h"
#include "lldpRemManAddrTable_data_get.h"
#include "lldpRemManAddrTable_data_set.h"

#endif
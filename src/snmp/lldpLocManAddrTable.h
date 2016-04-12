#ifndef LLDPLOCMANADDRTABLE_H
#define LLDPLOCMANADDRTABLE_H
#include <net-snmp/library/asn1.h>
#include "lldpLocManAddrTable_oids.h"
#include "lldpLocManAddrTable_enums.h"

void init_lldpLocManAddrTable(void);
void shutdown_lldpLocManAddrTable(void);

typedef netsnmp_data_list lldpLocManAddrTable_registration;

typedef struct lldpLocManAddrTable_data_s {
    long lldpLocManAddrLen;
    long lldpLocManAddrIfSubtype;
    long lldpLocManAddrIfId;
    oid lldpLocManAddrOID[MAX_OID_LEN];
    size_t lldpLocManAddrOID_len;
} lldpLocManAddrTable_data;

typedef struct lldpLocManAddrTable_mib_index_s {
    long lldpLocManAddrSubtype;
    char lldpLocManAddr[31];
    size_t lldpLocManAddr_len;
} lldpLocManAddrTable_mib_index;

typedef struct lldpLocManAddrTable_rowreq_ctx_s {
    netsnmp_index oid_idx;
    oid oid_tmp[MAX_OID_LEN];
    lldpLocManAddrTable_mib_index tbl_idx;
    lldpLocManAddrTable_data data;
    u_int rowreq_flags;
    netsnmp_data_list *lldpLocManAddrTable_data_list;
} lldpLocManAddrTable_rowreq_ctx;

typedef struct lldpLocManAddrTable_ref_rowreq_ctx_s {
    lldpLocManAddrTable_rowreq_ctx *rowreq_ctx;
} lldpLocManAddrTable_ref_rowreq_ctx;

int lldpLocManAddrTable_pre_request(
    lldpLocManAddrTable_registration *user_context);
int lldpLocManAddrTable_post_request(
    lldpLocManAddrTable_registration *user_context, int rc);
int lldpLocManAddrTable_rowreq_ctx_init(
    lldpLocManAddrTable_rowreq_ctx *rowreq_ctx, void *user_init_ctx);
void lldpLocManAddrTable_rowreq_ctx_cleanup(
    lldpLocManAddrTable_rowreq_ctx *rowreq_ctx);
lldpLocManAddrTable_rowreq_ctx *lldpLocManAddrTable_row_find_by_mib_index(
    lldpLocManAddrTable_mib_index *mib_idx);
extern const oid lldpLocManAddrTable_oid[];
extern const int lldpLocManAddrTable_oid_size;
#include "lldpLocManAddrTable_interface.h"
#include "lldpLocManAddrTable_data_access.h"
#include "lldpLocManAddrTable_data_get.h"
#include "lldpLocManAddrTable_data_set.h"

#endif
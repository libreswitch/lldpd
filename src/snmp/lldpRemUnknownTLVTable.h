#ifndef LLDPREMUNKNOWNTLVTABLE_H
#define LLDPREMUNKNOWNTLVTABLE_H
#include <net-snmp/library/asn1.h>
#include "lldpRemUnknownTLVTable_oids.h"
#include "lldpRemUnknownTLVTable_enums.h"

void init_lldpRemUnknownTLVTable(void);
void shutdown_lldpRemUnknownTLVTable(void);

typedef netsnmp_data_list lldpRemUnknownTLVTable_registration;

typedef struct lldpRemUnknownTLVTable_data_s {
    char lldpRemUnknownTLVInfo[511];
    size_t lldpRemUnknownTLVInfo_len;
} lldpRemUnknownTLVTable_data;

typedef struct lldpRemUnknownTLVTable_mib_index_s {
    long lldpRemTimeMark;
    long lldpRemLocalPortNum;
    long lldpRemIndex;
    long lldpRemUnknownTLVType;
} lldpRemUnknownTLVTable_mib_index;

typedef struct lldpRemUnknownTLVTable_rowreq_ctx_s {
    netsnmp_index oid_idx;
    oid oid_tmp[MAX_OID_LEN];
    lldpRemUnknownTLVTable_mib_index tbl_idx;
    lldpRemUnknownTLVTable_data data;
    u_int rowreq_flags;
    netsnmp_data_list *lldpRemUnknownTLVTable_data_list;
} lldpRemUnknownTLVTable_rowreq_ctx;

typedef struct lldpRemUnknownTLVTable_ref_rowreq_ctx_s {
    lldpRemUnknownTLVTable_rowreq_ctx *rowreq_ctx;
} lldpRemUnknownTLVTable_ref_rowreq_ctx;

int lldpRemUnknownTLVTable_pre_request(
    lldpRemUnknownTLVTable_registration *user_context);
int lldpRemUnknownTLVTable_post_request(
    lldpRemUnknownTLVTable_registration *user_context, int rc);
int lldpRemUnknownTLVTable_rowreq_ctx_init(
    lldpRemUnknownTLVTable_rowreq_ctx *rowreq_ctx, void *user_init_ctx);
void lldpRemUnknownTLVTable_rowreq_ctx_cleanup(
    lldpRemUnknownTLVTable_rowreq_ctx *rowreq_ctx);
lldpRemUnknownTLVTable_rowreq_ctx *lldpRemUnknownTLVTable_row_find_by_mib_index(
    lldpRemUnknownTLVTable_mib_index *mib_idx);
extern const oid lldpRemUnknownTLVTable_oid[];
extern const int lldpRemUnknownTLVTable_oid_size;
#include "lldpRemUnknownTLVTable_interface.h"
#include "lldpRemUnknownTLVTable_data_access.h"
#include "lldpRemUnknownTLVTable_data_get.h"
#include "lldpRemUnknownTLVTable_data_set.h"

#endif
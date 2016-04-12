#ifndef LLDPREMORGDEFINFOTABLE_H
#define LLDPREMORGDEFINFOTABLE_H
#include <net-snmp/library/asn1.h>
#include "lldpRemOrgDefInfoTable_oids.h"
#include "lldpRemOrgDefInfoTable_enums.h"

void init_lldpRemOrgDefInfoTable(void);
void shutdown_lldpRemOrgDefInfoTable(void);

typedef netsnmp_data_list lldpRemOrgDefInfoTable_registration;

typedef struct lldpRemOrgDefInfoTable_data_s {
    char lldpRemOrgDefInfo[507];
    size_t lldpRemOrgDefInfo_len;
} lldpRemOrgDefInfoTable_data;

typedef struct lldpRemOrgDefInfoTable_mib_index_s {
    long lldpRemTimeMark;
    long lldpRemLocalPortNum;
    long lldpRemIndex;
    char lldpRemOrgDefInfoOUI[3];
    size_t lldpRemOrgDefInfoOUI_len;
    long lldpRemOrgDefInfoSubtype;
    long lldpRemOrgDefInfoIndex;
} lldpRemOrgDefInfoTable_mib_index;

typedef struct lldpRemOrgDefInfoTable_rowreq_ctx_s {
    netsnmp_index oid_idx;
    oid oid_tmp[MAX_OID_LEN];
    lldpRemOrgDefInfoTable_mib_index tbl_idx;
    lldpRemOrgDefInfoTable_data data;
    u_int rowreq_flags;
    netsnmp_data_list *lldpRemOrgDefInfoTable_data_list;
} lldpRemOrgDefInfoTable_rowreq_ctx;

typedef struct lldpRemOrgDefInfoTable_ref_rowreq_ctx_s {
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx;
} lldpRemOrgDefInfoTable_ref_rowreq_ctx;

int lldpRemOrgDefInfoTable_pre_request(
    lldpRemOrgDefInfoTable_registration *user_context);
int lldpRemOrgDefInfoTable_post_request(
    lldpRemOrgDefInfoTable_registration *user_context, int rc);
int lldpRemOrgDefInfoTable_rowreq_ctx_init(
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx, void *user_init_ctx);
void lldpRemOrgDefInfoTable_rowreq_ctx_cleanup(
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx);
lldpRemOrgDefInfoTable_rowreq_ctx *lldpRemOrgDefInfoTable_row_find_by_mib_index(
    lldpRemOrgDefInfoTable_mib_index *mib_idx);
extern const oid lldpRemOrgDefInfoTable_oid[];
extern const int lldpRemOrgDefInfoTable_oid_size;
#include "lldpRemOrgDefInfoTable_interface.h"
#include "lldpRemOrgDefInfoTable_data_access.h"
#include "lldpRemOrgDefInfoTable_data_get.h"
#include "lldpRemOrgDefInfoTable_data_set.h"

#endif
#ifndef LLDPREMTABLE_H
#define LLDPREMTABLE_H
#include <net-snmp/library/asn1.h>
#include "lldpRemTable_oids.h"
#include "lldpRemTable_enums.h"

void init_lldpRemTable(void);
void shutdown_lldpRemTable(void);

typedef netsnmp_data_list lldpRemTable_registration;

typedef struct lldpRemTable_data_s {
    long lldpRemChassisIdSubtype;
    char lldpRemChassisId[255];
    size_t lldpRemChassisId_len;
    long lldpRemPortIdSubtype;
    char lldpRemPortId[255];
    size_t lldpRemPortId_len;
    char lldpRemPortDesc[255];
    size_t lldpRemPortDesc_len;
    char lldpRemSysName[255];
    size_t lldpRemSysName_len;
    char lldpRemSysDesc[255];
    size_t lldpRemSysDesc_len;
    u_long lldpRemSysCapSupported;
    u_long lldpRemSysCapEnabled;
} lldpRemTable_data;

typedef struct lldpRemTable_mib_index_s {
    long lldpRemTimeMark;
    long lldpRemLocalPortNum;
    long lldpRemIndex;
} lldpRemTable_mib_index;

typedef struct lldpRemTable_rowreq_ctx_s {
    netsnmp_index oid_idx;
    oid oid_tmp[MAX_OID_LEN];
    lldpRemTable_mib_index tbl_idx;
    lldpRemTable_data data;
    u_int rowreq_flags;
    netsnmp_data_list *lldpRemTable_data_list;
} lldpRemTable_rowreq_ctx;

typedef struct lldpRemTable_ref_rowreq_ctx_s {
    lldpRemTable_rowreq_ctx *rowreq_ctx;
} lldpRemTable_ref_rowreq_ctx;

int lldpRemTable_pre_request(lldpRemTable_registration *user_context);
int lldpRemTable_post_request(lldpRemTable_registration *user_context, int rc);
int lldpRemTable_rowreq_ctx_init(lldpRemTable_rowreq_ctx *rowreq_ctx,
                                 void *user_init_ctx);
void lldpRemTable_rowreq_ctx_cleanup(lldpRemTable_rowreq_ctx *rowreq_ctx);
lldpRemTable_rowreq_ctx *
lldpRemTable_row_find_by_mib_index(lldpRemTable_mib_index *mib_idx);
extern const oid lldpRemTable_oid[];
extern const int lldpRemTable_oid_size;
#include "lldpRemTable_interface.h"
#include "lldpRemTable_data_access.h"
#include "lldpRemTable_data_get.h"
#include "lldpRemTable_data_set.h"

#endif
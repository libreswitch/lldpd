#ifndef LLDPLOCPORTTABLE_H
#define LLDPLOCPORTTABLE_H
#include <net-snmp/library/asn1.h>
#include "lldpLocPortTable_oids.h"
#include "lldpLocPortTable_enums.h"

void init_lldpLocPortTable(void);
void shutdown_lldpLocPortTable(void);

typedef netsnmp_data_list lldpLocPortTable_registration;

typedef struct lldpLocPortTable_data_s {
    long lldpLocPortIdSubtype;
    char lldpLocPortId[255];
    size_t lldpLocPortId_len;
    char lldpLocPortDesc[255];
    size_t lldpLocPortDesc_len;
} lldpLocPortTable_data;

typedef struct lldpLocPortTable_mib_index_s {
    long lldpLocPortNum;
} lldpLocPortTable_mib_index;

typedef struct lldpLocPortTable_rowreq_ctx_s {
    netsnmp_index oid_idx;
    oid oid_tmp[MAX_OID_LEN];
    lldpLocPortTable_mib_index tbl_idx;
    lldpLocPortTable_data data;
    u_int rowreq_flags;
    netsnmp_data_list *lldpLocPortTable_data_list;
} lldpLocPortTable_rowreq_ctx;

typedef struct lldpLocPortTable_ref_rowreq_ctx_s {
    lldpLocPortTable_rowreq_ctx *rowreq_ctx;
} lldpLocPortTable_ref_rowreq_ctx;

int lldpLocPortTable_pre_request(lldpLocPortTable_registration *user_context);
int lldpLocPortTable_post_request(lldpLocPortTable_registration *user_context,
                                  int rc);
int lldpLocPortTable_rowreq_ctx_init(lldpLocPortTable_rowreq_ctx *rowreq_ctx,
                                     void *user_init_ctx);
void lldpLocPortTable_rowreq_ctx_cleanup(
    lldpLocPortTable_rowreq_ctx *rowreq_ctx);
lldpLocPortTable_rowreq_ctx *
lldpLocPortTable_row_find_by_mib_index(lldpLocPortTable_mib_index *mib_idx);
extern const oid lldpLocPortTable_oid[];
extern const int lldpLocPortTable_oid_size;
#include "lldpLocPortTable_interface.h"
#include "lldpLocPortTable_data_access.h"
#include "lldpLocPortTable_data_get.h"
#include "lldpLocPortTable_data_set.h"

#endif
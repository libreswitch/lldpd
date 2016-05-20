#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/table_container.h>
#include <net-snmp/library/container.h>
#include "lldpRemManAddrTable.h"
#include "lldpRemManAddrTable_interface.h"

netsnmp_feature_require(baby_steps) netsnmp_feature_require(row_merge)
    netsnmp_feature_require(check_all_requests_error)

        typedef struct lldpRemManAddrTable_interface_ctx_s {
    netsnmp_container *container;
    netsnmp_cache *cache;
    lldpRemManAddrTable_registration *user_ctx;
    netsnmp_table_registration_info tbl_info;
    netsnmp_baby_steps_access_methods access_multiplexer;
} lldpRemManAddrTable_interface_ctx;

static lldpRemManAddrTable_interface_ctx lldpRemManAddrTable_if_ctx;
static void
_lldpRemManAddrTable_container_init(lldpRemManAddrTable_interface_ctx *if_ctx);
static void _lldpRemManAddrTable_container_shutdown(
    lldpRemManAddrTable_interface_ctx *if_ctx);

netsnmp_container *lldpRemManAddrTable_container_get(void) {
    return lldpRemManAddrTable_if_ctx.container;
}

lldpRemManAddrTable_registration *lldpRemManAddrTable_registration_get(void) {
    return lldpRemManAddrTable_if_ctx.user_ctx;
}

lldpRemManAddrTable_registration *
lldpRemManAddrTable_registration_set(lldpRemManAddrTable_registration *newreg) {
    lldpRemManAddrTable_registration *old = lldpRemManAddrTable_if_ctx.user_ctx;
    lldpRemManAddrTable_if_ctx.user_ctx = newreg;
    return old;
}

int lldpRemManAddrTable_container_size(void) {
    return CONTAINER_SIZE(lldpRemManAddrTable_if_ctx.container);
}

static Netsnmp_Node_Handler _mfd_lldpRemManAddrTable_pre_request;
static Netsnmp_Node_Handler _mfd_lldpRemManAddrTable_post_request;
static Netsnmp_Node_Handler _mfd_lldpRemManAddrTable_object_lookup;
static Netsnmp_Node_Handler _mfd_lldpRemManAddrTable_get_values;

void _lldpRemManAddrTable_initialize_interface(
    lldpRemManAddrTable_registration *reg_ptr, u_long flags) {
    netsnmp_baby_steps_access_methods *access_multiplexer =
        &lldpRemManAddrTable_if_ctx.access_multiplexer;
    netsnmp_table_registration_info *tbl_info =
        &lldpRemManAddrTable_if_ctx.tbl_info;
    netsnmp_handler_registration *reginfo;
    netsnmp_mib_handler *handler;
    int mfd_modes = 0;

    DEBUGMSGTL(("internal:lldpRemManAddrTable:_lldpRemManAddrTable_initialize_"
                "interface",
                "called\n"));

    netsnmp_table_helper_add_indexes(tbl_info, ASN_TIMETICKS, ASN_INTEGER,
                                     ASN_INTEGER, ASN_INTEGER, ASN_OCTET_STR,
                                     0);

    tbl_info->min_column = LLDPREMMANADDRTABLE_MIN_COL;
    tbl_info->max_column = LLDPREMMANADDRTABLE_MAX_COL;
    lldpRemManAddrTable_if_ctx.user_ctx = reg_ptr;
    lldpRemManAddrTable_init_data(reg_ptr);
    _lldpRemManAddrTable_container_init(&lldpRemManAddrTable_if_ctx);
    if (NULL == lldpRemManAddrTable_if_ctx.container) {
        snmp_log(LOG_ERR,
                 "could not initialize container for lldpRemManAddrTable\n");
        return;
    }

    access_multiplexer->object_lookup = _mfd_lldpRemManAddrTable_object_lookup;
    access_multiplexer->get_values = _mfd_lldpRemManAddrTable_get_values;

    access_multiplexer->pre_request = _mfd_lldpRemManAddrTable_pre_request;
    access_multiplexer->post_request = _mfd_lldpRemManAddrTable_post_request;

    DEBUGMSGTL(
        ("lldpRemManAddrTable:init_lldpRemManAddrTable",
         "Registering lldpRemManAddrTable as a mibs-for-dummies table.\n"));

    handler = netsnmp_baby_steps_access_multiplexer_get(access_multiplexer);
    reginfo = netsnmp_handler_registration_create(
        "lldpRemManAddrTable", handler, lldpRemManAddrTable_oid,
        lldpRemManAddrTable_oid_size,
        HANDLER_CAN_BABY_STEP | HANDLER_CAN_RONLY);

    if (NULL == reginfo) {
        snmp_log(LOG_ERR, "error registering table lldpRemManAddrTable\n");
        return;
    }

    reginfo->my_reg_void = &lldpRemManAddrTable_if_ctx;

    if (access_multiplexer->object_lookup)
        mfd_modes |= BABY_STEP_OBJECT_LOOKUP;
    if (access_multiplexer->pre_request)
        mfd_modes |= BABY_STEP_PRE_REQUEST;
    if (access_multiplexer->post_request)
        mfd_modes |= BABY_STEP_POST_REQUEST;

    handler = netsnmp_baby_steps_handler_get(mfd_modes);
    netsnmp_inject_handler(reginfo, handler);

    handler = netsnmp_get_row_merge_handler(reginfo->rootoid_len + 2);
    netsnmp_inject_handler(reginfo, handler);

    handler = netsnmp_container_table_handler_get(
        tbl_info, lldpRemManAddrTable_if_ctx.container,
        TABLE_CONTAINER_KEY_NETSNMP_INDEX);
    netsnmp_inject_handler(reginfo, handler);

    if (NULL != lldpRemManAddrTable_if_ctx.cache) {
        handler = netsnmp_cache_handler_get(lldpRemManAddrTable_if_ctx.cache);
        netsnmp_inject_handler(reginfo, handler);
    }

    netsnmp_register_table(reginfo, tbl_info);
}

void _lldpRemManAddrTable_shutdown_interface(
    lldpRemManAddrTable_registration *reg_ptr) {
    _lldpRemManAddrTable_container_shutdown(&lldpRemManAddrTable_if_ctx);
}

void lldpRemManAddrTable_valid_columns_set(netsnmp_column_info *vc) {
    lldpRemManAddrTable_if_ctx.tbl_info.valid_columns = vc;
}

int lldpRemManAddrTable_index_to_oid(netsnmp_index *oid_idx,
                                     lldpRemManAddrTable_mib_index *mib_idx) {
    int err = SNMP_ERR_NOERROR;
    netsnmp_variable_list var_lldpRemTimeMark;
    netsnmp_variable_list var_lldpRemLocalPortNum;
    netsnmp_variable_list var_lldpRemIndex;
    netsnmp_variable_list var_lldpRemManAddrSubtype;
    netsnmp_variable_list var_lldpRemManAddr;

    memset(&var_lldpRemTimeMark, 0x00, sizeof(var_lldpRemTimeMark));
    var_lldpRemTimeMark.type = ASN_TIMETICKS;
    var_lldpRemTimeMark.next_variable = &var_lldpRemLocalPortNum;

    memset(&var_lldpRemLocalPortNum, 0x00, sizeof(var_lldpRemLocalPortNum));
    var_lldpRemLocalPortNum.type = ASN_INTEGER;
    var_lldpRemLocalPortNum.next_variable = &var_lldpRemIndex;

    memset(&var_lldpRemIndex, 0x00, sizeof(var_lldpRemIndex));
    var_lldpRemIndex.type = ASN_INTEGER;
    var_lldpRemIndex.next_variable = &var_lldpRemManAddrSubtype;

    memset(&var_lldpRemManAddrSubtype, 0x00, sizeof(var_lldpRemManAddrSubtype));
    var_lldpRemManAddrSubtype.type = ASN_INTEGER;
    var_lldpRemManAddrSubtype.next_variable = &var_lldpRemManAddr;

    memset(&var_lldpRemManAddr, 0x00, sizeof(var_lldpRemManAddr));
    var_lldpRemManAddr.type = ASN_OCTET_STR;
    var_lldpRemManAddr.next_variable = NULL;

    DEBUGMSGTL(("verbose:lldpRemManAddrTable:lldpRemManAddrTable_index_to_oid",
                "called\n"));

    snmp_set_var_value(&var_lldpRemTimeMark, &mib_idx->lldpRemTimeMark,
                       sizeof(mib_idx->lldpRemTimeMark));
    snmp_set_var_value(&var_lldpRemLocalPortNum, &mib_idx->lldpRemLocalPortNum,
                       sizeof(mib_idx->lldpRemLocalPortNum));
    snmp_set_var_value(&var_lldpRemIndex, &mib_idx->lldpRemIndex,
                       sizeof(mib_idx->lldpRemIndex));
    snmp_set_var_value(&var_lldpRemManAddrSubtype,
                       &mib_idx->lldpRemManAddrSubtype,
                       sizeof(mib_idx->lldpRemManAddrSubtype));
    snmp_set_var_value(&var_lldpRemManAddr, &mib_idx->lldpRemManAddr,
                       mib_idx->lldpRemManAddr_len *
                           sizeof(mib_idx->lldpRemManAddr[0]));
    err = build_oid_noalloc(oid_idx->oids, oid_idx->len, &oid_idx->len, NULL, 0,
                            &var_lldpRemTimeMark);
    if (err) {
        snmp_log(LOG_ERR, "error %d converting index to oid\n", err);
    }

    snmp_reset_var_buffers(&var_lldpRemTimeMark);
    return err;
}

int lldpRemManAddrTable_index_from_oid(netsnmp_index *oid_idx,
                                       lldpRemManAddrTable_mib_index *mib_idx) {
    int err = SNMP_ERR_NOERROR;
    netsnmp_variable_list var_lldpRemTimeMark;
    netsnmp_variable_list var_lldpRemLocalPortNum;
    netsnmp_variable_list var_lldpRemIndex;
    netsnmp_variable_list var_lldpRemManAddrSubtype;
    netsnmp_variable_list var_lldpRemManAddr;

    memset(&var_lldpRemTimeMark, 0x00, sizeof(var_lldpRemTimeMark));
    var_lldpRemTimeMark.type = ASN_TIMETICKS;
    var_lldpRemTimeMark.next_variable = &var_lldpRemLocalPortNum;

    memset(&var_lldpRemLocalPortNum, 0x00, sizeof(var_lldpRemLocalPortNum));
    var_lldpRemLocalPortNum.type = ASN_INTEGER;
    var_lldpRemLocalPortNum.next_variable = &var_lldpRemIndex;

    memset(&var_lldpRemIndex, 0x00, sizeof(var_lldpRemIndex));
    var_lldpRemIndex.type = ASN_INTEGER;
    var_lldpRemIndex.next_variable = &var_lldpRemManAddrSubtype;

    memset(&var_lldpRemManAddrSubtype, 0x00, sizeof(var_lldpRemManAddrSubtype));
    var_lldpRemManAddrSubtype.type = ASN_INTEGER;
    var_lldpRemManAddrSubtype.next_variable = &var_lldpRemManAddr;

    memset(&var_lldpRemManAddr, 0x00, sizeof(var_lldpRemManAddr));
    var_lldpRemManAddr.type = ASN_OCTET_STR;
    var_lldpRemManAddr.next_variable = NULL;

    DEBUGMSGTL(
        ("verbose:lldpRemManAddrTable:lldpRemManAddrTable_index_from_oid",
         "called\n"));

    err = parse_oid_indexes(oid_idx->oids, oid_idx->len, &var_lldpRemTimeMark);
    if (err == SNMP_ERR_NOERROR) {
        mib_idx->lldpRemTimeMark = *((long *)var_lldpRemTimeMark.val.string);
        mib_idx->lldpRemLocalPortNum =
            *((long *)var_lldpRemLocalPortNum.val.string);
        mib_idx->lldpRemIndex = *((long *)var_lldpRemIndex.val.string);
        mib_idx->lldpRemManAddrSubtype =
            *((long *)var_lldpRemManAddrSubtype.val.string);
        if (var_lldpRemManAddr.val_len > sizeof(mib_idx->lldpRemManAddr)) {
            err = SNMP_ERR_GENERR;
        } else {
            memcpy(mib_idx->lldpRemManAddr, var_lldpRemManAddr.val.string,
                   var_lldpRemManAddr.val_len);
            mib_idx->lldpRemManAddr_len =
                var_lldpRemManAddr.val_len / sizeof(mib_idx->lldpRemManAddr[0]);
        }
    }

    snmp_reset_var_buffers(&var_lldpRemTimeMark);
    return err;
}

lldpRemManAddrTable_rowreq_ctx *
lldpRemManAddrTable_allocate_rowreq_ctx(void *user_init_ctx) {
    lldpRemManAddrTable_rowreq_ctx *rowreq_ctx =
        SNMP_MALLOC_TYPEDEF(lldpRemManAddrTable_rowreq_ctx);

    DEBUGMSGTL(
        ("internal:lldpRemManAddrTable:lldpRemManAddrTable_allocate_rowreq_ctx",
         "called\n"));

    if (NULL == rowreq_ctx) {
        snmp_log(LOG_ERR, "Could not allocate memory for a "
                          "lldpRemManAddrTable_rowreq_ctx.\n");
        return NULL;
    }

    rowreq_ctx->oid_idx.oids = rowreq_ctx->oid_tmp;
    rowreq_ctx->lldpRemManAddrTable_data_list = NULL;
    if (!(rowreq_ctx->rowreq_flags & MFD_ROW_DATA_FROM_USER)) {
        if (SNMPERR_SUCCESS !=
            lldpRemManAddrTable_rowreq_ctx_init(rowreq_ctx, user_init_ctx)) {
            lldpRemManAddrTable_release_rowreq_ctx(rowreq_ctx);
            rowreq_ctx = NULL;
        }
    }
    return rowreq_ctx;
}

void lldpRemManAddrTable_release_rowreq_ctx(
    lldpRemManAddrTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(
        ("internal:lldpRemManAddrTable:lldpRemManAddrTable_release_rowreq_ctx",
         "called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    lldpRemManAddrTable_rowreq_ctx_cleanup(rowreq_ctx);
    if (rowreq_ctx->oid_idx.oids != rowreq_ctx->oid_tmp) {
        free(rowreq_ctx->oid_idx.oids);
    }

    SNMP_FREE(rowreq_ctx);
}

static int _mfd_lldpRemManAddrTable_pre_request(
    netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
    netsnmp_agent_request_info *agtreq_info, netsnmp_request_info *requests) {
    int rc;
    DEBUGMSGTL(
        ("internal:lldpRemManAddrTable:_mfd_lldpRemManAddrTable_pre_request",
         "called\n"));

    if (1 != netsnmp_row_merge_status_first(reginfo, agtreq_info)) {
        DEBUGMSGTL(("internal:lldpRemManAddrTable",
                    "skipping additional pre_request\n"));
        return SNMP_ERR_NOERROR;
    }

    rc = lldpRemManAddrTable_pre_request(lldpRemManAddrTable_if_ctx.user_ctx);
    if (MFD_SUCCESS != rc) {
        DEBUGMSGTL(("lldpRemManAddrTable",
                    "error %d from lldpRemManAddrTable_pre_requests\n", rc));
        netsnmp_request_set_error_all(requests, SNMP_VALIDATE_ERR(rc));
    }

    return SNMP_ERR_NOERROR;
}

static int _mfd_lldpRemManAddrTable_post_request(
    netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
    netsnmp_agent_request_info *agtreq_info, netsnmp_request_info *requests) {
    lldpRemManAddrTable_rowreq_ctx *rowreq_ctx =
        (lldpRemManAddrTable_rowreq_ctx *)netsnmp_container_table_row_extract(
            requests);
    int rc, packet_rc;
    DEBUGMSGTL(
        ("internal:lldpRemManAddrTable:_mfd_lldpRemManAddrTable_post_request",
         "called\n"));

    if (rowreq_ctx && (rowreq_ctx->rowreq_flags & MFD_ROW_DELETED)) {
        lldpRemManAddrTable_release_rowreq_ctx(rowreq_ctx);
    }

    if (1 != netsnmp_row_merge_status_last(reginfo, agtreq_info)) {
        DEBUGMSGTL(("internal:lldpRemManAddrTable",
                    "waiting for last post_request\n"));
        return SNMP_ERR_NOERROR;
    }

    packet_rc = netsnmp_check_all_requests_error(agtreq_info->asp, 0);
    rc = lldpRemManAddrTable_post_request(lldpRemManAddrTable_if_ctx.user_ctx,
                                          packet_rc);
    if (MFD_SUCCESS != rc) {
        DEBUGMSGTL(("lldpRemManAddrTable",
                    "error %d from lldpRemManAddrTable_post_request\n", rc));
    }
    return SNMP_ERR_NOERROR;
}

static int _mfd_lldpRemManAddrTable_object_lookup(
    netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
    netsnmp_agent_request_info *agtreq_info, netsnmp_request_info *requests) {
    int rc = SNMP_ERR_NOERROR;
    lldpRemManAddrTable_rowreq_ctx *rowreq_ctx =
        (lldpRemManAddrTable_rowreq_ctx *)netsnmp_container_table_row_extract(
            requests);

    DEBUGMSGTL(
        ("internal:lldpRemManAddrTable:_mfd_lldpRemManAddrTable_object_lookup",
         "called\n"));

    if (NULL == rowreq_ctx) {
        rc = SNMP_ERR_NOCREATION;
    }

    if (MFD_SUCCESS != rc) {
        netsnmp_request_set_error_all(requests, rc);
    } else {
        lldpRemManAddrTable_row_prep(rowreq_ctx);
    }

    return SNMP_VALIDATE_ERR(rc);
}

NETSNMP_STATIC_INLINE int
_lldpRemManAddrTable_get_column(lldpRemManAddrTable_rowreq_ctx *rowreq_ctx,
                                netsnmp_variable_list *var, int column) {
    int rc = SNMPERR_SUCCESS;

    DEBUGMSGTL(
        ("internal:lldpRemManAddrTable:_mfd_lldpRemManAddrTable_get_column",
         "called for %d\n", column));

    netsnmp_assert(NULL != rowreq_ctx);

    switch (column) {
    case COLUMN_LLDPREMMANADDRIFSUBTYPE: {
        var->type = ASN_INTEGER;
        var->val_len = sizeof(long);
        rc = lldpRemManAddrIfSubtype_get(rowreq_ctx, (long *)var->val.string);
    } break;
    case COLUMN_LLDPREMMANADDRIFID: {
        var->type = ASN_INTEGER;
        var->val_len = sizeof(long);
        rc = lldpRemManAddrIfId_get(rowreq_ctx, (long *)var->val.string);
    } break;
    case COLUMN_LLDPREMMANADDROID: {
        var->type = ASN_OBJECT_ID;
        rc = lldpRemManAddrOID_get(rowreq_ctx, (oid **)&var->val.string,
                                   &var->val_len);
    } break;
    default:
        if (LLDPREMMANADDRTABLE_MIN_COL <= column &&
            column <= LLDPREMMANADDRTABLE_MAX_COL) {
            DEBUGMSGTL(("internal:lldpRemManAddrTable:_mfd_lldpRemManAddrTable_"
                        "get_column",
                        "assume column %d is reserved\n", column));
            rc = MFD_SKIP;
        } else {
            snmp_log(LOG_ERR,
                     "unknown column %d in _lldpRemManAddrTable_get_column\n",
                     column);
        }
        break;
    }

    return rc;
}

int _mfd_lldpRemManAddrTable_get_values(netsnmp_mib_handler *handler,
                                        netsnmp_handler_registration *reginfo,
                                        netsnmp_agent_request_info *agtreq_info,
                                        netsnmp_request_info *requests) {
    lldpRemManAddrTable_rowreq_ctx *rowreq_ctx =
        (lldpRemManAddrTable_rowreq_ctx *)netsnmp_container_table_row_extract(
            requests);
    netsnmp_table_request_info *tri;
    u_char *old_string;
    void (*dataFreeHook)(void *);
    int rc;

    DEBUGMSGTL(
        ("internal:lldpRemManAddrTable:_mfd_lldpRemManAddrTable_get_values",
         "called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    for (; requests; requests = requests->next) {
        old_string = requests->requestvb->val.string;
        dataFreeHook = requests->requestvb->dataFreeHook;
        if (NULL == requests->requestvb->val.string) {
            requests->requestvb->val.string = requests->requestvb->buf;
            requests->requestvb->val_len = sizeof(requests->requestvb->buf);
        } else if (requests->requestvb->buf ==
                   requests->requestvb->val.string) {
            if (requests->requestvb->val_len !=
                sizeof(requests->requestvb->buf)) {
                requests->requestvb->val_len = sizeof(requests->requestvb->buf);
            }
        }

        tri = netsnmp_extract_table_info(requests);
        if (NULL == tri) {
            continue;
        }
        rc = _lldpRemManAddrTable_get_column(rowreq_ctx, requests->requestvb,
                                             tri->colnum);
        if (rc) {
            if (MFD_SKIP == rc) {
                requests->requestvb->type = SNMP_NOSUCHINSTANCE;
                rc = SNMP_ERR_NOERROR;
            }
        } else if (NULL == requests->requestvb->val.string) {
            snmp_log(LOG_ERR, "NULL varbind data pointer!\n");
            rc = SNMP_ERR_GENERR;
        }
        if (rc) {
            netsnmp_request_set_error(requests, SNMP_VALIDATE_ERR(rc));
        }

        if (old_string && (old_string != requests->requestvb->buf) &&
            (requests->requestvb->val.string != old_string)) {
            if (dataFreeHook) {
                (*dataFreeHook)(old_string);
            } else {
                free(old_string);
            }
        }
    }

    return SNMP_ERR_NOERROR;
}

static void _container_free(netsnmp_container *container);

static int _cache_load(netsnmp_cache *cache, void *vmagic) {
    DEBUGMSGTL(("internal:lldpRemManAddrTable:_cache_load", "called\n"));

    if ((NULL == cache) || (NULL == cache->magic)) {
        snmp_log(LOG_ERR, "invalid cache for lldpRemManAddrTable_cache_load\n");
        return -1;
    }

    netsnmp_assert((0 == cache->valid) || (1 == cache->expired));

    return lldpRemManAddrTable_container_load(
        (netsnmp_container *)cache->magic);
}

static void _cache_free(netsnmp_cache *cache, void *magic) {
    netsnmp_container *container;
    DEBUGMSGTL(("internal:lldpRemManAddrTable:_cache_free", "called\n"));

    if ((NULL == cache) || (NULL == cache->magic)) {
        snmp_log(LOG_ERR, "invalid cache in lldpRemManAddrTable_cache_free\n");
        return;
    }

    container = (netsnmp_container *)cache->magic;
    _container_free(container);
}

static void _container_item_free(lldpRemManAddrTable_rowreq_ctx *rowreq_ctx,
                                 void *context) {
    DEBUGMSGTL(
        ("internal:lldpRemManAddrTable:_container_item_free", "called\n"));

    if (NULL == rowreq_ctx) {
        return;
    }
    lldpRemManAddrTable_release_rowreq_ctx(rowreq_ctx);
}

static void _container_free(netsnmp_container *container) {
    DEBUGMSGTL(("internal:lldpRemManAddrTable:_container_free", "called\n"));

    if (NULL == container) {
        snmp_log(LOG_ERR,
                 "invalid container in lldpRemManAddrTable_container_free\n");
        return;
    }
    lldpRemManAddrTable_container_free(container);
    CONTAINER_CLEAR(container,
                    (netsnmp_container_obj_func *)_container_item_free, NULL);
}

void _lldpRemManAddrTable_container_init(
    lldpRemManAddrTable_interface_ctx *if_ctx) {
    DEBUGMSGTL(
        ("internal:lldpRemManAddrTable:_lldpRemManAddrTable_container_init",
         "called\n"));

    if_ctx->cache = netsnmp_cache_create(30, _cache_load, _cache_free,
                                         lldpRemManAddrTable_oid,
                                         lldpRemManAddrTable_oid_size);

    if (NULL == if_ctx->cache) {
        snmp_log(LOG_ERR, "error creating cache for lldpRemManAddrTable\n");
        return;
    }

    if_ctx->cache->flags = NETSNMP_CACHE_DONT_INVALIDATE_ON_SET;
    lldpRemManAddrTable_container_init(&if_ctx->container, if_ctx->cache);
    if (NULL == if_ctx->container) {
        if_ctx->container =
            netsnmp_container_find("lldpRemManAddrTable:table_container");
    }
    if (NULL == if_ctx->container) {
        snmp_log(
            LOG_ERR,
            "error creating container in lldpRemManAddrTable_container_init\n");
        return;
    }

    if (NULL != if_ctx->cache) {
        if_ctx->cache->magic = (void *)if_ctx->container;
    }
}

void _lldpRemManAddrTable_container_shutdown(
    lldpRemManAddrTable_interface_ctx *if_ctx) {
    DEBUGMSGTL(
        ("internal:lldpRemManAddrTable:_lldpRemManAddrTable_container_shutdown",
         "called\n"));

    lldpRemManAddrTable_container_shutdown(if_ctx->container);
    _container_free(if_ctx->container);
}

lldpRemManAddrTable_rowreq_ctx *lldpRemManAddrTable_row_find_by_mib_index(
    lldpRemManAddrTable_mib_index *mib_idx) {
    lldpRemManAddrTable_rowreq_ctx *rowreq_ctx;
    oid oid_tmp[MAX_OID_LEN];
    netsnmp_index oid_idx;
    int rc;

    oid_idx.oids = oid_tmp;
    oid_idx.len = sizeof(oid_tmp) / sizeof(oid);

    rc = lldpRemManAddrTable_index_to_oid(&oid_idx, mib_idx);
    if (MFD_SUCCESS != rc) {
        return NULL;
    }

    rowreq_ctx = (lldpRemManAddrTable_rowreq_ctx *)CONTAINER_FIND(
        lldpRemManAddrTable_if_ctx.container, &oid_idx);

    return rowreq_ctx;
}

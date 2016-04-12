#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/table_container.h>
#include <net-snmp/library/container.h>
#include "lldpLocPortTable.h"
#include "lldpLocPortTable_interface.h"

netsnmp_feature_require(baby_steps) netsnmp_feature_require(row_merge)
    netsnmp_feature_require(check_all_requests_error)

        typedef struct lldpLocPortTable_interface_ctx_s {
    netsnmp_container *container;
    netsnmp_cache *cache;
    lldpLocPortTable_registration *user_ctx;
    netsnmp_table_registration_info tbl_info;
    netsnmp_baby_steps_access_methods access_multiplexer;
} lldpLocPortTable_interface_ctx;

static lldpLocPortTable_interface_ctx lldpLocPortTable_if_ctx;
static void
_lldpLocPortTable_container_init(lldpLocPortTable_interface_ctx *if_ctx);
static void
_lldpLocPortTable_container_shutdown(lldpLocPortTable_interface_ctx *if_ctx);

netsnmp_container *lldpLocPortTable_container_get(void) {
    return lldpLocPortTable_if_ctx.container;
}

lldpLocPortTable_registration *lldpLocPortTable_registration_get(void) {
    return lldpLocPortTable_if_ctx.user_ctx;
}

lldpLocPortTable_registration *
lldpLocPortTable_registration_set(lldpLocPortTable_registration *newreg) {
    lldpLocPortTable_registration *old = lldpLocPortTable_if_ctx.user_ctx;
    lldpLocPortTable_if_ctx.user_ctx = newreg;
    return old;
}

int lldpLocPortTable_container_size(void) {
    return CONTAINER_SIZE(lldpLocPortTable_if_ctx.container);
}

static Netsnmp_Node_Handler _mfd_lldpLocPortTable_pre_request;
static Netsnmp_Node_Handler _mfd_lldpLocPortTable_post_request;
static Netsnmp_Node_Handler _mfd_lldpLocPortTable_object_lookup;
static Netsnmp_Node_Handler _mfd_lldpLocPortTable_get_values;

void _lldpLocPortTable_initialize_interface(
    lldpLocPortTable_registration *reg_ptr, u_long flags) {
    netsnmp_baby_steps_access_methods *access_multiplexer =
        &lldpLocPortTable_if_ctx.access_multiplexer;
    netsnmp_table_registration_info *tbl_info =
        &lldpLocPortTable_if_ctx.tbl_info;
    netsnmp_handler_registration *reginfo;
    netsnmp_mib_handler *handler;
    int mfd_modes = 0;

    DEBUGMSGTL(
        ("internal:lldpLocPortTable:_lldpLocPortTable_initialize_interface",
         "called\n"));

    netsnmp_table_helper_add_indexes(tbl_info, ASN_INTEGER, 0);

    tbl_info->min_column = LLDPLOCPORTTABLE_MIN_COL;
    tbl_info->max_column = LLDPLOCPORTTABLE_MAX_COL;
    lldpLocPortTable_if_ctx.user_ctx = reg_ptr;
    lldpLocPortTable_init_data(reg_ptr);
    _lldpLocPortTable_container_init(&lldpLocPortTable_if_ctx);
    if (NULL == lldpLocPortTable_if_ctx.container) {
        snmp_log(LOG_ERR,
                 "could not initialize container for lldpLocPortTable\n");
        return;
    }

    access_multiplexer->object_lookup = _mfd_lldpLocPortTable_object_lookup;
    access_multiplexer->get_values = _mfd_lldpLocPortTable_get_values;

    access_multiplexer->pre_request = _mfd_lldpLocPortTable_pre_request;
    access_multiplexer->post_request = _mfd_lldpLocPortTable_post_request;

    DEBUGMSGTL(("lldpLocPortTable:init_lldpLocPortTable",
                "Registering lldpLocPortTable as a mibs-for-dummies table.\n"));

    handler = netsnmp_baby_steps_access_multiplexer_get(access_multiplexer);
    reginfo = netsnmp_handler_registration_create(
        "lldpLocPortTable", handler, lldpLocPortTable_oid,
        lldpLocPortTable_oid_size, HANDLER_CAN_BABY_STEP | HANDLER_CAN_RONLY);

    if (NULL == reginfo) {
        snmp_log(LOG_ERR, "error registering table lldpLocPortTable\n");
        return;
    }

    reginfo->my_reg_void = &lldpLocPortTable_if_ctx;

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
        tbl_info, lldpLocPortTable_if_ctx.container,
        TABLE_CONTAINER_KEY_NETSNMP_INDEX);
    netsnmp_inject_handler(reginfo, handler);

    if (NULL != lldpLocPortTable_if_ctx.cache) {
        handler = netsnmp_cache_handler_get(lldpLocPortTable_if_ctx.cache);
        netsnmp_inject_handler(reginfo, handler);
    }

    netsnmp_register_table(reginfo, tbl_info);
}

void _lldpLocPortTable_shutdown_interface(
    lldpLocPortTable_registration *reg_ptr) {
    _lldpLocPortTable_container_shutdown(&lldpLocPortTable_if_ctx);
}

void lldpLocPortTable_valid_columns_set(netsnmp_column_info *vc) {
    lldpLocPortTable_if_ctx.tbl_info.valid_columns = vc;
}

int lldpLocPortTable_index_to_oid(netsnmp_index *oid_idx,
                                  lldpLocPortTable_mib_index *mib_idx) {
    int err = SNMP_ERR_NOERROR;
    netsnmp_variable_list var_lldpLocPortNum;

    memset(&var_lldpLocPortNum, 0x00, sizeof(var_lldpLocPortNum));
    var_lldpLocPortNum.type = ASN_INTEGER;
    var_lldpLocPortNum.next_variable = NULL;

    DEBUGMSGTL(
        ("verbose:lldpLocPortTable:lldpLocPortTable_index_to_oid", "called\n"));

    snmp_set_var_value(&var_lldpLocPortNum, &mib_idx->lldpLocPortNum,
                       sizeof(mib_idx->lldpLocPortNum));
    err = build_oid_noalloc(oid_idx->oids, oid_idx->len, &oid_idx->len, NULL, 0,
                            &var_lldpLocPortNum);
    if (err) {
        snmp_log(LOG_ERR, "error %d converting index to oid\n", err);
    }

    snmp_reset_var_buffers(&var_lldpLocPortNum);
    return err;
}

int lldpLocPortTable_index_from_oid(netsnmp_index *oid_idx,
                                    lldpLocPortTable_mib_index *mib_idx) {
    int err = SNMP_ERR_NOERROR;
    netsnmp_variable_list var_lldpLocPortNum;

    memset(&var_lldpLocPortNum, 0x00, sizeof(var_lldpLocPortNum));
    var_lldpLocPortNum.type = ASN_INTEGER;
    var_lldpLocPortNum.next_variable = NULL;

    DEBUGMSGTL(("verbose:lldpLocPortTable:lldpLocPortTable_index_from_oid",
                "called\n"));

    err = parse_oid_indexes(oid_idx->oids, oid_idx->len, &var_lldpLocPortNum);
    if (err == SNMP_ERR_NOERROR) {
        mib_idx->lldpLocPortNum = *((long *)var_lldpLocPortNum.val.string);
    }

    snmp_reset_var_buffers(&var_lldpLocPortNum);
    return err;
}

lldpLocPortTable_rowreq_ctx *
lldpLocPortTable_allocate_rowreq_ctx(void *user_init_ctx) {
    lldpLocPortTable_rowreq_ctx *rowreq_ctx =
        SNMP_MALLOC_TYPEDEF(lldpLocPortTable_rowreq_ctx);

    DEBUGMSGTL(
        ("internal:lldpLocPortTable:lldpLocPortTable_allocate_rowreq_ctx",
         "called\n"));

    if (NULL == rowreq_ctx) {
        snmp_log(
            LOG_ERR,
            "Could not allocate memory for a lldpLocPortTable_rowreq_ctx.\n");
        return NULL;
    }

    rowreq_ctx->oid_idx.oids = rowreq_ctx->oid_tmp;
    rowreq_ctx->lldpLocPortTable_data_list = NULL;
    if (!(rowreq_ctx->rowreq_flags & MFD_ROW_DATA_FROM_USER)) {
        if (SNMPERR_SUCCESS !=
            lldpLocPortTable_rowreq_ctx_init(rowreq_ctx, user_init_ctx)) {
            lldpLocPortTable_release_rowreq_ctx(rowreq_ctx);
            rowreq_ctx = NULL;
        }
    }
    return rowreq_ctx;
}

void lldpLocPortTable_release_rowreq_ctx(
    lldpLocPortTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(("internal:lldpLocPortTable:lldpLocPortTable_release_rowreq_ctx",
                "called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    lldpLocPortTable_rowreq_ctx_cleanup(rowreq_ctx);
    if (rowreq_ctx->oid_idx.oids != rowreq_ctx->oid_tmp) {
        free(rowreq_ctx->oid_idx.oids);
    }

    SNMP_FREE(rowreq_ctx);
}

static int _mfd_lldpLocPortTable_pre_request(
    netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
    netsnmp_agent_request_info *agtreq_info, netsnmp_request_info *requests) {
    int rc;
    DEBUGMSGTL(("internal:lldpLocPortTable:_mfd_lldpLocPortTable_pre_request",
                "called\n"));

    if (1 != netsnmp_row_merge_status_first(reginfo, agtreq_info)) {
        DEBUGMSGTL(
            ("internal:lldpLocPortTable", "skipping additional pre_request\n"));
        return SNMP_ERR_NOERROR;
    }

    rc = lldpLocPortTable_pre_request(lldpLocPortTable_if_ctx.user_ctx);
    if (MFD_SUCCESS != rc) {
        DEBUGMSGTL(("lldpLocPortTable",
                    "error %d from lldpLocPortTable_pre_requests\n", rc));
        netsnmp_request_set_error_all(requests, SNMP_VALIDATE_ERR(rc));
    }

    return SNMP_ERR_NOERROR;
}

static int _mfd_lldpLocPortTable_post_request(
    netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
    netsnmp_agent_request_info *agtreq_info, netsnmp_request_info *requests) {
    lldpLocPortTable_rowreq_ctx *rowreq_ctx =
        (lldpLocPortTable_rowreq_ctx *)netsnmp_container_table_row_extract(
            requests);
    int rc, packet_rc;
    DEBUGMSGTL(("internal:lldpLocPortTable:_mfd_lldpLocPortTable_post_request",
                "called\n"));

    if (rowreq_ctx && (rowreq_ctx->rowreq_flags & MFD_ROW_DELETED)) {
        lldpLocPortTable_release_rowreq_ctx(rowreq_ctx);
    }

    if (1 != netsnmp_row_merge_status_last(reginfo, agtreq_info)) {
        DEBUGMSGTL(
            ("internal:lldpLocPortTable", "waiting for last post_request\n"));
        return SNMP_ERR_NOERROR;
    }

    packet_rc = netsnmp_check_all_requests_error(agtreq_info->asp, 0);
    rc = lldpLocPortTable_post_request(lldpLocPortTable_if_ctx.user_ctx,
                                       packet_rc);
    if (MFD_SUCCESS != rc) {
        DEBUGMSGTL(("lldpLocPortTable",
                    "error %d from lldpLocPortTable_post_request\n", rc));
    }
    return SNMP_ERR_NOERROR;
}

static int _mfd_lldpLocPortTable_object_lookup(
    netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
    netsnmp_agent_request_info *agtreq_info, netsnmp_request_info *requests) {
    int rc = SNMP_ERR_NOERROR;
    lldpLocPortTable_rowreq_ctx *rowreq_ctx =
        (lldpLocPortTable_rowreq_ctx *)netsnmp_container_table_row_extract(
            requests);

    DEBUGMSGTL(("internal:lldpLocPortTable:_mfd_lldpLocPortTable_object_lookup",
                "called\n"));

    if (NULL == rowreq_ctx) {
        rc = SNMP_ERR_NOCREATION;
    }

    if (MFD_SUCCESS != rc) {
        netsnmp_request_set_error_all(requests, rc);
    } else {
        lldpLocPortTable_row_prep(rowreq_ctx);
    }

    return SNMP_VALIDATE_ERR(rc);
}

NETSNMP_STATIC_INLINE int
_lldpLocPortTable_get_column(lldpLocPortTable_rowreq_ctx *rowreq_ctx,
                             netsnmp_variable_list *var, int column) {
    int rc = SNMPERR_SUCCESS;

    DEBUGMSGTL(("internal:lldpLocPortTable:_mfd_lldpLocPortTable_get_column",
                "called for %d\n", column));

    netsnmp_assert(NULL != rowreq_ctx);

    switch (column) {
    case COLUMN_LLDPLOCPORTIDSUBTYPE: {
        var->type = ASN_INTEGER;
        var->val_len = sizeof(long);
        rc = lldpLocPortIdSubtype_get(rowreq_ctx, (long *)var->val.string);
    } break;
    case COLUMN_LLDPLOCPORTID: {
        var->type = ASN_OCTET_STR;
        rc = lldpLocPortId_get(rowreq_ctx, (char **)&var->val.string,
                               &var->val_len);
    } break;
    case COLUMN_LLDPLOCPORTDESC: {
        var->type = ASN_OCTET_STR;
        rc = lldpLocPortDesc_get(rowreq_ctx, (char **)&var->val.string,
                                 &var->val_len);
    } break;
    default:
        if (LLDPLOCPORTTABLE_MIN_COL <= column &&
            column <= LLDPLOCPORTTABLE_MAX_COL) {
            DEBUGMSGTL(
                ("internal:lldpLocPortTable:_mfd_lldpLocPortTable_get_column",
                 "assume column %d is reserved\n", column));
            rc = MFD_SKIP;
        } else {
            snmp_log(LOG_ERR,
                     "unknown column %d in _lldpLocPortTable_get_column\n",
                     column);
        }
        break;
    }

    return rc;
}

int _mfd_lldpLocPortTable_get_values(netsnmp_mib_handler *handler,
                                     netsnmp_handler_registration *reginfo,
                                     netsnmp_agent_request_info *agtreq_info,
                                     netsnmp_request_info *requests) {
    lldpLocPortTable_rowreq_ctx *rowreq_ctx =
        (lldpLocPortTable_rowreq_ctx *)netsnmp_container_table_row_extract(
            requests);
    netsnmp_table_request_info *tri;
    u_char *old_string;
    void (*dataFreeHook)(void *);
    int rc;

    DEBUGMSGTL(("internal:lldpLocPortTable:_mfd_lldpLocPortTable_get_values",
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
        rc = _lldpLocPortTable_get_column(rowreq_ctx, requests->requestvb,
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
    DEBUGMSGTL(("internal:lldpLocPortTable:_cache_load", "called\n"));

    if ((NULL == cache) || (NULL == cache->magic)) {
        snmp_log(LOG_ERR, "invalid cache for lldpLocPortTable_cache_load\n");
        return -1;
    }

    netsnmp_assert((0 == cache->valid) || (1 == cache->expired));

    return lldpLocPortTable_container_load((netsnmp_container *)cache->magic);
}

static void _cache_free(netsnmp_cache *cache, void *magic) {
    netsnmp_container *container;
    DEBUGMSGTL(("internal:lldpLocPortTable:_cache_free", "called\n"));

    if ((NULL == cache) || (NULL == cache->magic)) {
        snmp_log(LOG_ERR, "invalid cache in lldpLocPortTable_cache_free\n");
        return;
    }

    container = (netsnmp_container *)cache->magic;
    _container_free(container);
}

static void _container_item_free(lldpLocPortTable_rowreq_ctx *rowreq_ctx,
                                 void *context) {
    DEBUGMSGTL(("internal:lldpLocPortTable:_container_item_free", "called\n"));

    if (NULL == rowreq_ctx) {
        return;
    }
    lldpLocPortTable_release_rowreq_ctx(rowreq_ctx);
}

static void _container_free(netsnmp_container *container) {
    DEBUGMSGTL(("internal:lldpLocPortTable:_container_free", "called\n"));

    if (NULL == container) {
        snmp_log(LOG_ERR,
                 "invalid container in lldpLocPortTable_container_free\n");
        return;
    }
    lldpLocPortTable_container_free(container);
    CONTAINER_CLEAR(container,
                    (netsnmp_container_obj_func *)_container_item_free, NULL);
}

void _lldpLocPortTable_container_init(lldpLocPortTable_interface_ctx *if_ctx) {
    DEBUGMSGTL(("internal:lldpLocPortTable:_lldpLocPortTable_container_init",
                "called\n"));

    if_ctx->cache =
        netsnmp_cache_create(30, _cache_load, _cache_free, lldpLocPortTable_oid,
                             lldpLocPortTable_oid_size);

    if (NULL == if_ctx->cache) {
        snmp_log(LOG_ERR, "error creating cache for lldpLocPortTable\n");
        return;
    }

    if_ctx->cache->flags = NETSNMP_CACHE_DONT_INVALIDATE_ON_SET;
    lldpLocPortTable_container_init(&if_ctx->container, if_ctx->cache);
    if (NULL == if_ctx->container) {
        if_ctx->container =
            netsnmp_container_find("lldpLocPortTable:table_container");
    }
    if (NULL == if_ctx->container) {
        snmp_log(
            LOG_ERR,
            "error creating container in lldpLocPortTable_container_init\n");
        return;
    }

    if (NULL != if_ctx->cache) {
        if_ctx->cache->magic = (void *)if_ctx->container;
    }
}

void _lldpLocPortTable_container_shutdown(
    lldpLocPortTable_interface_ctx *if_ctx) {
    DEBUGMSGTL(
        ("internal:lldpLocPortTable:_lldpLocPortTable_container_shutdown",
         "called\n"));

    lldpLocPortTable_container_shutdown(if_ctx->container);
    _container_free(if_ctx->container);
}

lldpLocPortTable_rowreq_ctx *
lldpLocPortTable_row_find_by_mib_index(lldpLocPortTable_mib_index *mib_idx) {
    lldpLocPortTable_rowreq_ctx *rowreq_ctx;
    oid oid_tmp[MAX_OID_LEN];
    netsnmp_index oid_idx;
    int rc;

    oid_idx.oids = oid_tmp;
    oid_idx.len = sizeof(oid_tmp) / sizeof(oid);

    rc = lldpLocPortTable_index_to_oid(&oid_idx, mib_idx);
    if (MFD_SUCCESS != rc) {
        return NULL;
    }

    rowreq_ctx = (lldpLocPortTable_rowreq_ctx *)CONTAINER_FIND(
        lldpLocPortTable_if_ctx.container, &oid_idx);

    return rowreq_ctx;
}

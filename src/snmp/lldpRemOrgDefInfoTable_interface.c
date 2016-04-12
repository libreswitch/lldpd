#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/table_container.h>
#include <net-snmp/library/container.h>
#include "lldpRemOrgDefInfoTable.h"
#include "lldpRemOrgDefInfoTable_interface.h"

netsnmp_feature_require(baby_steps) netsnmp_feature_require(row_merge)
    netsnmp_feature_require(check_all_requests_error)

        typedef struct lldpRemOrgDefInfoTable_interface_ctx_s {
    netsnmp_container *container;
    netsnmp_cache *cache;
    lldpRemOrgDefInfoTable_registration *user_ctx;
    netsnmp_table_registration_info tbl_info;
    netsnmp_baby_steps_access_methods access_multiplexer;
} lldpRemOrgDefInfoTable_interface_ctx;

static lldpRemOrgDefInfoTable_interface_ctx lldpRemOrgDefInfoTable_if_ctx;
static void _lldpRemOrgDefInfoTable_container_init(
    lldpRemOrgDefInfoTable_interface_ctx *if_ctx);
static void _lldpRemOrgDefInfoTable_container_shutdown(
    lldpRemOrgDefInfoTable_interface_ctx *if_ctx);

netsnmp_container *lldpRemOrgDefInfoTable_container_get(void) {
    return lldpRemOrgDefInfoTable_if_ctx.container;
}

lldpRemOrgDefInfoTable_registration *
lldpRemOrgDefInfoTable_registration_get(void) {
    return lldpRemOrgDefInfoTable_if_ctx.user_ctx;
}

lldpRemOrgDefInfoTable_registration *lldpRemOrgDefInfoTable_registration_set(
    lldpRemOrgDefInfoTable_registration *newreg) {
    lldpRemOrgDefInfoTable_registration *old =
        lldpRemOrgDefInfoTable_if_ctx.user_ctx;
    lldpRemOrgDefInfoTable_if_ctx.user_ctx = newreg;
    return old;
}

int lldpRemOrgDefInfoTable_container_size(void) {
    return CONTAINER_SIZE(lldpRemOrgDefInfoTable_if_ctx.container);
}

static Netsnmp_Node_Handler _mfd_lldpRemOrgDefInfoTable_pre_request;
static Netsnmp_Node_Handler _mfd_lldpRemOrgDefInfoTable_post_request;
static Netsnmp_Node_Handler _mfd_lldpRemOrgDefInfoTable_object_lookup;
static Netsnmp_Node_Handler _mfd_lldpRemOrgDefInfoTable_get_values;

void _lldpRemOrgDefInfoTable_initialize_interface(
    lldpRemOrgDefInfoTable_registration *reg_ptr, u_long flags) {
    netsnmp_baby_steps_access_methods *access_multiplexer =
        &lldpRemOrgDefInfoTable_if_ctx.access_multiplexer;
    netsnmp_table_registration_info *tbl_info =
        &lldpRemOrgDefInfoTable_if_ctx.tbl_info;
    netsnmp_handler_registration *reginfo;
    netsnmp_mib_handler *handler;
    int mfd_modes = 0;

    DEBUGMSGTL(("internal:lldpRemOrgDefInfoTable:_lldpRemOrgDefInfoTable_"
                "initialize_interface",
                "called\n"));

    netsnmp_table_helper_add_indexes(tbl_info, ASN_TIMETICKS, ASN_INTEGER,
                                     ASN_INTEGER, ASN_OCTET_STR, ASN_INTEGER,
                                     ASN_INTEGER, 0);

    tbl_info->min_column = LLDPREMORGDEFINFOTABLE_MIN_COL;
    tbl_info->max_column = LLDPREMORGDEFINFOTABLE_MAX_COL;
    lldpRemOrgDefInfoTable_if_ctx.user_ctx = reg_ptr;
    lldpRemOrgDefInfoTable_init_data(reg_ptr);
    _lldpRemOrgDefInfoTable_container_init(&lldpRemOrgDefInfoTable_if_ctx);
    if (NULL == lldpRemOrgDefInfoTable_if_ctx.container) {
        snmp_log(LOG_ERR,
                 "could not initialize container for lldpRemOrgDefInfoTable\n");
        return;
    }

    access_multiplexer->object_lookup =
        _mfd_lldpRemOrgDefInfoTable_object_lookup;
    access_multiplexer->get_values = _mfd_lldpRemOrgDefInfoTable_get_values;

    access_multiplexer->pre_request = _mfd_lldpRemOrgDefInfoTable_pre_request;
    access_multiplexer->post_request = _mfd_lldpRemOrgDefInfoTable_post_request;

    DEBUGMSGTL(
        ("lldpRemOrgDefInfoTable:init_lldpRemOrgDefInfoTable",
         "Registering lldpRemOrgDefInfoTable as a mibs-for-dummies table.\n"));

    handler = netsnmp_baby_steps_access_multiplexer_get(access_multiplexer);
    reginfo = netsnmp_handler_registration_create(
        "lldpRemOrgDefInfoTable", handler, lldpRemOrgDefInfoTable_oid,
        lldpRemOrgDefInfoTable_oid_size,
        HANDLER_CAN_BABY_STEP | HANDLER_CAN_RONLY);

    if (NULL == reginfo) {
        snmp_log(LOG_ERR, "error registering table lldpRemOrgDefInfoTable\n");
        return;
    }

    reginfo->my_reg_void = &lldpRemOrgDefInfoTable_if_ctx;

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
        tbl_info, lldpRemOrgDefInfoTable_if_ctx.container,
        TABLE_CONTAINER_KEY_NETSNMP_INDEX);
    netsnmp_inject_handler(reginfo, handler);

    if (NULL != lldpRemOrgDefInfoTable_if_ctx.cache) {
        handler =
            netsnmp_cache_handler_get(lldpRemOrgDefInfoTable_if_ctx.cache);
        netsnmp_inject_handler(reginfo, handler);
    }

    netsnmp_register_table(reginfo, tbl_info);
}

void _lldpRemOrgDefInfoTable_shutdown_interface(
    lldpRemOrgDefInfoTable_registration *reg_ptr) {
    _lldpRemOrgDefInfoTable_container_shutdown(&lldpRemOrgDefInfoTable_if_ctx);
}

void lldpRemOrgDefInfoTable_valid_columns_set(netsnmp_column_info *vc) {
    lldpRemOrgDefInfoTable_if_ctx.tbl_info.valid_columns = vc;
}

int lldpRemOrgDefInfoTable_index_to_oid(
    netsnmp_index *oid_idx, lldpRemOrgDefInfoTable_mib_index *mib_idx) {
    int err = SNMP_ERR_NOERROR;
    netsnmp_variable_list var_lldpRemTimeMark;
    netsnmp_variable_list var_lldpRemLocalPortNum;
    netsnmp_variable_list var_lldpRemIndex;
    netsnmp_variable_list var_lldpRemOrgDefInfoOUI;
    netsnmp_variable_list var_lldpRemOrgDefInfoSubtype;
    netsnmp_variable_list var_lldpRemOrgDefInfoIndex;

    memset(&var_lldpRemTimeMark, 0x00, sizeof(var_lldpRemTimeMark));
    var_lldpRemTimeMark.type = ASN_TIMETICKS;
    var_lldpRemTimeMark.next_variable = &var_lldpRemLocalPortNum;

    memset(&var_lldpRemLocalPortNum, 0x00, sizeof(var_lldpRemLocalPortNum));
    var_lldpRemLocalPortNum.type = ASN_INTEGER;
    var_lldpRemLocalPortNum.next_variable = &var_lldpRemIndex;

    memset(&var_lldpRemIndex, 0x00, sizeof(var_lldpRemIndex));
    var_lldpRemIndex.type = ASN_INTEGER;
    var_lldpRemIndex.next_variable = &var_lldpRemOrgDefInfoOUI;

    memset(&var_lldpRemOrgDefInfoOUI, 0x00, sizeof(var_lldpRemOrgDefInfoOUI));
    var_lldpRemOrgDefInfoOUI.type = ASN_OCTET_STR;
    var_lldpRemOrgDefInfoOUI.next_variable = &var_lldpRemOrgDefInfoSubtype;

    memset(&var_lldpRemOrgDefInfoSubtype, 0x00,
           sizeof(var_lldpRemOrgDefInfoSubtype));
    var_lldpRemOrgDefInfoSubtype.type = ASN_INTEGER;
    var_lldpRemOrgDefInfoSubtype.next_variable = &var_lldpRemOrgDefInfoIndex;

    memset(&var_lldpRemOrgDefInfoIndex, 0x00,
           sizeof(var_lldpRemOrgDefInfoIndex));
    var_lldpRemOrgDefInfoIndex.type = ASN_INTEGER;
    var_lldpRemOrgDefInfoIndex.next_variable = NULL;

    DEBUGMSGTL(
        ("verbose:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_index_to_oid",
         "called\n"));

    snmp_set_var_value(&var_lldpRemTimeMark, &mib_idx->lldpRemTimeMark,
                       sizeof(mib_idx->lldpRemTimeMark));
    snmp_set_var_value(&var_lldpRemLocalPortNum, &mib_idx->lldpRemLocalPortNum,
                       sizeof(mib_idx->lldpRemLocalPortNum));
    snmp_set_var_value(&var_lldpRemIndex, &mib_idx->lldpRemIndex,
                       sizeof(mib_idx->lldpRemIndex));
    snmp_set_var_value(&var_lldpRemOrgDefInfoOUI,
                       &mib_idx->lldpRemOrgDefInfoOUI,
                       mib_idx->lldpRemOrgDefInfoOUI_len *
                           sizeof(mib_idx->lldpRemOrgDefInfoOUI[0]));
    snmp_set_var_value(&var_lldpRemOrgDefInfoSubtype,
                       &mib_idx->lldpRemOrgDefInfoSubtype,
                       sizeof(mib_idx->lldpRemOrgDefInfoSubtype));
    snmp_set_var_value(&var_lldpRemOrgDefInfoIndex,
                       &mib_idx->lldpRemOrgDefInfoIndex,
                       sizeof(mib_idx->lldpRemOrgDefInfoIndex));
    err = build_oid_noalloc(oid_idx->oids, oid_idx->len, &oid_idx->len, NULL, 0,
                            &var_lldpRemTimeMark);
    if (err) {
        snmp_log(LOG_ERR, "error %d converting index to oid\n", err);
    }

    snmp_reset_var_buffers(&var_lldpRemTimeMark);
    return err;
}

int lldpRemOrgDefInfoTable_index_from_oid(
    netsnmp_index *oid_idx, lldpRemOrgDefInfoTable_mib_index *mib_idx) {
    int err = SNMP_ERR_NOERROR;
    netsnmp_variable_list var_lldpRemTimeMark;
    netsnmp_variable_list var_lldpRemLocalPortNum;
    netsnmp_variable_list var_lldpRemIndex;
    netsnmp_variable_list var_lldpRemOrgDefInfoOUI;
    netsnmp_variable_list var_lldpRemOrgDefInfoSubtype;
    netsnmp_variable_list var_lldpRemOrgDefInfoIndex;

    memset(&var_lldpRemTimeMark, 0x00, sizeof(var_lldpRemTimeMark));
    var_lldpRemTimeMark.type = ASN_TIMETICKS;
    var_lldpRemTimeMark.next_variable = &var_lldpRemLocalPortNum;

    memset(&var_lldpRemLocalPortNum, 0x00, sizeof(var_lldpRemLocalPortNum));
    var_lldpRemLocalPortNum.type = ASN_INTEGER;
    var_lldpRemLocalPortNum.next_variable = &var_lldpRemIndex;

    memset(&var_lldpRemIndex, 0x00, sizeof(var_lldpRemIndex));
    var_lldpRemIndex.type = ASN_INTEGER;
    var_lldpRemIndex.next_variable = &var_lldpRemOrgDefInfoOUI;

    memset(&var_lldpRemOrgDefInfoOUI, 0x00, sizeof(var_lldpRemOrgDefInfoOUI));
    var_lldpRemOrgDefInfoOUI.type = ASN_OCTET_STR;
    var_lldpRemOrgDefInfoOUI.next_variable = &var_lldpRemOrgDefInfoSubtype;

    memset(&var_lldpRemOrgDefInfoSubtype, 0x00,
           sizeof(var_lldpRemOrgDefInfoSubtype));
    var_lldpRemOrgDefInfoSubtype.type = ASN_INTEGER;
    var_lldpRemOrgDefInfoSubtype.next_variable = &var_lldpRemOrgDefInfoIndex;

    memset(&var_lldpRemOrgDefInfoIndex, 0x00,
           sizeof(var_lldpRemOrgDefInfoIndex));
    var_lldpRemOrgDefInfoIndex.type = ASN_INTEGER;
    var_lldpRemOrgDefInfoIndex.next_variable = NULL;

    DEBUGMSGTL(
        ("verbose:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_index_from_oid",
         "called\n"));

    err = parse_oid_indexes(oid_idx->oids, oid_idx->len, &var_lldpRemTimeMark);
    if (err == SNMP_ERR_NOERROR) {
        mib_idx->lldpRemTimeMark = *((long *)var_lldpRemTimeMark.val.string);
        mib_idx->lldpRemLocalPortNum =
            *((long *)var_lldpRemLocalPortNum.val.string);
        mib_idx->lldpRemIndex = *((long *)var_lldpRemIndex.val.string);
        if (var_lldpRemOrgDefInfoOUI.val_len >
            sizeof(mib_idx->lldpRemOrgDefInfoOUI)) {
            err = SNMP_ERR_GENERR;
        } else {
            memcpy(mib_idx->lldpRemOrgDefInfoOUI,
                   var_lldpRemOrgDefInfoOUI.val.string,
                   var_lldpRemOrgDefInfoOUI.val_len);
            mib_idx->lldpRemOrgDefInfoOUI_len =
                var_lldpRemOrgDefInfoOUI.val_len /
                sizeof(mib_idx->lldpRemOrgDefInfoOUI[0]);
        }
        mib_idx->lldpRemOrgDefInfoSubtype =
            *((long *)var_lldpRemOrgDefInfoSubtype.val.string);
        mib_idx->lldpRemOrgDefInfoIndex =
            *((long *)var_lldpRemOrgDefInfoIndex.val.string);
    }

    snmp_reset_var_buffers(&var_lldpRemTimeMark);
    return err;
}

lldpRemOrgDefInfoTable_rowreq_ctx *
lldpRemOrgDefInfoTable_allocate_rowreq_ctx(void *user_init_ctx) {
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx =
        SNMP_MALLOC_TYPEDEF(lldpRemOrgDefInfoTable_rowreq_ctx);

    DEBUGMSGTL(("internal:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_"
                "allocate_rowreq_ctx",
                "called\n"));

    if (NULL == rowreq_ctx) {
        snmp_log(LOG_ERR, "Could not allocate memory for a "
                          "lldpRemOrgDefInfoTable_rowreq_ctx.\n");
        return NULL;
    }

    rowreq_ctx->oid_idx.oids = rowreq_ctx->oid_tmp;
    rowreq_ctx->lldpRemOrgDefInfoTable_data_list = NULL;
    if (!(rowreq_ctx->rowreq_flags & MFD_ROW_DATA_FROM_USER)) {
        if (SNMPERR_SUCCESS !=
            lldpRemOrgDefInfoTable_rowreq_ctx_init(rowreq_ctx, user_init_ctx)) {
            lldpRemOrgDefInfoTable_release_rowreq_ctx(rowreq_ctx);
            rowreq_ctx = NULL;
        }
    }
    return rowreq_ctx;
}

void lldpRemOrgDefInfoTable_release_rowreq_ctx(
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx) {
    DEBUGMSGTL(("internal:lldpRemOrgDefInfoTable:lldpRemOrgDefInfoTable_"
                "release_rowreq_ctx",
                "called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    lldpRemOrgDefInfoTable_rowreq_ctx_cleanup(rowreq_ctx);
    if (rowreq_ctx->oid_idx.oids != rowreq_ctx->oid_tmp) {
        free(rowreq_ctx->oid_idx.oids);
    }

    SNMP_FREE(rowreq_ctx);
}

static int _mfd_lldpRemOrgDefInfoTable_pre_request(
    netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
    netsnmp_agent_request_info *agtreq_info, netsnmp_request_info *requests) {
    int rc;
    DEBUGMSGTL(("internal:lldpRemOrgDefInfoTable:_mfd_lldpRemOrgDefInfoTable_"
                "pre_request",
                "called\n"));

    if (1 != netsnmp_row_merge_status_first(reginfo, agtreq_info)) {
        DEBUGMSGTL(("internal:lldpRemOrgDefInfoTable",
                    "skipping additional pre_request\n"));
        return SNMP_ERR_NOERROR;
    }

    rc = lldpRemOrgDefInfoTable_pre_request(
        lldpRemOrgDefInfoTable_if_ctx.user_ctx);
    if (MFD_SUCCESS != rc) {
        DEBUGMSGTL(("lldpRemOrgDefInfoTable",
                    "error %d from lldpRemOrgDefInfoTable_pre_requests\n", rc));
        netsnmp_request_set_error_all(requests, SNMP_VALIDATE_ERR(rc));
    }

    return SNMP_ERR_NOERROR;
}

static int _mfd_lldpRemOrgDefInfoTable_post_request(
    netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
    netsnmp_agent_request_info *agtreq_info, netsnmp_request_info *requests) {
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx =
        (lldpRemOrgDefInfoTable_rowreq_ctx *)
            netsnmp_container_table_row_extract(requests);
    int rc, packet_rc;
    DEBUGMSGTL(("internal:lldpRemOrgDefInfoTable:_mfd_lldpRemOrgDefInfoTable_"
                "post_request",
                "called\n"));

    if (rowreq_ctx && (rowreq_ctx->rowreq_flags & MFD_ROW_DELETED)) {
        lldpRemOrgDefInfoTable_release_rowreq_ctx(rowreq_ctx);
    }

    if (1 != netsnmp_row_merge_status_last(reginfo, agtreq_info)) {
        DEBUGMSGTL(("internal:lldpRemOrgDefInfoTable",
                    "waiting for last post_request\n"));
        return SNMP_ERR_NOERROR;
    }

    packet_rc = netsnmp_check_all_requests_error(agtreq_info->asp, 0);
    rc = lldpRemOrgDefInfoTable_post_request(
        lldpRemOrgDefInfoTable_if_ctx.user_ctx, packet_rc);
    if (MFD_SUCCESS != rc) {
        DEBUGMSGTL(("lldpRemOrgDefInfoTable",
                    "error %d from lldpRemOrgDefInfoTable_post_request\n", rc));
    }
    return SNMP_ERR_NOERROR;
}

static int _mfd_lldpRemOrgDefInfoTable_object_lookup(
    netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
    netsnmp_agent_request_info *agtreq_info, netsnmp_request_info *requests) {
    int rc = SNMP_ERR_NOERROR;
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx =
        (lldpRemOrgDefInfoTable_rowreq_ctx *)
            netsnmp_container_table_row_extract(requests);

    DEBUGMSGTL(("internal:lldpRemOrgDefInfoTable:_mfd_lldpRemOrgDefInfoTable_"
                "object_lookup",
                "called\n"));

    if (NULL == rowreq_ctx) {
        rc = SNMP_ERR_NOCREATION;
    }

    if (MFD_SUCCESS != rc) {
        netsnmp_request_set_error_all(requests, rc);
    } else {
        lldpRemOrgDefInfoTable_row_prep(rowreq_ctx);
    }

    return SNMP_VALIDATE_ERR(rc);
}

NETSNMP_STATIC_INLINE int _lldpRemOrgDefInfoTable_get_column(
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx, netsnmp_variable_list *var,
    int column) {
    int rc = SNMPERR_SUCCESS;

    DEBUGMSGTL(("internal:lldpRemOrgDefInfoTable:_mfd_lldpRemOrgDefInfoTable_"
                "get_column",
                "called for %d\n", column));

    netsnmp_assert(NULL != rowreq_ctx);

    switch (column) {
    case COLUMN_LLDPREMORGDEFINFO: {
        var->type = ASN_OCTET_STR;
        rc = lldpRemOrgDefInfo_get(rowreq_ctx, (char **)&var->val.string,
                                   &var->val_len);
    } break;
    default:
        if (LLDPREMORGDEFINFOTABLE_MIN_COL <= column &&
            column <= LLDPREMORGDEFINFOTABLE_MAX_COL) {
            DEBUGMSGTL(("internal:lldpRemOrgDefInfoTable:_mfd_"
                        "lldpRemOrgDefInfoTable_get_column",
                        "assume column %d is reserved\n", column));
            rc = MFD_SKIP;
        } else {
            snmp_log(
                LOG_ERR,
                "unknown column %d in _lldpRemOrgDefInfoTable_get_column\n",
                column);
        }
        break;
    }

    return rc;
}

int _mfd_lldpRemOrgDefInfoTable_get_values(
    netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
    netsnmp_agent_request_info *agtreq_info, netsnmp_request_info *requests) {
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx =
        (lldpRemOrgDefInfoTable_rowreq_ctx *)
            netsnmp_container_table_row_extract(requests);
    netsnmp_table_request_info *tri;
    u_char *old_string;
    void (*dataFreeHook)(void *);
    int rc;

    DEBUGMSGTL(("internal:lldpRemOrgDefInfoTable:_mfd_lldpRemOrgDefInfoTable_"
                "get_values",
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
        rc = _lldpRemOrgDefInfoTable_get_column(rowreq_ctx, requests->requestvb,
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
    DEBUGMSGTL(("internal:lldpRemOrgDefInfoTable:_cache_load", "called\n"));

    if ((NULL == cache) || (NULL == cache->magic)) {
        snmp_log(LOG_ERR,
                 "invalid cache for lldpRemOrgDefInfoTable_cache_load\n");
        return -1;
    }

    netsnmp_assert((0 == cache->valid) || (1 == cache->expired));

    return lldpRemOrgDefInfoTable_container_load(
        (netsnmp_container *)cache->magic);
}

static void _cache_free(netsnmp_cache *cache, void *magic) {
    netsnmp_container *container;
    DEBUGMSGTL(("internal:lldpRemOrgDefInfoTable:_cache_free", "called\n"));

    if ((NULL == cache) || (NULL == cache->magic)) {
        snmp_log(LOG_ERR,
                 "invalid cache in lldpRemOrgDefInfoTable_cache_free\n");
        return;
    }

    container = (netsnmp_container *)cache->magic;
    _container_free(container);
}

static void _container_item_free(lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx,
                                 void *context) {
    DEBUGMSGTL(
        ("internal:lldpRemOrgDefInfoTable:_container_item_free", "called\n"));

    if (NULL == rowreq_ctx) {
        return;
    }
    lldpRemOrgDefInfoTable_release_rowreq_ctx(rowreq_ctx);
}

static void _container_free(netsnmp_container *container) {
    DEBUGMSGTL(("internal:lldpRemOrgDefInfoTable:_container_free", "called\n"));

    if (NULL == container) {
        snmp_log(
            LOG_ERR,
            "invalid container in lldpRemOrgDefInfoTable_container_free\n");
        return;
    }
    lldpRemOrgDefInfoTable_container_free(container);
    CONTAINER_CLEAR(container,
                    (netsnmp_container_obj_func *)_container_item_free, NULL);
}

void _lldpRemOrgDefInfoTable_container_init(
    lldpRemOrgDefInfoTable_interface_ctx *if_ctx) {
    DEBUGMSGTL(("internal:lldpRemOrgDefInfoTable:_lldpRemOrgDefInfoTable_"
                "container_init",
                "called\n"));

    if_ctx->cache = netsnmp_cache_create(30, _cache_load, _cache_free,
                                         lldpRemOrgDefInfoTable_oid,
                                         lldpRemOrgDefInfoTable_oid_size);

    if (NULL == if_ctx->cache) {
        snmp_log(LOG_ERR, "error creating cache for lldpRemOrgDefInfoTable\n");
        return;
    }

    if_ctx->cache->flags = NETSNMP_CACHE_DONT_INVALIDATE_ON_SET;
    lldpRemOrgDefInfoTable_container_init(&if_ctx->container, if_ctx->cache);
    if (NULL == if_ctx->container) {
        if_ctx->container =
            netsnmp_container_find("lldpRemOrgDefInfoTable:table_container");
    }
    if (NULL == if_ctx->container) {
        snmp_log(LOG_ERR, "error creating container in "
                          "lldpRemOrgDefInfoTable_container_init\n");
        return;
    }

    if (NULL != if_ctx->cache) {
        if_ctx->cache->magic = (void *)if_ctx->container;
    }
}

void _lldpRemOrgDefInfoTable_container_shutdown(
    lldpRemOrgDefInfoTable_interface_ctx *if_ctx) {
    DEBUGMSGTL(("internal:lldpRemOrgDefInfoTable:_lldpRemOrgDefInfoTable_"
                "container_shutdown",
                "called\n"));

    lldpRemOrgDefInfoTable_container_shutdown(if_ctx->container);
    _container_free(if_ctx->container);
}

lldpRemOrgDefInfoTable_rowreq_ctx *lldpRemOrgDefInfoTable_row_find_by_mib_index(
    lldpRemOrgDefInfoTable_mib_index *mib_idx) {
    lldpRemOrgDefInfoTable_rowreq_ctx *rowreq_ctx;
    oid oid_tmp[MAX_OID_LEN];
    netsnmp_index oid_idx;
    int rc;

    oid_idx.oids = oid_tmp;
    oid_idx.len = sizeof(oid_tmp) / sizeof(oid);

    rc = lldpRemOrgDefInfoTable_index_to_oid(&oid_idx, mib_idx);
    if (MFD_SUCCESS != rc) {
        return NULL;
    }

    rowreq_ctx = (lldpRemOrgDefInfoTable_rowreq_ctx *)CONTAINER_FIND(
        lldpRemOrgDefInfoTable_if_ctx.container, &oid_idx);

    return rowreq_ctx;
}

/*
 * Hewlett-Packard Company Confidential (C) Copyright 2015
 * Hewlett-Packard Development Company, L.P.
 *
 * File: lldpd_ovsdb_if_test.c
 *
 * Purpose: Main file for testing lldpd integrated with ovsdb
 *          Its purpose is to provide hooks for ovs-appctl to trigger UT/CT
 * Example:
 *    ovs-appctl -t lldpd lldpd/test libevent 0  # starts libevent burst
 *
 *    ovs-appctl -t lldpd lldpd/test libevent 1  # checks libevent result
 *
 */

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/event_struct.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// OVS headers
#include "unixctl.h"
#include "openvswitch/vlog.h"
#include "coverage.h"
#include "lldpd_ovsdb_if.h"

extern u_int64_t ovs_libevent_get_counter(void);
extern void *ovs_libevent_get_arg(void);

VLOG_DEFINE_THIS_MODULE(lldpd_ovsdb_if_test);

/*
 * Unit test function to create a link list with 2 IPv4 addresses
 * for a p_nbr->p_chassis
 * To enable, g_lldpd_test_nbr_mgmt_addr_list = true
 */
void
ovsdb_test_nbr_mgmt_addr_list(struct lldpd_chassis *p_chassis)
{
    struct lldpd_mgmt *mgmt;
    int address1 = 0x09000001;
    int address2 = 0x09000002;

    TAILQ_INIT(&p_chassis->c_mgmt);
    mgmt = lldpd_alloc_mgmt(LLDPD_AF_IPV4, &address1, sizeof(struct in_addr), 0);
    if (mgmt != NULL) {
        TAILQ_INSERT_TAIL(&p_chassis->c_mgmt, mgmt, m_entries);
    }
    mgmt = lldpd_alloc_mgmt(LLDPD_AF_IPV4, &address2, sizeof(struct in_addr), 0);
    if (mgmt != NULL) {
        TAILQ_INSERT_TAIL(&p_chassis->c_mgmt, mgmt, m_entries);
    }
}


/*
 * Test ovsdb write failure (TBD)
 */
static void
test_ovsdb_write_failure(int param, char *return_status)
{
}

#define LIBEVENT_TEST_CNT 100
#define LLDP_TEST_START 0
#define LLDP_TEST_STOP 1
u_int64_t libevent_cnt_start, libevent_cnt_end;
/*
 * Test libevent loop by sending a burst of 100 events
 * and making sure they get executed
 * Execute test: 
 * ovs-appctl -t lldpd lldpd/test libevent 0
 * Checks result:
 * ovs-appctl -t lldpd lldpd/test libevent 1 
 */
static void
test_ovsdb_libevent_loop(int param, char *return_status)
{
    void *libevent_arg;
    u_int64_t libevent_cnt_total;
    int i;

    libevent_arg = ovs_libevent_get_arg();
    if (libevent_arg == NULL) {
        sprintf(return_status, "%s", "Error: Uninitlized lldpd config pointer");
        return;
    }

    if (param == LLDP_TEST_START) {
        libevent_cnt_start = ovs_libevent_get_counter();
        for (i=0; i < LIBEVENT_TEST_CNT; i++) {
            ovs_libevent_schedule_nbr(libevent_arg);
        }
        sprintf(return_status, "%s", "OK");
    } else {
        libevent_cnt_end = ovs_libevent_get_counter();
        libevent_cnt_total = libevent_cnt_end - libevent_cnt_start;
        if (libevent_cnt_total >= LIBEVENT_TEST_CNT) {
            VLOG_INFO("libevent unit test done - start=%lu end=%lu",
                       libevent_cnt_start, libevent_cnt_end);
            sprintf(return_status, "%s", "OK");
        } else {
            sprintf(return_status,
                    "Error: Missing lldpd events (expcted=%d) (arrived=%d)",
                    LIBEVENT_TEST_CNT, (int)libevent_cnt_total);
        }
    }
}

static void
test_parse_options(int argc, const char *argv[], char *return_status)
{
    if (argc != 3) {
        sprintf(return_status, "Wrong argument count - %d", argc);
        return;
    }

    if (!strcmp("ovsdb", argv[1])) {
        test_ovsdb_write_failure(atoi(argv[2]), return_status);
    } else
    if (!strcmp("libevent", argv[1])) {
        test_ovsdb_libevent_loop(atoi(argv[2]), return_status);
    } else {
        sprintf(return_status, "Unsupported test - %s", argv[1]);
    }
}

void
lldpd_unixctl_test(struct unixctl_conn *conn, int argc OVS_UNUSED,
                          const char *argv[] OVS_UNUSED, void *aux OVS_UNUSED)
{
    char return_status[80] = "OK";
    int i;

    for (i=0; i < argc; i++) {
        VLOG_INFO("lldpd/test arg %d - %s", i, argv[i]);
    }
    test_parse_options(argc, argv, return_status);

    if (!strcmp(return_status, "OK")) {
        unixctl_command_reply(conn, "OK");
    } else {
        unixctl_command_reply_error(conn, return_status);
    }

}

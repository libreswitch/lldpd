/*
 * (c) Copyright 2016 Hewlett Packard Enterprise Development LP
 *
 *   Licensed under the Apache License, Version 2.0 (the "License"); you may
 *   not use this file except in compliance with the License. You may obtain
 *   a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *   WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *   License for the specific language governing permissions and limitations
 *   under the License.
 *
 * File: lldpd_ovsdb_if.c
 *
 * Purpose: Main file for integrating lldpd with ovsdb and ovs poll-loop.
 *          Its purpose in life is to provide hooks to lldpd daemon to do
 *          following:
 *
 *                1. During start up, read lldpd related
 *                   configuration data and apply to lldpd.
 *
 *                2. During operations, receive administrative
 *                   configuration changes and apply to lldpd config.
 *
 *                3. Update statistics and neighbor tables periodically
 *                   to database.
 *
 *                4. Sync lldpd internal data structures from database
 *                   when restarting lldpd after a crash.
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
#include <arpa/inet.h>

/*
 * OVS headers
 */
#include "config.h"
#include "command-line.h"
#include "daemon.h"
#include "dirs.h"
#include "dummy.h"
#include "fatal-signal.h"
#include "poll-loop.h"
#include "stream.h"
#include "timeval.h"
#include "unixctl.h"
#include "openvswitch/vlog.h"
#include "vswitch-idl.h"
#include "coverage.h"
#include "lldpd.h"
#include "../lldp-const.h"
#include "openswitch-idl.h"
#include "lldpd_ovsdb_if.h"
#include "vlan-bitmap.h"
#include "eventlog.h"
#include  <diag_dump.h>
#include "LLDP_MIB_traps.h"

COVERAGE_DEFINE(lldpd_ovsdb_if);
VLOG_DEFINE_THIS_MODULE(lldpd_ovsdb_if);

#define KEY_VAL_STR_BUF_MAX 4096
#define KEY_VAL_STR_BUF_MAX 4096
#define MAX_DESCR 256
#define KEY_VAL_MAX 64
#define MGMT_IF_MAX 16
#define STR_BUF_DECODE_MAX 1024
#define DECODE_TABLE_MAX 1024
#define VLAN_LIST_STR_MAX (4096 * 256)
#define VLAN_LIST_INT_MAX (4096 * 16)
#define MGMTIP_LIST_MAX (MGMT_IF_MAX * INET6_ADDRSTRLEN)
#define BUF_LEN 16000
#define REM_BUF_LEN (buflen - 1 - strlen(buf))
#define MAX_ERR_STR_LEN 255
#define DEFAULT_INTERFACE "bridge_normal"

static struct ovsdb_idl *idl;
static unsigned int idl_seqno;
static struct event *timeout_event;
static struct event *nbr_event;
static int system_configured = false;
static int64_t libevent_cnt = 0;
static void *libevent_cb_arg = NULL;

static void ovs_libevent_cb(evutil_socket_t fd, short what, void *arg);
static void ovs_clear_libevents(void);
static int ovspoll_to_libevent(struct lldpd *cfg);
static void lldpd_run(struct lldpd *cfg);
static void lldpd_wait(void);
static int lldp_nbr_update(void *smap, struct lldpd_port *p_nbr);
static bool lldpd_ovsdb_clear_all_nbrs_run(struct ovsdb_idl *idl);

static char *appctl_path = NULL;
static struct unixctl_server *appctl;
static unixctl_cb_func lldpd_unixctl_dump;
static unixctl_cb_func ops_lldpd_exit;
static bool g_ovsdb_test_nbr_mgmt_addr_list = false;

unixctl_cb_func lldpd_unixctl_test;
void lldpd_unixctl_test(struct unixctl_conn *conn, int argc,
                        const char *argv[], void *aux OVS_UNUSED);
void ovsdb_test_nbr_mgmt_addr_list(struct lldpd_chassis *p_chassis);
static void lldpd_diag_dump_basic_cb(const char *feature , char **buf);

bool exiting = false;

static char *vlan_name_lookup_by_vid(int64_t vid);

/*
 * The structure contains interface table information for an interface
 */
struct interface_data {
	char *name;                 /* Always non null */
	int native_vid;             /* "tag" column - native VLAN ID. */
	int synced_from_db;
	const struct ovsrec_interface *ifrow;       /* Handle to ovsrec row */
	struct port_data *portdata; /* Handle to port data */
	struct lldpd_hardware *hw;  /* Handle to lldp hardware interface */
};

/*
 * port_data struct that contains PORT table information for a single port.
 */

struct port_data {
	char *name;
	enum ovsrec_port_vlan_mode_e vlan_mode;     /* "vlan_mode" column. */
	int native_vid;             /* "tag" column - native VLAN ID. */
	bool trunk_all_vlans;       /* Indicates whether this port is implicitly
				     * Trunking all VLANs defined in VLAN table.
				     */
	struct interface_data **interfaces; /* Handle to the hw interface */
	int n_interfaces;           /* Number of interfaces on this port */
	const struct ovsrec_port *portrow;  /* Handle to ovsrec tow */
};

/* Mapping of all the interfaces. */
static struct shash all_interfaces = SHASH_INITIALIZER(&all_interfaces);

/* Mapping of all the ports. */
static struct shash all_ports = SHASH_INITIALIZER(&all_ports);

char *lldp_decode_table[DECODE_TABLE_MAX];
uint32_t lldp_decode_start[LLDP_INDEX_MAX + 1];

#define LLDP_DECODE_SETUP(lldp_type, lldp_type_name, base)		\
	lldp_decode_table[ base + lldp_type ] = xstrdup(#lldp_type_name);

/*
 * LLDP Neighbor decode Functions
 */

/*
 * Finds a set bit in an 32b integer when
 * input is a bit mask option with only one bit set.
 */
inline static int
bit_set(uint32_t n)
{
	uint32_t l = 0;

	while (n >>= 1) {
		l++;
	}
	return l;
}

const struct ovsdb_idl*
get_idl(void){
    return idl;
}

/*
 * The function takes a bitmap of features as input and creates
 * a comma-separated list of feature names as output.
 * Category points to the decode table for the feature class.
 * For example, map a bit mask representing capabilities
 * bit 2 set => Bridge
 * bit 4 set => Router
 * Input
 * features (bitmask) == 10100b
 * Output
 * decode_str = "Bridge, Router"
 */
static void
decode_features(char *decode_str, uint32_t features, uint32_t category)
{
	int i, n, base, limit;

	base = lldp_decode_start[category];
	limit = lldp_decode_start[category + 1] - lldp_decode_start[category];

	sprintf(decode_str, "%s", " ");

	for (n = 0, i = 0; i < limit; i++) {
		if (features & (1 << i)) {
			if (n == 0) {
				n = sprintf(decode_str, "%s", lldp_decode_table[base + i]);
			} else {
				n += sprintf(&decode_str[n], ", %s",
					     lldp_decode_table[base + i]);
			}
		}
	}

}

/*
 * The function takes a property (opcode) as input
 * and creates a property name as output.
 * Category points to the decode table for the feature class.
 */
static void
decode_property(char *decode_str, uint32_t property, uint32_t category)
{
	int base = lldp_decode_start[category];
	int limit = lldp_decode_start[category + 1] - lldp_decode_start[category];

	sprintf(decode_str, "%s", " ");
	if ((property >= limit) || (lldp_decode_table[base + property] == NULL)) {
		return;
	}

	sprintf(decode_str, "%s", lldp_decode_table[base + property]);
}

/*
 * Converts a binary network address to an ASCII address in the form:
 * xx:yy:...:zz
 */
static void
decode_nw_addr(char *decode_str, char *user_str, int key_len)
{
	int i, n;
	unsigned char c;

	n = 0;
        if (key_len == ETHER_ADDR_LEN) {
            for (i = 0; i < key_len; i++) {
                c = user_str[i];
                if (i < key_len - 1)
                    n += sprintf(&decode_str[n], "%02x:", c);
                else
                    sprintf(&decode_str[n], "%02x", c);
            }
        }
        else {
            VLOG_ERR("Invalid length = %d", key_len);
        }
}

/*
 * This initialization function fills up the decode table.
 * The decode table is broken into categories.
 * Each category maps a list of options, starting from
 * 0, up to MAX, currently MAX < 64
 * The decode table maps an option from binary to string.
 *
 * When it's time to update neighbor table the code uses
 * the decode table to map a field from binary to feature
 * name or to a list of feature names.
 *
 * Each LLDP feature option range starts at lldp_decode_start[category]
 * and ends at lldp_decode_start[category+1]
 * The categories are enums mapping all feature classes
 */
static void
lldp_ovsdb_setup_decode()
{
	int index = 0;

	memset(lldp_decode_table, 0, sizeof lldp_decode_table);

/* Chassis ID subtype */
	lldp_decode_start[LLDP_CHASSISID_SUBTYPE_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_CHASSISID_SUBTYPE_CHASSIS, chassis, index);
	LLDP_DECODE_SETUP(LLDP_CHASSISID_SUBTYPE_IFALIAS, if_alias, index);
	LLDP_DECODE_SETUP(LLDP_CHASSISID_SUBTYPE_PORT, port, index);
	LLDP_DECODE_SETUP(LLDP_CHASSISID_SUBTYPE_LLADDR, link_local_addr,
                          index);
	LLDP_DECODE_SETUP(LLDP_CHASSISID_SUBTYPE_ADDR, addr, index);
	LLDP_DECODE_SETUP(LLDP_CHASSISID_SUBTYPE_IFNAME, if_name, index);
	LLDP_DECODE_SETUP(LLDP_CHASSISID_SUBTYPE_LOCAL, local, index);
	index += LLDP_CHASSISID_SUBTYPE_LOCAL + 1;

/* Port ID subtype */
	lldp_decode_start[LLDP_PORTID_SUBTYPE_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_PORTID_SUBTYPE_UNKNOWN, unknown, index);
	LLDP_DECODE_SETUP(LLDP_PORTID_SUBTYPE_IFALIAS, if_alias, index);
	LLDP_DECODE_SETUP(LLDP_PORTID_SUBTYPE_PORT, port, index);
	LLDP_DECODE_SETUP(LLDP_PORTID_SUBTYPE_LLADDR, link_local_addr, index);
	LLDP_DECODE_SETUP(LLDP_PORTID_SUBTYPE_ADDR, addr, index);
	LLDP_DECODE_SETUP(LLDP_PORTID_SUBTYPE_IFNAME, if_name, index);
	LLDP_DECODE_SETUP(LLDP_PORTID_SUBTYPE_AGENTCID, agent_cid, index);
	LLDP_DECODE_SETUP(LLDP_PORTID_SUBTYPE_LOCAL, if_name, index);
	index += LLDP_PORTID_SUBTYPE_LOCAL + 1;

/* Operational MAU Type field, from RFC 3636 */
	lldp_decode_start[LLDP_DOT3_MAU_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_AUI, AUI, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10BASE5, 10 BASE5, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10BASET, 10 BASE5, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10BASEFP, 10 BASEFP, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10BASEFB, 10 BASEFB, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10BASEFL, 10 BASEFL, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10BROAD36, 10 BROAD36, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10BASETHD, 10 BASETHD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10BASETFD, 10 BASETFD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10BASEFLHD, 10 BASEFLHD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10BASEFLFD, 10 BASEFLFD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10BASET4, 10 BASET4, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_100BASETXHD, 100 BASETXHD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_100BASETXFD, 100 BASETXFD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_100BASEFXHD, 100 BASEFXHD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_100BASEFXFD, 100 BASEFXFD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_100BASET2HD, 100 BASET2HD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_100BASET2FD, 100 BASET2FD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_1000BASEXHD, 1000 BASEXHD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_1000BASEXFD, 1000 BASEXFD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_1000BASELXHD, 1000 BASELXHD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_1000BASELXFD, 1000 BASELXFD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_1000BASESXHD, 1000 BASESXHD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_1000BASESXFD, 1000 BASESXFD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_1000BASECXHD, 1000 BASECXHD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_1000BASECXFD, 1000 BASECXFD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_1000BASETHD, 1000 BASETHD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_1000BASETFD, 1000 BASETFD, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10GIGBASEX, 10 GIGBASEX, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10GIGBASELX4, 10 GIGBASELX4, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10GIGBASER, 10 GIGBASEER, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10GIGBASEER, 10 GIGBASEER, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10GIGBASELR, 10 GIGBASELR, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10GIGBASESR, 10 GIGBASESR, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10GIGBASEW, 10 GIGBASEW, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10GIGBASEEW, 10 GIGBASEEW, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10GIGBASELW, 10 GIGBASELW, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_MAU_10GIGBASESW, 10 GIGBASESW, index);
	index += LLDP_DOT3_MAU_10GIGBASESW + 1;

	lldp_decode_start[LLDP_DOT3_POWER_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_DOT3_POWER_PSE, PSE, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_POWER_PD, PD, index);
	index += LLDP_DOT3_POWER_PD + 1;

/* Dot3 Power Pairs (RFC 3621) */
	lldp_decode_start[LLDP_DOT3_POWERPAIRS_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_DOT3_POWERPAIRS_SIGNAL, SIGNAL, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_POWERPAIRS_SPARE, SPATRE, index);
	index += LLDP_DOT3_POWERPAIRS_SPARE + 1;

/* Dot3 Power type (for 802.3at) */
	lldp_decode_start[LLDP_DOT3_POWER_8023AT_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_DOT3_POWER_8023AT_OFF, OFF, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_POWER_8023AT_TYPE1, TYPE1, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_POWER_8023AT_TYPE2, TYPE2, index);
	index += LLDP_DOT3_POWER_8023AT_TYPE2 + 1;

/* Dot3 power source */
	lldp_decode_start[LLDP_DOT3_POWER_SOURCE_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_DOT3_POWER_SOURCE_UNKNOWN, Unkown, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_POWER_SOURCE_PRIMARY, Primary, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_POWER_SOURCE_BACKUP, Backup, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_POWER_SOURCE_BOTH, Both, index);
	index += LLDP_DOT3_POWER_SOURCE_BOTH + 1;

/* Dot3 power priority */
	lldp_decode_start[LLDP_DOT3_POWER_PRIO_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_DOT3_POWER_PRIO_UNKNOWN, Unkown, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_POWER_PRIO_CRITICAL, Critical, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_POWER_PRIO_HIGH, High, index);
	LLDP_DECODE_SETUP(LLDP_DOT3_POWER_PRIO_LOW, Low, index);
	index += LLDP_DOT3_POWER_PRIO_LOW + 1;

/* PMD Auto-Negotiation Advertised Capability field, from RFC 3636 */
	lldp_decode_start[LLDP_DOT3_LINK_AUTONEG_INDEX] = index;
	LLDP_DECODE_SETUP(bit_set(LLDP_DOT3_LINK_AUTONEG_OTHER), Other, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_DOT3_LINK_AUTONEG_10BASE_T), 10 BASE_T,
			  index);
	LLDP_DECODE_SETUP(bit_set(LLDP_DOT3_LINK_AUTONEG_10BASET_FD),
			  10 BASET_FD, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_DOT3_LINK_AUTONEG_100BASE_T4),
			  100 BASE_T4, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_DOT3_LINK_AUTONEG_100BASE_TX),
			  100 BASE_TX, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_DOT3_LINK_AUTONEG_100BASE_TXFD),
			  100 BASE_TXFD, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_DOT3_LINK_AUTONEG_100BASE_T2),
			  100 BASE_T2, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_DOT3_LINK_AUTONEG_100BASE_T2FD),
			  100 BASE_T2FD, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_DOT3_LINK_AUTONEG_FDX_PAUSE), FDX_PAUSE,
			  index);
	LLDP_DECODE_SETUP(bit_set(LLDP_DOT3_LINK_AUTONEG_FDX_APAUSE),
			  FDX_APAUSE, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_DOT3_LINK_AUTONEG_FDX_SPAUSE),
			  FDX_SPAUSE, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_DOT3_LINK_AUTONEG_FDX_BPAUSE),
			  FDX_BPAUSE, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_DOT3_LINK_AUTONEG_1000BASE_X),
			  1000 BASE_X, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_DOT3_LINK_AUTONEG_1000BASE_XFD),
			  1000 BASE_XFD, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_DOT3_LINK_AUTONEG_1000BASE_T),
			  1000 BASE_T, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_DOT3_LINK_AUTONEG_1000BASE_TFD),
			  1000 BASE_TFD, index);
	index += bit_set(LLDP_DOT3_LINK_AUTONEG_OTHER) + 1;

/* Capabilities */
	lldp_decode_start[LLDP_CAP_INDEX] = index;
	LLDP_DECODE_SETUP(bit_set(LLDP_CAP_OTHER), Other, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_CAP_REPEATER), Repeater, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_CAP_BRIDGE), Bridge, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_CAP_WLAN), WLAN, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_CAP_ROUTER), Router, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_CAP_TELEPHONE), Telephone, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_CAP_DOCSIS), DOCSIS, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_CAP_STATION), Station, index);
	index += bit_set(LLDP_CAP_STATION) + 1;

	lldp_decode_start[LLDP_PPVID_CAP_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_PPVID_CAP_SUPPORTED, SUPPORTED, index);
	LLDP_DECODE_SETUP(LLDP_PPVID_CAP_ENABLED, ENABLED, index);
	index += bit_set(LLDP_PPVID_CAP_ENABLED) + 1;

/* See http://www.iana.org/assignments/address-family-numbers */
	lldp_decode_start[LLDP_MGMT_ADDR_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_MGMT_ADDR_NONE, None, index);
	LLDP_DECODE_SETUP(LLDP_MGMT_ADDR_IP4, IPv4, index);
	LLDP_DECODE_SETUP(LLDP_MGMT_ADDR_IP6, IPv6, index);
	index += LLDP_MGMT_ADDR_IP6 + 1;

	lldp_decode_start[LLDP_MGMT_IFACE_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_MGMT_IFACE_UNKNOWN, Unknown, index);
	LLDP_DECODE_SETUP(LLDP_MGMT_IFACE_IFINDEX, IF_INDEX, index);
	LLDP_DECODE_SETUP(LLDP_MGMT_IFACE_SYSPORT, Sysroot, index);
	index += LLDP_MGMT_IFACE_SYSPORT + 1;

	lldp_decode_start[LLDP_MED_CLASS_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_MED_CLASS_I, CLASS_I, index);
	LLDP_DECODE_SETUP(LLDP_MED_CLASS_II, CLASS_II, index);
	LLDP_DECODE_SETUP(LLDP_MED_CLASS_III, CLASS_III, index);
	LLDP_DECODE_SETUP(LLDP_MED_NETWORK_DEVICE, Network_Device, index);
	index += LLDP_MED_NETWORK_DEVICE + 1;

/* LLDP MED application ttpes */
	lldp_decode_start[LLDP_MED_APPTYPE_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_MED_APPTYPE_UNDEFINED, Undefined, index);
	LLDP_DECODE_SETUP(LLDP_MED_APPTYPE_VOICE, VOICE, index);
	LLDP_DECODE_SETUP(LLDP_MED_APPTYPE_VOICESIGNAL, VOICESIGNAL, index);
	LLDP_DECODE_SETUP(LLDP_MED_APPTYPE_GUESTVOICE, GUESTVOICE, index);
	LLDP_DECODE_SETUP(LLDP_MED_APPTYPE_GUESTVOICESIGNAL, GUESTVOICESIGNAL,
			  index);
	LLDP_DECODE_SETUP(LLDP_MED_APPTYPE_SOFTPHONEVOICE, SOFTPHONEVOICE,
			  index);
	LLDP_DECODE_SETUP(LLDP_MED_APPTYPE_VIDEOCONFERENCE, VIDEOCONFERENCE,
			  index);
	LLDP_DECODE_SETUP(LLDP_MED_APPTYPE_VIDEOSTREAM, VIDEOSTREAM, index);
	LLDP_DECODE_SETUP(LLDP_MED_APPTYPE_VIDEOSIGNAL, VIDEOSIGNAL, index);
	index += LLDP_MED_APPTYPE_LAST + 1;

/* LLDP MED location formats */
	lldp_decode_start[LLDP_MED_LOCFORMAT_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_MED_LOCFORMAT_COORD, COORD, index);
	LLDP_DECODE_SETUP(LLDP_MED_LOCFORMAT_CIVIC, CIVIC, index);
	LLDP_DECODE_SETUP(LLDP_MED_LOCFORMAT_ELIN, ELIN, index);
	index += LLDP_MED_LOCFORMAT_LAST + 1;

	lldp_decode_start[LLDP_MED_LOCATION_GEOID_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_MED_LOCATION_GEOID_WGS84, WGS84, index);
	LLDP_DECODE_SETUP(LLDP_MED_LOCATION_GEOID_NAD83, NAD83, index);
	LLDP_DECODE_SETUP(LLDP_MED_LOCATION_GEOID_NAD83_MLLW, NAD83_MLLW,
			  index);
	index += LLDP_MED_LOCATION_GEOID_NAD83_MLLW + 1;

	lldp_decode_start[LLDP_MED_LOCATION_ALTITUDE_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_MED_LOCATION_ALTITUDE_UNIT_METER, Meter, index);
	LLDP_DECODE_SETUP(LLDP_MED_LOCATION_ALTITUDE_UNIT_FLOOR, Floor, index);
	index += LLDP_MED_LOCATION_ALTITUDE_UNIT_FLOOR + 1;

/* LLDP MED power related constants */
	lldp_decode_start[LLDP_MED_POW_TYPE_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_MED_POW_TYPE_PSE, PSE, index);
	LLDP_DECODE_SETUP(LLDP_MED_POW_TYPE_PD, PD, index);
	LLDP_DECODE_SETUP(LLDP_MED_POW_TYPE_RESERVED, RESERVED, index);
	index += LLDP_MED_POW_TYPE_RESERVED + 1;

	lldp_decode_start[LLDP_MED_POW_SOURCE_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_MED_POW_SOURCE_UNKNOWN, UNKNOWN, index);
	LLDP_DECODE_SETUP(LLDP_MED_POW_SOURCE_PRIMARY, PRIMARY, index);
	LLDP_DECODE_SETUP(LLDP_MED_POW_SOURCE_BACKUP, BACKUP, index);
	LLDP_DECODE_SETUP(LLDP_MED_POW_SOURCE_RESERVED, RESERVED, index);
	LLDP_DECODE_SETUP(LLDP_MED_POW_SOURCE_PSE, PSE, index);
	LLDP_DECODE_SETUP(LLDP_MED_POW_SOURCE_LOCAL, LOCAL, index);
	LLDP_DECODE_SETUP(LLDP_MED_POW_SOURCE_BOTH, BOTH, index);
	index += LLDP_MED_POW_SOURCE_BOTH + 1;

	lldp_decode_start[LLDP_MED_POW_PRIO_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_MED_POW_PRIO_UNKNOWN, Unknown, index);
	LLDP_DECODE_SETUP(LLDP_MED_POW_PRIO_CRITICAL, Critical, index);
	LLDP_DECODE_SETUP(LLDP_MED_POW_PRIO_HIGH, High, index);
	LLDP_DECODE_SETUP(LLDP_MED_POW_PRIO_LOW, Low, index);
	index += LLDP_MED_POW_PRIO_LOW + 1;

/* LLDP MED capabilities */
	lldp_decode_start[LLDP_MED_CAP_INDEX] = index;
	LLDP_DECODE_SETUP(bit_set(LLDP_MED_CAP_CAP), CAP, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_MED_CAP_POLICY), POLICY, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_MED_CAP_LOCATION), LOCATION, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_MED_CAP_MDI_PSE), PSE, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_MED_CAP_MDI_PD), PD, index);
	LLDP_DECODE_SETUP(bit_set(LLDP_MED_CAP_IV), IV, index);
	index += bit_set(LLDP_MED_CAP_IV) + 1;

/* Protocol constants for multi-protocol lldpd */
	lldp_decode_start[LLDP_MODE_INDEX] = index;
	LLDP_DECODE_SETUP(LLDPD_MODE_LLDP, LLDP, index);
	LLDP_DECODE_SETUP(LLDPD_MODE_CDPV1, CDPV1, index);
	LLDP_DECODE_SETUP(LLDPD_MODE_CDPV2, CDPV2, index);
	LLDP_DECODE_SETUP(LLDPD_MODE_SONMP, SONMP, index);
	LLDP_DECODE_SETUP(LLDPD_MODE_EDP, EDP, index);
	LLDP_DECODE_SETUP(LLDPD_MODE_FDP, FDP, index);
	index += LLDPD_MODE_MAX + 1;

/* Bond slave src mac type constants */
	lldp_decode_start[LLDP_BOND_SLAVE_SRC_MAC_TYPE_INDEX] = index;
	LLDP_DECODE_SETUP(LLDP_BOND_SLAVE_SRC_MAC_TYPE_UNKNOWN, Unkown, index);
	LLDP_DECODE_SETUP(LLDP_BOND_SLAVE_SRC_MAC_TYPE_REAL, Real, index);
	LLDP_DECODE_SETUP(LLDP_BOND_SLAVE_SRC_MAC_TYPE_ZERO, Zero, index);
	LLDP_DECODE_SETUP(LLDP_BOND_SLAVE_SRC_MAC_TYPE_FIXED, Fixed, index);
	LLDP_DECODE_SETUP(LLDP_BOND_SLAVE_SRC_MAC_TYPE_LOCALLY_ADMINISTERED,
			  Locally_Administered, index);
	lldp_decode_start[LLDP_BOND_SLAVE_SRC_MAC_TYPE_INDEX + 1] =
		index + LLDP_BOND_SLAVE_SRC_MAC_TYPE_LOCALLY_ADMINISTERED;

}

/*
 * LLDP Helper Functions
 */

static void
lldpd_reset(struct lldpd *cfg, struct lldpd_hardware *hw)
{
	/* If hw is NULL reset all hw else reset only specific hw */
	if (!hw) {
		struct lldpd_hardware *hardware;

		/* h_flags is set to 0 for each port. We will cleanup all ports */
		TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries)
			hardware->h_flags = 0;
	} else {
		hw->h_flags = 0;
	}
	lldpd_cleanup(cfg);
}                               /* lldpd_reset */

#define CHANGED(x,y) (x != y)
#define CHANGED_STR(x,y) (!(x == y || (x && y && !strcmp(x,y))))

static bool
lldpd_apply_tlv_configs(const struct ovsrec_open_vswitch *ovs,
                        const char *tlv_name, u_int8_t * g_lldp_tlv_cfg)
{
	bool tlv_setting;
	bool send_update = 0;

	tlv_setting = smap_get_bool(&ovs->other_config, tlv_name,
				    SYSTEM_OTHER_CONFIG_MAP_LLDP_TLV_DEFAULT);
	if (CHANGED(tlv_setting, *g_lldp_tlv_cfg)) {
		*g_lldp_tlv_cfg = tlv_setting;
		VLOG_INFO("Configured %s=%d", tlv_name, *g_lldp_tlv_cfg);
		send_update = 1;
	}

	return send_update;
}                               /* lldpd_apply_tlv_configs */

/* Check if ip is a valid IPv4 or IPv6 address */
static bool
validate_ip(char *ip)
{
	struct interfaces_address *addr;

	if (ip && strpbrk(ip, "!,*?") == NULL) {
		if (inet_pton(lldpd_af(LLDPD_AF_IPV4), ip, &addr) == 1 ||
		    inet_pton(lldpd_af(LLDPD_AF_IPV6), ip, &addr) == 1)
			return true;
	}
	return false;
}                               /* validate_ip */

/*
 * Interface management functions
 */

static void
del_old_db_interface(struct shash_node *sh_node)
{
	if (sh_node) {
		struct interface_data *itf = sh_node->data;

		/*
		 * If the lldp_hardware is also cleaned up
		 * remove the entry else just nullify
		 * ovsdb record handle
		 */
		if (itf && !itf->hw) {
			free(itf->name);
			free(sh_node->data);
			shash_delete(&all_interfaces, sh_node);
		} else {
			itf->ifrow = NULL;
		}

	}
}                               /* del_old_interface */

static void
add_new_db_interface(const struct ovsrec_interface *ifrow)
{
	struct interface_data *new_itf = NULL;
	struct shash_node *sh_node = shash_find(&all_interfaces, ifrow->name);

	if (!ifrow)
		return;

	VLOG_DBG("Interface %s being added!\n", ifrow->name);

	if (!sh_node) {
		/* Allocate structure to save state information for this interface. */
		new_itf = xcalloc(1, sizeof *new_itf);

		if (!shash_add_once(&all_interfaces, ifrow->name, new_itf)) {
			VLOG_WARN("Interface %s specified twice", ifrow->name);
			free(new_itf);
		} else {
			new_itf->name = xstrdup(ifrow->name);
			new_itf->ifrow = ifrow;
			new_itf->synced_from_db = 0;
			VLOG_DBG("Created local data for interface %s", ifrow->name);
		}
	} else {
		new_itf = sh_node->data;
		new_itf->ifrow = ifrow;
	}
}                               /* add_new_interface */

#define CHK_AND_APPLY_ITF_CONFIG_CHANGE(cfg, itf, val, cfg_changed)	\
        if(itf->hw) {							\
		if(CHANGED(itf->hw->h_enable_dir, val)) {		\
			itf->hw->h_enable_dir = val;			\
			lldpd_reset(cfg, itf->hw);			\
			lldpd_update_localports(cfg);			\
			cfg_changed++;					\
		}							\
        }								\

static int
handle_interfaces_config_mods(struct shash *sh_idl_interfaces,
                              const struct ovsrec_system *system, struct lldpd *cfg)
{
	struct shash_node *sh_node;
	int rc = 0;

	/* Loop through all the current interfaces and figure out how many have
	 * config changes that need action. */
	SHASH_FOR_EACH(sh_node, &all_interfaces) {
		bool cfg_changed = false;
		struct interface_data *itf = sh_node->data;
		const struct ovsrec_interface *ifrow =
			shash_find_data(sh_idl_interfaces, sh_node->name);

		if (ifrow && (OVSREC_IDL_IS_ROW_INSERTED(ifrow, idl_seqno) ||
			      OVSREC_IDL_IS_ROW_MODIFIED(ifrow, idl_seqno))) {

			/* Update ifrow handle */
			itf->ifrow = ifrow;
			/* Check for other_config:lldp_enable_dir changes */
			const char *ifrow_other_config_lldp_enable_dir =
				smap_get(&ifrow->other_config,
					 INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR);

			if (ifrow_other_config_lldp_enable_dir) {
				if (strcmp(ifrow_other_config_lldp_enable_dir,
					   INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_OFF) == 0) {
					CHK_AND_APPLY_ITF_CONFIG_CHANGE(cfg, itf,
									HARDWARE_ENABLE_DIR_OFF,
									cfg_changed);
				} else if (strcmp (ifrow_other_config_lldp_enable_dir,
					           INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_RX) == 0) {
						CHK_AND_APPLY_ITF_CONFIG_CHANGE(cfg, itf,
										HARDWARE_ENABLE_DIR_RX,
										cfg_changed);
				} else if (strcmp (ifrow_other_config_lldp_enable_dir,
						  INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_TX) == 0) {
						CHK_AND_APPLY_ITF_CONFIG_CHANGE(cfg, itf,
										HARDWARE_ENABLE_DIR_TX,
										cfg_changed);
				} else if (strcmp (ifrow_other_config_lldp_enable_dir,
					           INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_RXTX) == 0) {
						CHK_AND_APPLY_ITF_CONFIG_CHANGE(cfg, itf,
										HARDWARE_ENABLE_DIR_RXTX,
										cfg_changed);
				}
				VLOG_DBG("lldp status change on interface %s: %s",
					 ifrow->name, ifrow_other_config_lldp_enable_dir);
			} else {
				CHK_AND_APPLY_ITF_CONFIG_CHANGE(cfg, itf,
								HARDWARE_ENABLE_DIR_RXTX,
								cfg_changed);
			}

			/* Check for link_state change */
			bool link_state_bool = false;

			if (ifrow && ifrow->link_state &&
			    !strcmp(ifrow->link_state, OVSREC_INTERFACE_LINK_STATE_UP)) {
				link_state_bool = true;
			}
			if (itf->hw && CHANGED(itf->hw->h_link_state, link_state_bool)) {
				VLOG_INFO("link state change on interface%s: %s", ifrow->name,
					  ifrow->link_state);
				itf->hw->h_link_state = link_state_bool;

				if (link_state_bool && system) {
                                        itf->hw->h_reinit_delay = smap_get_int(&system->other_config,
                                                                                SYSTEM_OTHER_CONFIG_MAP_LLDP_REINIT,
                                                                                SYSTEM_OTHER_CONFIG_MAP_LLDP_REINIT_DEFAULT);
                                        VLOG_INFO("Configured lldp reinit time [%d]s on interface%s",
                                                                        itf->hw->h_reinit_delay, ifrow->name);
                                }
				cfg_changed++;
			}
		}

		if (cfg_changed) {
			/* Update interface configuration */
			rc++;
		}
	}

	return rc;
}                               /* handle_interfaces_config_mods */

static void
lldpd_apply_interface_changes(struct ovsdb_idl *idl,
                              struct lldpd *g_lldp_cfg, bool * send_now)
{
	int rc = 0;
	const struct ovsrec_interface *ifrow;
	const struct ovsrec_system *system;
	struct shash sh_idl_interfaces;
	struct shash_node *sh_node, *sh_next;

	system = ovsrec_system_first(idl);

	/* Collect all the interfaces in the DB. */
	shash_init(&sh_idl_interfaces);
	OVSREC_INTERFACE_FOR_EACH(ifrow, idl) {
		/*
		 * Check if any table changes present
		 */
		if (ifrow && !OVSREC_IDL_ANY_TABLE_ROWS_INSERTED(ifrow, idl_seqno) &&
		    !OVSREC_IDL_ANY_TABLE_ROWS_DELETED(ifrow, idl_seqno) &&
		    !OVSREC_IDL_ANY_TABLE_ROWS_MODIFIED(ifrow, idl_seqno)) {
			VLOG_DBG("No Interface cfg changes");
			/* Destroy the shash of the IDL interfaces */
			shash_destroy(&sh_idl_interfaces);
			return;
		}

		if (!shash_add_once(&sh_idl_interfaces, ifrow->name, ifrow)) {
			VLOG_WARN("interface %s specified twice", ifrow->name);
		}
	}

	/* Delete old interfaces */
	SHASH_FOR_EACH_SAFE(sh_node, sh_next, &all_interfaces) {
		struct interface_data *new_itf =
			shash_find_data(&sh_idl_interfaces, sh_node->name);
		if (!new_itf) {
			del_old_db_interface(sh_node);
		}
	}
	/* Add new interfaces */
	SHASH_FOR_EACH(sh_node, &sh_idl_interfaces) {
		struct interface_data *new_itf = shash_find_data(&all_interfaces,
								 sh_node->name);

		if (!new_itf || !new_itf->ifrow) {
			VLOG_DBG("Found an added interface %s", sh_node->name);
			add_new_db_interface(sh_node->data);
		}
	}

	/* Check for interfaces that changed and need handling now */
	rc = handle_interfaces_config_mods(&sh_idl_interfaces, system, g_lldp_cfg);

	/* Destroy the shash of the IDL interfaces */
	shash_destroy(&sh_idl_interfaces);

	if (rc)
		*send_now = 1;
}                               /* lldpd_apply_interface_changes */

/*
 * Bridge management functions
 */

static void
lldpd_apply_bridge_changes(struct ovsdb_idl *idl,
                           struct lldpd *g_lldp_cfg, bool * send_now)
{
	const struct ovsrec_bridge *br;

	br = ovsrec_bridge_first(idl);
	/*
	 * Check if any table changes present
	 */
	if (br && !OVSREC_IDL_ANY_TABLE_ROWS_INSERTED(br, idl_seqno) &&
	    !OVSREC_IDL_ANY_TABLE_ROWS_DELETED(br, idl_seqno) &&
	    !OVSREC_IDL_ANY_TABLE_ROWS_MODIFIED(br, idl_seqno)) {
		VLOG_DBG("No Bridge cfg changes");
		return;
	}

	/* If bridge table has a row, enable bridge capabilities */
	if (br) {
		if (!(LOCAL_CHASSIS(g_lldp_cfg)->c_cap_enabled & LLDP_CAP_BRIDGE)) {
			LOCAL_CHASSIS(g_lldp_cfg)->c_cap_enabled |= LLDP_CAP_BRIDGE;
			/* Update the information to local Chasis */
			levent_update_now(g_lldp_cfg);
			*send_now = true;
		}
	} else {
		/*
		 * If bridge capabilty is set then clear since
		 * no bridge entries are left.
		 */
		if (LOCAL_CHASSIS(g_lldp_cfg)->c_cap_enabled & LLDP_CAP_BRIDGE) {
			LOCAL_CHASSIS(g_lldp_cfg)->c_cap_enabled &= ~LLDP_CAP_BRIDGE;
			/* Update the information to local Chasiss */
			levent_update_now(g_lldp_cfg);
			*send_now = true;
		}
	}
}                               /* lldpd_apply_bridge_changes */

/*
 * VRF management functions
 */

static void
lldpd_apply_vrf_changes(struct ovsdb_idl *idl,
                        struct lldpd *g_lldp_cfg, bool * send_now)
{
	const struct ovsrec_vrf *vrf;

	vrf = ovsrec_vrf_first(idl);
	/*
	 * Check if any table changes present
	 */
	if (vrf && !OVSREC_IDL_ANY_TABLE_ROWS_INSERTED(vrf, idl_seqno) &&
	    !OVSREC_IDL_ANY_TABLE_ROWS_DELETED(vrf, idl_seqno) &&
	    !OVSREC_IDL_ANY_TABLE_ROWS_MODIFIED(vrf, idl_seqno)) {
		VLOG_DBG("No VRF cfg changes");
		return;
	}

	/* If VRF table has a row, enable router capabilities */
	if (vrf) {
		if (!(LOCAL_CHASSIS(g_lldp_cfg)->c_cap_enabled & LLDP_CAP_ROUTER)) {
			LOCAL_CHASSIS(g_lldp_cfg)->c_cap_enabled |= LLDP_CAP_ROUTER;
			*send_now = true;
		}
	} else {
		/*
		 * If Router capabililty is set then clear since
		 * no VRF entries are left.
		 */
		if (LOCAL_CHASSIS(g_lldp_cfg)->c_cap_enabled & LLDP_CAP_ROUTER) {
			LOCAL_CHASSIS(g_lldp_cfg)->c_cap_enabled &= ~LLDP_CAP_ROUTER;
			*send_now = true;
		}
	}
}                               /* lldpd_apply_vrf_changes */

/*
 * Port/Vlan management functions
 */
static void
set_lldp_vlan_name_tlv(int64_t vlan, struct lldpd_hardware *hw)
{
	struct lldpd_vlan *v;

	/* Check if the VLAN is already here */
	struct lldpd_port *port = &hw->h_lport;
	char *vlan_name;

	if ((v = (struct lldpd_vlan *)
	     xcalloc(1, sizeof (struct lldpd_vlan))) == NULL)
		return;

	/* Get Vlan name from VLAN table */
	vlan_name = vlan_name_lookup_by_vid(vlan);

	if (!vlan_name) {
		VLOG_INFO("No Vlan found for vlan id %" PRIu64 "", vlan);
		free(v);
		return;
	}

	if ((v->v_name = strdup(vlan_name)) == NULL) {
		free(v);
		return;
	}
	v->v_vid = vlan;

	VLOG_INFO("VLAN ID = %d, VLAN NAME = %s", v->v_vid, v->v_name);

	TAILQ_INSERT_TAIL(&port->p_vlans, v, v_entries);
}                               /* set_lldp_vlan_name_tlv */

static void
set_lldp_trunk_vlans(const struct ovsrec_port *row,
                     struct interface_data *interface)
{

	int i;

	if (interface->hw) {
		for (i = 0; i < row->n_vlan_trunks; i++) {
			set_lldp_vlan_name_tlv(ops_port_get_trunks(row, i), interface->hw);
		}
	}
}                               /* set_lldp_trunk_vlans */

static void
set_lldp_pvid(const struct ovsrec_port *row, struct interface_data *interface)
{
	int native_vid = -1;

	if (!row) {
		VLOG_ERR("NULL row passed to %s", __FUNCTION__);
	}

	/* Get native VID from 'tag' column */
	if ((row->vlan_tag != NULL)) {
		native_vid = ops_port_get_tag(row);
		if (interface->hw) {
			interface->hw->h_lport.p_pvid = native_vid;
			VLOG_INFO("Setting pvid %d", interface->hw->h_lport.p_pvid);

			/* Also set vlan name */
			set_lldp_vlan_name_tlv(native_vid, interface->hw);
		}
	}

}                               /* set_lldp_pvid */

static int
del_old_port(struct shash_node *sh_node)
{
	int rc = 0;
	int k;

	if (sh_node) {
		struct port_data *port = sh_node->data;

		VLOG_INFO("number of interfaces in port = %d", port->n_interfaces);
		/* Clean up lldp hardware vlan info */
		if (port && port->n_interfaces && port->interfaces) {
			for (k = 0; k < port->n_interfaces; k++) {
				struct interface_data *intf = NULL;

				if (port->interfaces[k])
					intf =
						shash_find_data(&all_interfaces,
								port->interfaces[k]->hw->h_ifname);
				if (!intf) {
					continue;
				}
				intf->portdata = NULL;
				VLOG_INFO("Cleaning up vlan info for Interface %s",
					  port->interfaces[k]->hw->h_ifname);
				lldpd_vlan_cleanup(&port->interfaces[k]->hw->h_lport);
				port->interfaces[k]->hw->h_lport.p_pvid = 0;
				rc++;
			}
			free(port->interfaces);
			port->n_interfaces = 0;
		}

		/*
		 * Remove this port from the list of all_ports first.
		 * This is needed to correctly update VLAN membership.
		 */
		shash_delete(&all_ports, sh_node);

		/* Done -  Free the rest of the structure */
		free(port->name);
		free(port);
	}

	return rc;

}                               /* del_old_port */

static int
add_new_port(const struct ovsrec_port *port_row)
{
	struct port_data *new_port = NULL;
	int rc = 0;

	/* Allocate structure to save state information for this port */
	new_port = xzalloc(sizeof (struct port_data));

	if (!shash_add_once(&all_ports, port_row->name, new_port)) {
		VLOG_WARN("Port %s specified twice", port_row->name);
		free(new_port);
	} else {
		new_port->name = xstrdup(port_row->name);

		/* Initialize VLANs to NULL for now */
		new_port->native_vid = -1;
		new_port->trunk_all_vlans = false;
		new_port->vlan_mode = PORT_VLAN_MODE_ACCESS;
		new_port->portrow = port_row;
		VLOG_DBG("Created local data for Port %s", port_row->name);
	}
	return rc;
}                               /* add_new_port */

static void
lldpd_reconfigure_port(struct port_data *port)
{
	int k;
	const struct ovsrec_port *row = NULL;

	if (!port) {
		VLOG_DBG("Port is NULL");
		return;
	}

	row = port->portrow;

	/*
	 * Cleanup old interface list in hashmap and rebuild
	 *
	 */
	if (port->interfaces && port->n_interfaces) {
		for (k = 0; k < port->n_interfaces; k++) {
			if(port->interfaces[k] && port->interfaces[k]->hw) {
				VLOG_INFO("Cleaning up vlan info for Interface %s",
						   port->interfaces[k]->hw->h_ifname);
				lldpd_vlan_cleanup(&port->interfaces[k]->hw->h_lport);
			}
		}
		free(port->interfaces);
	}

	port->n_interfaces = row->n_interfaces;
	port->interfaces = xzalloc(port->n_interfaces * sizeof (struct interface_data *));

	/*
	 * Get vlan mode
	 *
	 */
	for (k = 0; k < row->n_interfaces; k++) {
		struct interface_data *intf;
		struct ovsrec_interface *iface = row->interfaces[k];

		intf = shash_find_data(&all_interfaces, iface->name);
                if (!intf) {
                    continue;
                }
		/*
		 * - Add lldp hardware to our structure to allow
		 *   cleanup of lldp hardware in case row gets deleted.
		 * - Cleanup existing lldp hardware vlan info
		 *   since the code will reconstruct this again.
		 *
		 */
		if (intf->hw) {
			intf->portdata = port;
			port->interfaces[k] = intf;
		}

		/* Get vlan_mode first. */
		if (row->vlan_mode) {
			if (strcmp(row->vlan_mode, OVSREC_PORT_VLAN_MODE_ACCESS) == 0) {
				port->vlan_mode = PORT_VLAN_MODE_ACCESS;
			} else if (strcmp(row->vlan_mode, OVSREC_PORT_VLAN_MODE_TRUNK) == 0) {
				port->vlan_mode = PORT_VLAN_MODE_TRUNK;
			} else if (strcmp(row->vlan_mode,
					   OVSREC_PORT_VLAN_MODE_NATIVE_TAGGED) == 0) {
				port->vlan_mode = PORT_VLAN_MODE_NATIVE_TAGGED;
			} else if (strcmp(row->vlan_mode,
					  OVSREC_PORT_VLAN_MODE_NATIVE_UNTAGGED) == 0) {
				port->vlan_mode = PORT_VLAN_MODE_NATIVE_UNTAGGED;
			} else {
				/* Should not happen.  Assume TRUNK mode to match
							 * bridge.c. */
				VLOG_ERR("Invalid VLAN mode %s", row->vlan_mode);
				port->vlan_mode = PORT_VLAN_MODE_TRUNK;
			}
		} else {
			/* 'vlan_mode' column is not specified.  Follow default
			 * rules: - If 'tag' contains a value, the port is an
			 * access port.  - Otherwise, the port is a trunk port. */
			if (row->vlan_tag != NULL) {
				port->vlan_mode = PORT_VLAN_MODE_ACCESS;
			} else {
				port->vlan_mode = PORT_VLAN_MODE_TRUNK;
			}
		}

		/*
		 * Set native vlan config into lldp
		 * Ignore tag (pvid) column when in trunk mode,
		 * will be applied only when in
		 * - access mode
		 * - native tagged
		 * - native untagged
		 *
		 */
		if (port->vlan_mode != PORT_VLAN_MODE_TRUNK)
			set_lldp_pvid(row, intf);

		/*
		 * Set Trunk Vlans into lldp
		 * Ignore trunks in access mode
		 *
		 */
		if (port->vlan_mode != PORT_VLAN_MODE_ACCESS)
			set_lldp_trunk_vlans(row, intf);
	}
	return;
}

static int
handle_port_config_mods(struct shash *sh_idl_ports, struct lldpd *cfg)
{
	struct shash_node *sh_node;
	int rc = 0;

	/* Check for changes in the port row entries */
	SHASH_FOR_EACH(sh_node, &all_ports) {
		const struct ovsrec_port *port_row = shash_find_data(sh_idl_ports,
								sh_node->name);

                if (!port_row) {
			continue;
		}
		/* Check for changes to row */
		if (OVSREC_IDL_IS_ROW_INSERTED(port_row, idl_seqno) ||
		    OVSREC_IDL_IS_ROW_MODIFIED(port_row, idl_seqno)) {
			((struct port_data *)sh_node->data)->portrow = port_row;
			lldpd_reconfigure_port((struct port_data *)sh_node->data);
			rc++;
		}
	}
	return rc;
}

static void
lldpd_apply_port_changes(struct ovsdb_idl *idl,
                         struct lldpd *g_lldp_cfg, bool * send_now)
{
	struct shash sh_idl_ports;
	const struct ovsrec_port *row;
	struct shash_node *sh_node, *sh_next;

	row = ovsrec_port_first(idl);

	/*
	 * Check if any table change present
	 */
	if (row && !OVSREC_IDL_ANY_TABLE_ROWS_INSERTED(row, idl_seqno) &&
	    !OVSREC_IDL_ANY_TABLE_ROWS_DELETED(row, idl_seqno) &&
	    !OVSREC_IDL_ANY_TABLE_ROWS_MODIFIED(row, idl_seqno)) {
		VLOG_DBG("No Port cfg changes");
		return;
	}

	/* Collect all the ports in the DB */
	shash_init(&sh_idl_ports);
	OVSREC_PORT_FOR_EACH(row, idl) {
		if (!shash_add_once(&sh_idl_ports, row->name, row)) {
			VLOG_WARN("port %s specified twice", row->name);
		}
	}

	/* Delete old ports */
	SHASH_FOR_EACH_SAFE(sh_node, sh_next, &all_ports) {
		struct port_data *port = shash_find_data(&sh_idl_ports, sh_node->name);

		if (!port) {
			VLOG_DBG("Found a deleted port %s", sh_node->name);
			if (del_old_port(sh_node)) {
				*send_now = 1;
			}
		}
	}

	/* Add new ports */
	SHASH_FOR_EACH(sh_node, &sh_idl_ports) {
		struct port_data *new_port =
			shash_find_data(&all_ports, sh_node->name);
		if (!new_port) {
			VLOG_DBG("Found an added port %s", sh_node->name);
			if (add_new_port(sh_node->data)) {
				*send_now = 1;
			}
		}
	}

	/* Handle any config changes */
	if (handle_port_config_mods(&sh_idl_ports, g_lldp_cfg))
		*send_now = 1;

	/* Destroy the shash of the IDL ports */
	shash_destroy(&sh_idl_ports);
}                               /* lldpd_apply_port_changes */

static char *
vlan_name_lookup_by_vid(int64_t vid)
{
	const struct ovsrec_vlan *row;

	/* Collect all the VLANs in the DB */

	OVSREC_VLAN_FOR_EACH(row, idl) {
		if (row->id == vid) {
			return row->name;
		}
	}

	return NULL;

}                               /* vlan_name_lookup_by_vid */

static bool
set_lldp_mgmt_address(const struct ovsrec_system *ovs,
                      struct lldpd *g_lldp_cfg, bool * send_now)
{
	const char *lldp_mgmt_pattern;
	bool update_now = 0 ;
	int af;
	struct in6_addr addr;
	size_t addr_size;

	if(!ovs) {
		VLOG_ERR("NULL row passed to %s", __FUNCTION__);
		return update_now;
	}

	lldpd_chassis_mgmt_cleanup(LOCAL_CHASSIS(g_lldp_cfg));

	lldp_mgmt_pattern = smap_get(&ovs->other_config,
				     SYSTEM_OTHER_CONFIG_MAP_LLDP_MGMT_ADDR);

	if (lldp_mgmt_pattern != NULL) {
		for (af = LLDPD_AF_UNSPEC + 1; af != LLDPD_AF_LAST; af++) {
			switch (af) {
			case LLDPD_AF_IPV4:
				addr_size = sizeof(struct in_addr);
				break;
			case LLDPD_AF_IPV6:
				addr_size = sizeof(struct in6_addr);
				break;
			default:
				assert(0);
			}

			if (inet_pton(lldpd_af(af), lldp_mgmt_pattern, &addr) == 1)
				break;
		}

		if (af == LLDPD_AF_LAST) {
			VLOG_ERR("Configured lldp mgmt_pattern is not a valid IP addr [%s]", lldp_mgmt_pattern);
		} else {
			struct lldpd_mgmt *mgmt;
			mgmt = lldpd_alloc_mgmt(af, &addr, addr_size, 0);

			if (mgmt == NULL) {
				VLOG_ERR("Unable to configure lldp mgmt_pattern, out of memory error");
				return update_now;
			}

			if (CHANGED_STR(lldp_mgmt_pattern, g_lldp_cfg->g_config.c_mgmt_pattern)) {
				if (g_lldp_cfg->g_config.c_mgmt_pattern != NULL)
					free(g_lldp_cfg->g_config.c_mgmt_pattern);

				g_lldp_cfg->g_config.c_mgmt_pattern = xstrdup(lldp_mgmt_pattern);
				VLOG_DBG("Configured lldp mgmt_pattern is [%s]", lldp_mgmt_pattern);
				log_event("MGMT_PATTERN", EV_KV("value", "%s", lldp_mgmt_pattern));
			}
                        TAILQ_INSERT_TAIL(&LOCAL_CHASSIS(g_lldp_cfg)->c_mgmt, mgmt, m_entries);
		}

		update_now = 1;
		*send_now = 1;
		return update_now;
	}

	/* If not mgmt address is configured, fetch mgmt interface IPV4 and IPV6 address*/
	lldp_mgmt_pattern = NULL;
	lldp_mgmt_pattern = smap_get(&ovs->mgmt_intf_status, SYSTEM_MGMT_INTF_MAP_IP);

	if (lldp_mgmt_pattern) {
		lldp_mgmt_pattern = strtok((char *) lldp_mgmt_pattern, "/");

		if (lldp_mgmt_pattern && validate_ip((char *) lldp_mgmt_pattern)) {
			struct lldpd_mgmt *mgmt;
			af = LLDPD_AF_IPV4;
			addr_size = sizeof(struct in_addr);

			inet_pton(lldpd_af(af), lldp_mgmt_pattern, &addr);

			mgmt = lldpd_alloc_mgmt(af, &addr, addr_size, 0);
			if (mgmt == NULL) {
				VLOG_ERR("Unable to configure lldp IPV4 mgmt_pattern, out of memory error");
				return update_now;
			}

			VLOG_DBG("Configured lldp IPV4 mgmt_pattern is [%s]", lldp_mgmt_pattern);
			TAILQ_INSERT_TAIL(&LOCAL_CHASSIS(g_lldp_cfg)->c_mgmt, mgmt, m_entries);
			update_now = 1;
			*send_now = 1;
		} else {
			VLOG_ERR("Configured lldp IPV4 mgmt_pattern is not a valid IP addr [%s]", lldp_mgmt_pattern);
		}
	}

	lldp_mgmt_pattern = NULL;
	lldp_mgmt_pattern = smap_get(&ovs->mgmt_intf_status, SYSTEM_MGMT_INTF_MAP_IPV6);

	if (lldp_mgmt_pattern) {
		lldp_mgmt_pattern = strtok((char *) lldp_mgmt_pattern, "/");
		af = LLDPD_AF_IPV6;
		addr_size = sizeof(struct in6_addr);

		if (lldp_mgmt_pattern && inet_pton(lldpd_af(af), lldp_mgmt_pattern, &addr)) {
			struct lldpd_mgmt *mgmt;

			mgmt = lldpd_alloc_mgmt(af, &addr, addr_size, 0);
			if (mgmt == NULL) {
				VLOG_ERR("Unable to configure lldp IPV6 mgmt_pattern, out of memory error");
				return update_now;
			}

			VLOG_DBG("Configured lldp IPV6 mgmt_pattern is [%s]", lldp_mgmt_pattern);
			TAILQ_INSERT_TAIL(&LOCAL_CHASSIS(g_lldp_cfg)->c_mgmt, mgmt, m_entries);
			update_now = 1;
			*send_now = 1;
		}else {
			VLOG_ERR("Configured lldp IPV6 mgmt_pattern is not a valid IP addr [%s]", lldp_mgmt_pattern);
		}
	}
	return update_now;
}	/* set_lldp_mgmt_address */

/*
 * Configuration management functions
 */

static void
lldpd_apply_global_changes(struct ovsdb_idl *idl,
                           struct lldpd *g_lldp_cfg, bool * send_now)
{
	int lldp_enabled = -1, tx_timer = 0, tx_hold_multiplier = 0;
	int i;
	bool update_now = 0;
	const struct ovsrec_system *ovs;
	struct ovsdb_idl_txn *lldp_clear_all_nbr_txn = NULL;
	bool nbr_change;

	ovs = ovsrec_system_first(idl);

	/*
	 * Check if any table changes present
	 */
	if (ovs && !OVSREC_IDL_ANY_TABLE_ROWS_INSERTED(ovs, idl_seqno) &&
	    !OVSREC_IDL_ANY_TABLE_ROWS_DELETED(ovs, idl_seqno) &&
	    !OVSREC_IDL_ANY_TABLE_ROWS_MODIFIED(ovs, idl_seqno)) {
		VLOG_DBG("No Open_vSwitch cfg changes");
		return;
	}

	if (ovs) {
		tx_timer = smap_get_int(&ovs->other_config,
					SYSTEM_OTHER_CONFIG_MAP_LLDP_TX_INTERVAL,
					SYSTEM_OTHER_CONFIG_MAP_LLDP_TX_INTERVAL_DEFAULT);

		/* Check the transmit interval and update */
		if (CHANGED(tx_timer, g_lldp_cfg->g_config.c_tx_interval) &&
		    tx_timer >= SYSTEM_OTHER_CONFIG_MAP_LLDP_TX_INTERVAL_MIN &&
		    tx_timer <= SYSTEM_OTHER_CONFIG_MAP_LLDP_TX_INTERVAL_MAX) {
			g_lldp_cfg->g_config.c_tx_interval = tx_timer;
			LOCAL_CHASSIS(g_lldp_cfg)->c_ttl = MIN(UINT16_MAX,
							       (g_lldp_cfg->g_config.
								c_tx_interval *
								g_lldp_cfg->g_config.
								c_tx_hold));
			VLOG_INFO("Configured lldp  tx-timer [%d]",
				  g_lldp_cfg->g_config.c_tx_interval);
                        /* Log an event log */
                        log_event("LLDP_TX_TIMER", EV_KV("value", "%d",
                            g_lldp_cfg->g_config.c_tx_interval));
			*send_now = 1;
		}

		/* Check for hold time and update */
		tx_hold_multiplier = smap_get_int(&ovs->other_config,
						  SYSTEM_OTHER_CONFIG_MAP_LLDP_HOLD,
						  SYSTEM_OTHER_CONFIG_MAP_LLDP_HOLD_DEFAULT);
		if (CHANGED(tx_hold_multiplier, g_lldp_cfg->g_config.c_tx_hold) &&
		    tx_hold_multiplier >= SYSTEM_OTHER_CONFIG_MAP_LLDP_HOLD_MIN &&
		    tx_hold_multiplier <= SYSTEM_OTHER_CONFIG_MAP_LLDP_HOLD_MAX) {
			g_lldp_cfg->g_config.c_tx_hold = tx_hold_multiplier;
			LOCAL_CHASSIS(g_lldp_cfg)->c_ttl = MIN(UINT16_MAX,
							       (g_lldp_cfg->g_config.
								c_tx_interval *
								g_lldp_cfg->g_config.
								c_tx_hold));
			VLOG_INFO("Configured lldp  tx-hold [%d]",
				  g_lldp_cfg->g_config.c_tx_hold);
                        log_event("LLDP_TX_HOLD", EV_KV("hold", "%d",
                            g_lldp_cfg->g_config.c_tx_hold));
			*send_now = 1;
		}

		update_now = set_lldp_mgmt_address(ovs, g_lldp_cfg, send_now);

		/* LLDP TLV Configuration */
		if (lldpd_apply_tlv_configs(ovs,
					    SYSTEM_OTHER_CONFIG_MAP_LLDP_TLV_SYS_NAME_ENABLE,
					    &g_lldp_cfg->g_config.
					    c_lldp_tlv_sys_name_enable)) {
			*send_now = 1;
		}

		if (lldpd_apply_tlv_configs(ovs,
					    SYSTEM_OTHER_CONFIG_MAP_LLDP_TLV_SYS_DESC_ENABLE,
					    &g_lldp_cfg->g_config.
					    c_lldp_tlv_sys_desc_enable)) {
			*send_now = 1;
		}

		if (lldpd_apply_tlv_configs(ovs,
					    SYSTEM_OTHER_CONFIG_MAP_LLDP_TLV_SYS_CAP_ENABLE,
					    &g_lldp_cfg->g_config.
					    c_lldp_tlv_sys_cap_enable)) {
			*send_now = 1;
		}

		if (lldpd_apply_tlv_configs(ovs,
					    SYSTEM_OTHER_CONFIG_MAP_LLDP_TLV_MGMT_ADDR_ENABLE,
					    &g_lldp_cfg->g_config.
					    c_lldp_tlv_mgmt_addr_enable)) {
			*send_now = 1;
		}

		if (lldpd_apply_tlv_configs(ovs,
					    SYSTEM_OTHER_CONFIG_MAP_LLDP_TLV_PORT_DESC_ENABLE,
					    &g_lldp_cfg->g_config.
					    c_lldp_tlv_port_desc_enable)) {
			*send_now = 1;
		}

		if (lldpd_apply_tlv_configs(ovs,
					    SYSTEM_OTHER_CONFIG_MAP_LLDP_TLV_PORT_VLAN_ID_ENABLE,
					    &g_lldp_cfg->g_config.
					    c_lldp_tlv_port_vlanid_enable)) {
			*send_now = 1;
		}

		if (lldpd_apply_tlv_configs(ovs,
					    SYSTEM_OTHER_CONFIG_MAP_LLDP_TLV_PORT_PROTO_VLAN_ID_ENABLE,
					    &g_lldp_cfg->g_config.
					    c_lldp_tlv_port_proto_vlanid_enable)) {
			*send_now = 1;
		}

		if (lldpd_apply_tlv_configs(ovs,
					    SYSTEM_OTHER_CONFIG_MAP_LLDP_TLV_PORT_VLAN_NAME_ENABLE,
					    &g_lldp_cfg->g_config.
					    c_lldp_tlv_port_vlan_name_enable)) {
			*send_now = 1;
		}

		if (lldpd_apply_tlv_configs(ovs,
					    SYSTEM_OTHER_CONFIG_MAP_LLDP_TLV_PORT_PROTO_ID_ENABLE,
					    &g_lldp_cfg->g_config.
					    c_lldp_tlv_port_proto_id_enable)) {
			*send_now = 1;
		}

		lldp_enabled = smap_get_bool(&ovs->other_config,
					     SYSTEM_OTHER_CONFIG_MAP_LLDP_ENABLE,
					     SYSTEM_OTHER_CONFIG_MAP_LLDP_ENABLE_DEFAULT);
		/* Check for lldp enable and update */
		bool is_any_protocol_enabled = false;

		for (i = 0; g_lldp_cfg->g_protocols[i].mode != 0; i++) {
			if (g_lldp_cfg->g_protocols[i].mode == LLDPD_MODE_LLDP) {
				if (CHANGED(lldp_enabled,
					    g_lldp_cfg->g_protocols[i].enabled)) {
					g_lldp_cfg->g_protocols[i].enabled = lldp_enabled;
					lldpd_reset(g_lldp_cfg, NULL);
					lldpd_update_localports(g_lldp_cfg);
					if (lldp_enabled) {
						*send_now = 1;
					}
					VLOG_INFO("lldp %s", g_lldp_cfg->g_protocols[i].enabled ?
						  "enabled" : "disabled");
                                        /* Log an Event log as well */
                                        if(g_lldp_cfg->g_protocols[i].enabled) {
                                            log_event("LLDP_ENABLED", NULL);
                                        }
                                        else {
                                            log_event("LLDP_DISABLED", NULL);
                                        }
				}
				if (!lldp_enabled) {
					lldp_clear_all_nbr_txn = ovsdb_idl_txn_create(idl);
					nbr_change = lldpd_ovsdb_clear_all_nbrs_run(idl);
					if (nbr_change) {
						ovsdb_idl_txn_commit_block(lldp_clear_all_nbr_txn);
					} else {
						ovsdb_idl_txn_destroy(lldp_clear_all_nbr_txn);
					}
				}
			}
			if (g_lldp_cfg->g_protocols[i].enabled) {
				/* Lets set is_any_protocol enabled to true */
				is_any_protocol_enabled = true;
			}
		}

		/*
		 * For now support only lldp but allow support for CDP
		 * and EDP in the future.
		 */
		if (is_any_protocol_enabled) {
			g_lldp_cfg->g_config.c_is_any_protocol_enabled = 1;
		} else {
			g_lldp_cfg->g_config.c_is_any_protocol_enabled = 0;
		}
	}

	if (update_now) {
		/* Update the information to local Chassis */
		levent_update_now(g_lldp_cfg);
	}
	return;
}                               /* lldpd_apply_global_changes */

/*
 * Ovs db poll to libevent & vice versa management functions
 */
u_int64_t
ovs_libevent_get_counter(void)
{
	VLOG_INFO("ovs_libevent_get_counter");
	return libevent_cnt;
}

void *
ovs_libevent_get_arg(void)
{
	return libevent_cb_arg;
}

void
ovs_libevent_schedule_nbr(void *arg)
{
	struct lldpd *cfg = arg;

	nbr_event = event_new(cfg->g_base, -1, 0, ovs_libevent_cb, cfg);
	event_active(nbr_event, EV_TIMEOUT, 1);

	return;
}

static void
ovs_clear_libevents()
{
	struct poll_loop *loop = poll_loop();
	struct poll_node *node;

	/* Clear and free all the fd events. */
	HMAP_FOR_EACH(node, hmap_node, &loop->poll_nodes) {
		if (node->event) {
			event_del((struct event *) node->event);
			event_free((struct event *) node->event);
			node->event = NULL;
		}
	}

	/* Clear and free timeout event */
	if (timeout_event) {
		event_del(timeout_event);
		event_free(timeout_event);
		timeout_event = NULL;
	}

	free_poll_nodes(loop);
	loop->timeout_when = LLONG_MAX;
	loop->timeout_where = NULL;
}                               /* ovs_clear_libevents */

static void
ovs_libevent_cb(evutil_socket_t fd, short what, void *arg)
{
	struct lldpd *cfg = arg;
	int retval = 0;
	struct timeval tv;

	libevent_cnt++;
	libevent_cb_arg = arg;

	ovs_clear_libevents();

	lldpd_run(cfg);
	lldpd_wait();

	retval = ovspoll_to_libevent(cfg);
	if (retval == -1)
		/*
		 * Nothing was scheduled due to connection error.
		 * Lets schedule cb after 1 sec
		 */
	{
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		timeout_event = event_new(cfg->g_base, -1, 0, ovs_libevent_cb, cfg);
		event_add(timeout_event, &tv);
	}
	return;
}                               /* ovs_libevent_cb */

static int
ovspoll_to_libevent(struct lldpd *cfg)
{
	struct poll_loop *loop = poll_loop();
	long long int timeout;
	struct poll_node *node;
	struct timeval tv;
	int retval = -1;
	int events_scheduled = 0;

	/* Populate with all the fds events. */
	HMAP_FOR_EACH(node, hmap_node, &loop->poll_nodes) {
		levent_make_socket_nonblocking(node->pollfd.fd);
		node->event = event_new(cfg->g_base, node->pollfd.fd,
					EV_READ, ovs_libevent_cb, cfg);
		retval = event_add((struct event *) node->event, NULL);
		if (!retval)
			events_scheduled++;
	}

	/* Populate the timeout event */
	timeout = loop->timeout_when - time_msec();
	if (timeout > 0 && loop->timeout_when > 0
	    && loop->timeout_when < LLONG_MAX) {
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		if (timeout < 1000)
			tv.tv_usec = (timeout) * 1000;
		else
			tv.tv_sec = timeout / 1000;
		timeout_event = event_new(cfg->g_base, -1, 0, ovs_libevent_cb, cfg);
		retval = event_add(timeout_event, &tv);
		if (!retval)
			events_scheduled++;
	}

	if (events_scheduled)
		return 0;

	/* Nothing was scheduled, return -1 */
	return -1;
}                               /* ovspoll_to_libevent */

void
init_ovspoll_to_libevent(struct lldpd *cfg)
{
	ovs_clear_libevents();
	lldpd_run(cfg);
	lldpd_wait();
	ovspoll_to_libevent(cfg);
	return;
}                               /* init_ovspoll_to_libevent */

/*
 * Global & per interface statistics/counter management functions
 */

/* Timer working variables */
static u_int64_t lldpd_stats_last_check_time = 0;
static u_int64_t lldpd_stats_check_interval =
	LLDP_CHECK_STATS_FREQUENCY_DFLT_MSEC;

/*
 * Global versions of interface counters (summed up)
 */
static u_int64_t total_h_tx_cnt = 0;
static u_int64_t total_h_rx_cnt = 0;
static u_int64_t total_h_rx_discarded_cnt = 0;
static u_int64_t total_h_rx_unrecognized_cnt = 0;
static u_int64_t total_h_ageout_cnt = 0;
static u_int64_t total_h_insert_cnt = 0;
static u_int64_t total_h_delete_cnt = 0;
static u_int64_t total_h_drop_cnt = 0;

static char *lldp_interface_statistics_keys[] = {
	INTERFACE_STATISTICS_LLDP_TX_COUNT,
	INTERFACE_STATISTICS_LLDP_RX_COUNT,
	INTERFACE_STATISTICS_LLDP_RX_DISCARDED_COUNT,
	INTERFACE_STATISTICS_LLDP_RX_UNRECOGNIZED_COUNT,
	INTERFACE_STATISTICS_LLDP_AGEOUT_COUNT,
	INTERFACE_STATISTICS_LLDP_INSERT_COUNT,
	INTERFACE_STATISTICS_LLDP_DELETE_COUNT,
	INTERFACE_STATISTICS_LLDP_DROP_COUNT
};

#define LLDPD_TOTAL_STATS_PER_INTERFACE					\
	(sizeof(lldp_interface_statistics_keys) / sizeof(char*))

/*
 * Read back a counter from interface & update the count on interface
 */
#define SYNC_COUNTER_FROM_DB(KEY, HW_COUNTER)				\
	{								\
		int64_t db_value = 0;					\
		(void) ovsdb_datum_get_int64_value_given_string_key(	\
			(struct ovsdb_datum *)datum, KEY, &db_value);	\
		dual_itf->hw->HW_COUNTER += db_value;			\
		VLOG_DBG("%s counter %s restored to %ld",		\
			 dual_itf->name, KEY, db_value);		\
	}

/*
 * Read back all counters from db in case process just restarted after a crash.
 * Note that it is NOT an error if counters cannot be read
 * back from the database.  It simply means that the database also just
 * re-started from scratch.
 */
static void
sync_lldp_counters_from_db(struct interface_data *dual_itf)
{
	const struct ovsdb_datum *datum;

	VLOG_DBG("entered sync_lldp_counters_from_db for %s", dual_itf->name);

	datum = ovsrec_interface_get_lldp_statistics(dual_itf->ifrow,
						     OVSDB_TYPE_STRING,
						     OVSDB_TYPE_INTEGER);

	/* Can happen if ovsdb also re-started from scratch */
	if (NULL == datum) {
		VLOG_DBG("ovsrec_interface_get_lldp_statistics returned NULL for %s",
			 dual_itf->name);
	} else {
		SYNC_COUNTER_FROM_DB(INTERFACE_STATISTICS_LLDP_TX_COUNT, h_tx_cnt);
		SYNC_COUNTER_FROM_DB(INTERFACE_STATISTICS_LLDP_RX_COUNT, h_rx_cnt);
		SYNC_COUNTER_FROM_DB(INTERFACE_STATISTICS_LLDP_RX_DISCARDED_COUNT,
				     h_rx_discarded_cnt);
		SYNC_COUNTER_FROM_DB(INTERFACE_STATISTICS_LLDP_RX_UNRECOGNIZED_COUNT,
				     h_rx_unrecognized_cnt);
		SYNC_COUNTER_FROM_DB(INTERFACE_STATISTICS_LLDP_AGEOUT_COUNT,
				     h_ageout_cnt);
		SYNC_COUNTER_FROM_DB(INTERFACE_STATISTICS_LLDP_INSERT_COUNT,
				     h_insert_cnt);
		SYNC_COUNTER_FROM_DB(INTERFACE_STATISTICS_LLDP_DELETE_COUNT,
				     h_delete_cnt);
		SYNC_COUNTER_FROM_DB(INTERFACE_STATISTICS_LLDP_DROP_COUNT, h_drop_cnt);
	}
}

/*
 * Report ONE interface's stats to the db
 */
static void
lldp_process_one_interface_counters(struct interface_data *dual_itf)
{
	int total = 0;
	int64_t values[LLDPD_TOTAL_STATS_PER_INTERFACE];
	struct lldpd_hardware *hardware = dual_itf->hw;
	struct ovsrec_interface *ifrow =
		(struct ovsrec_interface *) dual_itf->ifrow;

	/* If either pointer not found, cannot proceed */
	if (!hardware || !ifrow) {
		VLOG_DBG("could not check stats for %s (hardware %s, dbrow %s)",
			 dual_itf->name,
			 hardware ? "valid" : "NULL", ifrow ? "valid" : "NULL");
		return;
	}

	/* If started new, sync back the counters from the db */
	if (!dual_itf->synced_from_db) {
		sync_lldp_counters_from_db(dual_itf);
		dual_itf->synced_from_db = 1;
	}

/*---------------------------------------------------------------------------*/

#define PROCESS_INTERFACE_COUNTER(COUNTER)		\
	do {						\
		values[total] = hardware->COUNTER;	\
		total_ ## COUNTER += hardware->COUNTER; \
			total++;			\
	} while (0)

/*---------------------------------------------------------------------------*/

	/*
	 * Copy counters from LLDP internal interface to values[].
	 * Please note the counter order must match key order in
	 * lldp_interface_statistics_keys.
	 */
	PROCESS_INTERFACE_COUNTER(h_tx_cnt);
	PROCESS_INTERFACE_COUNTER(h_rx_cnt);
	PROCESS_INTERFACE_COUNTER(h_rx_discarded_cnt);
	PROCESS_INTERFACE_COUNTER(h_rx_unrecognized_cnt);
	PROCESS_INTERFACE_COUNTER(h_ageout_cnt);
	PROCESS_INTERFACE_COUNTER(h_insert_cnt);
	PROCESS_INTERFACE_COUNTER(h_delete_cnt);
	PROCESS_INTERFACE_COUNTER(h_drop_cnt);

	ovsrec_interface_set_lldp_statistics(ifrow,
					     lldp_interface_statistics_keys,
					     values, total);
}

static void
lldp_process_all_interfaces_counters(struct lldpd *cfg)
{
	struct shash_node *sh_node;
	struct interface_data *dual_itf;

	/* Start counting these in case anything changed */
	total_h_tx_cnt = 0;
	total_h_rx_cnt = 0;
	total_h_rx_discarded_cnt = 0;
	total_h_rx_unrecognized_cnt = 0;
	total_h_ageout_cnt = 0;
	total_h_insert_cnt = 0;
	total_h_delete_cnt = 0;
	total_h_drop_cnt = 0;

	/* For each interface, compare & update db */
	SHASH_FOR_EACH(sh_node, &all_interfaces) {
		dual_itf = sh_node->data;
		lldp_process_one_interface_counters(dual_itf);
	}
}

/*
 * Report changes on the GLOBAL lldp stats.
 * This is called ASSUMING that the values
 * have actually changed.
 */
static void
lldp_process_global_counters(struct lldpd *cfg)
{
	struct ovsrec_system *row;
	struct smap smap;

	smap_init(&smap);
	smap_add_format(&smap, OVSDB_STATISTICS_LLDP_TABLE_INSERTS,
			"%" PRIu64, total_h_insert_cnt);
	smap_add_format(&smap, OVSDB_STATISTICS_LLDP_TABLE_DELETES,
			"%" PRIu64, total_h_delete_cnt);
	smap_add_format(&smap, OVSDB_STATISTICS_LLDP_TABLE_DROPS,
			"%" PRIu64, total_h_drop_cnt);
	smap_add_format(&smap, OVSDB_STATISTICS_LLDP_TABLE_AGEOUTS,
			"%" PRIu64, total_h_ageout_cnt);

	VLOG_DBG("updating lldp global stats: %s=%" PRIu64 " %s=%" PRIu64
		 "%s=%" PRIu64 " %s=%" PRIu64,
		 OVSDB_STATISTICS_LLDP_TABLE_INSERTS, total_h_insert_cnt,
		 OVSDB_STATISTICS_LLDP_TABLE_DELETES, total_h_delete_cnt,
		 OVSDB_STATISTICS_LLDP_TABLE_DROPS, total_h_drop_cnt,
		 OVSDB_STATISTICS_LLDP_TABLE_AGEOUTS, total_h_ageout_cnt);

	row = (struct ovsrec_open_vswitch *) ovsrec_open_vswitch_first(idl);
	ovsrec_open_vswitch_set_lldp_statistics(row, &smap);
	smap_destroy(&smap);
}

static void
lldpd_stats_analyze(struct lldpd *cfg)
{

	static struct ovsdb_idl_txn *lldp_stats_txn = NULL;

	enum ovsdb_idl_txn_status status;

	/* Don't continue if database is not synced */
	if (!ovsdb_idl_has_ever_connected(idl)) {
		return;
	}

        /* Create transaction only if none outstanding */
	if (NULL == lldp_stats_txn) {
		lldp_stats_txn = ovsdb_idl_txn_create(idl);
		lldp_process_all_interfaces_counters(cfg);
		lldp_process_global_counters(cfg);
	}
	status = ovsdb_idl_txn_commit(lldp_stats_txn);
	if (status != TXN_INCOMPLETE) {
		ovsdb_idl_txn_destroy(lldp_stats_txn);
		lldp_stats_txn = NULL;
	}
}

/*
 * This is the lldp main stat reporting scheduling function.
 * It makes sure that it runs every "lldpd_stats_check_interval"
 * seconds and if somehow it gets called before that time has
 * elapsed, reschedules itself to be called at precisely that
 * interval.  When the exact time is hit, it calls the main worker
 * function "lldpd_stats_analyze".
 */
static void
lldpd_stats_run(struct lldpd *cfg)
{
	u_int64_t time_now = time_msec();
	int64_t time_elapsed_since_last_invocation =
		time_now - lldpd_stats_last_check_time;
	int64_t time_left_to_next_invocation =
		lldpd_stats_check_interval - time_elapsed_since_last_invocation;

	if (time_left_to_next_invocation < 0) {
		time_left_to_next_invocation = 0;
	}

	VLOG_DBG("lldpd_stats_run entered at time %" PRIu64, time_now);

	if (time_left_to_next_invocation > 0) {
		poll_timer_wait(time_left_to_next_invocation);
		VLOG_DBG("not our time yet, still have %" PRIu64 " msecs to wait",
			 time_left_to_next_invocation);
		return;
	}

	VLOG_DBG("checking lldpd stats NOW at %" PRIu64 " msecs!", time_now);
	lldpd_stats_last_check_time = time_now;
	lldpd_stats_analyze(cfg);
	poll_timer_wait(lldpd_stats_check_interval);
}

/*
 * Traverse interface table and clear neighbor data
 */
static bool
lldpd_ovsdb_clear_all_nbrs_run(struct ovsdb_idl *idl)
{
	const struct ovsrec_interface *ifrow;
	bool nbr_change = false;

	/* Scan all hardware interfaces in lldpd and reset nbr data */
	OVSREC_INTERFACE_FOR_EACH(ifrow, idl) {
		ovsrec_interface_set_lldp_neighbor_info(ifrow, NULL);
		nbr_change = true;
	}
	return (nbr_change);
}
/*
 * Clear LLDP Counters.
 */
static bool
lldpd_clear_counters(struct ovsdb_idl *idl, struct lldpd *cfg)
{

	struct lldpd_hardware *hardware, *hardware_next;
	const struct ovsrec_interface *ifrow;
	const struct ovsrec_system *sys_row = NULL;
	bool counter_change = false;
	int clear_counter_requested = 0;
	int last_clear_counter = 0;
	struct smap smap_status;
	char clear_counter_str[10] = {0};

	sys_row = ovsrec_system_first(idl);
	if (!sys_row) {
		return false;
	}

	/* Clear LLDP Counters.
	* if lldp_num_clear_counters_requested>lldp_last_clear_counters_performed
	* Reset all the counters and clear neighbor table.
	*/
	clear_counter_requested = smap_get_int(&sys_row->status,
			"lldp_num_clear_counters_requested", 0);
	last_clear_counter = smap_get_int(&sys_row->status,
			"lldp_last_clear_counters_performed", 0);
	sprintf(clear_counter_str, "%d", clear_counter_requested);
	if (clear_counter_requested>last_clear_counter){
		OVSREC_INTERFACE_FOR_EACH(ifrow, idl) {
			ovsrec_interface_set_lldp_neighbor_info(ifrow, NULL);
		}
		for (hardware = TAILQ_FIRST(&cfg->g_hardware); hardware != NULL;
			hardware = hardware_next) {
			hardware_next = TAILQ_NEXT(hardware, h_entries);
			hardware->h_tx_cnt = 0;
			hardware->h_rx_cnt = 0;
			hardware->h_rx_discarded_cnt = 0;
			hardware->h_rx_unrecognized_cnt = 0;
			hardware->h_ageout_cnt = 0;
			hardware->h_drop_cnt = 0;
			lldpd_remote_cleanup(hardware, NULL, 1);
			hardware->h_delete_cnt = 0;
		}
		/*
		* Update lldp_last_clear_counters_performed to be equal to last
		* known lldp_num_clear_counters_requested.
		*/
		sys_row = (struct ovsrec_open_vswitch *) ovsrec_open_vswitch_first(idl);
		smap_clone(&smap_status, &sys_row->status);
		smap_replace(&smap_status, "lldp_num_clear_counters_requested",
			clear_counter_str);
		smap_replace(&smap_status, "lldp_last_clear_counters_performed",
			clear_counter_str);
		ovsrec_open_vswitch_set_status(sys_row, &smap_status);
		smap_destroy(&smap_status);
		counter_change = true;
	}
	return counter_change;
}
/*
 * Clear LLDP neighbor table.
 */
static bool
lldpd_clear_nbr_table(struct ovsdb_idl *idl, struct lldpd *cfg)
{
	struct lldpd_hardware *hardware, *hardware_next;
	const struct ovsrec_interface *ifrow;
	const struct ovsrec_system *sys_row = NULL;
	bool nbr_change = false;
	int clear_table_requested = 0;
	int clear_table_performed = 0;
	struct smap smap_status;
	char clear_table_str[10] = {0};

	sys_row = ovsrec_system_first(idl);
        if (!sys_row) {
		return false;
	}
	clear_table_requested = smap_get_int(&sys_row->status,
			"lldp_num_clear_table_requested", 0);
	clear_table_performed = smap_get_int(&sys_row->status,
	"lldp_last_clear_table_performed", 0);
	/*
	* Clear LLDP neighbor table.
	* if lldp_num_clear_table_requested is greater
	* than lldp_last_clear_table_performed then clear all the table.
	* Scan all nbr tables in OVSDB and look for entries;
	* Delete any such nbr table from database.
	*/
	sprintf(clear_table_str, "%d", clear_table_requested);
	if (clear_table_requested > clear_table_performed)
	{
		OVSREC_INTERFACE_FOR_EACH(ifrow, idl) {
			ovsrec_interface_set_lldp_neighbor_info(ifrow, NULL);
		}
		for (hardware = TAILQ_FIRST(&cfg->g_hardware); hardware != NULL;
			hardware = hardware_next) {
			hardware_next = TAILQ_NEXT(hardware, h_entries);
			lldpd_remote_cleanup(hardware, NULL, 1);
		}
		/*
		*Update lldp_last_clear_table_performed to be equal to last known
		* lldp_num_clear_table_requested.
		*/
		smap_init(&smap_status);
		smap_add(&smap_status, "lldp_last_clear_table_performed",
			clear_table_str);
		smap_add(&smap_status, "lldp_num_clear_table_requested",
			clear_table_str);
		sys_row = (struct ovsrec_open_vswitch *) ovsrec_open_vswitch_first(idl);
		ovsrec_open_vswitch_set_status(sys_row, &smap_status);
		smap_destroy(&smap_status);
		nbr_change = true;
	}

	return nbr_change;
}

/*
 * The fuction scans all LLDP ports and looks for neighbor
 * changes triggered by LLDPD.
 * The following changes are supported: ADD, MOD, UPD and DEL.
 * Update means a keep alive refresh and only requires updating a timestamp.
 * ADD/MOD requires writing/rewriting the entire neighbor table to OVSDB
 * DEL clears up neighbor table info.
 * The function also looks for stale neighbors by checking update time
 * against current time. In this case the neighbor entry is deleted
 * from OVSDB.
 */
static bool
lldpd_ovsdb_nbrs_run(struct ovsdb_idl *idl, struct lldpd *cfg)
{
	struct lldpd_hardware *hardware;
	const struct ovsrec_interface *ifrow;
	struct lldpd_port *port;
	struct smap smap_nbr;
	const char *last_update_str;
	const char *port_ttl_str;
	time_t last_update_db;
	int port_ttl;
	bool nbr_change = false;
	struct shash_node *if_node;
	struct interface_data *itf;

	/*
	* Check for clear lldp neighbor info request.
	*/
	if(lldpd_clear_nbr_table(idl,cfg)){
		nbr_change = true;
	}
	/*
	* Check for clear lldp counters request.
	*/
	if(lldpd_clear_counters(idl,cfg)){
		nbr_change = true;
	}
	/*
	 * Scan all hardware interfaces in lldpd and look for updates.
	 * For any interface that got changed, find a corresponding
	 * OVSDB nbr and update that nbr according to lldps change opcode.
	 */
	SHASH_FOR_EACH(if_node, &all_interfaces) {
		itf = if_node->data;
		if (itf && itf->hw && itf->ifrow) {
			hardware = itf->hw;
			ifrow = itf->ifrow;
		} else {
			continue;
		}

		if (hardware->h_rport_change_opcode == LLDPD_AF_NBR_NOOP) {
			continue;
		} else {
			if (strcmp(ifrow->name, hardware->h_ifname) == 0) {
				/* If no neighbor info in DB, write everything */
				if (hardware->h_rport_change_opcode == LLDPD_AF_NBR_UPD) {
					port_ttl_str =
						smap_get(&ifrow->lldp_neighbor_info, "chassis_ttl");
					if (port_ttl_str == NULL) {
						hardware->h_rport_change_opcode = LLDPD_AF_NBR_ADD;
					}
				}
				switch (hardware->h_rport_change_opcode) {

				case LLDPD_AF_NBR_ADD:
				case LLDPD_AF_NBR_MOD:
					VLOG_INFO("%s i/f %s ADD/MOD", __FUNCTION__, ifrow->name);
                                        log_event("LLDP_NEIGHBOUR_ADD", EV_KV("interface", "%s",
                                            ifrow->name));
					TAILQ_FOREACH(port, &hardware->h_rports, p_entries) {
						lldp_nbr_update(&smap_nbr, port);
						ovsrec_interface_set_lldp_neighbor_info(ifrow,
											&smap_nbr);
						smap_destroy(&smap_nbr);
						nbr_change = true;
						break;
					}
					break;

				case LLDPD_AF_NBR_UPD:
					VLOG_DBG("%s i/f %s UPD", __FUNCTION__, ifrow->name);
                                        log_event("LLDP_NEIGHBOUR_UPDATE", EV_KV("interface", "%s",
                                            ifrow->name));
					TAILQ_FOREACH(port, &hardware->h_rports, p_entries) {
						char last_update_s[10];

						sprintf(last_update_s, "%llx",
							(long long) port->p_lastupdate);
						VLOG_DBG("Updating nbr time to %s", last_update_s);
						smap_clone(&smap_nbr, &ifrow->lldp_neighbor_info);
						smap_replace(&smap_nbr, "port_lastupdate",
							     last_update_s);
						ovsrec_interface_set_lldp_neighbor_info(ifrow,
											&smap_nbr);
						smap_destroy(&smap_nbr);
						nbr_change = true;
						break;
					}
					break;

				case LLDPD_AF_NBR_DEL:
					VLOG_INFO("%s i/f %s DEL", __FUNCTION__, ifrow->name);
                                        log_event("LLDP_NEIGHBOUR_DELETE", EV_KV("interface", "%s",
                                            ifrow->name));
					ovsrec_interface_set_lldp_neighbor_info(ifrow, NULL);
					nbr_change = true;
					break;

				case LLDPD_AF_NBR_NOOP:
					VLOG_DBG("%s i/f %s NOOP", __FUNCTION__, ifrow->name);
					break;

				default:
					VLOG_DBG("%s i/f %s dafault", __FUNCTION__, ifrow->name);
					/* Nothing to do */
					break;
				}               /* switch */
				hardware->h_rport_change_opcode = LLDPD_AF_NBR_NOOP;
			}                   /* strcmp */
		}                       /* h_rport_change_opcode != NULL */
	}                           /* interface loop */

        /*
	 * Scan all nbr tables in OVSDB and look for aged out entries;
	 * Delete any such nbr table from database.
	 * This covers any corner case, like port disconnect and restart,
         * in which lldpd fails to report aged out nbr entries in a timely
         * manner.
	 */

	OVSREC_INTERFACE_FOR_EACH(ifrow, idl) {
		last_update_str =
			smap_get(&ifrow->lldp_neighbor_info, "port_lastupdate");
		port_ttl_str = smap_get(&ifrow->lldp_neighbor_info, "chassis_ttl");
		if ((last_update_str != NULL) && (port_ttl_str != NULL)) {
			last_update_db = strtoll(last_update_str, 0, 16);
			port_ttl = atoi(port_ttl_str);
			VLOG_DBG
				("%s timestamps: cur=%0llx last_update=%llx port_ttl=%d (sec)",
				 __FUNCTION__, (long long) time(NULL),
				 (long long) last_update_db, port_ttl);
			if (time(NULL) - last_update_db > port_ttl + 2) {
				VLOG_INFO("%s aging out interfcae %s", __FUNCTION__,
					  ifrow->name);
				ovsrec_interface_set_lldp_neighbor_info(ifrow, NULL);
				nbr_change = true;
			}
		}
	}
	return nbr_change;
}

/*
 * This function sets the default value at first time,
 * by default lldp is enabled.
 */
static bool
lldpd_ovsdb_initialise(struct ovsdb_idl *idl)
{
	const struct ovsrec_system *sys_row = NULL;
	enum ovsdb_idl_txn_status txn_status = TXN_ERROR;
	struct ovsdb_idl_txn *status_txn = NULL;
	struct smap smap_other_config;
	bool ret = false;

	smap_init(&smap_other_config);
	sys_row = ovsrec_system_first(idl);

	if (sys_row != NULL) {
		const char *lldp_status = smap_get(&sys_row->other_config,
							SYSTEM_OTHER_CONFIG_MAP_LLDP_ENABLE);
	        if (lldp_status  == NULL)
	        {
			status_txn = ovsdb_idl_txn_create(idl);
	                smap_clone(&smap_other_config, &sys_row->other_config);
	                smap_replace(&smap_other_config, SYSTEM_OTHER_CONFIG_MAP_LLDP_ENABLE,"true");

	                ovsrec_system_set_other_config(sys_row, &smap_other_config);
	                txn_status = ovsdb_idl_txn_commit_block(status_txn);
			ovsdb_idl_txn_destroy(status_txn);

	                if(txn_status == TXN_SUCCESS || txn_status == TXN_UNCHANGED)
			{
				ret = true;
				VLOG_DBG("LLDP enable default setting success");
	                }
	        }
	}

	smap_destroy(&smap_other_config);
	return ret;
}




/*
 * This function is called to sync up LLDP internal neighbor info
 * with OVSDB by copying LLDP table info to OVSDB for each LLDP interface
 * It's called after a transaction failure (e.g. neighbor update failure)
 * since, at that point, OVSDB neighbor info could be out of sync.
 */
static void
lldpd_ovsdb_nbrs_change_all(struct ovsdb_idl *idl, struct lldpd *cfg)
{
	struct lldpd_hardware *hardware;
	const struct ovsrec_interface *ifrow;
	struct lldpd_port *port;
	struct smap smap_nbr;
	bool found = false;

	/*
	 * Scan all hardware interfaces in lldpd.
	 * Copy nbr info from lldpd to OVSDB whenever lldpd has a port entry.
	 * Otherwise, delete nbr info from OVSDB.
	 */
	TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {

		found = false;
		OVSREC_INTERFACE_FOR_EACH(ifrow, idl) {
			if (strcmp(ifrow->name, hardware->h_ifname) == 0) {
				TAILQ_FOREACH(port, &hardware->h_rports, p_entries) {
					lldp_nbr_update(&smap_nbr, port);
					ovsrec_interface_set_lldp_neighbor_info(ifrow, &smap_nbr);
					smap_destroy(&smap_nbr);
					found = true;
					break;
				}
				if (!found) {
					ovsrec_interface_set_lldp_neighbor_info(ifrow, NULL);
				}

				hardware->h_rport_change_opcode = LLDPD_AF_NBR_NOOP;
			}                   /* strcmp */
		}                       /* ovsrec loop */
	}                           /* hardware loop */

	return;
}


static void
lldpd_reconfigure(struct ovsdb_idl *idl, struct lldpd *g_lldp_cfg)
{
	unsigned int new_idl_seqno = ovsdb_idl_get_seqno(idl);
	bool send_now = false;

	COVERAGE_INC(lldpd_ovsdb_if);

	if (new_idl_seqno == idl_seqno) {
		return;
	}

	lldpd_apply_interface_changes(idl, g_lldp_cfg, &send_now);
	lldpd_apply_bridge_changes(idl, g_lldp_cfg, &send_now);
	lldpd_apply_vrf_changes(idl, g_lldp_cfg, &send_now);
	lldpd_apply_port_changes(idl, g_lldp_cfg, &send_now);
	lldpd_apply_global_changes(idl, g_lldp_cfg, &send_now);

	if (send_now) {
		/* An Asnychronous event as Information has changed */
		levent_send_now(g_lldp_cfg);
	}

	idl_seqno = new_idl_seqno;
}                               /* lldpd_reconfigure */


/*
 * Function       : lldpd_dump
 * Responsibility : populates buffer for unixctl reply
 * Parameters     : buffer , buffer length
 * Returns        : void
 */

static void
lldpd_dump(char* buf, int buflen)
{
	struct shash_node *sh_node;
	int first_row_done = 0;

	/*
	 * Loop through all the current interfaces and figure out how many
	 * have config changes that need action.
	 */
	SHASH_FOR_EACH(sh_node, &all_interfaces) {

		struct interface_data *itf = sh_node->data;

		if (itf->hw && first_row_done == 0) {
			if (itf->hw->h_cfg->g_protocols[0].enabled)
				strcpy(buf, "\nLLDP : ENABLED\n\n");
			else
				strcpy(buf, "\nLLDP : DISABLED\n\n");
			strncat(buf, "    intf name\t|   OVSDB interface\t|"
				"   LLDPD Interface\t|    LLDP Status\t|  Link State\n",
				REM_BUF_LEN);
			strncat(buf,
				"==============================================="
				"===============================================\n",
				REM_BUF_LEN);
			first_row_done++;
		}

                /* Display information for all interfaces except bridge_normal */
		if (strncmp(itf->name, DEFAULT_INTERFACE, strlen(DEFAULT_INTERFACE)) != 0) {
			strncat(buf, itf->name, REM_BUF_LEN);
			if (itf->ifrow)
				strncat(buf, "\t\t|    Yes", REM_BUF_LEN);
			else
				strncat(buf, "\t\t|    No\t", REM_BUF_LEN);
			if (itf->hw) {
				strncat(buf, "\t\t|    Yes", REM_BUF_LEN);
				if (itf->hw->h_enable_dir == HARDWARE_ENABLE_DIR_OFF)
					strncat(buf, "\t\t|    off", REM_BUF_LEN);
				if (itf->hw->h_enable_dir == HARDWARE_ENABLE_DIR_TX)
					strncat(buf, "\t\t|    tx", REM_BUF_LEN);
				if (itf->hw->h_enable_dir == HARDWARE_ENABLE_DIR_RX)
					strncat(buf, "\t\t|    rx", REM_BUF_LEN);
				if (itf->hw->h_enable_dir == HARDWARE_ENABLE_DIR_RXTX)
					strncat(buf, "\t\t|    rxtx", REM_BUF_LEN);
			} else
				strncat(buf, "\t\t|    No\t\t\t|", REM_BUF_LEN);
			if (itf->hw) {
				if (itf->hw->h_link_state == INTERFACE_LINK_STATE_UP)
					strncat(buf, "\t\t|    up", REM_BUF_LEN);
				else
					strncat(buf, "\t\t|    down", REM_BUF_LEN);
			}

			strncat(buf, "\n", REM_BUF_LEN);
		}
	}
}

static void
lldpd_unixctl_dump(struct unixctl_conn *conn, int argc OVS_UNUSED,
		const char *argv[]OVS_UNUSED, void *aux OVS_UNUSED)
{
	char err_str[MAX_ERR_STR_LEN];
	char *buf = xcalloc(1, BUF_LEN);
	if (buf){
		lldpd_dump(buf,BUF_LEN);
		unixctl_command_reply(conn, buf);
		free(buf);
	} else {
		snprintf(err_str,sizeof(err_str),
				"lldp daemon failed to allocate %d bytes", BUF_LEN );
		unixctl_command_reply(conn, err_str );
	}
	return;
}                               /* lldpd_unixctl_dump */


static void
ops_lldpd_exit(struct unixctl_conn *conn, int argc OVS_UNUSED,
	       const char *argv[]OVS_UNUSED, void *exiting_)
{
	bool *exiting = exiting_;

	*exiting = true;
	unixctl_command_reply(conn, NULL);
}                               /* ops_lldpd_exit */

static inline void
lldpd_chk_for_system_configured(void)
{
	const struct ovsrec_system *ovs_vsw = NULL;

	if (system_configured) {
		/* Nothing to do if lldpd is already configured. */
		return;
	}

	ovs_vsw = ovsrec_system_first(idl);

	if (ovs_vsw && (ovs_vsw->cur_cfg > (int64_t) 0)) {
		system_configured = true;
		VLOG_INFO("System is now configured (cur_cfg=%d).",
			  (int) ovs_vsw->cur_cfg);
	}

}                               /* lldpd_chk_for_system_configured */

/*****************************************************************************
 * poll/run/timer functions
 *****************************************************************************/

static bool confirm_txn_try_again = false;

static void
lldpd_run(struct lldpd *cfg)
{
	bool nbr_change;
	static struct ovsdb_idl_txn *confirm_txn = NULL;
	static bool lldpd_initialised;

	ovsdb_idl_run(idl);
	unixctl_server_run(appctl);

	if (ovsdb_idl_is_lock_contended(idl)) {
		static struct vlog_rate_limit rl = VLOG_RATE_LIMIT_INIT(1, 1);

		VLOG_ERR_RL(&rl, "another lldpd process is running, "
			    "disabling this process until it goes away");

		return;
	} else if (!ovsdb_idl_has_lock(idl)) {
		return;
	}

	lldpd_chk_for_system_configured();

	if (system_configured) {
		lldpd_initialised = lldpd_initialised ? lldpd_initialised : lldpd_ovsdb_initialise(idl);
		lldpd_reconfigure(idl, cfg);
		lldpd_stats_run(cfg);
		daemonize_complete();
		vlog_enable_async();
		VLOG_INFO_ONCE("%s (OPENSWITCH lldpd) %s", program_name, VERSION);
	}

	/* Create a confirmed database transaction for nbr and config updates */
	if (!confirm_txn) {
		confirm_txn = ovsdb_idl_txn_create(idl);

		if (!confirm_txn_try_again) {
			VLOG_DBG("Invoking lldpd_ovsdb_nbrs_run");
			nbr_change = lldpd_ovsdb_nbrs_run(idl, cfg);
			if (!nbr_change) {
				ovsdb_idl_txn_destroy(confirm_txn);
				confirm_txn = NULL;
			}
		} else {
			VLOG_INFO("Invoking lldpd_ovsdb_nbrs_change_all");
			lldpd_ovsdb_nbrs_change_all(idl, cfg);
			nbr_change = true;
		}
	}

	if (confirm_txn) {
		enum ovsdb_idl_txn_status status;

		status = ovsdb_idl_txn_commit(confirm_txn);
		if (status != TXN_INCOMPLETE) {
			ovsdb_idl_txn_destroy(confirm_txn);
			confirm_txn = NULL;

			/* Sets the 'status_txn_try_again' if the transaction fails. */
			if (status == TXN_SUCCESS) {
				confirm_txn_try_again = false;
			} else if (status == TXN_TRY_AGAIN) {
				confirm_txn_try_again = true;
			} else {
				VLOG_INFO("%s OVSDB write failure status= %d", __FUNCTION__,
					  status);
				confirm_txn_try_again = false;
			}
		}
	}
}                               /* lldpd_run */

static void
lldpd_wait(void)
{
	ovsdb_idl_wait(idl);
	unixctl_server_wait(appctl);
}                               /* lldpd_run */

/*
 * Create a connection to the OVSDB at db_path and create a dB cache
 * for this daemon.
 */
static void
ovsdb_init(const char *db_path)
{
	/* Initialize IDL through a new connection to the dB */
	idl = ovsdb_idl_create(db_path, &ovsrec_idl_class, false, true);
	idl_seqno = ovsdb_idl_get_seqno(idl);
	ovsdb_idl_set_lock(idl, "ops_lldpd");

	/* Choose some OVSDB tables and columns to cache */

	ovsdb_idl_add_table(idl, &ovsrec_table_system);
	ovsdb_idl_add_column(idl, &ovsrec_system_col_cur_cfg);
	ovsdb_idl_add_column(idl, &ovsrec_system_col_other_config);
	ovsdb_idl_add_column(idl, &ovsrec_system_col_lldp_statistics);
	ovsdb_idl_omit_alert(idl, &ovsrec_system_col_lldp_statistics);
	ovsdb_idl_add_column(idl, &ovsrec_system_col_mgmt_intf_status);
	ovsdb_idl_add_column(idl, &ovsrec_system_col_status);
	ovsdb_idl_add_column(idl, &ovsrec_system_col_hostname);
	ovsdb_idl_omit_alert(idl, &ovsrec_system_col_status);

	ovsdb_idl_add_table(idl, &ovsrec_table_interface);
	ovsdb_idl_add_column(idl, &ovsrec_interface_col_name);
	ovsdb_idl_add_column(idl, &ovsrec_interface_col_lldp_statistics);
	ovsdb_idl_omit_alert(idl, &ovsrec_interface_col_lldp_statistics);
	ovsdb_idl_add_column(idl, &ovsrec_interface_col_other_config);
	ovsdb_idl_add_column(idl, &ovsrec_interface_col_link_state);

	/* Per interface lldp_neighbor_info */
	ovsdb_idl_add_column(idl, &ovsrec_interface_col_name);
	ovsdb_idl_add_column(idl, &ovsrec_interface_col_lldp_neighbor_info);
	ovsdb_idl_omit_alert(idl, &ovsrec_interface_col_lldp_neighbor_info);

	ovsdb_idl_add_table(idl, &ovsrec_table_bridge);
	ovsdb_idl_add_column(idl, &ovsrec_bridge_col_name);

	ovsdb_idl_add_table(idl, &ovsrec_table_vrf);
	ovsdb_idl_add_column(idl, &ovsrec_vrf_col_name);

	ovsdb_idl_add_table(idl, &ovsrec_table_port);
	ovsdb_idl_add_column(idl, &ovsrec_port_col_name);
	ovsdb_idl_add_column(idl, &ovsrec_port_col_interfaces);
	ovsdb_idl_add_column(idl, &ovsrec_port_col_vlan_mode);
	ovsdb_idl_add_column(idl, &ovsrec_port_col_vlan_tag);
	ovsdb_idl_add_column(idl, &ovsrec_port_col_vlan_trunks);

	ovsdb_idl_add_table(idl, &ovsrec_table_vlan);
	ovsdb_idl_add_column(idl, &ovsrec_vlan_col_name);
	ovsdb_idl_add_column(idl, &ovsrec_vlan_col_id);

    /* Registering snmp trap table */
	init_ovsdb_snmp_notifications(idl);

	/* Register ovs-appctl commands for this daemon */
	unixctl_command_register("lldpd/dump", "", 0, 0, lldpd_unixctl_dump, NULL);
	unixctl_command_register("lldpd/test",
				 "libevent|ovsdb <test case no>",
				 2, 2, lldpd_unixctl_test, NULL);
	INIT_DIAG_DUMP_BASIC(lldpd_diag_dump_basic_cb);

}                               /* ovsdb_init */

static void
ovsdb_exit(void)
{
	ovsdb_idl_destroy(idl);
}                               /* ovsdb_exit */

static void
usage(void)
{
	printf("%s: OPENSWITCH lldpd daemon\n"
	       "usage: %s [OPTIONS] [DATABASE]\n"
	       "where DATABASE is a socket on which ovsdb-server is listening\n"
	       "      (default: \"unix:%s/db.sock\").\n",
	       program_name, program_name, ovs_rundir());
	stream_usage("DATABASE", true, false, true);
	daemon_usage();
	vlog_usage();
	printf("\nOther options:\n"
	       "  --unixctl=SOCKET        override default control socket name\n"
	       "  -h, --help              display this help message\n"
	       "  -V, --version           display version information\n");
	exit(EXIT_SUCCESS);
}                               /* usage */

static char *
lldp_ovsdb_parse_options(int argc, char *argv[], char **unixctl_pathp)
{
	enum {
		OPT_UNIXCTL = UCHAR_MAX + 1,
		VLOG_OPTION_ENUMS,
		DAEMON_OPTION_ENUMS,
		OVSDB_OPTIONS_END,
	};

	static const struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"unixctl", required_argument, NULL, OPT_UNIXCTL},
		DAEMON_LONG_OPTIONS,
		VLOG_LONG_OPTIONS,
		{"ovsdb-options-end", optional_argument, NULL, OVSDB_OPTIONS_END},
		{NULL, 0, NULL, 0},
	};
	char *short_options = long_options_to_short_options(long_options);

	for (;;) {
		int c;
		int end_options = 0;

		c = getopt_long(argc, argv, short_options, long_options, NULL);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'h':
			usage();

		case OPT_UNIXCTL:
			*unixctl_pathp = optarg;
			break;

			VLOG_OPTION_HANDLERS DAEMON_OPTION_HANDLERS case OVSDB_OPTIONS_END:
			end_options = 1;
			break;

		case '?':
			exit(EXIT_FAILURE);

		default:
			abort();
		}
		if (end_options)
			break;
	}
	free(short_options);

	argc -= optind;
	argv += optind;

	return xasprintf("unix:%s/db.sock", ovs_rundir());
}                               /* lldp_ovsdb_parse_options */

void
lldpd_ovsdb_init(int argc, char *argv[])
{
	int retval;
	char *ovsdb_sock;

	set_program_name(argv[0]);
	proctitle_init(argc, argv);
	fatal_ignore_sigpipe();

	/* Setup feature opcode decode table */
	lldp_ovsdb_setup_decode();

	/* Parse commandline args and get the name of the OVSDB socket */
	ovsdb_sock = lldp_ovsdb_parse_options(argc, argv, &appctl_path);

	/* Initialize the metadata for the IDL cache */
	ovsrec_init();
	/*
	 * Fork and return in child process; but don't notify parent of
	 * startup completion yet.
	 */
	daemonize_start();

	/* Create UDS connection for ovs-appctl */
	retval = unixctl_server_create(appctl_path, &appctl);
	if (retval) {
		exit(EXIT_FAILURE);
	}

	/* Register the ovs-appctl "exit" command for this daemon */
	unixctl_command_register("exit", "", 0, 0, ops_lldpd_exit, &exiting);

	/* Create the IDL cache of the dB at ovsdb_sock */
	ovsdb_init(ovsdb_sock);
	free(ovsdb_sock);

	/* Notify parent of startup completion */
	daemonize_complete();

	/* Enable asynch log writes to disk */
	vlog_enable_async();

        retval = event_log_init("LLDP");
        if(retval < 0) {
            VLOG_ERR("Event log initialization failed");
        }

	VLOG_INFO_ONCE("%s (OPENSWITCH LLDPD Daemon) started", program_name);
	return;
}                               /* lldpd_ovsdb_init */

void
lldpd_ovsdb_exit(void)
{
	ovsdb_exit();
}                               /* lldpd_ovsdb_exit */

/*
 * Functions to provide hooks to lldp code to add
 * and delete hardware interfaces to our global
 * interface hashmap
 */
void
del_lldpd_hardware_interface(struct lldpd_hardware *hw)
{
	if (hw) {
		struct shash_node *sh_node = shash_find(&all_interfaces, hw->h_ifname);
		struct interface_data *itf;

		if (!sh_node) {
			VLOG_ERR("Unable to delete an lldp interface %s that has no entry "
				 "in hash", hw->h_ifname);
			return;
		}

		itf = sh_node->data;

		/*
		 * If the ovs rec is also cleaned up
		 * remove the entry else just nullify
		 * ovsdb record handle
		 */
		if (!itf->ifrow) {
			free(itf->name);
			free(sh_node->data);
			shash_delete(&all_interfaces, sh_node);
		} else {
			itf->hw = NULL;
		}
	}
}                               /* del_lldpd_hardware_interface */

void
add_lldpd_hardware_interface(struct lldpd_hardware *hw)
{
	struct interface_data *itf = NULL;
	struct shash_node *sh_node = NULL;
	const char *ifrow_other_config_lldp_enable_dir = NULL;
	struct port_data *port = NULL;
	const struct ovsrec_port *portrow = NULL;
	struct shash_node *port_node;
	int i = 0;

	if (hw) {
		VLOG_DBG("lldpd hardware interface %s being added!\n", hw->h_ifname);
		sh_node = shash_find(&all_interfaces, hw->h_ifname);

		if (!sh_node) {
			/* Allocate structure to save state information for this
			 * interface. */
			itf = xcalloc(1, sizeof *itf);

			if (!shash_add_once(&all_interfaces, hw->h_ifname, itf)) {
				VLOG_WARN("Interface %s specified twice", hw->h_ifname);
				free(itf);
			} else {
				itf->name = xstrdup(hw->h_ifname);
				itf->hw = hw;
				itf->portdata = NULL;
				VLOG_DBG("Created local data for interface %s", hw->h_ifname);
			}
		} else {
			itf = sh_node->data;
			itf->hw = hw;
			itf->portdata = NULL;
            /* Hash entry exists since ovsdb row has been inserted already */
			/* Set interface lldp_enable_dir to hw from ovsrec */
			if (itf->ifrow) {
				ifrow_other_config_lldp_enable_dir =
						     smap_get(&itf->ifrow->other_config,
						     INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR);
			}

			if (ifrow_other_config_lldp_enable_dir) {
				if (strcmp(ifrow_other_config_lldp_enable_dir,
					   INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_OFF) ==
				    0) {
					hw->h_enable_dir = HARDWARE_ENABLE_DIR_OFF;
				} else if (strcmp(ifrow_other_config_lldp_enable_dir,
						  INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_RX)
					   == 0) {
					hw->h_enable_dir = HARDWARE_ENABLE_DIR_RX;
				} else if (strcmp(ifrow_other_config_lldp_enable_dir,
						  INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_TX)
					   == 0) {
					hw->h_enable_dir = HARDWARE_ENABLE_DIR_TX;
				} else if (strcmp(ifrow_other_config_lldp_enable_dir,
						  INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_RXTX)
					   == 0) {
					hw->h_enable_dir = HARDWARE_ENABLE_DIR_RXTX;
				}
			} else {
				hw->h_enable_dir = HARDWARE_ENABLE_DIR_RXTX;
			}

			/* Check for link_state change */
			bool link_state_bool = false;

			if (itf->ifrow && itf->ifrow->link_state &&
			    !strcmp(itf->ifrow->link_state,
				    OVSREC_INTERFACE_LINK_STATE_UP)) {
				link_state_bool = true;
			}
			itf->hw->h_link_state = link_state_bool;

			/* Apply Port configurations, for now VLAN configs */
			/* Find port corresponding to interface */
			OVSREC_PORT_FOR_EACH(portrow, idl) {
				for(i = 0; i < portrow->n_interfaces; i ++) {
					if(portrow->interfaces[i] == itf->ifrow) {
						port_node = shash_find(&all_ports, portrow->name);
						if(port_node) {
						    port = (struct port_data *)port_node->data;
						    break;
						}
					}
				}
				if (port) {
					break;
				}
			}

			if (port) {
				itf->portdata = port;
				lldpd_reconfigure_port(port);
			}
		}
	}
}                               /* add_new_interface */

void
add_vlans_from_ovsdb(char *hw_name)
{
	if (hw_name) {
		struct shash_node *sh_node = shash_find(&all_interfaces, hw_name);
		struct interface_data *itf = NULL;

		if (!sh_node) {
			VLOG_ERR("No hardware interface entry in hashmap or %s", hw_name);
			return;
		} else {

			itf = sh_node->data;
			/* VLAN TLV init */
			if (itf && itf->portdata) {
				struct port_data *port =
					shash_find_data(&all_ports, itf->portdata->name);
				if (port && port->vlan_mode != PORT_VLAN_MODE_TRUNK)
					set_lldp_pvid(itf->portdata->portrow, itf);
				if (port && port->vlan_mode != PORT_VLAN_MODE_ACCESS)
					set_lldp_trunk_vlans(itf->portdata->portrow, itf);
			}
		}
	}
}                               /* add_vlans_from_ovsdb */

/*
 * Set of macros create an array of smap keys and an array of smap values
 *
 * Every call creates a single key-value pair.
 * All strings are written to a single byte stream. *poffset is a
 * the byte stream index of next string; either key or value.
 * Once a key is written to the byte stream, *key_vec would point to it.
 * Once a val is written to the byte stream, *val_vec would point to it.
 *
 * LLDP_ENCODE_KEY_VAL     - key and value are strings
 * LLDP_ENCODE_KEY_VAL_INT - key is a string and value is an integer
 * LLDP_ENCODE_KEY_VAL_DBL - key is a string and value is a double (64b)
 */
#define NULL_STR ""
#define LLDP_ENCODE_KEY_VAL(pbuf, poffset, key, val, key_vec, val_vec)	\
	do {								\
		*key_vec = &pbuf[*poffset];				\
		*poffset += sprintf(&pbuf[*poffset], "%s", key);	\
		*poffset += 1;						\
		*val_vec = &pbuf[*poffset];				\
		*poffset += snprintf(&pbuf[*poffset], MAX_DESCR, "%s", ((val==NULL)?NULL_STR:val)); \
		*poffset += 1;						\
	} while(0);

#define LLDP_ENCODE_KEY_VAL_INT(pbuf, poffset, key, val, key_vec, val_vec) \
	do {								\
		*key_vec = &pbuf[*poffset];				\
		*poffset += sprintf(&pbuf[*poffset], "%s", key);	\
		*poffset += 1;						\
		*val_vec = &pbuf[*poffset];				\
		*poffset += snprintf(&pbuf[*poffset], MAX_DESCR, "%d", val); \
		*poffset += 1;						\
        } while(0);

#define LLDP_ENCODE_KEY_VAL_DBL(pbuf, poffset, key, val, key_vec, val_vec) \
	do {								\
		*key_vec = &pbuf[*poffset];				\
		*poffset += sprintf(&pbuf[*poffset], "%s", key);	\
		*poffset += 1;						\
		*val_vec = &pbuf[*poffset];				\
		*poffset += snprintf(&pbuf[*poffset], MAX_DESCR,	\
				     "%llx", (long long)val);		\
		*poffset += 1;						\
	} while(0);

/*
 * The functions creates a comma-separated list of strings,
 * one string at a time.
 */
static int
smap_list_set(char *svec, int *vec_cur, char *val)
{
	*vec_cur += sprintf(&svec[*vec_cur], "%s%s", *vec_cur ? "," : "", val);
	return 0;
}

/*
 * The functions gets one string at a time from a list of
 * a comma-separated strings.
 * Currently, it's unused.
 */
static int OVS_UNUSED
smap_list_get_next(char *svec, int *vec_cur, char *val)
{
	int i = 0, idx = *vec_cur;

	while ((svec[idx] != 0) && (svec[idx] != ',')) {
		val[i++] = svec[idx++];
	}

	val[i] = 0;

	if (svec[idx] == 0)
		*vec_cur = -1;
	else
		*vec_cur = idx + 1;

	return 0;
}

static void
lldp_create_mgmtip_list(char **mgmtip_list, struct lldpd_chassis *p_chassis)
{
	int offset_f0 = 0;
	int offset_f1 = 0;
	char mgmtip_str[INET6_ADDRSTRLEN];
	char iface_str[16];
	int if_cnt = 0;
	struct lldpd_mgmt *mgmtip;

	TAILQ_FOREACH(mgmtip, &p_chassis->c_mgmt, m_entries) {
		if (if_cnt++ >= MGMT_IF_MAX) {
			VLOG_ERR("Too many mgmt IP entries. Increase MGMT_IF_MAX");
			break;
		}
		VLOG_DBG("mgmt entry family= %d ip= %x if= %d",
			 mgmtip->m_family, mgmtip->m_addr.inet.s_addr,
			 mgmtip->m_iface);

		if (inet_ntop(lldpd_af(mgmtip->m_family), &mgmtip->m_addr,
			      mgmtip_str, INET6_ADDRSTRLEN) != NULL) {
			smap_list_set(mgmtip_list[0], &offset_f0, mgmtip_str);
			sprintf(iface_str, "%d", mgmtip->m_iface);
			smap_list_set(mgmtip_list[1], &offset_f1, iface_str);
		} else {
			VLOG_ERR("Failed to convert IP address to string. error: %s",
				 strerror(errno));
		}
	}
}

static void
lldp_create_vlan_list(char **vlan_list, struct lldpd_port *p_nbr)
{
	int offset_f0 = 0;
	int offset_f1 = 0;
	char vid_str[16];
	struct lldpd_vlan *vlan;

	TAILQ_FOREACH(vlan, &p_nbr->p_vlans, v_entries) {
		smap_list_set(vlan_list[0], &offset_f0, vlan->v_name);
		sprintf(vid_str, "%d", vlan->v_vid);
		smap_list_set(vlan_list[1], &offset_f1, vid_str);
	}
}

static void
lldp_create_pi_list(char **pi_list, struct lldpd_port *p_nbr)
{
	int offset_f0 = 0;
	int offset_f1 = 0;
	char vid_str[16];
	struct lldpd_pi *p_pi;

	TAILQ_FOREACH(p_pi, &p_nbr->p_pids, p_entries) {
		smap_list_set(pi_list[0], &offset_f0, p_pi->p_pi);
		sprintf(vid_str, "%d", p_pi->p_pi_len);
		smap_list_set(pi_list[1], &offset_f1, vid_str);
	}
}

static void
lldp_create_ppvid_list(char **ppvids_list, struct lldpd_port *p_nbr)
{
	int offset_f0 = 0;
	int offset_f1 = 0;
	char vid_str[16];
	struct lldpd_ppvid *p_ppvid;

	TAILQ_FOREACH(p_ppvid, &p_nbr->p_ppvids, p_entries) {
		sprintf(vid_str, "%d", p_ppvid->p_cap_status);
		smap_list_set(ppvids_list[0], &offset_f0, vid_str);
		sprintf(vid_str, "%d", p_ppvid->p_ppvid);
		smap_list_set(ppvids_list[1], &offset_f1, vid_str);
	}
}

/*
 * The update function writes new neighbor information
 * into OVSDB from lldpd port.
 */
static int
lldp_nbr_update(void *smap, struct lldpd_port *p_nbr)
{
	char *pbuf = NULL;
	char *decode_str = NULL;
	char *mgmtip_list[2] = { 0, 0 };    /* Interface list and IP list */
	char *vlan_list[2] = { 0, 0 };      /* vlan name list and vlan id list */
	char *ppvids_list[2] = { 0, 0 };    /* ppvid cap list and ppvid id list */
	char *pids_list[2] = { 0, 0 };      /* pi name list and pi length list */
	int offset = 0;
	int idx = 0;
	int i;
	char *key_array[KEY_VAL_MAX];
	char *val_array[KEY_VAL_MAX];

#ifdef ENABLE_DOT3
	struct lldpd_dot3_macphy *p_macphy;
	struct lldpd_dot3_power *p_power;
#endif
	struct lldpd_chassis *p_chassis;

	pbuf = xcalloc(1, KEY_VAL_STR_BUF_MAX);
	decode_str = xcalloc(1, STR_BUF_DECODE_MAX);
	if (p_nbr == NULL || decode_str == NULL) {
		VLOG_ERR("%s NULL port pointer", __FUNCTION__);
		goto cleanup;
	}
#ifdef ENABLE_DOT3
	p_macphy = &p_nbr->p_macphy;
	p_power = &p_nbr->p_power;
#endif

	p_chassis = p_nbr->p_chassis;

	/* Populate chassis key/val */
	if (p_chassis) {
		LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_CHASSIS_REF_COUNT,
					p_chassis->c_refcount, &key_array[idx],
					&val_array[idx++]);
		LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_CHASSIS_IDX,
					p_chassis->c_index, &key_array[idx],
					&val_array[idx++]);

		decode_property(decode_str, p_chassis->c_protocol, LLDP_MODE_INDEX);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_CHASSIS_PROTOCOL,
				    decode_str, &key_array[idx], &val_array[idx++]);

		decode_property(decode_str, p_chassis->c_id_subtype,
				LLDP_CHASSISID_SUBTYPE_INDEX);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_CHASSIS_ID_SUBTYPE,
				    decode_str, &key_array[idx], &val_array[idx++]);

		decode_nw_addr(decode_str, p_chassis->c_id, p_chassis->c_id_len);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_CHASSIS_ID, decode_str,
				    &key_array[idx], &val_array[idx++]);
		LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_CHASSIS_ID_LEN,
					p_chassis->c_id_len, &key_array[idx],
					&val_array[idx++]);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_CHASSIS_NAME,
				    p_chassis->c_name, &key_array[idx],
				    &val_array[idx++]);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_CHASSIS_DESCR,
				    p_chassis->c_descr, &key_array[idx],
				    &val_array[idx++]);

		decode_features(decode_str, p_chassis->c_cap_available,
				LLDP_CAP_INDEX);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_CHASSIS_CAP_AVAIL,
				    decode_str, &key_array[idx], &val_array[idx++]);

		decode_features(decode_str, p_chassis->c_cap_enabled, LLDP_CAP_INDEX);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_CHASSIS_CAP_ENABLE,
				    decode_str, &key_array[idx], &val_array[idx++]);

		LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_CHASSIS_TTL,
					p_chassis->c_ttl, &key_array[idx],
					&val_array[idx++]);

		if (g_ovsdb_test_nbr_mgmt_addr_list) {
			ovsdb_test_nbr_mgmt_addr_list(p_chassis);
		}

		if (!TAILQ_EMPTY(&p_chassis->c_mgmt)) {
			mgmtip_list[0] = xcalloc(1, MGMTIP_LIST_MAX);
			mgmtip_list[1] = xcalloc(1, MGMTIP_LIST_MAX);
			if (!mgmtip_list[0] || !mgmtip_list[1]) {
				VLOG_ERR("Error allocating mgmtip_list");
				goto cleanup;
			}
			lldp_create_mgmtip_list(mgmtip_list, p_chassis);
			LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_MGMT_IP_LIST,
					    mgmtip_list[0], &key_array[idx],
					    &val_array[idx++]);
			LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_MGMT_IF_LIST,
					    mgmtip_list[1], &key_array[idx],
					    &val_array[idx++]);
		}
	}

	/* Populate port key/val */
	LLDP_ENCODE_KEY_VAL_DBL(pbuf, &offset, LLDP_NBR_PORT_LASTCHANGE,
				p_nbr->p_lastchange, &key_array[idx],
				&val_array[idx++]);
	LLDP_ENCODE_KEY_VAL_DBL(pbuf, &offset, LLDP_NBR_PORT_LASTUPDATE,
				p_nbr->p_lastupdate, &key_array[idx],
				&val_array[idx++]);

	decode_property(decode_str, p_nbr->p_protocol, LLDP_MODE_INDEX);
	LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_PORT_PROTOCOL, decode_str,
			    &key_array[idx], &val_array[idx++]);

	LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_PORT_HIDDEN_IN,
				p_nbr->p_hidden_in, &key_array[idx],
				&val_array[idx++]);
	LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_PORT_HIDDEN_OUT,
				p_nbr->p_hidden_out, &key_array[idx],
				&val_array[idx++]);

	decode_property(decode_str, p_nbr->p_id_subtype,
			LLDP_PORTID_SUBTYPE_INDEX);
	LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_PORT_ID_SUBTYPE, decode_str,
			    &key_array[idx], &val_array[idx++]);

	if (p_nbr->p_id_subtype == LLDP_PORTID_SUBTYPE_LLADDR) {
		decode_nw_addr(decode_str, p_nbr->p_id, p_nbr->p_id_len);
	} else {
		sprintf(decode_str, "%s", p_nbr->p_id);
	}
	LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_PORT_ID, decode_str,
			    &key_array[idx], &val_array[idx++]);
	LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_PORT_ID_LEN,
				p_nbr->p_id_len, &key_array[idx],
				&val_array[idx++]);
	LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_PORT_DESCR, p_nbr->p_descr,
			    &key_array[idx], &val_array[idx++]);

	LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_PORT_MFS, p_nbr->p_mfs,
				&key_array[idx], &val_array[idx++]);

#ifdef ENABLE_DOT3
	/* MAC PHY - DOT3 */
	if (p_macphy) {
		decode_features(decode_str, p_macphy->autoneg_support,
				LLDP_DOT3_LINK_AUTONEG_INDEX);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_MAC_AUTONEG_SUPPORT,
				    decode_str, &key_array[idx], &val_array[idx++]);

		decode_features(decode_str, p_macphy->autoneg_enabled,
				LLDP_DOT3_LINK_AUTONEG_INDEX);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_MAC_AUTONEG_ENB,
				    decode_str, &key_array[idx], &val_array[idx++]);

		decode_features(decode_str, p_macphy->autoneg_advertised,
				LLDP_DOT3_LINK_AUTONEG_INDEX);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_MAC_AUTONEG_ADV,
				    decode_str, &key_array[idx], &val_array[idx++]);

		decode_property(decode_str, p_macphy->mau_type, LLDP_DOT3_MAU_INDEX);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_MAC_AUTONEG_TYPE,
				    decode_str, &key_array[idx], &val_array[idx++]);
	}

	/* POWER mgmt - DOT3 */
	if (p_power) {
		LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_POWER_DEVICE,
					p_power->devicetype, &key_array[idx],
					&val_array[idx++]);
		LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_POWER_SUPPORT,
					p_power->supported, &key_array[idx],
					&val_array[idx++]);
		LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_POWER_ENABLED,
					p_power->enabled, &key_array[idx],
					&val_array[idx++]);
		LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_POWER_PAIRCTL,
					p_power->paircontrol, &key_array[idx],
					&val_array[idx++]);
		LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_POWER_PAIR,
					p_power->pairs, &key_array[idx],
					&val_array[idx++]);
		LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_POWER_CLASS,
					p_power->class, &key_array[idx],
					&val_array[idx++]);
		LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_POWER_TYPE,
					p_power->powertype, &key_array[idx],
					&val_array[idx++]);
		LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_POWER_SRC,
					p_power->source, &key_array[idx],
					&val_array[idx++]);
		LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_POWER_PRIO,
					p_power->priority, &key_array[idx],
					&val_array[idx++]);
		LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_POWER_REQ,
					p_power->requested, &key_array[idx],
					&val_array[idx++]);
		LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_POWER_ALLOC,
					p_power->allocated, &key_array[idx],
					&val_array[idx++]);
	}
#endif

	/* vlans */
	LLDP_ENCODE_KEY_VAL_INT(pbuf, &offset, LLDP_NBR_PORT_PVID, p_nbr->p_pvid,
				&key_array[idx], &val_array[idx++]);

#ifdef ENABLE_DOT1
	if (!TAILQ_EMPTY(&p_nbr->p_vlans)) {
		vlan_list[0] = xcalloc(1, VLAN_LIST_STR_MAX);
		vlan_list[1] = xcalloc(1, VLAN_LIST_INT_MAX);
		if (!vlan_list[0] || !vlan_list[1]) {
			VLOG_ERR("Error allocating vlan_list");
			goto cleanup;
		}
		lldp_create_vlan_list(vlan_list, p_nbr);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_VLAN_NAME_LIST,
				    vlan_list[0], &key_array[idx], &val_array[idx++]);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_VLAN_ID_LIST, vlan_list[1],
				    &key_array[idx], &val_array[idx++]);
	}

	if (!TAILQ_EMPTY(&p_nbr->p_ppvids)) {
		ppvids_list[0] = xcalloc(1, VLAN_LIST_INT_MAX);
		ppvids_list[1] = xcalloc(1, VLAN_LIST_INT_MAX);
		if (!ppvids_list[0] || !ppvids_list[1]) {
			VLOG_ERR("Error allocating ppvids_list");
			goto cleanup;
		}
		lldp_create_ppvid_list(ppvids_list, p_nbr);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_PPVIDS_CAP_LIST,
				    ppvids_list[0], &key_array[idx],
				    &val_array[idx++]);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_PPVIDS_PPVID_LIST,
				    ppvids_list[1], &key_array[idx],
				    &val_array[idx++]);
	}

	if (!TAILQ_EMPTY(&p_nbr->p_pids)) {
		pids_list[0] = xcalloc(1, VLAN_LIST_STR_MAX);
		pids_list[1] = xcalloc(1, VLAN_LIST_INT_MAX);
		if (!pids_list[0] || !pids_list[1]) {
			VLOG_ERR("Error allocating pids_list");
			goto cleanup;
		}
		lldp_create_pi_list(pids_list, p_nbr);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_PIDS_NAME_LIST,
				    pids_list[0], &key_array[idx], &val_array[idx++]);
		LLDP_ENCODE_KEY_VAL(pbuf, &offset, LLDP_NBR_PIDS_LEN_LIST,
				    pids_list[1], &key_array[idx], &val_array[idx++]);
	}
#endif

	/* Debug - print key/val arrays */
#ifdef LLDP_NBR_DEBUG
	VLOG_INFO("log: key_array[] val_array[]");
	for (i = 0; i < idx; i++) {
		VLOG_INFO("%s - %s \n", key_array[i], val_array[i]);
	}
	VLOG_INFO("buffer offset %d\n", offset);
#endif

	/*
	 *  Invoke smap_set to save in OVSDB
	 */

	smap_init(smap);
	for (i = 0; i < idx; i++) {
		struct smap_node *node;

		node = smap_add(smap, key_array[i], val_array[i]);
		if (node == NULL) {
			VLOG_ERR("Failed to add key=%s, val=%s",
				 key_array[i], val_array[i]);
			goto cleanup;
		}
	}

cleanup:

	if (mgmtip_list[0]) {
		free(mgmtip_list[0]);
	}
	if (mgmtip_list[1]) {
		free(mgmtip_list[1]);
	}
	if (vlan_list[0]) {
		free(vlan_list[0]);
	}
	if (vlan_list[1]) {
		free(vlan_list[1]);
	}
	if (ppvids_list[0]) {
		free(ppvids_list[0]);
	}
	if (ppvids_list[1]) {
		free(ppvids_list[1]);
	}
	if (pids_list[0]) {
		free(pids_list[0]);
	}
	if (pids_list[1]) {
		free(pids_list[1]);
	}
	if (pbuf) {
		free(pbuf);
	}
	if (decode_str) {
		free(decode_str);
	}

	return 0;
}

/*
 * Function       : lldpd_diag_dump_basic_cb
 * Responsibility : callback handler function for diagnostic dump basic
 *                  it allocates memory as per requirment and populates data.
 *                  INIT_DIAG_DUMP_BASIC will free allocated memory.
 * Parameters     : feature name string,buffer ptr
 * Returns        : void
 */

static void
lldpd_diag_dump_basic_cb(const char *feature , char **buf)
{
	if (!buf)
		return;
	*buf =  xcalloc(1,BUF_LEN);
	if (*buf) {
		/* populate basic diagnostic data to buffer  */
		lldpd_dump(*buf,BUF_LEN);
		VLOG_DBG("basic diag-dump data populated for feature %s",
				feature);
	} else{
		VLOG_ERR("Memory allocation failed for feature %s , %d bytes",
				feature , BUF_LEN);
	}
	return ;
}

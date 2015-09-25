
/*
 * (c) Copyright 2015 Hewlett Packard Enterprise Development LP
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
 * File: lldpd_ovsdb_if.h
 *
 * Purpose: This file includes all public interface defines needed by
 *          the new lldpd_ovsdb.c for lldpd - ovsdb integration
 */

#ifndef LLDPD_OVSDB_H
#define LLDPD_OVSDB_H 1

#include "lldpd.h"

enum {
	LLDP_DECODE_LIST,
	LLDP_DECODE_BITMAP
};

enum {
	LLDP_CHASSISID_SUBTYPE_INDEX,
	LLDP_PORTID_SUBTYPE_INDEX,
	LLDP_DOT3_MAU_INDEX,
	LLDP_DOT3_POWER_INDEX,
	LLDP_DOT3_POWERPAIRS_INDEX,
	LLDP_DOT3_POWER_8023AT_INDEX,
	LLDP_DOT3_POWER_SOURCE_INDEX,
	LLDP_DOT3_POWER_PRIO_INDEX,
	LLDP_DOT3_LINK_AUTONEG_INDEX,
	LLDP_CAP_INDEX,
	LLDP_PPVID_CAP_INDEX,
	LLDP_MGMT_ADDR_INDEX,
	LLDP_MGMT_IFACE_INDEX,
	LLDP_MED_CLASS_INDEX,
	LLDP_MED_APPTYPE_INDEX,
	LLDP_MED_LOCFORMAT_INDEX,
	LLDP_MED_LOCATION_GEOID_INDEX,
	LLDP_MED_LOCATION_ALTITUDE_INDEX,
	LLDP_MED_POW_TYPE_INDEX,
	LLDP_MED_POW_SOURCE_INDEX,
	LLDP_MED_POW_PRIO_INDEX,
	LLDP_MED_CAP_INDEX,
	LLDP_MODE_INDEX,
	LLDP_BOND_SLAVE_SRC_MAC_TYPE_INDEX,
	LLDP_INDEX_MAX
};

#define LLDP_CHASSISID_SUBTYPE_DECODE       LLDP_DECODE_LIST
#define LLDP_PORTID_SUBTYPE_DECODE          LLDP_DECODE_LIST
#define LLDP_DOT3_MAU_DECODE                LLDP_DECODE_LIST
#define LLDP_DOT3_POWER_DECODE              LLDP_DECODE_LIST
#define LLDP_DOT3_POWERPAIRS_DECODE         LLDP_DECODE_LIST
#define LLDP_DOT3_POWER_8023AT_DECODE       LLDP_DECODE_LIST
#define LLDP_DOT3_POWER_SOURCE_DECODE       LLDP_DECODE_LIST
#define LLDP_DOT3_POWER_PRIO_DECODE         LLDP_DECODE_LIST
#define LLDP_DOT3_LINK_AUTONEG_DECODE       LLDP_DECODE_BITMAP
#define LLDP_CAP_DECODE                     LLDP_DECODE_BITMAP
#define LLDP_PPVID_CAP_DECODE               LLDP_DECODE_BITMAP
#define LLDP_MGMT_ADDR_DECODE               LLDP_DECODE_LIST
#define LLDP_MGMT_IFACE_DECODE              LLDP_DECODE_LIST
#define LLDP_MED_CLASS_DECODE               LLDP_DECODE_LIST
#define LLDP_MED_APPTYPE_DECODE             LLDP_DECODE_LIST
#define LLDP_MED_LOCFORMAT_DECODE           LLDP_DECODE_LIST
#define LLDP_MED_LOCATION_GEOID_DECODE      LLDP_DECODE_LIST
#define LLDP_MED_LOCATION_ALTITUDE_DECODE   LLDP_DECODE_LIST
#define LLDP_MED_POW_TYPE_DECODE            LLDP_DECODE_LIST
#define LLDP_MED_POW_SOURCE_DECODE          LLDP_DECODE_LIST
#define LLDP_MED_POW_PRIO_DECODE            LLDP_DECODE_LIST
#define LLDP_MED_CAP_DECODE                 LLDP_DECODE_BITMAP
#define LLDPD_MODE_DECODE                   LLDP_DECODE_LIST
#define LLDP_BOND_SLAVE_SRC_MAC_TYPE_DECODE LLDP_DECODE_LIST

/*
 * Neighbor info keys
 */
#define LLDP_NBR_CHASSIS_CAP_AVAIL "chassis_capability_available"
#define LLDP_NBR_CHASSIS_CAP_ENABLE "chassis_capability_enabled"
#define LLDP_NBR_CHASSIS_DESCR "chassis_description"
#define LLDP_NBR_CHASSIS_ID "chassis_id"
#define LLDP_NBR_CHASSIS_ID_LEN "chassis_id_len"
#define LLDP_NBR_CHASSIS_ID_SUBTYPE "chassis_id_subtype"
#define LLDP_NBR_CHASSIS_IDX "chassis_index"
#define LLDP_NBR_CHASSIS_NAME "chassis_name"
#define LLDP_NBR_CHASSIS_PROTOCOL "chassis_protocol"
#define LLDP_NBR_CHASSIS_REF_COUNT "chassis_refcount"
#define LLDP_NBR_CHASSIS_TTL "chassis_ttl"
#define LLDP_NBR_MAC_AUTONEG_ADV "macphy_autoneg_advertised"
#define LLDP_NBR_MAC_AUTONEG_ENB "macphy_autoneg_enabled"
#define LLDP_NBR_MAC_AUTONEG_SUPPORT "macphy_autoneg_support"
#define LLDP_NBR_MAC_AUTONEG_TYPE "macphy_mau_type"
#define LLDP_NBR_PORT_DESCR "port_description"
#define LLDP_NBR_PORT_HIDDEN_IN "port_hidden_in"
#define LLDP_NBR_PORT_HIDDEN_OUT "port_hidden_out"
#define LLDP_NBR_PORT_ID "port_id"
#define LLDP_NBR_PORT_ID_LEN "port_id_len"
#define LLDP_NBR_PORT_ID_SUBTYPE "port_id_subtype"
#define LLDP_NBR_PORT_LASTCHANGE "port_lastchange"
#define LLDP_NBR_PORT_LASTUPDATE "port_lastupdate"
#define LLDP_NBR_PORT_MFS "port_mfs"
#define LLDP_NBR_PORT_PROTOCOL "port_protocol"
#define LLDP_NBR_PORT_PVID "port_pvid"
#define LLDP_NBR_POWER_ALLOC "power_allocated"
#define LLDP_NBR_POWER_CLASS "power_class"
#define LLDP_NBR_POWER_DEVICE "power_devicetype"
#define LLDP_NBR_POWER_ENABLED "power_enabled"
#define LLDP_NBR_POWER_PAIRCTL "power_paircontrol"
#define LLDP_NBR_POWER_PAIR "power_pairs"
#define LLDP_NBR_POWER_TYPE "power_powertype"
#define LLDP_NBR_POWER_PRIO "power_priority"
#define LLDP_NBR_POWER_REQ "power_requested"
#define LLDP_NBR_POWER_SRC "power_source"
#define LLDP_NBR_POWER_SUPPORT "power_supported"

#define LLDP_NBR_MGMT_IP_LIST "mgmt_ip_list"
#define LLDP_NBR_MGMT_IF_LIST "mgmt_iface_list"
#define LLDP_NBR_VLAN_NAME_LIST "vlan_name_list"
#define LLDP_NBR_VLAN_ID_LIST "vlan_id_list"
#define LLDP_NBR_PPVIDS_CAP_LIST "ppvids_cap_list"
#define LLDP_NBR_PPVIDS_PPVID_LIST "ppvids_ppvid_list"
#define LLDP_NBR_PIDS_NAME_LIST "pids_name_list"
#define LLDP_NBR_PIDS_LEN_LIST "pids_len_list"

/* check any stat/counter changes every so often and report if changed */
#define LLDP_CHECK_STATS_FREQUENCY_DFLT             5   /* seconds */
#define LLDP_CHECK_STATS_FREQUENCY_DFLT_MSEC				\
	(LLDP_CHECK_STATS_FREQUENCY_DFLT * 1000)    /* milliseconds */

void lldpd_ovsdb_init(int argc, char *argv[]);
void init_ovspoll_to_libevent(struct lldpd *cfg);
void lldpd_ovsdb_exit(void);
void add_lldpd_hardware_interface(struct lldpd_hardware *hw);
void del_lldpd_hardware_interface(struct lldpd_hardware *hw);
void ovs_libevent_schedule_nbr(void *arg);
void add_vlans_from_ovsdb(char *hw_name);
#endif

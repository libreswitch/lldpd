## Contents

  * [Component design of LLDP](#component-design-of-lldp)
    * [OVSDB Integration](#ovsdb-integration)
    * [Daemon interactions](#daemon-interactions)
    * [OVSDB-Schema](#ovsdb-schema)
      * [System table](#system-table)
      * [Interface table](#interface-table)
    * [Code Design](#code-design)
      * [OVSDB initialization](#ovsdb-initialization)
      * [Global configs](#global-configs)
      * [Per interface configurations](#per-interface-configurations)
      * [Neighbor table syncronization](#neighbor-table-syncronization)
        * [LLDP Neighbor decode Functions](#lldp-neighbor-decode-functions)
        * [LLDP Neighbor key-value pair updates](#lldp-neighbor-key-value-pair-updates)
      * [Statistics and Counter transactions](#statistics-and-counter-transactions)
  * [References](#references)

# Component design of LLDP
The "lldpd" project from https://vincentbernat.github.io/lldpd/ is used for the ops-lldpd daemon. This daemon is responsible for advertising and receiving LLDP packets from its neighbors. The ability to interface with OVSDB for configurations and to push neighbor updates and statistics was added in the OVSDB Interface layer.

## OVSDB Integration
The lldpd daemon is a single threaded daemon and uses libevent which calls back associated callback functions for events.
Daemons which need to interface with OVSDB using OVSDB-IDL use the poll_loop structure to populate sockets and timeout events. All "run" and "wait" functions are called inside a while(1) {..} loop and poll_block() is used to wait on events.

```
Example OVS Loop

while(!exiting)
{
    run functions
    wait functions - adds sockets and timers to poll_loop
    poll_block - waits for events
}
```
To integrate lldpd with OVSDB, the OVS style poll_loop is integrated into the libevent style looping in lldpdi as follows:
- One callback function "ovs_libevent_cb" is registered with libevent for all OVSDB events (notifications, timers).
- After calling the "wait" functions, which updates the poll_loop structure with the sockets and timeouts, these sockets and timeouts are registered with libevent.
- The "ovs_libevent_cb" calls the "run" functions, and the "wait" functions and then re-registers "ovs_libevent_cb" for new set of socket or timeout events.

```
Pseudo-code for ovs_libevent_cb

ovs_libevent_cb(..)
{
    ovs_clear_libevents
    Run all run() functions
    Run all wait() functions
    Register poll_loop events with libevent
    Register poll_loop timeout with libevent
}
```

##Daemon interactions

```ditaa
+----------------------------------------+      +------------+
|               LLDP Daemon              |      |    OVSDB   |
|  +--------------+   +---------+ Global Configs| +--------+ |
|  |              |   |         +<----------------+ System | |
|  |              |   | LLDPD   |  Global stats | | TABLE  | |
|  |  Open Source |   | OVSDB   | -------+------> |        | |
|  |    lldpd     +<->+Interface|        |      | +--------+ |
|  |              |   |         | Interface cfg | +--------+ |
|  |              |   |         +<--------------+-+        | |
|  |              |   |         | Neighbors/stats |        | |
|  +--------------+   +---------+ --------------> |Interface |
+---^-----^------------------------^-----+      | |  Table | |
    |     |                        |            | |        | |
    |     |    LLDP frames rx/tx   |            | +--------+ |
    v     v                        v            +------------+
+---+-----+------------------------+------+
|                KERNEL                   |
|  +-+   +-+  . . . . . . . .     +-+     |
|  +-+   +-+   interfaces         +-+     |
|                                         |
+-----------------------------------------+

```

## OVSDB-Schema
### System table
```
System:other_config
Keys:
lldp_enable
lldp_tlv_mgmt_addr_enable
lldp_tlv_port_desc_enable
lldp_tlv_port_vlan_enable
lldp_tlv_sys_cap_enable
lldp_tlv_sys_desc_enable
lldp_tlv_sys_name_enable
lldp_mgmt_addr
lldp_tx_interval
lldp_hold

System:statistics
Keys:
lldp_table_inserts
lldp_table_deletes
lldp_table_drops
lldp_table_ageouts
```

### Interface table
```
Interface:other_config
Keys:
lldp_enable_dir ("off","rx","tx","rxtx")

Interface:status
Keys:
lldp_local_port_vlan
lldp_local_port_desc

Interface:statistics
Keys:
lldp_tx
lldp_rx
lldp_rx_discard
lldp_rx_tlv_disc

Interface:lldp_neighbor_info
Keys:
port_description
port_id
port_protocol
port_pvid
chassis_description
chassis_id
chassis_id_subtype
chassis_name
chassis_capability_available
chassis_capability_enabled
mgmt_ip_list
mgmt_iface_list
vlan_name_list
vlan_id_list
```

## Code design
The OVSDB Interface component for ops-lldpd is enabled using the --enable-ovsdb configuration option. Code added into original source lldpd source files for this enablement are under the ENABLE_OVSDB macro.

The OVSDB interface layer code resides in lldpd_ovsdb_if.c.

The OVSDB interface layer code integrates lldpd with OVSDB and ovs poll_loop and it provides hooks to the lldpd daemon to perform the following tasks:
-Reads lldpd related configuration data and applies configuration to lldpd during startup.
-Receives administrative configuration changes and applies changes to lldpd config during operations.
-Updates statistics and neighbor tables periodically in the database and syncronizes statistics and neighbor information in the internal lldp data structure on lldpd restart.

### OVSDB initialization
The ops-lldpd module registers with OVSDB tables and with the columns for lldp configuration notifications.

###Global configs
The lldp source stores global configurations in  "struct lldpd", which is updated when receiving configuration updates from the OVSDB.
All global OVSDB configurations in the System:user_config column such as lldp_enable, lldp_mgmt_addr, lldp_hold, lldp_tx_interval, lldp_tlv_mgmt_addr_enable are handled in "lldpd_apply_global_changes".

### Per interface configurations

We keep the hash of all interfaces in the system in the "all_interfaces" hash. This is a hash which stores the following data structure for each interface.
```
struct interface_data {
        char *name;                 /* Always non null */
        int native_vid;             /* "tag" column - native VLAN ID. */
        int synced_from_db;
        const struct ovsrec_interface *ifrow;       /* Handle to ovsrec row */
        struct port_data *portdata; /* Handle to port data */
        struct lldpd_hardware *hw;  /* Handle to lldp hardware interface */
};
```
This holds a pointer to the OVSDB row entry of the interface and corresponding lldpd_hardware that is maintained by lldpd.

This is used to configure the per interface level configuration lldp_enable_dir. The per interface configuration is handled by "lldpd_apply_interface_changes".

###Neighbor table syncronization

LLDP triggers a neighbor info change by setting a change request in lldp's interface and sending a libevent message to schedule a database update.

LLDP scheduling logic makes sure no outstanding transaction is in progress as well as ensures that the previous neighbor transaction succeeded. It then scans all LLDP ports and looks for neighbor changes triggered by LLDPD. The following changes are supported:
-ADD--Writes or rewrites the entire neighbor table to the OVSDB.
-MOD--Writes or rewrites the entire neighbor table to the OVSDB.
-UPDATE--Keepalive refresh and only requires an update to the time change.
-DELETE--Removes neighbor info from the OVSDB.

LLDP scans all LLDP ports looking for any old neighbor information. Old neighbor information is detected by checking the update time against the current time. In this case the neighbor cell is deleted. This covers any corner cases, such as port disconnects or restarts, in which lldpd fails to report old neighbor entries in a timely manner.

Any neighbor transaction failure triggers a full synchronization of LLDP internal neighbor iinformation in the database by copying the LLDP table information from LLDP to the OVSDB for each active LLDP interface.

If the LLDP feature is disabled, the neighbor information is deleted from all interfaces.

####LLDP neighbor decode functions
The LLDPD internal structures are strings, numbers, network addresses, opcodes and bitmasks. Strings are stored as strings, numbers are converted to strings and network addresses are parsed and converted to strings.

Opcodes and bitmasks use a decoder table to convert an opcode into a string and a bitmask into a list of strings (a string for each bit in the mask).

Examples:

Management address is an opcode with possible values:
NONE -   0
IP4  -   1
IP6  -   2

lldp_decode_table[IP_ADDRESS_BASE]   = "None"
lldp_decode_table[IP_ADDRESS_BASE+1] = "IPv4"
lldp_decode_table[IP_ADDRESS_BASE+2] = "IPv6"

Port capability is a logical or of the following modes:
OTHER     -  0x01
REPEATER  -  0x02
BRIDGE    -  0x04
WLAN      -  0x08
ROUTER    -  0x10
TELEPHONE -  0x20
DOCSIS    -  0x40
STATION   -  0x80

lldp_decode_table[CAPABILITY_BASE+1]   = "Other"
lldp_decode_table[CAPABILITY_BASE+2]   = "Repeater"
lldp_decode_table[CAPABILITY_BASE+3]   = "Bridge"
lldp_decode_table[CAPABILITY_BASE+4]   = "WLAN"
lldp_decode_table[CAPABILITY_BASE+5]   = "Router"
lldp_decode_table[CAPABILITY_BASE+6]   = "Telephone"
lldp_decode_table[CAPABILITY_BASE+7]   = "DOCSIS"
lldp_decode_table[CAPABILITY_BASE+8]   = "Station"

####LLDP neighbor key-value pair updates
The database updates parses every lldpd interface field and decodes it according to field type. Each field is stored as a key-value pair in neighbor information. Linked list are broken into multiple lists of structure members, and each list is stored as a comma-seperated linked list of strings.

###Statistics and counter transactions
Statistic reporting is scheduled every "lldpd_stats_check_interval" seconds and if somehow it gets called before that time has elapsed, it reschedules itself to be called at precisely that interval. The main worker function for updating statistics to OVSDB is done in "lldpd_stats_analyze"

#References
http://vincentbernat.github.io/lldpd/

if ENABLE_OVSDB
AM_CFLAGS = -I $(LOCAL_SYSROOT_DIR)/usr/local/include/ovs -DOPS
endif

libdir = /usr/lib/snmp/plugins
lib_LTLIBRARIES = liblldp_snmp.la

liblldp_snmp_la_SOURCES = LLDP_MIB_custom.c LLDP_MIB_scalars_ovsdb_get.c lldpPortConfigTable_data_get.c	\
                          lldpPortConfigTable_ovsdb_get.c LLDP_MIB_plugins.c  lldpPortConfigTable.c \
                          lldpPortConfigTable_data_set.c LLDP_MIB_scalars.c  \
                          lldpPortConfigTable_data_access.c  lldpPortConfigTable_interface.c \
                          lldpConfigManAddrTable.c lldpConfigManAddrTable_data_access.c \
                          lldpConfigManAddrTable_data_get.c lldpConfigManAddrTable_interface.c lldpConfigManAddrTable_ovsdb_get.c \
                          lldpLocManAddrTable.c lldpLocManAddrTable_data_access.c lldpLocManAddrTable_data_get.c \
                          lldpLocManAddrTable_data_set.c  lldpLocManAddrTable_interface.c lldpLocManAddrTable_ovsdb_get.c \
                          lldpLocPortTable.c  lldpLocPortTable_data_access.c lldpLocPortTable_data_get.c \
                          lldpLocPortTable_data_set.c lldpLocPortTable_interface.c lldpLocPortTable_ovsdb_get.c  \
                          lldpRemManAddrTable.c lldpRemManAddrTable_data_access.c lldpRemManAddrTable_data_get.c \
                          lldpRemManAddrTable_data_set.c lldpRemManAddrTable_interface.c lldpRemManAddrTable_ovsdb_get.c \
                          lldpRemOrgDefInfoTable.c lldpRemOrgDefInfoTable_data_access.c  lldpRemOrgDefInfoTable_data_get.c \
                          lldpRemOrgDefInfoTable_data_set.c  lldpRemOrgDefInfoTable_interface.c lldpRemOrgDefInfoTable_ovsdb_get.c \
                          lldpRemTable.c lldpRemTable_data_access.c lldpRemTable_data_get.c   lldpRemTable_data_set.c   \
                          lldpRemTable_interface.c  lldpRemTable_ovsdb_get.c   \
                          lldpRemUnknownTLVTable.c  lldpRemUnknownTLVTable_data_access.c  lldpRemUnknownTLVTable_data_get.c \
                          lldpRemUnknownTLVTable_data_set.c  lldpRemUnknownTLVTable_interface.c \
                          lldpRemUnknownTLVTable_ovsdb_get.c   \
                          lldpStatsRxPortTable.c  lldpStatsRxPortTable_data_access.c  lldpStatsRxPortTable_data_get.c \
                          lldpStatsRxPortTable_data_set.c  lldpStatsRxPortTable_interface.c \
                          lldpStatsRxPortTable_ovsdb_get.c \
                          lldpStatsTxPortTable.c  lldpStatsTxPortTable_data_access.c  lldpStatsTxPortTable_data_get.c \
                          lldpStatsTxPortTable_data_set.c  lldpStatsTxPortTable_interface.c \
                          lldpStatsTxPortTable_ovsdb_get.c
liblldp_snmp_la_LDFLAGS = -module -shared -avoid-version

if ENABLE_OVSDB
AM_CFLAGS = -I $(LOCAL_SYSROOT_DIR)/usr/local/include/ovs -DOPS
endif

libdir = /usr/lib/snmp/plugins
lib_LTLIBRARIES = liblldp_snmp.la

liblldp_snmp_la_SOURCES = LLDP_MIB_custom.c LLDP_MIB_scalars_ovsdb_get.c lldpPortConfigTable_data_get.c	\
                          lldpPortConfigTable_ovsdb_get.c LLDP_MIB_plugins.c  lldpPortConfigTable.c \
                          lldpPortConfigTable_data_set.c LLDP_MIB_scalars.c  \
                          lldpPortConfigTable_data_access.c  lldpPortConfigTable_interface.c
liblldp_snmp_la_LDFLAGS = -module -shared -avoid-version

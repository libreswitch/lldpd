# -*- coding: utf-8 -*-
# (C) Copyright 2015 Hewlett Packard Enterprise Development LP
# All Rights Reserved.
#
#    Licensed under the Apache License, Version 2.0 (the "License"); you may
#    not use this file except in compliance with the License. You may obtain
#    a copy of the License at
#
#         http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
#    WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
#    License for the specific language governing permissions and limitations
#    under the License.
#
##########################################################################

"""
OpenSwitch Test for lldp SNMP traps
Author: Avinash(avinash.varma@hpe.com)
Description: Tests for snmp traps in LLDP
"""

# from pytest import mark
from time import sleep

TOPOLOGY = """
# +-------+
# |       |     +-------+     +-------+
# |  hs1  <----->  ops1  <----->  ops2  |
# |       |     +-------+     +-------+
# +-------+

# Nodes
[type=openswitch name="OpenSwitch 1"] ops1
[type=openswitch name="OpenSwitch 2"] ops2
[type=oobmhost name="Host 1" image="openswitch/snmpd:latest"] hs1

# Ports
[force_name=oobm] ops1:sp1

# Links
ops1:if01 -- ops2:if01
ops1:sp1 -- hs1:1
"""


def config_lldp(ops):
    with ops.libs.vtysh.Configure() as ctx:
        ctx.lldp_enable()


def config_trapreceiver(ops, hs):
    opsmgmtip = '10.10.10.4/24'
    trapserverip = '10.10.10.5'

    with ops.libs.vtysh.ConfigInterfaceMgmt() as ctx:
        ctx.ip_static(opsmgmtip)

    with ops.libs.vtysh.Configure() as ctx:
        ctx.snmp_server_host_trap_version(trapserverip, 'v1')

    with ops.libs.vtysh.Configure() as ctx:
        ctx.snmp_server_host_trap_version(trapserverip, 'v2c')

    ops.libs.vtysh.show_running_config()

    hs('echo "authCommunity log,execute,net public" > \
        /etc/snmp/snmptrapd.conf')

    hs("ifconfig")
    hs("ifconfig eth0 10.10.10.5/24 up")

    sleep(10)

    ping = hs("ping -c 1 10.10.10.4")
    assert "1 packets transmitted, 1 packets received" in ping

    hs('snmptrapd -Lftemp.txt')


def verify_lldpremtableschange(ops1, ops2, hs1):
    ops1p1 = ops1.ports['if01']
    ops2p1 = ops2.ports['if01']

    # This is main trap oid which is present in the trap output
    remtableschangeoid = 'iso.0.8802.1.1.2.0.0.1'
    # This is the insert counter which is incremented when lldp is enabled.
    # Verify if the count has increased in the trap output
    remtablesinsertoid = 'iso.0.8802.1.1.2.1.2.2'

    config_lldp(ops1)
    config_lldp(ops2)

    config_trapreceiver(ops1, hs1)

    with ops1.libs.vtysh.ConfigInterface(ops1p1) as ctx:
        ctx.no_shutdown()

    with ops2.libs.vtysh.ConfigInterface(ops2p1) as ctx:
        ctx.no_shutdown()

    ops1("show run", shell="vtysh")
    ops2("show run", shell="vtysh")

    sleep(40)
    ops1("show lldp neighbor-info 1", shell="vtysh")
    ops2("show lldp neighbor-info 1", shell="vtysh")

    with ops2.libs.vtysh.ConfigInterface(ops1p1) as ctx:
        ctx.shutdown()

    ops2("show run", shell="vtysh")

    sleep(130)
    ops1("show lldp neighbor-info 1", shell="vtysh")
    ops2("show lldp neighbor-info 1", shell="vtysh")

    ret = hs1('cat temp.txt')
    trapcount = 0
    incount = 0
    for line in ret.split('\n'):
        if remtableschangeoid in line:
            trapcount = trapcount + 1
        if remtablesinsertoid + ' = Gauge32: 1' in line:
            incount = incount + 1

    assert trapcount == 4
    assert incount == 2


def test_snmptrap_ft_lldp(topology, step):
    ops1 = topology.get('ops1')
    ops2 = topology.get('ops2')
    hs1 = topology.get('hs1')

    assert ops1 is not None
    assert ops2 is not None
    assert hs1 is not None

    verify_lldpremtableschange(ops1, ops2, hs1)

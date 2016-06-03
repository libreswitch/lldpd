# -*- coding: utf-8 -*-
# (C) Copyright 2016 Hewlett Packard Enterprise Development LP
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
###############################################################################
#
# No description or steps defined on original test case.
# Ported by:   Mauricio Fonseca
# Condition to pass ->
#              fail ->
#
###############################################################################

"""
OpenSwitch Test for vlan related configurations.
"""

from pytest import mark
from time import sleep

TOPOLOGY = """
# +-------+     +--------+
# |  ops1  <----->  ops2 |
# +-------+     +--------+

# Nodes
[type=openswitch name="OpenSwitch 1"] ops1
[type=openswitch name="OpenSwitch 2"] ops2

# Links
ops1:1 -- ops2:1
ops1:2 -- ops2:2
ops1:3 -- ops2:3
ops1:4 -- ops2:4
"""


@mark.test_id(15305)
def test_lldp_ft_interface_txrx(topology):
    ops1 = topology.get('ops1')
    ops2 = topology.get('ops2')

    assert ops1 is not None
    assert ops2 is not None

    def findneighbor(dut, port, remoteport):
        value = None
        for retry in range(1, 3):
            neighborinfo = dut.libs.vtysh.show_lldp_neighbor_info(port)
            print(neighborinfo['neighbor_port_id'])
            if str(neighborinfo['neighbor_port_id']) == str(remoteport):
                value = neighborinfo['neighbor_port_id']
                break
            else:
                cmd1 = ops1("ovs-vsctl list interface 1 | grep "
                            "lldp_neighbor_info", shell="bash")
                print(cmd1)
                cmd2 = ops2("ip netns exec swns ifconfig", shell="bash")
                print(cmd2)
                value = neighborinfo['neighbor_port_id']
                print("Not found on retry: " + str(retry))
                sleep(10)
        return value

    print("Step 1- Configure lldp interfaces on switch 1")
    with ops1.libs.vtysh.ConfigInterface('2') as ctx:
        ctx.no_lldp_transmit()
    with ops1.libs.vtysh.ConfigInterface('3') as ctx:
        ctx.no_lldp_receive()
    with ops1.libs.vtysh.ConfigInterface('4') as ctx:
        ctx.no_lldp_transmit()
    with ops1.libs.vtysh.ConfigInterface('4') as ctx:
        ctx.no_lldp_receive()

    print("Step 2- Configure lldp on switch 1")
    with ops1.libs.vtysh.Configure() as ctx:
        ctx.lldp_enable()

    print("Step 3- Configure lldp on switch 2")
    with ops2.libs.vtysh.Configure() as ctx:
        ctx.lldp_enable()

    print("Step 4- Disabling routing on the port 1 of switch 1")
    with ops1.libs.vtysh.ConfigInterface('1') as ctx:
        ctx.no_routing()

    print("Step 5- Enabling interface 1 and 2 on switch 1")
    with ops1.libs.vtysh.ConfigInterface('1') as ctx:
        ctx.no_shutdown()

    with ops1.libs.vtysh.ConfigInterface('2') as ctx:
        ctx.no_shutdown()

    print("Step 6- Disabling routing on the port 2 of switch 1")
    with ops1.libs.vtysh.ConfigInterface('2') as ctx:
        ctx.no_routing()

    print("Step 7- Enabling interface 3 on switch 1")
    with ops1.libs.vtysh.ConfigInterface('3') as ctx:
        ctx.no_shutdown()

    print("Step 8- Disabling routing on the port 3 of switch 1")
    with ops1.libs.vtysh.ConfigInterface('3') as ctx:
        ctx.no_routing()

    print("Step 9- Enabling interface 4 on switch 1")
    with ops1.libs.vtysh.ConfigInterface('4') as ctx:
        ctx.no_shutdown()

    print("Step 10- Disabling routing on the port 4 of switch 1")
    with ops1.libs.vtysh.ConfigInterface('4') as ctx:
        ctx.no_routing()

    print("Step 11- Enabling interface 1 on switch 2")
    with ops2.libs.vtysh.ConfigInterface('1') as ctx:
        ctx.no_shutdown()

    print("Step 12- Disabling routing on the port 1 of switch 2")
    with ops2.libs.vtysh.ConfigInterface('1') as ctx:
        ctx.no_routing()

    print("Step 13- Enabling interface 2 on switch 2")
    with ops2.libs.vtysh.ConfigInterface('2') as ctx:
        ctx.no_shutdown()

    print("Step 14-  Disabling routing on the port 2 of switch 2")
    with ops2.libs.vtysh.ConfigInterface('2') as ctx:
        ctx.no_routing()

    print("Step 15- Enabling interface 3 on switch 2")
    with ops2.libs.vtysh.ConfigInterface('3') as ctx:
        ctx.no_shutdown()

    print("Step 16-  Disabling routing on the port 3 of switch 2")
    with ops2.libs.vtysh.ConfigInterface('3') as ctx:
        ctx.no_routing()

    print("Step 17- Enabling interface 4 on switch 2")
    with ops2.libs.vtysh.ConfigInterface('4') as ctx:
        ctx.no_shutdown()

    print("Step 18-  Disabling routing on the port 4 of switch 2")
    with ops2.libs.vtysh.ConfigInterface('4') as ctx:
        ctx.no_routing()

    sleep(30)

    s1p1 = ops1.ports['1']
    s1p2 = ops1.ports['2']
    s1p3 = ops1.ports['3']
    s1p4 = ops1.ports['4']
    s2p1 = ops2.ports['1']
    s2p2 = ops2.ports['2']
    s2p3 = ops2.ports['3']
    s2p4 = ops2.ports['4']

    print("Step 19- Case 1: tx and rx enabled on SW1")
    value = findneighbor(ops1, s1p1, s2p1)
    assert str(value) == s2p1
    value = findneighbor(ops2, s2p1, s1p1)
    assert str(value) == s1p1

    print("Step 20- Case 2: tx disabled on SW1")
    value = findneighbor(ops1, s1p2, s2p2)
    assert str(value) == s2p2
    value = findneighbor(ops2, s2p2, '')
    assert value is None

    print("Step 21- Case 3: rx disabled on SW1")
    value = findneighbor(ops1, s1p3, '')
    assert value is None
    value = findneighbor(ops2, s2p3, s1p3)
    assert str(value) == s1p3

    print("Step 22- Case 4:tx and rx disabled on SW1")
    value = findneighbor(ops1, s1p4, '')
    assert value is None
    value = findneighbor(ops2, s2p4, '')
    assert value is None

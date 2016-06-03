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

# from pytest import mark
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
"""


# @mark.test_id(15306)
def test_lldp_ft_wait_hold(topology):
    ops1 = topology.get('ops1')
    ops2 = topology.get('ops2')

    assert ops1 is not None
    assert ops2 is not None

    def findneighbor(dut, port, remoteport):
        value = None
        for retry in range(1, 3):
            neighborinfo = dut.libs.vtysh.show_lldp_neighbor_info(port)
            print(neighborinfo)
            print(neighborinfo['neighbor_port_id'])
            if str(neighborinfo['neighbor_port_id']) == str(remoteport):
                value = neighborinfo['neighbor_port_id']
                break
            else:
                cmd1 = ops1("ovs-vsctl list interface 1 | grep"
                            " lldp_neighbor_info", shell="bash")
                print(cmd1)
                cmd2 = ops2("ip netns exec swns ifconfig", shell="bash")
                print(cmd2)
                value = neighborinfo['neighbor_port_id']
                print("Not found on retry: " + str(retry))
                sleep(10)
        return value

    print("Step 1- Configure lldp timers on interface 1 on switch 1")
    ops1("config terminal")
    ops1("interface 1")
    ops1("lldp holdtime 2")
    ops1("lldp timer 5")
    ops1("end")

    print("Step 2- Configure lldp timers on interface 2 on switch 2")
    ops2("config terminal")
    ops2("interface 1")
    ops2("lldp holdtime 2")
    ops2("lldp timer 5")
    ops2("end")

    print("Step 3- Enable lldp on switch 1 and switch 2")
    with ops1.libs.vtysh.Configure() as ctx:
        ctx.lldp_enable()

    with ops2.libs.vtysh.Configure() as ctx:
        ctx.lldp_enable()

    print("Step 4- Enabling interface 1 on switch 1 and switch 2")
    with ops1.libs.vtysh.ConfigInterface('1') as ctx:
        ctx.no_shutdown()
    with ops2.libs.vtysh.ConfigInterface('1') as ctx:
        ctx.no_shutdown()

    print("Step 5- Disabling routing on the port 1 of switch 1 and switch 2")
    with ops1.libs.vtysh.ConfigInterface('1') as ctx:
        ctx.no_routing()
    with ops2.libs.vtysh.ConfigInterface('1') as ctx:
        ctx.no_routing()

    sleep(15)

    s1p1 = ops1.ports["1"]
    s2p1 = ops2.ports["1"]

    print("Step 6- Verifying neighbor info on switch 1 and switch 2")
    value = findneighbor(ops1, s1p1, s2p1)
    assert str(value) == '1'
    value = findneighbor(ops2, s2p1, s1p1)
    assert str(value) == '1'

    print("Step 7- Disabling lldp on switch 2")
    with ops2.libs.vtysh.Configure() as ctx:
        ctx.no_lldp_enable()

    sleep(15)

    print("Step 8- Verifying no neighbor on switch 1 and switch 2")
    value = findneighbor(ops1, s1p1, '')
    assert value is None
    value = findneighbor(ops2, s2p1, '')
    assert value is None

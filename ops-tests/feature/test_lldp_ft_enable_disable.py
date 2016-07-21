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
# Condition to pass -> Both duts are able to enable lldp and see each other as
#                      as neighbors. Both duts disable lldp and are capable to
#                      to unconfigure properly
#              fail -> Not able to enable lldp and see its neighbor. Not able
#                      to disable and unconfigure lldp properly
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


# @mark.test_id(15304)
def test_lldp_ft_enable_disable(topology):
    ops1 = topology.get('ops1')
    ops2 = topology.get('ops2')

    assert ops1 is not None
    assert ops2 is not None

    def findneighbor(dut, port, remoteport):
        value = None
        for retry in range(1, 3):
            neighborinfo = dut.libs.vtysh.show_lldp_neighbor_info(port)
            print("########################################")
            print(neighborinfo['neighbor_port_id'])
            print("########################################")
            if str(neighborinfo['neighbor_port_id']) == str(remoteport):
                print('Chassis capabilities available: ' +
                      neighborinfo['chassis_capabilities_available'])
                print('Chassis capabilities enabled: ' +
                      neighborinfo['chassis_capabilities_enabled'])
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

    print("Step 1- Configure lldp on switch 1")
    with ops1.libs.vtysh.Configure() as ctx:
        ctx.lldp_enable()

    print("Step 2- Enabling interface on switch 1")
    with ops1.libs.vtysh.ConfigInterface('1') as ctx:
        ctx.no_shutdown()

    print("Step 3- Disabling routing on the port of switch 1")
    with ops1.libs.vtysh.ConfigInterface('1') as ctx:
        ctx.no_routing()

    print("Step 4- Configure lldp on switch 2")
    with ops2.libs.vtysh.Configure() as ctx:
        ctx.lldp_enable()

    print("Step 5- Enabling interface on switch 2")
    with ops2.libs.vtysh.ConfigInterface('1') as ctx:
        ctx.no_shutdown()

    print("Step 6- Disabling routing on the port of switch 2")
    with ops2.libs.vtysh.ConfigInterface('1') as ctx:
        ctx.no_routing()

    sleep(30)

    print("Step 7- Verify neighbor info on switch 1")
    value = findneighbor(ops1, ops1.ports['1'], ops2.ports['1'])
    # Modifying Hardcoded interface 1 to use dynamic assignation
    assert str(value) == '{}'.format(ops2.ports['1'])

    print("Step 8- Verify neighbor info on switch 2")
    value = findneighbor(ops2, ops2.ports['1'], ops1.ports['1'])
    # Modifying Hardcoded interface 1 to use dynamic assignation
    assert str(value) == '{}'.format(ops1.ports['1'])

    print("Step 9- Disabling lldp on both switches")
    with ops1.libs.vtysh.Configure() as ctx:
        ctx.no_lldp_enable()
    with ops2.libs.vtysh.Configure() as ctx:
        ctx.no_lldp_enable()

    sleep(15)

    print("Step 10- Verify no neighbor info on switch 1")
    value = findneighbor(ops1, ops1.ports['1'], '')
    assert value is None

    print("Step 11- Verify no neighbor info on switch 2")
    value = findneighbor(ops2, ops2.ports['1'], '')
    assert value is None

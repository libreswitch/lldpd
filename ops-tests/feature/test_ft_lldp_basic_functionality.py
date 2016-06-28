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
###############################################################################
#
# No description or steps defined on original test case.
# Ported by:   Vigneshkumar Kirubanandham.
# Condition to pass -> LLDP functionality should work fine
#             fail -> LLDP functionality not working
###############################################################################

"""
OpenSwitch Test for LLDP Basic functionality.
"""

from pytest import mark
from time import sleep

TOPOLOGY = """
# +-------+     +--------+
# |       |<--->|        |
# |       |<--->|        |
# |       |<--->|        |
# |  ops1 |<--->|  ops2  |
# +-------+     +--------+


# Nodes
[type=openswitch name="OpenSwitch 1"] ops1
[type=openswitch name="OpenSwitch 2"] ops2

# Links
ops1:a1 -- ops2:b1
ops1:a2 -- ops2:b2
ops1:a3 -- ops2:b3
ops1:a4 -- ops2:b4
"""


def findneighbor(dut, port, remoteport):
    value = None
    neighborinfo = dut.libs.vtysh.show_lldp_neighbor_info(port)
    if str(neighborinfo['neighbor_port_id']) == str(remoteport):
        value = neighborinfo['neighbor_port_id']
    return value


def lldp_no_tx(ops, interface):
    with ops.libs.vtysh.ConfigInterface(interface) as ctx:
        ctx.no_lldp_transmit()


def lldp_no_rx(ops, interface):
    with ops.libs.vtysh.ConfigInterface(interface) as ctx:
        ctx.no_lldp_receive()


def config_switch(ops, interface):
    with ops.libs.vtysh.ConfigInterface(interface) as ctx:
        ctx.no_routing()
        ctx.no_shutdown()


def lldp_enable(ops):
    with ops.libs.vtysh.Configure() as ctx:
        ctx.lldp_enable()


def lldp_disable(ops):
    with ops.libs.vtysh.Configure() as ctx:
        ctx.no_lldp_enable()


@mark.test_id(15305)
def test_dummy_lldp_ft_basic_functionality(topology):
    ops1 = topology.get('ops1')
    ops2 = topology.get('ops2')

    assert ops1 is not None
    assert ops2 is not None
    a1 = ops1.ports['a1']
    a2 = ops1.ports['a2']
    a3 = ops1.ports['a3']
    a4 = ops1.ports['a4']
    b1 = ops2.ports['b1']
    b2 = ops2.ports['b2']
    b3 = ops2.ports['b3']
    b4 = ops2.ports['b4']

    print("Step 1- Configure lldp on switch 1")
    lldp_enable(ops1)
    print("Step 2- Configure lldp on switch 2")
    lldp_enable(ops2)
    # Verify tx, rx behavior ( enabling disabling tx, rx )
    print("Step 3- Configure lldp tx, rx in switch 1 interfaces")
    lldp_no_tx(ops1, a2)
    lldp_no_rx(ops1, a3)
    lldp_no_tx(ops1, a4)
    lldp_no_rx(ops1, a4)

    print("Step 4- config switch interfaces")
    config_switch(ops1, a1)
    config_switch(ops1, a2)
    config_switch(ops1, a3)
    config_switch(ops1, a4)
    config_switch(ops2, b1)
    config_switch(ops2, b2)
    config_switch(ops2, b3)
    config_switch(ops2, b4)
    sleep(50)

    print("Step 5- Case 1: tx and rx enabled on SW1")
    value = findneighbor(ops1, a1, b1)
    assert str(value) == b1
    value = findneighbor(ops2, b1, a1)
    assert str(value) == a1

    print("Step 6- Case 2: tx disabled on SW1")
    value = findneighbor(ops1, a2, b2)
    assert str(value) == b2
    value = findneighbor(ops2, b2, '')
    assert str(value) == 'None'

    print("Step 7- Case 3: rx disabled on SW1")
    value = findneighbor(ops1, a3, '')
    assert str(value) == 'None'
    value = findneighbor(ops2, b3, a3)
    assert str(value) == a3

    print("Step 8- Case 4:tx and rx disabled on SW1")
    value = findneighbor(ops1, a4, '')
    assert str(value) == 'None'
    value = findneighbor(ops2, b4, '')
    assert str(value) == 'None'

    # Disable LLDP feature on both switches and verify the behavior :

    print("Step 9 - Disabling lldp on both switches")
    lldp_disable(ops1)
    lldp_disable(ops2)
    sleep(15)

    print("Step 10- Verify no neighbor info on switch 1")
    value = findneighbor(ops1, a1, '')
    assert str(value) == 'None'

    print("Step 11- Verify no neighbor info on switch 2")
    value = findneighbor(ops2, a1, '')
    assert str(value) == 'None'

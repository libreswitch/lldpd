# Licensed under the Apache License, Version 2.0 (the "License"); you may
# not use this file except in compliance with the License. You may obtain
# a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License
# Copyright (C) 2016 Hewlett Packard Enterprise Development LP
# All Rights Reserved.

from time import sleep

TOPOLOGY = """
#
# +-------+
# |       |     +-------+     +-------+
# |  hs1  <----->  sw2  <----->  sw1  |
# |       |     +-------+     +-------+
# +-------+
#

# Nodes
[type=openswitch name="Switch 1"] sw1
[type=openswitch name="Switch 2"] sw2
[type=host name="host 1"] h1

# Links
sw1:if01 -- sw2:if01
sw2:if02 -- h1:if01
"""


def test_lldp_tlv(topology, step):
    sw1 = topology.get('sw1')
    sw2 = topology.get('sw2')
    h1 = topology.get('h1')

    assert sw1 is not None
    assert sw2 is not None
    assert h1 is not None

    sw1p1 = sw1.ports['if01']
    sw2p1 = sw2.ports['if01']
    sw2p2 = sw2.ports['if02']

    # Configuring no routing on interface
    # Entering interface
    print("Switch 1 interface is : {sw1p1}".format(**locals()))
    with sw1.libs.vtysh.ConfigInterface(sw1p1) as ctx:
        ctx.no_routing()

    # Configuring no routing on interface
    # Entering interface 1
    print("Switch 2 interface is : {sw2p1}".format(**locals()))
    with sw2.libs.vtysh.ConfigInterface(sw2p1) as ctx:
        ctx.no_routing()

    # Setting tx time to 5 sec on SW1 and SW2
    # Entering confi terminal SW1
    sw1("configure terminal")

    # Setting tx time to 5 seconds on SW1
    print("\nConfiguring transmit time of 5 sec on SW1")
    # FIXME
    sw1("lldp timer 5")
    sw1("end")

    # Entering config terminal SW2
    sw2("configure terminal")

    # Setting tx time to 5 seconds on SW2
    print("\nConfiguring transmit time of 5 sec on SW2")
    # FIXME
    sw2("lldp timer 5")
    sw2("end")

    # Configuring lldp on SW1
    print("\n\n\nConfig lldp on SW1")
    with sw1.libs.vtysh.Configure() as ctx:
        ctx.lldp_enable()

    # Enabling interface 1 SW1
    print("Enabling interface on SW1")
    with sw1.libs.vtysh.ConfigInterface(sw1p1) as ctx:
            ctx.no_shutdown()

    # Configuring lldp on SW2
    print("\n\n\nConfig lldp on SW2")
    with sw2.libs.vtysh.Configure() as ctx:
        ctx.lldp_enable()

    # Enabling interface 1 SW2
    print("Enabling interface on SW2")
    with sw2.libs.vtysh.ConfigInterface(sw2p1) as ctx:
        ctx.no_shutdown()

    # Waiting for neighbour to advertise
    sleep(25)
    # Set my default context to linux temporarily
    # device1.setDefaultContext(context="linux")
    # Start loop
    for retry in range(1, 3):
        # Parsing neighbour info for SW1
        print("\nShowing Lldp neighbourship on SW1")
        out = sw1.libs.vtysh.show_lldp_neighbor_info(sw1p1)

        print("CLI_Switch1")
        print("\nExpected Neighbor Port ID: " +
              str(out["neighbor_port_id"]).rstrip())
        nei_port_id = str(out["neighbor_port_id"]).rstrip()
        if nei_port_id.isdigit() is True:
            break
        else:
            # Dump out the ovs-vsctl interface information
            print("Didn't receive integer value for "
                  "Neightbor_portID, dumping ovs-vsctl interface stats...")
            dev_cmd = "ovs-vsctl list interface " + sw1p1 + \
                      " | grep lldp_neighbor_info"
            neighbor_output = sw1(dev_cmd.format(**locals()), shell="bash")
            print("ovs-vsctl list interface output:\n" + str(neighbor_output))
            dev_cmd = "ip netns exec swns ifconfig " + sw1p1
            ifconfig_output = sw1(dev_cmd.format(**locals()), shell="bash")
            print(dev_cmd + " output\n" + str(ifconfig_output))
            sleep(10)
    # end loop
    # Set my default context to linux temporarily
    # device1.setDefaultContext(context="vtyShell")
    assert out["neighbor_port_id"] == 1
    assert "null" not in out['neighbor_chassis_name']
    assert "null" not in out['neighbor_chassis_description']
    assert "Bridge, Router" in out['chassis_capabilities_available']
    if out['neighbor_port_id']:
        print("\nCase Passed, Neighborship established by SW1")
        print("\nNeighbor Chassis-Name :" + str(out['neighbor_chassis_name']))
        print("\nNeighbor Chassis-Description :" +
              str(out['neighbor_chassis_description']))
        print("\nChassie Capabilities available : " +
              str(out['chassis_capabilities_available']))
        print("\nChassis Capabilities Enabled : " +
              str(out['chassis_capabilities_enabled']))

    # Parsing neighbor info for SW2
    # Loop for switch 2
    # device2.setDefaultContext(context="linux")
    for retry in range(1, 3):
        print("\nShowing Lldp neighborship on SW2")
        out = sw2.libs.vtysh.show_lldp_neighbor_info(sw2p1)

        print("\nExpected Neighbor Port ID: " + str(out['neighbor_port_id']))
        nei_port_id = str(out['neighbor_port_id']).rstrip()
        if nei_port_id.isdigit() is True:
            break
        else:
            # Dump out the ovs-vsctl interface information
            print("Didn't receive integer value for "
                  "Neightbor_portID, dumping ovs-vsctl interface stats...")
            dev_cmd = "ovs-vsctl list interface " + sw2p1 + \
                      " | grep lldp_neighbor_info"
            neighbor_output = sw2(dev_cmd.format(**locals()), shell="bash")
            print("ovs-vsctl list interface output:\n" + str(neighbor_output))
            dev_cmd = "ip netns exec swns ifconfig " + sw2p1
            ifconfig_output = sw2(dev_cmd.format(**locals()), shell="bash")
            print(dev_cmd + " output\n" + str(ifconfig_output))
            sleep(10)
    # device2.setDefaultContext(context="vtyShell")
    assert int(str(out['neighbor_port_id']).rstrip()) == 1
    assert "null" not in out['neighbor_chassis_name']
    assert "null" not in out['neighbor_chassis_description']
    assert "Bridge, Router" in out['chassis_capabilities_available']
    if out['neighbor_port_id']:
        print("\nCase Passed, Neighborship established by SW2")
        print("\nNeighbor Chassis-Name :" + str(out['neighbor_chassis_name']))
        print("\nNeighbor Chassis-Description :" +
              str(out['neighbor_chassis_description']))
        print("\nChassie Capablities available : " +
              str(out['chassis_capabilities_available']))
        print("\nChassis Capabilities Enabled : " +
              str(out['chassis_capabilities_enabled']))

    # Disabling chassie name for neighbor
    # Entering confi terminal SW1
    sw1("configure terminal")

    # Disabling system-name on SW1
    print("\nDisabling system-name for SW1")
    sw1("no lldp select-tlv system-name")

    # Checking SW2 to see if system-name is removed
    # device2.setDefaultContext(context="linux")
    for retry in range(1, 3):
        print("\n\nCase 1:System-Name Disabled")
        out = sw2.libs.vtysh.show_lldp_neighbor_info(sw2p1)

        print("CLI_Switch2")
        if out['neighbor_chassis_name'] == "":
            break
        else:
            # Dump out the ovs-vsctl interface information
            print("Didn't clear Neighbor Chassis-Name")
            dev_cmd = "ovs-vsctl list interface " + sw2p1 + \
                      " | grep lldp_neighbor_info"
            neighbor_output = sw2(dev_cmd.format(**locals()), shell="bash")
            print("ovs-vsctl list interface output:\n" + str(neighbor_output))
            dev_cmd = "ip netns exec swns ifconfig " + sw2p1
            ifconfig_output = sw2(dev_cmd.format(**locals()), shell="bash")
            print(dev_cmd + " output\n" + str(ifconfig_output))
            sleep(10)
    # device2.setDefaultContext(context="vtyShell")
    assert out['neighbor_chassis_name'] is None
    print("#Case Passed,No neighbor Chassis-Name present#")

    # Enabling System-Name
    print("\nEnabling System-Name for SW1")
    sw1("lldp select-tlv system-name")

    # Checking SW2 to see if system-name is reset
    print("\n\nCase 2 :System-Name Enabled")
    # Parsing lldp neighbour info SW2
    # device2.setDefaultContext(context="linux")
    for retry in range(1, 3):
        out = sw2.libs.vtysh.show_lldp_neighbor_info(sw2p1)

        print("CLI_Switch2")
        if "null" not in out['neighbor_chassis_name']:
            break
        else:
            # Dump out the ovs-vsctl interface information
            print("Didn't receive Chassis-Name")
            dev_cmd = "ovs-vsctl list interface " + sw2p1 + \
                      " | grep lldp_neighbor_info"
            neighbor_output = sw2(dev_cmd.format(**locals()), shell="bash")
            print("ovs-vsctl list interface output:\n" + str(neighbor_output))
            dev_cmd = "ip netns exec swns ifconfig " + sw2p1
            ifconfig_output = sw2(dev_cmd.format(**locals()), shell="bash")
            print(dev_cmd + " output\n" + str(ifconfig_output))
            sleep(10)
    # device2.setDefaultContext(context="vtyShell")
    assert "null" not in out['neighbor_chassis_name']
    print("#Case Passed,Neighbor Chassis-Name is present#")

    # Disabling Neighbor Chassis-Description
    print("\nDisabling System-Description for SW1")
    sw1("no lldp select-tlv system-description")

    # Checking SW2 to see if system-description is removed
    print("\n\nCase 3: System-Description Disabled")
    # Parsing lldp neighbor info SW2
    # device2.setDefaultContext(context="linux")
    for retry in range(1, 3):
        out = sw2.libs.vtysh.show_lldp_neighbor_info(sw2p1)

        print("CLI_Switch2")
        if out['neighbor_chassis_description'] == "":
            break
        else:
            # Dump out the ovs-vsctl interface information
            print("Didn't receive System-Description")
            dev_cmd = "ovs-vsctl list interface " + sw2p1 + \
                      " | grep lldp_neighbor_info"
            neighbor_output = sw2(dev_cmd.format(**locals()), shell="bash")
            print("ovs-vsctl list interface output:\n" + str(neighbor_output))
            dev_cmd = "ip netns exec swns ifconfig " + sw2p1
            ifconfig_output = sw2(dev_cmd.format(**locals()), shell="bash")
            print(dev_cmd + " output\n" + str(ifconfig_output))
            sleep(10)
    # device2.setDefaultContext(context="vtyShell")

    assert out['neighbor_chassis_description'] is None
    print("#Case Passed,No neighbor Chassis-Description present#")

    # Enabling System-Description
    print("\nEnabling System-description for SW1")
    sw1("lldp select-tlv system-description")

    # Checking SW2 to see if system-name is reset
    print("\n\nCase 4: System-Description Enabled")
    # Parsing lldp neighbour info SW2
    # device2.setDefaultContext(context="linux")
    for retry in range(1, 3):
        out = sw2.libs.vtysh.show_lldp_neighbor_info(sw2p1)

        print("CLI_Switch2")
        if "null" not in out['neighbor_chassis_description']:
            break
        else:
            # Dump out the ovs-vsctl interface information
            print("Didn't receive System-Description")
            dev_cmd = "ovs-vsctl list interface " + sw2p1 + \
                      " | grep lldp_neighbor_info"
            neighbor_output = sw2(dev_cmd.format(**locals()), shell="bash")
            print("ovs-vsctl list interface output:\n" + str(neighbor_output))
            dev_cmd = "ip netns exec swns ifconfig " + sw2p1
            ifconfig_output = sw2(dev_cmd.format(**locals()), shell="bash")
            print(dev_cmd + " output\n" + str(ifconfig_output))
            sleep(10)
    # device2.setDefaultContext(context="vtyShell")
    assert "null" not in out['neighbor_chassis_description']
    print("#Case Passed,Neighbor Chassis-Description is present#")

    # Disabling System-Capabilities
    print("\nDisabling Neighor Chassis-Capabilities for SW1")
    sw1("no lldp select-tlv system-capabilities")

    # Checking SW2 to see if system-Capabilities is removed
    print("\n\nCase 5: System-Capabilities disabled")
    # device2.setDefaultContext(context="linux")
    for retry in range(1, 3):
        out = sw2.libs.vtysh.show_lldp_neighbor_info(sw2p1)

        print("CLI_Switch2")
        if out['chassis_capabilities_available'] == "" and \
           out['chassis_capabilities_enabled'] == "":
            break
        else:
            # Dump out the ovs-vsctl interface information
            print("Didn't receive System-Capabilities")
            dev_cmd = "ovs-vsctl list interface " + sw2p1 + \
                      " | grep lldp_neighbor_info"
            neighbor_output = sw2(dev_cmd.format(**locals()), shell="bash")
            print("ovs-vsctl list interface output:\n" + str(neighbor_output))
            dev_cmd = "ip netns exec swns ifconfig " + sw2p1
            ifconfig_output = sw2(dev_cmd.format(**locals()), shell="bash")
            print(dev_cmd + " output\n" + str(ifconfig_output))
            sleep(10)
    # device2.setDefaultContext(context="vtyShell")
    assert out['chassis_capabilities_available'] is None
    print("#Case Passed,No neighbor Chassis-capablities available#")
    assert out['chassis_capabilities_enabled'] is None
    print("#Case Passed,No neighbor Chassis-capablities enabled#")

    # System-Capabilities Enabled
    print("\nEnabling Neighbor Chassis-Capabilities for SW1")
    sw1("lldp select-tlv system-capabilities")
    sw1("end")

    # Checking SW2 to see if system-name is reset
    print("\n\nCase 6: System-Capabilities enabled")
    # device2.setDefaultContext(context="linux")
    for retry in range(1, 3):
        out = sw2.libs.vtysh.show_lldp_neighbor_info(sw2p1)

        print("CLI_Switch2")
        if "Bridge, Router" in out['chassis_capabilities_available'] and \
           "Bridge, Router" in out['chassis_capabilities_enabled']:
            break
        else:
            # Dump out the ovs-vsctl interface information
            print("Didn't receive System-Capabilities")
            dev_cmd = "ovs-vsctl list interface " + sw2p1 + \
                      " | grep lldp_neighbor_info"
            neighbor_output = sw2(dev_cmd.format(**locals()), shell="bash")
            print("ovs-vsctl list interface output:\n" + str(neighbor_output))
            dev_cmd = "ip netns exec swns ifconfig " + sw2p1
            ifconfig_output = sw2(dev_cmd.format(**locals()), shell="bash")
            print(dev_cmd + " output\n" + str(ifconfig_output))
            sleep(10)
    # device2.setDefaultContext(context="vtyShell")
    assert "Bridge, Router" in out['chassis_capabilities_available']
    print("#Case Passed,Neighbor Chassis-capablities available#")
    assert "Bridge, Router" in out['chassis_capabilities_enabled']
    print("#Case Passed,Neighbor Chassis-capablities enabled#")

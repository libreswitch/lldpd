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
OpenSwitch Test for lldp related configurations.
"""

from pytest import mark
from time import sleep

TOPOLOGY = """
# +-------+  +------+
# |  ops1 <--> ops2 |
# +-------+  +------+

# Nodes
[type=openswitch name="OpenSwitch 1"] ops1
[type=openswitch name="OpenSwitch 2"] ops2

# Links
ops1:if01 -- ops2:if01
"""


# Test case Variables
TIMEOUT = '0'


def session_timeout(switch, timeout, step):
    step('Configure session timeout on DUTS')
    # Session timeout configuration on DUTs
    with switch.libs.vtysh.Configure() as ctx:
        ctx.session_timeout(timeout)


def configure_lldp(dut, number, step):
    step("Step 1- Configure lldp on {} interface {}".format(dut, number))
    pid1 = dut("pidof ops-lldpd", shell="bash").strip()
    assert pid1 != ""

    uuid = dut("list system | grep _uuid | awk '{print $3}'",
               shell="vsctl").strip()
    print("Got switch uuid - {uuid}.".format(uuid=uuid))

    dut("-t 60 set system {uuid} other_config:lldp_enable=true "
        "other_config:lldp_mgmt_addr=204.152.189.{number} "
        "other_config:lldp_tx_interval=5".format(
            uuid=uuid, number=number), shell="vsctl")

    sleep(1)
    out = dut("list system | grep other_config", shell="vsctl")
    assert "lldp_enable=\"true\"" in out
    return pid1


def bring_interface_up(dut, int):
    with dut.libs.vtysh.ConfigInterface(int) as ctx:
        ctx.no_routing()

    port = dut.ports[int]
    dut("-t 60 set interface {port} user_config:admin=up "
        "user_config:autoneg=on".format(port=port), shell="vsctl")

    sleep(1)
    result = dut("list interface {port} | grep link_state | "
                 "grep -v grep".format(port=port), shell="vsctl")
    assert 'up' in result


def verify_lldp_rx_count_after_crash(dut, int):
    port = dut.ports[int]
    rxc_before_crash = get_lldp_rx_count(dut, port)
    dut("killp ops-lldpd", shell="bash")
    sleep(2)
    rxc_after_crash = get_lldp_rx_count(dut, port)
    assert rxc_after_crash >= rxc_before_crash


def get_lldp_rx_count(dut, port):
    out = dut("list interface {port} | grep lldp_statistics".format(
        **locals()
    ), shell="vsctl")
    for raw in out.split():
        if raw.startswith('lldp_rx'):
            break
    raw = raw.replace('lldp_rx=', '')
    raw = raw.replace(',', '')
    lldp_rx_int_value = int(raw)
    return lldp_rx_int_value


@mark.timeout(1000)
def test_lldpd_ct_counters_recovery(topology, step):
    ops1 = topology.get('ops1')
    ops2 = topology.get('ops2')

    assert ops1 is not None
    assert ops2 is not None

    dut_1_int1 = ops1.ports['if01']
    dut_2_int1 = ops2.ports['if01']

    # Configure session timeout on DUTs
    session_timeout(ops1, TIMEOUT, step)
    session_timeout(ops2, TIMEOUT, step)

    # step("Step 1- Configure lldp on switch 1")
    pid_start1 = configure_lldp(ops1, dut_1_int1, step)

    # step("Step 2- Configure lldp on switch 2")
    pid_start2 = configure_lldp(ops2, dut_2_int1, step)

    step("Step 3- Bring interface up on switch 1")
    bring_interface_up(ops1, 'if01')

    step("Step 4- Bring interface up on switch 2")
    bring_interface_up(ops2, 'if01')

    step("Step 5- Wait for lldp packets exchange")
    for i in range(30):
        sleep(1)
        pid1 = ops1("pidof ops-lldpd", shell="bash").strip()
        pid2 = ops2("pidof ops-lldpd", shell="bash").strip()

        assert pid_start1 == pid1 and pid_start2 == pid2

    step("Step 6- Verify lldp rx count after crash on switch 1")
    verify_lldp_rx_count_after_crash(ops1, 'if01')

    step("Step 7- Verify lldp rx count after crash on switch 2")
    verify_lldp_rx_count_after_crash(ops2, 'if01')

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

# from pytest import mark
from time import sleep

TOPOLOGY = """
# +-------+
# |  ops1 |
# +-------+

# Nodes
[type=openswitch name="OpenSwitch 1"] ops1

# Links
# ops1:if01 -- ops2:if01
"""


def test_lldpd_ct_call_libevent(topology, step):
    ops1 = topology.get('ops1')

    assert ops1 is not None

    step("Step 1- Enabling LLDP globally for switch")
    pid1 = ops1("pidof ops-lldpd", shell="bash").strip()
    assert pid1 != ""

    uuid = ops1("list system | grep _uuid | awk '{print $3}'",
                shell="vsctl").strip()
    print("Got switch uuid - {uuid}.".format(uuid=uuid))

    step("Step 2- Enable lldp on switch")
    ops1("-t 30 set system {uuid} other_config:lldp_enable=true "
         "other_config:lldp_tx_interval=5".format(uuid=uuid), shell="vsctl")

    step("Step 3- Wakeup and verify lldp enable flag")
    out = ops1("list system | grep other_config", shell="vsctl")
    assert "lldp_enable=\"true\"" in out
    print("Lldp configured correctly on switch.")

    step("Step 4- Test lldpd burst of libevents")
    pid_start = ops1("pidof ops-lldpd", shell="bash").strip()
    assert pid_start != ""
    out = ops1("ovs-appctl -t ops-lldpd lldpd/test libevent 0", shell="bash")
    assert "OK" in out
    sleep(2)
    pid_end = ops1("pidof ops-lldpd", shell="bash").strip()
    assert pid_end != ""
    out = ops1("ovs-appctl -t ops-lldpd lldpd/test libevent 1", shell="bash")
    assert "OK" in out

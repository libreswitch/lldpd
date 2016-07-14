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
OpenSwitch Test for lldp related SNMP operations.
"""

from pytest import mark
from time import sleep


TOPOLOGY = """
# +-------+
# |       |     +--------+
# |  hs1  <----->  ops1  |
# |       |     +--------+
# +-------+

# Nodes
[type=openswitch name="OpenSwitch 1"] ops1
[type=host name="Host 1"] hs1

# Ports
[force_name=oobm] ops1:sp1

# Links
hs1:1 -- ops1:sp1
"""


def snmpbulkget(ops1, version, accesscontrol, host, oid, extrav3conf=None):
    if version is "v2c":
        retstruct = ops1("snmpbulkget -v2c -c" + accesscontrol + " "
                         "" + host + " "
                         "" + oid + " ", shell='bash')
        return retstruct
    elif version is "v3":
        retstruct = ops1("snmpbulkget -v3 " + extrav3conf + ""
                         " " + host + " " + oid + " ", shell='bash')
        return retstruct


def snmpbulkget_v2c_test_local(ops1):
    retstruct = snmpbulkget(ops1, "v2c", "public", "localhost", ""
                            ".1.0.8802.1.1.2.1.1.1.0")
    resultget_split = retstruct.splitlines()
    count = 0
    for line in resultget_split:
        if "iso.0.8802.1.1.2.1.1" in line:
            count += 1
    assert count is 10, "snmpbulkget v2 failed to 10 entries from the OVSDB"

    for line in resultget_split:
        assert "Error in packet" not in line, "General failure accured in"
        " snmpwalk"

    for line in resultget_split:
        assert "Timeout: No Response" not in line, "snmpbulkget timed out"


def snmpbulkget_v3_test_local(ops1):
    retstruct = snmpbulkget(ops1, "v3", "None", "localhost", ""
                            ".1.0.8802.1.1.2.1.1.1.0",
                            "-u testv3user -l authNoPriv -a md5 -A password")
    resultget_split = retstruct.splitlines()

    count = 0
    for line in resultget_split:
        if "iso.0.8802.1.1.2.1.1" in line:
            count += 1
    assert count is 10, "snmpbulkget v3 failed to get default from the OVSDB"

    for line in resultget_split:
        assert "Error in packet" not in line, "General failure accured in"
        " snmpwalk"

    for line in resultget_split:
        assert "Timeout: No Response" not in line, "snmpbulkget timed out"


def config(ops1, hs1):

    # Configure auth user - testv3user
    with ops1.libs.vtysh.Configure() as ctx:
        ctx.snmpv3_user_auth_auth_pass('testv3user', auth_protocol='md5',
                                       auth_password='password')
    result = ops1.libs.vtysh.show_snmpv3_users()
    assert 'testv3user' in result
    assert result['testv3user']['AuthMode'] == 'md5'

    # Configure static IP for management I/F
    with ops1.libs.vtysh.ConfigInterfaceMgmt() as ctx:
        ctx.ip_static("10.10.10.4/24")
    ops1.libs.vtysh.show_running_config()
    # Configure host interfaces
    hs1.libs.ip.interface('1', addr='10.10.10.5/24', up=True)

    sleep(3)

    ping = hs1.libs.ping.ping(1, '10.10.10.4')
    assert ping['transmitted'] == ping['received'] == 1


def unconfig(ops1, hs1):
    # Configure auth user - testv3user
    with ops1.libs.vtysh.Configure() as ctx:
        ctx.no_snmpv3_user_auth_auth_pass('testv3user', auth_protocol='md5',
                                          auth_password='password')


def snmpbulkget_v2c_test_remote(hs1):

    retstruct = hs1("snmpbulkget -v2c -cpublic 10.10.10.4:161"
                    " .1.0.8802.1.1.2.1.1.1.0")

    resultget_split = retstruct.splitlines()

    count = 0
    for line in resultget_split:
        if "iso.0.8802.1.1.2.1.1" in line:
            count += 1
    assert count is 10, "snmpbulkget v2c failed from remote host"

    for line in resultget_split:
        assert "Error in packet" not in line, "General failure accured"
        " in snmpwalk"

    for line in resultget_split:
        assert "Timeout: No Response" not in line, "snmpbulkget timed out"


def snmpbulkget_v3_test_remote(ops1, hs1):

    retstruct = hs1("snmpbulkget -v3 -u testv3user -l "
                    "authNoPriv -a md5 -A password "
                    "10.10.10.4:161 .1.0.8802.1.1.2.1.1.1.0")

    resultget_split = retstruct.splitlines()

    count = 0
    for line in resultget_split:
        if "iso.0.8802.1.1.2.1.1" in line:
            count += 1
    assert count is 10, "snmpbulkget v3 failed from remote host"

    for line in resultget_split:
        assert "Error in packet" not in line, "General failure accured"
        " in snmpwalk"

    for line in resultget_split:
        assert "Timeout: No Response" not in line, "snmpbulkget timed out"


@mark.platform_incompatible(['docker'])
def test_snmpbulkget_ft_lldp(topology, step):
    ops1 = topology.get("ops1")
    hs1 = topology.get("hs1")

    config(ops1, hs1)
    sleep(30)
    snmpbulkget_v2c_test_local(ops1)
    snmpbulkget_v3_test_local(ops1)
    snmpbulkget_v2c_test_remote(hs1)
    snmpbulkget_v3_test_remote(ops1, hs1)
    unconfig(ops1, hs1)

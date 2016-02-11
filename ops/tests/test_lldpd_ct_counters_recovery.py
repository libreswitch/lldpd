#!/usr/bin/python
#
# Copyright (C) 2015 Hewlett Packard Enterprise Development LP
# All Rights Reserved.
#
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
# under the License.

import os
import sys
import time
import pytest
import subprocess

from opsvsi.docker import *
from opsvsi.opsvsitest import *
from opsvsiutils.systemutil import *

#
#
# For this test, we only need 2 back to back connected switches
# which start exchanging lldp messages.  We do not need the
# hosts. BUT.... note that since the host links will
# not be configured now, the switch links will use interface
# 1 instead of interface 2, since addLink assigns interfaces
# one at a time.
#
# S1 [interface 1]<--->[interface 1] S2
#
# The purpose of this test is to make sure that after an
# lldp daemon crash, when lldp recovers, it does not lose
# its interface counters/statistics and updates them from
# the ovs db such that it "seems" like it never crashed.
#


class myTopo(Topo):

    def build(self, hsts=0, sws=2, **_opts):

        self.hsts = hsts
        self.sws = sws

        # Add switches
        for s in irange(1, sws):
            switch = self.addSwitch('s%s' % s)

        # connect them together
        self.addLink('s1', 's2')


class lldpTest (OpsVsiTest):

    def setupNet(self):

        # if you override this function, make sure to
        # either pass getNodeOpts() into hopts/sopts of the topology that
        # you build or into addHost/addSwitch calls
        self.net = Mininet(topo=myTopo(hsts=0, sws=2,
                                       hopts=self.getHostOpts(),
                                       sopts=self.getSwitchOpts()),
                           switch=VsiOpenSwitch,
                           host=Host,
                           link=OpsVsiLink,
                           controller=None,
                           build=True)

    def switch_variable(self, switch_number):
        if (switch_number == 1):
            return self.net.switches[0]
        else:
            return self.net.switches[1]

    def show_interface(self, switch_number, interface_number):
        s = self.switch_variable(switch_number)
        command = "ovs-vsctl list interface " + str(interface_number)
        out = "\n\n### " + s.cmd(command) + " ###\n"
        info(out)

    def is_interface_down(self, switch_number, interface_number):
        info("### checking link state for switch %d interface %d: ###\n"
             % (switch_number, interface_number))
        command = "ovs-vsctl list interface " + str(interface_number) + \
                  " | grep link_state | grep -v grep"
        s = self.switch_variable(switch_number)
        result = s.cmd(command)
        if ('up' in result):
            info("### UP ###\n")
            return 0
        else:
            info("### DOWN ###\n")
            return 1

    def bring_interface_up(self, switch_number, interface_number):
        info("\n### bringing interface %d up for switch %d ###\n"
             % (interface_number, switch_number))
        s = self.switch_variable(switch_number)
        # Configure interface on switch as no routing else the interface
        # will use the port admin logic to set its own hw_intf_config state
        s.cmdCLI("configure terminal")
        s.cmdCLI("interface " + str(interface_number))
        s.cmdCLI("no routing")
        s.cmdCLI("exit")
        # set admin state & autoneg
        command = "ovs-vsctl -t 60 set interface " + str(interface_number) + \
                  " user_config:admin=up user_config:autoneg=on"
        s.cmd(command)
        # set connector
        command = "ovs-vsctl -t 60 set interface " + str(interface_number) + \
            " pm_info:connector=SFP_RJ45 " + \
            "pm_info:connector_status=supported"
        s.cmd(command)
        output = "switch " + str(switch_number) + " interface " + \
                 str(interface_number)
        time.sleep(1)
        assert self.is_interface_down(switch_number, interface_number) == 0, \
            output + " did NOT come up\n"
        output = "### " + output + "is now successfully up ###\n"
        info(output)

    def lldp_process_restarted(self, switch_number):
        s = self.switch_variable(switch_number)
        pid = s.cmd("pidof ops-lldpd").strip()
        if (switch_number == 1):
            if (pid == self.pid1):
                return 0, pid
            else:
                return 1, pid
        else:
            if (pid == self.pid2):
                return 0, pid
            else:
                return 1, pid

    def configure_lldp(self, switch_number):
        info("\n### enabling lldp globally for switch %d ###\n"
             % switch_number)
        s = self.switch_variable(switch_number)
        if (switch_number == 1):
            self.pid1 = s.cmd("pidof ops-lldpd").strip()
            assert self.pid1 != "", "lldp process not running on switch " + \
                str(switch_number)
            info("### lldp process id on switch " +
                 str(switch_number) +
                 " is " +
                 str(self.pid1) + " ###\n")
        else:
            self.pid2 = s.cmd("pidof ops-lldpd").strip()
            assert self.pid2 != "", "lldp process not running on switch " + \
                str(switch_number)
            info("### lldp process id on switch " +
                 str(switch_number) +
                 " is " +
                 str(self.pid2) + " ###\n")
        uuid = s.ovscmd("ovs-vsctl list system | grep _uuid | " +
                     "awk '{print $3}'").strip()
        s.ovscmd("ovs-vsctl -t 60 set system %s "
              "other_config:lldp_enable=true "
              "other_config:lldp_mgmt_addr=204.152.189.%d "
              "other_config:lldp_tx_interval=5" % (uuid, switch_number))
        time.sleep(1)
        out = s.ovscmd("ovs-vsctl list system | grep other_config")
        assert 'lldp_enable=true' in out, "lldp not enabled on switch " + \
            str(switch_number)
        info("### lldp configured correctly on switch %d ###\n"
             % switch_number)

    #
    # get the numerical value of 'lldp_rx' statistic value
    #
    def get_lldp_rx_count(self, switch_number, interface_number):
        s = self.switch_variable(switch_number)
        # extract numerical value of (N) from string "lldp_rx=N,"
        command = "ovs-vsctl list interface " + str(interface_number) + \
                  " | grep lldp_statistics"
        out = s.cmd(command)
        for s in out.split():
            if s.startswith('lldp_rx'):
                break
        s = s.replace('lldp_rx=', '')
        s = s.replace(',', '')
        lldp_rx_int_value = int(s)
        return lldp_rx_int_value

    def wait_for_lldp_packets_exchange(self):
        info("\n### waiting for some lldp packets to be exchanged ###\n")
        count = 0
        while (count < 30):
            time.sleep(1)
            # info(".")
            crashed, pid = self.lldp_process_restarted(1)
            assert crashed != True, "lldp crashed on switch 1, exiting"

            crashed, pid = self.lldp_process_restarted(2)
            assert crashed != True, "lldp crashed on switch 2, exiting"
            count = count + 1
    #
    # verify that rx count is > after an lldp crash & recovery
    #

    def verify_lldp_rx_count_after_crash(self,
                                         switch_number,
                                         interface_number):
        info("\n### kill lldp on switch %d and verify rx cnts on intf %d ###\n"
             % (switch_number, interface_number))
        s = self.switch_variable(switch_number)
        rxc_before_crash = self.get_lldp_rx_count(switch_number,
                                                  interface_number)
        s.cmd("killp ops-lldpd")
        time.sleep(2)
        rxc_after_crash = self.get_lldp_rx_count(switch_number,
                                                 interface_number)
        assert rxc_after_crash >= rxc_before_crash, \
            "switch %d interface %d test FAILED: before %d, after %d\n" % \
            (switch_number, interface_number,
             rxc_before_crash, rxc_after_crash)
        info("### switch %d interface %d test PASSED: before %d, after "
             "%d ###\n"
             % (switch_number, interface_number,
                rxc_before_crash, rxc_after_crash))

#
# final "Test_" class
#

class Test_lldp:

    def setup(self):
        pass

    def teardown(self):
        pass

    def setup_class(cls):
    # Create the Mininet topology based on mininet.
        Test_lldp.test_var = lldpTest()
        pass

    def teardown_class(cls):
        Test_lldp.test_var.net.stop()

    def setup_method(self, method):
        pass

    def teardown_method(self, method):
        pass

    def __del__(self):
        del self.test_var

    # the actual test function
    def test_lldp_full(self):
        info("\n########## Test lldpd counter recovery after restart "
             "##########\n")
        self.test_var.configure_lldp(1)
        self.test_var.configure_lldp(2)
        self.test_var.bring_interface_up(1, 1)
        self.test_var.bring_interface_up(2, 1)
        self.test_var.wait_for_lldp_packets_exchange()
        self.test_var.verify_lldp_rx_count_after_crash(1, 1)
        self.test_var.verify_lldp_rx_count_after_crash(2, 1)

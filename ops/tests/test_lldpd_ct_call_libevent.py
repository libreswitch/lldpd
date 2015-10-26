# !/usr/bin/python
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
# For this test, we only need only one switch
# The test send a burst of messages via libevent
# and confirms that all the messages have arrived
# The tests creates a link between two switches
# for future use
#
# S1[1]
#


class myTopo(Topo):

    def build(self, hsts=2, sws=2, **_opts):

        self.hsts = hsts
        self.sws = sws

        # Add switches
        for s in irange(1, sws):
            switch = self.addSwitch('s%s' % s)


class lldpTest (OpsVsiTest):

    def setupNet(self):

        # if you override this function, make sure to
        # either pass getNodeOpts() into hopts/sopts of the topology that
        # you build or into addHost/addSwitch calls
        self.net = Mininet(topo=myTopo(hsts=0, sws=1,
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

    def configure_lldp(self, switch_number):
        info("\n")
        info("### enabling lldp globally for switch %d ###\n" % switch_number)
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
            assert switch_number == 1, "Invalid switch " + \
                str(switch_number)
        uuid = s.ovscmd("ovs-vsctl list system | grep _uuid | " +
                     "awk '{print $3}'").strip()
        info("### got openvswitch uuid - %s ###\n" % uuid)
        s.ovscmd("ovs-vsctl -t 30 set system %s "
              "other_config:lldp_enable=true "
              "other_config:lldp_tx_interval=5" % uuid)
        info("### Enabled lldp on openvswitch ###\n")
        # time.sleep(1)
        info("### Wakeup and verify lldp enable flag ###\n")
        out = s.ovscmd("ovs-vsctl list system | grep other_config")
        assert 'lldp_enable=true' in out, "lldp not enabled on switch " + \
            str(switch_number)
        info("### lldp configured correctly on switch %d ###\n"
             % switch_number)

    #
    # Trigger a burst of events via ovs-appctl and make sure
    # it arrived by checking event counter
    #

    def lldp_libevent(self, switch_number):
        s = self.switch_variable(switch_number)
        self.pid_start = s.cmd("pidof ops-lldpd").strip()
        out = s.cmd("ovs-appctl -t ops-lldpd lldpd/test libevent 0")
        assert "OK" in out, "switch %d libevent test init FAILED: %s" \
            % (switch_number, out)
        time.sleep(2)
        self.pid_end = s.cmd("pidof ops-lldpd").strip()
        assert self.pid_start == self.pid_end, "lldpd restarted new pid %s" \
            % self.pid_end
        info("### lldp process id is " + str(self.pid1) + " ###\n")
        out = s.cmd("ovs-appctl -t ops-lldpd lldpd/test libevent 1").strip()
        assert "OK" in out, "switch %d libevent test FAILED: %s" \
            % (switch_number, out)
        info("### switch %d libevent test PASSED ###\n" % (switch_number))


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
        info("\n########## Test lldpd burst of libevents ##########\n")
        self.test_var.configure_lldp(1)
        self.test_var.lldp_libevent(1)
        # CLI(self.test_var.net)

#!/usr/bin/env python

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

"""
import lib
import pytest
import re
import common
import switch
#from lib import testEnviron
from lib import *
from switch.CLI.lldp import *
from switch.CLI.interface import *

# Topology definition
topoDict = {"topoExecution": 1000,
            "topoTarget": "dut01 dut02",
            "topoDevices": "dut01 dut02",
            "topoLinks": "lnk01:dut01:dut02,lnk02:dut01:dut02,lnk03:dut01:dut02,lnk04:dut01:dut02",
            "topoFilters": "dut01:system-category:switch,dut02:system-category:switch"}

def lldp_interface_txrx(**kwargs):
    device1 = kwargs.get('device1',None)
    device2 = kwargs.get('device2',None)
    caseReturnCode = 0

    #Defining the test steps
    common.LogOutput('info', "\n\nCase 1:\nSW 1 : lldp tx and rx enabled on link 1")
    common.LogOutput('info', "\n\nCase 2:\nSW 1 : lldp tx disabled on link 2")
    common.LogOutput('info', "\n\nCase 3:\nSW 1 : lldp rx disabled on link 3")
    common.LogOutput('info', "\n\nCase 4:\nSW 1 : lldp tx and rx disabled on link 4")


    #Entering interface for link 2 SW1, disabling tx
    retStruct = LldpInterfaceConfig(deviceObj=device1, interface=device1.linkPortMapping['lnk02'], transmission=False)
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to disable tx on SW1"
       caseReturnCode = 1

    #Entering interface for link 3 SW1, disabling rx
    retStruct = LldpInterfaceConfig(deviceObj=device1, interface=device1.linkPortMapping['lnk03'], reception=False)
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to disable rx on SW1"
       caseReturnCode = 1

    #Entering interface for link 4 SW1, disabling rx and tx
    retStruct = LldpInterfaceConfig(deviceObj=device1, interface=device1.linkPortMapping['lnk04'], transmission=False)
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to disable tx on SW1"
       caseReturnCode = 1

    retStruct = LldpInterfaceConfig(deviceObj=device1, interface=device1.linkPortMapping['lnk04'], reception=False)
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to disable rx on SW1"
       caseReturnCode = 1

    #Configuring lldp on SW1
    common.LogOutput('info', "\n\n\nConfig lldp on SW1")
    retStruct = LldpConfig(deviceObj=device1, enable=True)
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to configure LLDP on SW1"
       caseReturnCode = 1

    #Configuring lldp on SW1
    common.LogOutput('info', "\n\n\nConfig lldp on SW2")
    retStruct = LldpConfig(deviceObj=device2, enable=True)
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to configure lldp on SW2"
       caseReturnCode = 1

    #Enabling interface 1 SW1
    common.LogOutput('info', "Enabling interface on SW1")
    retStruct = InterfaceEnable(deviceObj=device1, enable=True, interface=device1.linkPortMapping['lnk01'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to enabling interafce on SW1"
       caseReturnCode = 1

    #Enabling interface 2 SW1
    common.LogOutput('info', "Enabling interface on SW1")
    retStruct = InterfaceEnable(deviceObj=device1, enable=True, interface=device1.linkPortMapping['lnk02'])
    retCode = retStruct.returnCode
    if retCode != 0:
       assert "Unable to enabling interafce on SW1"
       caseReturnCode = 1

    #Enabling interface 3 SW1
    common.LogOutput('info', "Enabling interface on SW1")
    retStruct = InterfaceEnable(deviceObj=device1, enable=True, interface=device1.linkPortMapping['lnk03'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to enabling interafce on SW1"
       caseReturnCode = 1

    #Enabling interface 4 SW1
    common.LogOutput('info', "Enabling interface on SW1")
    retStruct = InterfaceEnable(deviceObj=device1, enable=True, interface=device1.linkPortMapping['lnk04'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to enabling interafce on SW1"
       caseReturnCode = 1

    #Enabling interface 1 SW2
    common.LogOutput('info', "Enabling interface on SW2")
    retStruct = InterfaceEnable(deviceObj=device2, enable=True, interface=device2.linkPortMapping['lnk01'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to enabling interafce on SW2"
       caseReturnCode = 1

    #Enabling interface 2 SW2
    common.LogOutput('info', "Enabling interface on SW2")
    retStruct = InterfaceEnable(deviceObj=device2, enable=True, interface=device2.linkPortMapping['lnk02'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to enabling interafce on SW2"
       caseReturnCode = 1

    #Enabling interface 3 SW2
    common.LogOutput('info', "Enabling interface on SW2")
    retStruct = InterfaceEnable(deviceObj=device2, enable=True, interface=device2.linkPortMapping['lnk03'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to enabling interafce on SW2"
       caseReturnCode = 1

    #Enabling interface 4 SW2
    common.LogOutput('info', "Enabling interface on SW2")
    retStruct = InterfaceEnable(deviceObj=device2, enable=True, interface=device2.linkPortMapping['lnk04'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to enabling interafce on SW2"
       caseReturnCode = 1

    #Parsing neighbour info for SW1 and SW2
    #Case 1
    common.LogOutput('info', "\n\n\n### Case 1: tx and rx enabled on SW1 ###\n\n\n")

    common.LogOutput('info', "\nShowing Lldp neighborship by SW1 on Link 1")
    retStruct = ShowLldpNeighborInfo(deviceObj=device1, port=device1.linkPortMapping['lnk01'])
    retCode = retStruct.returnCode()
    if retCode:
        assert "\nFailed to show neighbor info"
        caseReturnCode = 1

    common.LogOutput('info', "CLI_Switch1Link1")
    retStruct.printValueString()
    lnk01PrtStats = retStruct.valueGet(key='portStats')

    if (lnk01PrtStats[device1.linkPortMapping['lnk01']]['Neighbor_portID']):
        common.LogOutput('info',"Case Passed, Neighborship established by SW1 on Link1")
        common.LogOutput('info', "\nPort of SW1 neighbor is :" + str(lnk01PrtStats[device1.linkPortMapping['lnk01']]['Neighbor_portID']))
        common.LogOutput('info',"\nChassie Capabilities available : "+str(lnk01PrtStats[device1.linkPortMapping['lnk01']]['Chassis_Capabilities_Available']))
        common.LogOutput('info', "\nChassis Capabilities Enabled : "+str(lnk01PrtStats[device1.linkPortMapping['lnk01']]['Chassis_Capabilities_Enabled']))
    else:
        common.LogOutput('info',"Case Failed, No Neighbor is present for SW1 on Link 1")
        assert "Case Failed"
        caseReturnCode = 1

    common.LogOutput('info', "\nShowing Lldp neighborship on SW2 Link 1")
    retStruct= ShowLldpNeighborInfo(deviceObj=device2, port=device2.linkPortMapping['lnk01'])
    if retCode:
        assert "\nFailed to show neighbor info"
        caseReturnCode = 1
    lnk01PrtStats = retStruct.valueGet(key='portStats')

    common.LogOutput('info', "CLI_Switch2")
    retStruct.printValueString()
    if (lnk01PrtStats[device2.linkPortMapping['lnk01']]['Neighbor_portID']):
        common.LogOutput('info',"\nCase Passed, Neighborship established by SW2 on Link 1")
        common.LogOutput('info', "\nPort of SW1 neighbor is :" + str(lnk01PrtStats[device2.linkPortMapping['lnk01']]['Neighbor_portID']))
        common.LogOutput('info',"\nChassie Capablities available : "+ str(lnk01PrtStats[device2.linkPortMapping['lnk01']]['Chassis_Capabilities_Available']))
        common.LogOutput('info', "\nChassis Capabilities Enabled : "+ str( lnk01PrtStats[device2.linkPortMapping['lnk01']]['Chassis_Capabilities_Enabled']
))
    else :

        common.LogOutput('info',"Case Failed, No Neighbor is present for SW2 on Link 1")
        assert "Case Failed"
        caseReturnCode = 1
    #Case 2

    common.LogOutput('info', "\n\n\n### Case 2: tx disabled on SW1 ###\n\n\n")

    common.LogOutput('info', "\nShowing Lldp neighborship on Link 2 for SW1")

    retStruct = ShowLldpNeighborInfo(deviceObj=device1, port=device1.linkPortMapping['lnk02'])
    retCode = retStruct.returnCode()
    if retCode:
        assert "\nFailed to show neighbor info"
        caseReturnCode = 1
    lnk02PrtStats = retStruct.valueGet(key='portStats')

    common.LogOutput('info', "CLI_Switch1")
    retStruct.printValueString()
    if (lnk02PrtStats[device1.linkPortMapping['lnk02']]['Neighbor_portID']):
        common.LogOutput('info',"Case Passed,  Neighborship established for SW1 on link 2")
        common.LogOutput('info', "\nPort of SW1 neighbor is :" + str(lnk02PrtStats[device1.linkPortMapping['lnk02']]['Neighbor_portID']))
        common.LogOutput('info',"\nChassie Capabilities available : "+str(lnk02PrtStats[device1.linkPortMapping['lnk02']]['Chassis_Capabilities_Available']))
        common.LogOutput('info', "\nChassis Capabilities Enabled : "+str(lnk02PrtStats[device1.linkPortMapping['lnk02']]['Chassis_Capabilities_Enabled']))

    else:
        common.LogOutput('info',"Case Failed, No Neighbor is present for SW1 on Link 2")
        assert "Case Failed"
        caseReturnCode = 1
    common.LogOutput('info', "\nShowing Lldp neighborship for SW2 on Link 2 ")

    retStruct = ShowLldpNeighborInfo(deviceObj=device2, port=device2.linkPortMapping['lnk02'])
    if retCode:
        assert "\nFailed to show neighbor info"
        caseReturnCode = 1
    lnk02PrtStats = retStruct.valueGet(key='portStats')


    common.LogOutput('info', "CLI_Switch2")
    retStruct.printValueString()

    if (lnk02PrtStats[device2.linkPortMapping['lnk02']]['Neighbor_portID']):
        common.LogOutput('info',"\nCase Failed, Neighborship established by SW2 on Link 2")
        common.LogOutput('info', "\nPort of SW2 neighbor is :" + str(lnk02PrtStats[device2.linkPortMapping['lnk02']]['Neighbor_portID']))
        common.LogOutput('info',"\nChassie Capablities available : "+ str(lnk02PrtStats[device2.linkPortMapping['lnk02']]['Chassis_Capabilities_Available']))
        common.LogOutput('info', "\nChassis Capabilities Enabled : "+ str( lnk02PrtStats[device2.linkPortMapping['lnk02']]['Chassis_Capabilities_Enabled']
))
        assert "Case Failed"
        caseReturnCode = 1
    else :
        common.LogOutput('info',"Case Passed ,No Neighbor is present for SW2 on Link 2")


    #Case 3


    common.LogOutput('info', "\n\n\n### Case 3: rx disabled on SW1 ###\n\n\n")

    common.LogOutput('info', "\nShowing Lldp neighborship for SW1 on Link 3")
    retStruct = ShowLldpNeighborInfo(deviceObj=device1, port=device1.linkPortMapping['lnk03'])
    retCode = retStruct.returnCode()
    if retCode:
        assert "\nFailed to show neighbor info"
        caseReturnCode = 1

    lnk03PrtStats = retStruct.valueGet(key='portStats')

    common.LogOutput('info', "CLI_Switch1")
    retStruct.printValueString()

    if (lnk03PrtStats[device1.linkPortMapping['lnk03']]['Neighbor_portID']):
        common.LogOutput('info',"\nCase Failed, Neighborship established by SW1 on Link 3")
        common.LogOutput('info', "\nPort of SW1 neighbor is :" + str(lnk03PrtStats[device1.linkPortMapping['lnk03']]['Neighbor_portID']))
        common.LogOutput('info',"\nChassie Capabilities available : "+str(lnk03PrtStats[device1.linkPortMapping['lnk03']]['Chassis_Capabilities_Available']))
        common.LogOutput('info', "\nChassis Capabilities Enabled : "+str(lnk03PrtStats[device1.linkPortMapping['lnk03']]['Chassis_Capabilities_Enabled']))
        assert "Case Failed"
        caseReturnCode = 1

    else:
        common.LogOutput('info',"Case Passed, No Neighbor is present for SW1 on Link 3")


    common.LogOutput('info', "\nShowing Lldp neighborship for SW2 on Link 3")
    retStruct = ShowLldpNeighborInfo(deviceObj=device2, port=device2.linkPortMapping['lnk03'])
    if retCode:
        assert "\nFailed to show neighbor info"
        caseReturnCode = 1
    lnk03PrtStats = retStruct.valueGet(key='portStats')

    common.LogOutput('info', "CLI_Switch2")
    retStruct.printValueString()
    if (lnk03PrtStats[device2.linkPortMapping['lnk03']]['Neighbor_portID']):
        common.LogOutput('info',"\nCase Passed, Neighborship established by SW2 on Link 3")
        common.LogOutput('info', "\nPort of SW2 neighbour is :" + str(lnk03PrtStats[device2.linkPortMapping['lnk03']]['Neighbor_portID']))
        common.LogOutput('info',"\nChassie Capablities available : "+ str(lnk03PrtStats[device2.linkPortMapping['lnk03']]['Chassis_Capabilities_Available']))
        common.LogOutput('info', "\nChassis Capabilities Enabled : "+ str( lnk03PrtStats[device2.linkPortMapping['lnk03']]['Chassis_Capabilities_Enabled']))
    else :
        common.LogOutput('info',"Case Failed, No Neighbor is present for SW2 on Link 3")
        assert "Case Failed"
        caseReturnCode = 1

    #Case 4

    common.LogOutput('info', "\n\n\n### Case 4:tx and rx disabled on SW1 ###\n\n\n")

    common.LogOutput('info', "\nShowing Lldp neighborship on SW1 Port 4")

    retStruct = ShowLldpNeighborInfo(deviceObj=device1, port=device1.linkPortMapping['lnk04'])
    retCode = retStruct.returnCode()
    if retCode:
        assert "\nFailed to show neighbor info"
        caseReturnCode = 1


    common.LogOutput('info', "CLI_Switch1")
    retStruct.printValueString()

    lnk04PrtStats = retStruct.valueGet(key='portStats')
    if (lnk04PrtStats[device1.linkPortMapping['lnk04']]['Neighbor_portID']):
        common.LogOutput('info',"\nCase Failed, Neighborship established by SW1 on Link 4")
        common.LogOutput('info', "\nPort of SW1 neighbor is :" + str(lnk04PrtStats[device1.linkPortMapping['lnk04']]['Neighbor_portID']))
        common.LogOutput('info',"\nChassie Capabilities available : "+str(lnk04PrtStats[device1.linkPortMapping['lnk04']]['Chassis_Capabilities_Available']))
        common.LogOutput('info', "\nChassis Capabilities Enabled : "+str(lnk04PrtStats[device1.linkPortMapping['lnk04']]['Chassis_Capabilities_Enabled']))
        assert "Case Failed"
        caseReturnCode = 1
    else:
        common.LogOutput('info',"Case Passed, No Neighbor is present for SW1 on Link 4")

    common.LogOutput('info', "\nShowing Lldp neighborship on SW2")
    retStruct = ShowLldpNeighborInfo(deviceObj=device2, port=device2.linkPortMapping['lnk04'])
    if retCode:
        assert "Failed to show neighbor info"
        caseReturnCode = 1
    lnk04PrtStats = retStruct.valueGet(key='portStats')
    common.LogOutput('info', "CLI_Switch2")
    retStruct.printValueString()


    if (lnk04PrtStats[device2.linkPortMapping['lnk04']]['Neighbor_portID']):
        common.LogOutput('info',"\nCase Failed, Neighborship established by SW2 on Link 4")
        common.LogOutput('info', "\nPort of SW2 neighbour is :" + str(lnk04PrtStats[device2.linkPortMapping['lnk04']]['Neighbor_portID']))
        common.LogOutput('info',"\nChassie Capablities available : "+ str(lnk04PrtStats[device2.linkPortMapping['lnk04']]['Chassis_Capabilities_Available']))
        common.LogOutput('info', "\nChassis Capabilities Enabled : "+ str( lnk04PrtStats[device2.linkPortMapping['lnk04']]['Chassis_Capabilities_Enabled']
))
        assert "Case Failed"
        caseReturnCode = 1

    else :
        common.LogOutput('info',"Case Passed, No Neighbor is present for SW2 on Link 4")

    # Down the interfaces
    common.LogOutput('info', "Disabling interface on SW1")
    retStruct = InterfaceEnable(deviceObj=device1, enable=False, interface=device1.linkPortMapping['lnk01'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to disable interface"
       caseReturnCode = 1

    retStruct = InterfaceEnable(deviceObj=device1, enable=False, interface=device1.linkPortMapping['lnk02'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to disable interface"
       caseReturnCode = 1

    retStruct = InterfaceEnable(deviceObj=device1, enable=False, interface=device1.linkPortMapping['lnk03'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to disable interface"
       caseReturnCode = 1

    retStruct = InterfaceEnable(deviceObj=device1, enable=False, interface=device1.linkPortMapping['lnk04'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to disable interface"
       caseReturnCode = 1

    common.LogOutput('info', "Disabling interface on SW2")
    retStruct = InterfaceEnable(deviceObj=device2, enable=False, interface=device2.linkPortMapping['lnk01'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to disable interface"
       caseReturnCode = 1

    retStruct = InterfaceEnable(deviceObj=device2, enable=False, interface=device2.linkPortMapping['lnk02'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to disable interface"
       caseReturnCode = 1

    retStruct = InterfaceEnable(deviceObj=device2, enable=False, interface=device2.linkPortMapping['lnk03'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to disable interface"
       caseReturnCode = 1

    retStruct = InterfaceEnable(deviceObj=device2, enable=False, interface=device2.linkPortMapping['lnk04'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to disable interface"
       return caseReturnCode
    return caseReturnCode





def lldp_enable_disable(**kwargs):
    device1 = kwargs.get('device1',None)
    device2 = kwargs.get('device2',None)
    caseReturnCode = 0

    #Configuring lldp on SW1
    common.LogOutput('info', "\n\n\nConfig lldp on SW1")
    retStruct = LldpConfig(deviceObj=device1, enable=True)
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to configure LLDP on SW1"
       caseReturnCode = 1

    #Enabling interface 1 SW1
    common.LogOutput('info', "Enabling interface on SW1")
    retStruct = InterfaceEnable(deviceObj=device1, enable=True, interface=device1.linkPortMapping['lnk01'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to enabling interafce on SW1"
       caseReturnCode = 1

    common.LogOutput('info', "\n\n\nConfig lldp on SW2")
    retStruct = LldpConfig(deviceObj=device2, enable=True)
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Configure lldp on SW2"
       caseReturnCode = 1
    #Enabling interface 1 SW2
    common.LogOutput('info', "Enabling interface on SW2")
    retStruct = InterfaceEnable(deviceObj=device2, enable=True, interface=device2.linkPortMapping['lnk01'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to enable interface on SW2"
       caseReturnCode = 1
    #Waiting for neighbour entry to flood
    common.Sleep(seconds=25, message="\nWaiting for switch processes to fully come up")

    #Parsing neighbour info for SW1
    common.LogOutput('info', "\nShowing Lldp neighbourship on SW1")
    retStruct = ShowLldpNeighborInfo(deviceObj=device1, port=device1.linkPortMapping['lnk01'])
    if retCode:
       assert "Failed to show neighbour info"
       caseReturnCode = 1

    common.LogOutput('info', "CLI_Switch1")
    retStruct.printValueString()
    lnk01PrtStats = retStruct.valueGet(key='portStats')
    if (lnk01PrtStats[device1.linkPortMapping['lnk01']]['Neighbor_portID']):
       common.LogOutput('info',"\nCase Passed, Neighborship established by SW1")
       common.LogOutput('info', "\nPort of SW1 neighbor is :" + str(lnk01PrtStats[device1.linkPortMapping['lnk01']]['Neighbor_portID']))
       common.LogOutput('info',"\nChassie Capabilities available : "+str(lnk01PrtStats[device1.linkPortMapping['lnk01']]['Chassis_Capabilities_Available']))
       common.LogOutput('info', "\nChassis Capabilities Enabled : "+str(lnk01PrtStats[device1.linkPortMapping['lnk01']]['Chassis_Capabilities_Enabled']))
    else :
       common.LogOutput('info',"\nCase Failed, No Neighbour is present for SW2")
       assert "Case Failed"
       caseReturnCode = 1
    #Parsing neighbour info for SW2
    common.LogOutput('info', "\nShowing Lldp neighborship on SW2")
    retStruct = ShowLldpNeighborInfo(deviceObj=device2, port=device2.linkPortMapping['lnk01'])

    if retCode:
       common.LogOutput('error', "\nFailed to show neighbor info")
       assert "Case Failed"
       caseReturnCode = 1

    common.LogOutput('info', "CLI_Switch2")
    retStruct.printValueString()
    lnk01PrtStats = retStruct.valueGet(key='portStats')
    if (lnk01PrtStats[device2.linkPortMapping['lnk01']]['Neighbor_portID']):
       common.LogOutput('info',"\nCase Passed, Neighborship established by SW2")
       common.LogOutput('info', "\nPort of SW2 neighbor is :" + str(lnk01PrtStats[device2.linkPortMapping['lnk01']]['Neighbor_portID']))
       common.LogOutput('info',"\nChassie Capablities available : "+ str(lnk01PrtStats[device2.linkPortMapping['lnk01']]['Chassis_Capabilities_Available']))
       common.LogOutput('info', "\nChassis Capabilities Enabled : "+ str( lnk01PrtStats[device2.linkPortMapping['lnk01']]['Chassis_Capabilities_Enabled']))
    else :
       common.LogOutput('info',"Case Failed, No Neighbor is present for SW2")
       assert "Case Failed"
       caseReturnCode = 1

    common.LogOutput('info', "\nDisabling lldp on SW1")
    retStruct = LldpConfig(deviceObj=device1, enable=False)
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to disable interface"
       caseReturnCode = 1

    common.LogOutput('info', "\nDisabling lldp on SW2")
    retStruct = LldpConfig(deviceObj=device2, enable=False)
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to disable interface"
       caseReturnCode = 1

    #Parsing lldp neighbour info SW1
    common.LogOutput('info', "\nShowing Lldp neighborship on SW1")
    retStruct = ShowLldpNeighborInfo(deviceObj=device1, port=device1.linkPortMapping['lnk01'])
    retCode = retStruct.returnCode()
    if retCode:
       assert "Failed to show neighbor info"
       caseReturnCode = 1

    common.LogOutput('info', "CLI_Switch1")
    retStruct.printValueString()

    lnk01PrtStats = retStruct.valueGet(key='portStats')
    if (lnk01PrtStats[device1.linkPortMapping['lnk01']]['Neighbor_portID']):
       common.LogOutput('info',"\nCase Failed, Neighborship still present on SW1")
       common.LogOutput('info', "\nPort of SW1 neighbor is :" + str(lnk01PrtStats[device1.linkPortMapping['lnk01']]['Neighbor_portID']))
       assert "Case Failed"
       caseReturnCode = 1

    else:
       common.LogOutput('info',"\nCase Passed, No Neighbor is present for SW1")

    #Parsing lldp neighbour info SW2
    common.LogOutput('info', "\nShowing Lldp neighborship on SW2")
    retStruct = ShowLldpNeighborInfo(deviceObj=device2, port=device2.linkPortMapping['lnk01'])
    if retCode:
       common.LogOutput('error', "\nFailed to show neighbor info")
       assert "Case Failed"
    lnk01PrtStats = retStruct.valueGet(key='portStats')
    common.LogOutput('info', "CLI_Switch2")
    retStruct.printValueString()
    if (lnk01PrtStats[device2.linkPortMapping['lnk01']]['Neighbor_portID']):
       common.LogOutput('info',"\nCase Failed, Neighborship is still present on SW2")
       assert "Case Failed"
       caseReturnCode = 1

    else :
       common.LogOutput('info',"Case Passed, No Neighbor is present for SW2")


    # Down the interfaces
    common.LogOutput('info', "Disabling interface on SW1")
    retStruct = InterfaceEnable(deviceObj=device1, enable=False, interface=device1.linkPortMapping['lnk01'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to disable interface"
       caseReturnCode = 1

    common.LogOutput('info', "Disabling interface on SW2")
    retStruct = InterfaceEnable(deviceObj=device2, enable=False, interface=device2.linkPortMapping['lnk01'])
    retCode = retStruct.returnCode()
    if retCode != 0:
       assert "Unable to disable interface"
       caseReturnCode = 1
    return caseReturnCode


class Test_lldp_configuration:
    def setup_class (cls):
        # Test object will parse command line and formulate the env
        Test_lldp_configuration.testObj = testEnviron(topoDict=topoDict)
        #    Get topology object
        Test_lldp_configuration.topoObj = Test_lldp_configuration.testObj.topoObjGet()

    def teardown_class (cls):
        Test_lldp_configuration.topoObj.terminate_nodes()

    def test_lldp_interface_txrx(self):
        dut01Obj = self.topoObj.deviceObjGet(device="dut01")
        dut02Obj = self.topoObj.deviceObjGet(device="dut02")
        retValue = lldp_interface_txrx(device1=dut01Obj, device2=dut02Obj)
        if retValue != 0:
            assert "Test failed"
        else:
            common.LogOutput('info', "test passed\n\n\n\n############################# Next Test #########################\n")

    def test_lldp_enable_disable(self):
        dut01Obj = self.topoObj.deviceObjGet(device="dut01")
        dut02Obj = self.topoObj.deviceObjGet(device="dut02")
        retValue = lldp_enable_disable(device1=dut01Obj, device2=dut02Obj)
        if retValue != 0:
            assert "Test failed"
        else:
            common.LogOutput('info', "test passed")
"""

ops-lldpd
===========

What is ops-lldpd?
--------------------
The ops-lldpd repository houses code for the OpenSwitch ops-lldpd daemon. The ops-lldpd daemon is responsible for advertising identies and capabilities using LLDP over the interfaces. It also receives advertisements from neighbors, parses the LLDP messages and populates the neighbor in the OVSDB Interface table.

What is the structure of the repository?
----------------------------------------
No changes in the directory structure were made to "lldpd" from https://github.com/vincentbernat/lldpd

What is the license?
--------------------
Apache 2.0 license. For more details refer to [COPYING](https://git.openswitch.net/cgit/openswitch/ops-lldpd/tree/ops/docs/COPYING)

What other documents are available?
-----------------------------------
For the high level design of LLDP, refer to [DESIGN](http://openswitch.net/documents/user/lldp_design)
For the component level design of ops-lldpd, refer to [DESIGN](https://git.openswitch.net/cgit/openswitch/ops-lldpd/tree/ops/docs/DESIGN)
For the current list of contributors and maintainers, refer to [AUTHORS](https://git.openswitch.net/cgit/openswitch/ops-lldpd/tree/ops/docs/AUTHORS)

For general information about the OpenSwitch project refer to http://www.openswitch.net

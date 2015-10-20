
# LLDP Component Test Cases

## Contents

- [Counters and statistics recovery test](#counters-and-statistics-recovery-test)
- [Call Libevent test](#call-libevent-test)

## Counters and statistics recovery test

### Objective

This test verifies that after an lldp daemon crashes and recovers,
its counters and statistics will not reset back to 0 but
will be maintained with the values before the crash.

### Requirements

- Virtual Mininet Test Setup
- **CT File**: ops-lldpd/tests/test_lldpd_ct_counters_recovery.py

### Setup

The setup requires one switch and at least one host that supports
LLDP.

### Description

The two devices are allowed to run for about 30 seconds
to 1 minute, so that the switch lldp daemon accumulates some
counters and statistics.  Then the LLDP daemon is deliberately killed
and is almost immediately restarted by the system daemon.  After a few
seconds have elapsed, the counters are checked in the OVSDB to make
sure that they have not been reset back to 0 and in fact have at least the
values before the crash.

There are no CLI commands to enter.  The test is run by executing
the python test script (test_lldpd_ct_counters_recovery.py) in the
correct framework.

### Test result criteria

#### Test pass or fail criteria

The test script displays pass or fail results.

The test passes if the LLDP counters or statistics in the OVSDB are
higher than the last time their values were calculated just before the daemon
was terminated. It will fail otherwise. The daemon is expected to
have synchronised its counters in the OVSDB as soon as it has restarted.

There is no user configuration to perform or configuration output
to monitor.

## Call libevent test

### Objective

This test verifies that the LLDP daemon event scheduling is working
correctly by sending a burst of 100 events and then checking that each
event actually did get processed.

### Requirements

- Virtual Mininet Test Setup
- **CT File**: ops-lldpd/tests/test_lldpd_ct_call_libevent.py

### Setup

The setup requires only one switch.

### Description

The test sends a burst of 100 libevents and waits 2 seconds to allow
these events to get processed by the LLDP daemon. It then checks that at
least 100 events were processed.

There are no CLI commands to run.  The test is run by executing
the python test script (test_lldpd_ct_call_libevent.py) in the correct
framework.

### Test result criteria

#### Test pass or fail criteria

The test script displays pass or fail results.

There is no user configuration to perform or configuration output
to monitor.

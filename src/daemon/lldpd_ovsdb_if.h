/*
 * Copyright (C) 2015 Hewlett-Packard Development Company, L.P.
 * All Rights Reserved.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License"); you may
 *   not use this file except in compliance with the License. You may obtain
 *   a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *   WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *   License for the specific language governing permissions and limitations
 *   under the License.
 *
 * File: lldpd_ovsdb_if.h
 *
 * Purpose: This file includes all public interface defines needed by
 *          the new lldpd_ovsdb.c for lldpd - ovsdb integration
 */

#ifndef LLDPD_OVSDB_H
#define LLDPD_OVSDB_H 1

#include "lldpd.h"

/* check any stat/counter changes every so often and report if changed */
#define LLDP_CHECK_STATS_FREQUENCY_DFLT             5 /* seconds */
#define LLDP_CHECK_STATS_FREQUENCY_DFLT_MSEC \
    (LLDP_CHECK_STATS_FREQUENCY_DFLT * 1000)        /* milliseconds */

struct lldp_iface {
    char     *name;               /* Always nonnull. */
    const struct ovsrec_interface *ifrow; /* handle to ovsrec row */
    struct lldpd_hardware *hw; /* handle to lldp harware */
};

void lldpd_ovsdb_init(int argc, char *argv[]);
void init_ovspoll_to_libevent(struct lldpd *cfg);
void lldpd_ovsdb_exit(void);
void add_lldpd_hardware_interface(struct lldpd_hardware *hw);
void del_lldpd_hardware_interface(struct lldpd_hardware *hw);
void ovs_libevent_schedule_nbr(void *arg);

#endif //lldpd_ovsdb_if.h

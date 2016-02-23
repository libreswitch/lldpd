/* LLDP daemon client callback resigitration source files.
 *
 * Copyright (C) 2016 Hewlett Packard Enterprise Development LP.
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 * File: vtysh_ovsdb_lldp_context.c
 *
 * Purpose: Source for registering sub-context callback.
 */

#include "vtysh/vty.h"
#include "vtysh/vector.h"
#include "vswitch-idl.h"
#include "openswitch-idl.h"
#include "vtysh/vtysh_ovsdb_if.h"
#include "vtysh/vtysh_ovsdb_config.h"
#include "vtysh/utils/system_vtysh_utils.h"
#include "vtysh/utils/intf_vtysh_utils.h"
#include "vtysh_ovsdb_lldp_context.h"

/*-----------------------------------------------------------------------------
| Function : vtysh_intf_context_lldp_clientcallback
| Responsibility : Interface context, LLDP sub-context callback routine.
| Parameters :
|     p_private: Void pointer for holding address of vtysh_ovsdb_cbmsg_ptr
|                structure object.
| Return : void
-----------------------------------------------------------------------------*/
vtysh_ret_val
vtysh_intf_context_lldp_clientcallback(void *p_private)
{
  const char *data = NULL;
  vtysh_ovsdb_cbmsg_ptr p_msg = (vtysh_ovsdb_cbmsg *)p_private;
  const struct ovsrec_interface *ifrow = NULL;
  const char *if_name;

  ifrow = (struct ovsrec_interface *)p_msg->feature_row;
  if(NULL == &ifrow->other_config)
  {
    return e_vtysh_error;
  }

  data = smap_get(&ifrow->other_config,
                  INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR);
  if_name = ifrow->name;
  if (data)
  {
    if (VTYSH_STR_EQ(data, INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_TX))
    {
      PRINT_INTERFACE_NAME(p_msg->disp_header_cfg, p_msg, if_name)
      vtysh_ovsdb_cli_print(p_msg, "%4s%s", "", "no lldp reception");
    }
    else if (VTYSH_STR_EQ(data, INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_RX))
    {
      PRINT_INTERFACE_NAME(p_msg->disp_header_cfg, p_msg, if_name)
      vtysh_ovsdb_cli_print(p_msg, "%4s%s", "", "no lldp transmission");
    }
    else if (VTYSH_STR_EQ(data, INTERFACE_OTHER_CONFIG_MAP_LLDP_ENABLE_DIR_OFF))
    {
      PRINT_INTERFACE_NAME(p_msg->disp_header_cfg, p_msg, if_name)
      vtysh_ovsdb_cli_print(p_msg, "%4s%s", "", "no lldp transmission");
      vtysh_ovsdb_cli_print(p_msg, "%4s%s", "", "no lldp reception");
    }
  }

  return e_vtysh_ok;
}

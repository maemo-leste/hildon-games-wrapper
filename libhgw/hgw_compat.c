/*
 * This file is part of hildon-games-wrapper
 *
 * Copyright (C) 2006-2007 Nokia Corporation.
 *
 * Contact: Kuisma Salonen <kuisma.salonen@nokia.com>
 * Author: Kuisma Salonen <kuisma.salonen@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */


#include "hgw.h"
#include "hgw_priv.h"

#include "../hgw_debug.h"


int sapp_error = 0;

/* weak aliases */

HgwContext *sapp_initialize(int argc, char **argv) __attribute__ ((weak, alias("hgw_context_compat_init")));
HgwError sapp_deinitialize(HgwContext *context) __attribute__ ((weak, alias("hgw_context_compat_destroy_deinit")));
HgwError sapp_quit(HgwContext *context) __attribute__ ((weak, alias("hgw_context_compat_destroy_quit")));
HgwError sapp_check(HgwContext *ctx) __attribute__ ((weak, alias("hgw_context_compat_check")));


HgwError sapp_set_msg_cb_top(HgwContext *ctx, HgwCBFunc func) __attribute__ ((weak, alias("hgw_compat_set_cb_top")));
HgwError sapp_set_msg_cb_exit(HgwContext *ctx, HgwCBFunc func) __attribute__ ((weak, alias("hgw_compat_set_cb_exit")));
HgwError sapp_set_msg_cb_quit(HgwContext *ctx, HgwCBFunc func) __attribute__ ((weak, alias("hgw_compat_set_cb_quit")));
HgwError sapp_set_msg_cb_continue(HgwContext *ctx, HgwCBFunc func) __attribute__ ((weak, alias("hgw_compat_set_cb_continue")));
HgwError sapp_set_msg_cb_restart(HgwContext *ctx, HgwCBFunc func) __attribute__ ((weak, alias("hgw_compat_set_cb_restart")));
HgwError sapp_set_msg_cb_flush(HgwContext *ctx, HgwCBFunc func) __attribute__ ((weak, alias("hgw_compat_set_cb_flush")));


HgwError sapp_mainloop(HgwContext *ctx, HgwMessageFlags flags) __attribute__ ((weak, alias("hgw_msg_compat_receive")));
HgwError sapp_pause(HgwContext *ctx) __attribute__ ((weak, alias("hgw_compat_pause")));


/* init & deinit functions */

HgwContext *hgw_context_compat_init(int argc, char **argv)
{
  return hgw_context_init();
}


HgwError hgw_context_compat_destroy_deinit(HgwContext *context)
{
  hgw_context_destroy(context, HGW_BYE_PAUSED);

  return HGW_ERR_NONE;
}

HgwError hgw_context_compat_destroy_quit(HgwContext *context)
{
  hgw_context_destroy(context, HGW_BYE_PAUSED);

  return HGW_ERR_NONE;
}


int hgw_context_compat_check(HgwContext *ctx)
{
  HgwStartCommand cmd;
  int cont = 0;

  DEBUG_PRINT("entering hgw_context_compat_check()\n")


  cmd = hgw_context_get_start_command(ctx);


  switch(cmd) {
  case HGW_COMM_INVALID:
    /* old system isn't supposed to be used directly through the menu */
  case HGW_COMM_NONE:
    break;
  case HGW_COMM_RESTART:
    hgw_invoke_callback(ctx, HGW_CB_FLUSH, -15);
  case HGW_COMM_CONT:
    cont = 1;
    break;
  case HGW_COMM_QUIT:
    hgw_invoke_callback(ctx, HGW_CB_FLUSH, -15);
    hgw_invoke_callback(ctx, HGW_CB_QUIT, -15);

    /* duh, what was the point of this originally? */
    hgw_context_destroy(ctx, HGW_BYE_INACTIVE);
    break;
  default:
    break;
  }


  DEBUG_PRINT("will_continue: %d (%d)\n", cont, cmd)


  return cont;
}


/* callback setting functions */

HgwError hgw_compat_set_cb_top(HgwContext *ctx, HgwCBFunc func)
{
  return hgw_set_cb(ctx, HGW_CB_TOP, func);
}

HgwError hgw_compat_set_cb_exit(HgwContext *ctx, HgwCBFunc func)
{
  return hgw_set_cb(ctx, HGW_CB_EXIT, func);
}

HgwError hgw_compat_set_cb_quit(HgwContext *ctx, HgwCBFunc func)
{
  return hgw_set_cb(ctx, HGW_CB_QUIT, func);
}

HgwError hgw_compat_set_cb_continue(HgwContext *ctx, HgwCBFunc func)
{
  return hgw_set_cb(ctx, HGW_CB_CONTINUE, func);
}

HgwError hgw_compat_set_cb_restart(HgwContext *ctx, HgwCBFunc func)
{
  return hgw_set_cb(ctx, HGW_CB_RESTART, func);
}

HgwError hgw_compat_set_cb_flush(HgwContext *ctx, HgwCBFunc func)
{
  return hgw_set_cb(ctx, HGW_CB_FLUSH, func);
}


/* messaging functions */
/* jinxme: poll until no more messages, with blocking have flags */
HgwError hgw_msg_compat_receive(HgwContext *ctx, HgwMessageFlags flags)
{
  HgwError err = HGW_ERR_NONE;
  HgwMessage msg;


  DEBUG_PRINT("entering compat receive\n")


  err = hgw_msg_check_incoming(ctx, &msg, flags);

  DEBUG_PRINT("received %d (%d)\n", msg.type, err)


  if(!err) {
    if(msg.type == HGW_MSG_TYPE_CBREQ && msg.e_val < HGW_CB_LAST) {
      hgw_invoke_callback(ctx, msg.e_val, 0);
    }
  } else if(err == HGW_ERR_QUEUE_EMPTY) {
    err = HGW_ERR_NONE;
  }


  return err;
}

HgwError hgw_compat_pause(HgwContext *ctx)
{
  return hgw_notify_startup(ctx, HGW_SUN_PAUSE);
}

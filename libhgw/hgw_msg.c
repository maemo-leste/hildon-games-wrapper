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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>


#include "hgw.h"
#include "hgw_priv.h"

#include "../hgw_debug.h"


  /* the sessionbus - sysbus priorities are not really that good - TODO: add some poll() call here */
HgwError hgw_msg_check_incoming(HgwContext *ctx, HgwMessage *msg, HgwMessageFlags flags)
{
  DBusMessage *message = NULL;
  DBusMessageIter iter;
  DBusError err;
  const char *mthd;
  int message_useful = 0;


  dbus_error_init(&err);

  if(flags & HGW_MSG_FLAG_BLOCK) {
    while((dbus_connection_read_write(ctx->dbus_conn, 1000)   && !(message = dbus_connection_pop_message(ctx->dbus_conn))) &&
          (dbus_connection_read_write(ctx->sysbus_conn, 1000) && !(message = dbus_connection_pop_message(ctx->sysbus_conn))));
  } else {
    dbus_connection_read_write(ctx->dbus_conn, 0);

    message = dbus_connection_pop_message(ctx->dbus_conn);

    if(!message) {
      dbus_connection_read_write(ctx->sysbus_conn, 0);

      message = dbus_connection_pop_message(ctx->sysbus_conn);
    }
  }

  if(!message) {
    return HGW_ERR_QUEUE_EMPTY;
  }

  if(dbus_message_get_type(message) == DBUS_MESSAGE_TYPE_METHOD_CALL) {
    message_useful = 1;
    mthd = dbus_message_get_member(message);

    msg->type = HGW_MSG_TYPE_CBREQ;
    msg->allocated = 0;

    if(!strcmp(mthd, "game_continue") || !strcmp(mthd, "game_run")) {
      msg->e_val = HGW_CB_CONTINUE;
    } else if(!strcmp(mthd, "game_close")) {
      msg->e_val = HGW_CB_QUIT;
    } else if(!strcmp(mthd, "game_restart")) {
      msg->e_val = HGW_CB_RESTART;
    } else {
      msg->e_val = 0xffffffff;
    }
  } else if(dbus_message_get_type(message) == DBUS_MESSAGE_TYPE_SIGNAL) {
    mthd = dbus_message_get_member(message);

    dbus_message_iter_init(message, &iter);

    if(!strcmp(mthd, "display_status_ind")) {
      char *tmp;

      dbus_message_iter_get_basic(&iter, &tmp);

      DEBUG_PRINT("  display status: %s\n", tmp)

      if(strcmp(tmp, "off")) {
        if((ctx->devstate & HGW_DEVICE_STATE_DISPLAYOFF) == HGW_DEVICE_STATE_DISPLAYOFF) {
          ctx->devstate ^= HGW_DEVICE_STATE_DISPLAYOFF;
          msg->e_val = HGW_DEVICE_STATE_DISPLAYOFF;
          message_useful = 1;
        }
      } else {
        if((ctx->devstate & HGW_DEVICE_STATE_DISPLAYOFF) != HGW_DEVICE_STATE_DISPLAYOFF) {
          ctx->devstate |= HGW_DEVICE_STATE_DISPLAYOFF;
          msg->e_val = HGW_DEVICE_STATE_DISPLAYOFF | HGW_DEVICE_STATE_ON;
          message_useful = 1;
        }
      }
    } else if(!strcmp(mthd, "system_inactivity_ind")) {
      int tmp;

      dbus_message_iter_get_basic(&iter, &tmp);

      DEBUG_PRINT("  inactivity status: %s\n", tmp ? "TRUE" : "FALSE")

      if(tmp && ((ctx->devstate & HGW_DEVICE_STATE_INACTIVE) != HGW_DEVICE_STATE_INACTIVE)) {
        ctx->devstate |= HGW_DEVICE_STATE_INACTIVE;
        msg->e_val = HGW_DEVICE_STATE_INACTIVE | HGW_DEVICE_STATE_ON;
        message_useful = 1;
      } else if((ctx->devstate & HGW_DEVICE_STATE_INACTIVE) == HGW_DEVICE_STATE_INACTIVE) {
        ctx->devstate ^= HGW_DEVICE_STATE_INACTIVE;
        msg->e_val = HGW_DEVICE_STATE_INACTIVE;
        message_useful = 1;
      }
    } else if(!strcmp(mthd, "shutdown_ind") || !strcmp(mthd, "thermal_shutdown_ind")) {
      message_useful = 1;
      ctx->devstate |= HGW_DEVICE_STATE_SHUTDOWN;
      msg->e_val = HGW_DEVICE_STATE_SHUTDOWN | HGW_DEVICE_STATE_ON;
    } else if(!strcmp(mthd, "battery_low")) {
      message_useful = 1;
      ctx->devstate |= HGW_DEVICE_STATE_BATTERYLOW;
      msg->e_val = HGW_DEVICE_STATE_BATTERYLOW | HGW_DEVICE_STATE_ON;
    } else if(!strcmp(mthd, "charger_connected")) {
      if(ctx->devstate & HGW_DEVICE_STATE_BATTERYLOW) {
        message_useful = 1;
        ctx->devstate ^= HGW_DEVICE_STATE_BATTERYLOW; /* &= (-1 ^ HGW_DEVICE_STATE_BATTERYLOW) */
        msg->e_val = HGW_DEVICE_STATE_BATTERYLOW;
      }
    }

    DEBUG_PRINT("  got D-BUS signal (%s)\n", dbus_message_get_member(message))
    if(message_useful) {
      msg->type = HGW_MSG_TYPE_DEVSTATE;
      msg->allocated = 0;
    }
  }


  dbus_message_unref(message);


  if(!message_useful) {
    return HGW_ERR_QUEUE_EMPTY;
  }


  if((flags & HGW_MSG_FLAG_INVOKE_CB) &&
     (msg->type == HGW_MSG_TYPE_CBREQ) &&
     (msg->e_val != 0xffffffff)) {
    hgw_invoke_callback(ctx, msg->e_val, 0);
  }
     


  return HGW_ERR_NONE;
}


void hgw_msg_free_data(HgwMessage *msg)
{
  if(msg) {
    if(msg->allocated) {
      free(msg->v_val);
    }
  }
}


HgwError hgw_notify_startup(HgwContext *ctx, HgwStartupNotification sun)
{
  char *msg = NULL;


  switch(sun) {
  case HGW_SUN_PAUSE:
    msg = "game_pause";
    break;
  default:
    break;
  }


  if(msg) {
    hgw_send_message_to_startup(ctx, msg);
  }


  return HGW_ERR_NONE;
}

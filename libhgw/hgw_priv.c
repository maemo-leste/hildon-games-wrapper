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
#include <memory.h>
#include <string.h>

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

#include "hgw.h"
#include "hgw_priv.h"

#include "../hgw_debug.h"


void __attribute__ ((visibility("hidden"))) hgw_invoke_callback(HgwContext *ctx, HgwCallback cb, int code)
{
  DEBUG_PRINT("calling callback %d with code %d\n", cb, code)

  if(ctx->compat.callbacks[cb]) {
    ctx->compat.callbacks[cb](code);
  }
}


void __attribute__ ((visibility("hidden"))) hgw_send_message_to_startup(HgwContext *ctx, char *msg)
{
  DBusMessage *message;
  char *svc, *path, *iface;
  int size;


  size = strlen(ctx->service) + 9;
  svc = (char *)malloc(size);
  memset(svc, 0, size);
  sprintf(svc, "%s.startup", ctx->service);

  size = strlen(ctx->path) + 9;
  path = (char *)malloc(size);
  memset(path, 0, size);
  sprintf(path, "%s/startup", ctx->path);

  size = strlen(ctx->interface) + 9;
  iface = (char *)malloc(size);
  memset(iface, 0, size);
  sprintf(iface, "%s.startup", ctx->interface);


  message = dbus_message_new_method_call(svc, path, iface, msg);

  dbus_connection_send(ctx->dbus_conn, message, NULL);
  dbus_connection_flush(ctx->dbus_conn);

  dbus_message_unref(message);


  free(svc);
  free(path);
  free(iface);
}

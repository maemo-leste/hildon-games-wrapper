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
#include <unistd.h>

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

#include <sys/time.h>


#include "hgw.h"
#include "hgw_priv.h"

#include "../hgw_debug.h"


static void get_service_names(char **service, char **path, char **interface);

static void replace_chars_in_string(char *s, char orig, char rep);

static void reply_to_method_call(HgwContext *ctx, DBusMessage *call);

inline int millitime();


HgwContext *hgw_context_init()
{
  HgwContext *ret = NULL;
  DBusConnection *conn;
  DBusConnection *sysconn;
  DBusError err;

  char *svc = NULL, *path = NULL, *iface = NULL;
  int size;

  DEBUG_PRINT("entering hgw_context_init()\n")


  dbus_error_init(&err);

  conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
  if(dbus_error_is_set(&err)) {
    return NULL;
  }


  get_service_names(&svc, &path, &iface);

  DEBUG_PRINT("got names,\n  service: %s\n  path: %s\n  interface: %s\n", svc, path, iface)


  if(dbus_bus_request_name(conn, svc, DBUS_NAME_FLAG_REPLACE_EXISTING , &err) != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
    dbus_connection_unref(conn);
    if (svc)
      free (svc);
    if (path)
      free (path);
    if (iface)
      free (iface);
    return NULL;
  }


  dbus_error_init(&err);

  sysconn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
  if(dbus_error_is_set(&err)) {
    dbus_connection_unref(sysconn);
    if (svc)
      free (svc);
    if (path)
      free (path);
    if (iface)
      free (iface);
    return NULL;
  }


  if(dbus_bus_request_name(sysconn, svc, DBUS_NAME_FLAG_REPLACE_EXISTING , &err) != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
    dbus_connection_unref(conn);
    dbus_connection_unref(sysconn);
    if (svc)
      free (svc);
    if (path)
      free (path);
    if (iface)
      free (iface);
    return NULL;
  }


  dbus_error_init(&err);

  dbus_bus_add_match(sysconn, "type='signal',interface='com.nokia.mce.signal'", &err);

  if(dbus_error_is_set(&err)) {
    DEBUG_PRINT(" ** WARNING: cannot add match for dbus connection: %s\n", err.message)
    dbus_error_free(&err);
  }


  dbus_error_init(&err);

  dbus_bus_add_match(sysconn, "type='signal',interface='com.nokia.bme.signal'", &err);

  if(dbus_error_is_set(&err)) {
    DEBUG_PRINT(" ** WARNING: cannot add match for dbus connection: %s\n", err.message)
    dbus_error_free(&err);
  }


  size = sizeof(HgwContext) + strlen(svc) + strlen(path) + strlen(iface) + 3;

  ret = (HgwContext *)malloc(size);
  memset(ret, 0, size);


  ret->dbus_conn = conn;

  ret->sysbus_conn = sysconn;


  ret->service = ((char *)ret) + sizeof(HgwContext);
  memcpy(ret->service, svc, strlen(svc) + 1);

  ret->path = ret->service + strlen(ret->service) + 1;
  memcpy(ret->path, path, strlen(path) + 1);

  ret->interface = ret->path + strlen(ret->path) + 1;
  memcpy(ret->interface, iface, strlen(iface) + 1);

  if (svc)
    free (svc);
  if (path)
    free (path);
  if (iface)
    free (iface);

  DEBUG_PRINT("returning proper context, dbugconns: %d %d\n", (int)ret->dbus_conn, (int)ret->sysbus_conn)


  ret->compat.deinit = hgw_context_compat_destroy_deinit;
  ret->compat.smain = hgw_msg_compat_receive;


    /* TODO: query this first from the mce */
  ret->devstate = HGW_DEVICE_STATE_NONE;


  return ret;
}


void hgw_context_destroy(HgwContext *context, HgwByeMessage bye)
{
  char *msg = NULL;

  DEBUG_PRINT("destroying tunjo context, bye: %d\n", bye)


  if(bye) {
    switch(bye) {
    case HGW_BYE_PAUSED:
      msg = "game_pause";
      break;
    case HGW_BYE_INACTIVE:
      msg = "game_close";
      break;
    default:
      break;
    }
    hgw_send_message_to_startup(context, msg);
  }


  dbus_connection_unref(context->dbus_conn);

  dbus_connection_unref(context->sysbus_conn);


  free(context);
}

/* fixme: replace with proper loop with timeout */
HgwStartCommand hgw_context_get_start_command(HgwContext *ctx)
{
  DBusMessage *msg;
  const char *mthd;
  HgwStartCommand ret = HGW_COMM_NONE;
  int starttime;

  DEBUG_PRINT("checking start command\n")


  starttime = millitime();

  while(millitime() - starttime < 4000) {
    dbus_connection_read_write(ctx->dbus_conn, 0);

    msg = dbus_connection_pop_message(ctx->dbus_conn);


    if(msg) {
      DEBUG_PRINT("message received\n")

      if(dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_METHOD_CALL) {
        mthd = dbus_message_get_member(msg);

        DEBUG_PRINT("  method call: %s\n", mthd)

        if(!mthd) {
          continue;
        } else if(strcmp(mthd, "top_application") == 0) {
          ret = HGW_COMM_NONE;
          break;
        } else if(strcmp(mthd, "game_run") == 0) {
          /*
           * Send a dbus reply back or osso-games-startup will complain with
           * "game not started yet".
           */
          reply_to_method_call(ctx, msg);
          ret = HGW_COMM_CONT;
          break;
        } else if(strcmp(mthd, "game_continue") == 0) {
          ret = HGW_COMM_CONT;
          break;
        } else if(strcmp(mthd, "game_restart") == 0) {
          ret = HGW_COMM_RESTART;
          break;
        } else if(strcmp(mthd, "game_close") == 0) {
          ret = HGW_COMM_QUIT;
          break;
        }
      }
    }

    usleep(2000);
  }


  DEBUG_PRINT("got message, returning %d\n", ret)


  return ret;
}


/* private functions */

static void get_service_names(char **service, char **path, char **interface)
{
  char *servicename;
  char *tmp;
  int id_n, i;
  char hexnum[2];


  servicename = getenv("HGW_EXEC_SERVICE");
  if(servicename) {
    *service = strdup(servicename);

    tmp = getenv("HGW_EXEC_PATH");
    if(!tmp) {
      *path = (char *)malloc(strlen(*service) + 2);
      *path[0] = '.';
      memcpy(*path + 1, *service, strlen(*service) + 1);
      replace_chars_in_string(*path, '.', '/');
    } else {
      *path = strdup(tmp);
    }

    tmp = getenv("HGW_EXEC_IFACE");
    *interface = tmp ? strdup(tmp) : strdup(*service);

    return;
  }


/*servicename = dbus_bus_get_activated_service();*/
  if(servicename) {
    *service   = strdup(servicename);
    *interface = strdup(servicename);

    *path = (char *)malloc(strlen(servicename) + 2);
    *path[0] = '.';
    memcpy(*path + 1, servicename, strlen(servicename) + 1);

    replace_chars_in_string(*path, '.', '/');
  } else { /* fallback - shouldn't occur */
    *service   = strdup("org.maemo.hgwXXXX");
    *interface = strdup("org.maemo.hgwXXXX");
    *path      = strdup("/org/maemo/hgwXXXX");


    id_n = getpid();
    for(i = 0; i < 4; i++) {
      hexnum[0] = (id_n >> (12 - 4*i)) & 0xf;
      hexnum[1] = (hexnum[0] < 10) ? hexnum[0] + 0x30 : hexnum[0] + 0x57;
      (*service)[13+i]   = hexnum[1];
      (*interface)[13+i] = hexnum[1];
      (*path)[14+i]      = hexnum[1];
    }
  }
}


static void replace_chars_in_string(char *s, char orig, char rep)
{
  while(*s) {
    if(*s == orig) {
      *s = rep;
    }

    s++;
  }
}


/** Send a "success" reply back to osso-games-startup for a given method call. */
static void reply_to_method_call(HgwContext *ctx, DBusMessage *call)
{
	DBusMessage *msg = dbus_message_new_method_return(call);

	dbus_bool_t ret = TRUE;
	dbus_message_append_args(msg, DBUS_TYPE_BOOLEAN, &ret, DBUS_TYPE_INVALID);

	dbus_connection_send(ctx->dbus_conn, msg, NULL);
	dbus_connection_flush(ctx->dbus_conn);

	dbus_message_unref(msg);
}


inline int millitime()
{
  struct timeval tv;

  gettimeofday(&tv, NULL);

  return ((tv.tv_sec - 1166619105) * 1000) + (tv.tv_usec / 1000);
}

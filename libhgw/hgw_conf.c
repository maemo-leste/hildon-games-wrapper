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


#include <stdlib.h>
#include <string.h>
#include <memory.h>

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

/* evil - needed for GCONF_VALUE_ defines */
#include <gconf/gconf-client.h>


#include "hgw.h"
#include "hgw_priv.h"

#include "../hgw_debug.h"


static HgwError get_value_from_daemon(DBusConnection *conn, char *path, void *dest, int type);

static HgwError dig_value_out_of_message(DBusMessageIter *iter, void *dest, int type);

static char *get_db_path(DBusConnection *conn);


HgwError hgw_conf_request_int(HgwContext *ctx, char *value_path, int *dest)
{
#ifdef DEBUG
  HgwError ret = get_value_from_daemon(ctx->sysbus_conn, value_path, dest, GCONF_VALUE_INT);

  DEBUG_PRINT("requesting int for path %s: %s\n", value_path, (ret == HGW_ERR_NONE) ? "success" : "failure")

  return ret;
#else
  return get_value_from_daemon(ctx->sysbus_conn, value_path, dest, GCONF_VALUE_INT);
#endif
}

HgwError hgw_conf_request_string(HgwContext *ctx, char *value_path, char *dest)
{
#ifdef DEBUG
  HgwError ret = get_value_from_daemon(ctx->sysbus_conn, value_path, dest, GCONF_VALUE_STRING);

  DEBUG_PRINT("requesting string for path %s: %s\n", value_path, (ret == HGW_ERR_NONE) ? "success" : "failure")

  return ret;
#else
  return get_value_from_daemon(ctx->sysbus_conn, value_path, dest, GCONF_VALUE_STRING);
#endif
}

HgwError hgw_conf_request_bool(HgwContext *ctx, char *value_path, char *dest)
{
#ifdef DEBUG
  HgwError ret = get_value_from_daemon(ctx->sysbus_conn, value_path, dest, GCONF_VALUE_BOOL);

  DEBUG_PRINT("requesting bool for path %s: %s\n", value_path, (ret == HGW_ERR_NONE) ? "success" : "failure")

  return ret;
#else
  return get_value_from_daemon(ctx->sysbus_conn, value_path, dest, GCONF_VALUE_BOOL);
#endif
}


static HgwError get_value_from_daemon(DBusConnection *conn, char *path, void *dest, int type)
{
  DBusMessage *msg;
  DBusMessage *ret;
  DBusMessageIter iter;
  DBusError err;

  HgwError retval = HGW_ERR_NONE;

  static char *db = NULL;
  int use_default_chema = 1;
  char *locale = getenv("LANG");


  if(!locale) {
    locale = "C";
  }


  if(!db) {
    db = get_db_path(conn);

    if(!db) {
      DEBUG_PRINT("  ** cannot get db path\n")
      return HGW_ERR_COMMUNICATION;
    }
  }

  DEBUG_PRINT("  db path: %s\n", db)


  msg = dbus_message_new_method_call("org.gnome.GConf", db, "org.gnome.GConf.Database", "LookupExtended");

  dbus_message_append_args (msg,
                            DBUS_TYPE_STRING, &path,
                            DBUS_TYPE_STRING, &locale,
                            DBUS_TYPE_BOOLEAN, &use_default_chema,
                            DBUS_TYPE_INVALID);

  dbus_error_init(&err);

  ret = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);

  dbus_message_unref(msg);


  if(dbus_error_is_set(&err)) {
    DEBUG_PRINT("  ** requesting value: %s\n", err.message)
    dbus_error_free(&err);
    return HGW_ERR_COMMUNICATION;
  }


  dbus_message_iter_init(ret, &iter);

  retval = dig_value_out_of_message(&iter, dest, type);

  dbus_message_unref(ret);


  return retval;
}


static HgwError
dig_value_out_of_message(DBusMessageIter *iter, void *dest, int type)
{
  DBusMessageIter sub, sub2;
  int real_type, tmp2;
  char *tmp;

  if (!iter || 
      (dbus_message_iter_get_arg_type (iter) == DBUS_TYPE_INVALID))
  {
    DEBUG_PRINT ("invalid message: empty\n");
    return HGW_ERR_NO_VALUE;
  }

  dbus_message_iter_recurse(iter, &sub);
  dbus_message_iter_next(&sub);


  dbus_message_iter_recurse(&sub, &sub2);
  dbus_message_iter_get_basic(&sub2, &real_type);


  if(real_type != type) {
    DEBUG_PRINT("invalid type: %d while expecting %d\n", type, real_type)
    return HGW_ERR_INVALID_TYPE;
  }


  dbus_message_iter_next(&sub2);

  if(type == GCONF_VALUE_STRING) {
    dbus_message_iter_get_basic(&sub2, &tmp);
    memcpy(dest, tmp, (strlen(tmp) < 255) ? strlen(tmp) : 255);
    ((char *)dest)[(strlen(tmp) < 255) ? strlen(tmp) : 255] = 0;
  } else if(type == GCONF_VALUE_BOOL) {
    dbus_message_iter_get_basic(&sub2, &tmp2);
    *((char *)dest) = (tmp2 != 0);
  } else {
    dbus_message_iter_get_basic(&sub2, dest);
  }


  return HGW_ERR_NONE;
}


static char *get_db_path(DBusConnection *conn)
{
  DBusMessage *msg;
  DBusMessage *ret;
  DBusError err;
  char *dbpath = NULL;


  dbus_error_init(&err);

  dbus_bus_start_service_by_name(conn, "org.gnome.GConf", 0, NULL, &err);

  if(dbus_error_is_set(&err)) {
    DEBUG_PRINT("  ** unable to start GConf service: %s\n", err.message);
    dbus_error_free(&err);
    return NULL;
  }


  msg = dbus_message_new_method_call("org.gnome.GConf", "/org/gnome/GConf/Server", "org.gnome.GConf.Server", "GetDefaultDatabase");


  dbus_error_init(&err);

  ret = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);

  dbus_message_unref(msg);


  if(dbus_error_is_set(&err)) {
    DEBUG_PRINT("  ** get default db path: %s (%d)\n", err.message, (int)ret)
    dbus_error_free(&err);
    return NULL;
  }


  dbus_message_get_args(ret, NULL, DBUS_TYPE_STRING, &dbpath, DBUS_TYPE_INVALID);

  dbus_message_unref(ret);


  return strdup(dbpath);
}


  /* Weak aliases of compat functions - gcc 4.2 doesn't like them being in a
   * separate file */
HgwError sapp_gconf_request_int(HgwContext *context, char *value_path, int *dest) __attribute__ ((weak, alias("hgw_conf_request_int")));
HgwError sapp_gconf_request_string(HgwContext *context, char *value_path, char *dest) __attribute__ ((weak, alias("hgw_conf_request_string")));
HgwError sapp_gconf_request_bool(HgwContext *context, char *value_path, char *dest) __attribute__ ((weak, alias("hgw_conf_request_bool")));

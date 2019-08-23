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


#ifndef __HGW_PRIV_H__
#define __HGW_PRIV_H__


#ifndef DBUS_H
#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
#endif /* DBUS_H */


  /* hgw_sapp_compat structure - remainings from old lessertunjo for
     backwards compatibility */
typedef struct {
  int realbuflen, sockfd;
  struct sockaddr_un *saddr;
  char inbuf[16], realbuf[256];

  char *configuration_socket;

  int command;

  /* "object oriented" design :D - remains from old lessertunjo */
  HgwError (*deinit)(HgwContext *);
  HgwError (*smain)(HgwContext *, HgwMessageFlags);

  /* function pointers - for binary compatibility order of HgwCallback enum
     MUST be preserved */
/*int (*msg_cb_flush)(int);
  int (*msg_cb_restart)(int);
  int (*msg_cb_continue)(int);
  int (*msg_cb_exit)(int);
  int (*msg_cb_quit)(int);
  int (*msg_cb_top)(int);*/
  HgwCBFunc callbacks[HGW_CB_LAST];
} hgw_sapp_compat;


struct HgwContext_ {
  /* hgw_sapp_compat in the beginning to maintain binary compatibility */
  hgw_sapp_compat compat;


  DBusConnection *dbus_conn;
  DBusConnection *sysbus_conn;


  HgwDeviceState devstate;


  char *service;
  char *path;
  char *interface;
};


void hgw_invoke_callback(HgwContext *ctx, HgwCallback cb, int code);

void hgw_send_message_to_startup();


#endif /* __HGW_PRIV_H__ */

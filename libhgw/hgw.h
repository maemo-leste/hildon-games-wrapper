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


#ifndef __HGW_H__
#define __HGW_H__


#ifdef __cplusplus
extern "C" {
#endif


/* type declarations & enumerations */

typedef struct HgwContext_ HgwContext;


typedef enum {
  HGW_MSG_TYPE_CBREQ,
  HGW_MSG_TYPE_DEVSTATE,
  HGW_MSG_TYPE_LAST
} HgwMessageType;


typedef struct {
  HgwMessageType type;

  unsigned long e_val;
  int allocated;
  void *v_val;
} HgwMessage;


typedef enum {
  HGW_ERR_NO_VALUE = -23,
  HGW_ERR_INVALID_TYPE = -22,
  HGW_ERR_COMMUNICATION = -21,
  HGW_ERR_QUEUE_EMPTY = -20,
  HGW_ERR_NONE = 0
} HgwError;

typedef enum {
  HGW_CB_FLUSH = 0,
  HGW_CB_RESTART,
  HGW_CB_CONTINUE,
  HGW_CB_EXIT,
  HGW_CB_QUIT,
  HGW_CB_TOP,
  HGW_CB_LAST
} HgwCallback;

typedef enum {
  HGW_DEVICE_STATE_NONE = 0,
  HGW_DEVICE_STATE_INACTIVE = 0x01,
  HGW_DEVICE_STATE_MEMLOW = 0x02,
  HGW_DEVICE_STATE_SHUTDOWN = 0x04,
  HGW_DEVICE_STATE_DISPLAYOFF = 0x08,
  HGW_DEVICE_STATE_BATTERYLOW = 0x10,
  HGW_DEVICE_STATE_ON = 0x8000
} HgwDeviceState;

typedef enum {
  HGW_MSG_FLAG_NONE = 0x00,
  HGW_MSG_FLAG_BLOCK = 0x01,
  HGW_MSG_FLAG_INVOKE_CB = 0x02
} HgwMessageFlags;

typedef enum {
  HGW_COMM_INVALID = -1,
  HGW_COMM_NONE = 0,
  HGW_COMM_CONT,
  HGW_COMM_RESTART,
  HGW_COMM_QUIT
} HgwStartCommand;

typedef enum {
  HGW_BYE_NONE = 0,
  HGW_BYE_PAUSED,
  HGW_BYE_INACTIVE
} HgwByeMessage;

typedef enum {
  HGW_SUN_NONE = 0,
  HGW_SUN_PAUSE
} HgwStartupNotification;


typedef int (*HgwCBFunc)(int);


/* (de)initialization functions */

HgwContext *hgw_context_init(void);

void hgw_context_destroy(HgwContext *context, HgwByeMessage bye);

HgwStartCommand hgw_context_get_start_command(HgwContext *ctx);


/* compat (de)initialization */

HgwContext *hgw_context_compat_init(int argc, char **argv);

HgwError hgw_context_compat_destroy_deinit(HgwContext *context);
HgwError hgw_context_compat_destroy_quit(HgwContext *context);

int hgw_context_compat_check(HgwContext *ctx);


/* callbacks, mostly compat code */

HgwError hgw_set_cb(HgwContext *ctx, HgwCallback cb, HgwCBFunc func);

HgwError hgw_compat_set_cb_top(HgwContext *ctx, HgwCBFunc func);
HgwError hgw_compat_set_cb_exit(HgwContext *ctx, HgwCBFunc func);
HgwError hgw_compat_set_cb_quit(HgwContext *ctx, HgwCBFunc func);
HgwError hgw_compat_set_cb_continue(HgwContext *ctx, HgwCBFunc func);
HgwError hgw_compat_set_cb_restart(HgwContext *ctx, HgwCBFunc func);
HgwError hgw_compat_set_cb_flush(HgwContext *ctx, HgwCBFunc func);


/* messaging API */

HgwError hgw_msg_check_incoming(HgwContext *ctx, HgwMessage *msg, HgwMessageFlags flags);

void hgw_msg_free_data(HgwMessage *msg);

HgwError hgw_msg_compat_receive(HgwContext *ctx, HgwMessageFlags flags);

 /* is this needed? the reason it's here is that user wouldn't send
    accidentally game terminated - messages */
HgwError hgw_notify_startup(HgwContext *ctx, HgwStartupNotification sun);

HgwError hgw_compat_pause(HgwContext *ctx);


/* gconf wrapping */

HgwError hgw_conf_request_int(HgwContext *ctx, char *value_path, int *dest);
HgwError hgw_conf_request_string(HgwContext *ctx, char *value_path, char *dest);
HgwError hgw_conf_request_bool(HgwContext *ctx, char *value_path, char *dest);


#ifdef __cplusplus
}
#endif


#endif /* __HGW_H__ */

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

/*
 * Backwards compatibility header for tunjo - doesn't do anything useful apart
 * from that. Do not use in production code.
 */

#ifndef _SHADOW_APPLICATION_HEADER_
#define _SHADOW_APPLICATION_HEADER_

#include <hgw/hgw.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef int     sapp_boolean;


#define sapp_data HgwContext


#define sapp_initialize(A, B) hgw_context_compat_init(A, B)
#define sapp_deinitialize(A) hgw_context_compat_destroy_deinit(A)
#define sapp_quit(A) hgw_context_compat_destroy_quit(A)
#define sapp_check(A) hgw_context_compat_check(A)
#define sapp_set_msg_cb_top(A, B) hgw_compat_set_cb_top(A, B)
#define sapp_set_msg_cb_exit(A, B) hgw_compat_set_cb_exit(A, B)
#define sapp_set_msg_cb_quit(A, B) hgw_compat_set_cb_quit(A, B)
#define sapp_set_msg_cb_continue(A, B) hgw_compat_set_cb_continue(A, B)
#define sapp_set_msg_cb_restart(A, B) hgw_compat_set_cb_restart(A, B)
#define sapp_set_msg_cb_flush(A, B) hgw_compat_set_cb_flush(A, B)
#define sapp_mainloop(A, B) hgw_msg_compat_receive(A, B)
#define sapp_pause(A) hgw_notify_startup(A, HGW_SUN_PAUSE)


/* additional gconf requesting functions */
#define sapp_gconf_request_int(A, B, C) hgw_conf_request_int(A, B, C)
#define sapp_gconf_request_string(A, B, C) hgw_conf_request_string(A, B, C)
#define sapp_gconf_request_bool(A, B, C) hgw_conf_request_bool(A, B, C)


extern int sapp_error;

#define SAPP_FALSE      0
#define SAPP_TRUE       !SAPP_FALSE /* its a better declaration of TRUE */

#define SAPP_ERROR_NONE            HGW_ERR_NONE
#define SAPP_ERROR_NOMEMORY       -1
#define SAPP_ERROR_INVALIDPARAMS  -2
#define SAPP_ERROR_SOCKET         -3
#define SAPP_ERROR_CONNECTION     -4
#define SAPP_ERROR_CONNANOTHER    -5
#define SAPP_ERROR_WRONGID        -6
#define SAPP_ERROR_BYE            -7
#define SAPP_ERROR_BYESEND        -8
#define SAPP_ERROR_CONNECTIONLOST -9
#define SAPP_ERROR_FILEOPEN       -10
#define SAPP_ERROR_FILEFORMAT     -11
#define SAPP_ERROR_OSSOINIT       -12
#define SAPP_ERROR_EXECUTION      -13
#define SAPP_ERROR_INVALIDARGS    -14
#define SAPP_ERROR_QUIT           -15
#define SAPP_ERROR_NOVALUE        -16

#define SAPP_FLAGS_BLOCK           HGW_MSG_FLAG_BLOCK


#ifdef __cplusplus
}
#endif
#endif /* _SHADOW_APPLICATION_HEADER_ */

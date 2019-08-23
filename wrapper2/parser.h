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


#ifndef __PARSER_H__
#define __PARSER_H__


/* this struct should be treated as READ-ONLY outside the parser API */
typedef struct {
  char *execfile;
  char *service;
  char *path;
  char *interface;
} game_data;


/* parses old-style game config files */
game_data *wrapper_parse_config(char *filename);

void wrapper_free_game_data(game_data *data);


#endif /* __PARSER_H__ */

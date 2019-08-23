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
#include <memory.h>


#include "parser.h"


static void remove_linefeeds(char *s);


game_data *wrapper_parse_config(char *filename)
{
  FILE *f;
  char tmp[4][256], *ptr;
  game_data *ret = NULL;


  f = fopen(filename, "r");
  if(f) {
    memset(tmp[0], 0, 256);
    fgets(tmp[0], 256, f);
    if((ptr = strstr(tmp[0], " ")) != NULL)
      *ptr = 0;

    memset(tmp[0], 0, 256);
    fgets(tmp[0], 256, f);
    if(((ptr = strstr(tmp[0], " ")) != NULL) || ((ptr = strstr(tmp[0], "\n")) != NULL))
      *ptr = 0;

    memset(tmp[1], 0, 256);
    fgets(tmp[1], 256, f);
    if(((ptr = strstr(tmp[1], " ")) != NULL) || ((ptr = strstr(tmp[0], "\n")) != NULL))
      *ptr = 0;

    memset(tmp[2], 0, 256);
    fgets(tmp[2], 256, f);
    if(((ptr = strstr(tmp[2], " ")) != NULL) || ((ptr = strstr(tmp[0], "\n")) != NULL))
      *ptr = 0;

    memset(tmp[3], 0, 256);
    fgets(tmp[3], 256, f);
    if(((ptr = strstr(tmp[3], " ")) != NULL) || ((ptr = strstr(tmp[0], "\n")) != NULL))
      *ptr = 0;


    ret = (game_data *)malloc(sizeof(game_data) + strlen(tmp[0]) + strlen(tmp[1]) + strlen(tmp[2]) + strlen(tmp[3]) + 4);

    ret->execfile = ((char *)ret) + sizeof(game_data);
    remove_linefeeds(tmp[0]);
    memcpy(ret->execfile, tmp[0], strlen(tmp[0]) + 1);

    ret->service = ret->execfile + strlen(ret->execfile) + 1;
    remove_linefeeds(tmp[1]);
    memcpy(ret->service, tmp[1], strlen(tmp[1]) + 1);

    ret->path = ret->service + strlen(ret->service) + 1;
    remove_linefeeds(tmp[2]);
    memcpy(ret->path, tmp[2], strlen(tmp[2]) + 1);

    ret->interface = ret->path + strlen(ret->path) + 1;
    remove_linefeeds(tmp[3]);
    memcpy(ret->interface, tmp[3], strlen(tmp[3]) + 1);


    fclose(f);
  }


  return ret;
}


void wrapper_free_game_data(game_data *data)
{
  free(data);
}


static void remove_linefeeds(char *s)
{
  while(*s) {
    if(*s == '\n') {
      *s = '\0';
    }

    s++;
  }
}

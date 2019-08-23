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
#include <unistd.h>

#include "parser.h"

#include "../hgw_debug.h"


int main(int argc, char *argv[])
{
  game_data *data;

  DEBUG_PRINT("--- entering wrapper ---\n")


  if(argc < 2) {
    return 1;
  }


  DEBUG_PRINT("parsing %s\n", argv[1])


  data = wrapper_parse_config(argv[1]);
  if(!data) {
    return 1;
  }

  DEBUG_PRINT("parsed following:\n  service: %s\n  path: %s\n  interface: %s\n", data->service, data->path, data->interface)
  DEBUG_PRINT("will execute: %s\n", data->execfile)


  setenv("HGW_EXEC_SERVICE", data->service, 1);
  setenv("HGW_EXEC_PATH", data->path, 1);
  setenv("HGW_EXEC_IFACE", data->interface, 1);

  execlp(data->execfile, data->execfile, NULL);


    /* seriously, those won't be executed anyway... */
  wrapper_free_game_data(data);
  data = NULL;

  DEBUG_PRINT("** ERROR: execution failed\n")


  return 0;
}

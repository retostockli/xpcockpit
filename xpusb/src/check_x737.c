/* This is the check_x737.c code which tries to check for the x737 dataref periodically.
   This is needed when the x737 airplane from EADT is not loaded at the beginning of the flight
   simulator startup and the client already runs. We thus need to unload the x737 plugin dataref
   from time to time to re-check its existence.

   Copyright (C) 2018 Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <sys/types.h>

#include "common.h"
#include "iniparser.h"
#include "libiocards.h"
#include "handleserver.h"
#include "serverdata.h"

#include "check_x737.h"

void check_x737(void) {

  struct timeval x737_time2;
  double dt;

  int *status_x737 = link_dataref_int("x737/systems/afds/plugin_status");

  if (*status_x737 == INT_MISS) {

    /* get new time */
    gettimeofday(&x737_time2,NULL);
  
    dt = ((x737_time2.tv_sec - x737_time1.tv_sec) + (x737_time2.tv_usec - x737_time1.tv_usec) / 1000000.0);

    /* check every 10 seconds */
    if (dt > 10.0) {

      /* unlink the dataref so that it can be checked again later */
      int ret = unlink_dataref("x737/systems/afds/plugin_status");

      //      printf("10 Seconds have passed \n");

      x737_time1 = x737_time2;

    }

  }

}

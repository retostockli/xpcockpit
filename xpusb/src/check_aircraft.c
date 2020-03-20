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

#include "check_zibo.h"

void check_zibo(void) {

  /* determine ACF by tail number */
  unsigned char *tailnum = link_dataref_byte_arr("sim/aircraft/view/acf_tailnum",  40, -1);  

  /* 0: not loaded; 1: x737; 2: standard B738; 3: B738 ZIBO MOD */
  int *status_737 = link_dataref_int("xpserver/status_737");

  if (tailnum) {
    //    printf("ACF_TAILNUM: %s \n",tailnum);

    if (strcmp((const char*) tailnum,"ZB738")==0) {
      *status_737 = 3;
    } else if (strcmp((const char*) tailnum,"NN816N")==0) {
      *status_737 = 2;
    } else if (strcmp((const char*) tailnum,"OY-GRL")==0) {
      *status_737 = 1;
    } else if (strcmp((const char*) tailnum,"D-ATUC")==0) {
      *status_737 = 1;
    } else {
      *status_737 = 0;
    }
    
  }
  
}

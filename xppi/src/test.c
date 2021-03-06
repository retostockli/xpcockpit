/* This is the test.c code which can be used to test basic I/O of the Arduino

   Copyright (C) 2021 Reto Stockli

   Additions for analog axes treatment by Hans Jansen 2011
   Also several cosmetic changes and changes for Linux compilation
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
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>

#include "common.h"
#include "serverdata.h"
#include "test.h"

void test(void)
{

  int ret;

  int LED_PIN = 22; /* GPIO 21, Physical Pin 31 */
  int KEY_PIN = 24; /* GPIO 24, Physical Pin 35 */
  
  /* link integer data like a switch in the cockpit */
  int *value = link_dataref_int("sim/cockpit/electrical/landing_lights_on");
  
  /* link floating point dataref with precision 10e-1 to local variable. This means
     that we only transfer the variable if changed by 0.1 or more */
  float *fvalue = link_dataref_flt("sim/flightmodel/controls/parkbrake",-3);
 
  /* not needed, only if you run without x-plane connection */
  if (*fvalue == FLT_MISS) *fvalue = 0.0;
  if (*value == INT_MISS) *value = 1;

  pinMode(LED_PIN, OUTPUT);
  if (*value != INT_MISS) digitalWrite(LED_PIN, *value);

  pinMode(KEY_PIN, INPUT);
  pullUpDnControl(KEY_PIN, PUD_UP);
  ret = digitalRead(KEY_PIN);
  if (ret == 0) {
    printf("Key Pressed\n");
    *fvalue = 1.0;
  } else {
    *fvalue = 0.0;
  }

  
}

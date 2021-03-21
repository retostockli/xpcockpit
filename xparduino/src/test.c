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
#include "libarduino.h"
#include "serverdata.h"
#include "test.h"

int digitalvalue;

void test(void)
{

  int ret;
  int ard = 0; /* arduino number according to ini file */

  /* link integer data like a switch in the cockpit */
  int *value = link_dataref_int("sim/cockpit/electrical/landing_lights_on");
  
  /* link floating point dataref with precision 10e-1 to local variable. This means
     that we only transfer the variable if changed by 0.1 or more */
  float *fvalue = link_dataref_flt("sim/flightmodel/controls/parkbrake",-3);

  /* link NAV1 Frequency to encoder value */
  int *encodervalue = link_dataref_int("sim/cockpit/radios/nav1_freq_hz");

  /* not needed, only if you run without x-plane connection */
  if (*encodervalue == INT_MISS) *encodervalue = 0;
  if (*fvalue == FLT_MISS) *fvalue = 0.0;
  if (*value == INT_MISS) *fvalue = 1;

  /* read encoder at inputs 13 and 15 */
  ret = encoder_input(ard, 8, 9, encodervalue, 5, 1);
  if (ret == 1) {
    printf("Encoder changed to: %i \n",*encodervalue);
  }
  
  /* read digital input (#8) */  
  ret = digital_input(ard, 8, &digitalvalue, 0);
  if (ret == 1) {
    printf("Digital Input changed to: %i \n",digitalvalue);
  } 

  /* read first analog input (#0) */
  ret = analog_input(ard,0,fvalue,0.0,1.0);
  if (ret == 1) {
    /* ret is 1 only if analog input has changed */
    printf("Analog Input changed to: %f \n",*fvalue);
  }
  
  /* set LED connected to second output (#1) to value landing lights dataref */
  ret = digital_output(ard, 1, value);
  
}

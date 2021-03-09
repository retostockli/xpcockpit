/* This is the test.c code which can be used to test basic I/O of the SISMO Master and Daughter
   ards

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

int encodervalue;

void test(void)
{

  int ret;
  int ard = 0;

  /* link integer data like a switch in the cockpit */
  int *value = link_dataref_int("sim/cockpit/electrical/landing_lights_on");
  
  /* link floating point dataref with precision 10e-1 to local variable. This means
     that we only transfer the variable if changed by 0.1 or more */
  float *fvalue = link_dataref_flt("sim/flightmodel/controls/parkbrake",-3);

  /* link NAV1 Frequency to encoder value */
  int *encodervalue = link_dataref_int("sim/cockpit/radios/nav1_freq_hz");

  if (*fvalue == FLT_MISS) *fvalue = 0.0;
  
  /* read second digital input (#1) */
  ret = digital_input(ard, 1, value, 0);
  if (ret == 1) {
    /* ret is 1 only if input has changed */
    printf("Digital Input changed to: %i \n",*value);
  }

  /* read first analog input (#0) */
  ret = analog_input(ard,0,fvalue,0.0,1.0);
  if (ret == 1) {
    /* ret is 1 only if analog input has changed */
    //    printf("Analog Input changed to: %f \n",*fvalue);
  }

  float degrees = *fvalue * 360.0;
  int y = (int) 255.0 * cos(degrees*3.14/180.0);
  int x = (int) 255.0 * sin(degrees*3.14/180.0);
  int absx = abs(x);
  int absy = abs(y);
  int revabsx = 255-abs(x);
  int revabsy = 255-abs(y);
  int zero = 0;
  int one = 1;
  printf("%f %i %i \n",degrees,x,y);

  if (x > 0) {
    ret = analog_output(ard,5,&absx);
    ret = digital_output(ard,3,&zero);
  } else {
    ret = analog_output(ard,5,&revabsx);
    ret = digital_output(ard,3,&one);
  }
  
  if (y > 0) {
    ret = analog_output(ard,6,&absy);
    ret = digital_output(ard,7,&zero);
  } else {
    ret = analog_output(ard,6,&revabsy);
    ret = digital_output(ard,7,&one);
  }
  
  /* read encoder at inputs 13 and 15 */
  /*
  ret = encoder_input(ard, 13, 15, encodervalue, 5, 1);
  if (ret == 1) {
    printf("Encoder changed to: %i \n",*encodervalue);
    }*/
  
  /* set LED connected to second output (#1) to value of above input */
  *value = 1;
  ret = digital_output(ard, 2, value);
 
  *value = 200;
  ret = analog_output(ard, 3, value);
 
}

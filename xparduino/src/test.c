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

int digitalvalue;

static float y[22] = {-70,-40,-15,  0, 25, 35, 40, 45, 60, 90,120,150,180,210,225,230,235,250,290,320,345,360}; /* Input Degrees */
static float x[22] = {-10,  8, 26, 38, 60, 75, 85,100,125,150,170,180,205,230,250,267,295,317,350,368,386,398}; /* Degrees on Compass */

float interpolate(float xval)
{
  int left;
  int right;
  float val;
  int n=sizeof(x)/sizeof(float);

  left = n-2;
  right = n-1;
  for (int i=1;i<(n-1);i++) {
    if (xval < x[i]) {
      left = i-1;
      right = i;
      break;
    }
  }

  if (xval < x[0]) {
    val = y[0];
  } else if (xval > x[n-1]) {
    val = y[n-1];
  } else {
    val = ((x[right] - xval) * y[left] + (xval - x[left]) * y[right]) / (x[right] - x[left]);
  }
  
  //  printf("%f %i %i %f \n",xval,left,right,val);

  return val;
}

void test(void)
{

  int ret;
  int ard = 0;
  //  int input;

  /* link integer data like a switch in the cockpit */
  int *value = link_dataref_int("sim/cockpit/electrical/landing_lights_on");
  
  /* link floating point dataref with precision 10e-1 to local variable. This means
     that we only transfer the variable if changed by 0.1 or more */
  float *fvalue = link_dataref_flt("sim/flightmodel/controls/parkbrake",-3);

  /* link NAV1 Frequency to encoder value */
  int *encodervalue = link_dataref_int("sim/cockpit/radios/nav1_freq_hz");

  if (*encodervalue == INT_MISS) *encodervalue = 0;
  if (*fvalue == FLT_MISS) *fvalue = 0.0;
  
  /* read encoder at inputs 13 and 15 */
  ret = encoder_input(ard, 8, 9, encodervalue, 5, 2);
  if (ret == 1) {
    if (*encodervalue > 360) *encodervalue -= 360;
    if (*encodervalue < 0) *encodervalue += 360;
    printf("Encoder changed to: %i \n",*encodervalue);
  }
  
  /* read digital input (#8) */
  /*
  input = 8;
  ret = digital_input(ard, input, &digitalvalue, 0);
  if (ret == 1) {
    printf("Digital Input %i changed to: %i \n",input,digitalvalue);
    } */


  /* read first analog input (#0) */
  ret = analog_input(ard,0,fvalue,0.0,1.0);
  if (ret == 1) {
    /* ret is 1 only if analog input has changed */
    printf("Analog Input changed to: %f \n",*fvalue);
  }

  float degrees=interpolate((float) *encodervalue);
  //  float degrees = (float) *encodervalue;
  int y = (int) 255.0 * cos(degrees*3.14/180.0);
  int x = (int) 255.0 * sin(degrees*3.14/180.0);
  int absx = abs(x);
  int absy = abs(y);
  int revabsx = 255-abs(x);
  int revabsy = 255-abs(y);
  int zero = 0;
  int one = 1;

  //ret = compass_output(ard,&degrees);
  
  // printf("%f %i %i \n",degrees,x,y);

  if (x > 0) {
    ret = analog_output(ard,5,&absx);
    ret = digital_output(ard,3,&zero);
  } else {
    ret = analog_output(ard,5,&revabsx);
    ret = digital_output(ard,3,&one);
  }
  
  if (y < 0) {
    ret = analog_output(ard,6,&absy);
    ret = digital_output(ard,7,&zero);
  } else {
    ret = analog_output(ard,6,&revabsy);
    ret = digital_output(ard,7,&one);
  }
  
  
  
  /* set LED connected to second output (#1) to value of above input */
  *value = 1;
  ret = digital_output(ard, 2, value);
  
}

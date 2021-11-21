/* This is the compass.c code which drives the analog compass by use of
   a H-Bridge and 2 PWM signals

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

int LIGHT_PIN = 8; /* GPIO 2, Physical Pin 3 */
int A1A_PIN = 9;   /* GPIO 3, Physical Pin 5 */
int A1B_PIN = 7;   /* GPIO 4, Physical Pin 7 */
int B1A_PIN = 15;  /* GPIO 14, Physical Pin 8 */
int B1B_PIN = 16;  /* GPIO 15, Physical Pin 10 */

/* This is the translation table of magnetic deg input (x) to degees shown on compass (y) */
/* Since the coils on the compass are not exactly orthogonal and the magnetic field is not perfect */
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

int compass_init(void) {

  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(A1A_PIN, OUTPUT);
  pinMode(A1B_PIN, OUTPUT);
  pinMode(B1A_PIN, OUTPUT);
  pinMode(B1B_PIN, OUTPUT);
  softPwmCreate(A1A_PIN,0,255); //Pin,initalValue,pwmRange    
  softPwmCreate(B1A_PIN,0,255); //Pin,initalValue,pwmRange    

  return 0;
  
}

void compass(void)
{

  int calibrate = 0;  /* set to 1 in order to create a new x/y calibration table (see above) */
  float compassdegrees;
 
  /* link integer data like a switch in the cockpit */
  int *value = link_dataref_int("sim/cockpit/electrical/landing_lights_on");
  
 
  /* not needed, only if you run without x-plane connection */
  if (*value == INT_MISS) *value = 1;

  if (*value != INT_MISS) {
    digitalWrite(LIGHT_PIN, *value);
  }

  //  float *heading_mag = link_dataref_flt("sim/flightmodel/position/magpsi",-1);
  float *heading_mag = link_dataref_flt("sim/cockpit/autopilot/heading_mag",0);      
  
  /* read encoder at inputs 13 and 15 */
  if (*heading_mag == FLT_MISS) *heading_mag = 0.0;

  if (calibrate) {
    compassdegrees = *heading_mag;
  } else {
    compassdegrees = interpolate(*heading_mag);
  }
  int y = (int) 255.0 * cos(compassdegrees*3.14/180.0);
  int x = (int) 255.0 * sin(compassdegrees*3.14/180.0);
  int absx = abs(x);
  int absy = abs(y);
  int revabsx = 255-abs(x);
  int revabsy = 255-abs(y);
  int zero = 0;
  int one = 1;

  if (x > 0) {
    softPwmWrite(A1A_PIN, absx);
    digitalWrite(A1B_PIN, zero);
  } else {
    softPwmWrite(A1A_PIN, revabsx);
    digitalWrite(A1B_PIN, one);
  }
  
  if (y < 0) {
    softPwmWrite(B1A_PIN, absy);
    digitalWrite(B1B_PIN, zero);
  } else {
    softPwmWrite(B1A_PIN, revabsy);
    digitalWrite(B1B_PIN, one);
  }
    
}

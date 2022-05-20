/* This is the B737 compass.c code which drives a wet compass attached with 2 coils through
   a L9110 motor H-Bridge and 2 PWM signals

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
#include "b737_compass.h"


int LIGHT_PIN = 2; /* GPIO 2, Physical Pin 3 */
int A1A_PIN = 3;   /* GPIO 3, Physical Pin 5 */
int A1B_PIN = 4;   /* GPIO 4, Physical Pin 7 */
int B1A_PIN = 14;  /* GPIO 14, Physical Pin 8 */
int B1B_PIN = 15;  /* GPIO 15, Physical Pin 10 */
int COMPASS_PWM_RANGE = 360;
int COMPASS_PWM_FREQ = 40;

/* This is the translation table of magnetic deg input (x) to degees shown on compass (y) */
/* Since the coils on the compass are not exactly orthogonal and the magnetic field is not perfect */

static float y[22] = { 45, 60, 90,110,140,146,148,153,161,171,180,215,245,276,310,330,350,360,375,390,405,420}; /* Input Degrees */
static float x[22] = { -2,  7, 25, 40, 75,105,110,120,130,140,150,177,192,225,277,312,328,335,345,350,358,367}; /* Degrees on Compass NEED TO COVER 0..360 degrees at least */

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

int b737_compass_init(void) {

  /* we need to use both L9110 pins as PWM for each motor since the generation of the 
     PWM signal has a minimum duty cycle which prevents a near 0 PWM frequency. So we
     start from maximum PWM for positive and negative motor directions */
  
#ifdef PIGPIO
  gpioSetMode(LIGHT_PIN, PI_OUTPUT);
  gpioSetMode(A1A_PIN, PI_OUTPUT);
  gpioSetMode(A1B_PIN, PI_OUTPUT);
  gpioSetMode(B1A_PIN, PI_OUTPUT);
  gpioSetMode(B1B_PIN, PI_OUTPUT);
  gpioSetPWMrange(A1A_PIN,COMPASS_PWM_RANGE);
  gpioSetPWMrange(A1B_PIN,COMPASS_PWM_RANGE);
  gpioSetPWMrange(B1A_PIN,COMPASS_PWM_RANGE);
  gpioSetPWMrange(B1B_PIN,COMPASS_PWM_RANGE);
  gpioSetPWMfrequency(A1A_PIN,COMPASS_PWM_FREQ);
  gpioSetPWMfrequency(A1B_PIN,COMPASS_PWM_FREQ);
  gpioSetPWMfrequency(B1A_PIN,COMPASS_PWM_FREQ);
  gpioSetPWMfrequency(B1B_PIN,COMPASS_PWM_FREQ);
  gpioPWM(A1A_PIN, 0);
  gpioPWM(A1B_PIN, 0);
  gpioPWM(B1A_PIN, 0);
  gpioPWM(B1B_PIN, 0);
#else
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(A1A_PIN, OUTPUT);
  pinMode(A1B_PIN, OUTPUT);
  pinMode(B1A_PIN, OUTPUT);
  pinMode(B1B_PIN, OUTPUT);
  softPwmCreate(A1A_PIN,0,COMPASS_PWM_RANGE); //Pin,initalValue,pwmRange    
  softPwmCreate(A1B_PIN,0,COMPASS_PWM_RANGE); //Pin,initalValue,pwmRange    
  softPwmCreate(B1A_PIN,0,COMPASS_PWM_RANGE); //Pin,initalValue,pwmRange    
  softPwmCreate(B1B_PIN,0,COMPASS_PWM_RANGE); //Pin,initalValue,pwmRange    
#endif
  
  return 0;
  
}

void b737_compass(void)
{

  int calibrate = 0;  /* set to 1 in order to create a new x/y calibration table (see above) */
  float compassdegrees;

  int ret;

  int zero = 0;
  int one = 1;
  
  /* link integer data like a switch in the cockpit */
  //  int *value = link_dataref_int("sim/cockpit/electrical/landing_lights_on");
  int *value = link_dataref_int("sim/cockpit/electrical/avionics_on");
  
 
  /* not needed, only if you run without x-plane connection */
  if (*value == INT_MISS) *value = 1;

  if (*value != INT_MISS) {
#ifdef PIGPIO
    gpioWrite(LIGHT_PIN, *value);
#else
    digitalWrite(LIGHT_PIN, *value);
#endif
  }
  
  float *heading_mag;
  if (calibrate) {
    heading_mag = link_dataref_flt("sim/cockpit/autopilot/heading_mag",0);
  } else {
    heading_mag = link_dataref_flt("sim/flightmodel/position/magpsi",-1);
  }
    
  if (*heading_mag == FLT_MISS) *heading_mag = 0.0;

  if (calibrate) {
    compassdegrees = *heading_mag;
  } else {
    compassdegrees = interpolate(*heading_mag);
  }
  int y = (int) (((float) COMPASS_PWM_RANGE) * cos(compassdegrees*3.14/180.0));
  int x = (int) (((float) COMPASS_PWM_RANGE) * sin(compassdegrees*3.14/180.0));
  int absx = abs(x);
  int absy = abs(y);
  int revabsx = COMPASS_PWM_RANGE-abs(x);
  int revabsy = COMPASS_PWM_RANGE-abs(y);

#ifdef PIGPIO
  if (x < 0) {
    gpioPWM(A1A_PIN, COMPASS_PWM_RANGE);
    gpioPWM(A1B_PIN, revabsx);
  } else {
    gpioPWM(A1A_PIN, revabsx);
    gpioPWM(A1B_PIN, COMPASS_PWM_RANGE);
  }  
  if (y > 0) {
    gpioPWM(B1A_PIN, COMPASS_PWM_RANGE);
    gpioPWM(B1B_PIN, revabsy);
  } else {
    gpioPWM(B1A_PIN, revabsy);
    gpioPWM(B1B_PIN, COMPASS_PWM_RANGE);
  }
#else
  if (x < 0) {
    softPwmWrite(A1A_PIN, COMPASS_PWM_RANGE);
    softPwmWrite(A1B_PIN, revabsx);
  } else {
    softPwmWrite(A1A_PIN, revabsx);
    softPwmWrite(A1B_PIN, COMPASS_PWM_RANGE);
  }  
  if (y > 0) {
    softPwmWrite(B1A_PIN, COMPASS_PWM_RANGE);
    softPwmWrite(B1B_PIN, revabsy);
  } else {
    softPwmWrite(B1A_PIN, revabsy);
    softPwmWrite(B1B_PIN, COMPASS_PWM_RANGE);
  }
#endif
  
}

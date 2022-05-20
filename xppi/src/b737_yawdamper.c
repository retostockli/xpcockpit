/* This is the B737 yawdamper.c code which drives the original B737 yaw damper which uses a coil. 
   This coil is driven through a L9110 motor H-Bridge and 1 PWM signal

   Copyright (C) 2022 Reto Stockli

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
#include "b737_yawdamper.h"


int A1_PIN = 8;   /* Physical Pin 24 */
int B1_PIN = 11;   /* Physical Pin 23 */
int YAW_LED_PIN = 7; /* Physical Pin 26 */
int YAW_PWM_RANGE = 360;
int YAW_PWM_FREQ = 40;

int b737_yawdamper_init(void) {

  int ret;
  
#ifdef PIGPIO
  gpioSetMode(YAW_LED_PIN, PI_OUTPUT);
  gpioSetMode(A1_PIN, PI_OUTPUT);
  gpioSetMode(B1_PIN, PI_OUTPUT);
  gpioSetPWMrange(A1_PIN,YAW_PWM_RANGE);
  gpioSetPWMrange(B1_PIN,YAW_PWM_RANGE);
  gpioSetPWMfrequency(A1_PIN,YAW_PWM_FREQ);
  gpioSetPWMfrequency(B1_PIN,YAW_PWM_FREQ);
  gpioPWM(A1_PIN, 0);
  gpioPWM(B1_PIN, 0);
#else
  pinMode(YAW_LED_PIN, OUTPUT);
  pinMode(A1_PIN, OUTPUT);
  pinMode(B1_PIN, OUTPUT);
  softPwmCreate(A1_PIN,0,YAW_PWM_RANGE);     
  softPwmCreate(B1_PIN,0,YAW_PWM_RANGE);     
#endif
  
  return 0;
  
}

void b737_yawdamper(void)
{

  /* we need to use both L9110 pins as PWM for each motor since the generation of the 
     PWM signal has a minimum duty cycle which prevents a near 0 PWM frequency. So we
     start from maximum PWM for positive and negative motor directions */

  int zero = 0;
  int one = 1;
  int ret;
  int x;
 
  /* link integer data like a switch in the cockpit */
  //  int *value = link_dataref_int("sim/cockpit/electrical/landing_lights_on");
  int *value = link_dataref_int("sim/cockpit/electrical/avionics_on");
  
 
  /* not needed, only if you run without x-plane connection */
  if (*value == INT_MISS) *value = 1;

  if (*value != INT_MISS) {
#ifdef PIGPIO
    gpioWrite(YAW_LED_PIN, *value);
#else
    digitalWrite(YAW_LED_PIN, *value);
#endif
  }
  
  float *yaw_damper;
  if (acf_type == 3) {
    yaw_damper = link_dataref_flt("laminar/B738/gauges/yd_indicator",-2);
  } else {
    /* for testing in other acf: link to autopilot heading indicator */
    yaw_damper = link_dataref_flt("sim/cockpit/autopilot/heading_mag",0);
  }

  if (*yaw_damper == FLT_MISS) {
    x = 0;
  } else {
    if (acf_type == 3) {
      x = (int) ((float) YAW_PWM_RANGE) * *yaw_damper;
    } else {
      x = (int) (((float) YAW_PWM_RANGE) * (*yaw_damper-180.0)/180.0);
    }
  }
  int absx = abs(x);
  int revabsx = YAW_PWM_RANGE-abs(x);
    
#ifdef PIGPIO
  if (x < 0) {
    gpioPWM(A1_PIN, YAW_PWM_RANGE);
    gpioPWM(B1_PIN, revabsx);
  } else {
    gpioPWM(A1_PIN, revabsx);
    gpioPWM(B1_PIN, YAW_PWM_RANGE);
  }
#else
  if (x < 0) {
    softPwmWrite(A1_PIN, YAW_PWM_RANGE);
    softPwmWrite(B1_PIN, revabsx);
  } else {
    softPwmWrite(A1_PIN, revabsx);
    softPwmWrite(B1_PIN, YAW_PWM_RANGE);
  }  
#endif
    
}

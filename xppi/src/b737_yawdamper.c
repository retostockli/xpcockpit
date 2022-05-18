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


int A1_PIN = 2;   /* Physical Pin 3 */
int B1_PIN = 3;   /* Physical Pin 5 */
int YAW_LED_PIN = 4; /* Physical Pin 7 */

int b737_yawdamper_init(void) {
  
#ifdef PIGPIO
  gpioSetMode(YAW_LED_PIN, PI_OUTPUT);
  gpioSetMode(A1_PIN, PI_OUTPUT);
  gpioSetMode(B1_PIN, PI_OUTPUT);
  gpioSetPWMrange(A1_PIN,255);     
#else
  pinMode(YAW_LED_PIN, OUTPUT);
  pinMode(A1_PIN, OUTPUT);
  pinMode(B1_PIN, OUTPUT);
  softPwmCreate(A1_PIN,0,255);     
#endif
  
  return 0;
  
}

void b737_yawdamper(void)
{


  int ret;
  
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
  
  float *heading_mag = link_dataref_flt("sim/cockpit/autopilot/heading_mag",0);
    
  if (*heading_mag == FLT_MISS) *heading_mag = 180.0;

  int x = (int) 255.0 * (*heading_mag-180.0)/180.0;
  int absx = abs(x);
  int revabsx = 255-abs(x);
  int zero = 0;
  int one = 1;

#ifdef PIGPIO
  if (x < 0) {
    gpioPWM(A1_PIN, absx);
    gpioWrite(B1_PIN, zero);
  } else {
    gpioPWM(A1_PIN, revabsx);
    gpioWrite(B1_PIN, one);
  }
#else
  if (x < 0) {
    softPwmWrite(A1_PIN, absx);
    digitalWrite(B1_PIN, zero);
  } else {
    softPwmWrite(A1_PIN, revabsx);
    digitalWrite(B1_PIN, one);
  }
#endif
    
}

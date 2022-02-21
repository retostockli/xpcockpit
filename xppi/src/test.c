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

//int PWM_PIN = 24; /* GPIO 10, Physical Pin 35 */
//int LED_PIN = 13; /* GPIO 9, Physical Pin 21 */
//int KEY_INPUT_PIN = 29;	/* p.40 GPIO 21 Physical Pin 40 */
//int KEY_OUTPUT_PIN = 15; /* GPIO 14 Physical Pin 8 */

int PWM_PIN = 19; /* GPIO 10, Physical Pin 35 */
int LED_PIN = 9; /* GPIO 9, Physical Pin 21 */
int KEY_INPUT_PIN = 21;	/* GPIO 21 Physical Pin 40 */
int KEY_OUTPUT_PIN = 14; /* GPIO 14 Physical Pin 8 */

int test_init(void) {

#ifdef PIGPIO
  gpioSetMode(PWM_PIN, PI_OUTPUT);
  gpioSetPWMrange(PWM_PIN,100); //Pin,initalValue,pwmRange    
  gpioSetMode(KEY_INPUT_PIN, PI_INPUT);
  gpioSetPullUpDown(KEY_INPUT_PIN, PI_PUD_UP);
  gpioSetMode(KEY_OUTPUT_PIN, PI_OUTPUT);
  gpioSetMode(LED_PIN, PI_OUTPUT);

  gpioWrite(KEY_OUTPUT_PIN,0);

  gpioPWM(PWM_PIN, 50);
#else
  pinMode(PWM_PIN, OUTPUT);
  softPwmCreate(PWM_PIN,0,100); //Pin,initalValue,pwmRange    
  pinMode(KEY_INPUT_PIN, INPUT);
  pullUpDnControl(KEY_INPUT_PIN, PUD_UP);
  pinMode(KEY_OUTPUT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(KEY_OUTPUT_PIN,0);

  softPwmWrite(PWM_PIN, 50);
#endif
  
  return 0;
  
}

void test(void)
{

  int ret;
  
  /* link integer data like a switch in the cockpit */
  int *value = link_dataref_int("sim/cockpit/electrical/landing_lights_on");
  
  /* link floating point dataref with precision 10e-1 to local variable. This means
     that we only transfer the variable if changed by 0.1 or more */
  float *fvalue = link_dataref_flt("sim/flightmodel/controls/parkbrake",-3);
 
  /* not needed, only if you run without x-plane connection */
  if (*fvalue == FLT_MISS) *fvalue = 0.0;
  if (*value == INT_MISS) *value = 1;

  if (*value != INT_MISS) {
  }

#ifdef PIGPIO
  ret = gpioRead(KEY_INPUT_PIN);
#else
  ret = digitalRead(KEY_INPUT_PIN);
#endif
  if (ret == 0) {
    printf("Key Pressed\n");
#ifdef PIGPIO
    gpioWrite(LED_PIN,1);
#else
    digitalWrite(LED_PIN,1);
#endif
    *fvalue = 1.0;
  } else {
#ifdef PIGPIO
    gpioWrite(LED_PIN,0);
#else
    digitalWrite(LED_PIN,0);
#endif
    *fvalue = 0.0;
  }

  
}

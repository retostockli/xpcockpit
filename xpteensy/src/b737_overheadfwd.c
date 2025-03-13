/* This is the b737_overheadfwd.c code which simulates all I/O to the Boeing 737 overhead panel
   using Teensy microcontrollers

   Copyright (C) 2025-2025 Reto Stockli

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
#include "libteensy.h"
#include "serverdata.h"
#include "test.h"

void init_b737_overheadfwd(void)
{
  int te = 0;
  int pin;
  int dev;

  /*
  for (int i=0;i<42;i++) {
    teensy[te].pinmode[i] = PINMODE_INPUT;
  }
  */
  
  /* teensy[te].pinmode[37] = PINMODE_PWM; */
  /* //  teensy[te].pinmode[0] = PINMODE_OUTPUT; */
  /* teensy[te].pinmode[3] = PINMODE_INPUT; */
  /* teensy[te].pinmode[4] = PINMODE_INPUT; */
  /* //  teensy[te].pinmode[5] = PINMODE_INPUT; */
  /* teensy[te].pinmode[41] = PINMODE_ANALOGINPUTMEAN  ; */
  /* //  teensy[te].pinmode[6] = PINMODE_INTERRUPT; */
  /* //  teensy[te].pinmode[7] = PINMODE_INTERRUPT; */
  /* teensy[te].pinmode[14] = PINMODE_ANALOGINPUTMEAN; */
  /* teensy[te].pinmode[16] = PINMODE_I2C; */
  /* teensy[te].pinmode[17] = PINMODE_I2C; */
  /* teensy[te].pinmode[23] = PINMODE_OUTPUT; */
  /* teensy[te].pinmode[30] = PINMODE_INPUT; // direction */
  /* teensy[te].pinmode[31] = PINMODE_INPUT; // brake */
  /* teensy[te].pinmode[33] = PINMODE_MOTOR; // motor EN */
  /* teensy[te].arg1[33] = 34; // motor IN1 */
  /* teensy[te].arg2[33] = 35; // motor IN1 */
  /* teensy[te].arg3[33] = 38; // motor Current Sense */
  
  /* teensy[te].pinmode[24] = PINMODE_MOTOR; // motor EN */
  /* teensy[te].arg1[24] = 26; // motor IN1 */
  /* teensy[te].arg2[24] = 25; // motor IN1 */
  //teensy[te].arg3[24] = 38; // motor Current Sense

  //teensy[te].pinmode[38] = PINMODE_ANALOGINPUTMEAN;
  
   

  dev = 0;
  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 8; // also define pin 6 of teensy as INTERRUPT above!
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x20; // I2C address of MCP23017 device
  
  dev = 1;
  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  mcp23017[te][dev].intpin = INITVAL; // also define pin 6 of teensy as INTERRUPT above!
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x21; // I2C address of MCP23017 device
  

  /* mcp23017[te][1].pinmode[2] = PINMODE_INPUT; */
  /* mcp23017[te][1].pinmode[3] = PINMODE_INPUT; */
  /* mcp23017[te][1].pinmode[4] = PINMODE_INPUT; */
  /* mcp23017[te][1].pinmode[10] = PINMODE_INPUT; */
  /* mcp23017[te][1].pinmode[11] = PINMODE_INPUT; */
  /* mcp23017[te][1].pinmode[12] = PINMODE_INPUT; */
  /* mcp23017[te][1].pinmode[5] = PINMODE_OUTPUT; */
  /* mcp23017[te][1].pinmode[7] = PINMODE_OUTPUT; */
  /* mcp23017[te][1].intpin = 7; // also define pin 6 of teensy as INTERRUPT above! */
  /* mcp23017[te][1].wire = 0;  // I2C Bus: 0, 1 or 2 */
  /* mcp23017[te][1].address = 0x20; // I2C address of MCP23017 device */


  /* pca9685[te][0].pinmode[0] = PINMODE_PWM; */
  /* //pca9685[te][0].pinmode[2] = PINMODE_PWM; */
  /* pca9685[te][0].wire = 0; */
  /* pca9685[te][0].address = 0x40; */


  /* ht16k33[te][0].brightness = 10; */
  /* ht16k33[te][0].wire = 0; */
  /* ht16k33[te][0].address = 0x70; */

  
}

void b737_overheadfwd(void)
{

  int ret;
  int te = 0;
  int pin;
  int dev;

  int inputvalue;

  /* link integer data like a switch in the cockpit */
  //int *value = link_dataref_int("sim/cockpit/electrical/landing_lights_on");
  int *value = link_dataref_int("xpserver/digitalvalue");

  //if (*fvalue == FLT_MISS) *fvalue = 0.0;
  if (*value == INT_MISS) *value = 1;

  /* /\* read analog input (#14) *\/ */
  /* ret = analog_input(te,41,fvalue,0.0,1023.0); */
  /* if (ret == 1) { */
  /*   printf("Analog Input changed to: %f \n",*fvalue); */
  /* } */

  //float *wind_speed = link_dataref_flt("sim/cockpit2/gauges/indicators/wind_speed_kts",0);
  //float *time = link_dataref_flt("sim/time/framerate_period",-3);
  
  dev = 0;
  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
    ret = digital_input(te, MCP23017_TYPE, dev, pin, &inputvalue, 0);
    if (ret == 1) {
      printf("Digital Input %i changed to: %i \n", pin, inputvalue);
    }
  }


  /* ret = digital_input(te, TEENSY_TYPE, 0, 30, &direction, 0); */
  /* if (ret == 1) { */
  /*   printf("Motor Direction changed to: %i \n",direction); */
  /* } */

  /*
  ret = digital_input(te, TEENSY_TYPE, 0, 31, &brake, 0);
  if (ret == 1) {
    printf("Motor Brake changed to: %i \n",brake);
  }
  */


  dev = 1;
  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
    ret = digital_output(te, MCP23017_TYPE, dev, pin, value);
  }
  //ret = pwm_output(te, TEENSY_TYPE, 0, 37, fvalue,5.0,1023.0);

  /* change Servo according to rotary position */
  //ret = servo_output(te, TEENSY_TYPE, 0, 23, fvalue,0.0,1.0,0.2,0.8);
  //ret = servo_output(te, PCA9685_TYPE, 0, 0, fvalue,0.0,1.0,0.2,0.8);

  /* *fvalue = 1.0; */
  /* ret = pwm_output(te, PCA9685_TYPE, 0, 0, fvalue,0.0,1.0); */

  /* *value = 12345; */
  /* int dp = -1; */
  /* int brightness = 8; */
  
  /* ret = display_output(te, HT16K33_TYPE, 0, 8, 5, value, dp, brightness); */
  
}

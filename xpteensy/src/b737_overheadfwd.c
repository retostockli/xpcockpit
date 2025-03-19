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

  /* Controlling Solid State Relays (SSR) */
  /* 26: On/Off Pedestal Light */
  /* 27: On/Off Background Light */
  /* 29: On/Off Yaw Damper Coil */
  /* 30: On/Off Engine 1 Start Switch Coil */
  /* 31: On/Off Engine 2 Start Switch Coil */
  for (pin=26;pin<34;pin++) {
    if (pin != 28) {
      teensy[te].pinmode[pin] = PINMODE_OUTPUT;
    }
  }
  // Controls Overhead-to-Pedestal Light Intensity from Pedestal Potentiometer
  teensy[te].pinmode[28] = PINMODE_PWM; 

  dev = 0;
  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 0; // also define pin 6 of teensy as INTERRUPT above!
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x20; // I2C address of MCP23017 device
  
  dev = 1;
  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  mcp23017[te][dev].intpin = INITVAL; // also define pin 6 of teensy as INTERRUPT above!
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x21; // I2C address of MCP23017 device

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

  int one = 1;
  int zero = 0;

  int inputvalue;

  /* link integer data like a switch in the cockpit */
  int *value = link_dataref_int("xpserver/digitalvalue");
  float *fvalue = link_dataref_flt("xpserver/analogvalue",0);

  if (*fvalue == FLT_MISS) *fvalue = 0.0;
  if (*value == INT_MISS) *value = 1;

  /* /\* read analog input (#14) *\/ */
  /* ret = analog_input(te,41,fvalue,0.0,1023.0); */
  /* if (ret == 1) { */
  /*   printf("Analog Input changed to: %f \n",*fvalue); */
  /* } */
 
  dev = 0;
  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
    ret = digital_input(te, MCP23017_TYPE, dev, pin, &inputvalue, 0);
    if (ret == 1) {
      printf("Digital Input %i changed to: %i \n", pin, inputvalue);
    }
  }


  /* Pedestal Light PWM controlled by Pedestal Potentiometer */
  *fvalue = 0.5;
  ret = digital_output(te, TEENSY_TYPE, 0, 26, &one);
  ret = pwm_output(te, TEENSY_TYPE, 0, 28, fvalue,0.0,1.0);

  /* Background Lighting (Direct Potentiometer MOSFET control */
  ret = digital_output(te, TEENSY_TYPE, 0, 27, &one);

  /* Yaw Damper Coil */
  ret = digital_output(te, TEENSY_TYPE, 0, 29, &zero);

  *value = 0;
  dev = 1;
  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
    ret = digital_output(te, MCP23017_TYPE, dev, pin, value);
  }

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

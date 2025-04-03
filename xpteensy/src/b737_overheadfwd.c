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

/*
  LTC4316 Address Translator
  Switch		A4	A5
  XOR 0x40       	ON	ON
  XOR 0x50       	OFF	ON
  XOR 0x60       	ON	OFF
  XOR 0x70       	OFF 	OFF
*/

/* permanent storage */
float dc_power_pos;
float ac_power_pos;
float l_eng_start_pos;
float r_eng_start_pos;
float air_valve_ctrl_pos;
float l_wiper_pos;
float r_wiper_pos;
float vhf_nav_source_pos;
float irs_source_pos;
float display_source_pos;
float control_panel_source_pos;
float flt_ctrl_A_pos;
float flt_ctrl_B_pos;

int smoking1;
int smoking2;

int count;

void init_b737_overheadfwd(void)
{
  int te = 0;
  int pin;
  int dev;

  /* ----------- */
  /* TEENSY HOST */
  /* ----------- */
  
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

  /* Analog Inputs on Teensy:
     38 (A14) CONT CAB
     39 (A15) FWD CAB
     40 (A16) AFT CAB
     41 (A17) CIRCUIT BREAKER POT */
  for (pin=38;pin<42;pin++) {
    teensy[te].pinmode[pin] = PINMODE_ANALOGINPUTMEDIAN;
  }

  /* ---------- */
  /* SERVO CARD */
  /* ---------- */
  
  dev = 0;
  for (pin=0;pin<PCA9685_MAX_PINS;pin++) {
    pca9685[te][dev].pinmode[pin] = PINMODE_SERVO;
  }
  pca9685[te][dev].wire = 0;
  pca9685[te][dev].address = 0x40;

  /* -------- */
  /* PWM CARD */
  /* -------- */
  
  dev = 1;
  for (pin=0;pin<PCA9685_MAX_PINS;pin++) {
    pca9685[te][dev].pinmode[pin] = PINMODE_PWM;
  }
  pca9685[te][dev].wire = 0;
  pca9685[te][dev].address = 0x41;

  /* -------------- */
  /* DISPLAY CARD 1 */
  /* -------------- */
  
  /* ht16k33[te][0].brightness = 10; */
  /* ht16k33[te][0].wire = 0; */
  /* ht16k33[te][0].address = 0x70; */
    
  /* -------------- */
  /* DISPLAY CARD 2 */
  /* -------------- */
  
  /* ht16k33[te][0].brightness = 10; */
  /* ht16k33[te][0].wire = 0; */
  /* ht16k33[te][0].address = 0x70; */
    
  /* ---- -----------  */
  /* FLT CONTROL PANEL */
  /* ----------------- */
  
  dev = 0;
  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 0; // also define pin 6 of teensy as INTERRUPT above!
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x20 ^ 0x70; // I2C address of MCP23017 device

  dev = 1;
  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  mcp23017[te][dev].intpin = INITVAL; // also define pin 6 of teensy as INTERRUPT above!
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x21 ^ 0x70; // I2C address of MCP23017 device

  /* ------------------ */
  /* TEMP CONTROL PANEL */
  /* ------------------ */
  
  dev = 2;  // CHANGE ME
  for (pin=0;pin<8;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  for (pin=8;pin<11;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  mcp23017[te][dev].intpin = 16; // also define pin 6 of teensy as INTERRUPT above!
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x20 ^ 0x40; // I2C address of MCP23017 device
 
}

void b737_overheadfwd(void)
{

  int ret;
  int te = 0;
  int pin;
  int dev;

  int one = 1;
  int zero = 0;

  int ival;
  int ival2;
  float fval;

  /* Switch Covers and related switch states*/
  char buffer[100];
  int cover;
  int ncover;
 
  int inputvalue;


  /* link integer data like a switch in the cockpit */
  int *value = link_dataref_int("xpserver/digitalvalue");
  float *fvalue = link_dataref_flt("xpserver/analogvalue",0);

  if (*fvalue == FLT_MISS) *fvalue = 0.0;
  if (*value == INT_MISS) *value = 1;

  /* only run for Laminar 737 or ZIBO 737 */
  if ((acf_type == 2) || (acf_type == 3)) {
    
    float *lights_test = link_dataref_flt("laminar/B738/annunciator/test",-1);
    int *avionics_on = link_dataref_int("sim/cockpit2/switches/avionics_power_on");


    /* ------------- */
    /* SWITCH COVERS */
    /* ------------- */
   
    ncover = 11;
    int *switch_cover_toggle;
    float *switch_cover_pos = link_dataref_flt_arr("laminar/B738/button_switch/cover_position",ncover,-1,-3);
    /* open all covers */
    for (cover=0;cover<ncover;cover++) {
      sprintf(buffer, "laminar/B738/button_switch_cover%02d", cover);
      switch_cover_toggle = link_dataref_cmd_once(buffer);
      ival = 1;
      ret = set_switch_cover(switch_cover_pos+cover,switch_cover_toggle,ival);     
    }

    ncover = 5;
    /* set second datarefs to switch cover positions */
    char *switch_cover_pos_name[5] = {"laminar/B738/switches/alt_flaps_cover_pos",
				      "laminar/B738/switches/flt_ctr_A_cover_pos",
				      "laminar/B738/switches/flt_ctr_B_cover_pos",
				      "laminar/B738/switches/spoiler_A_cover_pos",
				      "laminar/B738/switches/spoiler_B_cover_pos"};
    /* set second datarefs to switch ocver commands */
    char *switch_cover_toggle_name[5] = {"laminar/B738/toggle_switch/alt_flaps_cover",
					 "laminar/B738/toggle_switch/flt_ctr_A_cover",
					 "laminar/B738/toggle_switch/flt_ctr_B_cover",
					 "laminar/B738/toggle_switch/spoiler_A_cover",
					 "laminar/B738/toggle_switch/spoiler_B_cover"};
    for (cover=0;cover<ncover;cover++) {
      switch_cover_pos = link_dataref_flt(switch_cover_pos_name[cover],-3);
      switch_cover_toggle = link_dataref_cmd_once(switch_cover_toggle_name[cover]);
      ival = 1;
      ret = set_switch_cover(switch_cover_pos,switch_cover_toggle,ival);
    }



    /* Pedestal Light PWM controlled by Pedestal Potentiometer */
    /* TODO: LINK PEDESTAL POTENTIOMETER DATAREF */
    *fvalue = 0.5;
    ret = digital_output(te, TEENSY_TYPE, 0, 26, avionics_on);
    ret = pwm_output(te, TEENSY_TYPE, 0, 28, fvalue,0.0,1.0);

    /* Background Lighting (Direct Potentiometer MOSFET control */
    ret = digital_output(te, TEENSY_TYPE, 0, 27, avionics_on);
  
    /* ---- -----------  */
    /* FLT CONTROL PANEL */
    /* ----------------- */

    dev = 0;
    int *alt_flaps_ctrl_up = link_dataref_cmd_once("laminar/B738/toggle_switch/alt_flaps_ctrl_up");
    int *alt_flaps_ctrl_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/alt_flaps_ctrl_dn");
    float *alt_flaps_ctrl_pos = link_dataref_flt("laminar/B738/toggle_switch/alt_flaps_ctrl",0);
    ival = 0;
    ival2 = 0;
    ret = digital_input(te, MCP23017_TYPE, dev, 2, &ival, 0);
    ret = digital_input(te, MCP23017_TYPE, dev, 3, &ival2, 0);
    fval = 0;
    if (ival == 1) fval = 1;
    if (ival2 == 1) fval = -1;
    ret = set_state_updnf(&fval,alt_flaps_ctrl_pos,alt_flaps_ctrl_dn,alt_flaps_ctrl_up);
    if (ret != 0) {
      printf("ALT FLAPS CTRL %f %i %i  \n",fval,ival,ival2);
    }

    for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
      ret = digital_input(te, MCP23017_TYPE, dev, pin, &inputvalue, 0);
      if (ret == 1) {
	printf("Digital Input %i of MCP23017 %i changed to: %i \n", pin, dev, inputvalue);

	if (inputvalue == 1) {
	  count ++;
	  if (count > 15) count = 0;
	  printf("COUNT: %i \n",count);
	}
      }
    }

    /* Yaw Damper Coil */
    ret = digital_output(te, TEENSY_TYPE, 0, 29, &zero);

    *value = 0;
    dev = 1;
    for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
     ret = digital_output(te, MCP23017_TYPE, dev, pin, value);
    }


    /* ------------------ */
    /* TEMP CONTROL PANEL */
    /* ------------------ */
 
    dev = 2;
    for (pin=0;pin<8;pin++) {
      ret = digital_input(te, MCP23017_TYPE, dev, pin, &inputvalue, 0);
      if (ret == 1) {
	printf("Digital Input %i of MCP23017 %i changed to: %i \n", pin, dev, inputvalue);
      }
    }
    
    dev = 2;
    *value = 1;
    for (pin=8;pin<11;pin++) {
      ret = digital_output(te, MCP23017_TYPE, dev, pin, value);
    }

    /* read analog input (Pin 38 / A14) for CONT CAB TEMPERATURE */
    ret = analog_input(te,38,fvalue,0.0,100.0);
    if (ret == 1) {
      printf("Analog Input changed to: %f \n",*fvalue);
    }
  
    /* Temp indicator Gauge Servo */
    dev = 0;
    ret = servo_output(te, PCA9685_TYPE, dev, 0, fvalue,0.0,100.0,0.04,0.85);

  } else {
    /* A few basic switches for other ACF */
  }
    
}

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

    /* FOR TESTING */
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

    int *spoiler_A_toggle = link_dataref_cmd_once("laminar/B738/toggle_switch/spoiler_A");
    float *spoiler_A_pos = link_dataref_flt("laminar/B738/switches/spoiler_A_pos",0);
    ret = digital_input(te, MCP23017_TYPE, dev, 0, &ival, 0);
    if (ival == 0) { fval = 1.0; } else { fval = 0.0; };
    ret = set_state_togglef(&fval,spoiler_A_pos,spoiler_A_toggle);
    if (ret != 0) {
      printf("SPOILER A OFF %i \n",(int) fval);
    }

    int *spoiler_B_toggle = link_dataref_cmd_once("laminar/B738/toggle_switch/spoiler_B");
    float *spoiler_B_pos = link_dataref_flt("laminar/B738/switches/spoiler_B_pos",0);
    ret = digital_input(te, MCP23017_TYPE, dev, 1, &ival, 0);
    if (ival == 0) { fval = 1.0; } else { fval = 0.0; };
    ret = set_state_togglef(&fval,spoiler_B_pos,spoiler_B_toggle);
    if (ret != 0) {
      printf("SPOILER B OFF %i \n",(int) fval);
    }

    int *alt_flaps_ctrl_up = link_dataref_cmd_once("laminar/B738/toggle_switch/alt_flaps_ctrl_up");
    int *alt_flaps_ctrl_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/alt_flaps_ctrl_dn");
    int *alt_flaps_ctrl_dn_HOLD = link_dataref_cmd_hold("laminar/B738/toggle_switch/alt_flaps_ctrl_dn");
    float *alt_flaps_ctrl_pos = link_dataref_flt("laminar/B738/toggle_switch/alt_flaps_ctrl",0);
    ret = digital_input(te, MCP23017_TYPE, dev, 2, alt_flaps_ctrl_dn_HOLD, 0);
    if (ret != 0) {
      printf("ALT FLAPS CTRL DOWN HOLD %i \n",*alt_flaps_ctrl_dn_HOLD);
    }
    
    ret = digital_input(te, MCP23017_TYPE, dev, 3, &ival, 0);
    if ((*alt_flaps_ctrl_dn_HOLD == 0) && (*alt_flaps_ctrl_pos != 1.0)) {
      if (ival == 1) {fval = -1.0;} else {fval = 0.0;};
      ret = set_state_updnf(&fval,alt_flaps_ctrl_pos,alt_flaps_ctrl_dn,alt_flaps_ctrl_up);
      if (ret != 0) {
	printf("ALT FLAPS CTRL %i \n",(int) fval);
      }
    }

    int *alt_flaps_toggle = link_dataref_cmd_once("laminar/B738/toggle_switch/alt_flaps");
    float *alt_flaps_pos = link_dataref_flt("laminar/B738/switches/alt_flaps_pos",0);
    ret = digital_input(te, MCP23017_TYPE, dev, 4, &ival, 0);
    if (ival == 0) { fval = 1.0; } else { fval = 0.0; };
    ret = set_state_togglef(&fval,alt_flaps_pos,alt_flaps_toggle);
    if (ret != 0) {
      printf("ALTERNATE FLAPS ARM: %i \n",(int) fval);
    }

    int *flt_ctrl_A_up = link_dataref_cmd_once("laminar/B738/toggle_switch/flt_ctr_A_up");
    int *flt_ctrl_A_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/flt_ctr_A_dn");
    float *flt_ctrl_A_pos = link_dataref_flt("laminar/B738/switches/flt_ctr_A_pos",0);
    fval = 0.0;
    ret = digital_input(te, MCP23017_TYPE, dev, 8, &ival, 0);
    if (ival == 1) fval = 1.0;
    ret = digital_input(te, MCP23017_TYPE, dev, 9, &ival, 0);
    if (ival == 1) fval = -1.0;
    ret = set_state_updnf(&fval,flt_ctrl_A_pos,flt_ctrl_A_dn,flt_ctrl_A_up);
    if (ret != 0) {
      printf("FLT CTRL A %i \n",(int) fval);
    }

    int *flt_ctrl_B_up = link_dataref_cmd_once("laminar/B738/toggle_switch/flt_ctr_B_up");
    int *flt_ctrl_B_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/flt_ctr_B_dn");
    float *flt_ctrl_B_pos = link_dataref_flt("laminar/B738/switches/flt_ctr_B_pos",0);
    fval = 0.0;
    ret = digital_input(te, MCP23017_TYPE, dev, 10, &ival, 0);
    if (ival == 1) fval = 1.0;
    ret = digital_input(te, MCP23017_TYPE, dev, 11, &ival, 0);
    if (ival == 1) fval = -1.0;
    ret = set_state_updnf(&fval,flt_ctrl_B_pos,flt_ctrl_B_dn,flt_ctrl_B_up);
    if (ret != 0) {
      printf("FLT CTRL B %i \n",(int) fval);
    }

    int *yaw_damper_toggle = link_dataref_cmd_once("laminar/B738/toggle_switch/yaw_dumper");
    float *yaw_damper_pos = link_dataref_flt("laminar/B738/toggle_switch/yaw_dumper_pos",0);
    ret = digital_input(te, MCP23017_TYPE, dev, 12, &ival, 0);
    if (ival == 1) { fval = 1.0; } else { fval = 0.0; };
    ret = set_state_togglef(&fval,yaw_damper_pos,yaw_damper_toggle);
    if (ret != 0) {
      printf("YAW DAMPER %i %f %i \n",ival, *yaw_damper_pos, *yaw_damper_toggle);
    }

    /* Yaw Damper Coil: activate if X-Plane's YD Switch is ON.
       This needs at least one ping pong round of signal between
       pressing the hardware switch and receiving the switch position
       back from X-Plane. But this allows that X-Plane cancels the coil. */
    ret = digital_outputf(te, TEENSY_TYPE, 0, 29, yaw_damper_pos);

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

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

int count;

/* to be preserved variables */
float l_wiper;
float r_wiper;
float l_eng_start;
float r_eng_start;
float ac_power;
float dc_power;
float air_valve_ctrl;

void init_b737_overheadfwd(void)
{
  int te = 1; // Teensy Number (as in ini file)
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
  /* 32: On/Off Wing Anti Ice Switch Coil */
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
  
  /* ----------------- */
  /* FLT CONTROL PANEL */
  /* ----------------- */
  
  dev = 0;
  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 0; // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x20 ^ 0x50; //(0x70) I2C address of MCP23017 device

  dev = 1;
  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  mcp23017[te][dev].intpin = INITVAL;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x21 ^ 0x50; // (0x71) I2C address of MCP23017 device

  /* -----------------------  */
  /* NAV / DISP CONTROL PANEL */
  /* ------------------------ */
  
  dev = 2;
  for (pin=0;pin<8;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 1;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x22 ^ 0x50; // (0x72) I2C address of MCP23017 device

  /* --------------- */
  /* FUEL PUMP Panel */
  /* --------------- */

  dev = 3;
  for (pin=0;pin<8;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  for (pin=8;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 2;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x23 ^ 0x50; // (0x73) I2C address of MCP23017 device

  /* --------------------- */
  /* Power & Battery Panel */
  /* --------------------- */

  dev = 4;
  for (pin=0;pin<8;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  for (pin=8;pin<11;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  mcp23017[te][dev].intpin = 3;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x24 ^ 0x50; // (0x74) I2C address of MCP23017 device

  dev = 5;
  for (pin=0;pin<2;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  for (pin=8;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 4;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x25 ^ 0x50; // (0x75) I2C address of MCP23017 device

  dev = 0;
  ht16k33[te][dev].brightness = 10; // Display Brightness (0-15)
  ht16k33[te][dev].wire = 0; // I2C Bus: 0, 1 or 2
  ht16k33[te][dev].address = 0x77 ^ 0x50; // (0x27) I2C address of HT16K33 device

  /* ------------------- */
  /*  POWER SOURCE Panel */
  /* ------------------- */
    
  dev = 6;
  for (pin=0;pin<8;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  for (pin=8;pin<11;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  for (pin=11;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 5;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x26 ^ 0x50; // (0x76) I2C address of MCP23017 device
  
  dev = 7;
  for (pin=0;pin<8;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  for (pin=8;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  mcp23017[te][dev].intpin = 6;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x27 ^ 0x50; // (0x77) I2C address of MCP23017 device
  
  /* ------------- */
  /*  CENTER Panel */
  /* ------------- */

  dev = 8;
  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 8;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x20; // I2C address of MCP23017 device
  
  /* --------------------- */
  /*  LANDING LIGHTS Panel */
  /* --------------------- */

  dev = 9;
  for (pin=0;pin<4;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  for (pin=4;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 9;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x21; // I2C address of MCP23017 device
  
  /* ------------------------------------- */
  /*  AIRCRAFT LIGHTS & ENGINE START Panel */
  /* ------------------------------------- */

  dev = 10;
  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 10;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x22; // I2C address of MCP23017 device
  
  /* -------------------------- */
  /*  WINDOW & PROBE HEAT Panel */
  /* -------------------------- */

  dev = 11;
  for (pin=0;pin<8;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  for (pin=8;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  mcp23017[te][dev].intpin = 11;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x23; // I2C address of MCP23017 device
  
  dev = 12;
  for (pin=0;pin<8;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  for (pin=8;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 12;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x24; // I2C address of MCP23017 device

  /* ------------------------------ */
  /*  Anti Ice and Hydraulics Panel */
  /* ------------------------------ */

  dev = 13;
  for (pin=0;pin<8;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  for (pin=8;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 13;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x25; // I2C address of MCP23017 device
  
  /* ------------------------------ */
  /*  Doors, CVR and Altitude Panel */
  /* ------------------------------ */

  dev = 14;
  for (pin=0;pin<11;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  for (pin=12;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 14;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x26; // I2C address of MCP23017 device
  
  /* ------------------ */
  /* TEMP CONTROL PANEL */
  /* ------------------ */
  
  dev = 15; 
  for (pin=0;pin<8;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  for (pin=8;pin<11;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  mcp23017[te][dev].intpin = 16; // also define pin 6 of teensy as INTERRUPT above!
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x20 ^ 0x40; // (0x60) I2C address of MCP23017 device

  /* --------------- */
  /* Air Bleed Panel */
  /* --------------- */

  dev = 16;
  for (pin=0;pin<8;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  for (pin=8;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  mcp23017[te][dev].intpin = 17; // also define pin 6 of teensy as INTERRUPT above!
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x21 ^ 0x40; // (0x61) I2C address of MCP23017 device
    
  dev = 17;
  for (pin=0;pin<8;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  for (pin=8;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  mcp23017[te][dev].intpin = 20; // also define pin 6 of teensy as INTERRUPT above!
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x22 ^ 0x40; // (0x62) I2C address of MCP23017 device
 
  /* -------------- */
  /* ALTITUDE Panel */
  /* -------------- */

  dev = 18;
  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 21; // also define pin 6 of teensy as INTERRUPT above!
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x23 ^ 0x40; // (0x63) I2C address of MCP23017 device

  dev = 1;
  ht16k33[te][dev].brightness = 10; // Display Brightness (0-15)
  ht16k33[te][dev].wire = 0; // I2C Bus: 0, 1 or 2
  ht16k33[te][dev].address = 0x70 ^ 0x40; // (0x30) I2C address of HT16K33 device
     
 
}

void b737_overheadfwd(void)
{

  int te = 1; // Teensy Number (as in ini file)

  int ret;
  int ret2;
  int dev;

  //int one = 1;
  int zero = 0;

  int ival;
  int ival2;
  float fval;

  /* Switch Covers and related switch states*/
  char buffer[100];
  int cover;
  int ncover;

  /* SET CONT CAB POTENTIOMETER IN TEMP PANEL TO TEST SERVOS IN OVHD PANEL
     AND ADJUST THEIR RANGE */
  float servoval;
  int servotest = 0;
  ret = analog_input(te,38,&servoval,0.0,100.0);

  int *avionics_on = link_dataref_int("sim/cockpit2/switches/avionics_power_on");
 
  /* only run for Laminar 737 or ZIBO 737 */
  if ((acf_type == 2) || (acf_type == 3)) {
    
    float *lights_test = link_dataref_flt("laminar/B738/annunciator/test",-1);


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

    /* Pedestal Flood Light On/Off */
    ret = digital_output(te, TEENSY_TYPE, 0, 26, avionics_on);

    /* Pedestal Flood Light PWM controlled by Pedestal Potentiometer */
    float *flood_brightness = link_dataref_flt("xpserver/pedestal_flood_brightness",-1);
    ret = pwm_output(te, TEENSY_TYPE, 0, 28, flood_brightness,0.1,1.0);

    /* Background Lighting (Direct Potentiometer MOSFET control */
    ret = digital_output(te, TEENSY_TYPE, 0, 27, avionics_on);
  
    /* ---- -----------  */
    /* FLT CONTROL PANEL */
    /* ----------------- */

    dev = 0;

    int *spoiler_A_toggle = link_dataref_cmd_once("laminar/B738/toggle_switch/spoiler_A");
    float *spoiler_A_pos = link_dataref_flt("laminar/B738/switches/spoiler_A_pos",0);
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 0, &ival, 0);
    if (ival != INT_MISS) {
      if (ival == 0) { fval = 1.0; } else { fval = 0.0; };
      ret = set_state_togglef(&fval,spoiler_A_pos,spoiler_A_toggle);
      if (ret != 0) {
	printf("SPOILER A OFF %i \n",(int) fval);
      }
    }

    int *spoiler_B_toggle = link_dataref_cmd_once("laminar/B738/toggle_switch/spoiler_B");
    float *spoiler_B_pos = link_dataref_flt("laminar/B738/switches/spoiler_B_pos",0);
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 1, &ival, 0);
    if (ival != INT_MISS) {
      if (ival == 0) { fval = 1.0; } else { fval = 0.0; };
      ret = set_state_togglef(&fval,spoiler_B_pos,spoiler_B_toggle);
      if (ret != 0) {
	printf("SPOILER B OFF %i \n",(int) fval);
      }
    }

    int *alt_flaps_ctrl_up = link_dataref_cmd_once("laminar/B738/toggle_switch/alt_flaps_ctrl_up");
    int *alt_flaps_ctrl_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/alt_flaps_ctrl_dn");
    int *alt_flaps_ctrl_dn_HOLD = link_dataref_cmd_hold("laminar/B738/toggle_switch/alt_flaps_ctrl_dn");
    float *alt_flaps_ctrl_pos = link_dataref_flt("laminar/B738/toggle_switch/alt_flaps_ctrl",0);
    ret = digital_input(te, MCP23017_TYPE, dev, 2, alt_flaps_ctrl_dn_HOLD, 0);
    if (ret != 0) {
      printf("ALT FLAPS CTRL DOWN HOLD %i \n",*alt_flaps_ctrl_dn_HOLD);
    }

    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 3, &ival, 0);
    if ((*alt_flaps_ctrl_dn_HOLD == 0) && (*alt_flaps_ctrl_pos != 1.0) && (ival != INT_MISS)) {
      if (ival == 1) {fval = -1.0;} else {fval = 0.0;};
      ret = set_state_updnf(&fval,alt_flaps_ctrl_pos,alt_flaps_ctrl_dn,alt_flaps_ctrl_up);
      if (ret != 0) {
	printf("ALT FLAPS CTRL %i \n",(int) fval);
      }
    }

    int *alt_flaps_toggle = link_dataref_cmd_once("laminar/B738/toggle_switch/alt_flaps");
    float *alt_flaps_pos = link_dataref_flt("laminar/B738/switches/alt_flaps_pos",0);
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 4, &ival, 0);
    if (ival != INT_MISS) {
      if (ival == 0) { fval = 1.0; } else { fval = 0.0; };
    } else {
      fval = FLT_MISS;
    }
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
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 12, &ival, 0);
    if (ival != INT_MISS) {
      if (ival == 1) { fval = 1.0; } else { fval = 0.0; };
    } else {
      fval = FLT_MISS;
    }
    ret = set_state_togglef(&fval,yaw_damper_pos,yaw_damper_toggle);
    if (ret != 0) {
      printf("YAW DAMPER %i %f %i \n",ival, *yaw_damper_pos, *yaw_damper_toggle);
    }
   

    /* Yaw Damper Coil: activate if X-Plane's YD Switch is ON.
       This needs at least one ping pong round of signal between
       pressing the hardware switch and receiving the switch position
       back from X-Plane. But this allows that X-Plane cancels the coil. */
    ret = digital_outputf(te, TEENSY_TYPE, 0, 29, yaw_damper_pos);

    /* Annunciators */
    dev = 1;

    if ((*lights_test == 1.0) && (*avionics_on == 1)) {ival = 1;} else {ival = 0;};
    
    /* FEEL DIFF PRESS 0 */
    float *diff_press = link_dataref_flt("laminar/B738/annunciator/feel_diff_press",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 0, diff_press);
    /* SPEED TRIM FAIL 1 */
    ret = digital_output(te, MCP23017_TYPE, dev, 1, &ival);
    /* MACH TRIM FAIL 2 */
    ret = digital_output(te, MCP23017_TYPE, dev, 2, &ival);
    /* AUTO SLAT FAIL 3 */
    float *auto_slat_fail = link_dataref_flt("laminar/B738/annunciator/auto_slat_fail",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 3, auto_slat_fail);
    /* STDBY HYDRAULICS LOW QUANTITY 4 */
    ret = digital_output(te, MCP23017_TYPE, dev, 4, &ival);
    /* STDBY HYDRAULICS LOW_PRESSURE 5 */
    float *stby_hyd_press = link_dataref_flt("laminar/B738/annunciator/hyd_stdby_rud",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 5, stby_hyd_press);
    /* STDBY HYDRAULICS STBY RUD ON 6 */
    float *stby_rud_on = link_dataref_flt("laminar/B738/annunciator/std_rud_on",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 6, stby_rud_on);
    /* YAW DAMPER 8 */
    float *yaw_damper_on = link_dataref_flt("laminar/B738/annunciator/yaw_damp",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 8, yaw_damper_on);
    /* FLT CONTROL B LOW PRESSURE 9 */
    float *hyd_B_press = link_dataref_flt("laminar/B738/annunciator/hyd_B_rud",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 9, hyd_B_press);
    /* FLT CONTROL A LOW PRESSURE 10 */
    float *hyd_A_press = link_dataref_flt("laminar/B738/annunciator/hyd_A_rud",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 10, hyd_A_press);

    /* -----------------------  */
    /* NAV / DISP CONTROL PANEL */
    /* ------------------------ */
    
    dev = 2;

    int *vhf_nav_source_left = link_dataref_cmd_once("laminar/B738/toggle_switch/vhf_nav_source_lft");
    int *vhf_nav_source_right = link_dataref_cmd_once("laminar/B738/toggle_switch/vhf_nav_source_rgt");
    float *vhf_nav_source_pos = link_dataref_flt("laminar/B738/toggle_switch/vhf_nav_source",0);
    ival = INT_MISS;
    ival2 = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 0, &ival, 0);
    ret = digital_input(te, MCP23017_TYPE, dev, 1, &ival2, 0);
    float vhf_nav_source = FLT_MISS;
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) vhf_nav_source = (float) (ival2 - ival);
    ret = set_state_updnf(&vhf_nav_source,vhf_nav_source_pos,vhf_nav_source_right,vhf_nav_source_left);
    if (ret != 0) {
      printf("VHF NAV SOURCE %i \n",(int) vhf_nav_source);
    }
    
    int *irs_source_left = link_dataref_cmd_once("laminar/B738/toggle_switch/irs_source_left");
    int *irs_source_right = link_dataref_cmd_once("laminar/B738/toggle_switch/irs_source_right");
    float *irs_source_pos = link_dataref_flt("laminar/B738/toggle_switch/irs_source",0);
    ival = INT_MISS;
    ival2 = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 2, &ival, 0);
    ret = digital_input(te, MCP23017_TYPE, dev, 3, &ival2, 0);
    float irs_source = FLT_MISS;
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) irs_source = (float) (ival2 - ival);
    ret = set_state_updnf(&irs_source,irs_source_pos,irs_source_right,irs_source_left);
    if (ret != 0) {
      printf("IRS SOURCE %i \n",(int) irs_source);
    }

    int *control_panel_source_left = link_dataref_cmd_once("laminar/B738/toggle_switch/dspl_ctrl_pnl_left");
    int *control_panel_source_right = link_dataref_cmd_once("laminar/B738/toggle_switch/dspl_ctrl_pnl_right");
    float *control_panel_source_pos = link_dataref_flt("laminar/B738/toggle_switch/dspl_ctrl_pnl",0);
    ival = INT_MISS;
    ival2 = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 4, &ival, 0);
    ret = digital_input(te, MCP23017_TYPE, dev, 5, &ival2, 0);
    float control_panel_source = FLT_MISS;
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) control_panel_source = (float) (ival2 - ival);
    ret = set_state_updnf(&control_panel_source,control_panel_source_pos,
			  control_panel_source_right,control_panel_source_left);
    if (ret != 0) {
      printf("CONTROL PANEL SOURCE %i \n",(int) control_panel_source);
    }

    int *display_source_left = link_dataref_cmd_once("laminar/B738/toggle_switch/dspl_source_left");
    int *display_source_right = link_dataref_cmd_once("laminar/B738/toggle_switch/dspl_source_right");
    float *display_source_pos = link_dataref_flt("laminar/B738/toggle_switch/dspl_source",0);
    ival = INT_MISS;
    ival2 = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 6, &ival, 0);
    ret = digital_input(te, MCP23017_TYPE, dev, 7, &ival2, 0);
    float display_source = FLT_MISS;
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) display_source = (float) (ival2 - ival);
    ret = set_state_updnf(&display_source,display_source_pos,display_source_right,display_source_left);
    if (ret != 0) {
      printf("DISPLAY SOURCE %i \n",(int) display_source);
    }
    
    /* --------------- */
    /* FUEL PUMP Panel */
    /* --------------- */
    
    dev = 3;

    float *cross_feed = link_dataref_flt("laminar/B738/knobs/cross_feed_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 8, cross_feed, 0);
    if (ret != 0) {
      printf("CROSS FEED %i \n",(int) *cross_feed);
    }
    float *fuel_pump_ctr_1_on = link_dataref_flt("laminar/B738/fuel/fuel_tank_pos_ctr1",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 9, fuel_pump_ctr_1_on, 0);
    if (ret != 0) {
      printf("CENTER FUEL PUMP L %i \n",(int) *fuel_pump_ctr_1_on);
    }
    float *fuel_pump_ctr_2_on = link_dataref_flt("laminar/B738/fuel/fuel_tank_pos_ctr2",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 10, fuel_pump_ctr_2_on, 0);
    if (ret != 0) {
      printf("CENTER FUEL PUMP R %i \n",(int) *fuel_pump_ctr_2_on);
    }
    float *fuel_pump_aft_1_on = link_dataref_flt("laminar/B738/fuel/fuel_tank_pos_lft1",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 11, fuel_pump_aft_1_on, 0);
    if (ret != 0) {
      printf("AFT FUEL PUMP 1 %i \n",(int) *fuel_pump_aft_1_on);
    }
    float *fuel_pump_fwd_1_on = link_dataref_flt("laminar/B738/fuel/fuel_tank_pos_lft2",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 12, fuel_pump_fwd_1_on, 0);
    if (ret != 0) {
      printf("FWD FUEL PUMP 1 %i \n",(int) *fuel_pump_fwd_1_on);
    }
    float *fuel_pump_fwd_2_on = link_dataref_flt("laminar/B738/fuel/fuel_tank_pos_rgt2",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 13, fuel_pump_fwd_2_on, 0);
    if (ret != 0) {
      printf("FWD FUEL PUMP 2 %i \n",(int) *fuel_pump_fwd_2_on);
    }
    float *fuel_pump_aft_2_on = link_dataref_flt("laminar/B738/fuel/fuel_tank_pos_rgt1",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 14, fuel_pump_aft_2_on, 0);
    if (ret != 0) {
      printf("AFT FUEL PUMP 2 %i \n",(int) *fuel_pump_aft_2_on);
    }

    float *bypass_filter_1 = link_dataref_flt("laminar/B738/annunciator/bypass_filter_1",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 0, bypass_filter_1);
    float *bypass_filter_2 = link_dataref_flt("laminar/B738/annunciator/bypass_filter_2",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 1, bypass_filter_2);
    float *low_press_ctr_1 = link_dataref_flt("laminar/B738/annunciator/low_fuel_press_c1",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 2, low_press_ctr_1);
    float *low_press_ctr_2 = link_dataref_flt("laminar/B738/annunciator/low_fuel_press_c2",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 3, low_press_ctr_2);
    float *low_press_aft_1 = link_dataref_flt("laminar/B738/annunciator/low_fuel_press_l1",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 4, low_press_aft_1);
    float *low_press_fwd_1 = link_dataref_flt("laminar/B738/annunciator/low_fuel_press_l2",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 5, low_press_fwd_1);
    float *low_press_fwd_2 = link_dataref_flt("laminar/B738/annunciator/low_fuel_press_r2",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 6, low_press_fwd_2);
    float *low_press_aft_2 = link_dataref_flt("laminar/B738/annunciator/low_fuel_press_r1",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 7, low_press_aft_2);

    dev = 1;
    float *eng_valve_closed_1 = link_dataref_flt("laminar/B738/annunciator/eng1_valve_closed",-2);
    ret = pwm_output(te, PCA9685_TYPE, dev, 0, eng_valve_closed_1,0.0,0.05);
    float *spar_valve_closed_1 = link_dataref_flt("laminar/B738/annunciator/spar1_valve_closed",-2);
    ret = pwm_output(te, PCA9685_TYPE, dev, 1, spar_valve_closed_1,0.0,0.05);
    float *eng_valve_closed_2 = link_dataref_flt("laminar/B738/annunciator/eng2_valve_closed",-2);
    ret = pwm_output(te, PCA9685_TYPE, dev, 2, eng_valve_closed_2,0.0,0.05);
    float *spar_valve_closed_2 = link_dataref_flt("laminar/B738/annunciator/spar2_valve_closed",-2);
    ret = pwm_output(te, PCA9685_TYPE, dev, 3, spar_valve_closed_2,0.0,0.05);
    float *cross_feed_valve = link_dataref_flt("laminar/B738/annunciator/crossfeed",-2);
    ret = pwm_output(te, PCA9685_TYPE, dev, 4, cross_feed_valve,0.0,0.05);

    dev = 0;
    float *fuel_temp = link_dataref_flt("laminar/B738/engine/fuel_temp",0);
    if (servotest == 1) {
      ret = servo_output(te, PCA9685_TYPE, dev, 1, &servoval,0.0,100.0,0.03,0.85);
    } else {
      ret = servo_output(te, PCA9685_TYPE, dev, 1, fuel_temp,-50.0,50.0,0.03,0.85);
    }


    /* --------------------- */
    /* Power & Battery Panel */
    /* --------------------- */

    dev = 4;
    
    /* AC Power Knob */
    int *ac_power_up = link_dataref_cmd_once("laminar/B738/knob/ac_power_up");
    int *ac_power_dn = link_dataref_cmd_once("laminar/B738/knob/ac_power_dn");
    float *ac_power_pos = link_dataref_flt("laminar/B738/knob/ac_power",0);
    
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 0, &ival, 0);
    if (ival == 1) ac_power = 0.0; // STBY PWR
    ret = digital_input(te, MCP23017_TYPE, dev, 1, &ival, 0);
    if (ival == 1) ac_power = 1.0; // GRD PWR
    ret = digital_input(te, MCP23017_TYPE, dev, 2, &ival, 0);
    if (ival == 1) ac_power = 2.0; // GEN1
    ret = digital_input(te, MCP23017_TYPE, dev, 3, &ival, 0);
    if (ival == 1) ac_power = 3.0; // APU GEN
    ret = digital_input(te, MCP23017_TYPE, dev, 4, &ival, 0);
    if (ival == 1) ac_power = 4.0; // GEN2
    ret = digital_input(te, MCP23017_TYPE, dev, 5, &ival, 0);
    if (ival == 1) ac_power = 5.0; // INV
    ret = digital_input(te, MCP23017_TYPE, dev, 6, &ival, 0);
    if (ival == 1) ac_power = 6.0; // TEST

    if ((ac_power < 0.0) || (ac_power > 6.0) || (ival == INT_MISS)) ac_power = FLT_MISS;

    ret = set_state_updnf(&ac_power,ac_power_pos,ac_power_up,ac_power_dn);
    if (ret != 0) {
      printf("AC Power knob %i \n",(int) ac_power);
    }

    /* MAINT Pushbutton */
    int *maint = link_dataref_cmd_once("laminar/B738/push_button/acdc_maint");
    ret = digital_input(te, MCP23017_TYPE, dev, 7, maint, 0);
    if (ret == 1) {
      printf("MAINT Button %i \n",*maint);
    }

    /* Annunciators */
    float *bat_discharge = link_dataref_flt("laminar/B738/annunciator/bat_discharge",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 9, bat_discharge);
    float *tr_unit = link_dataref_flt("laminar/B738/annunciator/tr_unit",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 10, tr_unit);
    float *elec = link_dataref_flt("laminar/B738/annunciator/elec",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 8, elec);
 
    dev = 5;

    /* Galley PWR */
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 0, &ival, 0);
    if (ret == 1) {
      printf("Galley PWR %i \n",ival);
    }

    /* Battery Switch: dn is moving switch to 1 and up to 0 */
    int *battery_up = link_dataref_cmd_once("laminar/B738/push_button/batt_full_off"); // includes guard
    //int *battery_up = link_dataref_cmd_once("laminar/B738/switch/battery_up");
    int *battery_dn = link_dataref_cmd_once("laminar/B738/switch/battery_dn");
    float *battery_pos = link_dataref_flt("laminar/B738/electric/battery_pos",0);
    float battery = FLT_MISS;
    ret = digital_inputf(te, MCP23017_TYPE, dev, 1, &battery, 0);
    if (battery != FLT_MISS) battery = 1.0 - battery;
    
    ret = set_state_updnf(&battery,battery_pos,battery_dn,battery_up);
    if (ret != 0) {
      printf("Battery Switch %i \n",(int) battery);
    }

    /* DC Power Knob */
    int *dc_power_up = link_dataref_cmd_once("laminar/B738/knob/dc_power_up");
    int *dc_power_dn = link_dataref_cmd_once("laminar/B738/knob/dc_power_dn");
    float *dc_power_pos = link_dataref_flt("laminar/B738/knob/dc_power",0);
   
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 8, &ival, 0);
    if (ival == 1) dc_power = 0.0; // STBY PWR
    ret = digital_input(te, MCP23017_TYPE, dev, 9, &ival, 0);
    if (ival == 1) dc_power = 1.0; // BAT BUS
    ret = digital_input(te, MCP23017_TYPE, dev, 10, &ival, 0);
    if (ival == 1) dc_power = 2.0; // BAT
    ret = digital_input(te, MCP23017_TYPE, dev, 11, &ival, 0);
    if (ival == 1) dc_power = 2.0; // AUX BAT (INOP in ZIBO
    ret = digital_input(te, MCP23017_TYPE, dev, 12, &ival, 0);
    if (ival == 1) dc_power = 3.0; // TR1
    ret = digital_input(te, MCP23017_TYPE, dev, 13, &ival, 0);
    if (ival == 1) dc_power = 4.0; // TR2
    ret = digital_input(te, MCP23017_TYPE, dev, 14, &ival, 0);
    if (ival == 1) dc_power = 5.0; // TR3
    ret = digital_input(te, MCP23017_TYPE, dev, 15, &ival, 0);
    if (ival == 1) dc_power = 6.0; // TEST

    if ((dc_power < 0.0) || (dc_power > 6.0) || (ival == INT_MISS)) dc_power = FLT_MISS;

    ret = set_state_updnf(&dc_power,dc_power_pos,dc_power_up,dc_power_dn);
    if (ret != 0) {
      printf("DC Power knob %i \n",(int) dc_power);
    }
    
    /* 7 Segment Amps & Volts */
    dev = 0;
    
    float *dc_volt = link_dataref_flt("laminar/B738/dc_volt_value",0);
    float *dc_amps = link_dataref_flt("laminar/B738/dc_amp_value",0);
    float *ac_volt = link_dataref_flt("laminar/B738/ac_volt_value",0);
    float *ac_amps = link_dataref_flt("laminar/B738/ac_amp_value",0);
    float *ac_freq = link_dataref_flt("laminar/B738/ac_freq_value",0);
    int brightness = 10;
    int dp = -1;
    if (*avionics_on == 1) {
      ret = display_outputf(te, HT16K33_TYPE, dev, 6, 2, dc_amps, dp, brightness);
      ret = display_outputf(te, HT16K33_TYPE, dev, 3, 3, ac_freq, dp, brightness);
      ret = display_outputf(te, HT16K33_TYPE, dev, 13, 2, dc_volt, dp, brightness);
      ret = display_outputf(te, HT16K33_TYPE, dev, 11, 2, ac_amps, dp, brightness);
      ret = display_outputf(te, HT16K33_TYPE, dev, 8, 3, ac_volt, dp, brightness);
    } else {
      ival = 16;
      for (int i=0;i<16;i++) {
	ret = display_output(te, HT16K33_TYPE, dev, i, 1, &ival, dp, brightness);
      }
    }
    

    /* ------------------- */
    /*  POWER SOURCE Panel */
    /* ------------------- */

    dev = 6;

    int *grd_pwr_up = link_dataref_cmd_hold("laminar/B738/toggle_switch/gpu_up");
    int *grd_pwr_dn = link_dataref_cmd_hold("laminar/B738/toggle_switch/gpu_dn");
    ret = digital_input(te, MCP23017_TYPE, dev, 0, grd_pwr_dn, 0);
    if (ret == 1) {
      printf("GRD PWR ON %i \n", *grd_pwr_dn);
    }
    ret = digital_input(te, MCP23017_TYPE, dev, 1, grd_pwr_up, 0);
    if (ret == 1) {
      printf("GRD PWR OFF %i \n", *grd_pwr_up);
    }
     
    int *bus_transfer = link_dataref_int("sim/cockpit2/electrical/cross_tie");
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 2, &ival, 0);
    if (ival != INT_MISS) *bus_transfer = 1-ival;
    if (ret == 1) {
      printf("BUS TRANSFER %i \n", *bus_transfer);
    }
    
    int *l_wiper_up = link_dataref_cmd_once("laminar/B738/knob/left_wiper_up");
    int *l_wiper_dn = link_dataref_cmd_once("laminar/B738/knob/left_wiper_dn");
    float *l_wiper_pos = link_dataref_flt("laminar/B738/switches/left_wiper_pos",0);

    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 3, &ival, 0);
    if (ival == 1) l_wiper = 0.0; // PARK
    ret = digital_input(te, MCP23017_TYPE, dev, 4, &ival, 0);
    if (ival == 1) l_wiper = 1.0; // INT
    ret = digital_input(te, MCP23017_TYPE, dev, 5, &ival, 0);
    if (ival == 1) l_wiper = 2.0; // LOW
    ret = digital_input(te, MCP23017_TYPE, dev, 6, &ival, 0);
    if (ival == 1) l_wiper = 3.0; // HIGH

    if ((l_wiper < 0.0) || (l_wiper > 3.0) || (ival == INT_MISS)) l_wiper = FLT_MISS;

    ret = set_state_updnf(&l_wiper,l_wiper_pos,l_wiper_up,l_wiper_dn);
    if (ret != 0) {
      printf("Left Wiper %i \n",(int) l_wiper);
    }

    float *standby_power_switch = link_dataref_flt("laminar/B738/electric/standby_bat_pos",0);
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 11, &ival, 0);
    ival2 = INT_MISS;
    ret2 = digital_input(te, MCP23017_TYPE, dev, 12, &ival2, 0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) *standby_power_switch = (float) (ival2 - ival);
    if ((ret == 1) || (ret2 == 1)) {
      printf("STANDBY PWR Switch %i \n",(int) *standby_power_switch);
    }

    int *drive_discon_1_up = link_dataref_cmd_once("laminar/B738/one_way_switch/drive_disconnect1");
    int *drive_discon_1_dn = link_dataref_cmd_once("laminar/B738/one_way_switch/drive_disconnect1_off");
    float *drive_discon_1_pos = link_dataref_flt("laminar/B738/one_way_switch/drive_disconnect1_pos",0);

    float drive_discon_1 = FLT_MISS;
    ret = digital_inputf(te, MCP23017_TYPE, dev, 14, &drive_discon_1, 0);
    ret = set_state_updnf(&drive_discon_1,drive_discon_1_pos,drive_discon_1_up,drive_discon_1_dn);
    if (ret != 0) {
      printf("Drive Disconnect 1 %i \n",(int) drive_discon_1);
    }
    
    int *drive_discon_2_up = link_dataref_cmd_once("laminar/B738/one_way_switch/drive_disconnect2");
    int *drive_discon_2_dn = link_dataref_cmd_once("laminar/B738/one_way_switch/drive_disconnect2_off");
    float *drive_discon_2_pos = link_dataref_flt("laminar/B738/one_way_switch/drive_disconnect2_pos",0);

    float drive_discon_2 = FLT_MISS;
    ret = digital_inputf(te, MCP23017_TYPE, dev, 13, &drive_discon_2, 0);
    ret = set_state_updnf(&drive_discon_2,drive_discon_2_pos,drive_discon_2_up,drive_discon_2_dn);
    if (ret != 0) {
      printf("Drive Disconnect 1 %i \n",(int) drive_discon_2);
    }
    
    float *drive_1_ann = link_dataref_flt("laminar/B738/annunciator/drive1",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 8, drive_1_ann);
    float *standby_pwr_ann = link_dataref_flt("laminar/B738/annunciator/standby_pwr_off",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 9, standby_pwr_ann);
    float *drive_2_ann = link_dataref_flt("laminar/B738/annunciator/drive2",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 10, drive_2_ann);

    dev = 7;

    int *gen1_switch_up = link_dataref_cmd_hold("laminar/B738/toggle_switch/gen1_up");
    int *gen1_switch_dn = link_dataref_cmd_hold("laminar/B738/toggle_switch/gen1_dn");
    ret = digital_input(te, MCP23017_TYPE, dev, 0, gen1_switch_dn, 0);
    if (ret == 1) {
      printf("GEN 1 ON %i \n", *gen1_switch_dn);
    }
    ret = digital_input(te, MCP23017_TYPE, dev, 1, gen1_switch_up, 0);
    if (ret == 1) {
      printf("GEN 1 OFF %i \n", *gen1_switch_up);
    }

    int *apu_gen1_switch_up = link_dataref_cmd_hold("laminar/B738/toggle_switch/apu_gen1_up");
    int *apu_gen1_switch_dn = link_dataref_cmd_hold("laminar/B738/toggle_switch/apu_gen1_dn");
    ret = digital_input(te, MCP23017_TYPE, dev, 2, apu_gen1_switch_dn, 0);
    if (ret == 1) {
      printf("APU GEN 1 ON %i \n", *apu_gen1_switch_dn);
    }
    ret = digital_input(te, MCP23017_TYPE, dev, 3, apu_gen1_switch_up, 0);
    if (ret == 1) {
      printf("APU GEN 1 OFF %i \n", *apu_gen1_switch_up);
    }

    int *apu_gen2_switch_up = link_dataref_cmd_hold("laminar/B738/toggle_switch/apu_gen2_up");
    int *apu_gen2_switch_dn = link_dataref_cmd_hold("laminar/B738/toggle_switch/apu_gen2_dn");
    ret = digital_input(te, MCP23017_TYPE, dev, 4, apu_gen2_switch_dn, 0);
    if (ret == 1) {
      printf("APU GEN 2 ON %i \n", *apu_gen2_switch_dn);
    }
    ret = digital_input(te, MCP23017_TYPE, dev, 5, apu_gen2_switch_up, 0);
    if (ret == 1) {
      printf("APU GEN 2 OFF %i \n", *apu_gen2_switch_up);
    }

    int *gen2_switch_up = link_dataref_cmd_hold("laminar/B738/toggle_switch/gen2_up");
    int *gen2_switch_dn = link_dataref_cmd_hold("laminar/B738/toggle_switch/gen2_dn");
    ret = digital_input(te, MCP23017_TYPE, dev, 6, gen2_switch_dn, 0);
    if (ret == 1) {
      printf("GEN 2 ON %i \n", *gen2_switch_dn);
    }
    ret = digital_input(te, MCP23017_TYPE, dev, 7, gen2_switch_up, 0);
    if (ret == 1) {
      printf("GEN 2 OFF %i \n", *gen2_switch_up);
    }

    float *transfer_bus_off_1 = link_dataref_flt("laminar/B738/annunciator/trans_bus_off1",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 8, transfer_bus_off_1);
    float *source_off_1 = link_dataref_flt("laminar/B738/annunciator/source_off1",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 9, source_off_1);
    float *transfer_bus_off_2 = link_dataref_flt("laminar/B738/annunciator/trans_bus_off2",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 10, transfer_bus_off_2);
    float *source_off_2 = link_dataref_flt("laminar/B738/annunciator/source_off2",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 11, source_off_2);
    float *apu_low_oil = link_dataref_flt("laminar/B738/annunciator/apu_low_oil",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 12, apu_low_oil);
    float *apu_fault = link_dataref_flt("laminar/B738/annunciator/apu_fault",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 13, apu_fault);
    /* YELLOW OVER SPEED ANNUNCIATOR NOT YET AVAILABLE IN ZIBO MOD */
    ret = digital_outputf(te, MCP23017_TYPE, dev, 14, lights_test);


    /* Blue Annunciators via PWM driver */
    dev = 1;
    float *grd_pwr_avail = link_dataref_flt("laminar/B738/annunciator/ground_power_avail",-2);
    ret = pwm_output(te, PCA9685_TYPE, dev, 5, grd_pwr_avail,0.0,0.05);    
    float *gen_off_bus_1 = link_dataref_flt("laminar/B738/annunciator/gen_off_bus1",-2);
    ret = pwm_output(te, PCA9685_TYPE, dev, 8, gen_off_bus_1,0.0,0.05);    
    float *apu_gen_off_bus = link_dataref_flt("laminar/B738/annunciator/apu_gen_off_bus",-2);
    ret = pwm_output(te, PCA9685_TYPE, dev, 9, apu_gen_off_bus,0.0,0.05);    
    float *gen_off_bus_2 = link_dataref_flt("laminar/B738/annunciator/gen_off_bus2",-2);
    ret = pwm_output(te, PCA9685_TYPE, dev, 10, gen_off_bus_2,0.0,0.05);    
    /* BLUE MAINT ANNUNCIATOR NOT YET AVAILABLE IN ZIBO MOD */
    ret = pwm_output(te, PCA9685_TYPE, dev, 11, lights_test,0.0,0.05);    

    dev = 0;
    /* EGT dataref: A value of 100 means 800 degrees, so multiply by 1.25 */
    float *apu_temp = link_dataref_flt("laminar/B738/electrical/apu_temp",-1);
    if (servotest == 1) {
      ret = servo_output(te, PCA9685_TYPE, dev, 2, &servoval,0.0,100.0,0.13,0.94);
    } else {
      ret = servo_output(te, PCA9685_TYPE, dev, 2, apu_temp,0.0,125.0,0.13,0.94);
    }

    /* ------------- */
    /*  CENTER Panel */
    /* ------------- */
    
    dev = 8;

    /* Fasten Seat Belts */
    int *belts_up = link_dataref_cmd_once("laminar/B738/toggle_switch/seatbelt_sign_up");
    int *belts_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/seatbelt_sign_dn");
    float *belts_pos = link_dataref_flt("laminar/B738/toggle_switch/seatbelt_sign_pos",0);
    ret = digital_input(te, MCP23017_TYPE, dev, 0, &ival2, 0);
    ret = digital_input(te, MCP23017_TYPE, dev, 1, &ival, 0);
    float belts = FLT_MISS;
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) belts = (float) (ival*2 + (1-ival)*(1-ival2));
    ret = set_state_updnf(&belts,belts_pos,belts_dn,belts_up);
    if (ret == 1) {
      printf("Fasten Belts: %i \n",(int) belts);
    }
    
    /* Chimes Only / No Smoking */
    /* TEMPORARY Set to Payload Presets */
    int *zone1 = link_dataref_int_arr("laminar/B738/tab/zone1_payload",3,-1);
    int *zone2 = link_dataref_int_arr("laminar/B738/tab/zone2_payload",3,-1);
    int *zone3 = link_dataref_int_arr("laminar/B738/tab/zone3_payload",3,-1);
    int *zone4 = link_dataref_int_arr("laminar/B738/tab/zone4_payload",3,-1);
    int *zone5 = link_dataref_int_arr("laminar/B738/tab/zone5_payload",3,-1);
    int *cargo1 = link_dataref_int("laminar/B738/tab/zone_cargo1_payload");
    int *cargo2 = link_dataref_int("laminar/B738/tab/zone_cargo2_payload");

    ret = digital_input(te, MCP23017_TYPE, dev, 3, &ival2, 0);
    if (ret == 1) {
      if (ival2 == 0) {
	printf("Aircraft Payload: Medium\n");
      } else {
	printf("Aircraft Payload: Full\n");
      }
    }
    ret = digital_input(te, MCP23017_TYPE, dev, 2, &ival, 0);
    if (ret == 1) {
      if (ival == 1) {
	printf("Aircraft Payload: Empty\n");
      } else {
	printf("Aircraft Payload: Medium\n");
      }
    }

    if ((ival == 0) && (ival2 == 1)) {
      /* Empty */
      zone1[0] = 0;
      zone1[1] = 0;
      zone1[2] = 0;
      zone2[0] = 0;
      zone2[1] = 0;
      zone2[2] = 0;
      zone3[0] = 0;
      zone3[1] = 0;
      zone3[2] = 0;
      zone4[0] = 0;
      zone4[1] = 0;
      zone4[2] = 0;
      zone5[0] = 0;
      zone5[1] = 0;
      zone5[2] = 0;
      *cargo1 = 0;
      *cargo2 = 0;
    } else if ((ival == 0) && (ival2 == 0)) {
      /* Medium */
      zone1[0] = 2;
      zone1[1] = 3;
      zone1[2] = 2;
      zone2[0] = 6;
      zone2[1] = 6;
      zone2[2] = 6;
      zone3[0] = 6;
      zone3[1] = 6;
      zone3[2] = 6;
      zone4[0] = 6;
      zone4[1] = 6;
      zone4[2] = 6;
      zone5[0] = 6;
      zone5[1] = 6;
      zone5[2] = 6;
      *cargo1 = 1650;
      *cargo2 = 2300;
     } else {
      /* Full */
      zone1[0] = 6;
      zone1[1] = 4;
      zone1[2] = 6;
      zone2[0] = 12;
      zone2[1] = 12;
      zone2[2] = 12;
      zone3[0] = 12;
      zone3[1] = 12;
      zone3[2] = 12;
      zone4[0] = 12;
      zone4[1] = 12;
      zone4[2] = 12;
      zone5[0] = 12;
      zone5[1] = 12;
      zone5[2] = 12;
      *cargo1 = 3500;
      *cargo2 = 4600;
     }

    int *exit_lights_up = link_dataref_cmd_once("laminar/B738/toggle_switch/emer_exit_lights_up");
    int *exit_lights_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/emer_exit_lights_dn");
    float *exit_lights_pos = link_dataref_flt("laminar/B738/toggle_switch/emer_exit_lights",0);
    ret = digital_input(te, MCP23017_TYPE, dev, 4, &ival, 0);
    ret = digital_input(te, MCP23017_TYPE, dev, 5, &ival2, 0);
    float exit_lights = FLT_MISS;
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) exit_lights = (float) (2*ival2 + (1-ival));
    ret = set_state_updnf(&exit_lights,exit_lights_pos,exit_lights_dn,exit_lights_up);
    if (ret == 1) {
      printf("EMERGENCY Exit Lights: %i \n",(int) exit_lights);
    }

    float *eq_cool_supply = link_dataref_flt("laminar/B738/toggle_switch/eq_cool_supply",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 7, eq_cool_supply, 0);
    if (ret == 1) {
      printf("EQUIP COOLING SUPPLY: %i \n",(int) *eq_cool_supply);
    }
    float *eq_cool_exhaust = link_dataref_flt("laminar/B738/toggle_switch/eq_cool_exhaust",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 6, eq_cool_exhaust, 0);
    if (ret == 1) {
      printf("EQUIP COOLING EXHAUST: %i \n",(int) *eq_cool_exhaust);
    }
      
    int *attend = link_dataref_cmd_hold("laminar/B738/push_button/attend");
    ret = digital_input(te, MCP23017_TYPE, dev, 8, attend, 0);
    if (ret == 1) {
      printf("ATTEND: %i \n",*attend);
    }

    int *grd_call = link_dataref_cmd_hold("laminar/B738/push_button/grd_call");
    ret = digital_input(te, MCP23017_TYPE, dev, 9, grd_call, 0);
    if (ret == 1) {
      printf("GRD CALL: %i \n",*grd_call);
    }
     
    ret = digital_input(te, MCP23017_TYPE, dev, 10, &ival, 0);
    if (ret == 1) {
      printf("RAIN REPPELLENT L: %i \n",ival);
    }
     
    ret = digital_input(te, MCP23017_TYPE, dev, 11, &ival, 0);
    if (ret == 1) {
      printf("RAIN REPPELLENT R: %i \n",ival);
    }
     
    int *r_wiper_up = link_dataref_cmd_once("laminar/B738/knob/right_wiper_up");
    int *r_wiper_dn = link_dataref_cmd_once("laminar/B738/knob/right_wiper_dn");
    float *r_wiper_pos = link_dataref_flt("laminar/B738/switches/right_wiper_pos",0);

    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 12, &ival, 0);
    if (ival == 1) r_wiper = 0.0; // PARK
    ret = digital_input(te, MCP23017_TYPE, dev, 13, &ival, 0);
    if (ival == 1) r_wiper = 1.0; // INT
    ret = digital_input(te, MCP23017_TYPE, dev, 14, &ival, 0);
    if (ival == 1) r_wiper = 2.0; // LOW
    ret = digital_input(te, MCP23017_TYPE, dev, 15, &ival, 0);
    if (ival == 1) r_wiper = 3.0; // HIGHT

    if ((r_wiper < 0.0) || (r_wiper > 3.0) || (ival == INT_MISS)) r_wiper = FLT_MISS;

    ret = set_state_updnf(&r_wiper,r_wiper_pos,r_wiper_up,r_wiper_dn);
    if (ret != 0) {
      printf("Right Wiper %i \n",(int) r_wiper);
    }

    
    /* --------------------- */
    /*  LANDING LIGHTS Panel */
    /* --------------------- */
    
    dev = 9;
  
    /* CHECK AVAILABILITY OF THOSE ANNUNCIATORS IN ZIBO */
    
    if ((*lights_test == 1.0) && (*avionics_on == 1)) {ival = 1;} else {ival = 0;};
    /* ALTERNATE EQ EXHAUST OFF */
    ret = digital_output(te, MCP23017_TYPE, dev, 0, &ival);
    /* ALTERNATE EQ SUPPLY OFF */
    ret = digital_output(te, MCP23017_TYPE, dev, 1, &ival);
    /* NOT ARMED Annunciator */
    ret = digital_output(te, MCP23017_TYPE, dev, 2, &ival);
    /* CALL Annunciator */
    ret = digital_output(te, MCP23017_TYPE, dev, 3, &ival);

    
    /* Left Retractable Landing Light */
    int *l_land_retractable_up = link_dataref_cmd_once("laminar/B738/switch/land_lights_ret_left_up");
    int *l_land_retractable_dn = link_dataref_cmd_once("laminar/B738/switch/land_lights_ret_left_dn");
    float *l_land_retractable_pos = link_dataref_flt("laminar/B738/switch/land_lights_ret_left_pos",0);
    ival = INT_MISS;
    ival2 = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 4, &ival, 0);
    ret = digital_input(te, MCP23017_TYPE, dev, 5, &ival2, 0);
    float l_land_retractable = FLT_MISS;
    if ((ival2 != INT_MISS) && (ival != INT_MISS)) l_land_retractable = (float) (ival + ival2);
    ret = set_state_updnf(&l_land_retractable,l_land_retractable_pos,l_land_retractable_dn,l_land_retractable_up);
    if (ret != 0) {
      printf("Left Retractable Landing Light %i \n",(int) l_land_retractable);
    }

     /* Right Retractable Landing Light */
    int *r_land_retractable_up = link_dataref_cmd_once("laminar/B738/switch/land_lights_ret_right_up");
    int *r_land_retractable_dn = link_dataref_cmd_once("laminar/B738/switch/land_lights_ret_right_dn");
    float *r_land_retractable_pos = link_dataref_flt("laminar/B738/switch/land_lights_ret_right_pos",0);
    ival = INT_MISS;
    ival2 = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 6, &ival, 0);
    ret = digital_input(te, MCP23017_TYPE, dev, 7, &ival2, 0);
    float r_land_retractable = FLT_MISS;
    if ((ival2 != INT_MISS) && (ival != INT_MISS)) r_land_retractable = (float) (ival + ival2);
    ret = set_state_updnf(&r_land_retractable,r_land_retractable_pos,r_land_retractable_dn,r_land_retractable_up);
    if (ret != 0) {
      printf("Right Retractable Landing Light %i \n",(int) r_land_retractable);
    }

    /* Left Landing Light */
    int *l_land_off = link_dataref_cmd_once("laminar/B738/switch/land_lights_left_off");
    int *l_land_on = link_dataref_cmd_once("laminar/B738/switch/land_lights_left_on");
    float *l_land_pos = link_dataref_flt("laminar/B738/switch/land_lights_left_pos",0);
    float l_land = FLT_MISS;
    ret = digital_inputf(te, MCP23017_TYPE, dev, 8, &l_land, 0);
    ret = set_state_updnf(&l_land,l_land_pos,l_land_on,l_land_off);
    if (ret != 0) {
      printf("Left Landing Light %i \n",(int) l_land);
    }

    /* Right Landing Light */
    int *r_land_off = link_dataref_cmd_once("laminar/B738/switch/land_lights_right_off");
    int *r_land_on = link_dataref_cmd_once("laminar/B738/switch/land_lights_right_on");
    float *r_land_pos = link_dataref_flt("laminar/B738/switch/land_lights_right_pos",0);
    float r_land = FLT_MISS;
    ret = digital_inputf(te, MCP23017_TYPE, dev, 9, &r_land, 0);
    ret = set_state_updnf(&r_land,r_land_pos,r_land_on,r_land_off);
    if (ret != 0) {
      printf("Right Landing Light %i \n",(int) r_land);
    }

    /* Left Runway Turnoff Light */
    int *l_rwy_off = link_dataref_cmd_once("laminar/B738/switch/rwy_light_left_off");
    int *l_rwy_on = link_dataref_cmd_once("laminar/B738/switch/rwy_light_left_on");
    float *l_rwy_pos = link_dataref_flt("laminar/B738/toggle_switch/rwy_light_left",0);
    float l_rwy = FLT_MISS;
    ret = digital_inputf(te, MCP23017_TYPE, dev, 10, &l_rwy, 0);
    ret = set_state_updnf(&l_rwy,l_rwy_pos,l_rwy_on,l_rwy_off);
    if (ret != 0) {
      printf("Left RWY TURNOFF Light %i \n",(int) l_rwy);
    }
    
    /* Right Runway Turnoff Light */
    int *r_rwy_off = link_dataref_cmd_once("laminar/B738/switch/rwy_light_right_off");
    int *r_rwy_on = link_dataref_cmd_once("laminar/B738/switch/rwy_light_right_on");
    float *r_rwy_pos = link_dataref_flt("laminar/B738/toggle_switch/rwy_light_right",0);
    float r_rwy = FLT_MISS;
    ret = digital_inputf(te, MCP23017_TYPE, dev, 11, &r_rwy, 0);
    ret = set_state_updnf(&r_rwy,r_rwy_pos,r_rwy_on,r_rwy_off);
    if (ret != 0) {
      printf("Right RWY TURNOFF Light %i \n",(int) r_rwy);
    }
   
    /* Taxi Light */
    int *taxi_off = link_dataref_cmd_once("laminar/B738/toggle_switch/taxi_light_brightness_off");
    int *taxi_on = link_dataref_cmd_once("laminar/B738/toggle_switch/taxi_light_brightness_on");
    float *taxi_pos = link_dataref_flt("laminar/B738/toggle_switch/taxi_light_brightness_pos",0);
    float taxi = FLT_MISS;
    ret = digital_inputf(te, MCP23017_TYPE, dev, 12, &taxi, 0);
    if (taxi == 1.0) taxi = 2.0; // Taxi light dataref is either 0 or 2 (?)
    ret = set_state_updnf(&taxi,taxi_pos,taxi_on,taxi_off);
    if (ret != 0) {
      printf("Taxi Light %f \n", taxi);
    }

    /* APU Switch */
    int *apu_dn = link_dataref_cmd_once("laminar/B738/spring_toggle_switch/APU_start_pos_dn");
    int *apu_up = link_dataref_cmd_once("laminar/B738/spring_toggle_switch/APU_start_pos_up");
    int *apu_hold_dn = link_dataref_cmd_hold("laminar/B738/spring_toggle_switch/APU_start_pos_dn");
    float *apu_pos = link_dataref_flt("laminar/B738/spring_toggle_switch/APU_start_pos",0);
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 13, &ival, 0);
    float apu = FLT_MISS;
    if (*apu_pos <= 1.0) {
      if ((ival != INT_MISS) && (ival2 != INT_MISS)) apu = (float) (1 - ival); 
      ret = set_state_updnf(&apu,apu_pos,apu_dn,apu_up);
      if (ret != 0) {
	printf("APU Switch ON %i \n",(int) apu);
      }
    }
    if (*apu_pos >= 1.0) {
      ret = digital_input(te, MCP23017_TYPE, dev, 14, apu_hold_dn, 0);
      if (ret != 0) {
	printf("APU Switch START %i \n", *apu_hold_dn);
      }
    }
    
    /* --------------------------------------- */
    /*  AIRCRAFT LIGHTS AND ENGINE START Panel */
    /* --------------------------------------- */

    dev = 10;

    /* Ignitior Switch */
    float *ign_source = link_dataref_flt("laminar/B738/toggle_switch/eng_start_source",0);
    *ign_source = 0.0;
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 8, &ival, 0);
    if (ival == 1) *ign_source = 1.0;
    if (ret == 1) {
      printf("IGN SOURCE %i \n",(int) *ign_source);
    }
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 9, &ival, 0);
    if (ival == 1) *ign_source = -1.0;
    if (ret == 1) {
      printf("IGN SOURCE %i \n",(int) *ign_source);
    }

    /* Left Engine Start Knob */
    int *l_eng_start_left = link_dataref_cmd_once("laminar/B738/knob/eng1_start_left");
    int *l_eng_start_right = link_dataref_cmd_once("laminar/B738/knob/eng1_start_right");
    float *l_eng_start_pos = link_dataref_flt("laminar/B738/engine/starter1_pos",-1);
    
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 0, &ival, 0);
    if (ival == 1) l_eng_start = 0.0; // GRD
    ret = digital_input(te, MCP23017_TYPE, dev, 1, &ival, 0);
    if (ival == 1) l_eng_start = 1.0; // AUTO
    ret = digital_input(te, MCP23017_TYPE, dev, 2, &ival, 0);
    if (ival == 1) l_eng_start = 2.0; // CONT
    ret = digital_input(te, MCP23017_TYPE, dev, 3, &ival, 0);
    if (ival == 1) l_eng_start = 3.0; // FLT

    if ((l_eng_start < 0.0) || (l_eng_start > 3.0) || (ival == INT_MISS)) l_eng_start = FLT_MISS;

    ret = set_state_updnf(&l_eng_start,l_eng_start_pos,l_eng_start_right,l_eng_start_left);
    if (ret != 0) {
      printf("L Engine Start %f %f %i %i \n",
	     l_eng_start,*l_eng_start_pos,*l_eng_start_right,*l_eng_start_left);
    }

    /* SET RELAY */
    ival = 0;
    if ((*l_eng_start_pos <= 0.5) && (*l_eng_start_pos != FLT_MISS)) ival = 1;
    ret = digital_output(te, TEENSY_TYPE, 0, 30, &ival);

    /* Right Engine Start Knob */
    int *r_eng_start_left = link_dataref_cmd_once("laminar/B738/knob/eng2_start_left");
    int *r_eng_start_right = link_dataref_cmd_once("laminar/B738/knob/eng2_start_right");
    float *r_eng_start_pos = link_dataref_flt("laminar/B738/engine/starter2_pos",-1);

    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 4, &ival, 0);
    if (ival == 1) r_eng_start = 0.0; // GRD
    ret = digital_input(te, MCP23017_TYPE, dev, 5, &ival, 0);
    if (ival == 1) r_eng_start = 1.0; // AUTO
    ret = digital_input(te, MCP23017_TYPE, dev, 6, &ival, 0);
    if (ival == 1) r_eng_start = 2.0; // CONT
    ret = digital_input(te, MCP23017_TYPE, dev, 7, &ival, 0);
    if (ival == 1) r_eng_start = 3.0; // FLT

    if ((r_eng_start < 0.0) || (r_eng_start > 3.0) || (ival == INT_MISS)) r_eng_start = FLT_MISS;
    
    ret = set_state_updnf(&r_eng_start,r_eng_start_pos,r_eng_start_right,r_eng_start_left);
    if (ret != 0) {
      printf("R Engine Start %f %f %i %i \n",
	     r_eng_start,*r_eng_start_pos,*r_eng_start_right,*r_eng_start_left);
    }

    /* SET RELAY */
    ival = 0;
    if ((*r_eng_start_pos <= 0.5) && (*r_eng_start_pos != FLT_MISS)) ival = 1;
    ret = digital_output(te, TEENSY_TYPE, 0, 31, &ival);

    
    /* Logo */
    int *logo_off = link_dataref_cmd_once("laminar/B738/switch/logo_light_off");
    int *logo_on = link_dataref_cmd_once("laminar/B738/switch/logo_light_on");
    float *logo_pos = link_dataref_flt("laminar/B738/toggle_switch/logo_light",0);
    float logo = FLT_MISS;
    ret = digital_inputf(te, MCP23017_TYPE, dev, 12, &logo, 0);
    ret = set_state_updnf(&logo,logo_pos,logo_on,logo_off);
    if (ret != 0) {
      printf("Logo Light %i \n",(int) logo);
    }

    /* Position Steady */
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 11, &ival, 0);
    /* Position Strobe */
    ival2 = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 14, &ival2, 0);
    int *position_up = link_dataref_cmd_once("laminar/B738/toggle_switch/position_light_up");
    int *position_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/position_light_down");
    float *position_pos = link_dataref_flt("laminar/B738/toggle_switch/position_light_pos",0);
    float position = FLT_MISS;
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) position = (float) (-ival + ival2);
    ret = set_state_updnf(&position,position_pos,position_up,position_dn);
    if (ret != 0) {
      printf("Position Light %i \n",(int) position);
    }

    /* Anti Collision: ZIBO INOP? */
    int *beacon = link_dataref_int("sim/cockpit/electrical/beacon_lights_on");
    ret = digital_input(te, MCP23017_TYPE, dev, 10, beacon, 0);
    if (ret == 1) {
      printf("Anti Collision Light %i \n", *beacon);
    }

    /* Wing: ZIBO need state dataref */
    int *wing_off = link_dataref_cmd_once("laminar/B738/switch/wing_light_off");
    int *wing_on = link_dataref_cmd_once("laminar/B738/switch/wing_light_on");
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 15, &ival, 0);
    if ((ret == 1) && (ival == 1)) *wing_on = 1;
    if ((ret == 1) && (ival == 0)) *wing_off = 1;
    if (ret == 1) {
      printf("Wing Light %i \n", ival);
    }

    /* Wheel Well */
    int *wheel_off = link_dataref_cmd_once("laminar/B738/switch/wheel_light_off");
    int *wheel_on = link_dataref_cmd_once("laminar/B738/switch/wheel_light_on");
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 13, &ival, 0);
    if ((ret == 1) && (ival == 0)) *wheel_on = 1;
    if ((ret == 1) && (ival == 1)) *wheel_off = 1;        
    if (ret == 1) {
      printf("Wheel Well Light %i \n", ival);
    }

    
    /* -------------------------- */
    /*  WINDOW & PROBE HEAT Panel */
    /* -------------------------- */
    
    dev = 11;

    int *tat_test = link_dataref_cmd_hold("laminar/B738/push_button/tat_test");
    ret = digital_input(te, MCP23017_TYPE, dev, 0, tat_test, 0);
    if (ret == 1) {
      printf("TAT Test: %i \n",*tat_test);
    }

    float *probe_heat_fo = link_dataref_flt("laminar/B738/toggle_switch/fo_probes_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 1, probe_heat_fo, 0);
    if (ret == 1) {
      printf("Probe Heat First Officer: %i \n",(int) *probe_heat_fo);
    }
 
    float *window_heat_r_side = link_dataref_flt("laminar/B738/ice/window_heat_r_side_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 2, window_heat_r_side, 0);
    if (ret == 1) {
      printf("Window Heat Right Side: %i \n",(int) *window_heat_r_side);
    }

    float *window_heat_r_fwd = link_dataref_flt("laminar/B738/ice/window_heat_r_fwd_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 3, window_heat_r_fwd, 0);
    if (ret == 1) {
      printf("Window Heat Right forward: %i \n",(int) *window_heat_r_fwd);
    }

    float *window_overheat_test = link_dataref_flt("laminar/B738/toggle_switch/window_ovht_test",0);
    ival = INT_MISS;
    ival2 = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 4, &ival2, 0);
    if (ret == 1) {
      printf("PROBE HEAT OVHT TEST: %i \n",ival2);
    }
    ret = digital_input(te, MCP23017_TYPE, dev, 5, &ival, 0);
    if (ret == 1) {
      printf("PROBE HEAT PWR TEST: %i \n",ival);
    }
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) *window_overheat_test = (float) (ival - ival2);

    float *window_ovht_r_fwd_ann = link_dataref_flt("laminar/B738/annunciator/window_heat_ovht_rf",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 8, window_ovht_r_fwd_ann);
    float *window_ovht_r_side_ann = link_dataref_flt("laminar/B738/annunciator/window_heat_ovht_rs",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 9, window_ovht_r_side_ann);

    float *window_heat_r_fwd_ann = link_dataref_flt("laminar/B738/annunciator/window_heat_r_fwd",-2);
    if (*avionics_on) {
      ret = digital_outputf(te, MCP23017_TYPE, dev, 10, window_heat_r_fwd_ann);
    } else {
      ret = digital_output(te, MCP23017_TYPE, dev, 10, &zero);
    }
    float *window_heat_r_side_ann = link_dataref_flt("laminar/B738/annunciator/window_heat_r_side",-2);
    if (*avionics_on) {
      ret = digital_outputf(te, MCP23017_TYPE, dev, 11, window_heat_r_side_ann);
    } else {
      ret = digital_output(te, MCP23017_TYPE, dev, 11, &zero);
    }

    float *fo_pitot = link_dataref_flt("laminar/B738/annunciator/fo_pitot_off",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 12, fo_pitot); // FO PITOT
    ret = digital_outputf(te, MCP23017_TYPE, dev, 13, fo_pitot); // R ELEV PITOT
    ret = digital_outputf(te, MCP23017_TYPE, dev, 15, fo_pitot); // AUX PITOT
    float *fo_aoa = link_dataref_flt("laminar/B738/annunciator/fo_aoa_off",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 14, fo_aoa); // R ALPHA VANE

    dev = 12;
    float *window_heat_l_side = link_dataref_flt("laminar/B738/ice/window_heat_l_side_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 8, window_heat_l_side, 0);
    if (ret == 1) {
      printf("Window Heat Left Side: %i \n",(int) *window_heat_l_side);
    }
    
    float *window_heat_l_fwd = link_dataref_flt("laminar/B738/ice/window_heat_l_fwd_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 9, window_heat_l_fwd, 0);
    if (ret == 1) {
      printf("Window Heat Left forward: %i \n",(int) *window_heat_l_fwd);
    }
    
    float *probe_heat_capt = link_dataref_flt("laminar/B738/toggle_switch/capt_probes_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 10, probe_heat_capt, 0);
    if (ret == 1) {
      printf("Probe Heat Captain: %i \n",(int) *probe_heat_capt);
    }

    float *window_ovht_l_side_ann = link_dataref_flt("laminar/B738/annunciator/window_heat_ovht_ls",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 0, window_ovht_l_side_ann);
    float *window_ovht_l_fwd_ann = link_dataref_flt("laminar/B738/annunciator/window_heat_ovht_lf",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 1, window_ovht_l_fwd_ann);

    float *window_heat_l_side_ann = link_dataref_flt("laminar/B738/annunciator/window_heat_l_side",-2);
    if (*avionics_on) {
      ret = digital_outputf(te, MCP23017_TYPE, dev, 2, window_heat_l_side_ann);
    } else {
      ret = digital_output(te, MCP23017_TYPE, dev, 2, &zero);
    }
    float *window_heat_l_fwd_ann = link_dataref_flt("laminar/B738/annunciator/window_heat_l_fwd",-2);
    if (*avionics_on) {
      ret = digital_outputf(te, MCP23017_TYPE, dev, 3, window_heat_l_fwd_ann);
    } else {
      ret = digital_output(te, MCP23017_TYPE, dev, 3, &zero);
    }

    float *capt_pitot = link_dataref_flt("laminar/B738/annunciator/capt_pitot_off",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 4, capt_pitot); // CAPT PITOT
    ret = digital_outputf(te, MCP23017_TYPE, dev, 5, capt_pitot); // L ELEV PITOT
    ret = digital_outputf(te, MCP23017_TYPE, dev, 7, capt_pitot); // TEMP PROBE
    float *capt_aoa = link_dataref_flt("laminar/B738/annunciator/capt_aoa_off",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 6, capt_aoa); // L ALPHA VANE

    
    /* ------------------------------ */
    /*  Anti Ice and Hydraulics Panel */
    /* ------------------------------ */

    dev = 13;
    
    float *hydro_pumps1 = link_dataref_flt("laminar/B738/toggle_switch/hydro_pumps1_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 8, hydro_pumps1, 0);
    if (ret == 1) {
      printf("Hydraulic Pumps 1: %i \n",(int) *hydro_pumps1);
    }
    float *elec_hydro_pumps1 = link_dataref_flt("laminar/B738/toggle_switch/electric_hydro_pumps1_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 9, elec_hydro_pumps1, 0);
    if (ret == 1) {
      printf("Electric Hydraulic Pumps 1: %i \n",(int) *elec_hydro_pumps1);
    }
    float *elec_hydro_pumps2 = link_dataref_flt("laminar/B738/toggle_switch/electric_hydro_pumps2_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 10, elec_hydro_pumps2, 0);
    if (ret == 1) {
      printf("Electric Hydraulic Pumps 2: %i \n",(int) *elec_hydro_pumps2);
    }
    float *hydro_pumps2 = link_dataref_flt("laminar/B738/toggle_switch/hydro_pumps2_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 11, hydro_pumps2, 0);
    if (ret == 1) {
      printf("Hydraulic Pumps 2: %i \n",(int) *hydro_pumps2);
    }

    float *wing_anti_ice_pos = link_dataref_flt("laminar/B738/ice/wing_heat_pos",0);
    int *wing_anti_ice_toggle = link_dataref_cmd_once("laminar/B738/toggle_switch/wing_heat");
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 12, &ival, 0);
    if (ival != INT_MISS) {
      if (ival == 1) { fval = 1.0; } else { fval = 0.0; };
    } else {
      fval = FLT_MISS;
    }
    ret = set_state_togglef(&fval,wing_anti_ice_pos,wing_anti_ice_toggle);
    if (ret != 0) {
      printf("WING ANTI ICE %i %f %i \n",ival, *wing_anti_ice_pos, *wing_anti_ice_toggle);
    }

    /* Wing Anti Ice Coil: activate if X-Plane's Wing Anti Ice Switch is ON.
       This needs at least one ping pong round of signal between
       pressing the hardware switch and receiving the switch position
       back from X-Plane. But this allows that X-Plane cancels the coil. */
    ret = digital_outputf(te, TEENSY_TYPE, 0, 32, wing_anti_ice_pos);

    float *eng1_anti_ice = link_dataref_flt("laminar/B738/ice/eng1_heat_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 13, eng1_anti_ice, 0);
    if (ret == 1) {
      printf("Engine 1 Anti Ice: %i \n",(int) *eng1_anti_ice);
    }
    float *eng2_anti_ice = link_dataref_flt("laminar/B738/ice/eng2_heat_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 14, eng2_anti_ice, 0);
    if (ret == 1) {
      printf("Engine 2 Anti Ice: %i \n",(int) *eng2_anti_ice);
    }

    /* Yellow Annunciators */
    float *elec_hydro_ovht1 = link_dataref_flt("laminar/B738/annunciator/el_hyd_ovht_1",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 0, elec_hydro_ovht1);
    float *elec_hydro_ovht2 = link_dataref_flt("laminar/B738/annunciator/el_hyd_ovht_2",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 1, elec_hydro_ovht2);
    float *hydro_press1 = link_dataref_flt("laminar/B738/annunciator/hyd_press_a",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 2, hydro_press1);
    float *elec_hydro_press1 = link_dataref_flt("laminar/B738/annunciator/hyd_el_press_a",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 3, elec_hydro_press1);
    float *elec_hydro_press2 = link_dataref_flt("laminar/B738/annunciator/hyd_el_press_b",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 4, elec_hydro_press2);
    float *hydro_press2 = link_dataref_flt("laminar/B738/annunciator/hyd_press_b",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 5, hydro_press2);
    float *cowl_anti_ice1 = link_dataref_flt("laminar/B738/annunciator/cowl_ice_0",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 6, cowl_anti_ice1);
    float *cowl_anti_ice2 = link_dataref_flt("laminar/B738/annunciator/cowl_ice_1",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 7, cowl_anti_ice2);

    /* Blue Annunciators */
    dev = 1;
    float *wing_anti_ice1 = link_dataref_flt("laminar/B738/annunciator/wing_ice_on_L",-2);
    ret = pwm_output(te, PCA9685_TYPE, dev, 12, wing_anti_ice1,0.0,0.05);
    float *wing_anti_ice2 = link_dataref_flt("laminar/B738/annunciator/wing_ice_on_R",-2);
    ret = pwm_output(te, PCA9685_TYPE, dev, 13, wing_anti_ice2,0.0,0.05);
    float *cowl_valve_open1 = link_dataref_flt("laminar/B738/annunciator/cowl_ice_on_0",-2);
    ret = pwm_output(te, PCA9685_TYPE, dev, 14, cowl_valve_open1,0.0,0.05);
    float *cowl_valve_open2 = link_dataref_flt("laminar/B738/annunciator/cowl_ice_on_1",-2);
    ret = pwm_output(te, PCA9685_TYPE, dev, 15, cowl_valve_open2,0.0,0.05);

    /* ------------------------------ */
    /*  Doors, CVR and Altitude Panel */
    /* ------------------------------ */
    
    dev = 14;
    
    float *fwd_entry_door = link_dataref_flt("laminar/B738/annunciator/fwd_entry",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 0, fwd_entry_door);
    float *fwd_service_door = link_dataref_flt("laminar/B738/annunciator/fwd_service",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 1, fwd_service_door);
    float *left_fwd_overwing_door = link_dataref_flt("laminar/B738/annunciator/left_fwd_overwing",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 2, left_fwd_overwing_door);
    float *right_fwd_overwing_door = link_dataref_flt("laminar/B738/annunciator/right_fwd_overwing",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 3, right_fwd_overwing_door);
    float *fwd_cargo_door = link_dataref_flt("laminar/B738/annunciator/fwd_cargo",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 4, fwd_cargo_door);
    float *equip_door = link_dataref_flt("laminar/B738/annunciator/equip_door",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 5, equip_door);
    float *left_aft_overwing_door = link_dataref_flt("laminar/B738/annunciator/left_aft_overwing",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 6, left_aft_overwing_door);
    float *right_aft_overwing_door = link_dataref_flt("laminar/B738/annunciator/right_aft_overwing",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 7, right_aft_overwing_door);
    float *aft_cargo_door = link_dataref_flt("laminar/B738/annunciator/aft_cargo",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 8, aft_cargo_door);
    float *aft_entry_door = link_dataref_flt("laminar/B738/annunciator/aft_entry",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 9, aft_entry_door);
    float *aft_service_door = link_dataref_flt("laminar/B738/annunciator/aft_service",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 10, aft_service_door);

    /* Altitude Warning Horn Cutout */
    int *alt_horn_cutout = link_dataref_cmd_hold("laminar/B738/alert/alt_horn_cutout");
    ret = digital_input(te, MCP23017_TYPE, dev, 14, alt_horn_cutout, 0);

    float *cabin_climb = link_dataref_flt("laminar/B738/cabin_vvi",1);    
    float *cabin_altitude = link_dataref_flt("laminar/B738/cabin_alt",2);
    float *cabin_pressure_diff = link_dataref_flt("laminar/B738/cabin_pressure_diff",-1);

    dev = 0;
    if (servotest == 1) {
      ret = servo_output(te, PCA9685_TYPE, dev, 3, &servoval,0.0,100.0,0.10,0.90);
      ret = servo_output(te, PCA9685_TYPE, dev, 4, &servoval,0.0,100.0,0.06,0.97);
      ret = servo_output(te, PCA9685_TYPE, dev, 5, &servoval,0.0,100.0,0.03,0.99);
   } else {
      ret = servo_output(te, PCA9685_TYPE, dev, 3, cabin_climb,-2000.0,2000.0,0.11,0.90);
      ret = servo_output(te, PCA9685_TYPE, dev, 4, cabin_altitude,0.0,28000,0.06,0.97);
      ret = servo_output(te, PCA9685_TYPE, dev, 5, cabin_pressure_diff,0.0,8.0,0.03,0.99);
    }
     
    /* ------------------ */
    /* TEMP CONTROL PANEL */
    /* ------------------ */

    /* Air Temperature Source Selector Knob */
    dev = 15;
    
    float *air_temp_source = link_dataref_flt("laminar/B738/toggle_switch/air_temp_source",0);
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 0, &ival, 0);
    if (ival == 1) *air_temp_source = 0.0; // DUCT SUPPLY CONT CAB
    ret = digital_input(te, MCP23017_TYPE, dev, 1, &ival, 0);
    if (ival == 1) *air_temp_source = 1.0; // DUCT SUPPLY FWD
    ret = digital_input(te, MCP23017_TYPE, dev, 2, &ival, 0);
    if (ival == 1) *air_temp_source = 2.0; // DUCT SUPPLY AFT
    ret = digital_input(te, MCP23017_TYPE, dev, 3, &ival, 0);
    if (ival == 1) *air_temp_source = 3.0; // PASS CAB FWD
    ret = digital_input(te, MCP23017_TYPE, dev, 4, &ival, 0);
    if (ival == 1) *air_temp_source = 4.0; // PASS CAB AFT
    ret = digital_input(te, MCP23017_TYPE, dev, 5, &ival, 0);
    if (ival == 1) *air_temp_source = 5.0; // PACK R
    ret = digital_input(te, MCP23017_TYPE, dev, 6, &ival, 0);
    if (ival == 1) *air_temp_source = 6.0; // PACK L

    /* Trim Air Switch */
    float *trim_air = link_dataref_flt("laminar/B738/air/trim_air_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 7, trim_air, 0);

    /* Yellow Annunciators (INOP IN ZIBO)*/   
    if ((*lights_test == 1.0) && (*avionics_on == 1)) {ival = 1;} else {ival = 0;};
    /* CONT CAB ZONE TEMP ANNUNCIATOR */
    ret = digital_output(te, MCP23017_TYPE, dev, 8, &ival);
    /* FWD CAB ZONE TEMP ANNUNCIATOR */
    ret = digital_output(te, MCP23017_TYPE, dev, 9, &ival);
    /* AFT CAB ZONE TEMP ANNUNCIATOR */
    ret = digital_output(te, MCP23017_TYPE, dev, 10, &ival);

    /* Zone Temp Potentiometers */
    float *cont_cab_rheostat = link_dataref_flt("laminar/B738/air/cont_cab_temp/rheostat",-2);
    ret = analog_input(te,38,cont_cab_rheostat,0.0,1.0);
    if (ret == 1) {
      printf("CONT CAB TEMP RHEO: %f \n",*cont_cab_rheostat);
    }
    float *fwd_cab_rheostat = link_dataref_flt("laminar/B738/air/fwd_cab_temp/rheostat",-2);
    ret = analog_input(te,39,fwd_cab_rheostat,0.0,1.0);
    if (ret == 1) {
      printf("FWD CAB TEMP RHEO: %f \n",*fwd_cab_rheostat);
    }
    float *aft_cab_rheostat = link_dataref_flt("laminar/B738/air/aft_cab_temp/rheostat",-2);
    ret = analog_input(te,40,aft_cab_rheostat,0.0,1.0);
    if (ret == 1) {
      printf("AFT CAB TEMP RHEO: %f \n",*aft_cab_rheostat);
    }
  
    /* Temp indicator Gauge Servo */
    dev = 0;
    float *zone_temp = link_dataref_flt("laminar/B738/zone_temp",0);
    if (servotest == 1) {
      ret = servo_output(te, PCA9685_TYPE, dev, 0, &servoval,0.0,100.0,0.05,0.9);
    } else {
      ret = servo_output(te, PCA9685_TYPE, dev, 0, zone_temp,0.0,100.0,0.05,0.9);
    }

    
    /* --------------- */
    /* Air Bleed Panel */
    /* --------------- */

    dev = 16;

    /* L Recirculation Fan Switch */
    float *l_recirc_fan = link_dataref_flt("laminar/B738/air/l_recirc_fan_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 0, l_recirc_fan, 0);
    if (ret == 1) {
      printf("L RECIRC FAN AUTO %i \n",(int) *l_recirc_fan);
    }
    /* R Recirculation Fan Switch */
    float *r_recirc_fan = link_dataref_flt("laminar/B738/air/r_recirc_fan_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 1, r_recirc_fan, 0);
    if (ret == 1) {
      printf("R RECIRC FAN AUTO %i \n",(int) *r_recirc_fan);
    }

    /* L Pack Switch */
    float *l_pack = link_dataref_flt("laminar/B738/air/l_pack_pos",0);
    ival = INT_MISS;
    ival2 = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 2, &ival, 0);
    ret = digital_input(te, MCP23017_TYPE, dev, 3, &ival2, 0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) {
      *l_pack = (float) ((1-ival) + ival2);
    }
    if ((ret == 1) || (ret2 == 1)) {
      printf("L PACK SWITCH %i \n",(int) *l_pack);
    }

    /* Isolation Valve Switch */
    float *isolation_valve = link_dataref_flt("laminar/B738/air/isolation_valve_pos",0);
    ival = INT_MISS;
    ival2 = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 4, &ival, 0);
    ret = digital_input(te, MCP23017_TYPE, dev, 5, &ival2, 0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) {
      *isolation_valve = (float) ((1-ival) + ival2);
    }
    if ((ret == 1) || (ret2 == 1)) {
      printf("ISOLATION VALVE SWITCH %i \n",(int) *isolation_valve);
    }
    
    /* R Pack Switch */
    float *r_pack = link_dataref_flt("laminar/B738/air/r_pack_pos",0);
    ival = INT_MISS;
    ival2 = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 6, &ival, 0);
    ret = digital_input(te, MCP23017_TYPE, dev, 7, &ival2, 0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) {
      *r_pack = (float) ((1-ival) + ival2);
    }
    if ((ret == 1) || (ret2 == 1)) {
      printf("R PACK SWITCH %i \n",(int) *r_pack);
    }

    float *dual_bleed = link_dataref_flt("laminar/B738/annunciator/dual_bleed",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 8, dual_bleed);
    float *pack_left = link_dataref_flt("laminar/B738/annunciator/pack_left",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 9, pack_left);
    float *wing_ovht_left = link_dataref_flt("laminar/B738/annunciator/wing_body_ovht_left",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 10, wing_ovht_left);
    float *bleed_trip_left = link_dataref_flt("laminar/B738/annunciator/bleed_trip_1",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 11, bleed_trip_left);
    float *pack_right = link_dataref_flt("laminar/B738/annunciator/pack_right",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 12, pack_right);
    float *wing_ovht_right = link_dataref_flt("laminar/B738/annunciator/wing_body_ovht_right",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 13, wing_ovht_right);
    float *bleed_trip_right = link_dataref_flt("laminar/B738/annunciator/bleed_trip_2",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 14, bleed_trip_right);
    
    dev = 17;

    /* Duct Overheat Test */
    int *duct_ovht_test = link_dataref_cmd_hold("laminar/B738/push_button/duct_ovht_test");
    ret = digital_input(te, MCP23017_TYPE, dev, 0, duct_ovht_test, 0);
    if (ret == 1) {
      printf("DUCT OVHT TEST %i \n", *duct_ovht_test);
    }
    
    /* Trip Reset */
    int *trip_reset = link_dataref_cmd_hold("laminar/B738/push_button/bleed_trip_reset");
    ret = digital_input(te, MCP23017_TYPE, dev, 1, trip_reset, 0);
    if (ret == 1) {
      printf("TRIP RESET %i \n", *trip_reset);
    }
    
    /* L Bleed Air Switch */
    float *l_bleed_air = link_dataref_flt("laminar/B738/toggle_switch/bleed_air_1_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 2, l_bleed_air, 0);
    if (ret == 1) {
      printf("L BLEED AIR %i \n",(int) *l_bleed_air);
    }

    /* APU Bleed Air Switch */
    float *apu_bleed_air = link_dataref_flt("laminar/B738/toggle_switch/bleed_air_apu_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 3, apu_bleed_air, 0);
    if (ret == 1) {
      printf("APU BLEED AIR %i \n",(int) *apu_bleed_air);
    }

    /* R Bleed Air Switch */
    float *r_bleed_air = link_dataref_flt("laminar/B738/toggle_switch/bleed_air_2_pos",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 4, r_bleed_air, 0);
    if (ret == 1) {
      printf("R BLEED AIR %i \n",(int) *r_bleed_air);
    }


    float *auto_fail = link_dataref_flt("laminar/B738/annunciator/autofail",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 8, auto_fail);
    float *off_sched_descent = link_dataref_flt("laminar/B738/annunciator/off_sched_descent",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 9, off_sched_descent);
    float *altn_press = link_dataref_flt("laminar/B738/annunciator/altn_press",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 10, altn_press);
    float *manual_press = link_dataref_flt("laminar/B738/annunciator/manual_press",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 11, manual_press);

    /* Blue Annunciators */
    dev = 1;
    float *l_ram_door = link_dataref_flt("laminar/B738/annunciator/ram_door_open1",-2);
    ret = pwm_output(te, PCA9685_TYPE, dev, 6, l_ram_door,0.0,0.05);    
    float *r_ram_door = link_dataref_flt("laminar/B738/annunciator/ram_door_open2",-2);
    ret = pwm_output(te, PCA9685_TYPE, dev, 7, r_ram_door,0.0,0.05);    

    /* Duct Press Gauge */
    dev = 0;
    float *duct_press_left = link_dataref_flt("laminar/B738/indicators/duct_press_L",0);
    float *duct_press_right = link_dataref_flt("laminar/B738/indicators/duct_press_R",0);
    if (servotest == 1) {
      ret = servo_output(te, PCA9685_TYPE, dev, 6, &servoval,0.0,100.0,0.02,0.93);
      ret = servo_output(te, PCA9685_TYPE, dev, 7, &servoval,0.0,100.0,0.02,0.88);
    } else {
      ret = servo_output(te, PCA9685_TYPE, dev, 6, duct_press_left,0.0,80.0,0.02,0.93);
      ret = servo_output(te, PCA9685_TYPE, dev, 7, duct_press_right,0.0,80.0,0.02,0.88);
    }

    
    /* -------------- */
    /* ALTITUDE Panel */
    /* -------------- */

    dev = 18;
     
    float *flt_alt = link_dataref_flt("sim/cockpit2/pressurization/actuators/max_allowable_altitude_ft",0);
    float *land_alt = link_dataref_flt("laminar/B738/pressurization/knobs/landing_alt",0);

    ret = encoder_inputf(te, MCP23017_TYPE, dev, 0, 1, flt_alt, 50, 2);
    if (*flt_alt != FLT_MISS) {
      if (*flt_alt < -1000.0) *flt_alt = -1000.0;
      if (*flt_alt > 42000.0) *flt_alt = 42000.0;
    }
    if (ret == 1) printf("Flight Altitude Selector: %f \n",*flt_alt);
    
    ret = encoder_inputf(te, MCP23017_TYPE, dev, 3, 2, land_alt, 50, 2);
    if (*land_alt != FLT_MISS) {
      if (*land_alt < -1000.0) *land_alt = -1000.0;
      if (*land_alt > 42000.0) *land_alt = 42000.0;
    }
    if (ret == 1) printf("Landing Altitude Selector: %f \n",*land_alt);

    int *air_valve_manual_left = link_dataref_cmd_hold("laminar/B738/toggle_switch/air_valve_manual_left");
    ret = digital_input(te, MCP23017_TYPE, dev, 4, air_valve_manual_left, 0);
    if (ret != 0) {
      printf("Air Valve Manual CLOSED: %i \n",*air_valve_manual_left);
    }
    int *air_valve_manual_right = link_dataref_cmd_hold("laminar/B738/toggle_switch/air_valve_manual_right");
    ret = digital_input(te, MCP23017_TYPE, dev, 5, air_valve_manual_right, 0);
    if (ret != 0) {
      printf("Air Valve Manual OPEN: %i \n",*air_valve_manual_right);
    }

    int *air_valve_ctrl_left = link_dataref_cmd_once("laminar/B738/toggle_switch/air_valve_ctrl_left");
    int *air_valve_ctrl_right = link_dataref_cmd_once("laminar/B738/toggle_switch/air_valve_ctrl_right");
    float *air_valve_ctrl_pos = link_dataref_flt("laminar/B738/toggle_switch/air_valve_ctrl",0);
    ret = digital_input(te, MCP23017_TYPE, dev, 8, &ival, 0);
    if (ival == 1) air_valve_ctrl = 0.0;
    ret = digital_input(te, MCP23017_TYPE, dev, 9, &ival, 0);
    if (ival == 1) air_valve_ctrl = 1.0;
    ret = digital_input(te, MCP23017_TYPE, dev, 10, &ival, 0);
    if (ival == 1) air_valve_ctrl = 2.0;
    ret = set_state_updnf(&air_valve_ctrl,air_valve_ctrl_pos,air_valve_ctrl_right,air_valve_ctrl_left);
    if (ret != 0) {
      printf("ALTN Selector: %i \n",(int) air_valve_ctrl);
    }

    dev = 0;
    float *outflow_valve = link_dataref_flt("laminar/B738/outflow_valve",-2);
    if (servotest == 1) {
      ret = servo_output(te, PCA9685_TYPE, dev, 8, &servoval,0.0,100.0,0.02,0.46);
    } else {
      ret = servo_output(te, PCA9685_TYPE, dev, 8, outflow_valve,0.0,1.0,0.02,0.46);
    }

    dev = 1;
    brightness = 10;
    dp = -1;
    if (*avionics_on == 1) {
      ret = display_outputf(te, HT16K33_TYPE, dev, 0, 5, flt_alt, dp, brightness);
      ret = display_outputf(te, HT16K33_TYPE, dev, 8, 5, land_alt, dp, brightness);
    } else {
      ival = 16;
      for (int i=0;i<16;i++) {
	ret = display_output(te, HT16K33_TYPE, dev, i, 1, &ival, dp, brightness);
      }
    }

  } else {

    /* A few basic switches for other ACF */

    /* Avionics / Battery */
    dev = 5;    
    int *battery_i = link_dataref_int("sim/cockpit/electrical/battery_on");
    int *avionics_i = link_dataref_int("sim/cockpit2/switches/avionics_power_on");
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 1, &ival, 0);
    if (ival != INT_MISS) {
      *battery_i = 1-ival;
      *avionics_i = 1-ival;
    }

    /* Background Lighting (Direct Potentiometer MOSFET control */
    ret = digital_output(te, TEENSY_TYPE, 0, 27, avionics_i);
   
    /* Set ignitior Switch of first engine for all engines */
    dev = 10;
    //int *ignition_on = link_dataref_int_arr("sim/cockpit/engine/ignition_on", 8,-1);
    int *ignition_on = link_dataref_int_arr("sim/cockpit/engine/ignition_on", 16,-1);
    for (int i=0;i<8;i++) {
      ignition_on[i] = 3;
      ival = INT_MISS;
      ret = digital_input(te, MCP23017_TYPE, dev, 9, &ival, 0);
      if (ival == 1) ignition_on[i] = 2;
      ival = INT_MISS;
      ret = digital_input(te, MCP23017_TYPE, dev, 8, &ival, 0);
      if (ival == 1) ignition_on[i] = 1;
    }
    
    /* Engine Starters */
    dev = 10;
    int *engine_start = link_dataref_cmd_hold("sim/engines/engage_starters");
    ret = digital_input(te, MCP23017_TYPE, dev, 0, engine_start, 0);
   
    /* Fuel Pump */
    dev = 3;
    //int *fuel_pump_on = link_dataref_int_arr("sim/cockpit/engine/fuel_pump_on", 8,-1);
    int *fuel_pump_on = link_dataref_int_arr("sim/cockpit/engine/fuel_pump_on", 16,-1);
    ival = INT_MISS;
    ret = digital_input(te, MCP23017_TYPE, dev, 11, &ival, 0);
    if (ival != INT_MISS) {
      for (int i=0;i<8;i++) {
	fuel_pump_on[i] = ival;
      }
    }

  /*   /\* weapon arming for fighter jets *\/ */
  /*   int *weapons_arm_cover_pos = link_dataref_int("laminar/f14/anm/weapons/arm_switch_guard_target_pos"); */
  /*   int *weapons_arm_cover_toggle = link_dataref_cmd_once("laminar/f14/weapons/switch/master_arm_switch_guard"); */
  /*   ival = 1; */
  /*   float cover_pos = FLT_MISS; */
  /*   if (*weapons_arm_cover_pos != INT_MISS) { */
  /*     cover_pos = (float) *weapons_arm_cover_pos; */
  /*   }     */
  /*   ret = set_switch_cover(&cover_pos,weapons_arm_cover_toggle,ival); */

  /*   int *weapons_arm_pos = link_dataref_int("sim/cockpit2/weapons/master_arm"); */
  /*   int *weapons_arm_toggle = link_dataref_cmd_once("sim/weapons/master_arm_on"); */
  /*   device = mastercard; */
  /*   card = 1; */
  /*   ret = digital_input(device,card,16,&ival,0); */
  /*   if (ret == 1) { */
  /*     printf("Weapons armed: %i \n",ival); */
  /*   } */
  /*   ret = set_state_toggle(&ival,weapons_arm_pos,weapons_arm_toggle); */

  /*   device = mastercard; */
  /*   card = 1; */

  /*   /\* GRD CALL BUTTON: air to ground missile *\/ */
  /*   int *air2ground = link_dataref_cmd_once("sim/weapons/fire_air_to_ground"); */
  /*   ret = digital_input(device,card,31,air2ground,0); */
  /*   if (ret == 1) { */
  /*     printf("FIRE AIR TO GROUND MISSILE/BOMB: %i \n",*air2ground); */
  /*   } */
     
  /*   device = mastercard; */
  /*   card = 0; */

  /*   /\* ATTEND BUTTON: air to air missile *\/ */
  /*   int *air2air = link_dataref_cmd_once("sim/weapons/fire_air_to_air"); */
  /*   ret = digital_input(device,card,53,air2air,0); */
  /*   if (ret == 1) { */
  /*     printf("FIRE AIR TO AIR MISSILE: %i \n",*air2air); */
  /*   } */
    
  /*   /\* CVR ERASE: deploy flares *\/ */
  /*   int *flares = link_dataref_cmd_once("sim/weapons/deploy_flares"); */
  /*   ret = digital_input(device,card,70,flares,0); */
  /*   if (ret == 1) { */
  /*     printf("DEPLOY FLARES: %i \n",*flares); */
  /*   } */
  /*   /\* CVR Test: deploy chaff *\/ */
  /*   int *chaff= link_dataref_cmd_once("sim/weapons/deploy_chaff"); */
  /*   ret = digital_input(device,card,71,chaff,0); */
  /*   if (ret == 1) { */
  /*     printf("DEPLOY CHAFF: %i \n",*chaff); */
  /*   } */
    
  }
    
}

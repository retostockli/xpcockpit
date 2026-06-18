/* This is the b737_overheadaft.c code which simulates all I/O to the Boeing 737 aft overhead panel
   using Teensy microcontrollers

   Copyright (C) 2026-2026 Reto Stockli

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
#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>

#include "common.h"
#include "libteensy.h"
#include "serverdata.h"
#include "b737_overheadaft.h"

/*
  LTC4316 Address Translator
  Switch		A4	A5
  XOR 0x40       	ON	ON
  XOR 0x50       	OFF	ON
  XOR 0x60       	ON	OFF
  XOR 0x70       	OFF 	OFF
*/

/* IRS DISP Keypad */
#define nCols 3
#define nRows 4

/* to be preserved variables */
float disp_sel;
int keyvalue[nRows*nCols];
int is_N;
int is_S;
int is_W;
int is_E;
int last;
float irs_l;
float irs_r;
float elt;
float pass_oxy;

int acp3_micsel_vhf1;
int acp3_micsel_vhf2;
int acp3_micsel_vhf3;
int acp3_micsel_hf1;
int acp3_micsel_hf2;
int acp3_micsel_flt;
int acp3_micsel_svc;
int acp3_micsel_pa;
int acp3_rt_ic;
int acp3_sel_v;
int acp3_sel_b;
int acp3_sel_r;
int acp3_sel_vbr;
int acp3_alt_norm;

/* IRS DISP Keypad */
const char zibo_keyname[nRows*nCols][5] = {"_1","_2","_3","_4","_5","_6","_7","_8","_9","_ent","_0","_clr"};

int is_numeric(const unsigned char *s) {
    if (*s == '\0') return 0; // empty string is not numeric

    while (*s) {
        if (!isdigit((unsigned char)*s))
            return 0;
        s++;
    }
    return 1;
}

void init_b737_overheadaft(void)
{
  int te = 3; // Teensy Number (as in ini file)
  int pin;
  int dev;
  int ret;

  /* Teensy I/O Pin configurations */
  //teensy[te].pinmode[0] = PINMODE_INPUT;
  //teensy[te].pinmode[0] = PINMODE_OUTPUT;
  //teensy[te].pinmode[26] = PINMODE_ANALOGINPUTMEAN;

  teensy[te].pinmode[24] = PINMODE_OUTPUT; /* Backlight Relay */
  teensy[te].pinmode[26] = PINMODE_ANALOGINPUTMEAN; /* 7 Seg Disp Potentiometer */

  /* MCP23017 on IRS Display */
  dev = 0;
  for (pin=0;pin<10;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  for (pin=10;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  mcp23017[te][dev].intpin = 0;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x20; // I2C address of MCP23017 device
 
  /* 2 MCP23017 on FLAPS Panel */
  dev = 1;
  for (pin=0;pin<14;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  for (pin=14;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 4;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x21; // I2C address of MCP23017 device

  dev = 2;
  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  mcp23017[te][dev].intpin = INITVAL;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x22; // I2C address of MCP23017 device

  /* MCP23017 on IRS Selector Panel */
  dev = 3;
  for (pin=0;pin<8;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  for (pin=8;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  mcp23017[te][dev].intpin = 8;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x23; // I2C address of MCP23017 device
  
  /* MCP23017 on EEC Panel */
  dev = 4;
  for (pin=0;pin<4;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  for (pin=4;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  mcp23017[te][dev].intpin = 12;  // Interrupt Pin on Teensy (INITVAL if OUTPUT ONLY DEVICE)
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x24; // I2C address of MCP23017 device
  
  /* HT16K33 7 Segment Driver on IRS Display Panel */
  ht16k33[te][0].brightness = 10;
  ht16k33[te][0].wire = 0;
  ht16k33[te][0].address = 0x70;

  /* This program simulates a key matrix on the IRS DISPLAY PANEL MCP23017 I2C device */
  ret = program_keymatrix_init(te, 0, 0, 10, 3, 6, 4);
  if (ret != 0) printf("Initialization of Key Matrix Program 0 failed\n");
 
}

void b737_overheadaft(void)
{

  int te = 3; // Teensy Number (as in ini file)

  int ret;
  int dev;

  int one = 1;
  int zero = 0;

  int i;
  int ival;
  int temp;
  float fval;
  int display;
  char substr[2];

  char datarefname[100];
  int *datarefptr[nRows*nCols];    

  int *avionics_on = link_dataref_int("sim/cockpit2/switches/avionics_power_on");

  *avionics_on = 1;
 
  float *lights_test;
  if ((acf_type == 2) || (acf_type == 3)) {
    lights_test = link_dataref_flt("laminar/B738/toggle_switch/bright_test",0);
  } else {
    lights_test = link_dataref_flt("xpserver/lights_test",0);
  }

  /* *avionics_on = 1; */
  /* acf_type = 3; */
  /* *lights_test = 1.0; */
  
  /*** Background Lighting ***/

  /* turn off background lighting if avionics are off */
  ret = digital_output(te, TEENSY_TYPE, 0, 24, avionics_on); /* 12V on Extension board */
  ret = digital_output(te, MCP23017_TYPE, 0, 15, avionics_on); /* 5V on IRS Disp panel */

  /* dev=0; */
  /* for (int i=0;i<10;i++) { */
  /*   ret = digital_input(te, MCP23017_TYPE, dev, i, &ival, 0); */
  /*   if (ret == 1) { */
  /*     printf("MCP23017 %i Input %i changed to: %i \n",dev,i,ival); */
  /*   } */
  /* } */

  /*** CENTER SWITCHES (DOME WHITE DIRECTLY WIRED TO DIGITAL RELAY ***/
  dev = 4; /* Service Interphone Switch connected to EEC Panel */
  int *service_interphone = link_dataref_int("xpserver/service_interphone");
  ret = digital_input(te, MCP23017_TYPE, dev, 3, service_interphone, 0);
  if (ret == 1) {
    printf("Service Interphone Switch: %i \n",*service_interphone);
  }

  /*************************/
  /*** IRS Display Panel ***/
  /*************************/

  if ((acf_type == 2) || (acf_type == 3)) {

    dev = 0; /* MCP23017 Device Number */

    int dp = -1; /* not displaying native 7 Segment Decimal points */
    
    /* blank displays if avionics are off */
    int display_brightness = 10;
    
     /* BRT Potentiometer driving 7 segment brightness */
    ret = analog_input(te,26,&fval,1.0,16.0);
    display_brightness = (int) fval;
    if (ret == 1) {
      /* ret is 1 only if analog input has changed */
      printf("Display Brightness changed to: %i \n",display_brightness);
    }

    if (*avionics_on != 1) display_brightness = 0;

    /* SYS DISPL Switch */
    float *sys_disp = link_dataref_flt("laminar/B738/toggle_switch/irs_sys_dspl",0);
    ret = digital_inputf(te, MCP23017_TYPE, dev, 0, sys_disp, 0);
    if (ret == 1) {
      printf("SYS DISP Switch changed to: %i\n", (int) *sys_disp);
    }
    
    /* DISPL SEL Rotary */
    float *disp_sel_status = link_dataref_flt("laminar/B738/toggle_switch/irs_dspl_sel",0);
    int *disp_sel_left = link_dataref_cmd_once("laminar/B738/toggle_switch/irs_dspl_sel_left");
    int *disp_sel_right = link_dataref_cmd_once("laminar/B738/toggle_switch/irs_dspl_sel_right");

    ret = digital_input(te, MCP23017_TYPE, dev, 1, &ival, 0);
    if (ival == 1) disp_sel = 0.0;
    ret = digital_input(te, MCP23017_TYPE, dev, 2, &ival, 0);
    if (ival == 1) disp_sel = 1.0;
    ret = digital_input(te, MCP23017_TYPE, dev, 3, &ival, 0);
    if (ival == 1) disp_sel = 2.0;
    ret = digital_input(te, MCP23017_TYPE, dev, 4, &ival, 0);
    if (ival == 1) disp_sel = 3.0;
    ret = digital_input(te, MCP23017_TYPE, dev, 5, &ival, 0);
    if (ival == 1) disp_sel = 4.0;
    
    ret = set_state_updnf(&disp_sel, disp_sel_status, disp_sel_right, disp_sel_left);
    if (ret != 0) {
      printf("DISP SEL Switch changed to: %i\n", (int) disp_sel);
    }

    /* 12 key keyboard: Matrix arrangement */
    for (i=0;i<(nCols*nRows);i++) {
      datarefptr[i] = NULL;
      if (strcmp(zibo_keyname[i],"")) {
	strncpy(datarefname,"laminar/B738/push_button/irs_key",sizeof(datarefname));
	strcat(datarefname,zibo_keyname[i]);
	//printf("%s\n",datarefname);
	datarefptr[i] = link_dataref_cmd_once(datarefname);
	if ((keyvalue[i] != 0) && (keyvalue[i] != 1)) keyvalue[i] = 0;
      }
    }

    /* Keyboard Matrix */
    ret = program_keymatrix(te, 0, 1,&ival);
    if (ret == 1) {
      printf("IRS Display Key %s pressed \n",zibo_keyname[ival]);
      *datarefptr[ival] = 1;
      last = ival + 1; /* remember last keypress */    
    }


    /* ENT Key Light */
    float *irs_ent = link_dataref_flt("laminar/B738/irs_ent_light",0);
    ret = digital_outputf(te, MCP23017_TYPE, 0, 13, irs_ent);
    
    /* CLR Key Light */
    float *irs_clr = link_dataref_flt("laminar/B738/irs_clr_light",0);
    ret = digital_outputf(te, MCP23017_TYPE, 0, 14, irs_clr);

    char *irs_val;
    if (*sys_disp == 0) {
      //irs_val = link_dataref_byte_arr("laminar/B738/irs/irs1_pos",100,-1);
      irs_val = link_dataref_byte_arr("laminar/B738/irs/irs_pos",100,-1);
    } else {
      irs_val = link_dataref_byte_arr("laminar/B738/irs/irs2_pos",100,-1);
    }

    float *irs_left1_show = link_dataref_flt("laminar/B738/irs_left1_show",0);
    float *irs_left2_show = link_dataref_flt("laminar/B738/irs_left2_show",0);
    float *irs_left1 = link_dataref_flt("laminar/B738/irs_left1",0);
    float *irs_left2 = link_dataref_flt("laminar/B738/irs_left2",0);
    float *irs_right1_show = link_dataref_flt("laminar/B738/irs_right1_show",0);
    float *irs_right2_show = link_dataref_flt("laminar/B738/irs_right2_show",0);
    float *irs_right1 = link_dataref_flt("laminar/B738/irs_right1",0);
    float *irs_right2 = link_dataref_flt("laminar/B738/irs_right2",0);

    float *irs_entry_len = link_dataref_flt("laminar/B738/irs_entry_len",0);
    float *irs_entry_pos_show = link_dataref_flt("laminar/B738/irs_entry_pos_show",0);
    float *irs_entry = link_dataref_flt_arr("laminar/B738/irs_entry",11,-1,0);
    if (*irs_entry_pos_show == 0.0) {
      is_N = 0;
      is_S = 0;
      is_W = 0;
      is_E = 0;
    }
    
    /* 7 Segment IRS Display (Original Boeing Module, incandescent bulbs) */
    if ((*irs_left1_show == 1.0) || (*irs_left2_show == 1.0) ||
	(*irs_right1_show == 1.0) || (*irs_right2_show == 1.0)) {
      
      /* IRS does not show Coordinates */
      
      if (*irs_left1_show == 1.0) {
	if (*irs_left1 == 88.0) {
	  display = 888;
	  ret = display_output(te, HT16K33_TYPE, 0, 0, 3, &display, dp, display_brightness);
	} else {
	}
      } else {
	display = 16; /* Blank Digit */
	for (i=0;i<3;i++) {
	  ret = display_output(te, HT16K33_TYPE, 0, i, 1, &display, dp, display_brightness);
	}
       }
	
      if (*irs_left2_show == 1.0) {
	temp = (int) *irs_left2;
	display = temp/100;
	ret = display_output(te, HT16K33_TYPE, 0, 3, 1, &display, dp, display_brightness);
	display = temp/10 - temp/100*10;
	ret = display_output(te, HT16K33_TYPE, 0, 4, 1, &display, dp, display_brightness);
 	display = temp - temp/10*10;
	ret = display_output(te, HT16K33_TYPE, 0, 5, 1, &display, dp, display_brightness);
      } else {
	display = 16; /* Blank Digit */
	for (i=3;i<6;i++) {
	  ret = display_output(te, HT16K33_TYPE, 0, i, 1, &display, dp, display_brightness);
	}
      }
	
      if (*irs_right1_show == 1.0) {
	if (*irs_right1 == 188.0) {
	  display = 888;
	  ret = display_output(te, HT16K33_TYPE, 0, 8, 3, &display, dp, display_brightness);
	} else {
	}
      } else {
	display = 16; /* Blank Digit */
	for (i=8;i<11;i++) {
	  ret = display_output(te, HT16K33_TYPE, 0, i, 1, &display, dp, display_brightness);
	}
      }

      if (*irs_right2_show == 1.0) {
	temp = (int) *irs_right2;
	display = temp/100;
	ret = display_output(te, HT16K33_TYPE, 0, 11, 1, &display, dp, display_brightness);
	display = temp/10 - temp/100*10;
	ret = display_output(te, HT16K33_TYPE, 0, 12, 1, &display, dp, display_brightness);
 	display = temp - temp/10*10;
	ret = display_output(te, HT16K33_TYPE, 0, 13, 1, &display, dp, display_brightness);
      } else {
	display = 16; /* Blank Digit */
	for (i=11;i<14;i++) {
	  ret = display_output(te, HT16K33_TYPE, 0, i, 1, &display, dp, display_brightness);
	}
      }
	
    } else if (*irs_entry_pos_show == 1.0) {

      /* Show manually entered IRS coordinates */

      if (*irs_entry_len == 1) {
	if ((is_N == 0) && (is_S == 0)) {
	  if (last == 2) is_N = 1;
	  if (last == 8) is_S = 1;
	}
      }
      if (*irs_entry_len == 7) {
	if ((is_W == 0) && (is_E == 0)) {
	  if (last == 4) is_W = 1;
	  if (last == 6) is_E = 1;
	}
      }

      //printf("%i %i %i %i %i %i \n",(int) *irs_entry_len,last, is_N, is_S, is_W, is_E);
      
      /* N/S Digits */
      if ((is_N == 1) || (is_S == 1)) {
	if (is_N == 1) {
	  display = 4; /* 7 seg bit value for N */
	} else {
	  display = 8; /* 7 seg bit value for S */
	}
	display += 16; /* Add Decimal Points */
	display += 32; /* Add Decimal Points */
	ret = display_output(te, HT16K33_TYPE, 0, 0, 1, &display, -10, display_brightness);
      } else {
	display = 16; /* Add Decimal Points */
	display += 32; /* Add Decimal Points */
	ret = display_output(te, HT16K33_TYPE, 0, 0, 1, &display, -10, display_brightness);
      }
      for (i=1;i<6;i++) {
	if (i < *irs_entry_len) {
	  display = (int) irs_entry[i-1];
	  ret = display_output(te, HT16K33_TYPE, 0, i, 1, &display, dp, display_brightness);
	} else {
	  display = 16; /* Blank Digit */
	  ret = display_output(te, HT16K33_TYPE, 0, i, 1, &display, dp, display_brightness);
	}
      }
      
      /* W/E Digits */
      if ((is_W == 1) || (is_E == 1)) {
	if (is_E == 1) {
	  display = 4; /* 7 seg bit value for E */
	} else {
	  display = 8; /* 7 seg bit value for W */
	}
	if (*irs_entry_len >= 8) {
	  if ((int) irs_entry[5] == 1) {
	    display += 64; /* add bit value for 100's in W/E coordinates */
	  }
	} 
	display += 16; /* Add Decimal Points */
	display += 32; /* Add Decimal Points */
	ret = display_output(te, HT16K33_TYPE, 0, 8, 1, &display, -10, display_brightness);
      } else {
	display = 16; /* Add Decimal Points */
	display += 32; /* Add Decimal Points */
	ret = display_output(te, HT16K33_TYPE, 0, 8, 1, &display, -10, display_brightness);
      }
      for (i=9;i<14;i++) {
	if (i <= *irs_entry_len) {
	  display = (int) irs_entry[i-3];
	  ret = display_output(te, HT16K33_TYPE, 0, i, 1, &display, dp, display_brightness);
	} else {
	  display = 16 /* Blank Digit */;
	  ret = display_output(te, HT16K33_TYPE, 0, i, 1, &display, dp, display_brightness);
	}
      }
      
    } else {

      /* IRS shows regular coordinates */

      if ((strncmp(irs_val,"N",1)==0) || (strncmp(irs_val,"S",1)==0)) {
	/* VALID IRS ENTRY */

	/* N/S Digits */
	if (strncmp(irs_val,"N",1)==0) {
	  display = 4; /* 7 seg bit value for N */
	} else {
	  display = 8; /* 7 seg bit value for S */
	}
	display += 16; /* Add Decimal Points */
	display += 32; /* Add Decimal Points */
	ret = display_output(te, HT16K33_TYPE, 0, 0, 1, &display, -10, display_brightness);

	for (i=1;i<5;i++) {
	  substr[0] = irs_val[i];
	  substr[1]='\0';
	  display = atoi(substr);
	  ret = display_output(te, HT16K33_TYPE, 0, i, 1, &display, dp, display_brightness);
	}
	substr[0] = irs_val[6];
	substr[1]='\0';
	display = atoi(substr);
	ret = display_output(te, HT16K33_TYPE, 0, 5, 1, &display, dp, display_brightness);
	
	/* W/E Digits */
	if (strncmp(irs_val+7,"E",1)==0) {
	  display = 4; /* 7 seg bit value for E */
	} else {
	  display = 8; /* 7 seg bit value for W */
	}
	if (strncmp(irs_val+8,"1",1)==0) {
	  display += 64; /* add bit value for 100's in W/E coordinates */
	} else {
	  /* ORIGINAL B737 IRS MODULE DOES NOT PRINT LEADING 0 if W/E Degrees are less than 100 */
	}
	display += 16; /* Add Decimal Points */
	display += 32; /* Add Decimal Points */
	ret = display_output(te, HT16K33_TYPE, 0, 8, 1, &display, -10, display_brightness);
    
	for (i=9;i<13;i++) {
	  substr[0] = irs_val[i];
	  substr[1]='\0';
	  display = atoi(substr);
	  ret = display_output(te, HT16K33_TYPE, 0, i, 1, &display, dp, display_brightness);
	}
	substr[0] = irs_val[14];
	substr[1]='\0';
	display = atoi(substr);
	ret = display_output(te, HT16K33_TYPE, 0, 13, 1, &display, dp, display_brightness);

      } else {
	/* blank IRS display if no coordinates */
	display = 16; /* Blank Digit */
	for (i=0;i<16;i++) {
	  ret = display_output(te, HT16K33_TYPE, 0, i, 1, &display, dp, display_brightness);
	}
      }
    }
    
    //display = 88888888;
    //display = 8;
    //int value = 2;
    //int dp = -1;

    /* display = 12345; */
    //ret = display_output(te, HT16K33_TYPE, 0, 0, 8, &display, dp, display_brightness);
    //ret = display_output(te, HT16K33_TYPE, 0, 0, 1, &display, dp, display_brightness);
    /* display = 56789; */
    //ret = display_output(te, HT16K33_TYPE, 0, 8, 8, &display, dp, display_brightness);

  }

  
  /*******************/
  /*** FLAPS Panel ***/
  /*******************/
  if ((acf_type == 2) || (acf_type == 3)) {

    if (*avionics_on == 1) {
      int *flaps_test =  link_dataref_cmd_hold("laminar/B738/push_button/flaps_test");
      dev = 1; /* MCP23017 Device Number */
      ret = digital_input(te, MCP23017_TYPE, dev, 14, flaps_test, 0);
      if (ret == 1) {
	printf("Flaps Panel Test Button: %i \n",*flaps_test);
      }

      float *flaps_test_ann = link_dataref_flt("laminar/B738/annunciator/flaps_test",-2);
      if (*flaps_test_ann > 0.05) {
	dev = 1; /* MCP23017 Device Number */
	for (i=0;i<13;i++) {
	  ret = digital_output(te, MCP23017_TYPE, dev, i, &one);
	}
	dev = 2; /* MCP23017 Device Number */
	for (i=0;i<13;i++) {
	  ret = digital_output(te, MCP23017_TYPE, dev, i, &one);
	}
      } else {
	float *slats_extend = link_dataref_flt("laminar/B738/annunciator/slats_extend",-2);
	float *slats_transit = link_dataref_flt("laminar/B738/annunciator/slats_transit",-2);
	float *slats1 = link_dataref_flt("laminar/B738/controls/slat1_deploy_ratio",-3);
	float *slats2 = link_dataref_flt("laminar/B738/controls/slat2_deploy_ratio",-3);
	/* SLATS ANNUNCIATORS */
	if (*slats_transit > 0.05) {
	  dev = 1;
	  ret = digital_output(te, MCP23017_TYPE, dev, 0, &one);
	  ret = digital_output(te, MCP23017_TYPE, dev, 1, &one);
	  ret = digital_output(te, MCP23017_TYPE, dev, 2, &one);
	  dev = 2;
	  ret = digital_output(te, MCP23017_TYPE, dev, 0, &one);
	  ret = digital_output(te, MCP23017_TYPE, dev, 1, &one);
	  ret = digital_output(te, MCP23017_TYPE, dev, 2, &one);
	} else {
	  dev = 1;
	  ret = digital_output(te, MCP23017_TYPE, dev, 0, &zero);
	  ret = digital_output(te, MCP23017_TYPE, dev, 1, &zero);
	  ret = digital_output(te, MCP23017_TYPE, dev, 2, &zero);
	  dev = 2;
	  ret = digital_output(te, MCP23017_TYPE, dev, 0, &zero);
	  ret = digital_output(te, MCP23017_TYPE, dev, 1, &zero);
	  ret = digital_output(te, MCP23017_TYPE, dev, 2, &zero);
	}
	
	if (*slats2 == 0.5) {
	  dev = 1;
	  ret = digital_output(te, MCP23017_TYPE, dev, 5, &one);
	  ret = digital_output(te, MCP23017_TYPE, dev, 6, &one);
	  ret = digital_output(te, MCP23017_TYPE, dev, 7, &one);
	  dev = 2;
	  ret = digital_output(te, MCP23017_TYPE, dev, 5, &one);
	  ret = digital_output(te, MCP23017_TYPE, dev, 6, &one);
	  ret = digital_output(te, MCP23017_TYPE, dev, 7, &one);
	} else {
	  dev = 1;
	  ret = digital_output(te, MCP23017_TYPE, dev, 5, &zero);
	  ret = digital_output(te, MCP23017_TYPE, dev, 6, &zero);
	  ret = digital_output(te, MCP23017_TYPE, dev, 7, &zero);
	  dev = 2;
	  ret = digital_output(te, MCP23017_TYPE, dev, 5, &zero);
	  ret = digital_output(te, MCP23017_TYPE, dev, 6, &zero);
	  ret = digital_output(te, MCP23017_TYPE, dev, 7, &zero);
	}
      
	if (*slats2 == 1.0) {
	  dev = 1;
	  ret = digital_output(te, MCP23017_TYPE, dev, 8, &one);
	  ret = digital_output(te, MCP23017_TYPE, dev, 9, &one);
	  ret = digital_output(te, MCP23017_TYPE, dev, 10, &one);
	  dev = 2;
	  ret = digital_output(te, MCP23017_TYPE, dev, 8, &one);
	  ret = digital_output(te, MCP23017_TYPE, dev, 9, &one);
	  ret = digital_output(te, MCP23017_TYPE, dev, 10, &one);
	} else {
	  dev = 1;
	  ret = digital_output(te, MCP23017_TYPE, dev, 8, &zero);
	  ret = digital_output(te, MCP23017_TYPE, dev, 9, &zero);
	  ret = digital_output(te, MCP23017_TYPE, dev, 10, &zero);
	  dev = 2;
	  ret = digital_output(te, MCP23017_TYPE, dev, 8, &zero);
	  ret = digital_output(te, MCP23017_TYPE, dev, 9, &zero);
	  ret = digital_output(te, MCP23017_TYPE, dev, 10, &zero);
	}

	/* FLAPS ANNUNCIATORS */
	if ((*slats1 > 0.0) && (*slats1 < 1.0)) {
	  dev = 1;
	  ret = digital_output(te, MCP23017_TYPE, dev, 3, &one);
	  ret = digital_output(te, MCP23017_TYPE, dev, 4, &one);
	  dev = 2;
	  ret = digital_output(te, MCP23017_TYPE, dev, 3, &one);
	  ret = digital_output(te, MCP23017_TYPE, dev, 4, &one);
	} else {
	  dev = 1;
	  ret = digital_output(te, MCP23017_TYPE, dev, 3, &zero);
	  ret = digital_output(te, MCP23017_TYPE, dev, 4, &zero);
	  dev = 2;
	  ret = digital_output(te, MCP23017_TYPE, dev, 3, &zero);
	  ret = digital_output(te, MCP23017_TYPE, dev, 4, &zero);
	}
	if (*slats1 == 1.0) {
	  dev = 1;
	  ret = digital_output(te, MCP23017_TYPE, dev, 11, &one);
	  ret = digital_output(te, MCP23017_TYPE, dev, 12, &one);
	  dev = 2;
	  ret = digital_output(te, MCP23017_TYPE, dev, 11, &one);
	  ret = digital_output(te, MCP23017_TYPE, dev, 12, &one);
	} else {dev = 1;
	  ret = digital_output(te, MCP23017_TYPE, dev, 11, &zero);
	  ret = digital_output(te, MCP23017_TYPE, dev, 12, &zero);
	  dev = 2;
	  ret = digital_output(te, MCP23017_TYPE, dev, 11, &zero);
	  ret = digital_output(te, MCP23017_TYPE, dev, 12, &zero);
	}
      }    
    } else {
      /* avionics off */
      dev = 1; /* MCP23017 Device Number */
      for (i=0;i<14;i++) {
	ret = digital_output(te, MCP23017_TYPE, dev, i, &zero);
      }
      dev = 2; /* MCP23017 Device Number */
      for (i=0;i<14;i++) {
	ret = digital_output(te, MCP23017_TYPE, dev, i, &zero);
      }
    }
  }

  
  /*******************/
  /*** ELT Panel ***/
  /*******************/
  if ((acf_type == 2) || (acf_type == 3)) {

    dev = 1;
    
    float *elt_status = link_dataref_flt("laminar/B738/toggle_switch/elt",0);
    int *elt_off = link_dataref_cmd_once("laminar/B738/toggle_switch/elt_arm");
    int *elt_on = link_dataref_cmd_once("laminar/B738/toggle_switch/elt_on");
    ret = digital_inputf(te, MCP23017_TYPE, dev, 15, &elt, 0);
    ret = set_state_updnf(&elt, elt_status, elt_on, elt_off);
    if (ret != 0) {
      printf("ELT Switch: %i \n",(int) elt);
    }
    
    float *elt_ann = link_dataref_flt("laminar/B738/annunciator/elt",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 13, elt_ann);
  }


  /*** PSEU & GEAR ANNUNCIATORS ***/
  if ((acf_type == 2) || (acf_type == 3)) {
    
    dev = 2;

    ret = digital_outputf(te, MCP23017_TYPE, dev, 13, lights_test);

    float *nose_gear= link_dataref_flt("laminar/B738/annunciator/nose_gear_safe",-1);
    float *left_gear = link_dataref_flt("laminar/B738/annunciator/left_gear_safe",-1);
    float *right_gear = link_dataref_flt("laminar/B738/annunciator/right_gear_safe",-1);

    /* Gear Annunciators connected to EEC Panel */
    dev = 4;
    ret = digital_outputf(te, MCP23017_TYPE, dev, 5, nose_gear);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 6, left_gear);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 7, right_gear);

  }
    
  /**************************/
  /*** IRS Selector PANEL ***/
  /**************************/

  if ((acf_type == 2) || (acf_type == 3)) {

    dev = 3;
    
    float *irs_l_status = link_dataref_flt("laminar/B738/toggle_switch/irs_left",0);
    int *irs_l_left = link_dataref_cmd_once("laminar/B738/toggle_switch/irs_L_left");
    int *irs_l_right = link_dataref_cmd_once("laminar/B738/toggle_switch/irs_L_right");

    ret = digital_input(te, MCP23017_TYPE, dev, 12, &temp, 0);
    if (temp == 1) irs_l = 0.0;
    ret = digital_input(te, MCP23017_TYPE, dev, 13, &temp, 0);
    if (temp == 1) irs_l = 1.0;
    ret = digital_input(te, MCP23017_TYPE, dev, 14, &temp, 0);
    if (temp == 1) irs_l = 2.0;
    ret = digital_input(te, MCP23017_TYPE, dev, 15, &temp, 0);
    if (temp == 1) irs_l = 3.0;
    ret = set_state_updnf(&irs_l, irs_l_status, irs_l_right, irs_l_left);
    if (ret != 0) {
      printf("IRS LEFT SELECT: %i \n",(int) irs_l);
    }

    float *irs_r_status = link_dataref_flt("laminar/B738/toggle_switch/irs_right",0);
    int *irs_r_left = link_dataref_cmd_once("laminar/B738/toggle_switch/irs_R_left");
    int *irs_r_right = link_dataref_cmd_once("laminar/B738/toggle_switch/irs_R_right");

    ret = digital_input(te, MCP23017_TYPE, dev, 8, &temp, 0);
    if (temp == 1) irs_r = 0.0;
    ret = digital_input(te, MCP23017_TYPE, dev, 9, &temp, 0);
    if (temp == 1) irs_r = 1.0;
    ret = digital_input(te, MCP23017_TYPE, dev, 10, &temp, 0);
    if (temp == 1) irs_r = 2.0;
    ret = digital_input(te, MCP23017_TYPE, dev, 11, &temp, 0);
    if (temp == 1) irs_r = 3.0;
    ret = set_state_updnf(&irs_r, irs_r_status, irs_r_right, irs_r_left);
    if (ret != 0) {
      printf("IRS RIGHT SELECT: %i \n",(int) irs_r);
    }
    
    float *align_left = link_dataref_flt("laminar/B738/annunciator/irs_align_left",-2);
    float *align_right = link_dataref_flt("laminar/B738/annunciator/irs_align_right",-2);
    float *fail_left = link_dataref_flt("laminar/B738/annunciator/irs_align_fail_left",-2);
    float *fail_right = link_dataref_flt("laminar/B738/annunciator/irs_align_fail_right",-2);
    float *on_dc_left = link_dataref_flt("laminar/B738/annunciator/irs_on_dc_left",-2);
    float *on_dc_right = link_dataref_flt("laminar/B738/annunciator/irs_on_dc_right",-2);
    float *dc_fail_left = link_dataref_flt("laminar/B738/annunciator/irs_dc_fail_left",-2);
    float *dc_fail_right = link_dataref_flt("laminar/B738/annunciator/irs_dc_fail_right",-2);
    float *gps = link_dataref_flt("laminar/B738/annunciator/gps",-2);

    ret = digital_outputf(te, MCP23017_TYPE, dev, 0, align_left);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 1, fail_left);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 2, on_dc_left);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 3, dc_fail_left);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 4, align_right);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 5, fail_right);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 6, on_dc_right);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 7, dc_fail_right);

    /* GPS Annunciator is on Flaps Panel board */
    dev = 2;
    ret = digital_outputf(te, MCP23017_TYPE, dev, 14, gps);
    
  }

  /**************************/
  /*** ENGINE / EEC Panel ***/
  /**************************/
  
  if ((acf_type == 2) || (acf_type == 3)) {

    dev = 4;
    
    float *reverser_fail_1 = link_dataref_flt("laminar/B738/annunciator/reverser_fail_0",0);
    float *reverser_fail_2 = link_dataref_flt("laminar/B738/annunciator/reverser_fail_1",0);
    float *fadec_fail_1 = link_dataref_flt("laminar/B738/annunciator/fadec_fail_0",0);
    float *fadec_fail_2 = link_dataref_flt("laminar/B738/annunciator/fadec_fail_1",0);
    float *fadec_off_1 = link_dataref_flt("laminar/B738/annunciator/fadec1_off",0);
    float *fadec_off_2 = link_dataref_flt("laminar/B738/annunciator/fadec2_off",0);

    printf("%f \n",*fadec_fail_1);
    
    ret = digital_outputf(te, MCP23017_TYPE, dev, 8, fadec_fail_1);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 9, fadec_fail_2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 10, reverser_fail_1);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 11, reverser_fail_2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 12, fadec_off_1);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 13, fadec_off_2);

    /* INOP FOR NOW */
    ret = digital_output(te, MCP23017_TYPE, dev, 14, &one); /* ON LEFT */
    ret = digital_output(te, MCP23017_TYPE, dev, 15, &one); /* ON RIGHT */

    /* EEC1 Button: Load Fuel entered in FMC1 Entry Line */
    unsigned char *fmc_entry = link_dataref_byte_arr("laminar/B738/fmc1/Line_entry", 40, -1);  
    int *fuel_truck = link_dataref_cmd_hold("laminar/B738/fuel_truck_toggle");
    int *fuel_req_kgs = link_dataref_int("laminar/B738/tab/req_fuel");
    ret = digital_input(te, MCP23017_TYPE, dev, 0, fuel_truck, 0);
    if (ret == 1) {
      printf("FADEC 1 Button: %i \n",*fuel_truck);
    }
    if ((fmc_entry) && (ret == 1)) {
      if (is_numeric(fmc_entry)) {
	int ival_len = strlen((const char*) fmc_entry);
	if ((ival_len >= 4) && (ival_len <= 5)) {
	  int ival = atoi((const char*) fmc_entry);
	  if (*fuel_truck == 1) {
	    printf("Call Fuel Truck and load %i kgs \n",ival);
	    *fuel_req_kgs = ival;
	  }
	}
      }
    }   

    /* EEC2 Button INOP FOR NOW */
    ret = digital_input(te, MCP23017_TYPE, dev, 1, &temp, 0);
    if (ret == 1) {
      printf("FADEC 2 Button: %i \n",temp);
    }
  }


  /*************************/
  /*** CREW OXYGEN Panel ***/
  /*************************/
 
  if ((acf_type == 2) || (acf_type == 3)) {
    
    dev = 4; /* Connected to EEC Panel MCP23017 */
    
    float *pass_oxy_status = link_dataref_flt("laminar/B738/one_way_switch/pax_oxy_pos",0);
    int *pass_oxy_norm = link_dataref_cmd_once("laminar/B738/one_way_switch/pax_oxy_norm");
    int *pass_oxy_on = link_dataref_cmd_once("laminar/B738/one_way_switch/pax_oxy_on");
    ret = digital_inputf(te, MCP23017_TYPE, dev, 2, &pass_oxy, 0);
    ret = set_state_updnf(&pass_oxy, pass_oxy_status, pass_oxy_on, pass_oxy_norm);
    if (ret != 0) {
      printf("Pass Oxygen Switch: %i \n",(int) pass_oxy);
    }

    float *pass_oxy_ann = link_dataref_flt("laminar/B738/annunciator/pax_oxy",-2);
    ret = digital_outputf(te, MCP23017_TYPE, dev, 4, pass_oxy_ann);

    /* CREW Oxygen Amount not yet found as dataref in ZIBO MOD */
    /* float servoval = 0.5; */
    /* ret = servo_outputf(card,0,&servoval, 0.0,1.0); */

  }

  
}

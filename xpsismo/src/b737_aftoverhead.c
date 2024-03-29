/* This is the b737_aftoverhead.c code which connects to the SISMO AFT OVERHEAD PANEL

   Copyright (C) 2023 Reto Stockli

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>

#include "common.h"
#include "libsismo.h"
#include "serverdata.h"
#include "b737_aftoverhead.h"

#define COUNT_MAX 3
#define nCols 3
#define nRows 4

int col;
int counter;
int keyvalue[nRows][nCols];
float disp_sel;

/* 3D array with 3 Columns x 4 Rows x 4 Chars */
const char zibo[nRows][nCols][5] = {{"_1","_2","_3"},{"_4","_5","_6"},{"_7","_8","_9"},{"_ent","_0","_clr"}};

void b737_aftoverhead(void)
{
  int card = 4; /* SISMO card according to ini file */

  int zero = 0;
  int ret;
  int temp;
  float fvalue;
  int display;
  int i;
  int c,r;
  int outvalue;
  int i0;
  int o0;
  char datarefname[100];
  int *datarefptr[nRows][nCols];    
  char substr[2];

  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");

  float *lights_test;
  if ((acf_type == 2) || (acf_type == 3)) {
    lights_test = link_dataref_flt("laminar/B738/toggle_switch/bright_test",0);
  } else {
    lights_test = link_dataref_flt("xpserver/lights_test",0);
  }
    
  /*** Background Lighting ***/

  /* blank displays if avionics are off */
  int display_brightness = 10;
  if (*avionics_on != 1) display_brightness = 0;

  /* turn off background lighting if avionics are off */
  //o0 = 0;
  //ret = digital_output(card,o0+X,avionics_on);


  int one=1;
  for (i=0;i<32;i++) {
    ret = digital_output(card,i,&one);
  }
      


  /*** IRS DISPLAY PANEL ***/
  i0 = 40;
  o0 = 24;
 
  if ((acf_type == 2) || (acf_type == 3)) {
    
    /* BRT Potentiometer driving 7 segment brightness */
    i=1;
    ret = analog_input(card,i,&fvalue,5.0,15.0);
    display_brightness = (int) fvalue;
    if (ret == 1) {
      /* ret is 1 only if analog input has changed */
      //printf("Analog Input %i changed to: %f \n",i,fvalue);
    }

    /* SYS DISPL Switch */
    float *sys_disp = link_dataref_flt("laminar/B738/toggle_switch/irs_sys_dspl",0);
    ret = digital_inputf(card, i0+0, sys_disp, 0);

    /* DISPL SEL Rotary */
    float *disp_sel_status = link_dataref_flt("laminar/B738/toggle_switch/irs_dspl_sel",0);
    int *disp_sel_left = link_dataref_cmd_once("laminar/B738/toggle_switch/irs_dspl_sel_left");
    int *disp_sel_right = link_dataref_cmd_once("laminar/B738/toggle_switch/irs_dspl_sel_right");

    ret = digital_input(card, i0+1, &temp, 0);
    if (temp == 1) disp_sel = 0.0;
    ret = digital_input(card, i0+2, &temp, 0);
    if (temp == 1) disp_sel = 1.0;
    ret = digital_input(card, i0+3, &temp, 0);
    if (temp == 1) disp_sel = 2.0;
    ret = digital_input(card, i0+4, &temp, 0);
    if (temp == 1) disp_sel = 3.0;
    ret = digital_input(card, i0+5, &temp, 0);
    if (temp == 1) disp_sel = 4.0;
    
    ret = set_state_updnf(&disp_sel, disp_sel_status, disp_sel_right, disp_sel_left);
    

    /* 12 key keyboard: Matrix arrangement */
    for (r=0;r<nRows;r++) {
      for (c=0;c<nCols;c++) {
	datarefptr[r][c] = NULL;
	if (strcmp(zibo[r][c],"")) {
	  strncpy(datarefname,"laminar/B738/push_button/irs_key",sizeof(datarefname));
	  strcat(datarefname,zibo[r][c]);
	  //printf("%s\n",datarefname);
	  datarefptr[r][c] = link_dataref_cmd_once(datarefname);
	  if ((keyvalue[r][c] != 0) && (keyvalue[r][c] != 1)) keyvalue[r][c] = 0;
	}
      }
    }
        
    if ((col<0) || (col>=nCols)) col = 0;
    if ((counter<0) || (counter>COUNT_MAX)) counter = 0;
    
    for (c=0;c<nCols;c++) {
      if (c==col) { outvalue = 0; } else { outvalue = 1; }
      ret = digital_output(card,o0+c,&outvalue);
    }
    
    if (counter == COUNT_MAX) {
      i0=8;
      for (int r=0;r<nRows;r++) {
	temp = keyvalue[r][col];
	ret = digital_input(card, i0+r, &keyvalue[r][col], 0);
	if ((ret == 1) && (keyvalue[r][col] == 1)) {
	  /* ret is 1 only if input has changed */
	  printf("IRS Display Key %i Row %i pressed \n",col,r);
	  *datarefptr[r][col] = 1;
	}
      }
    }
    
    if (counter == COUNT_MAX) {
      col++;
      if (col>=nCols) col = 0;
      counter = 0;
    }
    counter++;

    /* ENT Key Light */
    float *irs_ent = link_dataref_flt("laminar/B738/irs_ent_light",0);
    ret = digital_outputf(card,o0+6,irs_ent);
    
    /* CLR Key Light */
    float *irs_clr = link_dataref_flt("laminar/B738/irs_clr_light",0);
    ret = digital_outputf(card,o0+7,irs_clr);

    char *irs_val;
    if (*sys_disp == 0) {
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
    
    /* Check IRS Display Mode */
    if ((*irs_left1_show == 1.0) || (*irs_left2_show == 1.0) ||
	(*irs_right1_show == 1.0) || (*irs_right2_show == 1.0)) {
      /* IRS does not show Coordinates */
      
      if (*irs_left1_show == 1.0) {
	if (*irs_left1 == 88.0) {
	  display = 888;
	  ret = display_output(card,0,3,&display,0,display_brightness);
	} else {
	}
      } else {
	display = 0;
	ret = display_output(card,0,3,&display,0,0);
      }
	
      if (*irs_left2_show == 1.0) {
	temp = (int) *irs_left2;
	display = temp/100;
	ret = display_output(card,3,1,&display,0,display_brightness);
	display = temp/10 - temp/100*10;
	ret = display_output(card,4,1,&display,0,display_brightness);
 	display = temp - temp/10*10;
	ret = display_output(card,5,1,&display,0,display_brightness);
     } else {
	display = 0;
	ret = display_output(card,3,3,&display,0,0);
      }
	
      if (*irs_right1_show == 1.0) {
	if (*irs_right1 == 188.0) {
	  display = 888;
	  ret = display_output(card,8,3,&display,0,display_brightness);
	} else {
	}
      } else {
	display = 0;
	ret = display_output(card,8,3,&display,0,0);
      }
	
      if (*irs_right2_show == 1.0) {
	temp = (int) *irs_right2;
	display = temp/100;
	ret = display_output(card,11,1,&display,0,display_brightness);
	display = temp/10 - temp/100*10;
	ret = display_output(card,12,1,&display,0,display_brightness);
 	display = temp - temp/10*10;
	ret = display_output(card,13,1,&display,0,display_brightness);
     } else {
	display = 0;
	ret = display_output(card,11,3,&display,0,0);
      }
	
    } else if (*irs_entry_pos_show == 1.0) {
      /* Show manually entered IRS coordinates */

    } else {
      /* IRS shows coordinates */
      if ((strncmp(irs_val,"N",1)==0) || (strncmp(irs_val,"S",1)==0)) {
	/* VALID IRS ENTRY */

	/* N/S Digits */
	if (strncmp(irs_val,"N",1)==0) {
	  display = 16;
	} else {
	  display = 8;
	}
	display += 7; /* Decimal Points */
	ret = display_output(card,0,1,&display,-10,display_brightness);
	for (i=1;i<5;i++) {
	  substr[0] = irs_val[i];
	  substr[1]='\0';
	  display = atoi(substr);
	  ret = display_output(card,i,1,&display,0,display_brightness);
	}
	substr[0] = irs_val[6];
	substr[1]='\0';
	display = atoi(substr);
	ret = display_output(card,5,1,&display,0,display_brightness);
    
	/* W/E Digits */
	if (strncmp(irs_val+7,"E",1)==0) {
	  display = 16;
	} else {
	  display = 8;
	}
	if (strncmp(irs_val+8,"1",1)==0) {
	  display += 1;
	} else {
	  /* DO NOT PRINT LEADING 0 if W/E Degrees are less than 100 */
	}
	display += 6; /* Decimal Points */
	ret = display_output(card,8,1,&display,-10,display_brightness);
    
	for (i=9;i<13;i++) {
	  substr[0] = irs_val[i];
	  substr[1]='\0';
	  display = atoi(substr);
	  ret = display_output(card,i,1,&display,0,display_brightness);
	}
	substr[0] = irs_val[14];
	substr[1]='\0';
	display = atoi(substr);
	ret = display_output(card,14,1,&display,0,display_brightness);

      } else {
	/* blank IRS display if no coordinates */
	display = 0;
	ret = display_output(card,0,14,&display,0,0);
      }
    }
      
    /* set 7 segment displays 0-5 to the 5 digit value of the encoder with a decimal point at digit 2 */
    //ret = display_output(card, 0, 6, &display, 0, display_brightness);
    //ret = display_output(card, 8, 6, &display, 0, display_brightness);

    //    display = 64;
    
    //    ret = display_output(card,1,1,&display,-10,display_brightness);
    
    }
    
}

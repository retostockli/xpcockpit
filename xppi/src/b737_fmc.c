/* This is the b737_fmc.c code to access the dotsha747 Boeing 737 FMC board
   http://blog.shahada.abubakar.net/post/737-fmc-cdu-interfacing-to-a-raspberry-pi

   Some Code here is copied from c++ of testFMCKeyPad.cpp found in
   https://github.com/dotsha747/Pi-XPlane-FMC-CDU

   Please respect the original author's license and always reference the original code
   when distributing this code here.

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
#include "b737_fmc.h"

// columns are outputs. All are set to HIGH except the column being scanned.
const int colPins[] = { 
		15, // p.8  BCM.14 	BLACK UART TX
		16, // p.10 BCM.15 	WHITE UART RX
		1, 	// p.12 BCM.18 	GRAY  PWM0
		4, 	// p.16 BCM.23 	PURPLE
		5, 	// p.18 BCM.24 	BLUE
		6,	// p.22 BCM.25 	GREEN
		10,	// p.24	BCM.8	YELLOW CE0
		11,	// p.26 BCM.7	ORANGE CE1
		27	// p.36 BCM.16	RED
		};
// 	// rows are inputs.
const int rowPins[] = { 
		29,	//	p.40 BCM.21	GRAY SCLK
		8,	//	p.3	BCM.2 PURPLE I2C SDA (fixed pull-up)
		9,	// 	p.5	BCM.3 BLUE I2C SCL (fixed pull-up)
		7,	// 	p.7 BCM.4 GREEN GPCLK0
		0,	//	p.11 BCM.17 YELLOW
		2, 	// 	p.13 BCM.27 ORANGE
		3,	//	p.15 BCM.22 RED
		12	// 	p.19 BCM.10 BROWN MOSI
		};

void b737_fmc(void)
{

  /* only run for ZIBO 737 */
  if (acf_type == 3) {

    int ret;
    int pin;
    int nRows = sizeof(rowPins)/sizeof(int);
    int nCols = sizeof(colPins)/sizeof(int);
    int nowCol;
    int nowRow;

    int exec_led_pin = 13; /* GPIO 9, Physical Pin 21 */
    int msg_led_pin = 14; 	// p.3 bcm.11 
    int dspy_led_pin = 21;	// p.29 bcm.5
    int ofst_led_pin = 22; 	// p.31 bcm.6
    int fail_led_pin = 23;	// p.33 bcm.13
    
    pinMode(exec_led_pin, OUTPUT);
    pinMode(msg_led_pin, OUTPUT);
    pinMode(dspy_led_pin, OUTPUT);
    pinMode(ofst_led_pin, OUTPUT);
    pinMode(fail_led_pin, OUTPUT);
    
    
    int *exec_led = link_dataref_int("laminar/B738/indicators/fmc_exec_lights");
    int *msg_led = link_dataref_int("laminar/B738/fmc/fmc_message");
    
    if (*exec_led != INT_MISS) digitalWrite(exec_led_pin, *exec_led);
    if (*msg_led != INT_MISS) digitalWrite(msg_led_pin, *msg_led);
    
    /* the following datarefs are, at least to me,  unknown yet
    int *dspy_led = link_dataref_int("");
    int *ofst_led = link_dataref_int("");
    int *fail_led = link_dataref_int("");
  
    if (*dspy_led != INT_MISS) digitalWrite(exec_led_pin, *dspy_led);
    if (*ofst_led != INT_MISS) digitalWrite(exec_led_pin, *ofst_led);
    if (*fail_led != INT_MISS) digitalWrite(exec_led_pin, *fail_led);
*/

    /* link key datarefs */
    
//Piki   //int *key_dot; now  renamed to key_period, like the datarefs 
    
    int *key_1L;	
    int *key_2L;	
    int *key_3L;	
    int *key_4L;	
    int *key_5L;	
    int *key_6L;	
    int *key_1R;	
    int *key_2R;	
    int *key_3R;	
    int *key_4R;	
    int *key_5R;	
    int *key_6R;	
    int *key_init_ref;   
    int *key_rte;		
    int *key_clb;		
    int *key_crz;		
    int *key_des;		
    int *key_menu;		
    int *key_legs;		
    int *key_dep_app;
    int *key_hold;		
    int *key_prog;		
    int *key_exec;		
    int *key_n1_lim;	
    int *key_fix;		    
    int *key_prev_page;
    int *key_next_page;   
    int *key_A;		
    int *key_B;		
    int *key_C;		
    int *key_D;		
    int *key_E;		
    int *key_F;		
    int *key_G;		
    int *key_H;		
    int *key_I;		
    int *key_J;		
    int *key_K;		
    int *key_L;		
    int *key_M;		
    int *key_N;		
    int *key_O;		
    int *key_P;		
    int *key_Q;		
    int *key_R;		
    int *key_S;		
    int *key_T;		
    int *key_U;		
    int *key_V;		
    int *key_W;		
    int *key_X;		
    int *key_Y;		
    int *key_Z;		
    int *key_SP;		
    int *key_del;	
    int *key_slash;
    int *key_clr;	
    int *key_period;
    int *key_0;		
    int *key_minus;
    int *key_7;		
    int *key_8;		
    int *key_9;		
    int *key_4;		
    int *key_5;		
    int *key_6;		
    int *key_1;		
    int *key_2;		
    int *key_3;		    
    
    if (is_copilot) 
    {// keyname			dataref							Hardwareswitch-Nr.
      key_1L		=link_dataref_cmd_once("laminar/B738/button/fmc2_1L");		// sw-1
      key_2L		=link_dataref_cmd_once("laminar/B738/button/fmc2_2L");		// sw-2
      key_3L		=link_dataref_cmd_once("laminar/B738/button/fmc2_3L");   	// sw-3
      key_4L		=link_dataref_cmd_once("laminar/B738/button/fmc2_4L"); 		// sw-4
      key_5L		=link_dataref_cmd_once("laminar/B738/button/fmc2_5L"); 		// sw-5
      key_6L		=link_dataref_cmd_once("laminar/B738/button/fmc2_6L"); 		// sw-6
      key_1R		=link_dataref_cmd_once("laminar/B738/button/fmc2_1R"); 		// sw-7
      key_2R		=link_dataref_cmd_once("laminar/B738/button/fmc2_2R"); 		// sw-8
      key_3R		=link_dataref_cmd_once("laminar/B738/button/fmc2_3R"); 		// sw-9
      key_4R		=link_dataref_cmd_once("laminar/B738/button/fmc2_4R");		// sw-10
      key_5R		=link_dataref_cmd_once("laminar/B738/button/fmc2_5R"); 		// sw-11
      key_6R		=link_dataref_cmd_once("laminar/B738/button/fmc2_6R"); 		// sw-12
      key_init_ref	=link_dataref_cmd_once("laminar/B738/button/fmc2_init_ref"); 	// sw-13
      key_rte		=link_dataref_cmd_once("laminar/B738/button/fmc2_rte");		// sw-14
      key_clb		=link_dataref_cmd_once("laminar/B738/button/fmc2_clb"); 	// sw-15
      key_crz		=link_dataref_cmd_once("laminar/B738/button/fmc2_crz");		// sw-16
      key_des		=link_dataref_cmd_once("laminar/B738/button/fmc2_des");		// sw-17
      key_menu		=link_dataref_cmd_once("laminar/B738/button/fmc2_menu");	// sw-18
      key_legs		=link_dataref_cmd_once("laminar/B738/button/fmc2_legs"); 	// sw-19
      key_dep_app	=link_dataref_cmd_once("laminar/B738/button/fmc2_dep_app");	// sw-20
      key_hold		=link_dataref_cmd_once("laminar/B738/button/fmc2_hold");	// sw-21
      key_prog		=link_dataref_cmd_once("laminar/B738/button/fmc2_prog");	// sw-22
      key_exec		=link_dataref_cmd_once("laminar/B738/button/fmc2_exec");	// sw-23
      key_n1_lim	=link_dataref_cmd_once("laminar/B738/button/fmc2_n1_lim"); 	// sw-24
      key_fix		=link_dataref_cmd_once("laminar/B738/button/fmc2_fix");		// sw-25
      key_prev_page	=link_dataref_cmd_once("laminar/B738/button/fmc2_prev_page");	// sw-26
      key_next_page	=link_dataref_cmd_once("laminar/B738/button/fmc2_next_page");	// sw-27
      key_A		=link_dataref_cmd_once("laminar/B738/button/fmc2_A");		// sw-28
      key_B		=link_dataref_cmd_once("laminar/B738/button/fmc2_B");		// sw-29
      key_C		=link_dataref_cmd_once("laminar/B738/button/fmc2_C");		// sw-30
      key_D		=link_dataref_cmd_once("laminar/B738/button/fmc2_D");		// sw-31
      key_E		=link_dataref_cmd_once("laminar/B738/button/fmc2_E");		// sw-32
      key_F		=link_dataref_cmd_once("laminar/B738/button/fmc2_F");		// sw-33
      key_G		=link_dataref_cmd_once("laminar/B738/button/fmc2_G");		// sw-34
      key_H		=link_dataref_cmd_once("laminar/B738/button/fmc2_H");		// sw-35
      key_I		=link_dataref_cmd_once("laminar/B738/button/fmc2_I");		// sw-36
      key_J		=link_dataref_cmd_once("laminar/B738/button/fmc2_J");		// sw-37
      key_K		=link_dataref_cmd_once("laminar/B738/button/fmc2_K");		// sw-38
      key_L		=link_dataref_cmd_once("laminar/B738/button/fmc2_L");		// sw-39
      key_M		=link_dataref_cmd_once("laminar/B738/button/fmc2_M");		// sw-40
      key_N		=link_dataref_cmd_once("laminar/B738/button/fmc2_N");		// sw-41
      key_O		=link_dataref_cmd_once("laminar/B738/button/fmc2_O");		// sw-42
      key_P		=link_dataref_cmd_once("laminar/B738/button/fmc2_P");		// sw-43
      key_Q		=link_dataref_cmd_once("laminar/B738/button/fmc2_Q");		// sw-44
      key_R		=link_dataref_cmd_once("laminar/B738/button/fmc2_R");		// sw-45
      key_S		=link_dataref_cmd_once("laminar/B738/button/fmc2_S");		// sw-46
      key_T		=link_dataref_cmd_once("laminar/B738/button/fmc2_T");		// sw-47
      key_U		=link_dataref_cmd_once("laminar/B738/button/fmc2_U");		// sw-48
      key_V		=link_dataref_cmd_once("laminar/B738/button/fmc2_V");		// sw-49
      key_W		=link_dataref_cmd_once("laminar/B738/button/fmc2_W");		// sw-50
      key_X		=link_dataref_cmd_once("laminar/B738/button/fmc2_X");		// sw-51
      key_Y		=link_dataref_cmd_once("laminar/B738/button/fmc2_Y");		// sw-52
      key_Z		=link_dataref_cmd_once("laminar/B738/button/fmc2_Z");		// sw-53
      key_SP		=link_dataref_cmd_once("laminar/B738/button/fmc2_SP");		// sw-54
      key_del		=link_dataref_cmd_once("laminar/B738/button/fmc2_del");		// sw-55
      key_slash		=link_dataref_cmd_once("laminar/B738/button/fmc2_slash");	// sw-56
      key_clr		=link_dataref_cmd_once("laminar/B738/button/fmc2_clr");		// sw-57
      key_period	=link_dataref_cmd_once("laminar/B738/button/fmc2_period");	// sw-58
      key_0		=link_dataref_cmd_once("laminar/B738/button/fmc2_0");		// sw-59
      key_minus		=link_dataref_cmd_once("laminar/B738/button/fmc2_minus");	// sw-60
      key_7		=link_dataref_cmd_once("laminar/B738/button/fmc2_7"); 		// sw-61
      key_8		=link_dataref_cmd_once("laminar/B738/button/fmc2_8");		// sw-62
      key_9		=link_dataref_cmd_once("laminar/B738/button/fmc2_9");		// sw-63
      key_4		=link_dataref_cmd_once("laminar/B738/button/fmc2_4");		// sw-64
      key_5		=link_dataref_cmd_once("laminar/B738/button/fmc2_5");		// sw-65
      key_6		=link_dataref_cmd_once("laminar/B738/button/fmc2_6");		// sw-66
      key_1		=link_dataref_cmd_once("laminar/B738/button/fmc2_1");		// sw-67
      key_2		=link_dataref_cmd_once("laminar/B738/button/fmc2_2");		// sw-68
      key_3		=link_dataref_cmd_once("laminar/B738/button/fmc2_3");		// sw-69
    }
 else 
    {

      key_1L		=link_dataref_cmd_once("laminar/B738/button/fmc1_1L");		// sw-1
      key_2L		=link_dataref_cmd_once("laminar/B738/button/fmc1_2L");		// sw-2
      key_3L		=link_dataref_cmd_once("laminar/B738/button/fmc1_3L");   	// sw-3
      key_4L		=link_dataref_cmd_once("laminar/B738/button/fmc1_4L"); 		// sw-4
      key_5L		=link_dataref_cmd_once("laminar/B738/button/fmc1_5L"); 		// sw-5
      key_6L		=link_dataref_cmd_once("laminar/B738/button/fmc1_6L"); 		// sw-6
      key_1R		=link_dataref_cmd_once("laminar/B738/button/fmc1_1R"); 		// sw-7
      key_2R		=link_dataref_cmd_once("laminar/B738/button/fmc1_2R"); 		// sw-8
      key_3R		=link_dataref_cmd_once("laminar/B738/button/fmc1_3R"); 		// sw-9
      key_4R		=link_dataref_cmd_once("laminar/B738/button/fmc1_4R");		// sw-10
      key_5R		=link_dataref_cmd_once("laminar/B738/button/fmc1_5R"); 		// sw-11
      key_6R		=link_dataref_cmd_once("laminar/B738/button/fmc1_6R"); 		// sw-12
      key_init_ref	=link_dataref_cmd_once("laminar/B738/button/fmc1_init_ref"); 	// sw-13
      key_rte		=link_dataref_cmd_once("laminar/B738/button/fmc1_rte");		// sw-14
      key_clb		=link_dataref_cmd_once("laminar/B738/button/fmc1_clb"); 	// sw-15
      key_crz		=link_dataref_cmd_once("laminar/B738/button/fmc1_crz");		// sw-16
      key_des		=link_dataref_cmd_once("laminar/B738/button/fmc1_des");		// sw-17
      key_menu		=link_dataref_cmd_once("laminar/B738/button/fmc1_menu");	// sw-18
      key_legs		=link_dataref_cmd_once("laminar/B738/button/fmc1_legs"); 	// sw-19
      key_dep_app	=link_dataref_cmd_once("laminar/B738/button/fmc1_dep_app");	// sw-20
      key_hold		=link_dataref_cmd_once("laminar/B738/button/fmc1_hold");		// sw-21
      key_prog		=link_dataref_cmd_once("laminar/B738/button/fmc1_prog");		// sw-22
      key_exec		=link_dataref_cmd_once("laminar/B738/button/fmc1_exec");		// sw-23
      key_n1_lim	=link_dataref_cmd_once("laminar/B738/button/fmc1_n1_lim"); 	// sw-24
      key_fix		=link_dataref_cmd_once("laminar/B738/button/fmc1_fix");		// sw-25
      key_prev_page	=link_dataref_cmd_once("laminar/B738/button/fmc1_prev_page");	// sw-26
      key_next_page	=link_dataref_cmd_once("laminar/B738/button/fmc1_next_page");	// sw-27
      key_A		=link_dataref_cmd_once("laminar/B738/button/fmc1_A");		// sw-28
      key_B		=link_dataref_cmd_once("laminar/B738/button/fmc1_B");		// sw-29
      key_C		=link_dataref_cmd_once("laminar/B738/button/fmc1_C");		// sw-30
      key_D		=link_dataref_cmd_once("laminar/B738/button/fmc1_D");		// sw-31
      key_E		=link_dataref_cmd_once("laminar/B738/button/fmc1_E");		// sw-32
      key_F		=link_dataref_cmd_once("laminar/B738/button/fmc1_F");		// sw-33
      key_G		=link_dataref_cmd_once("laminar/B738/button/fmc1_G");		// sw-34
      key_H		=link_dataref_cmd_once("laminar/B738/button/fmc1_H");		// sw-35
      key_I		=link_dataref_cmd_once("laminar/B738/button/fmc1_I");		// sw-36
      key_J		=link_dataref_cmd_once("laminar/B738/button/fmc1_J");		// sw-37
      key_K		=link_dataref_cmd_once("laminar/B738/button/fmc1_K");		// sw-38
      key_L		=link_dataref_cmd_once("laminar/B738/button/fmc1_L");		// sw-39
      key_M		=link_dataref_cmd_once("laminar/B738/button/fmc1_M");		// sw-40
      key_N		=link_dataref_cmd_once("laminar/B738/button/fmc1_N");		// sw-41
      key_O		=link_dataref_cmd_once("laminar/B738/button/fmc1_O");		// sw-42
      key_P		=link_dataref_cmd_once("laminar/B738/button/fmc1_P");		// sw-43
      key_Q		=link_dataref_cmd_once("laminar/B738/button/fmc1_Q");		// sw-44
      key_R		=link_dataref_cmd_once("laminar/B738/button/fmc1_R");		// sw-45
      key_S		=link_dataref_cmd_once("laminar/B738/button/fmc1_S");		// sw-46
      key_T		=link_dataref_cmd_once("laminar/B738/button/fmc1_T");		// sw-47
      key_U		=link_dataref_cmd_once("laminar/B738/button/fmc1_U");		// sw-48
      key_V		=link_dataref_cmd_once("laminar/B738/button/fmc1_V");		// sw-49
      key_W		=link_dataref_cmd_once("laminar/B738/button/fmc1_W");		// sw-50
      key_X		=link_dataref_cmd_once("laminar/B738/button/fmc1_X");		// sw-51
      key_Y		=link_dataref_cmd_once("laminar/B738/button/fmc1_Y");		// sw-52
      key_Z		=link_dataref_cmd_once("laminar/B738/button/fmc1_Z");		// sw-53
      key_SP		=link_dataref_cmd_once("laminar/B738/button/fmc1_SP");		// sw-54
      key_del		=link_dataref_cmd_once("laminar/B738/button/fmc1_del");		// sw-55
      key_slash		=link_dataref_cmd_once("laminar/B738/button/fmc1_slash");	// sw-56
      key_clr		=link_dataref_cmd_once("laminar/B738/button/fmc1_clr");		// sw-57
      key_period	=link_dataref_cmd_once("laminar/B738/button/fmc1_period");	// sw-58
      key_0		=link_dataref_cmd_once("laminar/B738/button/fmc1_0");		// sw-59
      key_minus		=link_dataref_cmd_once("laminar/B738/button/fmc1_minus");	// sw-60
      key_7		=link_dataref_cmd_once("laminar/B738/button/fmc1_7"); 		// sw-61
      key_8		=link_dataref_cmd_once("laminar/B738/button/fmc1_8");		// sw-62
      key_9		=link_dataref_cmd_once("laminar/B738/button/fmc1_9");		// sw-63
      key_4		=link_dataref_cmd_once("laminar/B738/button/fmc1_4");		// sw-64
      key_5		=link_dataref_cmd_once("laminar/B738/button/fmc1_5");		// sw-65
      key_6		=link_dataref_cmd_once("laminar/B738/button/fmc1_6");		// sw-66
      key_1		=link_dataref_cmd_once("laminar/B738/button/fmc1_1");		// sw-67
      key_2		=link_dataref_cmd_once("laminar/B738/button/fmc1_2");		// sw-68
      key_3		=link_dataref_cmd_once("laminar/B738/button/fmc1_3");		// sw-69      
    }
    
    
    *key_0 = 0;
    *key_1L = 0;	
    *key_2L = 0;	
    *key_3L = 0;	
    *key_4L = 0;	
    *key_5L = 0;	
    *key_6L = 0;	
    *key_1R = 0;	
    *key_2R = 0;	
    *key_3R = 0;	
    *key_4R = 0;	
    *key_5R = 0;	
    *key_6R = 0;	
    *key_init_ref = 0;   
    *key_rte = 0;		
    *key_clb = 0;		
    *key_crz = 0;		
    *key_des = 0;		
    *key_menu = 0;		
    *key_legs = 0;		
    *key_dep_app = 0;
    *key_hold = 0;		
    *key_prog = 0;		
    *key_exec = 0;		
    *key_n1_lim = 0;	
    *key_fix = 0;		    
    *key_prev_page = 0;
    *key_next_page = 0;   
    *key_A = 0;		
    *key_B = 0;		
    *key_C = 0;		
    *key_D = 0;		
    *key_E = 0;		
    *key_F = 0;		
    *key_G = 0;		
    *key_H = 0;		
    *key_I = 0;		
    *key_J = 0;		
    *key_K = 0;		
    *key_L = 0;		
    *key_M = 0;		
    *key_N = 0;		
    *key_O = 0;		
    *key_P = 0;		
    *key_Q = 0;		
    *key_R = 0;		
    *key_S = 0;		
    *key_T = 0;		
    *key_U = 0;		
    *key_V = 0;		
    *key_W = 0;		
    *key_X = 0;		
    *key_Y = 0;		
    *key_Z = 0;		
    *key_SP = 0;		
    *key_del = 0;	
    *key_slash = 0;
    *key_clr = 0;	
    *key_period = 0;
    *key_0 = 0;		
    *key_minus = 0;
    *key_7 = 0;		
    *key_8 = 0;		
    *key_9 = 0;		
    *key_4 = 0;		
    *key_5 = 0;		
    *key_6 = 0;		
    *key_1 = 0;		
    *key_2 = 0;		
    *key_3 = 0;		    

    /* Scan Keyboard Matrix */

    /* set columns as outputs, HIGH by default */
    for (pin = 0; pin < nCols; pin++) {
      digitalWrite(colPins[pin], HIGH);
      pinMode(colPins[pin], OUTPUT);
    }
  
    /* set rows as inputs, with Pull-UP enabled. */
    for (pin = 0; pin < nRows; pin++) {
      pinMode(rowPins[pin], INPUT);
      pullUpDnControl(rowPins[pin], PUD_UP);
    }

    /* iterate through the columns */
    int gotPress = 0;
    for (nowCol = 0; nowCol < nCols; nowCol++) 
    {
    
      // set the current column to LOW
      digitalWrite(colPins[nowCol], 0);
    
      // go through the rows, see who is low (pressed)
    
      for (nowRow = 0; nowRow < nRows; nowRow++) {
      
	// delay a bit for the GPIO state to settle
	usleep(5);
	int status = digitalRead(rowPins[nowRow]);
      
	// something pressed
	if (status == 0) {
	  gotPress = 1;
	  if (somethingPressed == 1) {
	    if (pressedRow == nowRow && pressedCol == nowCol) {
	    
	      // previous key still being held down.
	    
	    } else {
	      // multiple keys being pressed or another key pressed
	      // without the earlier being released. Do nothing.
	    
	      printf("Multiple keys pressed.\n");
	    }
	  } else {
	    // looks like a new key has been pressed
	    pressedRow = nowRow;
	    pressedCol = nowCol;
	    somethingPressed = 1;
// Piki;  i have changed nowRow with nowCol, because it represents the Logic of the Hardware-Keybord better; 	  
	    printf("KEY PRESS   col=%02d row=%02d\n", nowCol,nowRow);

	    if (nowCol==0) 
	    {	
		if (nowRow==0) {
		  *key_1L = 1;
		} else if (nowRow==1) {
		  *key_2L = 1;
		} else if (nowRow==2) {
		  *key_3L = 1;
		} else if (nowRow==3) {
		  *key_4L = 1;
		} else if (nowRow==4) {
		  *key_5L = 1;
		} else if (nowRow==5) {
		  *key_6L = 1;
		} else if (nowRow==6) {
		  *key_1R = 1;
		} else if (nowRow==7) {	
		      *key_2R  = 1;
		} 
	    } 
	    else if (nowCol==1) 
	    {
		if (nowRow==0) {
		  *key_3R = 1;
		} else if (nowRow==1) {
		   *key_4R = 1;
		} else if (nowRow==2) {
		  *key_5R = 1;
		} else if (nowRow==3) {
		  *key_6R = 1;
		} else if (nowRow==4) {
		  *key_init_ref = 1;
		} else if (nowRow==5) {
		  *key_rte = 1;
		} else if (nowRow==6) {
		  *key_clb = 1;
		} else if (nowRow==7) {	
		       *key_crz =1;
		} 
	    } 
	    else if (nowCol==2) 
	    {
		if (nowRow==0) {
		  *key_des = 1; 
		} else if (nowRow==1) {
		  *key_menu = 1;
		} else if (nowRow==2) {
		  *key_legs = 1;
		} else if (nowRow==3) {
		  *key_dep_app = 1;
		} else if (nowRow==4) {
		  *key_hold = 1;
		} else if (nowRow==5) {
		  *key_prog = 1;
		} else if (nowRow==6) {
		  *key_exec = 1;
		} else if (nowRow==7) {	 
		  *key_n1_lim = 1;     
		} 
	    } 
	    else if (nowCol==3) 
	      {
		if (nowRow==0) {
		  *key_fix = 1;
		} else if (nowRow==1) {
		  *key_prev_page = 1;
		} else if (nowRow==2) {
		  *key_next_page = 1;
		} else if (nowRow==3) {
		  *key_A = 1;
		} else if (nowRow==4) {
		  *key_B = 1;
		} else if (nowRow==5) {
		  *key_C = 1;
		} else if (nowRow==6) {
		  *key_D = 1;
		} else if (nowRow==7) {	 
		  *key_E = 1;     
		} 
	      } 
	    else if (nowCol==4) 
	    {
		if (nowRow==0) {
		  *key_F = 1;
		} else if (nowRow==1) {
		  *key_G = 1;
		} else if (nowRow==2) {
		  *key_H = 1;
		} else if (nowRow==3) {
		  *key_I = 1;
		} else if (nowRow==4) {
		  *key_J = 1;
		} else if (nowRow==5) {
		  *key_K = 1;
		} else if (nowRow==6) {
		  *key_L = 1;
		} else if (nowRow==7) {
		  *key_M = 1;	      
		} 
	    } 
	    else if (nowCol==5) 
	    {
		if (nowRow==0) {
		  *key_N = 1;
		} else if (nowRow==1) {
		  *key_O = 1;
		} else if (nowRow==2) {
		  *key_P = 1;
		} else if (nowRow==3) {
		  *key_Q = 1;
		} else if (nowRow==4) {
		  *key_R = 1;
		} else if (nowRow==5) {
		  *key_S = 1;
		} else if (nowRow==6) {
		  *key_T = 1;
		} else if (nowRow==7) {	
		  *key_U = 1;      
		} 
	    } 
	    else if (nowCol==6) 
	    {
		if (nowRow==0) {
		  *key_V = 1;
		} else if (nowRow==1) {
		  *key_W = 1;
		} else if (nowRow==2) {
		  *key_X = 1;
		} else if (nowRow==3) {
		  *key_Y = 1;
		} else if (nowRow==4) {
		  *key_Z = 1;
		} else if (nowRow==5) {
		  *key_SP = 1;
		} else if (nowRow==6) {
		  *key_del = 1;
		} else if (nowRow==7) {	
		  *key_slash = 1;
		} 
	    } 
	    else if (nowCol==7) 
	    {
		if (nowRow==0) {
		  *key_clr = 1;
		} else if (nowRow==1) {
		  *key_period = 1;
		} else if (nowRow==2) {
		  *key_0 = 1;
		} else if (nowRow==3) {
		  *key_minus = 1;
		} else if (nowRow==4) {
		  *key_7 = 1;
		} else if (nowRow==5) {
		  *key_8 = 1;
		} else if (nowRow==6) {
		  *key_9 = 1;
		} else if (nowRow==7) {	
		  *key_4 = 1;      
		}
	  }
	    else if (nowCol==8) 
	    {
		if (nowRow==0) {
		  *key_5 = 1; 
		} else if (nowRow==1) {
		  *key_6 = 1;
		} else if (nowRow==2) {
		  *key_1 = 1;
		} else if (nowRow==3) {
		  *key_2 = 1;
		} else if (nowRow==4) {
		  *key_3 = 1;
		} 
	    }
	  
	    // quick and dirty delay for 5ms to debounce
	    usleep(5000);
	  }
	
	}
      
      }
      // restore the current column to HIGH
      digitalWrite(colPins[nowCol], HIGH);
    
    }
  
    if (!gotPress) 
    {
      // nothing pressed this scan. If something was pressed before, consider
      // it released.
      if (somethingPressed) 
      {
	somethingPressed = 0;
      
	printf("KEY RELEASE col=%02d row=%02d\n",  pressedCol,pressedRow);
      }
    }

  } /* only run for ZIBO B737 */
  
}

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
#include <pthread.h>

#include "common.h"
#include "serverdata.h"
#include "b737_fmc.h"

/* WIRING PI PIN NUMBERS USED */

#define nCols 9
#define nRows 8

// columns are outputs. All are set to HIGH except the column being scanned.
/* const int colPins[] = {  */
/* 		15,     // p.8  BCM.14 	UART TX Pi3 OK */
/* 		16,     // p.10 BCM.15 	UART RX Pi3 OK */
/* 		1, 	// p.12 BCM.18 	PWM0 Pi3 OK */
/* 		4, 	// p.16 BCM.23 	Pi3 OK */
/* 		5, 	// p.18 BCM.24 	Pi3 OK */
/* 		6,	// p.22 BCM.25 	Pi3 OK */
/* 		10,	// p.24	BCM.8   CE0 Pi3 OK */
/* 		11,	// p.26 BCM.7	CE1 Pi3 OK */
/* 		27	// p.36 BCM.16  Pi3 OK */
/* 		}; */
/* // rows are inputs. */
/* const int rowPins[] = {  */
/* 		29,	// p.40 BCM.21	SCLK Pi3 OK */
/* 		8,	// p.3  BCM.2   I2C SDA Pi3 OK */
/* 		9,	// p.5  BCM.3   I2C SCL Pi3 OK */
/* 		7,	// p.7  BCM.4   GPCLK0 Pi3 OK */
/* 		0,	// p.11 BCM.17  Pi3 OK */
/* 		2, 	// p.13 BCM.27  Pi3 OK */
/* 		3,	// p.15 BCM.22  Pi3 OK */
/* 		12	// p.19 BCM.10  MOSI Pi3 OK */
/* 		}; */

/* const int exec_led_pin = 13; // p. 21 BCM 9  Pi3 OK */
/* const int msg_led_pin = 14;  // p. 23 BCM 11 Pi3 OK */
/* const int dspy_led_pin = 21; // p. 29 BCM 5  Pi3 OK */
/* const int ofst_led_pin = 22; // p. 31 BCM 6  Pi3 OK */
/* const int fail_led_pin = 23; // p. 33 BCM 13 Pi3 OK */
/* const int bgl_pwm_pin = 24;  // p. 35 BCM 19 Pi3 OK */
/* const int rot_sw_pin = 26;   // p. 32 BCM 12 */
/* const int rot_a_pin = 25;    // p. 37 BCM 26 */
/* const int rot_b_pin = 28;    // p. 38 BCM 20 */

const int colPins[] = { 
		14,     // p.8  BCM.14 	UART TX Pi3 OK
		15,     // p.10 BCM.15 	UART RX Pi3 OK
		18, 	// p.12 BCM.18 	PWM0 Pi3 OK
		23, 	// p.16 BCM.23 	Pi3 OK
		24, 	// p.18 BCM.24 	Pi3 OK
		25,	// p.22 BCM.25 	Pi3 OK
		8,	// p.24	BCM.8   CE0 Pi3 OK
		7,	// p.26 BCM.7	CE1 Pi3 OK
		16	// p.36 BCM.16  Pi3 OK
		};
// rows are inputs.
const int rowPins[] = { 
		21,	// p.40 BCM.21	SCLK Pi3 OK
		2,	// p.3  BCM.2   I2C SDA Pi3 OK
		3,	// p.5  BCM.3   I2C SCL Pi3 OK
		4,	// p.7  BCM.4   GPCLK0 Pi3 OK
		17,	// p.11 BCM.17  Pi3 OK
		27, 	// p.13 BCM.27  Pi3 OK
		22,	// p.15 BCM.22  Pi3 OK
		10	// p.19 BCM.10  MOSI Pi3 OK
		};

const int exec_led_pin = 9; // p. 21 BCM 9  Pi3 OK
const int msg_led_pin = 11;  // p. 23 BCM 11 Pi3 OK
const int dspy_led_pin = 5; // p. 29 BCM 5  Pi3 OK
const int ofst_led_pin = 6; // p. 31 BCM 6  Pi3 OK
const int fail_led_pin = 13; // p. 33 BCM 13 Pi3 OK
const int bgl_pwm_pin = 19;  // p. 35 BCM 19 Pi3 OK
const int rot_sw_pin = 12;   // p. 32 BCM 12
const int rot_a_pin = 26;    // p. 37 BCM 26
const int rot_b_pin = 20;    // p. 38 BCM 20

/* 3D array with 9 Columns x 8 Rows x 10 Chars */
const char zibo[nCols][nRows][12] = {{"_1L","_2L","_3L","_4L","_5L","_6L","_1R","_2R"},
				     {"_3R","_4R","_5R","_6R","_init_ref","_rte","_clb","_crz"},
				     {"_des","_menu","_legs","_dep_app","_hold","_prog","_exec","_n1_lim"},
				     {"_fix","_prev_page","_next_page","_A","_B","_C","_D","_E"},
				     {"_F","_G","_H","_I","_J","_K","_L","_M"},
				     {"_N","_O","_P","_Q","_R","_S","_T","_U"},
				     {"_V","_W","_X","_Y","_Z","_SP","_del","_slash"},
				     {"_clr","_period","_0","_minus","_7","_8","_9","_4"},
				     {"_5","_6","_1","_2","_3","","",""}};


float key_brightness_save;

int rot_sw_save;
int rot_a_save;
int rot_b_save;
int rot_direction;

pthread_t poll_thread;      /* read thread */
int poll_thread_exit_code;  /* read thread exit code */

int pressedRow;
int pressedCol;
int somethingPressed;

void *poll_thread_main()
/* thread handles inputs which change faster than xppi cycle */
{

  int changed;
  int rot_a = 0;
  int rot_b = 0;
  
  printf("Input thread running \n");

  while (!poll_thread_exit_code) {
    /* Put code here to asynchronously do operations from main thread */

#ifdef PIGPIO
#else
    rot_a = digitalRead(rot_a_pin);
#endif
    if (rot_a != rot_a_save) changed = 1;

#ifdef PIGPIO
#else
    rot_b = digitalRead(rot_b_pin);
#endif
    if (rot_b != rot_b_save) changed = 1;
      
    if (changed) {
      /* We have a rotary encoder which does a 2 x 2 byte change with each detent */ 
      if ((rot_a_save==0) && (rot_b_save==1) && (rot_a==0) && (rot_b==0)) {
	rot_direction = -1;
      } else if ((rot_a_save==0) && (rot_b_save==1) && (rot_a==1) && (rot_b==1)) {
	rot_direction = 1;
      } else if ((rot_a_save==1) && (rot_b_save==0) && (rot_a==1) && (rot_b==1)) {
	rot_direction = -1;
      } else if ((rot_a_save==1) && (rot_b_save==0) && (rot_a==0) && (rot_b==0)) {
	rot_direction = 1;
      }
      
      //printf("Rotary A/B/DIR: %i %i %i\n",rot_a,rot_b,rot_direction);

      /* update saved states */
      rot_a_save = rot_a;
      rot_b_save = rot_b;
      changed = 0;
    }
   
    usleep(1000);
  } /* while loop */
  
  /* thread was killed */
  printf("Input thread shutting down \n");

  return 0;
}

int b737_fmc_init()
{
  int pin;

  /* Initialize Values */
  key_brightness_save = 0.0;
  rot_sw_save = 0;
  rot_a_save = 0;
  rot_b_save = 0;
  rot_direction = 0;
  
  /* Initialize pins */
#ifdef PIGPIO
#else
  pinMode(exec_led_pin, OUTPUT);
  pinMode(msg_led_pin,  OUTPUT);
  pinMode(dspy_led_pin, OUTPUT);
  pinMode(ofst_led_pin, OUTPUT);
  pinMode(fail_led_pin, OUTPUT);
  pinMode(bgl_pwm_pin,  OUTPUT);
  softPwmCreate(bgl_pwm_pin,0,100); //Pin,initalValue,pwmRange    

  pinMode(rot_a_pin,  INPUT);
  pinMode(rot_a_pin,  INPUT);
  pinMode(rot_b_pin,  INPUT);
  pullUpDnControl(rot_sw_pin, PUD_UP);
  pullUpDnControl(rot_a_pin, PUD_UP);
  pullUpDnControl(rot_b_pin, PUD_UP);
#endif
  
  /* set columns as outputs, HIGH by default */
  for (pin = 0; pin < nCols; pin++) {
#ifdef PIGPIO
#else
    digitalWrite(colPins[pin], HIGH);
    pinMode(colPins[pin], OUTPUT);
#endif
  }
  
  /* set rows as inputs, with Pull-UP enabled. */
  for (pin = 0; pin < nRows; pin++) {
#ifdef PIGPIO
#else
    pinMode(rowPins[pin], INPUT);
    pullUpDnControl(rowPins[pin], PUD_UP);
#endif
  }

  /* init thread to read encoder since encoder changes are too fast
     for repeat cycle of xppi */
  poll_thread_exit_code = 0;
  if (pthread_create(&poll_thread, NULL, &poll_thread_main, NULL)>0) {
    printf("Poll thread could not be created.\n");
    return -1;
  }

  return 0;
}

void b737_fmc_exit(void)
{
  /* shut down read thread */
  poll_thread_exit_code = 1;
  pthread_join(poll_thread, NULL);
}

void b737_fmc()
{

  /* only run for ZIBO 737 for now */
  if (acf_type == 3) {

    int nowCol;
    int nowRow;
    int i,j;
    int status;
    char datarefname[100];
    int *datarefptr[nCols][nRows];

    /* Link Datarefs */
   
    float *key_brightness = link_dataref_flt_arr("laminar/B738/electric/panel_brightness",4,3,-2);
    
    int *exec_led = link_dataref_int("laminar/B738/indicators/fmc_exec_lights");
    int *msg_led = link_dataref_int("laminar/B738/fmc/fmc_message");
    /*
    int *dspy_led = link_dataref_int("");
    int *ofst_led = link_dataref_int("");
    int *fail_led = link_dataref_int("");
    */

    for (j=0;j<nCols;j++) {
      for (i=0;i<nRows;i++) {
	datarefptr[j][i] = NULL;
	if (strcmp(zibo[j][i],"")) {
	  if (is_copilot) {
	    strncpy(datarefname,"laminar/B738/button/fmc2",sizeof(datarefname));
	  } else {
	    strncpy(datarefname,"laminar/B738/button/fmc1",sizeof(datarefname));
	  }
	  strcat(datarefname,zibo[j][i]);
	  //printf("%s\n",datarefname);
	  datarefptr[j][i] = link_dataref_cmd_once(datarefname);
	}
      }
    }
    
    // Only update key brightness PWM if it has changed
    if ((*key_brightness != key_brightness_save) &&
	(*key_brightness != FLT_MISS)) {
      printf("New Key Brightness: %f \n",*key_brightness);
      key_brightness_save = *key_brightness;
#ifdef PIGPIO
#else
      softPwmWrite(bgl_pwm_pin, (int) (*key_brightness * 100.0));
#endif
    }
    
#ifdef PIGPIO
#else
    if (*exec_led != INT_MISS) digitalWrite(exec_led_pin, *exec_led);
    if (*msg_led != INT_MISS) digitalWrite(msg_led_pin, *msg_led);  
    /*
    if (*dspy_led != INT_MISS) digitalWrite(dspy_led_pin, *dspy_led);
    if (*ofst_led != INT_MISS) digitalWrite(ofst_led_pin, *ofst_led);
    if (*fail_led != INT_MISS) digitalWrite(fail_led_pin, *fail_led);
    */
#endif

    /* fetch rotary encoder switch: not needed since switch 
       is directly connected to the MENU key of the display */
    /*
    status = digitalRead(rot_sw_pin);
    if (status != rot_sw_save) {
      printf("Rotary Switch: %i\n",status);
      rot_sw_save = status;
    }
    */

    
    /* DSPY_LED connected to DOWN key of the display */
    if (rot_direction == -1) {
      printf("Rotary DOWN: \n");
#ifdef PIGPIO
#else
      digitalWrite(dspy_led_pin, 1);
#endif
      rot_direction = 0;
    } else {
#ifdef PIGPIO
#else
      digitalWrite(dspy_led_pin, 0);
#endif
    }
      
    /* FAIL_LED connected to UP key of the display */
    if (rot_direction == 1) {
      printf("Rotary UP: \n");
#ifdef PIGPIO
#else
      digitalWrite(fail_led_pin, 1);
#endif
      rot_direction = 0;
    } else {
#ifdef PIGPIO
#else
      digitalWrite(fail_led_pin, 0);
#endif
    }


    /* -------------------- */
    /* Scan Keyboard Matrix */
    /* -------------------- */

    /* iterate through the columns */
    int gotPress = 0;
    for (nowCol = 0; nowCol < nCols; nowCol++) 
    {
    
      // set the current column to LOW
#ifdef PIGPIO
#else
      digitalWrite(colPins[nowCol], 0);
#endif    
      // go through the rows, see who is low (pressed)
    
      for (nowRow = 0; nowRow < nRows; nowRow++) {
     
	// delay a bit for the GPIO state to settle
	usleep(5);
#ifdef PIGPIO
#else
	status = digitalRead(rowPins[nowRow]);
#endif      
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
	    // Piki;  i have changed nowRow with nowCol, because it
	    // represents the Logic of the Hardware-Keybord better; 	  
	    printf("KEY PRESS   col=%02d row=%02d\n", nowCol,nowRow);

	    if (datarefptr[nowCol][nowRow]) {
	      *datarefptr[nowCol][nowRow] = 1;
	    }
	    
	    // quick and dirty delay for 5ms to debounce
	    usleep(5000);
	  }
	
	}
      
      }
      // restore the current column to HIGH
#ifdef PIGPIO
#else
      digitalWrite(colPins[nowCol], HIGH);
#endif    
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

  } /* only run for ZIBO B737 for now */
  
}

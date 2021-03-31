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
const int colPins[] = { 15, // p.8  BCM.14 	BLACK UART TX
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
const int rowPins[] = { 29,	//	p.40 BCM.21	GRAY SCLK
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
    int *exec_led = link_dataref_int("laminar/B738/indicators/fmc_exec_lights");
  
    pinMode(exec_led_pin, OUTPUT);
    if (*exec_led != INT_MISS) digitalWrite(exec_led_pin, *exec_led);


    /* link key datarefs */
    int *key_dot;
    int *key_0;
    if (is_copilot) {
      key_dot=link_dataref_cmd_once("laminar/B738/button/fmc2_period");
      key_0=link_dataref_cmd_once("laminar/B738/button/fmc2_0");
    } else {
      key_dot=link_dataref_cmd_once("laminar/B738/button/fmc1_period");
      key_0=link_dataref_cmd_once("laminar/B738/button/fmc1_0");
    }
    *key_dot = 0;
    *key_0 = 0;

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
    for (nowCol = 0; nowCol < nCols; nowCol++) {
    
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
	  
	    printf("KEY PRESS   row=%02d col=%02d\n", nowRow,nowCol);

	    if (nowRow==0) {	    
	      if (nowCol==0) {
	      } else if (nowCol==1) {
	      } else if (nowCol==2) {
	      } else if (nowCol==3) {
	      } else if (nowCol==4) {
	      } else if (nowCol==5) {
	      } else if (nowCol==6) {
	      } else if (nowCol==7) {	      
	      } else if (nowCol==8) {
		*key_dot = 1;
	      } 
	    } else if (nowRow==1) {
	      if (nowCol==0) {
	      } else if (nowCol==1) {
	      } else if (nowCol==2) {
	      } else if (nowCol==3) {
	      } else if (nowCol==4) {
	      } else if (nowCol==5) {
	      } else if (nowCol==6) {
	      } else if (nowCol==7) {	      
	      } else if (nowCol==8) {
	      } 
	    } else if (nowRow==2) {
	      if (nowCol==0) {
	      } else if (nowCol==1) {
	      } else if (nowCol==2) {
	      } else if (nowCol==3) {
	      } else if (nowCol==4) {
	      } else if (nowCol==5) {
	      } else if (nowCol==6) {
	      } else if (nowCol==7) {	      
	      } else if (nowCol==8) {
	      } 
	    } else if (nowRow==3) {
	      if (nowCol==0) {
	      } else if (nowCol==1) {
	      } else if (nowCol==2) {
	      } else if (nowCol==3) {
	      } else if (nowCol==4) {
	      } else if (nowCol==5) {
	      } else if (nowCol==6) {
	      } else if (nowCol==7) {	      
	      } else if (nowCol==8) {
	      } 
	    } else if (nowRow==4) {
	      if (nowCol==0) {
	      } else if (nowCol==1) {
	      } else if (nowCol==2) {
	      } else if (nowCol==3) {
	      } else if (nowCol==4) {
	      } else if (nowCol==5) {
	      } else if (nowCol==6) {
	      } else if (nowCol==7) {	      
	      } else if (nowCol==8) {
	      } 
	    } else if (nowRow==5) {
	      if (nowCol==0) {
	      } else if (nowCol==1) {
	      } else if (nowCol==2) {
	      } else if (nowCol==3) {
	      } else if (nowCol==4) {
	      } else if (nowCol==5) {
	      } else if (nowCol==6) {
	      } else if (nowCol==7) {	      
	      } else if (nowCol==8) {
	      } 
	    } else if (nowRow==6) {
	      if (nowCol==0) {
	      } else if (nowCol==1) {
	      } else if (nowCol==2) {
	      } else if (nowCol==3) {
	      } else if (nowCol==4) {
	      } else if (nowCol==5) {
	      } else if (nowCol==6) {
	      } else if (nowCol==7) {	      
	      } else if (nowCol==8) {
	      } 
	    } else if (nowRow==7) {
	      if (nowCol==0) {
	      } else if (nowCol==1) {
	      } else if (nowCol==2) {
	      } else if (nowCol==3) {
	      } else if (nowCol==4) {
	      } else if (nowCol==5) {
	      } else if (nowCol==6) {
	      } else if (nowCol==7) {	      
	      } else if (nowCol==8) {
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
  
    if (!gotPress) {
      // nothing pressed this scan. If something was pressed before, consider
      // it released.
      if (somethingPressed) {
	somethingPressed = 0;
      
	printf("KEY RELEASE row=%02d col=%02d\n", pressedRow,pressedCol);
      }
    }

  } /* only run for ZIBO B737 */
  
}

/* This is the b737_fdas.c code which controls the flight deck access system (FDAS)
   in my Boeing 737 cockpit using a Teensy. It is built with the original B737
   post 9/11 FDAS components, but not using the original electronics since I could
   not reverse engineer its function due to missing schematics and microcontroller code.

   FDAS Function Video: https://www.youtube.com/watch?v=D7C67HSGAOA

   RED LED is always on, also when the FDAS Switch is set from NORM to OFF
   
   Standard Entry:
   - RED LED: Locked
   - Keypad: 1 + ENT
   - 2 Chime

   INDEPENENT OF Standard Entry:
   - Rotary: UNLK --> GREEN LED
   - Door open while UNLK is hold

   Emergency Entry
   - RED LED: Locked
   - Keypad: CODE + ENT
   - 2 Chime
   - YELLOW LED (correct code), else RED LED stays illuminated
   - AUTO UNLOCK annunciator illuminates
   - 5 seconds delay: 2 Chime
   - 5 seconds delay: steady chime + Flashing AUTO UNLOCK
   - 5 seconds delay: AUTO UNLOCK stops flashing + GREEN LED + Door Unlocks for 5 Seconds

   --> Pressing DENY: RED LED + stop AUTO UNLOCK
   --> Keypad disabled for X seconds

   --> FDAS Switch OFF: LOCK FAIL illuminates
   
   Copyright (C) 2025 Reto Stockli

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
#include "b737_fdas.h"

#define STD_CODE 1       /* Door entry code for regular entry */
#define EMER_CODE 1415   /* Door entry code for emergency entry */
#define DT_BLOCK 10.0    /* Timeout [s] before you can enter a new code after wrong code */
#define DT_TIMEOUT 10.0  /* Max time [s] to enter a code */
#define DT_DELAY 5.0     /* Delay [s] between emergency entry warnings */
#define DT_CHIME 0.5     /* Interval [s] between individual chime tones */
#define DT_OPEN 5.0      /* Time [s] for how long door is open after successful unlocking */

int phase; /* 0: FDAS off
	      1: Waiting for first key pressed and keypressing in progress
	      2: Regular Keycode correct. Timer running for unlock by Pilot
	      3: Emergency Keycode correct. Timer running for automatic unlock: 2 Chimes
	      4: Emergency Keycode correct. Timer running for automatic unlock: Pause
	      5: Emergency Keycode correct. Timer running for automatic unlock: 2 Chimes
	      6: Emergency Keycode correct. Timer running for automatic unlock: Pause
	      7: Emergency Keycode correct. Timer running for automatic unlock: Steady Chime
	      8: Emergency Keycode correct. Timer running for automatic unlock: Door Open
	      9: Wrong code entered. Block keypad for X seconds
	   */

struct timeval time_open;
struct timeval time_block;
struct timeval time_timeout;
struct timeval time_chime;
struct timeval time_ann;

int code;
int step;
int step2;

int te = 3; /* MCP23017 is connected to Aft Overhead Teensy */
int dev = 8;

int one = 1;
int zero = 0;

void init_b737_fdas(void)
{

  int pin;
 
  for (pin=0;pin<8;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_INPUT;
  }
  for (pin=8;pin<MCP23017_MAX_PINS;pin++) {
    mcp23017[te][dev].pinmode[pin] = PINMODE_OUTPUT;
  }
  mcp23017[te][dev].intpin = 23; // Interrupt pin on Teensy
  mcp23017[te][dev].wire = 0;  // I2C Bus: 0, 1 or 2
  mcp23017[te][dev].address = 0x20; // I2C address of MCP23017 device

  /* Audio Module DSUB 9 Pin Connector 1 */
  /* 1: GND
     2: 5V
     3: I2C SDA
     4: I2C SCL
     5: -
     6: I/O 8
     7: I/O 10
     8: I/O 12
     9: I/O 14
  */

  phase = 1;
  code = INT_MISS;
  step = 0;
  
}

/* Two chime tones with interval */
void chime_interval(struct timeval time_now) {
  
  float dt;
  int ret;
  
  if (step == 0) {
    gettimeofday(&time_chime,NULL);
    step = 1;
    ret = digital_output(te, MCP23017_TYPE, dev, 15, &one);
  } else if (step >= 1) {
    dt = ((float) (time_now.tv_sec - time_chime.tv_sec) +
	  ((float) (time_now.tv_usec - time_chime.tv_usec)) / 1000000.0);
    
    if (dt > DT_CHIME) {
      gettimeofday(&time_chime,NULL);
      step++;
      if ((step % 2) == 0) {
	ret = digital_output(te, MCP23017_TYPE, dev, 15, &zero);
      } else {
	ret = digital_output(te, MCP23017_TYPE, dev, 15, &one);
      }
    }
  }
}

/* Steady chime tone */
void chime_steady(struct timeval time_now) {
  
  float dt;
  int ret;
  
  if (step == 0) {
    gettimeofday(&time_chime,NULL);
    step = 1;
    ret = digital_output(te, MCP23017_TYPE, dev, 15, &one);
  } else if (step >= 1) {
    dt = ((float) (time_now.tv_sec - time_chime.tv_sec) +
	  ((float) (time_now.tv_usec - time_chime.tv_usec)) / 1000000.0);
    
    if (dt > DT_DELAY) {
      step++;
      ret = digital_output(te, MCP23017_TYPE, dev, 15, &zero);
    }
  }
}

/* Delay between chime warning sequences */
void chime_delay(struct timeval time_now) {
  
  float dt;
  
  if (step == 0) {
    gettimeofday(&time_chime,NULL);
    step = 1;
  } else if (step >= 1) {
    dt = ((float) (time_now.tv_sec - time_chime.tv_sec) +
	  ((float) (time_now.tv_usec - time_chime.tv_usec)) / 1000000.0);
    
    if (dt > DT_DELAY) {
      step++;
    }
  }
}

/* Door open delay */
void door_delay(struct timeval time_now) {
  
  float dt;
  
  if (step == 0) {
    gettimeofday(&time_chime,NULL);
    step = 1;
  } else if (step >= 1) {
    dt = ((float) (time_now.tv_sec - time_chime.tv_sec) +
	  ((float) (time_now.tv_usec - time_chime.tv_usec)) / 1000000.0);
    
    if (dt > DT_OPEN) {
      step++;
    }
  }
}

/* Delay after entering a wrong keycode */
void block_delay(struct timeval time_now) {
  
  float dt;
  
  if (step == 0) {
    gettimeofday(&time_chime,NULL);
    step = 1;
  } else if (step >= 1) {
    dt = ((float) (time_now.tv_sec - time_chime.tv_sec) +
	  ((float) (time_now.tv_usec - time_chime.tv_usec)) / 1000000.0);
    
    if (dt > DT_BLOCK) {
      step++;
    }
  }
}

void b737_fdas(void)
{
  
  //int te = 0;
  
  int ret;
  int pin;
  int ival;
  int i;
  float dt;

  struct timeval time_now;

  /* Flight Deck Door Knob and Annunciators */
  int *lock_fail = link_dataref_int("xpserver/lock_fail");
  int *auto_unlk = link_dataref_int("xpserver/auto_unlk");
  int *door_unlock = link_dataref_int("xpserver/door_unlock");
  int *door_auto = link_dataref_int("xpserver/door_auto");
  int *door_deny = link_dataref_int("xpserver/door_deny");

  int *avionics_on = link_dataref_int("sim/cockpit2/switches/avionics_power_on");

  *avionics_on = 1;
  
  float *lights_test;
  if ((acf_type == 2) || (acf_type == 3)) {
    lights_test = link_dataref_flt("laminar/B738/toggle_switch/bright_test",0);
  } else {
    lights_test = link_dataref_flt("xpserver/lights_test",0);
  }

  /* Main FDAS Switch (NORM / OFF) */
  /* drives LOCK FAIL Annunciator on Center Pedestal */
  ret = digital_input(te, MCP23017_TYPE, dev, 7, &ival, 0);
  if (ret == 1) {
    if (ival == 0) {
      phase = 1;
      printf("FDAS Power Switch ON\n");
      *lock_fail = 0;
    } else {
      printf("FDAS Power Switch OFF\n");
      phase = 0;
      *lock_fail = 1;
    }
  }

  phase = 1;
  
  if ((phase >= 1) && (*avionics_on == 1)) {
    /* FDAS turned on by main switch */

    gettimeofday(&time_now,NULL);

    if (phase == 1) {
    
      if (code != INT_MISS) {
	/* time passed since keypressing started [s] */
	dt = ((float) (time_now.tv_sec - time_timeout.tv_sec) +
	      ((float) (time_now.tv_usec - time_timeout.tv_usec)) / 1000000.0);

	/* reset code if try takes too long */
	if (dt > DT_TIMEOUT) {
	  printf("Keypress Timeout\n");
	  code = INT_MISS;
	}
      
      }
 
      // Inputs 0-4: Keys 1-5
      for (i=0;i<5;i++) {
	ret = digital_input(te, MCP23017_TYPE, dev, i, &ival, 0);
	if (ret == 1) {
	  if (ival == 1) {
	    printf("FDAS Key %i pressed\n",i+1);
	    if (code == INT_MISS) {
	      gettimeofday(&time_timeout,NULL);
	      code = i+1;
	      printf("CODE: %i\n",code);
	    } else {
	      if (code > 10000000) {
		code = INT_MISS;
		printf("Digit overflow, resetting code\n");
	      } else {
		code = code * 10 + i+1;
		printf("CODE: %i\n",code);
	      }
	    }
	  }
	}
      }

      // Input 5: ENT Key
      ret = digital_input(te, MCP23017_TYPE, dev, 5, &ival, 0);
      if (ret == 1) {
	if (ival == 1) {
	  printf("FDAS ENT Key pressed \n");
	  if (code == STD_CODE) {
	    printf("FDAS STANDARD CODE ENTERED!\n");
	    code = INT_MISS;
	    phase = 2;
	  } else if (code == EMER_CODE) {
	    printf("FDAS EMERGENCY CODE ENTERED!\n");
	    code = INT_MISS;
	    phase = 3;
	  } else {
	    printf("FDAS WRONG CODE ENTERED! BLOCK KEYPAD FOR X SECONDS \n");
	    code = INT_MISS;
	    phase = 9;
	  }
	}
      }

      /* Outputs */
      ret = digital_output(te, MCP23017_TYPE, dev, 8, &zero); // green led
      ret = digital_output(te, MCP23017_TYPE, dev, 9, &zero); // yellow led
      ret = digital_output(te, MCP23017_TYPE, dev, 10, &one); // red led
      ret = digital_output(te, MCP23017_TYPE, dev, 14, &one); // Solenoid Relais
      ret = digital_output(te, MCP23017_TYPE, dev, 15, &zero); // Tone generator

    } else if (phase == 2) {
      /* Correct Normal Entry Code: 2 chimes */
      
      chime_interval(time_now);
      
      /* End interval chime, step back to key press mode */
      if (step >= 4) {
	phase = 1;
	step = 0;
      }
								  
    } else if (phase == 3) {
      /* Correct Emergency Entry Code: 2 chimes */

      ret = digital_output(te, MCP23017_TYPE, dev, 8, &zero); // green led
      ret = digital_output(te, MCP23017_TYPE, dev, 9, &one); // yellow led
      ret = digital_output(te, MCP23017_TYPE, dev, 10, &zero); // red led
      *auto_unlk = 1;
 
      chime_interval(time_now);
      
      /* End interval chime, proceed with delay */
      if (step >= 4) {
	phase = 4;
	step = 0;
      }
								  
    } else if (phase == 4) {
      /* Delay between chime sequence */

      chime_delay(time_now);
      
      /* End delay, proceed with interval chime */
      if (step >= 2) {
	phase = 5;
	step = 0;
      }
								  
    } else if (phase == 5) {
      /* 2 Chimes */

      chime_interval(time_now);
      
      /* End interval chime, proceed with delay */
      if (step >= 4) {
	phase = 6;
	step = 0;
      }
								  
    } else if (phase == 6) {
      /* Delay between chime sequence */

      chime_delay(time_now);
      
      /* End delay, proceed with steady chime */
      if (step >= 2) {
	phase = 7;
	step = 0;
      }
								  
    } else if (phase == 7) {
      /* Steady chime and AUTO UNLK flashing */
 
      if (step == 0) {
	gettimeofday(&time_ann,NULL);
	step2 = 1;
	*auto_unlk = 0;
      }
      if (step2 >= 1) {
	dt = ((float) (time_now.tv_sec - time_ann.tv_sec) +
	      ((float) (time_now.tv_usec - time_ann.tv_usec)) / 1000000.0);
    
	if (dt > DT_CHIME) {
	  gettimeofday(&time_ann,NULL);
	  step2++;
	  if ((step2 % 2) == 0) {
	    *auto_unlk = 1;
	  } else {
	    *auto_unlk = 0;
	  }
	}
      }

      chime_steady(time_now);
      
      /* End steady chime, proceed with door open */
      if (step >= 2) {
	phase = 8;
	step = 0;
	*auto_unlk = 0;
      }
								  
    } else if (phase == 8) {
      /* Door open sequence */

      ret = digital_output(te, MCP23017_TYPE, dev, 8, &one); // green led
      ret = digital_output(te, MCP23017_TYPE, dev, 9, &zero); // yellow led
      ret = digital_output(te, MCP23017_TYPE, dev, 10, &zero); // red led
      ret = digital_output(te, MCP23017_TYPE, dev, 14, &zero); // Solenoid Relais

      door_delay(time_now);
      
      /* End door open, proceed with regular key entry */
      if (step >= 2) {
	phase = 1;
	step = 0;
      }

    } else if (phase == 9) {
      /* Block keypad entry after Deny or wrong code */

      /* Outputs */
      ret = digital_output(te, MCP23017_TYPE, dev, 8, &zero); // green led
      ret = digital_output(te, MCP23017_TYPE, dev, 9, &zero); // yellow led
      ret = digital_output(te, MCP23017_TYPE, dev, 10, &one); // red led
      ret = digital_output(te, MCP23017_TYPE, dev, 14, &one); // Solenoid Relais
      ret = digital_output(te, MCP23017_TYPE, dev, 15, &zero); // Tone generator

      block_delay(time_now);

      /* End keypad blocking, proceed with regular key entry */
      if (step >= 2) {
	phase = 1;
	step = 0;
	printf("FDAS KEYPAD READY AGAIN \n");
      }
      
    }
    
    /* The pilot can unlock the door any time */
    if (*door_unlock == 1) {
      ret = digital_output(te, MCP23017_TYPE, dev, 14, &zero); // Solenoid Relais
      ret = digital_output(te, MCP23017_TYPE, dev, 8, &one); // green led
      ret = digital_output(te, MCP23017_TYPE, dev, 9, &zero); // yellow led
      ret = digital_output(te, MCP23017_TYPE, dev, 10, &zero); // red led
    }
    
    /* The pilot can stop the auto unlock procedure any time */
    /* Will block keypad for X seconds */
    if (*door_deny == 1) {
      phase = 9;
      step = 0;
      *auto_unlk = 0;
    }
   
  } else {
    /* FDAS turned off by main switch or without avionics */
    ret = digital_output(te, MCP23017_TYPE, dev, 8, &zero); // green led
    ret = digital_output(te, MCP23017_TYPE, dev, 9, &zero); // yellow led
    ret = digital_output(te, MCP23017_TYPE, dev, 10, &zero); // red led
    
    ret = digital_output(te, MCP23017_TYPE, dev, 14, &zero); // Solenoid Relais
    ret = digital_output(te, MCP23017_TYPE, dev, 15, &zero); // Wave Generator
  }
  
}

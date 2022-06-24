/* This is the b737_awm.c code which drives the original B737 Aural Warning Module
   by use of a solid state relay (SSR) to switch 24V signals to trigger the individual
   warning types

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
#include "b737_awm.h"

/* #define CONT_HORN_PIN 0 */
/* #define INT_HORN_PIN 1 */
/* #define AP_DISC_PIN 2 */
/* #define HI_CHIME_PIN 3 */
/* #define HILOW_CHIME_PIN 4 */
/* #define LOW_CHIME_PIN 5 */
/* #define CLACKER_PIN 12 */
/* #define FIRE_BELL_PIN 13 */

#define CONT_HORN_PIN 17
#define INT_HORN_PIN 18
#define AP_DISC_PIN 27
#define HI_CHIME_PIN 22
#define HILOW_CHIME_PIN 23
#define LOW_CHIME_PIN 24
#define CLACKER_PIN 10
#define FIRE_BELL_PIN 9

int belts_save;
struct timeval attend_time1;

int b737_awm_init(void) {
 
#ifdef PIGPIO
  gpioSetMode(CONT_HORN_PIN, PI_OUTPUT);
  gpioSetMode(INT_HORN_PIN, PI_OUTPUT);
  gpioSetMode(AP_DISC_PIN, PI_OUTPUT);
  gpioSetMode(HI_CHIME_PIN, PI_OUTPUT);
  gpioSetMode(HILOW_CHIME_PIN, PI_OUTPUT);
  gpioSetMode(LOW_CHIME_PIN, PI_OUTPUT);
  gpioSetMode(CLACKER_PIN, PI_OUTPUT);
  gpioSetMode(FIRE_BELL_PIN, PI_OUTPUT);
#else
  pinMode(CONT_HORN_PIN, OUTPUT);
  pinMode(INT_HORN_PIN, OUTPUT);
  pinMode(AP_DISC_PIN, OUTPUT);
  pinMode(HI_CHIME_PIN, OUTPUT);
  pinMode(HILOW_CHIME_PIN, OUTPUT);
  pinMode(LOW_CHIME_PIN, OUTPUT);
  pinMode(CLACKER_PIN, OUTPUT);
  pinMode(FIRE_BELL_PIN, OUTPUT);
#endif
  
  belts_save = 0;
  
  return 0;
  
}

void b737_awm(void)
{

  int ret;
  struct timeval attend_time2;
  double dtime;

  if (acf_type == 3) {

    /* testing */
    //float *config_warn = link_dataref_flt("laminar/B738/toggle_switch/bright_test",0);
 
    /* link integer data like a switch in the cockpit */
    int *ap_disconnect = link_dataref_int("laminar/b738/fmodpack/fmod_ap_disconnect");
    int *belts = link_dataref_int("laminar/b738/fmodpack/play_seatbelt_no_smoke");
    float *attend = link_dataref_flt("laminar/B738/push_button/attend_pos",0);
    float *mach_warn = link_dataref_flt("laminar/B738/fmod/mach_warn",0);
    int *fire_bell = link_dataref_int("laminar/b738/annunciator/fire_bell_anun");
    float *config_warn = link_dataref_flt("laminar/B738/system/takeoff_config_warn",0);
    float *gear_warn = link_dataref_flt("laminar/b738/fmodpack/msg_too_low_gear",0);
    
    if (*ap_disconnect != INT_MISS) {
#ifdef PIGPIO
      gpioWrite(AP_DISC_PIN, *ap_disconnect);
#else
      digitalWrite(AP_DISC_PIN, *ap_disconnect);
#endif
    } else {
#ifdef PIGPIO
      gpioWrite(AP_DISC_PIN, 0);
#else
      digitalWrite(AP_DISC_PIN, 0);
#endif
    }
    
    if ((*belts != INT_MISS) && (*belts != belts_save)) {
#ifdef PIGPIO
      gpioWrite(HI_CHIME_PIN, 1);
#else
      digitalWrite(HI_CHIME_PIN, 1);
#endif
      belts_save = *belts;
    } else {
#ifdef PIGPIO
      gpioWrite(HI_CHIME_PIN, 0);
#else
      digitalWrite(HI_CHIME_PIN, 0);
#endif
    }
    
    if (*attend != FLT_MISS) {
      gettimeofday(&attend_time2,NULL);
      if ((int) *attend == 1) {
#ifdef PIGPIO
	gpioWrite(HILOW_CHIME_PIN, 1);
#else
	digitalWrite(HILOW_CHIME_PIN, 1);
#endif
	gettimeofday(&attend_time1,NULL);
      } else {
	dtime = ((attend_time2.tv_sec - attend_time1.tv_sec) +
		   (attend_time2.tv_usec - attend_time1.tv_usec) / 1000000.0);
	if (dtime > 1.0) {
#ifdef PIGPIO
	  gpioWrite(HILOW_CHIME_PIN, 0);
#else
	  digitalWrite(HILOW_CHIME_PIN, 0);
#endif
	}
      } 
    } else {
#ifdef PIGPIO
      gpioWrite(HILOW_CHIME_PIN, 0);
#else
      digitalWrite(HILOW_CHIME_PIN, 0);
#endif
    }

    
    if (*mach_warn != FLT_MISS) {
#ifdef PIGPIO
      gpioWrite(CLACKER_PIN, (int) *mach_warn);
#else
      digitalWrite(CLACKER_PIN, (int) *mach_warn);
#endif
    } else {
#ifdef PIGPIO
      gpioWrite(CLACKER_PIN, 0);
#else
      digitalWrite(CLACKER_PIN, 0);
#endif
    }
    if (*fire_bell != INT_MISS) {
#ifdef PIGPIO
      gpioWrite(FIRE_BELL_PIN, *fire_bell);
#else
      digitalWrite(FIRE_BELL_PIN, *fire_bell);
#endif
    } else {
#ifdef PIGPIO
      gpioWrite(FIRE_BELL_PIN, 0);
#else
      digitalWrite(FIRE_BELL_PIN, 0);
#endif
    }
    if (*config_warn != FLT_MISS) {
#ifdef PIGPIO
      gpioWrite(INT_HORN_PIN, (int) *config_warn);
#else
      digitalWrite(INT_HORN_PIN, (int) *config_warn);
#endif
    } else {
#ifdef PIGPIO
      gpioWrite(INT_HORN_PIN, 0);
#else
      digitalWrite(INT_HORN_PIN, 0);
#endif
    }
    if (*gear_warn != FLT_MISS) {
#ifdef PIGPIO
      gpioWrite(CONT_HORN_PIN, (int) *gear_warn);
#else
      digitalWrite(CONT_HORN_PIN, (int) *gear_warn);
#endif
    } else {
#ifdef PIGPIO
      gpioWrite(CONT_HORN_PIN, 0);
#else
      digitalWrite(CONT_HORN_PIN, 0);
#endif
    }
    
  }
  
}

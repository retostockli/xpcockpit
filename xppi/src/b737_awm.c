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
 
  pinMode(CONT_HORN_PIN, OUTPUT);
  pinMode(INT_HORN_PIN, OUTPUT);
  pinMode(AP_DISC_PIN, OUTPUT);
  pinMode(HI_CHIME_PIN, OUTPUT);
  pinMode(HILOW_CHIME_PIN, OUTPUT);
  pinMode(LOW_CHIME_PIN, OUTPUT);
  pinMode(CLACKER_PIN, OUTPUT);
  pinMode(FIRE_BELL_PIN, OUTPUT);

  belts_save = 0;
  
  return 0;
  
}

void b737_awm(void)
{

  int ret;
  struct timeval attend_time2;
  double dtime;

  if (acf_type == 3) {
  
    /* link integer data like a switch in the cockpit */
    int *value = link_dataref_int("sim/cockpit/electrical/landing_lights_on");

    int *ap_disconnect = link_dataref_int("laminar/b738/fmodpack/fmod_ap_disconnect");
    int *belts = link_dataref_int("laminar/b738/fmodpack/play_seatbelt_no_smoke");
    float *attend = link_dataref_flt("laminar/B738/push_button/attend_pos",0);
    float *mach_warn = link_dataref_flt("laminar/B738/fmod/mach_warn",0);
    int *fire_bell = link_dataref_int("laminar/b738/fmodpack/fmod_play_firebells");
    float *config_warn = link_dataref_flt("laminar/B738/system/takeoff_config_warn",0);
    float *gear_warn = link_dataref_flt("laminar/b738/fmodpack/msg_too_low_gear",0);
    
    /* not needed, only if you run without x-plane connection */
    if (*value == INT_MISS) *value = 0;

    if (*value != INT_MISS) {
      /*
        digitalWrite(LOW_CHIME_PIN, *value);
      */
    }

    
    if (*ap_disconnect != INT_MISS) digitalWrite(AP_DISC_PIN, *ap_disconnect);
    
    if ((*belts != INT_MISS) && (*belts != belts_save)) {
      digitalWrite(HI_CHIME_PIN, 1);
      belts_save = *belts;
    } else {
      digitalWrite(HI_CHIME_PIN, 0);
    }
    
    if (*attend != FLT_MISS) {
      gettimeofday(&attend_time2,NULL);
      if ((int) *attend == 1) {
	digitalWrite(HILOW_CHIME_PIN, 1);
	gettimeofday(&attend_time1,NULL);
      } else {
	dtime = ((attend_time2.tv_sec - attend_time1.tv_sec) +
		   (attend_time2.tv_usec - attend_time1.tv_usec) / 1000000.0);
	if (dtime > 1.0) digitalWrite(HILOW_CHIME_PIN, 0);	  
      } 
    } else {
      digitalWrite(HILOW_CHIME_PIN, 0);
    }

    if (*mach_warn != FLT_MISS) digitalWrite(CLACKER_PIN, (int) *mach_warn);
    
    if (*fire_bell != INT_MISS) digitalWrite(FIRE_BELL_PIN, *fire_bell);

    if (*config_warn != FLT_MISS) digitalWrite(INT_HORN_PIN, (int) *config_warn);

    if (*gear_warn != FLT_MISS) digitalWrite(CONT_HORN_PIN, (int) *gear_warn);
    
  }
  
}

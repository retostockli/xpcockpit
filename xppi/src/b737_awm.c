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

int hi_chime_status_save;
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
  
  hi_chime_status_save = 0;
  
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
    float *ap_disconnect = link_dataref_flt("laminar/b738/fmodpack/fmod_ap_disconnect",0);
    //float *belts = link_dataref_flt("laminar/b738/fmodpack/play_seatbelt_no_smoke",0);
    float *belts = link_dataref_flt("laminar/b738/fmodpack/seatbelt_on_light",0);
    float *attend = link_dataref_flt("laminar/B738/push_button/attend_pos",0);
    float *mach_warn = link_dataref_flt("laminar/B738/fmod/mach_warn",0);
    float *fire_warn = link_dataref_flt("laminar/B738/annunciator/fire_bell_annun2",-1);
    float *config_warn = link_dataref_flt("laminar/B738/system/takeoff_config_warn",0);
    float *gear_warn = link_dataref_flt("laminar/b738/fmodpack/msg_too_low_gear",0);
    float *alt_warn = link_dataref_flt("laminar/b738/fmodpack/horn_alert",0);

    /* BUTTONS ON ACP1 to test the AWM */
    int *ap_disconnect_test = link_dataref_int("xpserver/acp1_micsel_vhf3");
    int *belts_test  = link_dataref_int("xpserver/acp1_micsel_hf1");
    int *attend_test  = link_dataref_int("xpserver/acp1_micsel_hf2");
    int *mach_warn_test  = link_dataref_int("xpserver/acp1_micsel_flt");
    int *config_warn_test  = link_dataref_int("xpserver/acp1_micsel_svc");
    int *gear_warn_test   = link_dataref_int("xpserver/acp1_micsel_pa");


    int ap_disc;
    if ((*ap_disconnect != FLT_MISS) && (*ap_disconnect_test != INT_MISS)) {
      ap_disc = ((int) *ap_disconnect) || (*ap_disconnect_test == 1);
    } else {
      ap_disc = 0;
    }
    
#ifdef PIGPIO
    gpioWrite(AP_DISC_PIN, ap_disc);
#else
    digitalWrite(AP_DISC_PIN, ap_disc);
#endif

    int hi_chime;
    if ((*belts != FLT_MISS) && (*belts_test != INT_MISS)) {
      int hi_chime_status = ((int) *belts) || (*belts_test == 1);
      if (hi_chime_status != hi_chime_status_save) {
	hi_chime = 1;
      } else {
	hi_chime = 0;
      }
      hi_chime_status_save = hi_chime_status;
    } else {
      hi_chime = 0;
    }
#ifdef PIGPIO
    gpioWrite(HI_CHIME_PIN, hi_chime);
#else
    digitalWrite(HI_CHIME_PIN, hi_chime);
#endif

    int hilow_chime;
    if ((*attend != FLT_MISS) && (*attend_test != INT_MISS)) {
      if (((int) *attend == 1) || (*attend_test == 1)) {
	gettimeofday(&attend_time1,NULL);
      }
      gettimeofday(&attend_time2,NULL);
      dtime = ((attend_time2.tv_sec - attend_time1.tv_sec) +
	       (attend_time2.tv_usec - attend_time1.tv_usec) / 1000000.0);
      if (dtime < 1.0) {
	hilow_chime = 1;
      } else {
	hilow_chime = 0;
      }
    } else {
      hilow_chime = 0;
    }
          
#ifdef PIGPIO
    gpioWrite(HILOW_CHIME_PIN, hilow_chime);
#else
    digitalWrite(HILOW_CHIME_PIN, hilow_chime);
#endif

    int clacker;
    if ((*mach_warn != FLT_MISS) && (*mach_warn_test != INT_MISS)) {
      clacker = ((int) *mach_warn) || (*mach_warn_test == 1);
    } else {
      clacker = 0;
    }
#ifdef PIGPIO
    gpioWrite(CLACKER_PIN, clacker);
#else
    digitalWrite(CLACKER_PIN, clacker);
#endif

    int fire_bell;
    if (*fire_warn != FLT_MISS) {
      fire_bell = (int) (*fire_warn > 0.5);
    } else {
      fire_bell = 0;
    }
      
#ifdef PIGPIO
    gpioWrite(FIRE_BELL_PIN, fire_bell);
#else
    digitalWrite(FIRE_BELL_PIN, fire_bell);
#endif
  
    /* CONFIG and ALT Warn have the same sound */
    int int_horn;
    if ((*config_warn != FLT_MISS) && (*alt_warn != FLT_MISS) && (*config_warn_test != INT_MISS)) {
      int_horn = (int) *config_warn || (int) *alt_warn || (*config_warn_test == 1);
    } else {
      int_horn = 0;
    }
#ifdef PIGPIO
    gpioWrite(INT_HORN_PIN, int_horn);
#else
    digitalWrite(INT_HORN_PIN, int_horn);
#endif

    int cont_horn;
    if ((*gear_warn != FLT_MISS) && (*gear_warn_test != INT_MISS)) {
      cont_horn = (int) *gear_warn || (*gear_warn_test == 1);
    } else {
      cont_horn = 0;
    }
      
#ifdef PIGPIO
    gpioWrite(CONT_HORN_PIN, cont_horn);
#else
    digitalWrite(CONT_HORN_PIN, cont_horn);
#endif
    
  }
  
}

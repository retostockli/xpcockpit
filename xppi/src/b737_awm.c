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

int b737_awm_init(void) {
 
  pinMode(CONT_HORN_PIN, OUTPUT);
  pinMode(INT_HORN_PIN, OUTPUT);
  pinMode(AP_DISC_PIN, OUTPUT);
  pinMode(HI_CHIME_PIN, OUTPUT);
  pinMode(HILOW_CHIME_PIN, OUTPUT);
  pinMode(LOW_CHIME_PIN, OUTPUT);
  pinMode(CLACKER_PIN, OUTPUT);
  pinMode(FIRE_BELL_PIN, OUTPUT);

  return 0;
  
}

void b737_awm(void)
{

  int ret;
  
  /* link integer data like a switch in the cockpit */
  int *value = link_dataref_int("sim/cockpit/electrical/landing_lights_on");
  
 
  /* not needed, only if you run without x-plane connection */
  if (*value == INT_MISS) *value = 0;

  if (*value != INT_MISS) {
        digitalWrite(CONT_HORN_PIN, *value);
        digitalWrite(INT_HORN_PIN, *value);
        digitalWrite(AP_DISC_PIN, *value);
        digitalWrite(HI_CHIME_PIN, *value);
        digitalWrite(HILOW_CHIME_PIN, *value);
        digitalWrite(LOW_CHIME_PIN, *value);
        digitalWrite(CLACKER_PIN, *value);
        digitalWrite(FIRE_BELL_PIN, *value);
  }

}

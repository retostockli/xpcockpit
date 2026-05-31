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

/* to be preserved variables */


void init_b737_overheadaft(void)
{
  int te = 3; // Teensy Number (as in ini file)
  int pin;
  int dev;

  /* ----------- */
  /* TEENSY HOST */
  /* ----------- */
 
 
}

void b737_overheadaft(void)
{

  int te = 3; // Teensy Number (as in ini file)

  int ret;
  int dev;

  int one = 1;
  int zero = 0;

  int ival;
  float fval;

  int *avionics_on = link_dataref_int("sim/cockpit2/switches/avionics_power_on");
 
  /* only run for Laminar 737 or ZIBO 737 */
  if ((acf_type == 2) || (acf_type == 3)) {
    
    float *lights_test = link_dataref_flt("laminar/B738/annunciator/test",-1);

  }
    
}

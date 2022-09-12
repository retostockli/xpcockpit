/* This is the common.h header to the Rasperry Pi GPIO Interface defining common variables
   and functions in common.c

   Copyright (C) 2020  Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. 
*/

#include "check_aircraft.h"

#ifdef PIGPIO
#include <pigpio.h>
#else
#include <wiringPi.h>
#include <softPwm.h>
#endif

#define INTERVAL 50
//#define INTERVAL 1000

/* verbosity of xpsismo (0-4) */
extern int verbose;

extern int is_copilot; /* 0: Captain, 1: Copilot */

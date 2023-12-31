/* This is the common.h header to the usbiocards code defining common variables
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

#define INTERVAL 10 /* Polling interval (seconds) for both xpserver plugin and sismo cards */
/* State check interval (ms) for Up/Down commands
   Needs feedback time from flight simulator states */
#define INTERVAL_UPDN 100

/* verbosity of xpsismo (0-4) */
extern int verbose;

/* time step counter */
extern int interval_counter;

/* prototype functions */
int set_state_updnf(float *new_statef, float *old_statef, int *up, int *dn);
int set_state_updn(int *new_state, int *old_state, int *up, int *dn);
int set_state_togglef(float *new_statef, float *old_statef, int *toggle);
int set_state_toggle(int *new_state, int *old_state, int *toggle);
int set_switch_cover(float *switch_cover_pos, int *switch_cover_toggle, int on);

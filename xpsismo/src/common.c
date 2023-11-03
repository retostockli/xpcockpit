/* This is the common.c to the usbiocards code defining common functions

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

#include <math.h>
#include "serverdata.h"
#include "common.h"

/* allocation of global variables from common.h */
int verbose;

int set_state_updnf(float *new_statef, float *old_statef, int *up, int *dn)
{
  int new_state = INT_MISS;
  if (*new_statef != FLT_MISS) new_state = lroundf(*new_statef);
  int old_state = INT_MISS;
  if (*old_statef != FLT_MISS) old_state = lroundf(*old_statef);
  return set_state_updn(&new_state, &old_state, up,dn);
}

int set_state_updn(int *new_state, int *old_state, int *up, int *dn)
{

  int ret = 0;
  
  if ((new_state) && (old_state) && (up) && (dn)) {
    if ((*new_state != INT_MISS) && (*old_state != INT_MISS)) {

      if ((*up == 1) || (*dn == 1)) {
	/* we just upped or downed, so wait for a cycle */
      } else {
	if (*new_state > *old_state) {
	  *up = 1;
	  *dn = 0;
	  ret = 1;
	} else if (*new_state < *old_state) {
	  *up = 0;
	  *dn = 1;
	  ret =-1;
	} else {
	  /* nothing to do */
	}
      }

    }
  }

  return ret;
  
}

int set_state_togglef(float *new_statef, float *old_statef, int *toggle)
{
  int new_state = INT_MISS;
  if (*new_statef != FLT_MISS) new_state = lroundf(*new_statef);
  int old_state = INT_MISS;
  if (*old_statef != FLT_MISS) old_state = lroundf(*old_statef);
  return set_state_toggle(&new_state, &old_state, toggle);
}
 
int set_state_toggle(int *new_state, int *old_state, int *toggle)
{

  int ret = 0;
  
  if ((new_state) && (old_state) && (toggle)) {
    if ((*new_state != INT_MISS) && (*old_state != INT_MISS)) {

      if (*toggle == 1) {
	/* we just upped or downed, so wait for a cycle */
      } else {
	if (*new_state > *old_state) {
	  *toggle = 1;
	  ret = 1;
	} else if (*new_state < *old_state) {
	  *toggle = 1;
	  ret = 1;
	} else {
	  /* nothing to do */
	}
      }

    }
  }

  return ret;
  
}

int set_switch_cover(float *switch_cover_pos, int *switch_cover_toggle, int on)
{
  /* open or close switch covers in ZIBO mod. They are animated, so
     assume it is open as soon as the floating point value is > 0 or
     assume it is closed as soon as the floating point value is < 1 */
  int value;
  if (*switch_cover_pos != FLT_MISS) {
    if (on == 1) {
      value = *switch_cover_pos > 0.0;
    } else {
      value = *switch_cover_pos == 1.0;
    }
  } else {
    value = INT_MISS;
  }

  return set_state_toggle(&on,&value,switch_cover_toggle);

}

/* This is the b737_overhead_fwd.c code which contains a sample set-up for how to communicate with the 
   BOEING 737 Forward Overhead panel (switches, LED's, displays) connected 
   to the OpenCockpits IOCARDS USB device.

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "common.h"
#include "libiocards.h"
#include "serverdata.h"
#include "b737_overhead_fwd.h"

/* permanent storage */
int dc_power_pos;
int ac_power_pos;

int set_state_updn(int *new_state, int *old_state, int *up, int *dn)
{

  int ret = 0;
  
  if ((new_state) && (old_state) && (up) && (dn)) {
    if ((*new_state != INT_MISS) && (*old_state != INT_MISS)) {

      if ((*up == 1) || (*up == 2) || (*dn == 1) || (*dn == 2)) {
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

void b737_overhead_fwd(void)
{
  int ret;
  int ival;
  float fval;
  int device = 6;
  int card = 0;

  if ((acf_type == 2) || (acf_type == 3)) {

    device = 6;
    card = 0;
  
  
    /* Power & Battery Panel */
    /* --------------------- */

    /* Battery Switch: dn is moving switch to 1 and up to 0 */
    int *battery_up = link_dataref_cmd_once("laminar/B738/push_button/batt_full_off"); // includes guard
    //int *battery_up = link_dataref_cmd_once("laminar/B738/switch/battery_up");
    int *battery_dn = link_dataref_cmd_once("laminar/B738/switch/battery_dn");
    int *battery = link_dataref_int("laminar/B738/electric/battery_pos");
    ret = digital_input(device,card,8,&ival,0);
    if (ival != INT_MISS) ival = 1 - ival;
    ret = set_state_updn(&ival,battery,battery_dn,battery_up);
    if (ret != 0) {
      printf("battery %i %i %i %i %i\n",ret,ival,*battery,*battery_dn,*battery_up);
    }

    /* DC Power Knob */
    int *dc_power_up = link_dataref_cmd_once("laminar/B738/knob/dc_power_up");
    int *dc_power_dn = link_dataref_cmd_once("laminar/B738/knob/dc_power_dn");
    int *dc_power = link_dataref_int("laminar/B738/knob/dc_power");
    
    ret = digital_input(device,card,0,&ival,0);
    if (ival == 1) dc_power_pos = 0; // STBY PWR
    ret = digital_input(device,card,1,&ival,0);
    if (ival == 1) dc_power_pos = 1; // BAT BUS
    ret = digital_input(device,card,2,&ival,0);
    if (ival == 1) dc_power_pos = 2; // BAT
    ret = digital_input(device,card,3,&ival,0);
    if (ival == 1) dc_power_pos = 2; // AUX BAT (does not exist in ZIBO)
    ret = digital_input(device,card,4,&ival,0);
    if (ival == 1) dc_power_pos = 3; // TR1
    ret = digital_input(device,card,5,&ival,0);
    if (ival == 1) dc_power_pos = 4; // TR2
    ret = digital_input(device,card,6,&ival,0);
    if (ival == 1) dc_power_pos = 5; // TR3
    ret = digital_input(device,card,7,&ival,0);
    if (ival == 1) dc_power_pos = 6; // TEST

    ret = set_state_updn(&dc_power_pos,dc_power,dc_power_up,dc_power_dn);
    if (ret != 0) {
      printf("DC Power knob %i \n",dc_power_pos);
    }
    
    /* AC Power Knob */
    int *ac_power_up = link_dataref_cmd_once("laminar/B738/knob/ac_power_up");
    int *ac_power_dn = link_dataref_cmd_once("laminar/B738/knob/ac_power_dn");
    int *ac_power = link_dataref_int("laminar/B738/knob/ac_power");
    
    ret = digital_input(device,card,9,&ival,0);
    if (ival == 1) ac_power_pos = 0; // STBY PWR
    ret = digital_input(device,card,10,&ival,0);
    if (ival == 1) ac_power_pos = 1; // GRD PWR
    ret = digital_input(device,card,11,&ival,0);
    if (ival == 1) ac_power_pos = 2; // GEN1
    ret = digital_input(device,card,12,&ival,0);
    if (ival == 1) ac_power_pos = 3; // APU GEN
    ret = digital_input(device,card,13,&ival,0);
    if (ival == 1) ac_power_pos = 4; // GEN2
    ret = digital_input(device,card,14,&ival,0);
    if (ival == 1) ac_power_pos = 5; // INV
    ret = digital_input(device,card,15,&ival,0);
    if (ival == 1) ac_power_pos = 6; // TEST

    ret = set_state_updn(&ac_power_pos,ac_power,ac_power_up,ac_power_dn);
    if (ret != 0) {
      printf("AC Power knob %i \n",ac_power_pos);
    }

    /* Galley PWR */
    ret = digital_input(device,card,16,&ival,0);
    if (ret == 1) {
      printf("Galley PWR %i \n",ival);
    }

    /* MAINT Pushbutton */
    int *maint = link_dataref_cmd_once("laminar/B738/push_button/acdc_maint");
    ret = digital_input(device,card,17,maint,0);
    
    /* set relay 0 on output 49 on relay card through d-sub con */
    ret = digital_output(device,card,49,battery);

    /* Annunciators */
    int *bat_discharge = link_dataref_int("laminar/B738/annunciator/bat_discharge");
    ret = digital_output(device,card,11,bat_discharge);
    int *tr_unit = link_dataref_int("laminar/B738/annunciator/tr_unit");
    ret = digital_output(device,card,12,tr_unit);
    int *elec = link_dataref_int("laminar/B738/annunciator/elec");
    ret = digital_output(device,card,13,elec);

    /* AC/DC 7 Segment Displays
       Lower Row: DC Volts 6,7, AC Amps 4,5, AC Volts 1,2,3
       Upper Row: DC Amps 11,12, CPS FREQ 8,9,10
       Todo: show or not show values depending on AC/DC Knob positions
    */
    device = 6;
    card = 1;

    int *dc_volt = link_dataref_int("laminar/B738/dc_volt_value");
    ret = mastercard_display(device,card,6,2,dc_volt,0);
    int *dc_amps = link_dataref_int("laminar/B738/dc_amp_value");
    ret = mastercard_display(device,card,11,2,dc_amps,0);
    int *ac_volt = link_dataref_int("laminar/B738/ac_volt_value");
    ret = mastercard_display(device,card,1,3,ac_volt,0);
    int *ac_amps = link_dataref_int("laminar/B738/ac_amp_value");
    ret = mastercard_display(device,card,4,2,ac_amps,0);
    int *ac_freq = link_dataref_int("laminar/B738/ac_freq_value");
    ret = mastercard_display(device,card,8,3,ac_freq,0);
    
    /* Engine Start Panel */
    /* ------------------ */
    device = 6;
    card = 0;

    /* set relay 1 on output 50 on relay card through d-sub con */
    ret = digital_output(device,card,50,battery);
    /* set relay 2 on output 50 on relay card through d-sub con */
    ret = digital_output(device,card,51,battery);


  }
    
}

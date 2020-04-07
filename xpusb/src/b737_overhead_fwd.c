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
int l_eng_start_pos;
int r_eng_start_pos;
int air_valve_ctrl_pos;
int l_wiper_pos;
int r_wiper_pos;

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

int set_state_toggle(int *new_state, int *old_state, int *toggle)
{

  int ret = 0;
  
  if ((new_state) && (old_state) && (toggle)) {
    if ((*new_state != INT_MISS) && (*old_state != INT_MISS)) {

      if ((*toggle == 1) || (*toggle == 2)) {
	/* we just upped or downed, so wait for a cycle */
      } else {
	if (*new_state > *old_state) {
	  *toggle = 1;
	  ret = 1;
	} else if (*new_state < *old_state) {
	  *toggle = 1;
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
  int ival2;
  float fval;
  int device = 6;
  int card = 0;
  char buffer[100];

  if ((acf_type == 2) || (acf_type == 3)) {
    
    int *lights_test = link_dataref_int("laminar/B738/annunciator/test");

    /* Open SWITCH COVERS (Guards) in the airplane so that we can freely move HW switches */
    int ncover = 11;
    float *switch_cover = link_dataref_flt_arr("laminar/B738/button_switch/cover_position",ncover,-1,-3);
    int *switch_cover_toggle;
    for (int i=0;i<ncover;i++) {
      sprintf(buffer, "laminar/B738/button_switch_cover%02d", i);
      switch_cover_toggle = link_dataref_cmd_once(buffer);
      ival = 1;
      if (switch_cover[i] != FLT_MISS) {
	if (ival == 1) {
	  ival2 = switch_cover[i] > 0.0;
	} else {
	  ival2 = switch_cover[i] == 1.0;
	}
      } else {
	ival2 = INT_MISS;
      }
      ret = set_state_toggle(&ival,&ival2,switch_cover_toggle);
      if (ret != 0) {
	printf("Switch Cover %i value %i \n",i,ival);
      }
    }
    

    /* Circuit Breaker Light Potentiometer: used to test Servos for now */
    /* Panel Potentiometer is directly wired to the Overhead Backlighting */
    float servoval = 0.0;
    int servotest = 1; /* set to 1 for testing servos by using the potentiometer below */
    if (servotest == 1) {
      ret = axis_input(device,3,&servoval,0.0,1.0);
      if (ret == 1) {
	printf("SERVO TEST: %f \n",servoval);
      }
    }
 
    
    device = 6;
    card = 0;

    /* --------------------- */
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
      printf("Battery Switch %i \n",ival);
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
    int *dc_amps = link_dataref_int("laminar/B738/dc_amp_value");
    int *ac_volt = link_dataref_int("laminar/B738/ac_volt_value");
    int *ac_amps = link_dataref_int("laminar/B738/ac_amp_value");
    int *ac_freq = link_dataref_int("laminar/B738/ac_freq_value");
    if (*battery == 1) {
      ret = mastercard_display(device,card,11,2,dc_amps,0);
      ret = mastercard_display(device,card,6,2,dc_volt,0);
      ret = mastercard_display(device,card,1,3,ac_volt,0);
      ret = mastercard_display(device,card,4,2,ac_amps,0);
      ret = mastercard_display(device,card,8,3,ac_freq,0);
    } else {
      ival = 10;
      ret = mastercard_display(device,card,11,2,&ival,1);
      ret = mastercard_display(device,card,6,2,&ival,1);
      ret = mastercard_display(device,card,1,3,&ival,1);
      ret = mastercard_display(device,card,4,2,&ival,1);
      ret = mastercard_display(device,card,8,3,&ival,1);
    }
    
    /* ------------------ */
    /* Engine Start Panel */
    /* ------------------ */
    device = 6;
    card = 0;

    /* Ignitior Switch */
    int *ign_source = link_dataref_int("laminar/B738/toggle_switch/eng_start_source");
    *ign_source = 0;
    ret = digital_input(device,card,27,&ival,0);
    if (ival == 1) *ign_source = 1;
    ret = digital_input(device,card,28,&ival,0);
    if (ival == 1) *ign_source = -1;
    
    /* Left Engine Start Knob */
    int *l_eng_start_left = link_dataref_cmd_once("laminar/B738/knob/eng1_start_left");
    int *l_eng_start_right = link_dataref_cmd_once("laminar/B738/knob/eng1_start_right");
    int *l_eng_start = link_dataref_int("laminar/B738/engine/starter1_pos");
    
    ret = digital_input(device,card,18,&ival,0);
    if (ival == 1) l_eng_start_pos = 0; // GRD
    ret = digital_input(device,card,19,&ival,0);
    if (ival == 1) l_eng_start_pos = 1; // AUTO
    ret = digital_input(device,card,20,&ival,0);
    if (ival == 1) l_eng_start_pos = 2; // CONT
    ret = digital_input(device,card,21,&ival,0);
    if (ival == 1) l_eng_start_pos = 3; // FLT

    ret = set_state_updn(&l_eng_start_pos,l_eng_start,l_eng_start_right,l_eng_start_left);
    if (ret != 0) {
      printf("L Engine Start Pos %i \n",l_eng_start_pos);
    }

    /* set relay 2 on output 51 to hold left engine knob at GRD */
    ival = (*l_eng_start == 0);
    ret = digital_output(device,card,51,&ival);

    /* Right Engine Start Knob */
    int *r_eng_start_left = link_dataref_cmd_once("laminar/B738/knob/eng2_start_left");
    int *r_eng_start_right = link_dataref_cmd_once("laminar/B738/knob/eng2_start_right");
    int *r_eng_start = link_dataref_int("laminar/B738/engine/starter2_pos");
    
    ret = digital_input(device,card,22,&ival,0);
    if (ival == 1) r_eng_start_pos = 0; // GRD
    ret = digital_input(device,card,23,&ival,0);
    if (ival == 1) r_eng_start_pos = 1; // AUTO
    ret = digital_input(device,card,24,&ival,0);
    if (ival == 1) r_eng_start_pos = 2; // CONT
    ret = digital_input(device,card,25,&ival,0);
    if (ival == 1) r_eng_start_pos = 3; // FLT

    ret = set_state_updn(&r_eng_start_pos,r_eng_start,r_eng_start_right,r_eng_start_left);
    if (ret != 0) {
      printf("R Engine Start Pos %i \n",r_eng_start_pos);
    }

    /* set relay 1 on output 50 to hold right engine knob at GRD */
    ival = (*r_eng_start == 0);
    ret = digital_output(device,card,50,&ival);


    /* ---------------- */
    /* RHS Lights Panel */
    /* ---------------- */
    device = 6;
    card = 0;

    /* Logo */
    int *logo_off = link_dataref_cmd_once("laminar/B738/switch/logo_light_off");
    int *logo_on = link_dataref_cmd_once("laminar/B738/switch/logo_light_on");
    int *logo = link_dataref_int("laminar/B738/toggle_switch/logo_light");
    ret = digital_input(device,card,29,&ival,0);
    ret = set_state_updn(&ival,logo,logo_on,logo_off);
    /* Position Steady */
    ret = digital_input(device,card,30,&ival,0);
    /* Position Strobe */
    ret = digital_input(device,card,31,&ival2,0);
    int *position_up = link_dataref_cmd_once("laminar/B738/toggle_switch/position_light_up");
    int *position_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/position_light_down");
    int *position = link_dataref_int("laminar/B738/toggle_switch/position_light_pos");
    if (ival != INT_MISS) ival = -ival;
    if ((ival2 != INT_MISS) && (ival != INT_MISS)) ival += ival2;
    ret = set_state_updn(&ival,position,position_up,position_dn);
    /* Anti Collision: ZIBO INOP? */
    int *beacon = link_dataref_int("sim/cockpit/electrical/beacon_lights_on");
    ret = digital_input(device,card,32,beacon,0);
    /* Wing: ZIBO need state dataref */
    int *wing_off = link_dataref_cmd_once("laminar/B738/switch/wing_light_off");
    int *wing_on = link_dataref_cmd_once("laminar/B738/switch/wing_light_on");
    ret = digital_input(device,card,33,&ival,0);
    if ((ret == 1) && (ival == 1)) *wing_on = 1;
    if ((ret == 1) && (ival == 0)) *wing_off = 1;

    /* Wheel Well */
    int *wheel_off = link_dataref_cmd_once("laminar/B738/switch/wheel_light_off");
    int *wheel_on = link_dataref_cmd_once("laminar/B738/switch/wheel_light_on");
    ret = digital_input(device,card,34,&ival,0);
    if ((ret == 1) && (ival == 1)) *wheel_on = 1;
    if ((ret == 1) && (ival == 0)) *wheel_off = 1;        
    
    /* ---------------- */
    /* LHS Lights Panel */
    /* ---------------- */
    device = 6;
    card = 0;

    /* Left Retractable Landing Light */
    int *l_land_retractable_up = link_dataref_cmd_once("laminar/B738/switch/land_lights_ret_left_up");
    int *l_land_retractable_dn = link_dataref_cmd_once("laminar/B738/switch/land_lights_ret_left_dn");
    int *l_land_retractable = link_dataref_int("laminar/B738/switch/land_lights_ret_left_pos");
    ret = digital_input(device,card,36,&ival,0);
    ret = digital_input(device,card,37,&ival2,0);
    if ((ival2 != INT_MISS) && (ival != INT_MISS)) ival += ival2;
    ret = set_state_updn(&ival,l_land_retractable,l_land_retractable_dn,l_land_retractable_up);

    /* Right Retractable Landing Light */
    int *r_land_retractable_up = link_dataref_cmd_once("laminar/B738/switch/land_lights_ret_right_up");
    int *r_land_retractable_dn = link_dataref_cmd_once("laminar/B738/switch/land_lights_ret_right_dn");
    int *r_land_retractable = link_dataref_int("laminar/B738/switch/land_lights_ret_right_pos");
    ret = digital_input(device,card,38,&ival,0);
    ret = digital_input(device,card,39,&ival2,0);
    if ((ival2 != INT_MISS) && (ival != INT_MISS)) ival += ival2;
    ret = set_state_updn(&ival,r_land_retractable,r_land_retractable_dn,r_land_retractable_up);

    /* Left Landing Light */
    int *l_land_off = link_dataref_cmd_once("laminar/B738/switch/land_lights_left_off");
    int *l_land_on = link_dataref_cmd_once("laminar/B738/switch/land_lights_left_on");
    int *l_land = link_dataref_int("laminar/B738/switch/land_lights_left_pos");
    ret = digital_input(device,card,40,&ival,0);
    ret = set_state_updn(&ival,l_land,l_land_on,l_land_off);

    /* Right Landing Light */
    int *r_land_off = link_dataref_cmd_once("laminar/B738/switch/land_lights_right_off");
    int *r_land_on = link_dataref_cmd_once("laminar/B738/switch/land_lights_right_on");
    int *r_land = link_dataref_int("laminar/B738/switch/land_lights_right_pos");
    ret = digital_input(device,card,41,&ival,0);
    ret = set_state_updn(&ival,r_land,r_land_on,r_land_off);

    /* Left Runway Turnoff Light */
    int *l_rwy_off = link_dataref_cmd_once("laminar/B738/switch/rwy_light_left_off");
    int *l_rwy_on = link_dataref_cmd_once("laminar/B738/switch/rwy_light_left_on");
    int *l_rwy = link_dataref_int("laminar/B738/toggle_switch/rwy_light_left");
    ret = digital_input(device,card,42,&ival,0);
    ret = set_state_updn(&ival,l_rwy,l_rwy_on,l_rwy_off);

    /* Right Runway Turnoff Light */
    int *r_rwy_off = link_dataref_cmd_once("laminar/B738/switch/rwy_light_right_off");
    int *r_rwy_on = link_dataref_cmd_once("laminar/B738/switch/rwy_light_right_on");
    int *r_rwy_pos = link_dataref_int("laminar/B738/toggle_switch/rwy_light_right");
    ret = digital_input(device,card,43,&ival,0);
    ret = set_state_updn(&ival,r_rwy_pos,r_rwy_on,r_rwy_off);

    /* Taxi Light */
    int *taxi_off = link_dataref_cmd_once("laminar/B738/toggle_switch/taxi_light_brightness_off");
    int *taxi_on = link_dataref_cmd_once("laminar/B738/toggle_switch/taxi_light_brightness_on");
    int *taxi = link_dataref_int("laminar/B738/toggle_switch/taxi_light_brightness_pos");
    ret = digital_input(device,card,44,&ival,0);
    if (ival == 1) ival = 2;
    ret = set_state_updn(&ival,taxi,taxi_on,taxi_off);

    /* APU Switch */
    int *apu_dn = link_dataref_cmd_once("laminar/B738/spring_toggle_switch/APU_start_pos_dn");
    int *apu_up = link_dataref_cmd_once("laminar/B738/spring_toggle_switch/APU_start_pos_up");
    int *apu = link_dataref_int("laminar/B738/spring_toggle_switch/APU_start_pos");
    ret = digital_input(device,card,45,&ival,0);
    if (ival != INT_MISS) ival = 1 - ival;
    ret = digital_input(device,card,46,&ival2,0);
    /* only set APU switch to 2 (start) if pressed since the original switch is spring loaded */
    if ((ret == 1) && (ival != INT_MISS)) ival += ival2; 
    ret = set_state_updn(&ival,apu,apu_dn,apu_up);
    
    /* -------------- */
    /* ALTITUDE Panel */
    /* -------------- */
    device = 6;
    card = 0;
    float *flt_alt = link_dataref_flt("sim/cockpit2/pressurization/actuators/max_allowable_altitude_ft",0);
    float *land_alt = link_dataref_flt("laminar/B738/pressurization/knobs/landing_alt",0);

    ret = mastercard_encoder(device,card,47,land_alt,50.0,2,2);
    if (*land_alt != FLT_MISS) {
      if (*land_alt < -1000.0) *land_alt = -1000.0;
      if (*land_alt > 42000.0) *land_alt = 42000.0;
    }
    if (ret == 1) printf("Landing Altitude Selector: %f \n",*land_alt);
    ret = mastercard_encoder(device,card,49,flt_alt,500.0,2,2);
    if (*flt_alt != FLT_MISS) {
      if (*flt_alt < -1000.0) *flt_alt = -1000.0;
      if (*flt_alt > 42000.0) *flt_alt = 42000.0;
    }
    if (ret == 1) printf("Flight Altitude Selctor: %f \n",*flt_alt);
    
    if (*battery == 1) {
      int land_alt2 = INT_MISS;
      if (*land_alt != FLT_MISS) land_alt2 = (int) *land_alt;
      int flt_alt2 = INT_MISS;
      if (*flt_alt != FLT_MISS)	flt_alt2 = (int) *flt_alt;
      ret = mastercard_display(device,card,0,5,&land_alt2,0);
      ret = mastercard_display(device,card,6,5,&flt_alt2,0);
    } else {
      ival = 10;
      ret = mastercard_display(device,card,0,5,&ival,1);
      ret = mastercard_display(device,card,6,5,&ival,1);
    }

    int *air_valve_manual_left = link_dataref_cmd_hold("laminar/B738/toggle_switch/air_valve_manual_left");
    ret = digital_input(device,card,52,air_valve_manual_left,0);
    int *air_valve_manual_right = link_dataref_cmd_hold("laminar/B738/toggle_switch/air_valve_manual_right");
    ret = digital_input(device,card,51,air_valve_manual_right,0);

    int *air_valve_ctrl_left = link_dataref_cmd_once("laminar/B738/toggle_switch/air_valve_ctrl_left");
    int *air_valve_ctrl_right = link_dataref_cmd_once("laminar/B738/toggle_switch/air_valve_ctrl_right");
    int *air_valve_ctrl = link_dataref_int("laminar/B738/toggle_switch/air_valve_ctrl");
    ret = digital_input(device,card,54,&ival,0);
    if ((ret == 1) && (ival == 1)) air_valve_ctrl_pos = 0;
    ret = digital_input(device,card,55,&ival,0);
    if ((ret == 1) && (ival == 1)) air_valve_ctrl_pos = 1;
    ret = digital_input(device,card,56,&ival,0);
    if ((ret == 1) && (ival == 1)) air_valve_ctrl_pos = 2;
    ret = set_state_updn(&air_valve_ctrl_pos,air_valve_ctrl,air_valve_ctrl_right,air_valve_ctrl_left);

    device = 8;
    float *outflow_valve = link_dataref_flt("laminar/B738/outflow_valve",-2);
    if (servotest == 1) {
      ret = servos_output(device,0,&servoval,0.0,1.0,500,900);
    } else {
      ret = servos_output(device,0,outflow_valve,0.0,1.0,500,900);
    }
      
    
    /* ---------------- */
    /* PNEUMATICS Panel */
    /* ---------------- */
    
    device = 6;
    card = 0;
    /* R Recirculation Fan Switch */
    int *r_recirc_fan = link_dataref_int("laminar/B738/air/r_recirc_fan_pos");
    ret = digital_input(device,card,57,r_recirc_fan,0);
    /* L Recirculation Fan Switch */
    int *l_recirc_fan = link_dataref_int("laminar/B738/air/l_recirc_fan_pos");
    ret = digital_input(device,card,58,l_recirc_fan,0);
    
    /* R Pack Switch */
    int *r_pack = link_dataref_int("laminar/B738/air/r_pack_pos");
    ret = digital_input(device,card,59,&ival,0);
    ret = digital_input(device,card,60,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) {
      *r_pack = (1-ival2) + ival;
    }
    /* L Pack Switch */
    int *l_pack = link_dataref_int("laminar/B738/air/l_pack_pos");
    ret = digital_input(device,card,61,&ival,0);
    ret = digital_input(device,card,62,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) {
      *l_pack = (1-ival2) + ival;
    }
    /* Isolation Valve Switch */
    int *isolation_valve = link_dataref_int("laminar/B738/air/isolation_valve_pos");
    ret = digital_input(device,card,63,&ival,0);
    ret = digital_input(device,card,64,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) {
      *isolation_valve = (1-ival2) + ival;
    }
    
    /* R Bleed Air Switch */
    int *r_bleed_air = link_dataref_int("laminar/B738/toggle_switch/bleed_air_2_pos");
    ret = digital_input(device,card,65,r_bleed_air,0);
    /* APU Bleed Air Switch */
    int *apu_bleed_air = link_dataref_int("laminar/B738/toggle_switch/bleed_air_apu_pos");
    ret = digital_input(device,card,66,apu_bleed_air,0);
    /* L Bleed Air Switch */
    int *l_bleed_air = link_dataref_int("laminar/B738/toggle_switch/bleed_air_1_pos");
    ret = digital_input(device,card,67,l_bleed_air,0);
    
    /* Trip Reset */
    int *trip_reset = link_dataref_cmd_hold("laminar/B738/push_button/bleed_trip_reset");
    ret = digital_input(device,card,68,trip_reset,0);

    /* Blue Annunciators */
    int *l_ram_door = link_dataref_int("laminar/B738/annunciator/ram_door_open1");
    ret = mastercard_display(device,card,32,1,l_ram_door,0);
    int *r_ram_door = link_dataref_int("laminar/B738/annunciator/ram_door_open2");
    ret = mastercard_display(device,card,33,1,r_ram_door,0);
    /* Yellow / Green Annunciators */
    int *dual_bleed = link_dataref_int("laminar/B738/annunciator/dual_bleed");
    ret = digital_output(device,card,14,dual_bleed);
    int *pack_left = link_dataref_int("laminar/B738/annunciator/pack_left");
    ret = digital_output(device,card,15,pack_left);
    int *wing_ovht_left = link_dataref_int("laminar/B738/annunciator/wing_body_ovht_left");
    ret = digital_output(device,card,16,wing_ovht_left);
    int *bleed_trip_left = link_dataref_int("laminar/B738/annunciator/bleed_trip_1");
    ret = digital_output(device,card,17,bleed_trip_left);
    int *pack_right = link_dataref_int("laminar/B738/annunciator/pack_right");
    ret = digital_output(device,card,18,pack_right);
    int *wing_ovht_right = link_dataref_int("laminar/B738/annunciator/wing_body_ovht_right");
    ret = digital_output(device,card,19,wing_ovht_right);
    int *bleed_trip_right = link_dataref_int("laminar/B738/annunciator/bleed_trip_2");
    ret = digital_output(device,card,20,bleed_trip_right);
    int *auto_fail = link_dataref_int("laminar/B738/annunciator/autofail");
    ret = digital_output(device,card,21,auto_fail);
    int *off_sched_descent = link_dataref_int("laminar/B738/annunciator/off_sched_descent");
    ret = digital_output(device,card,22,off_sched_descent);
    int *altn_press = link_dataref_int("laminar/B738/annunciator/altn_press");
    ret = digital_output(device,card,23,altn_press);
    int *manual_press = link_dataref_int("laminar/B738/annunciator/manual_press");
    ret = digital_output(device,card,24,manual_press);

    device = 8;
    float *duct_press_left = link_dataref_flt("laminar/B738/indicators/duct_press_L",0);
    float *duct_press_right = link_dataref_flt("laminar/B738/indicators/duct_press_R",0);
    if (servotest == 1) {
      ret = servos_output(device,1,&servoval,0.0,1.0,200,1000);
      ret = servos_output(device,2,&servoval,0.0,1.0,200,1023);
    } else {
      ret = servos_output(device,1,duct_press_left,0.0,75.0,200,1000);
      ret = servos_output(device,2,duct_press_right,0.0,75.0,200,1023);
    }
    
      
    /* ----------------- */
    /* TEMPERATURE Panel */
    /* ----------------- */
    device = 6;
    card = 0;

    /* Trim Air Switch */
    int *trim_air = link_dataref_int("laminar/B738/air/trim_air_pos");
    ret = digital_input(device,card,26,trim_air,0);

    /* Air Temperature Source Selector Knob */
    device = 6;
    card = 1;
    int *air_temp_source = link_dataref_int("laminar/B738/toggle_switch/air_temp_source");
    ret = digital_input(device,card,0,&ival,0);
    if (ival == 1) *air_temp_source = 0; // DUCT SUPPLY CONT CAB
    ret = digital_input(device,card,1,&ival,0);
    if (ival == 1) *air_temp_source = 1; // DUCT SUPPLY FWD
    ret = digital_input(device,card,2,&ival,0);
    if (ival == 1) *air_temp_source = 2; // DUCT SUPPLY AFT
    ret = digital_input(device,card,3,&ival,0);
    if (ival == 1) *air_temp_source = 3; // PASS CAB FWD
    ret = digital_input(device,card,4,&ival,0);
    if (ival == 1) *air_temp_source = 4; // PASS CAB AFT
    ret = digital_input(device,card,5,&ival,0);
    if (ival == 1) *air_temp_source = 5; // PACK R
    ret = digital_input(device,card,6,&ival,0);
    if (ival == 1) *air_temp_source = 6; // PACK L

    /* Zone Temp Gauge */
    device = 8;
    float *zone_temp = link_dataref_flt("laminar/B738/zone_temp",-1);
    if (servotest == 1) {
      ret = servos_output(device,3,&servoval,0.0,1.0,210,1023);
    } else {
      ret = servos_output(device,3,zone_temp,0.0,90.0,210,1023);
    }
      
    /* Yellow Annunciators */
    device = 6;
    card = 0;
    ival = 1;
    /* CONT CAB ZONE TEMP ANNUNCIATOR */
    ret = digital_output(device,card,25,&ival);
    /* FWD CAB ZONE TEMP ANNUNCIATOR */
    ret = digital_output(device,card,26,&ival);
    /* AFT CAB ZONE TEMP ANNUNCIATOR */
    ret = digital_output(device,card,27,&ival);

    /* Zone Temp Potentiometers */
    float *cont_cab_rheostat = link_dataref_flt("laminar/B738/air/cont_cab_temp/rheostat",-2);
    ret = axis_input(device,0,cont_cab_rheostat,0.0,1.0);
    if (ret == 1) {
      printf("CONT CAB TEMP RHEO: %f \n",*cont_cab_rheostat);
    } 
    float *fwd_cab_rheostat = link_dataref_flt("laminar/B738/air/fwd_cab_temp/rheostat",-2);
    ret = axis_input(device,1,fwd_cab_rheostat,0.0,1.0);
    if (ret == 1) {
      printf("FWD CAB TEMP RHEO: %f \n",*fwd_cab_rheostat);
    } 
    float *aft_cab_rheostat = link_dataref_flt("laminar/B738/air/aft_cab_temp/rheostat",-2);
    ret = axis_input(device,2,aft_cab_rheostat,0.0,1.0);
    if (ret == 1) {
      printf("AFT CAB TEMP RHEO: %f \n",*aft_cab_rheostat);
    } 

    /* -------------------- */
    /* ALTITUDE GAUGE Panel */
    /* -------------------- */
    device = 6;
    card = 0;

    /* Altitude Warning Horn Cutout */
    int *alt_horn_cutout = link_dataref_cmd_hold("laminar/B738/alert/alt_horn_cutout");
    ret = digital_input(device,card,35,alt_horn_cutout,0);

    device = 7;
    float *cabin_altitude = link_dataref_flt("laminar/B738/cabin_alt",2);
    float *cabin_pressure_diff = link_dataref_flt("laminar/B738/cabin_pressure_diff",-1);
    float *cabin_climb = link_dataref_flt("laminar/B738/cabin_vvi",0);
    if (servotest == 1) {
      ret = servos_output(device,0,&servoval,0.0,1.0,200,1023);
      ret = servos_output(device,1,&servoval,0.0,1.0,210,1023);
      ret = servos_output(device,2,&servoval,0.0,1.0,200,900);
    } else {
      ret = servos_output(device,0,cabin_altitude,0.0,27000.0,200,1023);
      ret = servos_output(device,1,cabin_pressure_diff,0.0,7.0,210,1023);
      ret = servos_output(device,2,cabin_climb,-2.0,2.0,200,900);
    }
    
    /* ---------------------- */
    /* Cockpit Redorder Panel */
    /* ---------------------- */
    device = 6;
    card = 0;

    /* Cockpit Voice Recorder Status */
    ret = digital_input(device,card,69,&ival,0);
    if (ret == 1) {
      printf("Cockpit Voice Recorder Status: %i \n",ival);
    }
    /* Cockpit Voice Recorder Erase */
    ret = digital_input(device,card,70,&ival,0);
    if (ret == 1) {
      printf("Cockpit Voice Recorder Erase: %i \n",ival);
    }
    /* Cockpit Voice Recorder Test */
    ret = digital_input(device,card,71,&ival,0);
    if (ret == 1) {
      printf("Cockpit Voice Recorder Test: %i \n",ival);
    }

    /* --------------------- */
    /* DOOR Open/Close Panel */
    /* --------------------- */
    device = 6;
    card = 0;

    int *aft_entry_door = link_dataref_int("laminar/B738/annunciator/aft_entry");
    ret = digital_output(device,card,28,aft_entry_door);
    int *aft_service_door = link_dataref_int("laminar/B738/annunciator/aft_service");
    ret = digital_output(device,card,29,aft_service_door);
    int *equip_door = link_dataref_int("laminar/B738/annunciator/equip_door");
    ret = digital_output(device,card,30,equip_door);
    int *left_aft_overwing_door = link_dataref_int("laminar/B738/annunciator/left_aft_overwing");
    ret = digital_output(device,card,31,left_aft_overwing_door);
    int *right_aft_overwing_door = link_dataref_int("laminar/B738/annunciator/right_aft_overwing");
    ret = digital_output(device,card,32,right_aft_overwing_door);
    int *aft_cargo_door = link_dataref_int("laminar/B738/annunciator/aft_cargo");
    ret = digital_output(device,card,33,aft_cargo_door);
    int *left_fwd_overwing_door = link_dataref_int("laminar/B738/annunciator/left_fwd_overwing");
    ret = digital_output(device,card,34,left_fwd_overwing_door);
    int *right_fwd_overwing_door = link_dataref_int("laminar/B738/annunciator/right_fwd_overwing");
    ret = digital_output(device,card,35,right_fwd_overwing_door);
    int *fwd_cargo_door = link_dataref_int("laminar/B738/annunciator/fwd_cargo");
    ret = digital_output(device,card,36,fwd_cargo_door);
    int *fwd_entry_door = link_dataref_int("laminar/B738/annunciator/fwd_entry");
    ret = digital_output(device,card,37,fwd_entry_door);
    int *fwd_service_door = link_dataref_int("laminar/B738/annunciator/fwd_service");
    ret = digital_output(device,card,38,fwd_service_door);

    
    /* ---------------------- */
    /*  Hydraulic Pumps Panel */
    /* ---------------------- */
    device = 6;
    card = 1;

    int *hydro_pumps1 = link_dataref_int("laminar/B738/toggle_switch/hydro_pumps1_pos");
    ret = digital_input(device,card,9,hydro_pumps1,0);
    if (ret == 1) {
      printf("Hydraulic Pumps 1: %i \n",*hydro_pumps1);
    }
    int *elec_hydro_pumps1 = link_dataref_int("laminar/B738/toggle_switch/electric_hydro_pumps1_pos");
    ret = digital_input(device,card,10,elec_hydro_pumps1,0);
    if (ret == 1) {
      printf("Electric Hydraulic Pumps 1: %i \n",*elec_hydro_pumps1);
    }
    int *elec_hydro_pumps2 = link_dataref_int("laminar/B738/toggle_switch/electric_hydro_pumps2_pos");
    ret = digital_input(device,card,11,elec_hydro_pumps2,0);
    if (ret == 1) {
      printf("Electric Hydraulic Pumps 2: %i \n",*elec_hydro_pumps2);
    }
    int *hydro_pumps2 = link_dataref_int("laminar/B738/toggle_switch/hydro_pumps2_pos");
    ret = digital_input(device,card,12,hydro_pumps2,0);
    if (ret == 1) {
      printf("Hydraulic Pumps 2: %i \n",*hydro_pumps2);
    }

    device = 6;
    card = 0;
    
    int *elec_hydro_ovht1 = link_dataref_int("laminar/B738/annunciator/el_hyd_ovht_1");
    ret = digital_output(device,card,39,elec_hydro_ovht1);
    int *elec_hydro_ovht2 = link_dataref_int("laminar/B738/annunciator/el_hyd_ovht_2");
    ret = digital_output(device,card,40,elec_hydro_ovht2);
    int *hydro_press1 = link_dataref_int("laminar/B738/annunciator/hyd_press_a");
    ret = digital_output(device,card,41,hydro_press1);
    int *elec_hydro_press1 = link_dataref_int("laminar/B738/annunciator/hyd_el_press_a");
    ret = digital_output(device,card,42,elec_hydro_press1);
    int *elec_hydro_press2 = link_dataref_int("laminar/B738/annunciator/hyd_el_press_b");
    ret = digital_output(device,card,43,elec_hydro_press2);
    int *hydro_press2 = link_dataref_int("laminar/B738/annunciator/hyd_press_b");
    ret = digital_output(device,card,44,hydro_press2);
    
    /* --------------- */
    /*  Anti Ice Panel */
    /* --------------- */
    device = 6;
    card = 1;

    int *wing_anti_ice = link_dataref_int("laminar/B738/ice/wing_heat_pos");
    ret = digital_input(device,card,13,wing_anti_ice,0);
    if (ret == 1) {
      printf("Wing Anti Ice: %i \n",*wing_anti_ice);
    }
    int *eng1_anti_ice = link_dataref_int("laminar/B738/ice/eng1_heat_pos");
    ret = digital_input(device,card,14,eng1_anti_ice,0);
    if (ret == 1) {
      printf("Engine 1 Anti Ice: %i \n",*eng1_anti_ice);
    }
    int *eng2_anti_ice = link_dataref_int("laminar/B738/ice/eng2_heat_pos");
    ret = digital_input(device,card,15,eng2_anti_ice,0);
    if (ret == 1) {
      printf("Engine 2 Anti Ice: %i \n",*eng2_anti_ice);
    }

    device = 6;
    card = 0;
    
    int *cowl_anti_ice1 = link_dataref_int("laminar/B738/annunciator/cowl_ice_0");
    ret = digital_output(device,card,45,cowl_anti_ice1);
    int *cowl_anti_ice2 = link_dataref_int("laminar/B738/annunciator/cowl_ice_1");
    ret = digital_output(device,card,46,cowl_anti_ice2);

    /* Blue Annunciators */
    float *wing_anti_ice1 = link_dataref_flt("laminar/B738/annunciator/wing_ice_on_L",-1);
    ival = 0;
    if (*wing_anti_ice1 >= 0.5) ival = 2;
    if (*wing_anti_ice1 == 1.0) ival = 1;
    ret = mastercard_display(device,card,34,1,&ival,0);
    float *wing_anti_ice2 = link_dataref_flt("laminar/B738/annunciator/wing_ice_on_R",-1);
    ival = 0;
    if (*wing_anti_ice2 >= 0.5) ival = 2;
    if (*wing_anti_ice2 == 1.0) ival = 1;
    ret = mastercard_display(device,card,35,1,&ival,0);
    float *cowl_valve_open1 = link_dataref_flt("laminar/B738/annunciator/cowl_ice_on_0",-1);
    ival = 0;
    if (*cowl_valve_open1 >= 0.5) ival = 2;
    if (*cowl_valve_open1 == 1.0) ival = 1;
    ret = mastercard_display(device,card,36,1,&ival,0);
    float *cowl_valve_open2 = link_dataref_flt("laminar/B738/annunciator/cowl_ice_on_1",-1);
    ival = 0;
    if (*cowl_valve_open2 >= 0.5) ival = 2;
    if (*cowl_valve_open2 == 1.0) ival = 1;
    ret = mastercard_display(device,card,37,1,&ival,0);

    
    /* ----------- */
    /*  HEAT Panel */
    /* ----------- */
    device = 6;
    card = 1;

    int *tat_test = link_dataref_cmd_hold("laminar/B738/push_button/tat_test");
    ret = digital_input(device,card,18,tat_test,0);
    if (ret == 1) {
      printf("TAT Test: %i \n",*tat_test);
    }
    int *probe_heat_capt = link_dataref_int("laminar/B738/toggle_switch/capt_probes_pos");
    ret = digital_input(device,card,19,probe_heat_capt,0);
    if (ret == 1) {
      printf("Probe Heat Captain: %i \n",*probe_heat_capt);
    }
    int *probe_heat_fo = link_dataref_int("laminar/B738/toggle_switch/fo_probes_pos");
    ret = digital_input(device,card,20,probe_heat_fo,0);
    if (ret == 1) {
      printf("Probe Heat First Officer: %i \n",*probe_heat_fo);
    }
    int *window_heat_l_side = link_dataref_int("laminar/B738/ice/window_heat_l_side_pos");
    ret = digital_input(device,card,21,window_heat_l_side,0);
    if (ret == 1) {
      printf("Window Heat Left Side: %i \n",*window_heat_l_side);
    }
    int *window_heat_l_fwd = link_dataref_int("laminar/B738/ice/window_heat_l_fwd_pos");
    ret = digital_input(device,card,22,window_heat_l_fwd,0);
    if (ret == 1) {
      printf("Window Heat Left forward: %i \n",*window_heat_l_fwd);
    }
    int *window_overheat_test = link_dataref_int("laminar/B738/toggle_switch/window_ovht_test");
    ret = digital_input(device,card,23,&ival,0);
    ret = digital_input(device,card,24,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) *window_overheat_test = ival - ival2;

    int *window_heat_r_fwd = link_dataref_int("laminar/B738/ice/window_heat_r_fwd_pos");
    ret = digital_input(device,card,25,window_heat_r_fwd,0);
    if (ret == 1) {
      printf("Window Heat Right forward: %i \n",*window_heat_r_fwd);
    }
    int *window_heat_r_side = link_dataref_int("laminar/B738/ice/window_heat_r_side_pos");
    ret = digital_input(device,card,26,window_heat_r_side,0);
    if (ret == 1) {
      printf("Window Heat Right Side: %i \n",*window_heat_r_side);
    }

    int *capt_pitot = link_dataref_int("laminar/B738/annunciator/capt_pitot_off");
    ret = digital_output(device,card,11,capt_pitot);
    ret = digital_output(device,card,12,capt_pitot);
    int *capt_aoa = link_dataref_int("laminar/B738/annunciator/capt_aoa_off");
    ret = digital_output(device,card,13,capt_aoa);
    ret = digital_output(device,card,14,capt_pitot);
   
    int *fo_pitot = link_dataref_int("laminar/B738/annunciator/fo_pitot_off");
    ret = digital_output(device,card,15,fo_pitot);
    ret = digital_output(device,card,16,fo_pitot);
    int *fo_aoa = link_dataref_int("laminar/B738/annunciator/fo_aoa_off");
    ret = digital_output(device,card,17,fo_aoa);
    ret = digital_output(device,card,18,fo_pitot);

    int *window_ovht_l_side_ann = link_dataref_int("laminar/B738/annunciator/window_heat_ovht_ls");
    ret = digital_output(device,card,19,window_ovht_l_side_ann);
    int *window_ovht_l_fwd_ann = link_dataref_int("laminar/B738/annunciator/window_heat_ovht_lf");
    ret = digital_output(device,card,20,window_ovht_l_fwd_ann);
    int *window_ovht_r_fwd_ann = link_dataref_int("laminar/B738/annunciator/window_heat_ovht_rf");
    ret = digital_output(device,card,21,window_ovht_r_fwd_ann);
    int *window_ovht_r_side_ann = link_dataref_int("laminar/B738/annunciator/window_heat_ovht_rs");
    ret = digital_output(device,card,22,window_ovht_r_side_ann);

    int *window_heat_l_side_ann = link_dataref_int("laminar/B738/annunciator/window_heat_l_side");
    ret = digital_output(device,card,23,window_heat_l_side_ann);
    int *window_heat_l_fwd_ann = link_dataref_int("laminar/B738/annunciator/window_heat_l_fwd");
    ret = digital_output(device,card,24,window_heat_l_fwd_ann);
    int *window_heat_r_fwd_ann = link_dataref_int("laminar/B738/annunciator/window_heat_r_fwd");
    ret = digital_output(device,card,25,window_heat_r_fwd_ann);
    int *window_heat_r_side_ann = link_dataref_int("laminar/B738/annunciator/window_heat_r_side");
    ret = digital_output(device,card,26,window_heat_r_side_ann);

    /* ------------- */
    /*  CENTER Panel */
    /* ------------- */
    device = 6;
    card = 0;

    int *attend = link_dataref_cmd_hold("laminar/B738/push_button/attend");
    ret = digital_input(device,card,53,attend,0);
    if (ret == 1) {
      printf("ATTEND: %i \n",*attend);
    }

    device = 6;
    card = 1;
    /* Chimes Only / No Smoking */
    ret = digital_input(device,card,7,&ival,0);
    if (ret == 1) {
      printf("Chimes On: %i \n",ival);
    }
    ret = digital_input(device,card,8,&ival2,0);
    if (ret == 1) {
      printf("Chimes Off: %i \n",ival2);
    }

    /* Fasten Seat Belts */
    int *belts_up = link_dataref_cmd_once("laminar/B738/toggle_switch/seatbelt_sign_up");
    int *belts_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/seatbelt_sign_dn");
    int *belts = link_dataref_int("laminar/B738/toggle_switch/seatbelt_sign_pos");
    ret = digital_input(device,card,16,&ival,0);
    if (ret == 1) {
      printf("Fasten Belts On: %i \n",ival);
    }
    ret = digital_input(device,card,17,&ival2,0);
    if (ret == 1) {
      printf("Fasten Belts Off: %i \n",ival2);
    }
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) ival = ival*2 + (1-ival2);
    ret = set_state_updn(&ival,belts,belts_dn,belts_up);

    int *eq_cool_supply = link_dataref_int("laminar/B738/toggle_switch/eq_cool_supply");
    ret = digital_input(device,card,27,eq_cool_supply,0);
    int *eq_cool_exhaust = link_dataref_int("laminar/B738/toggle_switch/eq_cool_exhaust");
    ret = digital_input(device,card,28,eq_cool_exhaust,0);

    int *exit_lights_up = link_dataref_cmd_once("laminar/B738/toggle_switch/emer_exit_lights_up");
    int *exit_lights_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/emer_exit_lights_dn");
    int *exit_lights = link_dataref_int("laminar/B738/toggle_switch/emer_exit_lights");
    ret = digital_input(device,card,29,&ival,0);
    if (ret == 1) {
      printf("EMERGENCY Exit Lights ON: %i \n",ival);
    }
    ret = digital_input(device,card,30,&ival2,0);
    if (ret == 1) {
      printf("EMERGENCY Exit Lights OFF: %i \n",ival2);
    }
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) ival = ival*2 + (1-ival2);
    ret = set_state_updn(&ival,exit_lights,exit_lights_dn,exit_lights_up);

    int *grd_call = link_dataref_cmd_hold("laminar/B738/push_button/grd_call");
    ret = digital_input(device,card,31,grd_call,0);
    if (ret == 1) {
      printf("GRD CALL: %i \n",*grd_call);
    }
    
    int *r_wiper_up = link_dataref_cmd_once("laminar/B738/knob/right_wiper_up");
    int *r_wiper_dn = link_dataref_cmd_once("laminar/B738/knob/right_wiper_dn");
    int *r_wiper = link_dataref_int("laminar/B738/switches/right_wiper_pos");
    
    ret = digital_input(device,card,32,&ival,0);
    if (ival == 1) r_wiper_pos = 0; // PARK
    ret = digital_input(device,card,33,&ival,0);
    if (ival == 1) r_wiper_pos = 1; // INT
    ret = digital_input(device,card,34,&ival,0);
    if (ival == 1) r_wiper_pos = 2; // LOW
    ret = digital_input(device,card,35,&ival,0);
    if (ival == 1) r_wiper_pos = 3; // HIGHT

    ret = set_state_updn(&r_wiper_pos,r_wiper,r_wiper_up,r_wiper_dn);
    if (ret != 0) {
      printf("Right Wiper %i \n",r_wiper_pos);
    }

    /* Emergency Exit Lights NOT ARMED */
    ival = 0;
    if ((*exit_lights == 0) || (*exit_lights == 2)) ival = 1;
    if (*lights_test == 1) ival = 1;
    ret = digital_output(device,card,27,&ival);

    /* EQUIP COOLING SUPPLY OFF */
    ival = 0;
    if (*lights_test == 1) ival = 1;
    ret = digital_output(device,card,28,&ival);
    
    /* EQUIP COOLING EXHAUST OFF */
    ival = 0;
    if (*lights_test == 1) ival = 1;
    ret = digital_output(device,card,29,&ival);
    
    /* Lavatory Smoke */
    int *smoke = link_dataref_int("laminar/B738/annunciator/smoke");
    ret = digital_output(device,card,30,smoke);

    /* Blue CALL Annunciator */
    device = 6;
    card = 0;
    ret = mastercard_display(device,card,38,1,lights_test,0);


    /* -------------------- */
    /*  STANDBY POWER Panel */
    /* -------------------- */

    /* Use the analog inputs of the Servo Motors Card because there are not enough digital inputs */

    device = 8;
    int *drive_discon_1_up = link_dataref_cmd_once("laminar/B738/one_way_switch/drive_disconnect1");
    int *drive_discon_1_dn = link_dataref_cmd_once("laminar/B738/one_way_switch/drive_disconnect1_off");
    int *drive_discon_1 = link_dataref_int("laminar/B738/one_way_switch/drive_disconnect1_pos");

    int drive_discon_1_pos = 0;
    ret = axis_input(device,0,&fval,0.0,1.0);
    if (fval > 0.5) drive_discon_1_pos = 1;
    ret = set_state_updn(&drive_discon_1_pos,drive_discon_1,drive_discon_1_up,drive_discon_1_dn);
    if (ret != 0) {
      printf("Drive Disconnect 1 %i \n",drive_discon_1_pos);
    }

    int *drive_discon_2_up = link_dataref_cmd_once("laminar/B738/one_way_switch/drive_disconnect2");
    int *drive_discon_2_dn = link_dataref_cmd_once("laminar/B738/one_way_switch/drive_disconnect2_off");
    int *drive_discon_2 = link_dataref_int("laminar/B738/one_way_switch/drive_disconnect2_pos");

    int drive_discon_2_pos = 0;
    ret = axis_input(device,1,&fval,0.0,1.0);
    if (fval > 0.5) drive_discon_2_pos = 1;
    ret = set_state_updn(&drive_discon_2_pos,drive_discon_2,drive_discon_2_up,drive_discon_2_dn);
    if (ret != 0) {
      printf("Drive Disconnect 2 %i \n",drive_discon_2_pos);
    }

    device = 6;
    card = 1;

    int *standby_power_switch = link_dataref_int("laminar/B738/electric/standby_bat_pos");
    ret = digital_input(device,card,36,&ival,0);
    ret = digital_input(device,card,37,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) *standby_power_switch = ival - ival2;

    int *drive_1_ann = link_dataref_int("laminar/B738/annunciator/drive1");
    ret = digital_output(device,card,31,drive_1_ann);

    int *standby_pwr_ann = link_dataref_int("laminar/B738/annunciator/standby_pwr_off");
    ret = digital_output(device,card,32,standby_pwr_ann);

    int *drive_2_ann = link_dataref_int("laminar/B738/annunciator/drive2");
    ret = digital_output(device,card,33,drive_2_ann);

    
    /* ------------------- */
    /*  POWER SOURCE Panel */
    /* ------------------- */

    device = 6;
    card = 1;
    int *grd_pwr_switch = link_dataref_int("laminar/B738/electrical/gpu_pos");
    ret = digital_input(device,card,38,&ival,0);
    ret = digital_input(device,card,39,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) *grd_pwr_switch = ival - ival2;
   
    int *l_wiper_up = link_dataref_cmd_once("laminar/B738/knob/left_wiper_up");
    int *l_wiper_dn = link_dataref_cmd_once("laminar/B738/knob/left_wiper_dn");
    int *l_wiper = link_dataref_int("laminar/B738/switches/left_wiper_pos");
    
    ret = digital_input(device,card,40,&ival,0);
    if (ival == 1) l_wiper_pos = 0; // PARK
    ret = digital_input(device,card,41,&ival,0);
    if (ival == 1) l_wiper_pos = 1; // INT
    ret = digital_input(device,card,42,&ival,0);
    if (ival == 1) l_wiper_pos = 2; // LOW
    ret = digital_input(device,card,43,&ival,0);
    if (ival == 1) l_wiper_pos = 3; // HIGHT

    ret = set_state_updn(&l_wiper_pos,l_wiper,l_wiper_up,l_wiper_dn);
    if (ret != 0) {
      printf("Left Wiper %i \n",l_wiper_pos);
    }

    int *bus_transfer = link_dataref_int("sim/cockpit2/electrical/cross_tie");
    ret = digital_input(device,card,45,&ival,0);
    if (ival != INT_MISS) *bus_transfer = 1-ival;
    
    int *gen1_switch = link_dataref_int("laminar/B738/electrical/gen1_pos");
    ret = digital_input(device,card,46,&ival,0);
    ret = digital_input(device,card,47,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) *gen1_switch = ival - ival2;

    int *apu_gen1_switch = link_dataref_int("laminar/B738/electrical/apu_gen1_pos");
    ret = digital_input(device,card,48,&ival,0);
    ret = digital_input(device,card,49,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) *apu_gen1_switch = ival - ival2;

    int *apu_gen2_switch = link_dataref_int("laminar/B738/electrical/apu_gen2_pos");
    ret = digital_input(device,card,50,&ival,0);
    ret = digital_input(device,card,51,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) *apu_gen2_switch = ival - ival2;

    int *gen2_switch = link_dataref_int("laminar/B738/electrical/gen2_pos");
    ret = digital_input(device,card,52,&ival,0);
    ret = digital_input(device,card,53,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) *gen2_switch = ival - ival2;

    device = 6;
    card = 0;

    float *grd_pwr_avail = link_dataref_flt("laminar/B738/annunciator/ground_power_avail",-1);
    ival = 0;
    if (*grd_pwr_avail >= 0.5) ival = 2;
    if (*grd_pwr_avail == 1.0) ival = 1;
    ret = mastercard_display(device,card,39,1,&ival,0);

    float *apu_gen_off_bus = link_dataref_flt("laminar/B738/annunciator/apu_gen_off_bus",-1);
    ival = 0;
    if (*apu_gen_off_bus >= 0.5) ival = 2;
    if (*apu_gen_off_bus == 1.0) ival = 1;
    ret = mastercard_display(device,card,40,1,&ival,0);
    
    float *transfer_bus_off_1 = link_dataref_flt("laminar/B738/annunciator/trans_bus_off1",-1);
    ival = 0;
    if (*transfer_bus_off_1 >= 0.5) ival = 2;
    if (*transfer_bus_off_1 == 1.0) ival = 1;
    ret = mastercard_display(device,card,41,1,&ival,0);

    float *source_off_1 = link_dataref_flt("laminar/B738/annunciator/source_off1",-1);
    ival = 0;
    if (*source_off_1 >= 0.5) ival = 2;
    if (*source_off_1 == 1.0) ival = 1;
    ret = mastercard_display(device,card,42,1,&ival,0);

    float *gen_off_bus_1 = link_dataref_flt("laminar/B738/annunciator/gen_off_bus1",-1);
    ival = 0;
    if (*gen_off_bus_1 >= 0.5) ival = 2;
    if (*gen_off_bus_1 == 1.0) ival = 1;
    ret = mastercard_display(device,card,43,1,&ival,0);

    float *transfer_bus_off_2 = link_dataref_flt("laminar/B738/annunciator/trans_bus_off2",-1);
    ival = 0;
    if (*transfer_bus_off_2 >= 0.5) ival = 2;
    if (*transfer_bus_off_2 == 1.0) ival = 1;
    ret = mastercard_display(device,card,44,1,&ival,0);

    float *source_off_2 = link_dataref_flt("laminar/B738/annunciator/source_off2",-1);
    ival = 0;
    if (*source_off_2 >= 0.5) ival = 2;
    if (*source_off_2 == 1.0) ival = 1;
    ret = mastercard_display(device,card,45,1,&ival,0);

    float *gen_off_bus_2 = link_dataref_flt("laminar/B738/annunciator/gen_off_bus2",-1);
    ival = 0;
    if (*gen_off_bus_2 >= 0.5) ival = 2;
    if (*gen_off_bus_2 == 1.0) ival = 1;
    ret = mastercard_display(device,card,46,1,&ival,0);

    /* BLUE MAINT ANNUNCIATOR NOT YET AVAILABLE IN ZIBO MOD */
    ret = mastercard_display(device,card,47,1,lights_test,0);

    int *apu_low_oil = link_dataref_int("laminar/B738/annunciator/apu_low_oil");
    ret = mastercard_display(device,card,48,1,apu_low_oil,0);

    int *apu_fault = link_dataref_int("laminar/B738/annunciator/apu_fault");
    ret = mastercard_display(device,card,49,1,apu_fault,0);

    /* YELLOW OVER SPEED ANNUNCIATOR NOT YET AVAILABLE IN ZIBO MOD */
    ret = mastercard_display(device,card,50,1,lights_test,0);

    device = 7;
    float *apu_temp = link_dataref_flt("laminar/B738/electrical/apu_temp",-1);
    if (servotest == 1) {
      ret = servos_output(device,3,&servoval,0.0,1.0,200,990);
    } else {
      ret = servos_output(device,3,apu_temp,0.0,100.0,200,990);
    }

    
    /* --------------- */
    /* FUEL PUMP Panel */
    /* --------------- */

    device = 6;
    card = 1;

    int *cross_feed = link_dataref_int("laminar/B738/knobs/cross_feed_pos");
    ret = digital_input(device,card,54,cross_feed,0);

    int *fuel_pump_aft_1_on = link_dataref_int("laminar/B738/fuel/fuel_tank_pos_lft1");
    ret = digital_input(device,card,55,fuel_pump_aft_1_on,0);
    int *fuel_pump_fwd_1_on = link_dataref_int("laminar/B738/fuel/fuel_tank_pos_lft2");
    ret = digital_input(device,card,56,fuel_pump_fwd_1_on,0);
    int *fuel_pump_fwd_2_on = link_dataref_int("laminar/B738/fuel/fuel_tank_pos_rgt2");
    ret = digital_input(device,card,57,fuel_pump_fwd_2_on,0);
    int *fuel_pump_aft_2_on = link_dataref_int("laminar/B738/fuel/fuel_tank_pos_rgt1");
    ret = digital_input(device,card,58,fuel_pump_aft_2_on,0);

    /* use analog inputs of servo card for ctr fuel pump switches */
    device = 8;    
    int *fuel_pump_ctr_1_on = link_dataref_int("laminar/B738/fuel/fuel_tank_pos_ctr1");
    ret = axis_input(device,2,&fval,0.0,1.0);
    if (fval < 0.5) {
     *fuel_pump_ctr_1_on = 1;
    } else {
     *fuel_pump_ctr_1_on = 0;
    }
    int *fuel_pump_ctr_2_on = link_dataref_int("laminar/B738/fuel/fuel_tank_pos_ctr2");
    ret = axis_input(device,3,&fval,0.0,1.0);
    if (fval < 0.5) {
     *fuel_pump_ctr_2_on = 1;
    } else {
     *fuel_pump_ctr_2_on = 0;
    }

    float *fuel_temp = link_dataref_flt("xpserver/fuel_temp",0);
    if (servotest == 1) {
      ret = servos_output(device,4,&servoval,0.0,1.0,200,975);
    } else {
      ret = servos_output(device,4,fuel_temp,-50.0,50.0,200,975);
    }
      

    device = 6;
    card = 0;

    float *eng_valve_closed_1 = link_dataref_flt("laminar/B738/annunciator/eng1_valve_closed",-1);
    ival = 0;
    if (*eng_valve_closed_1 >= 0.5) ival = 2;
    if (*eng_valve_closed_1 == 1.0) ival = 1;
    ret = mastercard_display(device,card,51,1,&ival,0);
    
    float *spar_valve_closed_1 = link_dataref_flt("laminar/B738/annunciator/spar1_valve_closed",-1);
    ival = 0;
    if (*spar_valve_closed_1 >= 0.5) ival = 2;
    if (*spar_valve_closed_1 == 1.0) ival = 1;
    ret = mastercard_display(device,card,52,1,&ival,0);
    
    float *bypass_filter_1 = link_dataref_flt("laminar/B738/annunciator/bypass_filter_1",-1);
    ival = 0;
    if (*bypass_filter_1 >= 0.5) ival = 2;
    if (*bypass_filter_1 == 1.0) ival = 1;
    ret = mastercard_display(device,card,53,1,&ival,0);
    
    float *eng_valve_closed_2 = link_dataref_flt("laminar/B738/annunciator/eng2_valve_closed",-1);
    ival = 0;
    if (*eng_valve_closed_2 >= 0.5) ival = 2;
    if (*eng_valve_closed_2 == 1.0) ival = 1;
    ret = mastercard_display(device,card,54,1,&ival,0);
    
    float *spar_valve_closed_2 = link_dataref_flt("laminar/B738/annunciator/spar2_valve_closed",-1);
    ival = 0;
    if (*spar_valve_closed_2 >= 0.5) ival = 2;
    if (*spar_valve_closed_2 == 1.0) ival = 1;
    ret = mastercard_display(device,card,55,1,&ival,0);
    
    float *bypass_filter_2 = link_dataref_flt("laminar/B738/annunciator/bypass_filter_2",-1);
    ival = 0;
    if (*bypass_filter_2 >= 0.5) ival = 2;
    if (*bypass_filter_2 == 1.0) ival = 1;
    ret = mastercard_display(device,card,56,1,&ival,0);
    
    float *cross_feed_valve = link_dataref_flt("laminar/B738/fuel/cross_feed_valve",-1);
    ival = 0;
    if (*cross_feed_valve >= 0.5) ival = 2;
    if (*cross_feed_valve == 1.0) ival = 1;
    ret = mastercard_display(device,card,57,1,&ival,0);
    
    int *low_press_ctr_1 = link_dataref_int("laminar/B738/annunciator/low_fuel_press_c1");
    ret = mastercard_display(device,card,58,1,low_press_ctr_1,0);
    int *low_press_ctr_2 = link_dataref_int("laminar/B738/annunciator/low_fuel_press_c2");
    ret = mastercard_display(device,card,59,1,low_press_ctr_2,0);
    int *low_press_aft_1 = link_dataref_int("laminar/B738/annunciator/low_fuel_press_l1");
    ret = mastercard_display(device,card,60,1,low_press_aft_1,0);
    int *low_press_fwd_1 = link_dataref_int("laminar/B738/annunciator/low_fuel_press_l2");
    ret = mastercard_display(device,card,61,1,low_press_fwd_1,0);
    int *low_press_fwd_2 = link_dataref_int("laminar/B738/annunciator/low_fuel_press_r2");
    ret = mastercard_display(device,card,62,1,low_press_fwd_2,0);
    int *low_press_aft_2 = link_dataref_int("laminar/B738/annunciator/low_fuel_press_r1");
    ret = mastercard_display(device,card,63,1,low_press_aft_2,0);

    
    /* ------------------ */
    /* NAV / DISP CONTROL */
    /* ------------------ */

    device = 6;
    card = 1;

    int *vhf_nav_source = link_dataref_int("laminar/B738/toggle_switch/vhf_nav_source");
    ret = digital_input(device,card,59,&ival,0);
    ret = digital_input(device,card,60,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) *vhf_nav_source = ival - ival2;
    
    int *irs_source = link_dataref_int("laminar/B738/toggle_switch/irs_source");
    ret = digital_input(device,card,61,&ival,0);
    ret = digital_input(device,card,62,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) *irs_source = ival - ival2;
    
    int *display_source = link_dataref_int("laminar/B738/toggle_switch/dspl_source");
    ret = digital_input(device,card,63,&ival,0);
    ret = digital_input(device,card,64,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) *display_source = ival2 - ival;
    
    int *control_panel_source = link_dataref_int("laminar/B738/toggle_switch/dspl_ctrl_pnl");
    ret = digital_input(device,card,65,&ival,0);
    ret = digital_input(device,card,66,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) *control_panel_source = ival - ival2;
    
  }
    
}

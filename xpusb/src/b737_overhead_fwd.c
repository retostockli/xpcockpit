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
float dc_power_pos;
float ac_power_pos;
float l_eng_start_pos;
float r_eng_start_pos;
float air_valve_ctrl_pos;
float l_wiper_pos;
float r_wiper_pos;
float vhf_nav_source_pos;
float irs_source_pos;
float display_source_pos;
float control_panel_source_pos;
float flt_ctrl_A_pos;
float flt_ctrl_B_pos;


void b737_overhead_fwd(void)
{
  int ret;
  int ival;
  int ival2;
  float fval;
  int device;
  int card;
  int one = 1;
  int zero = 0;
  
  /* Switch Covers and related switch states*/
  char buffer[100];
  int cover;
  int ncover;
  //  int battery_switch_pos;
       
  /* device list */
  int mastercard = 6;
  int servo1 = 7;
  int servo2 = 8;

  /* only run for Laminar 737 or ZIBO 737 */
  if ((acf_type == 2) || (acf_type == 3)) {
    
    float *lights_test = link_dataref_flt("laminar/B738/annunciator/test",-1);
    int *avionics_on = link_dataref_int("sim/cockpit2/switches/avionics_power_on");


    /* ------------- */
    /* SWITCH COVERS */
    /* ------------- */
   
    ncover = 11;
    int *switch_cover_toggle;
    float *switch_cover_pos = link_dataref_flt_arr("laminar/B738/button_switch/cover_position",ncover,-1,-3);
    /* open all covers */
    for (cover=0;cover<ncover;cover++) {
      sprintf(buffer, "laminar/B738/button_switch_cover%02d", cover);
      switch_cover_toggle = link_dataref_cmd_once(buffer);
      ival = 1;
      ret = set_switch_cover(switch_cover_pos+cover,switch_cover_toggle,ival);     
    }

    ncover = 5;
    /* set second datarefs to switch cover positions */
    char *switch_cover_pos_name[5] = {"laminar/B738/switches/alt_flaps_cover_pos",
			      "laminar/B738/switches/flt_ctr_A_cover_pos",
			      "laminar/B738/switches/flt_ctr_B_cover_pos",
			      "laminar/B738/switches/spoiler_A_cover_pos",
			      "laminar/B738/switches/spoiler_B_cover_pos"};
    /* set second datarefs to switch ocver commands */
    char *switch_cover_toggle_name[5] = {"laminar/B738/toggle_switch/alt_flaps_cover",
				     "laminar/B738/toggle_switch/flt_ctr_A_cover",
				     "laminar/B738/toggle_switch/flt_ctr_B_cover",
				     "laminar/B738/toggle_switch/spoiler_A_cover",
				     "laminar/B738/toggle_switch/spoiler_B_cover"};
    for (cover=0;cover<ncover;cover++) {
      switch_cover_pos = link_dataref_flt(switch_cover_pos_name[cover],-3);
      switch_cover_toggle = link_dataref_cmd_once(switch_cover_toggle_name[cover]);
      ival = 1;
      ret = set_switch_cover(switch_cover_pos,switch_cover_toggle,ival);
    }



    device = mastercard;
   
    /* CONT CABIN TEMP Potentiometer: used to test Servos */
    /* Flood light and Panel Potentiometer are directly wired to the Overhead Backlighting */
    float servoval = 0.0;
    int *servotest = link_dataref_int("xpserver/servotest");
    if (*servotest == 1) {
      ret = axis_input(device,0,&servoval,0.0,1.0);
      if (ret == 1) {
	printf("SERVO TEST: %f \n",servoval);
      }
    }

    
    device = mastercard;
    card = 0;

    /* --------------------- */
    /* Power & Battery Panel */
    /* --------------------- */

    /* Battery Switch: dn is moving switch to 1 and up to 0 */
    int *battery_up = link_dataref_cmd_once("laminar/B738/push_button/batt_full_off"); // includes guard
    //int *battery_up = link_dataref_cmd_once("laminar/B738/switch/battery_up");
    int *battery_dn = link_dataref_cmd_once("laminar/B738/switch/battery_dn");
    float *battery = link_dataref_flt("laminar/B738/electric/battery_pos",0);
    ret = digital_inputf(device,card,8,&fval,0);
    if (fval != FLT_MISS) fval = 1.0 - fval;

    ret = set_state_updnf(&fval,battery,battery_dn,battery_up);
    if (ret != 0) {
      printf("Battery Switch %f \n",fval);
    }

    /* DC Power Knob */
    int *dc_power_up = link_dataref_cmd_once("laminar/B738/knob/dc_power_up");
    int *dc_power_dn = link_dataref_cmd_once("laminar/B738/knob/dc_power_dn");
    float *dc_power = link_dataref_flt("laminar/B738/knob/dc_power",0);

    ret = digital_input(device,card,0,&ival,0);
    if (ival == 1) dc_power_pos = 0.0; // STBY PWR
    ret = digital_input(device,card,1,&ival,0);
    if (ival == 1) dc_power_pos = 1.0; // BAT BUS
    ret = digital_input(device,card,2,&ival,0);
    if (ival == 1) dc_power_pos = 2.0; // BAT
    ret = digital_input(device,card,3,&ival,0);
    if (ival == 1) dc_power_pos = 2.0; // AUX BAT (does not exist in ZIBO)
    ret = digital_input(device,card,4,&ival,0);
    if (ival == 1) dc_power_pos = 3.0; // TR1
    ret = digital_input(device,card,5,&ival,0);
    if (ival == 1) dc_power_pos = 4.0; // TR2
    ret = digital_input(device,card,6,&ival,0);
    if (ival == 1) dc_power_pos = 5.0; // TR3
    ret = digital_input(device,card,7,&ival,0);
    if (ival == 1) dc_power_pos = 6.0; // TEST

    ret = set_state_updnf(&dc_power_pos,dc_power,dc_power_up,dc_power_dn);
    if (ret != 0) {
      printf("DC Power knob %f \n",dc_power_pos);
    }
    
    /* AC Power Knob */
    int *ac_power_up = link_dataref_cmd_once("laminar/B738/knob/ac_power_up");
    int *ac_power_dn = link_dataref_cmd_once("laminar/B738/knob/ac_power_dn");
    float *ac_power = link_dataref_flt("laminar/B738/knob/ac_power",0);
    
    ret = digital_input(device,card,9,&ival,0);
    if (ival == 1) ac_power_pos = 0.0; // STBY PWR
    ret = digital_input(device,card,10,&ival,0);
    if (ival == 1) ac_power_pos = 1.0; // GRD PWR
    ret = digital_input(device,card,11,&ival,0);
    if (ival == 1) ac_power_pos = 2.0; // GEN1
    ret = digital_input(device,card,12,&ival,0);
    if (ival == 1) ac_power_pos = 3.0; // APU GEN
    ret = digital_input(device,card,13,&ival,0);
    if (ival == 1) ac_power_pos = 4.0; // GEN2
    ret = digital_input(device,card,14,&ival,0);
    if (ival == 1) ac_power_pos = 5.0; // INV
    ret = digital_input(device,card,15,&ival,0);
    if (ival == 1) ac_power_pos = 6.0; // TEST

    ret = set_state_updnf(&ac_power_pos,ac_power,ac_power_up,ac_power_dn);
    if (ret != 0) {
      printf("AC Power knob %f \n",ac_power_pos);
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
    /* Backlighting Only when Battery is on */
    ret = digital_outputf(device,card,49,battery);

    /* set relay 3 on output 52 on relay card through d-sub con */
    /* Yaw Damper Solenoid driven by yaw damper position in X-Plane */
    float *yaw_damper = link_dataref_flt("laminar/B738/toggle_switch/yaw_dumper_pos",0);
    ret = digital_outputf(device,card,52,yaw_damper);

    /* Annunciators */
    float *bat_discharge = link_dataref_flt("laminar/B738/annunciator/bat_discharge",-1);
    ret = digital_outputf(device,card,11,bat_discharge);
    float *tr_unit = link_dataref_flt("laminar/B738/annunciator/tr_unit",-1);
    ret = digital_outputf(device,card,12,tr_unit);
    float *elec = link_dataref_flt("laminar/B738/annunciator/elec",-1);
    ret = digital_outputf(device,card,13,elec);

    /* AC/DC 7 Segment Displays
       Lower Row: DC Volts 6,7, AC Amps 4,5, AC Volts 1,2,3
       Upper Row: DC Amps 11,12, CPS FREQ 8,9,10
       Todo: show or not show values depending on AC/DC Knob positions
    */
    device = mastercard;
    card = 1;

    float *dc_volt = link_dataref_flt("laminar/B738/dc_volt_value",0);
    float *dc_amps = link_dataref_flt("laminar/B738/dc_amp_value",0);
    float *ac_volt = link_dataref_flt("laminar/B738/ac_volt_value",0);
    float *ac_amps = link_dataref_flt("laminar/B738/ac_amp_value",0);
    float *ac_freq = link_dataref_flt("laminar/B738/ac_freq_value",0);
    if (*avionics_on == 1) {
      ret = mastercard_displayf(device,card,11,2,dc_amps,0);
      ret = mastercard_displayf(device,card,6,2,dc_volt,0);
      ret = mastercard_displayf(device,card,1,3,ac_volt,0);
      ret = mastercard_displayf(device,card,4,2,ac_amps,0);
      ret = mastercard_displayf(device,card,8,3,ac_freq,0);
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
    device = mastercard;
    card = 0;

    /* Ignitior Switch */
    float *ign_source = link_dataref_flt("laminar/B738/toggle_switch/eng_start_source",0);
    *ign_source = 0.0;
    ret = digital_input(device,card,27,&ival,0);
    if (ival == 1) *ign_source = 1.0;
    ret = digital_input(device,card,28,&ival,0);
    if (ival == 1) *ign_source = -1.0;
    
    /* Left Engine Start Knob */
    int *l_eng_start_left = link_dataref_cmd_once("laminar/B738/knob/eng1_start_left");
    int *l_eng_start_right = link_dataref_cmd_once("laminar/B738/knob/eng1_start_right");
    float *l_eng_start = link_dataref_flt("laminar/B738/engine/starter1_pos",-1);
    
    ret = digital_input(device,card,18,&ival,0);
    if (ival == 1) l_eng_start_pos = 0.0; // GRD
    ret = digital_input(device,card,19,&ival,0);
    if (ival == 1) l_eng_start_pos = 1.0; // AUTO
    ret = digital_input(device,card,20,&ival,0);
    if (ival == 1) l_eng_start_pos = 2.0; // CONT
    ret = digital_input(device,card,21,&ival,0);
    if (ival == 1) l_eng_start_pos = 3.0; // FLT

    ret = set_state_updnf(&l_eng_start_pos,l_eng_start,l_eng_start_right,l_eng_start_left);
    if (ret != 0) {
      printf("L Engine Start Pos %f %f %i %i \n",
	     l_eng_start_pos,*l_eng_start,*l_eng_start_right,*l_eng_start_left);
    }

    /* set relay 2 on output 51 to hold left engine knob at GRD */
    ival = (*l_eng_start == 0.0);
    ret = digital_output(device,card,51,&ival);

    /* Right Engine Start Knob */
    int *r_eng_start_left = link_dataref_cmd_once("laminar/B738/knob/eng2_start_left");
    int *r_eng_start_right = link_dataref_cmd_once("laminar/B738/knob/eng2_start_right");
    float *r_eng_start = link_dataref_flt("laminar/B738/engine/starter2_pos",-1);
    
    ret = digital_input(device,card,22,&ival,0);
    if (ival == 1) r_eng_start_pos = 0.0; // GRD
    ret = digital_input(device,card,23,&ival,0);
    if (ival == 1) r_eng_start_pos = 1.0; // AUTO
    ret = digital_input(device,card,24,&ival,0);
    if (ival == 1) r_eng_start_pos = 2.0; // CONT
    ret = digital_input(device,card,25,&ival,0);
    if (ival == 1) r_eng_start_pos = 3.0; // FLT

    ret = set_state_updnf(&r_eng_start_pos,r_eng_start,r_eng_start_right,r_eng_start_left);
    if (ret != 0) {
      printf("R Engine Start Pos %f %f %i %i \n",
	     r_eng_start_pos,*r_eng_start,*r_eng_start_right,*r_eng_start_left);
    }

    /* set relay 1 on output 50 to hold right engine knob at GRD */
    ival = (*r_eng_start == 0.0);
    ret = digital_output(device,card,50,&ival);

    
    /* ---------------- */
    /* RHS Lights Panel */
    /* ---------------- */
    device = mastercard;
    card = 0;

    /* Logo */
    int *logo_off = link_dataref_cmd_once("laminar/B738/switch/logo_light_off");
    int *logo_on = link_dataref_cmd_once("laminar/B738/switch/logo_light_on");
    float *logo = link_dataref_flt("laminar/B738/toggle_switch/logo_light",0);
    ret = digital_inputf(device,card,29,&fval,0);
    ret = set_state_updnf(&fval,logo,logo_on,logo_off);
    /* Position Steady */
    ret = digital_input(device,card,30,&ival,0);
    /* Position Strobe */
    ret = digital_input(device,card,31,&ival2,0);
    int *position_up = link_dataref_cmd_once("laminar/B738/toggle_switch/position_light_up");
    int *position_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/position_light_down");
    float *position = link_dataref_flt("laminar/B738/toggle_switch/position_light_pos",0);
    fval = 0.0;
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) fval = (float) (-ival + ival2);
    ret = set_state_updnf(&fval,position,position_up,position_dn);
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
    if ((ret == 1) && (ival == 0)) *wheel_on = 1;
    if ((ret == 1) && (ival == 1)) *wheel_off = 1;        

    
    /* ---------------- */
    /* LHS Lights Panel */
    /* ---------------- */
    device = mastercard;
    card = 0;
    
    /* Left Retractable Landing Light */
    int *l_land_retractable_up = link_dataref_cmd_once("laminar/B738/switch/land_lights_ret_left_up");
    int *l_land_retractable_dn = link_dataref_cmd_once("laminar/B738/switch/land_lights_ret_left_dn");
    float *l_land_retractable = link_dataref_flt("laminar/B738/switch/land_lights_ret_left_pos",0);
    ret = digital_input(device,card,36,&ival,0);
    ret = digital_input(device,card,37,&ival2,0);
    fval = 0.0;
    if ((ival2 != INT_MISS) && (ival != INT_MISS)) fval = (float) (ival + ival2);
    ret = set_state_updnf(&fval,l_land_retractable,l_land_retractable_dn,l_land_retractable_up);

    /* Right Retractable Landing Light */
    int *r_land_retractable_up = link_dataref_cmd_once("laminar/B738/switch/land_lights_ret_right_up");
    int *r_land_retractable_dn = link_dataref_cmd_once("laminar/B738/switch/land_lights_ret_right_dn");
    float *r_land_retractable = link_dataref_flt("laminar/B738/switch/land_lights_ret_right_pos",0);
    ret = digital_input(device,card,38,&ival,0);
    ret = digital_input(device,card,39,&ival2,0);
    if ((ival2 != INT_MISS) && (ival != INT_MISS)) fval = (float) (ival + ival2);
    ret = set_state_updnf(&fval,r_land_retractable,r_land_retractable_dn,r_land_retractable_up);

    /* Left Landing Light */
    int *l_land_off = link_dataref_cmd_once("laminar/B738/switch/land_lights_left_off");
    int *l_land_on = link_dataref_cmd_once("laminar/B738/switch/land_lights_left_on");
    float *l_land = link_dataref_flt("laminar/B738/switch/land_lights_left_pos",0);
    ret = digital_inputf(device,card,40,&fval,0);
    ret = set_state_updnf(&fval,l_land,l_land_on,l_land_off);

    /* Right Landing Light */
    int *r_land_off = link_dataref_cmd_once("laminar/B738/switch/land_lights_right_off");
    int *r_land_on = link_dataref_cmd_once("laminar/B738/switch/land_lights_right_on");
    float *r_land = link_dataref_flt("laminar/B738/switch/land_lights_right_pos",0);
    ret = digital_inputf(device,card,41,&fval,0);
    ret = set_state_updnf(&fval,r_land,r_land_on,r_land_off);
    
    /* Left Runway Turnoff Light */
    int *l_rwy_off = link_dataref_cmd_once("laminar/B738/switch/rwy_light_left_off");
    int *l_rwy_on = link_dataref_cmd_once("laminar/B738/switch/rwy_light_left_on");
    float *l_rwy = link_dataref_flt("laminar/B738/toggle_switch/rwy_light_left",0);
    ret = digital_inputf(device,card,42,&fval,0);
    ret = set_state_updnf(&fval,l_rwy,l_rwy_on,l_rwy_off);
    
    /* Right Runway Turnoff Light */
    int *r_rwy_off = link_dataref_cmd_once("laminar/B738/switch/rwy_light_right_off");
    int *r_rwy_on = link_dataref_cmd_once("laminar/B738/switch/rwy_light_right_on");
    float *r_rwy_pos = link_dataref_flt("laminar/B738/toggle_switch/rwy_light_right",0);
    ret = digital_inputf(device,card,43,&fval,0);
    ret = set_state_updnf(&fval,r_rwy_pos,r_rwy_on,r_rwy_off);
   
    /* Taxi Light */
    int *taxi_off = link_dataref_cmd_once("laminar/B738/toggle_switch/taxi_light_brightness_off");
    int *taxi_on = link_dataref_cmd_once("laminar/B738/toggle_switch/taxi_light_brightness_on");
    float *taxi = link_dataref_flt("laminar/B738/toggle_switch/taxi_light_brightness_pos",0);
    ret = digital_inputf(device,card,44,&fval,0);
    if (fval == 1.0) fval = 2.0;
    ret = set_state_updnf(&fval,taxi,taxi_on,taxi_off);

    /* APU Switch */
    int *apu_dn = link_dataref_cmd_once("laminar/B738/spring_toggle_switch/APU_start_pos_dn");
    int *apu_up = link_dataref_cmd_once("laminar/B738/spring_toggle_switch/APU_start_pos_up");
    int *apu_hold_dn = link_dataref_cmd_hold("laminar/B738/spring_toggle_switch/APU_start_pos_dn");
    float *apu = link_dataref_flt("laminar/B738/spring_toggle_switch/APU_start_pos",0);
    ret = digital_input(device,card,45,&ival,0); // OFF
    if (*apu <= 1.0) {
      fval = FLT_MISS;
      if (ival != INT_MISS) fval = (float) (1 - ival); 
      ret = set_state_updnf(&fval,apu,apu_dn,apu_up);
    }
    if (*apu >= 1.0) {
      ret = digital_input(device,card,46,apu_hold_dn,0); // START: spring loaded
    }
    
    /* -------------- */
    /* ALTITUDE Panel */
    /* -------------- */
    device = mastercard;
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
    if (ret == 1) printf("Flight Altitude Selector: %f \n",*flt_alt);
    
    if (*battery == 1.0) {
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
    float *air_valve_ctrl = link_dataref_flt("laminar/B738/toggle_switch/air_valve_ctrl",0);
    ret = digital_input(device,card,54,&ival,0);
    if ((ret == 1) && (ival == 1)) air_valve_ctrl_pos = 0.0;
    ret = digital_input(device,card,55,&ival,0);
    if ((ret == 1) && (ival == 1)) air_valve_ctrl_pos = 1.0;
    ret = digital_input(device,card,56,&ival,0);
    if ((ret == 1) && (ival == 1)) air_valve_ctrl_pos = 2.0;
    ret = set_state_updnf(&air_valve_ctrl_pos,air_valve_ctrl,air_valve_ctrl_right,air_valve_ctrl_left);

    device = servo2;
    float *outflow_valve = link_dataref_flt("laminar/B738/outflow_valve",-2);
    if (*servotest == 1) {
      ret = servos_output(device,0,&servoval,0.0,1.0,550,930);
    } else {
      ret = servos_output(device,0,outflow_valve,0.0,1.0,550,930);
    }
      
    
    /* ---------------- */
    /* PNEUMATICS Panel */
    /* ---------------- */
    
    device = mastercard;
    card = 0;
    /* R Recirculation Fan Switch */
    float *r_recirc_fan = link_dataref_flt("laminar/B738/air/r_recirc_fan_pos",0);
    ret = digital_inputf(device,card,57,r_recirc_fan,0);
    /* L Recirculation Fan Switch */
    float *l_recirc_fan = link_dataref_flt("laminar/B738/air/l_recirc_fan_pos",0);
    ret = digital_inputf(device,card,58,l_recirc_fan,0);
    
    /* R Pack Switch */
    float *r_pack = link_dataref_flt("laminar/B738/air/r_pack_pos",0);
    ret = digital_input(device,card,59,&ival,0);
    ret = digital_input(device,card,60,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) {
      *r_pack = (float) ((1-ival2) + ival);
    }
    /* L Pack Switch */
    float *l_pack = link_dataref_flt("laminar/B738/air/l_pack_pos",0);
    ret = digital_input(device,card,61,&ival,0);
    ret = digital_input(device,card,62,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) {
      *l_pack = (float) ((1-ival2) + ival);
    }
    /* Isolation Valve Switch */
    float *isolation_valve = link_dataref_flt("laminar/B738/air/isolation_valve_pos",0);
    ret = digital_input(device,card,63,&ival,0);
    ret = digital_input(device,card,64,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) {
      *isolation_valve = (float) ((1-ival2) + ival);
    }
    
    /* R Bleed Air Switch */
    float *r_bleed_air = link_dataref_flt("laminar/B738/toggle_switch/bleed_air_2_pos",0);
    ret = digital_inputf(device,card,65,r_bleed_air,0);
    /* APU Bleed Air Switch */
    float *apu_bleed_air = link_dataref_flt("laminar/B738/toggle_switch/bleed_air_apu_pos",0);
    ret = digital_inputf(device,card,66,apu_bleed_air,0);
    /* L Bleed Air Switch */
    float *l_bleed_air = link_dataref_flt("laminar/B738/toggle_switch/bleed_air_1_pos",0);
    ret = digital_inputf(device,card,67,l_bleed_air,0);
    
    /* Trip Reset */
    int *trip_reset = link_dataref_cmd_hold("laminar/B738/push_button/bleed_trip_reset");
    ret = digital_input(device,card,68,trip_reset,0);

    /* Blue Annunciators */
    float *l_ram_door = link_dataref_flt("laminar/B738/annunciator/ram_door_open1",-1);
    ival = *l_ram_door >= 0.025;
    ret = mastercard_display(device,card,32,1,&ival,0);
    float *r_ram_door = link_dataref_flt("laminar/B738/annunciator/ram_door_open2",-1);
    ival = *r_ram_door >= 0.025;
    ret = mastercard_display(device,card,33,1,&ival,0);
    /* Yellow / Green Annunciators */
    float *dual_bleed = link_dataref_flt("laminar/B738/annunciator/dual_bleed",-1);
    ret = digital_outputf(device,card,14,dual_bleed);
    float *pack_left = link_dataref_flt("laminar/B738/annunciator/pack_left",-1);
    ret = digital_outputf(device,card,15,pack_left);
    float *wing_ovht_left = link_dataref_flt("laminar/B738/annunciator/wing_body_ovht_left",-1);
    ret = digital_outputf(device,card,16,wing_ovht_left);
    float *bleed_trip_left = link_dataref_flt("laminar/B738/annunciator/bleed_trip_1",-1);
    ret = digital_outputf(device,card,17,bleed_trip_left);
    float *pack_right = link_dataref_flt("laminar/B738/annunciator/pack_right",-1);
    ret = digital_outputf(device,card,18,pack_right);
    float *wing_ovht_right = link_dataref_flt("laminar/B738/annunciator/wing_body_ovht_right",-1);
    ret = digital_outputf(device,card,19,wing_ovht_right);
    float *bleed_trip_right = link_dataref_flt("laminar/B738/annunciator/bleed_trip_2",-1);
    ret = digital_outputf(device,card,20,bleed_trip_right);
    float *auto_fail = link_dataref_flt("laminar/B738/annunciator/autofail",-1);
    ret = digital_outputf(device,card,21,auto_fail);
    float *off_sched_descent = link_dataref_flt("laminar/B738/annunciator/off_sched_descent",-1);
    ret = digital_outputf(device,card,22,off_sched_descent);
    float *altn_press = link_dataref_flt("laminar/B738/annunciator/altn_press",-1);
    ret = digital_outputf(device,card,23,altn_press);
    float *manual_press = link_dataref_flt("laminar/B738/annunciator/manual_press",-1);
    ret = digital_outputf(device,card,24,manual_press);

    device = servo2;
    float *duct_press_left = link_dataref_flt("laminar/B738/indicators/duct_press_L",0);
    float *duct_press_right = link_dataref_flt("laminar/B738/indicators/duct_press_R",0);
    if (*servotest == 1) {
      ret = servos_output(device,1,&servoval,0.0,1.0,200,1000);
      ret = servos_output(device,2,&servoval,0.0,1.0,200,1023);
    } else {
      ret = servos_output(device,1,duct_press_left,0.0,75.0,200,1000);
      ret = servos_output(device,2,duct_press_right,0.0,75.0,200,1023);
    }

      
    /* ----------------- */
    /* TEMPERATURE Panel */
    /* ----------------- */
    device = mastercard;
    card = 0;

    /* Trim Air Switch */
    float *trim_air = link_dataref_flt("laminar/B738/air/trim_air_pos",0);
    ret = digital_inputf(device,card,26,trim_air,0);

    /* Air Temperature Source Selector Knob */
    device = mastercard;
    card = 1;
    float *air_temp_source = link_dataref_flt("laminar/B738/toggle_switch/air_temp_source",0);
    ret = digital_input(device,card,0,&ival,0);
    if (ival == 1) *air_temp_source = 0.0; // DUCT SUPPLY CONT CAB
    ret = digital_input(device,card,1,&ival,0);
    if (ival == 1) *air_temp_source = 1.0; // DUCT SUPPLY FWD
    ret = digital_input(device,card,2,&ival,0);
    if (ival == 1) *air_temp_source = 2.0; // DUCT SUPPLY AFT
    ret = digital_input(device,card,3,&ival,0);
    if (ival == 1) *air_temp_source = 3.0; // PASS CAB FWD
    ret = digital_input(device,card,4,&ival,0);
    if (ival == 1) *air_temp_source = 4.0; // PASS CAB AFT
    ret = digital_input(device,card,5,&ival,0);
    if (ival == 1) *air_temp_source = 5.0; // PACK R
    ret = digital_input(device,card,6,&ival,0);
    if (ival == 1) *air_temp_source = 6.0; // PACK L

    /* Zone Temp Gauge */
    device = servo2;
    float *zone_temp = link_dataref_flt("laminar/B738/zone_temp",-1);
    if (*servotest == 1) {
      ret = servos_output(device,3,&servoval,0.0,1.0,300,1000);
    } else {
      ret = servos_output(device,3,zone_temp,0.0,80.0,300,1000);
    }
      
    /* Yellow Annunciators */
    device = mastercard;
    card = 0;
    ival = 0;
    if ((*lights_test == 1.0) && (*battery == 1.0)) ival = 1;

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
    device = mastercard;
    card = 0;

    /* Altitude Warning Horn Cutout */
    int *alt_horn_cutout = link_dataref_cmd_hold("laminar/B738/alert/alt_horn_cutout");
    ret = digital_input(device,card,35,alt_horn_cutout,0);

    device = servo1;
    float *cabin_altitude = link_dataref_flt("laminar/B738/cabin_alt",2);
    float *cabin_pressure_diff = link_dataref_flt("laminar/B738/cabin_pressure_diff",-1);
    float *cabin_climb = link_dataref_flt("laminar/B738/cabin_vvi",1);    
    if (*servotest == 1) {
      ret = servos_output(device,0,&servoval,0.0,1.0,200,1023);
      ret = servos_output(device,1,&servoval,0.0,1.0,250,1023);
      ret = servos_output(device,2,&servoval,0.0,1.0,200,860);
    } else {
      ret = servos_output(device,0,cabin_altitude,0.0,27000.0,200,1023);
      ret = servos_output(device,1,cabin_pressure_diff,0.0,6.75,250,1023);
      ret = servos_output(device,2,cabin_climb,-1800.,1800.,200,860);
    }
    
    /* ---------------------- */
    /* Cockpit Redorder Panel */
    /* ---------------------- */
    device = mastercard;
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
    /* use for now for SERVO TEST */
    ret = digital_input(device,card,71,servotest,0);
    if (ret == 1) {
      printf("Servo Test: %i \n",*servotest);
    }

    /* --------------------- */
    /* DOOR Open/Close Panel */
    /* --------------------- */
    device = mastercard;
    card = 0;

    float *aft_entry_door = link_dataref_flt("laminar/B738/annunciator/aft_entry",-1);
    ret = digital_outputf(device,card,28,aft_entry_door);
    float *aft_service_door = link_dataref_flt("laminar/B738/annunciator/aft_service",-1);
    ret = digital_outputf(device,card,29,aft_service_door);
    float *equip_door = link_dataref_flt("laminar/B738/annunciator/equip_door",-1);
    ret = digital_outputf(device,card,30,equip_door);
    float *left_aft_overwing_door = link_dataref_flt("laminar/B738/annunciator/left_aft_overwing",-1);
    ret = digital_outputf(device,card,31,left_aft_overwing_door);
    float *right_aft_overwing_door = link_dataref_flt("laminar/B738/annunciator/right_aft_overwing",-1);
    ret = digital_outputf(device,card,32,right_aft_overwing_door);
    float *aft_cargo_door = link_dataref_flt("laminar/B738/annunciator/aft_cargo",-1);
    ret = digital_outputf(device,card,33,aft_cargo_door);
    float *left_fwd_overwing_door = link_dataref_flt("laminar/B738/annunciator/left_fwd_overwing",-1);
    ret = digital_outputf(device,card,34,left_fwd_overwing_door);
    float *right_fwd_overwing_door = link_dataref_flt("laminar/B738/annunciator/right_fwd_overwing",-1);
    ret = digital_outputf(device,card,35,right_fwd_overwing_door);
    float *fwd_cargo_door = link_dataref_flt("laminar/B738/annunciator/fwd_cargo",-1);
    ret = digital_outputf(device,card,36,fwd_cargo_door);
    float *fwd_entry_door = link_dataref_flt("laminar/B738/annunciator/fwd_entry",-1);
    ret = digital_outputf(device,card,37,fwd_entry_door);
    float *fwd_service_door = link_dataref_flt("laminar/B738/annunciator/fwd_service",-1);
    ret = digital_outputf(device,card,38,fwd_service_door);

    
    /* ---------------------- */
    /*  Hydraulic Pumps Panel */
    /* ---------------------- */
    device = mastercard;
    card = 1;

    float *hydro_pumps1 = link_dataref_flt("laminar/B738/toggle_switch/hydro_pumps1_pos",0);
    ret = digital_inputf(device,card,9,hydro_pumps1,0);
    if (ret == 1) {
      printf("Hydraulic Pumps 1: %f \n",*hydro_pumps1);
    }
    float *elec_hydro_pumps1 = link_dataref_flt("laminar/B738/toggle_switch/electric_hydro_pumps1_pos",0);
    ret = digital_inputf(device,card,10,elec_hydro_pumps1,0);
    if (ret == 1) {
      printf("Electric Hydraulic Pumps 1: %f \n",*elec_hydro_pumps1);
    }
    float *elec_hydro_pumps2 = link_dataref_flt("laminar/B738/toggle_switch/electric_hydro_pumps2_pos",0);
    ret = digital_inputf(device,card,11,elec_hydro_pumps2,0);
    if (ret == 1) {
      printf("Electric Hydraulic Pumps 2: %f \n",*elec_hydro_pumps2);
    }
    float *hydro_pumps2 = link_dataref_flt("laminar/B738/toggle_switch/hydro_pumps2_pos",0);
    ret = digital_inputf(device,card,12,hydro_pumps2,0);
    if (ret == 1) {
      printf("Hydraulic Pumps 2: %f \n",*hydro_pumps2);
    }

    device = mastercard;
    card = 0;
    
    float *elec_hydro_ovht1 = link_dataref_flt("laminar/B738/annunciator/el_hyd_ovht_1",-1);
    ret = digital_outputf(device,card,39,elec_hydro_ovht1);
    float *elec_hydro_ovht2 = link_dataref_flt("laminar/B738/annunciator/el_hyd_ovht_2",-1);
    ret = digital_outputf(device,card,40,elec_hydro_ovht2);
    float *hydro_press1 = link_dataref_flt("laminar/B738/annunciator/hyd_press_a",-1);
    ret = digital_outputf(device,card,41,hydro_press1);
    float *elec_hydro_press1 = link_dataref_flt("laminar/B738/annunciator/hyd_el_press_a",-1);
    ret = digital_outputf(device,card,42,elec_hydro_press1);
    float *elec_hydro_press2 = link_dataref_flt("laminar/B738/annunciator/hyd_el_press_b",-1);
    ret = digital_outputf(device,card,43,elec_hydro_press2);
    float *hydro_press2 = link_dataref_flt("laminar/B738/annunciator/hyd_press_b",-1);
    ret = digital_outputf(device,card,44,hydro_press2);
    
    /* --------------- */
    /*  Anti Ice Panel */
    /* --------------- */
    device = mastercard;
    card = 1;

    float *wing_anti_ice = link_dataref_flt("laminar/B738/ice/wing_heat_pos",0);
    ret = digital_inputf(device,card,13,wing_anti_ice,0);
    if (ret == 1) {
      printf("Wing Anti Ice: %f \n",*wing_anti_ice);
    }
    float *eng1_anti_ice = link_dataref_flt("laminar/B738/ice/eng1_heat_pos",0);
    ret = digital_inputf(device,card,14,eng1_anti_ice,0);
    if (ret == 1) {
      printf("Engine 1 Anti Ice: %f \n",*eng1_anti_ice);
    }
    float *eng2_anti_ice = link_dataref_flt("laminar/B738/ice/eng2_heat_pos",0);
    ret = digital_inputf(device,card,15,eng2_anti_ice,0);
    if (ret == 1) {
      printf("Engine 2 Anti Ice: %f \n",*eng2_anti_ice);
    }

    device = mastercard;
    card = 0;
    
    float *cowl_anti_ice1 = link_dataref_flt("laminar/B738/annunciator/cowl_ice_0",-1);
    ret = digital_outputf(device,card,45,cowl_anti_ice1);
    float *cowl_anti_ice2 = link_dataref_flt("laminar/B738/annunciator/cowl_ice_1",-1);
    ret = digital_outputf(device,card,46,cowl_anti_ice2);

    /* Blue Annunciators */
    float *wing_anti_ice1 = link_dataref_flt("laminar/B738/annunciator/wing_ice_on_L",-1);
    ival = 0;
    if (*wing_anti_ice1 >= 0.025) ival = 2;
    if (*wing_anti_ice1 == 1.0) ival = 1;
    ret = mastercard_display(device,card,34,1,&ival,0);
    float *wing_anti_ice2 = link_dataref_flt("laminar/B738/annunciator/wing_ice_on_R",-1);
    ival = 0;
    if (*wing_anti_ice2 >= 0.025) ival = 2;
    if (*wing_anti_ice2 == 1.0) ival = 1;
    ret = mastercard_display(device,card,35,1,&ival,0);
    float *cowl_valve_open1 = link_dataref_flt("laminar/B738/annunciator/cowl_ice_on_0",-1);
    ival = 0;
    if (*cowl_valve_open1 >= 0.025) ival = 2;
    if (*cowl_valve_open1 == 1.0) ival = 1;
    ret = mastercard_display(device,card,36,1,&ival,0);
    float *cowl_valve_open2 = link_dataref_flt("laminar/B738/annunciator/cowl_ice_on_1",-1);
    ival = 0;
    if (*cowl_valve_open2 >= 0.025) ival = 2;
    if (*cowl_valve_open2 == 1.0) ival = 1;
    ret = mastercard_display(device,card,37,1,&ival,0);

    
    /* ----------- */
    /*  HEAT Panel */
    /* ----------- */
    device = mastercard;
    card = 1;

    int *tat_test = link_dataref_cmd_hold("laminar/B738/push_button/tat_test");
    ret = digital_input(device,card,18,tat_test,0);
    if (ret == 1) {
      printf("TAT Test: %i \n",*tat_test);
    }
    float *probe_heat_capt = link_dataref_flt("laminar/B738/toggle_switch/capt_probes_pos",0);
    ret = digital_inputf(device,card,19,probe_heat_capt,0);
    if (ret == 1) {
      printf("Probe Heat Captain: %f \n",*probe_heat_capt);
    }
    float *probe_heat_fo = link_dataref_flt("laminar/B738/toggle_switch/fo_probes_pos",0);
    ret = digital_inputf(device,card,20,probe_heat_fo,0);
    if (ret == 1) {
      printf("Probe Heat First Officer: %f \n",*probe_heat_fo);
    }
    float *window_heat_l_side = link_dataref_flt("laminar/B738/ice/window_heat_l_side_pos",0);
    ret = digital_inputf(device,card,21,window_heat_l_side,0);
    if (ret == 1) {
      printf("Window Heat Left Side: %f \n",*window_heat_l_side);
    }
    float *window_heat_l_fwd = link_dataref_flt("laminar/B738/ice/window_heat_l_fwd_pos",0);
    ret = digital_inputf(device,card,22,window_heat_l_fwd,0);
    if (ret == 1) {
      printf("Window Heat Left forward: %f \n",*window_heat_l_fwd);
    }
    float *window_overheat_test = link_dataref_flt("laminar/B738/toggle_switch/window_ovht_test",0);
    ret = digital_input(device,card,23,&ival,0);
    ret = digital_input(device,card,24,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) *window_overheat_test = (float) (ival - ival2);

    float *window_heat_r_fwd = link_dataref_flt("laminar/B738/ice/window_heat_r_fwd_pos",0);
    ret = digital_inputf(device,card,25,window_heat_r_fwd,0);
    if (ret == 1) {
      printf("Window Heat Right forward: %f \n",*window_heat_r_fwd);
    }
    float *window_heat_r_side = link_dataref_flt("laminar/B738/ice/window_heat_r_side_pos",0);
    ret = digital_inputf(device,card,26,window_heat_r_side,0);
    if (ret == 1) {
      printf("Window Heat Right Side: %f \n",*window_heat_r_side);
    }

    float *capt_pitot = link_dataref_flt("laminar/B738/annunciator/capt_pitot_off",-1);
    ret = digital_outputf(device,card,11,capt_pitot);
    ret = digital_outputf(device,card,12,capt_pitot);
    float *capt_aoa = link_dataref_flt("laminar/B738/annunciator/capt_aoa_off",-1);
    ret = digital_outputf(device,card,13,capt_aoa);
    ret = digital_outputf(device,card,14,capt_pitot);
   
    float *fo_pitot = link_dataref_flt("laminar/B738/annunciator/fo_pitot_off",-1);
    ret = digital_outputf(device,card,15,fo_pitot);
    ret = digital_outputf(device,card,16,fo_pitot);
    float *fo_aoa = link_dataref_flt("laminar/B738/annunciator/fo_aoa_off",-1);
    ret = digital_outputf(device,card,17,fo_aoa);
    ret = digital_outputf(device,card,18,fo_pitot);

    float *window_ovht_l_side_ann = link_dataref_flt("laminar/B738/annunciator/window_heat_ovht_ls",-1);
    ret = digital_outputf(device,card,19,window_ovht_l_side_ann);
    float *window_ovht_l_fwd_ann = link_dataref_flt("laminar/B738/annunciator/window_heat_ovht_lf",-1);
    ret = digital_outputf(device,card,20,window_ovht_l_fwd_ann);
    float *window_ovht_r_fwd_ann = link_dataref_flt("laminar/B738/annunciator/window_heat_ovht_rf",-1);
    ret = digital_outputf(device,card,21,window_ovht_r_fwd_ann);
    float *window_ovht_r_side_ann = link_dataref_flt("laminar/B738/annunciator/window_heat_ovht_rs",-1);
    ret = digital_outputf(device,card,22,window_ovht_r_side_ann);

    float *window_heat_l_side_ann = link_dataref_flt("laminar/B738/annunciator/window_heat_l_side",-1);
    if (*avionics_on) {
      ret = digital_outputf(device,card,23,window_heat_l_side_ann);
    } else {
      ret = digital_output(device,card,23,&zero);
    }
    float *window_heat_l_fwd_ann = link_dataref_flt("laminar/B738/annunciator/window_heat_l_fwd",-1);
    if (*avionics_on) {
      ret = digital_outputf(device,card,24,window_heat_l_fwd_ann);
    } else {
      ret = digital_output(device,card,24,&zero);
    }
    float *window_heat_r_fwd_ann = link_dataref_flt("laminar/B738/annunciator/window_heat_r_fwd",-1);
    if (*avionics_on) {
      ret = digital_outputf(device,card,25,window_heat_r_fwd_ann);
    } else {
      ret = digital_output(device,card,25,&zero);
    }
    float *window_heat_r_side_ann = link_dataref_flt("laminar/B738/annunciator/window_heat_r_side",-1);
    if (*avionics_on) {
      ret = digital_outputf(device,card,26,window_heat_r_side_ann);
    } else {
      ret = digital_output(device,card,26,&zero);
    }

    /* ------------- */
    /*  CENTER Panel */
    /* ------------- */
    device = mastercard;
    card = 0;

    int *attend = link_dataref_cmd_hold("laminar/B738/push_button/attend");
    ret = digital_input(device,card,53,attend,0);
    if (ret == 1) {
      printf("ATTEND: %i \n",*attend);
    }

    device = mastercard;
    card = 1;
      
    /* Chimes Only / No Smoking */
    /* int test; */
    /* int test2; */
    /* ret = digital_input(device,card,7,&test,0); */
    /* if (ret == 1) { */
    /* } */
    /* ret = digital_input(device,card,8,&test2,0); */
    /* if (ret == 1) { */
    /* } */

    /* Fasten Seat Belts */
    int *belts_up = link_dataref_cmd_once("laminar/B738/toggle_switch/seatbelt_sign_up");
    int *belts_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/seatbelt_sign_dn");
    float *belts = link_dataref_flt("laminar/B738/toggle_switch/seatbelt_sign_pos",0);
    ret = digital_input(device,card,16,&ival,0);
    if (ret == 1) {
      printf("Fasten Belts On: %i \n",ival);
    }
    ret = digital_input(device,card,17,&ival2,0);
    if (ret == 1) {
      printf("Fasten Belts Off: %i \n",ival2);
    }
    fval = 0.0;
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) fval = (float) (ival*2 + (1-ival)*(1-ival2));
    ret = set_state_updnf(&fval,belts,belts_dn,belts_up);

    float *eq_cool_supply = link_dataref_flt("laminar/B738/toggle_switch/eq_cool_supply",0);
    ret = digital_inputf(device,card,27,eq_cool_supply,0);
    float *eq_cool_exhaust = link_dataref_flt("laminar/B738/toggle_switch/eq_cool_exhaust",0);
    ret = digital_inputf(device,card,28,eq_cool_exhaust,0);

    int *exit_lights_up = link_dataref_cmd_once("laminar/B738/toggle_switch/emer_exit_lights_up");
    int *exit_lights_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/emer_exit_lights_dn");
    float *exit_lights = link_dataref_flt("laminar/B738/toggle_switch/emer_exit_lights",0);
    ret = digital_input(device,card,29,&ival,0);
    if (ret == 1) {
      printf("EMERGENCY Exit Lights ON: %i \n",ival);
    }
    ret = digital_input(device,card,30,&ival2,0);
    if (ret == 1) {
      printf("EMERGENCY Exit Lights OFF: %i \n",ival2);
    }
    fval = 0.0;
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) fval = (float) ((2-ival) * (1-ival2));
    ret = set_state_updnf(&fval,exit_lights,exit_lights_dn,exit_lights_up);

    int *grd_call = link_dataref_cmd_hold("laminar/B738/push_button/grd_call");
    ret = digital_input(device,card,31,grd_call,0);
    if (ret == 1) {
      printf("GRD CALL: %i \n",*grd_call);
    }
    
    int *r_wiper_up = link_dataref_cmd_once("laminar/B738/knob/right_wiper_up");
    int *r_wiper_dn = link_dataref_cmd_once("laminar/B738/knob/right_wiper_dn");
    float *r_wiper = link_dataref_flt("laminar/B738/switches/right_wiper_pos",0);
    
    ret = digital_input(device,card,32,&ival,0);
    if (ival == 1) r_wiper_pos = 0.0; // PARK
    ret = digital_input(device,card,33,&ival,0);
    if (ival == 1) r_wiper_pos = 1.0; // INT
    ret = digital_input(device,card,34,&ival,0);
    if (ival == 1) r_wiper_pos = 2.0; // LOW
    ret = digital_input(device,card,35,&ival,0);
    if (ival == 1) r_wiper_pos = 3.0; // HIGHT

    ret = set_state_updnf(&r_wiper_pos,r_wiper,r_wiper_up,r_wiper_dn);
    if (ret != 0) {
      printf("Right Wiper %f \n",r_wiper_pos);
    }

    /* Emergency Exit Lights NOT ARMED */
    ival = 0;
    if (*battery == 1.0) {
      if ((*exit_lights == 0) || (*exit_lights == 2)) ival = 1;
      if (*lights_test == 1.0) ival = 1;
    }
    ret = digital_output(device,card,27,&ival);

    /* EQUIP COOLING SUPPLY OFF */
    ival = 0;
    if (*lights_test == 1.0) ival = 1;
    ret = digital_output(device,card,28,&ival);
    
    /* EQUIP COOLING EXHAUST OFF */
    ival = 0;
    if (*lights_test == 1.0) ival = 1;
    ret = digital_output(device,card,29,&ival);
    
    /* Lavatory Smoke */
    float *smoke = link_dataref_flt("laminar/B738/annunciator/smoke",-1);
    ret = digital_outputf(device,card,30,smoke);

    /* Blue CALL Annunciator */
    device = mastercard;
    card = 0;
    ret = mastercard_displayf(device,card,38,1,lights_test,0);


    /* -------------------- */
    /*  STANDBY POWER Panel */
    /* -------------------- */

    /* Use the analog inputs of the Servo Motors Card because there are not enough digital inputs */

    device = servo2;
    int *drive_discon_1_up = link_dataref_cmd_once("laminar/B738/one_way_switch/drive_disconnect1");
    int *drive_discon_1_dn = link_dataref_cmd_once("laminar/B738/one_way_switch/drive_disconnect1_off");
    float *drive_discon_1 = link_dataref_flt("laminar/B738/one_way_switch/drive_disconnect1_pos",0);

    float drive_discon_1_pos = 0.0;
    ret = axis_input(device,0,&fval,0.0,1.0);
    if (fval < 0.5) drive_discon_1_pos = 1.0;
    ret = set_state_updnf(&drive_discon_1_pos,drive_discon_1,drive_discon_1_up,drive_discon_1_dn);
    if (ret != 0) {
      printf("Drive Disconnect 1 %f \n",drive_discon_1_pos);
    }

    int *drive_discon_2_up = link_dataref_cmd_once("laminar/B738/one_way_switch/drive_disconnect2");
    int *drive_discon_2_dn = link_dataref_cmd_once("laminar/B738/one_way_switch/drive_disconnect2_off");
    float *drive_discon_2 = link_dataref_flt("laminar/B738/one_way_switch/drive_disconnect2_pos",0);

    float drive_discon_2_pos = 0.0;
    ret = axis_input(device,1,&fval,0.0,1.0);
    if (fval < 0.5) drive_discon_2_pos = 1.0;
    ret = set_state_updnf(&drive_discon_2_pos,drive_discon_2,drive_discon_2_up,drive_discon_2_dn);
    if (ret != 0) {
      printf("Drive Disconnect 2 %f \n",drive_discon_2_pos);
    }

    device = mastercard;
    card = 1;

    float *standby_power_switch = link_dataref_flt("laminar/B738/electric/standby_bat_pos",0);
    ret = digital_input(device,card,36,&ival,0);
    ret = digital_input(device,card,37,&ival2,0);
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) *standby_power_switch = (float) (ival - ival2);

    float *drive_1_ann = link_dataref_flt("laminar/B738/annunciator/drive1",-1);
    ret = digital_outputf(device,card,31,drive_1_ann);

    float *standby_pwr_ann = link_dataref_flt("laminar/B738/annunciator/standby_pwr_off",-1);
    ret = digital_outputf(device,card,32,standby_pwr_ann);

    float *drive_2_ann = link_dataref_flt("laminar/B738/annunciator/drive2",-1);
    ret = digital_outputf(device,card,33,drive_2_ann);

    
    /* ------------------- */
    /*  POWER SOURCE Panel */
    /* ------------------- */

    device = mastercard;
    card = 1;
    int *grd_pwr_up = link_dataref_cmd_hold("laminar/B738/toggle_switch/gpu_up");
    int *grd_pwr_dn = link_dataref_cmd_hold("laminar/B738/toggle_switch/gpu_dn");
    ret = digital_input(device,card,38,grd_pwr_dn,0);
    ret = digital_input(device,card,39,grd_pwr_up,0);
   
    int *l_wiper_up = link_dataref_cmd_once("laminar/B738/knob/left_wiper_up");
    int *l_wiper_dn = link_dataref_cmd_once("laminar/B738/knob/left_wiper_dn");
    float *l_wiper = link_dataref_flt("laminar/B738/switches/left_wiper_pos",0);
    
    ret = digital_input(device,card,40,&ival,0);
    if (ival == 1) l_wiper_pos = 0.0; // PARK
    ret = digital_input(device,card,41,&ival,0);
    if (ival == 1) l_wiper_pos = 1.0; // INT
    ret = digital_input(device,card,42,&ival,0);
    if (ival == 1) l_wiper_pos = 2.0; // LOW
    ret = digital_input(device,card,43,&ival,0);
    if (ival == 1) l_wiper_pos = 3.0; // HIGHT

    ret = set_state_updnf(&l_wiper_pos,l_wiper,l_wiper_up,l_wiper_dn);
    if (ret != 0) {
      printf("Left Wiper %f \n",l_wiper_pos);
    }

    int *bus_transfer = link_dataref_int("sim/cockpit2/electrical/cross_tie");
    ret = digital_input(device,card,45,&ival,0);
    if (ival != INT_MISS) *bus_transfer = 1-ival;

    int *gen1_switch_up = link_dataref_cmd_hold("laminar/B738/toggle_switch/gen1_up");
    int *gen1_switch_dn = link_dataref_cmd_hold("laminar/B738/toggle_switch/gen1_dn");
    ret = digital_input(device,card,46,gen1_switch_dn,0);
    ret = digital_input(device,card,47,gen1_switch_up,0);
           
    int *apu_gen1_switch_up = link_dataref_cmd_hold("laminar/B738/toggle_switch/apu_gen1_up");
    int *apu_gen1_switch_dn = link_dataref_cmd_hold("laminar/B738/toggle_switch/apu_gen1_dn");
    ret = digital_input(device,card,48,apu_gen1_switch_dn,0);
    ret = digital_input(device,card,49,apu_gen1_switch_up,0);

    int *apu_gen2_switch_up = link_dataref_cmd_hold("laminar/B738/toggle_switch/apu_gen2_up");
    int *apu_gen2_switch_dn = link_dataref_cmd_hold("laminar/B738/toggle_switch/apu_gen2_dn");
    ret = digital_input(device,card,50,apu_gen2_switch_dn,0);
    ret = digital_input(device,card,51,apu_gen2_switch_up,0);
    
    int *gen2_switch_up = link_dataref_cmd_hold("laminar/B738/toggle_switch/gen2_up");
    int *gen2_switch_dn = link_dataref_cmd_hold("laminar/B738/toggle_switch/gen2_dn");
    ret = digital_input(device,card,52,gen2_switch_dn,0);
    ret = digital_input(device,card,53,gen2_switch_up,0);

    device = mastercard;
    card = 0;

    float *grd_pwr_avail = link_dataref_flt("laminar/B738/annunciator/ground_power_avail",-1);
    ival = 0;
    if (*grd_pwr_avail >= 0.025) ival = 2;
    if (*grd_pwr_avail == 1.0) ival = 1;
    ret = mastercard_display(device,card,39,1,&ival,0);

    float *apu_gen_off_bus = link_dataref_flt("laminar/B738/annunciator/apu_gen_off_bus",-1);
    ival = 0;
    if (*apu_gen_off_bus >= 0.025) ival = 2;
    if (*apu_gen_off_bus == 1.0) ival = 1;
    ret = mastercard_display(device,card,40,1,&ival,0);
    
    float *transfer_bus_off_1 = link_dataref_flt("laminar/B738/annunciator/trans_bus_off1",-1);
    ival = 0;
    if (*transfer_bus_off_1 >= 0.025) ival = 2;
    if (*transfer_bus_off_1 == 1.0) ival = 1;
    ret = mastercard_display(device,card,41,1,&ival,0);

    float *source_off_1 = link_dataref_flt("laminar/B738/annunciator/source_off1",-1);
    ival = 0;
    if (*source_off_1 >= 0.025) ival = 2;
    if (*source_off_1 == 1.0) ival = 1;
    ret = mastercard_display(device,card,42,1,&ival,0);

    float *gen_off_bus_1 = link_dataref_flt("laminar/B738/annunciator/gen_off_bus1",-1);
    ival = 0;
    if (*gen_off_bus_1 >= 0.025) ival = 2;
    if (*gen_off_bus_1 == 1.0) ival = 1;
    ret = mastercard_display(device,card,43,1,&ival,0);

    float *transfer_bus_off_2 = link_dataref_flt("laminar/B738/annunciator/trans_bus_off2",-1);
    ival = 0;
    if (*transfer_bus_off_2 >= 0.025) ival = 2;
    if (*transfer_bus_off_2 == 1.0) ival = 1;
    ret = mastercard_display(device,card,44,1,&ival,0);

    float *source_off_2 = link_dataref_flt("laminar/B738/annunciator/source_off2",-1);
    ival = 0;
    if (*source_off_2 >= 0.025) ival = 2;
    if (*source_off_2 == 1.0) ival = 1;
    ret = mastercard_display(device,card,45,1,&ival,0);

    float *gen_off_bus_2 = link_dataref_flt("laminar/B738/annunciator/gen_off_bus2",-1);
    ival = 0;
    if (*gen_off_bus_2 >= 0.025) ival = 2;
    if (*gen_off_bus_2 == 1.0) ival = 1;
    ret = mastercard_display(device,card,46,1,&ival,0);

    /* BLUE MAINT ANNUNCIATOR NOT YET AVAILABLE IN ZIBO MOD */
    ret = mastercard_displayf(device,card,47,1,lights_test,0);

    float *apu_low_oil = link_dataref_flt("laminar/B738/annunciator/apu_low_oil",-1);
    ival = *apu_low_oil >= 0.025;
    ret = mastercard_display(device,card,48,1,&ival,0);

    float *apu_fault = link_dataref_flt("laminar/B738/annunciator/apu_fault",-1);
    ival = *apu_fault >= 0.025;
    ret = mastercard_display(device,card,49,1,&ival,0);

    /* YELLOW OVER SPEED ANNUNCIATOR NOT YET AVAILABLE IN ZIBO MOD */
    ret = mastercard_displayf(device,card,50,1,lights_test,0);

    device = servo1;
    float *apu_temp = link_dataref_flt("laminar/B738/electrical/apu_temp",-1);
    if (*servotest == 1) {
      ret = servos_output(device,3,&servoval,0.0,1.0,200,990);
    } else {
      ret = servos_output(device,3,apu_temp,0.0,100.0,200,990);
    }
    
    /* --------------- */
    /* FUEL PUMP Panel */
    /* --------------- */

    device = mastercard;
    card = 1;

    float *cross_feed = link_dataref_flt("laminar/B738/knobs/cross_feed_pos",0);
    ret = digital_inputf(device,card,44,cross_feed,0);

    float *fuel_pump_aft_1_on = link_dataref_flt("laminar/B738/fuel/fuel_tank_pos_lft1",0);
    ret = digital_inputf(device,card,67,fuel_pump_aft_1_on,0);
    float *fuel_pump_fwd_1_on = link_dataref_flt("laminar/B738/fuel/fuel_tank_pos_lft2",0);
    ret = digital_inputf(device,card,56,fuel_pump_fwd_1_on,0);
    float *fuel_pump_fwd_2_on = link_dataref_flt("laminar/B738/fuel/fuel_tank_pos_rgt2",0);
    ret = digital_inputf(device,card,57,fuel_pump_fwd_2_on,0);
    float *fuel_pump_aft_2_on = link_dataref_flt("laminar/B738/fuel/fuel_tank_pos_rgt1",0);
    ret = digital_inputf(device,card,58,fuel_pump_aft_2_on,0);

    /* use analog inputs of servo card for ctr fuel pump switches */
    device = servo2;    
    float *fuel_pump_ctr_1_on = link_dataref_flt("laminar/B738/fuel/fuel_tank_pos_ctr1",0);
    ret = axis_input(device,2,&fval,0.0,1.0);
    if (fval < 0.5) {
     *fuel_pump_ctr_1_on = 1.0;
    } else {
     *fuel_pump_ctr_1_on = 0.0;
    }
    float *fuel_pump_ctr_2_on = link_dataref_flt("laminar/B738/fuel/fuel_tank_pos_ctr2",0);
    ret = axis_input(device,3,&fval,0.0,1.0);
    if (fval < 0.5) {
     *fuel_pump_ctr_2_on = 1.0;
    } else {
     *fuel_pump_ctr_2_on = 0.0;
    }

    float *fuel_temp = link_dataref_flt("laminar/B738/engine/fuel_temp",0);
    if (*servotest == 1) {
      ret = servos_output(device,4,&servoval,0.0,1.0,240,1000);
    } else {
      ret = servos_output(device,4,fuel_temp,-50.0,50.0,240,1000);
    }
      

    device = mastercard;
    card = 0;

    float *eng_valve_closed_1 = link_dataref_flt("laminar/B738/annunciator/eng1_valve_closed",-1);
    ival = 0;
    if (*eng_valve_closed_1 >= 0.025) ival = 2;
    if (*eng_valve_closed_1 == 1.0) ival = 1;
    if (*avionics_on == 0) ival =0;
    ret = mastercard_display(device,card,51,1,&ival,0);
    
    float *spar_valve_closed_1 = link_dataref_flt("laminar/B738/annunciator/spar1_valve_closed",-1);
    ival = 0;
    if (*spar_valve_closed_1 >= 0.025) ival = 2;
    if (*spar_valve_closed_1 == 1.0) ival = 1;
    if (*avionics_on == 0) ival =0;
    ret = mastercard_display(device,card,52,1,&ival,0);
    
    float *bypass_filter_1 = link_dataref_flt("laminar/B738/annunciator/bypass_filter_1",-1);
    ival = 0;
    if (*bypass_filter_1 >= 0.025) ival = 2;
    if (*bypass_filter_1 == 1.0) ival = 1;
    if (*avionics_on == 0) ival =0;
    ret = mastercard_display(device,card,53,1,&ival,0);
    
    float *eng_valve_closed_2 = link_dataref_flt("laminar/B738/annunciator/eng2_valve_closed",-1);
    ival = 0;
    if (*eng_valve_closed_2 >= 0.025) ival = 2;
    if (*eng_valve_closed_2 == 1.0) ival = 1;
    if (*avionics_on == 0) ival =0;
    ret = mastercard_display(device,card,54,1,&ival,0);
    
    float *spar_valve_closed_2 = link_dataref_flt("laminar/B738/annunciator/spar2_valve_closed",-1);
    ival = 0;
    if (*spar_valve_closed_2 >= 0.025) ival = 2;
    if (*spar_valve_closed_2 == 1.0) ival = 1;
    if (*avionics_on == 0) ival =0;
    ret = mastercard_display(device,card,55,1,&ival,0);
    
    float *bypass_filter_2 = link_dataref_flt("laminar/B738/annunciator/bypass_filter_2",-1);
    ival = 0;
    if (*bypass_filter_2 >= 0.025) ival = 2;
    if (*bypass_filter_2 == 1.0) ival = 1;
    if (*avionics_on == 0) ival =0;
    ret = mastercard_display(device,card,56,1,&ival,0);
    
    float *cross_feed_valve = link_dataref_flt("laminar/B738/annunciator/crossfeed",-1);
    ival = 0;
    if (*cross_feed_valve >= 0.025) ival = 2;
    if (*cross_feed_valve == 1.0) ival = 1;
    if (*avionics_on == 0) ival =0;
    ret = mastercard_display(device,card,57,1,&ival,0);
    
    float *low_press_ctr_1 = link_dataref_flt("laminar/B738/annunciator/low_fuel_press_c1",-1);
    ival = *low_press_ctr_1 >= 0.025;
    if (*avionics_on == 0) ival =0;
    ret = mastercard_display(device,card,58,1,&ival,0);
    float *low_press_ctr_2 = link_dataref_flt("laminar/B738/annunciator/low_fuel_press_c2",-1);
    ival = *low_press_ctr_2 >= 0.025;
    if (*avionics_on == 0) ival =0;
    ret = mastercard_display(device,card,59,1,&ival,0);
    float *low_press_aft_1 = link_dataref_flt("laminar/B738/annunciator/low_fuel_press_l1",-1);
    ival = *low_press_aft_1 >= 0.025;
    if (*avionics_on == 0) ival =0;
    ret = mastercard_display(device,card,60,1,&ival,0);
    float *low_press_fwd_1 = link_dataref_flt("laminar/B738/annunciator/low_fuel_press_l2",-1);
    ival = *low_press_fwd_1 >= 0.025;
    if (*avionics_on == 0) ival =0;
    ret = mastercard_display(device,card,61,1,&ival,0);
    float *low_press_fwd_2 = link_dataref_flt("laminar/B738/annunciator/low_fuel_press_r2",-1);
    ival = *low_press_fwd_2 >= 0.025;
    if (*avionics_on == 0) ival =0;
    ret = mastercard_display(device,card,62,1,&ival,0);
    float *low_press_aft_2 = link_dataref_flt("laminar/B738/annunciator/low_fuel_press_r1",-1);
    ival = *low_press_aft_2 >= 0.025;
    if (*avionics_on == 0) ival =0;
    ret = mastercard_display(device,card,63,1,&ival,0);

    
    /* ------------------ */
    /* NAV / DISP CONTROL */
    /* ------------------ */

    device = mastercard;
    card = 1;

    int *vhf_nav_source_left = link_dataref_cmd_once("laminar/B738/toggle_switch/vhf_nav_source_lft");
    int *vhf_nav_source_right = link_dataref_cmd_once("laminar/B738/toggle_switch/vhf_nav_source_rgt");
    float *vhf_nav_source = link_dataref_flt("laminar/B738/toggle_switch/vhf_nav_source",0);
    ret = digital_input(device,card,59,&ival,0);
    ret = digital_input(device,card,60,&ival2,0);
    vhf_nav_source_pos = 0.0;
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) vhf_nav_source_pos = (float) (ival - ival2);
    ret = set_state_updnf(&vhf_nav_source_pos,vhf_nav_source,vhf_nav_source_right,vhf_nav_source_left);
    if (ret != 0) {
      printf("VHF NAV SOURCE %f \n",vhf_nav_source_pos);
    }
    
    int *irs_source_left = link_dataref_cmd_once("laminar/B738/toggle_switch/irs_source_left");
    int *irs_source_right = link_dataref_cmd_once("laminar/B738/toggle_switch/irs_source_right");
    float *irs_source = link_dataref_flt("laminar/B738/toggle_switch/irs_source",0);
    ret = digital_input(device,card,61,&ival,0);
    ret = digital_input(device,card,62,&ival2,0);
    irs_source_pos = 0.0;
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) irs_source_pos = (float) (ival - ival2);
    ret = set_state_updnf(&irs_source_pos,irs_source,irs_source_right,irs_source_left);
    if (ret != 0) {
      printf("IRS SOURCE %f \n",irs_source_pos);
    }
    
    int *display_source_left = link_dataref_cmd_once("laminar/B738/toggle_switch/dspl_source_left");
    int *display_source_right = link_dataref_cmd_once("laminar/B738/toggle_switch/dspl_source_right");
    float *display_source = link_dataref_flt("laminar/B738/toggle_switch/dspl_source",0);
    ret = digital_input(device,card,63,&ival,0);
    ret = digital_input(device,card,64,&ival2,0);
    display_source_pos = 0.0;
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) display_source_pos = (float) (ival2 - ival);
    ret = set_state_updnf(&display_source_pos,display_source,display_source_right,display_source_left);
    if (ret != 0) {
      printf("DISPLAY SOURCE %f \n",display_source_pos);
    }
    
    int *control_panel_source_left = link_dataref_cmd_once("laminar/B738/toggle_switch/dspl_ctrl_pnl_left");
    int *control_panel_source_right = link_dataref_cmd_once("laminar/B738/toggle_switch/dspl_ctrl_pnl_right");
    float *control_panel_source = link_dataref_flt("laminar/B738/toggle_switch/dspl_ctrl_pnl",0);
    ret = digital_input(device,card,65,&ival,0);
    ret = digital_input(device,card,66,&ival2,0);
    control_panel_source_pos = 0.0;
    if ((ival != INT_MISS) && (ival2 != INT_MISS)) control_panel_source_pos = (float) (ival - ival2);
    ret = set_state_updnf(&control_panel_source_pos,control_panel_source,
			  control_panel_source_right,control_panel_source_left);
    if (ret != 0) {
      printf("CONTROL PANEL SOURCE %f \n",control_panel_source_pos);
    }
    
    /* ------------------ */
    /* ALT FLT CTRL Panel */
    /* ------------------ */

    device = mastercard;
    card = 1;
    int *alt_flaps_ctrl_up = link_dataref_cmd_hold("laminar/B738/toggle_switch/alt_flaps_ctrl_up");
    int *alt_flaps_ctrl_dn = link_dataref_cmd_hold("laminar/B738/toggle_switch/alt_flaps_ctrl_dn");
    ret = digital_input(device,card,54,alt_flaps_ctrl_dn,0);
    ret = digital_input(device,card,55,alt_flaps_ctrl_up,0);

    int *flt_ctrl_A_up = link_dataref_cmd_once("laminar/B738/toggle_switch/flt_ctr_A_up");
    int *flt_ctrl_A_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/flt_ctr_A_dn");
    float *flt_ctrl_A = link_dataref_flt("laminar/B738/switches/flt_ctr_A_pos",0);
    flt_ctrl_A_pos = 0.0;
    ret = digital_input(device,card,68,&ival,0);
    if (ival == 1) flt_ctrl_A_pos = 1.0;
    ret = digital_input(device,card,69,&ival,0);
    if (ival == 1) flt_ctrl_A_pos = -1.0;
    ret = set_state_updnf(&flt_ctrl_A_pos,flt_ctrl_A,flt_ctrl_A_dn,flt_ctrl_A_up);
    if (ret != 0) {
      printf("FLT CTRL A %f \n",flt_ctrl_A_pos);
    }

    int *flt_ctrl_B_up = link_dataref_cmd_once("laminar/B738/toggle_switch/flt_ctr_B_up");
    int *flt_ctrl_B_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/flt_ctr_B_dn");
    float *flt_ctrl_B = link_dataref_flt("laminar/B738/switches/flt_ctr_B_pos",0);
    flt_ctrl_B_pos = 0.0;
    ret = digital_input(device,card,70,&ival,0);
    if (ival == 1) flt_ctrl_B_pos = 1.0;
    ret = digital_input(device,card,71,&ival,0);
    if (ival == 1) flt_ctrl_B_pos = -1.0;
    ret = set_state_updnf(&flt_ctrl_B_pos,flt_ctrl_B,flt_ctrl_B_dn,flt_ctrl_B_up);
    if (ret != 0) {
      printf("FLT CTRL B %f \n",flt_ctrl_B_pos);
    }


    /* spillover digital inputs routed through analog inputs of servo card */
    device = servo1;
    
    int *alt_flaps_toggle = link_dataref_cmd_once("laminar/B738/toggle_switch/alt_flaps");
    float *alt_flaps = link_dataref_flt("laminar/B738/switches/alt_flaps_pos",0);
    float alt_flaps_pos = 0.0;
    ret = axis_input(device,0,&fval,0.0,1.0);
    if (fval < 0.5) alt_flaps_pos = 1.0;
    ret = set_state_togglef(&alt_flaps_pos,alt_flaps,alt_flaps_toggle);
    if (ret != 0) {
      printf("ALTERNATE FLAPS %f \n",alt_flaps_pos);
    }

    int *yaw_damper_toggle = link_dataref_cmd_once("laminar/B738/toggle_switch/yaw_dumper");
    float yaw_damper_pos = 0.0;
    ret = axis_input(device,1,&fval,0.0,1.0);
    if (fval < 0.5) yaw_damper_pos = 1.0;
    ret = set_state_togglef(&yaw_damper_pos,yaw_damper,yaw_damper_toggle);
    if (ret != 0) {
      printf("YAW DAMPER %f %f %i \n",yaw_damper_pos, *yaw_damper, *yaw_damper_toggle);
    }

    int *spoiler_A_toggle = link_dataref_cmd_once("laminar/B738/toggle_switch/spoiler_A");
    float *spoiler_A = link_dataref_flt("laminar/B738/switches/spoiler_A_pos",0);
    float spoiler_A_pos = 0.0;
    ret = axis_input(device,2,&fval,0.0,1.0);
    if (fval < 0.5) spoiler_A_pos = 1.0;
    ret = set_state_togglef(&spoiler_A_pos,spoiler_A,spoiler_A_toggle);
    if (ret != 0) {
      printf("SPOILER A %f \n",spoiler_A_pos);
    }

    int *spoiler_B_toggle = link_dataref_cmd_once("laminar/B738/toggle_switch/spoiler_B");
    float *spoiler_B = link_dataref_flt("laminar/B738/switches/spoiler_B_pos",0);
    float spoiler_B_pos = 0.0;
    ret = axis_input(device,3,&fval,0.0,1.0);
    if (fval < 0.5) spoiler_B_pos = 1.0;
    ret = set_state_togglef(&spoiler_B_pos,spoiler_B,spoiler_B_toggle);
    if (ret != 0) {
      printf("SPOILER B %f \n",spoiler_B_pos);
    }

    device = mastercard;
    card = 1;

    ival = 0;
    if ((*lights_test == 1.0) && (*battery == 1.0)) ival = 1;
    /* STDBY HYDRAULICS Hydraulic Quantity */
    ret = digital_output(device,card,35,&ival);
    /* STDBY HYDRAULICS Low Pressure */
    float *stby_hyd_press = link_dataref_flt("laminar/B738/annunciator/hyd_stdby_rud",-1);
    ret = digital_outputf(device,card,36,stby_hyd_press);
    /* STDBY HYDRAULICS Low Pressure */
    float *stby_rud_on = link_dataref_flt("laminar/B738/annunciator/std_rud_on",-1);
    ret = digital_outputf(device,card,37,stby_rud_on);
    /* FLT CONTROL A LOW PRESSURE */
    float *hyd_A_press = link_dataref_flt("laminar/B738/annunciator/hyd_A_rud",-1);
    ret = digital_outputf(device,card,38,hyd_A_press);
    /* FLT CONTROL B LOW PRESSURE */
    float *hyd_B_press = link_dataref_flt("laminar/B738/annunciator/hyd_B_rud",-1);
    ret = digital_outputf(device,card,39,hyd_B_press);
    /* FEEL DIFF PRESSURE */
    float *diff_press = link_dataref_flt("laminar/B738/annunciator/feel_diff_press",-1);
    ret = digital_outputf(device,card,40,diff_press);
    /* SPEED TRIM FAIL */
    ret = digital_output(device,card,41,&ival);
    /* MACH TRIM FAIL */
    ret = digital_output(device,card,42,&ival);
    /* AUTO SLAT FAIL */
    float *auto_slat_fail = link_dataref_flt("laminar/B738/annunciator/auto_slat_fail",-1);
    if (*avionics_on == 1) {
      ret = digital_outputf(device,card,43,auto_slat_fail);
    } else {
      ret = digital_output(device,card,43,&zero);
    }
    /* YAW DAMPER */
    float *yaw_damper_on = link_dataref_flt("laminar/B738/annunciator/yaw_damp",-1);
    ret = digital_outputf(device,card,44,yaw_damper_on);
   
  } else {
    /* A few basic switches for other ACF */
    device = mastercard;
    card = 0;
    
    int *battery_i = link_dataref_int("sim/cockpit/electrical/battery_on");
    int *avionics_i = link_dataref_int("sim/cockpit2/switches/avionics_power_on");
    ret = digital_input(device,card,8,&ival,0);
    if (ival != INT_MISS) {
      *battery_i = 1-ival;
      *avionics_i = 1-ival;
    }

    /* set relay 0 on output 49 on relay card through d-sub con */
    /* Backlighting Only when Battery is on */
    device = mastercard;
    card = 0;
    ret = digital_output(device,card,49,battery_i);
   
    /* Ignitior Switch for first engine */
    device = mastercard;
    card = 0;
    int *ignition_on = link_dataref_int_arr("sim/cockpit/engine/ignition_on", 16,-1);
    for (int i=0;i<8;i++) {
      ignition_on[i] = 3;
      ret = digital_input(device,card,27,&ival,0);
      if (ival == 1) ignition_on[i] = 2;
      ret = digital_input(device,card,28,&ival,0);
      if (ival == 1) ignition_on[i] = 1;
    }
    
    /* Engine Starters */
    device = mastercard;
    card = 0;
    int *engine_start = link_dataref_cmd_hold("sim/engines/engage_starters");
    ret = digital_input(device,card,18,engine_start,0);

    /* Blank Altitude Panel */
    device = mastercard;
    card = 0;
    ival = 10;
    ret = mastercard_display(device,card,0,5,&ival,1);
    ret = mastercard_display(device,card,6,5,&ival,1);
    
    /* Fuel Pump */
    device = mastercard;
    card = 1;
    int *fuel_pump_on = link_dataref_int_arr("sim/cockpit/engine/fuel_pump_on", 16,-1);
    ret = digital_input(device,card,67,&ival,0);
    if (ival != INT_MISS) {
      for (int i=0;i<8;i++) {
	fuel_pump_on[i] = ival;
      }
    }

    /* Turn off Voltage / Amperage Display */
    device = mastercard;
    card = 1;
    ival = 10;
    ret = mastercard_display(device,card,11,2,&ival,1);
    ret = mastercard_display(device,card,6,2,&ival,1);
    ret = mastercard_display(device,card,1,3,&ival,1);
    ret = mastercard_display(device,card,4,2,&ival,1);
    ret = mastercard_display(device,card,8,3,&ival,1);
  
  }
    
}

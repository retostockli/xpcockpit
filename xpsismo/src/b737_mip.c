/* This is the b737_mip.c code which connects to the SISMO MCP V4

   Copyright (C) 2021 Reto Stockli

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
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>

#include "common.h"
#include "libsismo.h"
#include "serverdata.h"
#include "b737_mip.h"

float capt_main_du_pos;
float capt_lower_du_pos;
float fo_main_du_pos;
float fo_lower_du_pos;
float lights_test_pos;
float spd_ref_pos;
float n1_set_source_pos;
float autobrake_pos;

void b737_mip(void)
{

  /* In the SISMO MIP all switches are inverted (0=on; 1=off) */
  
  int ret;
  int temp;
  int temp2;
  int card = 1;
  int one = 1;
  int zero = 0;

  int afds_test_1_capt;
  int afds_test_2_capt;
  int afds_test_1_fo;
  int afds_test_2_fo;

  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");

  /* switch on/off background lighting */
  ret = digital_output(card, 18, avionics_on); // CAPT AFDS
  ret = digital_output(card, 19, avionics_on); // BACKGROUND
  ret = digital_output(card, 20, avionics_on); // CAPT PANEL/EFIS/MCP
  ret = digital_output(card, 21, avionics_on); // CAPT AFDS
  ret = digital_output(card, 22, avionics_on); // BACKGROUND
  ret = digital_output(card, 23, avionics_on); // CAPT PANEL/EFIS/MCP
  ret = digital_output(card, 62, avionics_on); // FO PANEL
  ret = digital_output(card, 63, avionics_on); // FO PANEL

  // gear handle
  int *gear_handle_down;
  float *gear_handle_position;
  float *gear_lock_override;
  if ((acf_type == 2) || (acf_type == 3)) {
    gear_handle_down = link_dataref_int("xpserver/gear_handle");
    gear_handle_position = link_dataref_flt("laminar/B738/controls/gear_handle_down",-1);
    gear_lock_override = link_dataref_flt("laminar/B738/gear_lock_ovrd/position",0);
    *gear_lock_override = 1.0;
  } else if (acf_type == 1) {
    gear_handle_down = link_dataref_int("xpserver/gear_handle");
    gear_handle_position = link_dataref_flt("x737/systems/landinggear/landingGearLever",-1);
  } else {
    gear_handle_down = link_dataref_int("sim/cockpit2/controls/gear_handle_down");
    gear_handle_position = link_dataref_flt("xpserver/gear_handle_position",-1);
  }

  temp = (*gear_handle_position == 0.0);
  ret = digital_input(card,22,&temp,-1);
  if (temp == 1) {
    if (ret==1) printf("gear handle up \n");
    *gear_handle_down = 0;
    *gear_handle_position = 0.0;
  } else {
    temp = (*gear_handle_position == 1.0);
    ret = digital_input(card,23,&temp,-1);
    if (temp == 1) {
      if (ret==1) printf("gear handle down \n");
      *gear_handle_down = 1;
      *gear_handle_position = 1.0;
    } else {
      *gear_handle_position = 0.5;
    }
  }
  
  if ((acf_type == 2) || (acf_type == 3)) {

    //* Keep guarded covers open at all times
    int *guarded_covers = link_dataref_int("laminar/B738/guarded_covers");
    *guarded_covers = 1;

    float *ap_prst_warn_capt = link_dataref_flt("laminar/B738/annunciator/ap_warn1",0);
    float *ap_prst_disc_capt = link_dataref_flt("laminar/B738/annunciator/ap_disconnect1",0);
    float *ap_prst_warn_fo = link_dataref_flt("laminar/B738/annunciator/ap_warn2",0);
    float *ap_prst_disc_fo = link_dataref_flt("laminar/B738/annunciator/ap_disconnect2",0);
    float *at_prst_warn_capt = link_dataref_flt("laminar/B738/annunciator/at_fms_warn1",0);
    float *at_prst_disc_capt = link_dataref_flt("laminar/B738/annunciator/at_disconnect1",0);
    float *at_prst_warn_fo = link_dataref_flt("laminar/B738/annunciator/at_fms_warn2",0);
    float *at_prst_disc_fo = link_dataref_flt("laminar/B738/annunciator/at_disconnect2",0);
    float *fmc_prst = link_dataref_flt("laminar/B738/fmc/fmc_message_warn",0);

    float *speedbrake_armed = link_dataref_flt("laminar/B738/annunciator/speedbrake_armed",0);
    float *speedbrake_extend = link_dataref_flt("laminar/B738/annunciator/speedbrake_extend",0);
    float *stab_outoftrim = link_dataref_flt("laminar/B738/annunciator/stab_out_of_trim",0);

    float *sixpack_flt_cont = link_dataref_flt("laminar/B738/annunciator/six_pack_flt_cont",0);
    float *sixpack_elec = link_dataref_flt("laminar/B738/annunciator/six_pack_elec",0);
    float *sixpack_irs = link_dataref_flt("laminar/B738/annunciator/six_pack_irs",0);
    float *sixpack_apu = link_dataref_flt("laminar/B738/annunciator/six_pack_apu",0);
    float *sixpack_fuel = link_dataref_flt("laminar/B738/annunciator/six_pack_fuel",0);
    float *sixpack_ovht = link_dataref_flt("laminar/B738/annunciator/six_pack_fire",0);

    float *sixpack_anti_ice = link_dataref_flt("laminar/B738/annunciator/six_pack_ice",0);
    float *sixpack_eng = link_dataref_flt("laminar/B738/annunciator/six_pack_eng",0);
    float *sixpack_hyd = link_dataref_flt("laminar/B738/annunciator/six_pack_hyd",0);
    float *sixpack_ovhd = link_dataref_flt("laminar/B738/annunciator/six_pack_overhead",0);
    float *sixpack_doors = link_dataref_flt("laminar/B738/annunciator/six_pack_doors",0);
    float *sixpack_air_cond = link_dataref_flt("laminar/B738/annunciator/six_pack_air_cond",0);

    float *master_caution_light = link_dataref_flt("laminar/B738/annunciator/master_caution_light",0);
    float *fire_bell_light_capt = link_dataref_flt("laminar/B738/annunciator/fire_bell_annun",0);
    float *fire_bell_light_fo = link_dataref_flt("laminar/B738/annunciator/fire_bell_annun2",0);

    float *below_gs = link_dataref_flt("laminar/B738/annunciator/below_gs",0);
    float *gpws_inop = link_dataref_flt("laminar/B738/annunciator/gpws",0);

    float *nose_gear_transit = link_dataref_flt("laminar/B738/annunciator/nose_gear_transit",0);
    float *nose_gear_safe = link_dataref_flt("laminar/B738/annunciator/nose_gear_safe",0);
    float *left_gear_transit = link_dataref_flt("laminar/B738/annunciator/left_gear_transit",0);
    float *left_gear_safe = link_dataref_flt("laminar/B738/annunciator/left_gear_safe",0);
    float *right_gear_transit = link_dataref_flt("laminar/B738/annunciator/right_gear_transit",0);
    float *right_gear_safe = link_dataref_flt("laminar/B738/annunciator/right_gear_safe",0);

    float *auto_brake_disarm = link_dataref_flt("laminar/B738/annunciator/auto_brake_disarm",0);
    float *anti_skid_inop = link_dataref_flt("laminar/B738/annunciator/anti_skid_inop",0);
    float *flaps_transit = link_dataref_flt("laminar/B738/annunciator/slats_transit",0);
    float *flaps_extend = link_dataref_flt("laminar/B738/annunciator/slats_extend",0);

    float *flaps_position = link_dataref_flt_arr("laminar/B738/flap_indicator",2,-1,-1);
    float *brake_pressure = link_dataref_flt("laminar/B738/brake/brake_press",0);
    float *yaw_damper = link_dataref_flt("laminar/B738/yaw_damper",-2);

    int *ap_prst_button = link_dataref_cmd_hold("laminar/B738/push_button/ap_light_pilot");
    int *at_prst_button = link_dataref_cmd_hold("laminar/B738/push_button/at_light_pilot");
    int *fmc_prst_button = link_dataref_cmd_hold("laminar/B738/push_button/fms_light_pilot");

    float *nose_steer = link_dataref_flt("laminar/B738/switches/nose_steer_pos",0);
    int *nose_steer_norm = link_dataref_cmd_hold("laminar/B738/switch/nose_steer_norm");
    int *nose_steer_alt = link_dataref_cmd_hold("laminar/B738/switch/nose_steer_alt");

    float *capt_main_du = link_dataref_flt("laminar/B738/toggle_switch/main_pnl_du_capt",0);
    int *capt_main_du_left = link_dataref_cmd_once("laminar/B738/toggle_switch/main_pnl_du_capt_left");
    int *capt_main_du_right = link_dataref_cmd_once("laminar/B738/toggle_switch/main_pnl_du_capt_right");
    
    float *capt_lower_du = link_dataref_flt("laminar/B738/toggle_switch/lower_du_capt",0);
    int *capt_lower_du_left = link_dataref_cmd_once("laminar/B738/toggle_switch/lower_du_capt_left");
    int *capt_lower_du_right = link_dataref_cmd_once("laminar/B738/toggle_switch/lower_du_capt_right");
    
    float *fo_main_du = link_dataref_flt("laminar/B738/toggle_switch/main_pnl_du_fo",0);
    int *fo_main_du_left = link_dataref_cmd_once("laminar/B738/toggle_switch/main_pnl_du_fo_left");
    int *fo_main_du_right = link_dataref_cmd_once("laminar/B738/toggle_switch/main_pnl_du_fo_right");
    
    float *fo_lower_du = link_dataref_flt("laminar/B738/toggle_switch/lower_du_fo",0);
    int *fo_lower_du_left = link_dataref_cmd_once("laminar/B738/toggle_switch/lower_du_fo_left");
    int *fo_lower_du_right = link_dataref_cmd_once("laminar/B738/toggle_switch/lower_du_fo_right");
    int *recall = link_dataref_cmd_hold("laminar/B738/push_button/capt_six_pack");
    int *master_caution = link_dataref_cmd_hold("laminar/B738/push_button/master_caution1");
    int *fire_warn = link_dataref_cmd_hold("laminar/B738/push_button/fire_bell_light1");
    
    float *lights_test = link_dataref_flt("laminar/B738/toggle_switch/bright_test",0);
    int *lights_test_up = link_dataref_cmd_once("laminar/B738/toggle_switch/bright_test_up");
    int *lights_test_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/bright_test_dn");

    int *spd_ref_adjust_left = link_dataref_cmd_once("laminar/B738/toggle_switch/spd_ref_adjust_left");
    int *spd_ref_adjust_right = link_dataref_cmd_once("laminar/B738/toggle_switch/spd_ref_adjust_right");
    
    float *spd_ref = link_dataref_flt("laminar/B738/toggle_switch/spd_ref",0);
    int *spd_ref_left = link_dataref_cmd_once("laminar/B738/toggle_switch/spd_ref_left");
    int *spd_ref_right = link_dataref_cmd_once("laminar/B738/toggle_switch/spd_ref_right");

    int *mfd_eng = link_dataref_cmd_hold("laminar/B738/LDU_control/push_button/MFD_ENG");
    int *mfd_sys = link_dataref_cmd_hold("laminar/B738/LDU_control/push_button/MFD_SYS");

    int *gpws_test = link_dataref_cmd_hold("laminar/B738/push_button/gpws_test");
    float *gpws_flap_pos = link_dataref_flt("laminar/B738/toggle_switch/gpws_flap_pos",0);
    int *gpws_flap = link_dataref_cmd_once("laminar/B738/toggle_switch/gpws_flap");
    float *gpws_gear_pos = link_dataref_flt("laminar/B738/toggle_switch/gpws_gear_pos",0);
    int *gpws_gear = link_dataref_cmd_once("laminar/B738/toggle_switch/gpws_gear");
    float *gpws_terr_pos = link_dataref_flt("laminar/B738/toggle_switch/gpws_terr_pos",0);
    int *gpws_terr = link_dataref_cmd_once("laminar/B738/toggle_switch/gpws_terr");
     
    float *n1_set_adjust = link_dataref_flt("laminar/B738/toggle_switch/n1_set_adjust",-3);
    float *n1_set_source = link_dataref_flt("laminar/B738/toggle_switch/n1_set_source",0);
    int *n1_set_source_left = link_dataref_cmd_once("laminar/B738/toggle_switch/n1_set_source_left");
    int *n1_set_source_right = link_dataref_cmd_once("laminar/B738/toggle_switch/n1_set_source_right");
    
    float *autobrake = link_dataref_flt("laminar/B738/autobrake/autobrake_pos",0);
    int *autobrake_up = link_dataref_cmd_once("laminar/B738/knob/autobrake_up");
    int *autobrake_dn = link_dataref_cmd_once("laminar/B738/knob/autobrake_dn");
      
    int *fuel_flow_reset = link_dataref_cmd_hold("laminar/B738/toggle_switch/fuel_flow_up");
    int *fuel_flow_used = link_dataref_cmd_hold("laminar/B738/toggle_switch/fuel_flow_dn");

    int *isfd_baro_std = link_dataref_cmd_hold("laminar/B738/toggle_switch/standby_alt_baro_std");
    int *isfd_baro_rst = link_dataref_cmd_hold("laminar/B738/toggle_switch/standby_alt_rst");
    int *isfd_baro_app = link_dataref_cmd_hold("laminar/B738/toggle_switch/standby_alt_app");
    int *isfd_baro_hpin = link_dataref_cmd_hold("laminar/B738/toggle_switch/standby_alt_hpin");
    int *isfd_baro_plus = link_dataref_cmd_hold("laminar/B738/toggle_switch/standby_alt_plus");
    int *isfd_baro_minus = link_dataref_cmd_hold("laminar/B738/toggle_switch/standby_alt_minus");
    int *isfd_baro_up = link_dataref_cmd_once("laminar/B738/knob/standby_alt_baro_up");
    int *isfd_baro_dn = link_dataref_cmd_once("laminar/B738/knob/standby_alt_baro_dn");

    int *ptt = link_dataref_cmd_hold("xpilot/ptt");
    int *mic_capt = link_dataref_int("xpserver/mic_capt");
    int *mic_fo = link_dataref_int("xpserver/mic_fo");
    int *clock_capt = link_dataref_int("xpserver/clock_capt");
    int *clock_fo = link_dataref_int("xpserver/clock_fo");
    
    /* INPUTS */

    /* AFDS Lights Test */
    ret = digital_input(card,0,&afds_test_1_capt,-1);
    ret = digital_input(card,1,&afds_test_2_capt,-1);
    ret = digital_input(card,2,&afds_test_1_fo,-1);
    ret = digital_input(card,3,&afds_test_2_fo,-1);

    /* AFDS RESET: single button assignment for CAPT and FO */
    ret = digital_input(card,4,ap_prst_button,-1);
    ret = digital_input(card,5,at_prst_button,-1);
    ret = digital_input(card,6,fmc_prst_button,-1);

    /* Nose Wheel Alt/Norm */
    *nose_steer_alt = 0;
    *nose_steer_norm = 0;
    ret = digital_input(card,7,&temp,0);
    if ((temp == 0) && (*nose_steer == 1.0)) {
      *nose_steer_alt = 1;
    }
    if ((temp == 1) && (*nose_steer == 0.0)) {
      *nose_steer_norm = 1;
    }

    /* CAPT Main Display Selector */
    ret = digital_input(card,8,&temp,-1);
    if (temp == 1) capt_main_du_pos = -1.0; 
    ret = digital_input(card,9,&temp,-1);
    if (temp == 1) capt_main_du_pos = 0.0; 
    ret = digital_input(card,10,&temp,-1);
    if (temp == 1) capt_main_du_pos = 1.0; 
    ret = digital_input(card,11,&temp,-1);
    if (temp == 1) capt_main_du_pos = 2.0; 
    ret = digital_input(card,12,&temp,-1);
    if (temp == 1) capt_main_du_pos = 3.0; 
   
    ret = set_state_updnf(&capt_main_du_pos,capt_main_du,capt_main_du_right,capt_main_du_left);
    if (ret != 0) {
      printf("CAPT Main DU: %f %f %i %i \n",
	     capt_main_du_pos,*capt_main_du,*capt_main_du_right,*capt_main_du_left);
    }

    /* CAPT Lower Display Selector */
    ret = digital_input(card,13,&temp,-1);
    if (temp == 1) capt_lower_du_pos = -1.0; 
    ret = digital_input(card,14,&temp,-1);
    if (temp == 1) capt_lower_du_pos = 0.0; 
    ret = digital_input(card,15,&temp,-1);
    if (temp == 1) capt_lower_du_pos = 1.0; 
   
    ret = set_state_updnf(&capt_lower_du_pos,capt_lower_du,capt_lower_du_right,capt_lower_du_left);
    if (ret != 0) {
      printf("CAPT Lower DU: %f %f %i %i \n",
	     capt_lower_du_pos,*capt_lower_du,*capt_lower_du_right,*capt_lower_du_left);
    }

    
    /* Recall: sixpack pushbutton */
    ret = digital_input(card,16,recall,-1);

    /* fire warn pushbutton */
    ret = digital_input(card,17,fire_warn,-1);

    /* master caution pushbutton */
    ret = digital_input(card,18,master_caution,-1);

    /* lights test switch */
    ret = digital_input(card,19,&temp,-1);
    ret = digital_input(card,20,&temp2,-1);
    if (temp == 1) lights_test_pos = 1.0; 
    if (temp2 == 1) lights_test_pos = -1.0;
    if ((temp == 0) && (temp2 == 0)) lights_test_pos = 0.0;
    ret = set_state_updnf(&lights_test_pos,lights_test,lights_test_up,lights_test_dn);
    if (ret != 0) {
      printf("Lights Test: %f %f %i %i \n",
	     lights_test_pos,*lights_test,*lights_test_up,*lights_test_dn);
    }

    
    /* FO Main Display Selector */
    ret = digital_input(card,24,&temp,-1);
    if (temp == 1) fo_main_du_pos = 3.0; 
    ret = digital_input(card,25,&temp,-1);
    if (temp == 1) fo_main_du_pos = 2.0; 
    ret = digital_input(card,26,&temp,-1);
    if (temp == 1) fo_main_du_pos = 1.0; 
    ret = digital_input(card,27,&temp,-1);
    if (temp == 1) fo_main_du_pos = 0.0; 
    ret = digital_input(card,28,&temp,-1);
    if (temp == 1) fo_main_du_pos = -1.0; 
   
    ret = set_state_updnf(&fo_main_du_pos,fo_main_du,fo_main_du_left,fo_main_du_right);
    if (ret != 0) {
      printf("FO Main DU: %f %f %i %i \n",
	     fo_main_du_pos,*fo_main_du,*fo_main_du_right,*fo_main_du_left);
    }

    /* FO Lower Display Selector */
    ret = digital_input(card,29,&temp,-1);
    if (temp == 1) fo_lower_du_pos = 1.0; 
    ret = digital_input(card,30,&temp,-1);
    if (temp == 1) fo_lower_du_pos = 0.0; 
    ret = digital_input(card,31,&temp,-1);
    if (temp == 1) fo_lower_du_pos = -1.0; 
   
    ret = set_state_updnf(&fo_lower_du_pos,fo_lower_du,fo_lower_du_left,fo_lower_du_right);
    if (ret != 0) {
      printf("FO Lower DU: %f %f %i %i \n",
	     fo_lower_du_pos,*fo_lower_du,*fo_lower_du_right,*fo_lower_du_left);
    }

    /* SPD REF Encoder */
    temp = 0;
    ret = encoder_input(card, 32, 33, &temp, 1, 1);
    if (ret == 1) {
      /* ret is 1 only if encoder has been turned */
      printf("SPD REF: %i \n",temp);
      if (temp > 0) *spd_ref_adjust_right = 1;
      if (temp < 0) *spd_ref_adjust_left = 1;
    }

    /* SPD REF Rotary Switch */
    ret = digital_input(card,35,&temp,-1);
    if (temp == 1) spd_ref_pos = 6.0; 
    ret = digital_input(card,36,&temp,-1);
    if (temp == 1) spd_ref_pos = 5.0; 
    ret = digital_input(card,38,&temp,-1);
    if (temp == 1) spd_ref_pos = 4.0; 
    ret = digital_input(card,39,&temp,-1);
    if (temp == 1) spd_ref_pos = 3.0; 
    ret = digital_input(card,41,&temp,-1);
    if (temp == 1) spd_ref_pos = 2.0; 
    ret = digital_input(card,42,&temp,-1);
    if (temp == 1) spd_ref_pos = 1.0; 
    ret = digital_input(card,44,&temp,-1);
    if (temp == 1) spd_ref_pos = 0.0; 

    ret = set_state_updnf(&spd_ref_pos,spd_ref,spd_ref_right,spd_ref_left);
    if (ret != 0) {
      printf("SPD REF: %f %f %i %i \n",
	     spd_ref_pos,*spd_ref,*spd_ref_right,*spd_ref_left);
    }

    /* MFD Pushbuttons */
    ret = digital_input(card,37,mfd_eng,-1);
    ret = digital_input(card,53,mfd_sys,-1);

    /* GPWS */
    ret = digital_input(card,43,gpws_test,-1);

    ret = digital_input(card,45,&temp,-1);
    if ((temp == 1) && (*gpws_flap_pos == 0.0)) *gpws_flap = 1;
    if ((temp == 0) && (*gpws_flap_pos == 1.0)) *gpws_flap = 1;
      
    ret = digital_input(card,46,&temp,-1);
    if ((temp == 1) && (*gpws_gear_pos == 0.0)) *gpws_gear = 1;
    if ((temp == 0) && (*gpws_gear_pos == 1.0)) *gpws_gear = 1;
      
    ret = digital_input(card,47,&temp,-1);
    if ((temp == 1) && (*gpws_terr_pos == 0.0)) *gpws_terr = 1;
    if ((temp == 0) && (*gpws_terr_pos == 1.0)) *gpws_terr = 1;

    /* N1 Set Encoder */
    ret = encoder_inputf(card, 48, 49, n1_set_adjust, 0.001, 1);
    if (*n1_set_adjust != FLT_MISS) {
      if (*n1_set_adjust > 1.03) *n1_set_adjust = 1.03;
      if (*n1_set_adjust < 0.70) *n1_set_adjust = 0.70;
    }

    /* N1 Set Source Rotary Switch */
    ret = digital_input(card,51,&temp,-1);
    if (temp == 1) n1_set_source_pos = 1.0; 
    ret = digital_input(card,52,&temp,-1);
    if (temp == 1) n1_set_source_pos = 0.0; 
    ret = digital_input(card,54,&temp,-1);
    if (temp == 1) n1_set_source_pos = -1.0; 
    ret = digital_input(card,55,&temp,-1);
    if (temp == 1) n1_set_source_pos = -2.0; 

    ret = set_state_updnf(&n1_set_source_pos,n1_set_source,n1_set_source_right,n1_set_source_left);
    if (ret != 0) {
      printf("N1 Set Source: %f %f %i %i \n",
	     n1_set_source_pos,*n1_set_source,*n1_set_source_right,*n1_set_source_left);
    }

    /* Autobrake Rotary Switch */
    ret = digital_input(card,56,&temp,-1);
    if (temp == 1) autobrake_pos = 0.0; 
    ret = digital_input(card,57,&temp,-1);
    if (temp == 1) autobrake_pos = 1.0; 
    ret = digital_input(card,58,&temp,-1);
    if (temp == 1) autobrake_pos = 2.0; 
    ret = digital_input(card,59,&temp,-1);
    if (temp == 1) autobrake_pos = 3.0; 
    ret = digital_input(card,60,&temp,-1);
    if (temp == 1) autobrake_pos = 4.0; 
    ret = digital_input(card,61,&temp,-1);
    if (temp == 1) autobrake_pos = 5.0; 

    ret = set_state_updnf(&autobrake_pos,autobrake,autobrake_up,autobrake_dn);
    if (ret != 0) {
      printf("AUTOBRAKE: %f %f %i %i \n",
	     autobrake_pos,*autobrake,*autobrake_up,*autobrake_dn);
    }

    /* Fuel Flow Switch */
    ret = digital_input(card,62,fuel_flow_reset,-1);
    ret = digital_input(card,63,fuel_flow_used,-1);

    /* ISFD */
    ret = digital_input(card,64,isfd_baro_rst,-1);
    ret = digital_input(card,65,isfd_baro_minus,-1);
    ret = digital_input(card,66,isfd_baro_plus,-1);
    ret = digital_input(card,67,isfd_baro_app,-1);
    ret = digital_input(card,68,isfd_baro_hpin,-1);
    ret = digital_input(card,69,isfd_baro_std,-1);

    temp = 0;
    ret = encoder_input(card, 70, 71, &temp, -1, 1);
    if (ret == 1) {
      /* ret is 1 only if encoder has been turned */
      printf("STDBY BARO UP/DN: %i \n",temp);
      if (temp > 0) *isfd_baro_up = 1;
      if (temp < 0) *isfd_baro_dn = 1;
    }

    /* MIC Button */
    ret = digital_input(card,72,mic_capt,-1);
    if (ret == 1) {
      printf("MIC CAPTAIN\n");
    }
    ret = digital_input(card,80,mic_fo,-1);
    if (ret == 1) {
      printf("MIC COPILOT\n");
    }
 
    if ((*mic_capt == 1) || (*mic_fo == 1)) {
      *ptt = 1;
    } else {
      *ptt = 0;
    }
    
    /* Clock Button */
    ret = digital_input(card,73,clock_capt,-1);
    if (ret == 1) {
      printf("CLOCK CAPTAIN\n");
    }
     ret = digital_input(card,81,clock_fo,-1);
    if (ret == 1) {
      printf("CLOCK COPILOT\n");
    }
 

    
    /* ANALOG INPUTS */
    //    ret = analog_input(card,0,fvalue,0.0,1.0); /* CAPT OUTBD DU BRT */
    //    ret = analog_input(card,1,fvalue,0.0,1.0); /* CAPT INBD DU BRT */
    //    ret = analog_input(card,2,fvalue,0.0,1.0); /* CAPT UPPER DU BRT */
    //    ret = analog_input(card,3,fvalue,0.0,1.0); /* FO OUTBD DU BRT */
    //    ret = analog_input(card,4,fvalue,0.0,1.0); /* FO INBD DU BRT */
    
    
    
    /* OUTPUTS */
    if ((afds_test_1_capt == 1) || (afds_test_2_capt == 1)) {
      if (afds_test_1_capt == 1) {
	ret = digital_output(card, 0, &one);   // yellow
	ret = digital_output(card, 2, &one);   // yellow
      } else {
	ret = digital_output(card, 1, &one);   // red
	ret = digital_output(card, 3, &one);   // red
      }
      ret = digital_output(card, 4, &one);  // yellow
    } else {
      ret = digital_outputf(card, 0, ap_prst_warn_capt);   // yellow
      ret = digital_outputf(card, 2, at_prst_warn_capt);   // yellow
      ret = digital_outputf(card, 1, ap_prst_disc_capt);   // red
      ret = digital_outputf(card, 3, at_prst_disc_capt);   // red
      ret = digital_outputf(card, 4, fmc_prst);  // yellow
    }
      
    ret = digital_outputf(card, 5, speedbrake_armed);
    ret = digital_outputf(card, 6, lights_test);   // speed brake do not arm
    ret = digital_outputf(card, 7, stab_outoftrim);

    ret = digital_outputf(card, 8, sixpack_flt_cont);
    ret = digital_outputf(card, 9, sixpack_elec);
    ret = digital_outputf(card, 10, sixpack_irs);
    ret = digital_outputf(card, 11, sixpack_apu);
    ret = digital_outputf(card, 12, sixpack_fuel);
    ret = digital_outputf(card, 13, sixpack_ovht);

    ret = digital_outputf(card, 14, master_caution_light);
    ret = digital_outputf(card, 15, fire_bell_light_capt);

    ret = digital_outputf(card, 16, below_gs);

    if ((afds_test_1_fo == 1) || (afds_test_2_fo == 1)) {
      if (afds_test_1_fo == 1) {
	ret = digital_output(card, 32, &one);   // yellow
	ret = digital_output(card, 34, &one);   // yellow
      } else {
	ret = digital_output(card, 33, &one);   // red
	ret = digital_output(card, 35, &one);   // red
      }
      ret = digital_output(card, 36, &one);  // yellow
    } else {
      ret = digital_outputf(card, 32, ap_prst_warn_fo);   // yellow
      ret = digital_outputf(card, 34, at_prst_warn_fo);  // yellow
      ret = digital_outputf(card, 33, ap_prst_disc_fo);   // red
      ret = digital_outputf(card, 35, at_prst_disc_fo);  // red
      ret = digital_outputf(card, 36, fmc_prst); // yellow
    }
      
    ret = digital_outputf(card, 37, speedbrake_extend);
    ret = digital_outputf(card, 38, below_gs);
    ret = digital_outputf(card, 39, gpws_inop);
    
    ret = digital_outputf(card, 40, sixpack_anti_ice);
    ret = digital_outputf(card, 41, sixpack_eng);
    ret = digital_outputf(card, 42, sixpack_hyd);
    ret = digital_outputf(card, 43, sixpack_ovhd);
    ret = digital_outputf(card, 44, sixpack_doors);
    ret = digital_outputf(card, 45, sixpack_air_cond);

    ret = digital_outputf(card, 46, master_caution_light);
    ret = digital_outputf(card, 47, fire_bell_light_fo);

    ret = digital_outputf(card, 48, right_gear_transit);
    ret = digital_outputf(card, 49, right_gear_safe);
    ret = digital_outputf(card, 50, left_gear_transit);
    ret = digital_outputf(card, 51, left_gear_safe);
    ret = digital_outputf(card, 52, nose_gear_transit);
    ret = digital_outputf(card, 53, nose_gear_safe);

    ret = digital_outputf(card, 56, auto_brake_disarm);
    ret = digital_outputf(card, 57, anti_skid_inop);

    ret = digital_outputf(card, 58, flaps_transit);
    ret = digital_outputf(card, 59, flaps_extend);


    
    /* SERVOS */
    ret = servo_outputf(card,0,flaps_position,-7.0,60.0);
    ret = servo_outputf(card,1,brake_pressure,-850.0,5300.0);
    
  }
    
}

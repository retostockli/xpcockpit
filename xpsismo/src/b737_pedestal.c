/* This is the b737_pedestal.c code which connects the Boeing 737 center pedestal via the SISMO Ethernet Card
   to X-Plane

   Copyright (C) 2023 Reto Stockli

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
#include "b737_pedestal.h"

#define max(A,B) ((A)>(B) ? (A) : (B)) 
#define min(A,B) ((A)<(B) ? (A) : (B)) 

/* Common Variables */
int com1_tfr_button;
int com1_test_button;

int com2_tfr_button;
int com2_test_button;

int nav1_tfr_button;
int nav1_test_button;

int nav2_tfr_button;
int nav2_test_button;

int adf1_tfr_button;
int adf1_ant_switch;
int adf1_tone_switch;

int adf2_tfr_button;
int adf2_ant_switch;
int adf2_tone_switch;


int acp1_micsel_vhf1;
int acp1_micsel_vhf2;
int acp1_micsel_vhf3;
int acp1_micsel_hf1;
int acp1_micsel_hf2;
int acp1_micsel_flt;
int acp1_micsel_svc;
int acp1_micsel_pa;
int acp1_rt_ic;
int acp1_mask_boom;
int acp1_sel_v;
int acp1_sel_b;
int acp1_sel_r;
int acp1_sel_vbr;
int acp1_alt_norm;

float acp1_vol_vhf1;
float acp1_vol_vhf2;
float acp1_vol_pa;
float acp1_vol_mkr;
float acp1_vol_spkr;


int acp2_micsel_vhf1;
int acp2_micsel_vhf2;
int acp2_micsel_vhf3;
int acp2_micsel_hf1;
int acp2_micsel_hf2;
int acp2_micsel_flt;
int acp2_micsel_svc;
int acp2_micsel_pa;
int acp2_rt_ic;
int acp2_mask_boom;
int acp2_sel_v;
int acp2_sel_b;
int acp2_sel_r;
int acp2_sel_vbr;
int acp2_alt_norm;

float acp2_vol_vhf1;
float acp2_vol_vhf2;
float acp2_vol_pa;
float acp2_vol_mkr;
float acp2_vol_spkr;

int xpndr_sel_switch;
int xpndr_src_switch;
int xpndr_mode_select;

int cargofire_test;
int cargofire_select_fwd_a;
int cargofire_select_fwd_b;
int cargofire_select_aft_a;
int cargofire_select_aft_b;
int cargofire_arm_fwd;
int cargofire_arm_aft;
int cargofire_disch;

int cargofire_fault;
int cargofire_test_fwd;
int cargofire_test_aft;

int wxr_mode_test;
int wxr_mode_wx;
int wxr_mode_turb;

int stab_trim_ovrd;
int flt_deck_door_unlock;
int flt_deck_door_auto;
int flt_deck_door_deny;
int flt_deck_door_pos;

int fire_eng1_ovht_det_a; 
int fire_eng1_ovht_det_b; 
int fire_eng2_ovht_det_a;
int fire_eng2_ovht_det_b;

float fire_eng1_pulled_input;
float fire_eng1_rotate_l_input;
float fire_eng1_rotate_r_input;
float fire_apu_pulled_input;
float fire_apu_rotate_l_input;
float fire_apu_rotate_r_input;
float fire_eng2_pulled_input;
float fire_eng2_rotate_l_input;
float fire_eng2_rotate_r_input;
int fire_bell_cutout;

void b737_pedestal(void)
{

  /* local variables */
  int card = 2; /* SISMO card according to configuration */
  
  int ret;
  int updn;
  int dp;
  int one=1;
  int temp;
  int temp2;
  int integer; /* integer part of displays */
  int decimal; /* decimal part of displays */
  int lasttwo; /* last two digits of displays */
  int i0; /* first input # per panel */
  int o0; /* first output # per panel */
  int d0; /* first display # per panel */
  int a0; /* first analog # per panel */

  /* parameters */
  /* X-Plane frequencies are scaled by 10 kHz except for adf and dme */
  int nav_min = 108; // MHz
  int nav_max = 117; // MHz
  int com_min = 118; // MHz
  int com_max = 136; // MHz
  int adf_min = 190; // kHz
  int adf_max = 1750; // kHz
  int transponder_min = 0;
  int transponder_max = 7;

  float trim_min = -1.0;
  float trim_max = 1.0;

  float flood_brightness;



  /* x-plane data */
  int *nav1_freq_active = link_dataref_int("sim/cockpit/radios/nav1_freq_hz");
  int *nav2_freq_active = link_dataref_int("sim/cockpit/radios/nav2_freq_hz");
  int *com1_freq_active = link_dataref_int("sim/cockpit2/radios/actuators/com1_frequency_hz_833");
  int *com2_freq_active = link_dataref_int("sim/cockpit2/radios/actuators/com2_frequency_hz_833");
  int *adf1_freq_active = link_dataref_int("sim/cockpit/radios/adf1_freq_hz");
  int *adf2_freq_active = link_dataref_int("sim/cockpit/radios/adf2_freq_hz");
  int *nav1_freq_stdby = link_dataref_int("sim/cockpit/radios/nav1_stdby_freq_hz");
  int *nav2_freq_stdby = link_dataref_int("sim/cockpit/radios/nav2_stdby_freq_hz");
  int *com1_freq_stdby = link_dataref_int("sim/cockpit2/radios/actuators/com1_standby_frequency_hz_833");
  int *com2_freq_stdby = link_dataref_int("sim/cockpit2/radios/actuators/com2_standby_frequency_hz_833");
  int *adf1_freq_stdby = link_dataref_int("sim/cockpit/radios/adf1_stdby_freq_hz");
  int *adf2_freq_stdby = link_dataref_int("sim/cockpit/radios/adf2_stdby_freq_hz"); 

  int *com1_fine_up = link_dataref_cmd_once("sim/radios/stby_com1_fine_up_833");
  int *com1_fine_dn = link_dataref_cmd_once("sim/radios/stby_com1_fine_down_833");
  int *com1_coarse_up = link_dataref_cmd_once("sim/radios/stby_com1_coarse_up_833");
  int *com1_coarse_dn = link_dataref_cmd_once("sim/radios/stby_com1_coarse_down_833");
  int *com2_fine_up = link_dataref_cmd_once("sim/radios/stby_com2_fine_up_833");
  int *com2_fine_dn = link_dataref_cmd_once("sim/radios/stby_com2_fine_down_833");
  int *com2_coarse_up = link_dataref_cmd_once("sim/radios/stby_com2_coarse_up_833");
  int *com2_coarse_dn = link_dataref_cmd_once("sim/radios/stby_com2_coarse_down_833");
  int *nav1_fine_up = link_dataref_cmd_once("sim/radios/stby_nav1_fine_up");
  int *nav1_fine_dn = link_dataref_cmd_once("sim/radios/stby_nav1_fine_down");
  int *nav1_coarse_up = link_dataref_cmd_once("sim/radios/stby_nav1_coarse_up");
  int *nav1_coarse_dn = link_dataref_cmd_once("sim/radios/stby_nav1_coarse_down");
  int *nav2_fine_up = link_dataref_cmd_once("sim/radios/stby_nav2_fine_up");
  int *nav2_fine_dn = link_dataref_cmd_once("sim/radios/stby_nav2_fine_down");
  int *nav2_coarse_up = link_dataref_cmd_once("sim/radios/stby_nav2_coarse_up");
  int *nav2_coarse_dn = link_dataref_cmd_once("sim/radios/stby_nav2_coarse_down");
  int *adf1_fine_up = link_dataref_cmd_once("sim/radios/stby_adf1_tens_up");
  int *adf1_fine_dn = link_dataref_cmd_once("sim/radios/stby_adf1_tens_down");
  int *adf1_coarse_up = link_dataref_cmd_once("sim/radios/stby_adf1_hundreds_thous_up");
  int *adf1_coarse_dn = link_dataref_cmd_once("sim/radios/stby_adf1_hundreds_thous_down");
  int *adf2_fine_up = link_dataref_cmd_once("sim/radios/stby_adf2_tens_up");
  int *adf2_fine_dn = link_dataref_cmd_once("sim/radios/stby_adf2_tens_down");
  int *adf2_coarse_up = link_dataref_cmd_once("sim/radios/stby_adf2_hundreds_thous_up");
  int *adf2_coarse_dn = link_dataref_cmd_once("sim/radios/stby_adf2_hundreds_thous_down");

  
  int *transponder_code = link_dataref_int("sim/cockpit/radios/transponder_code");
  int *transponder_fail = link_dataref_int("sim/operation/failures/rel_xpndr");
  
  int *transponder_ident;
  float *transponder_mode_f;
  int *transponder_mode_up;
  int *transponder_mode_dn;
  if ((acf_type == 2) || (acf_type == 3)) {
    transponder_mode_f = link_dataref_flt("laminar/B738/knob/transponder_pos",0);
    transponder_mode_up = link_dataref_cmd_once("laminar/B738/knob/transponder_mode_up");
    transponder_mode_dn = link_dataref_cmd_once("laminar/B738/knob/transponder_mode_dn");
    transponder_ident = link_dataref_cmd_once("laminar/B738/push_button/transponder_ident_dn");
  } else {
    transponder_ident = link_dataref_cmd_once("sim/transponder/transponder_ident");
  }
  int *transponder_mode = link_dataref_int("sim/cockpit/radios/transponder_mode");
    
  float *rudder_trim = link_dataref_flt("sim/flightmodel/controls/rud_trim",-3);
  int *rudder_trim_left = link_dataref_cmd_hold("sim/flight_controls/rudder_trim_left");
  int *rudder_trim_right = link_dataref_cmd_hold("sim/flight_controls/rudder_trim_right");
  
  //  float *aileron_trim = link_dataref_flt("sim/flightmodel/controls/ail_trim",-4);
  int *aileron_trim_left = link_dataref_cmd_hold("sim/flight_controls/aileron_trim_left");
  int *aileron_trim_right = link_dataref_cmd_hold("sim/flight_controls/aileron_trim_right");

  /* FIRE SWITCHES ETC */
  //  int *fire_test; 
  int *fire_test_ovht; 
  int *fire_test_fault; 
  //  int *fire_eng_ext_test;
  int *fire_eng_ext_test_left;
  int *fire_eng_ext_test_right;
  float *fire_eng1_ovht_ann;
  float *fire_eng2_ovht_ann;
  float *fire_wheelwell_ann;
  float *fire_fault_ann;
  float *fire_apu_det_inop_ann;
  float *fire_apu_bottle_disch_ann;
  float *fire_bottle_l_disch_ann;
  float *fire_bottle_r_disch_ann;
  float *fire_eng1_ann;
  float *fire_apu_ann;
  float *fire_eng2_ann;
  float *fire_eng1_test_ann;
  float *fire_apu_test_ann;
  float *fire_eng2_test_ann;
  int *fire_eng1_pulled;
  //  int *fire_eng1_rotate; 
  int *fire_eng1_rotate_l; 
  int *fire_eng1_rotate_r; 
  int *fire_apu_pulled;
  //  int *fire_apu_rotate; 
  int *fire_apu_rotate_l; 
  int *fire_apu_rotate_r; 
  int *fire_eng2_pulled;
  //  int *fire_eng2_rotate;
  int *fire_eng2_rotate_l;
  int *fire_eng2_rotate_r;
  float *fire_eng1_pulled_status;
  float *fire_eng1_rotate_status; 
  float *fire_apu_pulled_status;
  float *fire_apu_rotate_status; 
  float *fire_eng2_pulled_status;
  float *fire_eng2_rotate_status;
  if ((acf_type == 2) || (acf_type == 3)) {
    fire_test_fault = link_dataref_cmd_hold("laminar/B738/toggle_switch/fire_test_lft"); 
    fire_test_ovht = link_dataref_cmd_hold("laminar/B738/toggle_switch/fire_test_rgt"); 
    fire_eng_ext_test_left = link_dataref_cmd_hold("laminar/B738/toggle_switch/exting_test_lft"); 
    fire_eng_ext_test_right = link_dataref_cmd_hold("laminar/B738/toggle_switch/exting_test_rgt"); 

    fire_eng1_ovht_ann = link_dataref_flt("laminar/B738/annunciator/engine1_ovht",-1);
    fire_eng2_ovht_ann = link_dataref_flt("laminar/B738/annunciator/engine2_ovht",-1);
    fire_wheelwell_ann = link_dataref_flt("laminar/B738/annunciator/wheel_well_fire",-1);
    fire_fault_ann = link_dataref_flt("laminar/B738/annunciator/fire_fault_inop",-1);
    fire_apu_det_inop_ann = link_dataref_flt("laminar/B738/annunciator/fire_fault_inop",-1); // PLEASE REPLACE!!!
    fire_apu_bottle_disch_ann = link_dataref_flt("laminar/B738/annunciator/apu_bottle_discharge",-1);
    fire_bottle_l_disch_ann = link_dataref_flt("laminar/B738/annunciator/l_bottle_discharge",-1);
    fire_bottle_r_disch_ann = link_dataref_flt("laminar/B738/annunciator/r_bottle_discharge",-1);
    fire_eng1_ann = link_dataref_flt("laminar/B738/annunciator/engine1_fire",-1);
    fire_apu_ann = link_dataref_flt("laminar/B738/annunciator/apu_fire",-1);
    fire_eng2_ann = link_dataref_flt("laminar/B738/annunciator/engine2_fire",-1);
    fire_eng1_test_ann = link_dataref_flt("laminar/B738/annunciator/extinguisher_circuit_annun_left",-1);
    fire_apu_test_ann = link_dataref_flt("laminar/B738/annunciator/extinguisher_circuit_annun_apu",-1);
    fire_eng2_test_ann = link_dataref_flt("laminar/B738/annunciator/extinguisher_circuit_annun_right",-1);
   

    fire_eng1_pulled = link_dataref_cmd_once("laminar/B738/fire/engine01/ext_switch_arm"); 
    fire_eng1_rotate_l = link_dataref_cmd_once("laminar/B738/fire/engine01/ext_switch_L");
    fire_eng1_rotate_r = link_dataref_cmd_once("laminar/B738/fire/engine01/ext_switch_R");
    fire_apu_pulled = link_dataref_cmd_once("laminar/B738/fire/apu/ext_switch_arm"); 
    fire_apu_rotate_l = link_dataref_cmd_once("laminar/B738/fire/apu/ext_switch_L"); 
    fire_apu_rotate_r = link_dataref_cmd_once("laminar/B738/fire/apu/ext_switch_R"); 
    fire_eng2_pulled = link_dataref_cmd_once("laminar/B738/fire/engine02/ext_switch_arm"); 
    fire_eng2_rotate_l = link_dataref_cmd_once("laminar/B738/fire/engine02/ext_switch_L"); 
    fire_eng2_rotate_r = link_dataref_cmd_once("laminar/B738/fire/engine02/ext_switch_R"); 
    
    fire_eng1_pulled_status = link_dataref_flt("laminar/B738/fire/engine01/ext_switch/pos_arm",-2); 
    fire_eng1_rotate_status = link_dataref_flt("laminar/B738/fire/engine01/ext_switch/pos_disch",-2);
    fire_apu_pulled_status = link_dataref_flt("laminar/B738/fire/apu/ext_switch/pos_arm",-2); 
    fire_apu_rotate_status = link_dataref_flt("laminar/B738/fire/apu/ext_switch/pos_disch",-2);
    fire_eng2_pulled_status = link_dataref_flt("laminar/B738/fire/engine02/ext_switch/pos_arm",-2); 
    fire_eng2_rotate_status = link_dataref_flt("laminar/B738/fire/engine02/ext_switch/pos_disch",-2);
   
  } else if (acf_type == 1) {
    //    fire_test = link_dataref_int("x737/cockpit/fireSupPanel/fireWarnTestSw_state"); // 0: fault, 1: off, 2: ovht
    //    fire_eng_ext_test = link_dataref_int("x737/cockpit/fireSupPanel/fireExtTestSw_state"); //  0: '1', 1: center, 2: '2'
    fire_eng1_pulled = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle1Pulled_state"); // 0: down, 1: pulled
    //    fire_eng1_rotate = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle1Rotate_state"); // -1: 'L', 0: center, 1: 'R'
    fire_apu_pulled = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle2Pulled_state"); // 0: down, 1: pulled
    //    fire_apu_rotate = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle2Rotate_state"); // -1: 'L', 0: center, 1: 'R'
    fire_eng2_pulled = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle3Pulled_state"); // 0: down, 1: pulled
    //    fire_eng2_rotate = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle3Rotate_state"); // -1: 'L', 0: center, 1: 'R'
    
    fire_eng1_ovht_ann = link_dataref_flt("x737/cockpit/fireSupPanel/Eng1OvhtDet_annunc",-1);
    fire_eng2_ovht_ann = link_dataref_flt("x737/cockpit/fireSupPanel/Eng2OvhtDet_annunc",-1);
    fire_wheelwell_ann = link_dataref_flt("x737/cockpit/fireSupPanel/wheelWell_annunc",-1);
    fire_fault_ann = link_dataref_flt("x737/cockpit/fireSupPanel/fault_annunc",-1);
    fire_apu_det_inop_ann = link_dataref_flt("x737/cockpit/fireSupPanel/APUDetInop_annunc",-1);
    fire_apu_bottle_disch_ann = link_dataref_flt("x737/cockpit/fireSupPanel/APUBotDisch_annunc",-1);
    fire_bottle_l_disch_ann = link_dataref_flt("x737/cockpit/fireSupPanel/bottle1Disch_annunc",-1);
    fire_bottle_r_disch_ann = link_dataref_flt("x737/cockpit/fireSupPanel/bottle2Disch_annunc",-1);
    fire_eng1_ann = link_dataref_flt("x737/cockpit/fireSupPanel/fireHandle1_annunc",-1);
    fire_apu_ann = link_dataref_flt("x737/cockpit/fireSupPanel/fireHandle2_annunc",-1);
    fire_eng2_ann = link_dataref_flt("x737/cockpit/fireSupPanel/fireHandle3_annunc",-1);
    fire_eng1_test_ann = link_dataref_flt("x737/cockpit/fireSupPanel/fireExtTest1Btn_annunc",-1);
    fire_apu_test_ann = link_dataref_flt("x737/cockpit/fireSupPanel/fireExtTest2Btn_annunc",-1);
    fire_eng2_test_ann = link_dataref_flt("x737/cockpit/fireSupPanel/fireExtTest3Btn_annunc",-1);
  }
  //  int *engine_fires = link_dataref_int_arr("sim/cockpit2/annunciators/engine_fires",8,-1);



  /* TEMPORARY ASSIGNMENT TO BETTERPUSHBACK */
  int *vhf1 = link_dataref_cmd_once("sim/ground_ops/pushback_left");
  int *vhf2 = link_dataref_cmd_once("sim/ground_ops/pushback_straight");
  int *vhf3 = link_dataref_cmd_once("sim/ground_ops/pushback_right");
  int *hf1 = link_dataref_cmd_once("sim/ground_ops/pushback_stop");
  int *hf2 = link_dataref_int("xpserver/hf2");

  /* WXR Gain and Tilt */
  float *wxr_gain = link_dataref_flt("xpserver/wxr_gain",-2);
  float *wxr_tilt = link_dataref_flt("xpserver/wxr_tilt",-2);
  
  //  float *flt_deck_door;
  //  int *flt_deck_door_left;
  //  int *flt_deck_door_right;
  float *lock_fail;
  float *auto_unlk;
  if ((acf_type == 2) || (acf_type == 3)) {
    //    flt_deck_door = link_dataref_flt("laminar/B738/toggle_switch/flt_dk_door",0);
    //    flt_deck_door_left = link_dataref_cmd_once("laminar/B738/toggle_switch/flt_dk_door_left");
    //    flt_deck_door_right = link_dataref_cmd_once("laminar/B738/toggle_switch/flt_dk_door_right");
    lock_fail = link_dataref_flt("laminar/B738/annunciator/door_lock_fail",-1);
    auto_unlk = link_dataref_flt("laminar/B738/annunciator/door_auto_unlk",-1);
  } else {
    //    flt_deck_door = link_dataref_flt("xpserver/flt_dk_door",0);
    //    flt_deck_door_left = link_dataref_int("xpserver/flt_dk_door_left");
    //    flt_deck_door_right = link_dataref_int("xpserver/flt_dk_door_right");    
    lock_fail = link_dataref_flt("xpserver/lock_fail",0);
    auto_unlk = link_dataref_flt("xpserver/auto_unlk",0);
  }

  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");

  float *lights_test;
  if ((acf_type == 2) || (acf_type == 3)) {
    lights_test = link_dataref_flt("laminar/B738/toggle_switch/bright_test",0);
  } else {
    lights_test = link_dataref_flt("xpserver/lights_test",0);
  }
    
  /*** Background Lighting ***/

  /* blank displays if avionics are off */
  int display_brightness = 10;
  if (*avionics_on != 1) display_brightness = 0;

  /* turn off background lighting if avionics are off */
  o0 = 0;
  ret = digital_output(card,o0+24+6,avionics_on);


  
  /*** COM1 Panel ***/ 
  i0 = 64;
  d0 = 32; 
  
  /* COM1 tfr button */
  ret = digital_input(card,i0+0,&com1_tfr_button,0);
  if (ret == 1) {
    printf("COM1 TFR Button: %i \n",com1_tfr_button);
    if (com1_tfr_button == 1) {
      temp = *com1_freq_active;
      *com1_freq_active = *com1_freq_stdby;
      *com1_freq_stdby = temp;
    }
  }
  /* COM1 Test Button */
  ret = digital_input(card,i0+1,&com1_test_button,0);
  if (ret == 1) {
    printf("COM1 TEST Button: %i \n",com1_test_button);
  } 
  /* COM1 Outer Encoder (1 MHz step) */
  updn = 0;
  ret = encoder_input(card,i0+2,i0+3,&updn,1,1);
  if (ret == 1) {
    if (updn >= 1) {
      *com1_coarse_up = 1;
    } else {
      *com1_coarse_dn = 1;
    }
  }
  /* COM1 Inner Encoder (5 kHz step) */
  updn = 0;
  ret = encoder_input(card,i0+4,i0+5,&updn,5,1);
  if (ret == 1) {
    if (*com1_freq_stdby != INT_MISS) {
      integer = *com1_freq_stdby / 1000;
      decimal = *com1_freq_stdby - integer * 1000;
      decimal += updn;
      lasttwo = decimal - decimal/100*100;
      /* 8.33 kHz frequencies do not cover all 5kHz display steps */
      if ((lasttwo == 20) || (lasttwo == 45) || (lasttwo == 70) || (lasttwo == 95)) {
	if (updn > 0 ) decimal += 5;
	if (updn < 0 ) decimal -= 5;
      }
      if (decimal < 0) decimal = 990;
      if (decimal > 990) decimal = 0;
      *com1_freq_stdby = integer * 1000 + decimal;
      printf("COM1 STDBY FREQ: %i \n",*com1_freq_stdby);
    }
  }
  /* Does Not Work at High Turn Speeds
  ret = encoder_input(card,i0+4,i0+5,&updn,1,1);
  if (ret == 1) {
    if (updn >= 1) {
      printf("COM1 UP %i \n",*com1_fine_up);
      *com1_fine_up = 1;
    } else {
      *com1_fine_dn = 1;
      printf("COM1 DN %i \n",*com1_fine_dn);
    }
  }
  */
  
  /* COM1 Displays */
  ret = display_output(card, d0+0, 6, com1_freq_active, 3, display_brightness);
  ret = display_output(card, d0+8, 6, com1_freq_stdby, 3, display_brightness);



  
  /*** COM2 Panel ***/ 
  i0 = 128;
  d0 = 64; 
  
  /* COM2 tfr button */
  ret = digital_input(card,i0+0,&com2_tfr_button,0);
  if (ret == 1) {
    printf("COM2 TFR Button: %i \n",com2_tfr_button);
    if (com2_tfr_button == 1) {
      temp = *com2_freq_active;
      *com2_freq_active = *com2_freq_stdby;
      *com2_freq_stdby = temp;
    }
  }
  /* COM2 Test Button */
  ret = digital_input(card,i0+1,&com2_test_button,0);
  if (ret == 1) {
    printf("COM2 TEST Button: %i \n",com2_test_button);
  }
  /* COM2 Outer Encoder (1 MHz step) */
  updn = 0;
  ret = encoder_input(card,i0+2,i0+3,&updn,1,1);
  if (ret == 1) {
    if (updn >= 1) {
      *com2_coarse_up = 1;
    } else {
      *com2_coarse_dn = 1;
    }
  }
  /* COM2 Inner Encoder (5 kHz step) */
  updn = 0;
  ret = encoder_input(card,i0+4,i0+5,&updn,5,1);
  if (ret == 1) {
    if (*com2_freq_stdby != INT_MISS) {
      integer = *com2_freq_stdby / 1000;
      decimal = *com2_freq_stdby - integer * 1000;
      decimal += updn;
      lasttwo = decimal - decimal/100*100;
      /* 8.33 kHz frequencies do not cover all 5kHz display steps */
      if ((lasttwo == 20) || (lasttwo == 45) || (lasttwo == 70) || (lasttwo == 95)) {
	if (updn > 0 ) decimal += 5;
	if (updn < 0 ) decimal -= 5;
      }
      if (decimal < 0) decimal = 990;
      if (decimal > 990) decimal = 0;
      *com2_freq_stdby = integer * 1000 + decimal;
      printf("COM2 STDBY FREQ: %i \n",*com2_freq_stdby);
    }
  }
  /* Does Not Work at High Turn Speeds
  ret = encoder_input(card,i0+4,i0+5,&updn,1,1);
  if (ret == 1) {
    if (updn >= 1) {
      *com2_fine_up = 1;
    } else {
      *com2_fine_dn = 1;
    }
  }
  */
  
  /* COM2 Displays */
  ret = display_output(card, d0+0, 6, com2_freq_active, 3, display_brightness);
  ret = display_output(card, d0+8, 6, com2_freq_stdby, 3, display_brightness);


  
  /*** NAV1 Panel ***/ 
  i0 = 64+8;
  d0 = 32+16;
  
  /* NAV1 tfr button */
  ret = digital_input(card,i0+0,&nav1_tfr_button,0);
  if (ret == 1) {
    printf("NAV1 TFR Button: %i \n",nav1_tfr_button);
    if (nav1_tfr_button == 1) {
      temp = *nav1_freq_active;
      *nav1_freq_active = *nav1_freq_stdby;
      *nav1_freq_stdby = temp;
    }
  }
  /* NAV1 Test Button */
  ret = digital_input(card,i0+1,&nav1_test_button,0);
  if (ret == 1) {
    printf("NAV1 TEST Button: %i \n",nav1_test_button);
  }
  /* NAV1 Outer Encoder (1 MHz step) */
  updn = 0;
  ret = encoder_input(card,i0+2,i0+3,&updn,1,1);
  if (ret == 1) {
    if (updn >= 1) {
      *nav1_coarse_up = 1;
    } else {
      *nav1_coarse_dn = 1;
    }
  }
  /* NAV1 Inner Encoder (50 kHz step) */
  updn = 0;
  ret = encoder_input(card,i0+4,i0+5,&updn,1,1);
  if (ret == 1) {
    if (updn >= 1) {
      *nav1_fine_up = 1;
    } else {
      *nav1_fine_dn = 1;
    }
  }  
  /* NAV1 Displays */
  ret = display_output(card, d0+0, 5, nav1_freq_active, 2, display_brightness);
  ret = display_output(card, d0+8, 5, nav1_freq_stdby, 2, display_brightness);

  
  /*** NAV2 Panel ***/ 
  i0 = 128+8;
  d0 = 64+16;
  
  /* NAV2 tfr button */
  ret = digital_input(card,i0+0,&nav2_tfr_button,0);
  if (ret == 1) {
    printf("NAV2 TFR Button: %i \n",nav2_tfr_button);
    if (nav2_tfr_button == 1) {
      temp = *nav2_freq_active;
      *nav2_freq_active = *nav2_freq_stdby;
      *nav2_freq_stdby = temp;
    }
  }
  /* NAV2 Test Button */
  ret = digital_input(card,i0+1,&nav2_test_button,0);
  if (ret == 1) {
    printf("NAV2 TEST Button: %i \n",nav2_test_button);
  }
  /* NAV2 Outer Encoder (1 MHz step) */
  updn = 0;
  ret = encoder_input(card,i0+2,i0+3,&updn,1,1);
  if (ret == 1) {
    if (updn >= 1) {
      *nav2_coarse_up = 1;
    } else {
      *nav2_coarse_dn = 1;
    }
  }
  /* NAV2 Inner Encoder (50 kHz step) */
  updn = 0;
  ret = encoder_input(card,i0+4,i0+5,&updn,1,1);
  if (ret == 1) {
    if (updn >= 1) {
      *nav2_fine_up = 1;
    } else {
      *nav2_fine_dn = 1;
    }
  }  
  /* NAV2 Displays */
  ret = display_output(card, d0+0, 5, nav2_freq_active, 2, display_brightness);
  ret = display_output(card, d0+8, 5, nav2_freq_stdby, 2, display_brightness);


  
  /*** ADF1 Panel ***/ 
  i0 = 96;
  d0 = 0;
  
  /* ADF1 tfr button */
  ret = digital_input(card,i0+0,&adf1_tfr_button,0);
  if (ret == 1) {
    printf("ADF1 TFR Button: %i \n",adf1_tfr_button);
    if (adf1_tfr_button == 1) {
      temp = *adf1_freq_active;
      *adf1_freq_active = *adf1_freq_stdby;
      *adf1_freq_stdby = temp;
    }
  }
  /* ADF1 ANT switch */
  ret = digital_input(card,i0+1,&adf1_ant_switch,0);
  if (ret == 1) {
    printf("ADF1 ANT SWITCH: %i \n",adf1_ant_switch);
  }
  /* ADF1 TONE switch */
  ret = digital_input(card,i0+2,&adf1_tone_switch,0);
  if (ret == 1) {
    printf("ADF1 TONE SWITCH: %i \n",adf1_tone_switch);
  }
  /* ADF1 Outer Encoder (100 kHz step) */
  updn = 0;
  ret = encoder_input(card,i0+3,i0+4,&updn,1,1);
  if (ret == 1) {
    if (updn >= 1) {
      *adf1_coarse_up = 1;
    } else {
      *adf1_coarse_dn = 1;
    }
  }
  /* ADF1 Inner Encoder (1 kHz step) */
  updn = 0;
  ret = encoder_input(card,i0+5,i0+6,&updn,1,1);
  if (ret == 1) {
    if (updn >= 1) {
      *adf1_fine_up = 1;
    } else {
      *adf1_fine_dn = 1;
    }
  }  
  /* ADF1 Displays */
  temp = *adf1_freq_active * 10;
  ret = display_output(card, d0+0, 5, &temp, 1, display_brightness);
  temp = *adf1_freq_stdby * 10;
  ret = display_output(card, d0+8, 5, &temp, 1, display_brightness);

  
  /*** ADF2 Panel ***/ 
  i0 = 128+32+16;
  d0 = 16;
  
  /* ADF2 tfr button */
  ret = digital_input(card,i0+0,&adf2_tfr_button,0);
  if (ret == 1) {
    printf("ADF2 TFR Button: %i \n",adf2_tfr_button);
    if (adf2_tfr_button == 1) {
      temp = *adf2_freq_active;
      *adf2_freq_active = *adf2_freq_stdby;
      *adf2_freq_stdby = temp;
    }
  }
  /* ADF2 ANT switch */
  ret = digital_input(card,i0+1,&adf2_ant_switch,0);
  if (ret == 1) {
    printf("ADF2 ANT SWITCH: %i \n",adf2_ant_switch);
  }
  /* ADF2 TONE switch */
  ret = digital_input(card,i0+2,&adf2_tone_switch,0);
  if (ret == 1) {
    printf("ADF2 TONE SWITCH: %i \n",adf2_tone_switch);
  }
  /* ADF2 Outer Encoder (100 kHz step) */
  updn = 0;
  ret = encoder_input(card,i0+3,i0+4,&updn,1,1);
  if (ret == 1) {
    if (updn >= 1) {
      *adf2_coarse_up = 1;
    } else {
      *adf2_coarse_dn = 1;
    }
  }
  /* ADF2 Inner Encoder (1 kHz step) */
  updn = 0;
  ret = encoder_input(card,i0+5,i0+6,&updn,1,1);
  if (ret == 1) {
    if (updn >= 1) {
      *adf2_fine_up = 1;
    } else {
      *adf2_fine_dn = 1;
    }
  } 
  /* ADF2 Displays */
  temp = *adf2_freq_active * 10;
  ret = display_output(card, d0+0, 5, &temp, 1, display_brightness);
  temp = *adf2_freq_stdby * 10;
  ret = display_output(card, d0+8, 5, &temp, 1, display_brightness);


  
  /*** AUDIO CONTROL PANEL 1 ***/
  i0 = 96+8;
  o0 = 32+8;
  a0 = 5;

  /* MIC Selectors */
  ret = digital_input(card,i0+0,&acp1_micsel_vhf1,1);
  if (ret == 1) {
    printf("ACP1 MIC SELECTOR VHF1: %i \n",acp1_micsel_vhf1);
    if (acp1_micsel_vhf1 == 1) {
      //acp1_micsel_vhf1 = 0;
      acp1_micsel_vhf2 = 0;
      acp1_micsel_vhf3 = 0;
      acp1_micsel_hf1 = 0;
      acp1_micsel_hf2 = 0;
      acp1_micsel_flt = 0;
      acp1_micsel_svc = 0;
      acp1_micsel_pa = 0;
    }
  }    
  ret = digital_output(card,o0+0,&acp1_micsel_vhf1);

  ret = digital_input(card,i0+1,&acp1_micsel_vhf2,1);
  if (ret == 1) {
    printf("ACP1 MIC SELECTOR VHF2: %i \n",acp1_micsel_vhf2);
    if (acp1_micsel_vhf2 == 1) {
      acp1_micsel_vhf1 = 0;
      //acp1_micsel_vhf2 = 0;
      acp1_micsel_vhf3 = 0;
      acp1_micsel_hf1 = 0;
      acp1_micsel_hf2 = 0;
      acp1_micsel_flt = 0;
      acp1_micsel_svc = 0;
      acp1_micsel_pa = 0;
    }
  }    
  ret = digital_output(card,o0+1,&acp1_micsel_vhf2);
  
  ret = digital_input(card,i0+2,&acp1_micsel_vhf3,1);
  if (ret == 1) {
    printf("ACP1 MIC SELECTOR VHF3: %i \n",acp1_micsel_vhf3);
    if (acp1_micsel_vhf3 == 1) {
      acp1_micsel_vhf1 = 0;
      acp1_micsel_vhf2 = 0;
      //acp1_micsel_vhf3 = 0;
      acp1_micsel_hf1 = 0;
      acp1_micsel_hf2 = 0;
      acp1_micsel_flt = 0;
      acp1_micsel_svc = 0;
      acp1_micsel_pa = 0;
    }
  }    
  ret = digital_output(card,o0+2,&acp1_micsel_vhf3);
  
  ret = digital_input(card,i0+3,&acp1_micsel_hf1,1);
  if (ret == 1) {
    printf("ACP1 MIC SELECTOR HF1: %i \n",acp1_micsel_hf1);
    if (acp1_micsel_hf1 == 1) {
      acp1_micsel_vhf1 = 0;
      acp1_micsel_vhf2 = 0;
      acp1_micsel_vhf3 = 0;
      //acp1_micsel_hf1 = 0;
      acp1_micsel_hf2 = 0;
      acp1_micsel_flt = 0;
      acp1_micsel_svc = 0;
      acp1_micsel_pa = 0;
    }
  }    
  ret = digital_output(card,o0+3,&acp1_micsel_hf1);
  
  ret = digital_input(card,i0+4,&acp1_micsel_hf2,1);
  if (ret == 1) {
    printf("ACP1 MIC SELECTOR HF2: %i \n",acp1_micsel_hf2);
    if (acp1_micsel_hf2 == 1) {
      acp1_micsel_vhf1 = 0;
      acp1_micsel_vhf2 = 0;
      acp1_micsel_vhf3 = 0;
      acp1_micsel_hf1 = 0;
      //acp1_micsel_hf2 = 0;
      acp1_micsel_flt = 0;
      acp1_micsel_svc = 0;
      acp1_micsel_pa = 0;
    }
  }    
  ret = digital_output(card,o0+4,&acp1_micsel_hf2);
  
  ret = digital_input(card,i0+5,&acp1_micsel_flt,1);
  if (ret == 1) {
    printf("ACP1 MIC SELECTOR FLT: %i \n",acp1_micsel_flt);
    if (acp1_micsel_flt == 1) {
      acp1_micsel_vhf1 = 0;
      acp1_micsel_vhf2 = 0;
      acp1_micsel_vhf3 = 0;
      acp1_micsel_hf1 = 0;
      acp1_micsel_hf2 = 0;
      //acp1_micsel_flt = 0;
      acp1_micsel_svc = 0;
      acp1_micsel_pa = 0;
    }
  }    
  ret = digital_output(card,o0+5,&acp1_micsel_flt);
  
  ret = digital_input(card,i0+6,&acp1_micsel_svc,1);
  if (ret == 1) {
    printf("ACP1 MIC SELECTOR SVC: %i \n",acp1_micsel_svc);
    if (acp1_micsel_svc == 1) {
      acp1_micsel_vhf1 = 0;
      acp1_micsel_vhf2 = 0;
      acp1_micsel_vhf3 = 0;
      acp1_micsel_hf1 = 0;
      acp1_micsel_hf2 = 0;
      acp1_micsel_flt = 0;
      //acp1_micsel_svc = 0;
      acp1_micsel_pa = 0;
    }
  }    
  ret = digital_output(card,o0+6,&acp1_micsel_svc);
  
  ret = digital_input(card,i0+7,&acp1_micsel_pa,1);
  if (ret == 1) {
    printf("ACP1 MIC SELECTOR PA: %i \n",acp1_micsel_pa);
    if (acp1_micsel_pa == 1) {
      acp1_micsel_vhf1 = 0;
      acp1_micsel_vhf2 = 0;
      acp1_micsel_vhf3 = 0;
      acp1_micsel_hf1 = 0;
      acp1_micsel_hf2 = 0;
      acp1_micsel_flt = 0;
      acp1_micsel_svc = 0;
      //acp1_micsel_pa = 0;
    }
  }    
  ret = digital_output(card,o0+7,&acp1_micsel_pa);

  /* Other Switches */
  ret = digital_input(card,i0+8,&acp1_rt_ic,0);
  if (ret == 1) {
    printf("ACP1 R/T I/C Switch: %i \n",acp1_rt_ic);
  }    

  ret = digital_input(card,i0+9,&acp1_mask_boom,0);
  if (ret == 1) {
    printf("ACP1 MASK/BOOM Switch: %i \n",acp1_mask_boom);
  }    
  
  ret = digital_input(card,i0+10,&acp1_sel_v,0);
  if (ret == 1) {
    printf("ACP1 VBR Selector V: %i \n",acp1_sel_v);
  }    
  
  ret = digital_input(card,i0+11,&acp1_sel_b,0);
  if (ret == 1) {
    printf("ACP1 VBR Selector B: %i \n",acp1_sel_b);
  }    
  ret = digital_input(card,i0+12,&acp1_sel_r,0);
  if (ret == 1) {
    printf("ACP1 VBR Selector R: %i \n",acp1_sel_r);
  }

  acp1_sel_vbr = acp1_sel_v + acp1_sel_b*2 + acp1_sel_r*3;
  
  ret = digital_input(card,i0+13,&acp1_alt_norm,0);
  if (ret == 1) {
    printf("ACP1 ALT / NORM Switch: %i \n",acp1_alt_norm);
  }

  /* Audio Volume Potentiometers */
  ret = analog_input(card,a0+0,&acp1_vol_vhf1,0.0,1.0);
  if (ret == 1) {
    //    printf("ACP1 Volume VHF1: %f \n",acp1_vol_vhf1);
  }
  ret = analog_input(card,a0+1,&acp1_vol_vhf2,0.0,1.0);
  if (ret == 1) {
    //    printf("ACP1 Volume VHF2: %f \n",acp1_vol_vhf2);
  }
  ret = analog_input(card,a0+2,&acp1_vol_pa,0.0,1.0);
  if (ret == 1) {
    //    printf("ACP1 Volume PA: %f \n",acp1_vol_pa);
  }
  ret = analog_input(card,a0+3,&acp1_vol_mkr,0.0,1.0);
  if (ret == 1) {
    //    printf("ACP1 Volume MKR: %f \n",acp1_vol_mkr);
  }
  ret = analog_input(card,a0+4,&acp1_vol_spkr,0.0,1.0);
  if (ret == 1) {
    //    printf("ACP1 Volume SPKR: %f \n",acp1_vol_spkr);
  }



  
  /*** AUDIO CONTROL PANEL 2 ***/
  i0 = 128+32;
  o0 = 32+16;
  a0 = 10;

  /* MIC Selectors */
  ret = digital_input(card,i0+0,&acp2_micsel_vhf1,1);
  if (ret == 1) {
    printf("ACP2 MIC SELECTOR VHF1: %i \n",acp2_micsel_vhf1);
    if (acp2_micsel_vhf1 == 1) {
      //acp2_micsel_vhf1 = 0;
      acp2_micsel_vhf2 = 0;
      acp2_micsel_vhf3 = 0;
      acp2_micsel_hf1 = 0;
      acp2_micsel_hf2 = 0;
      acp2_micsel_flt = 0;
      acp2_micsel_svc = 0;
      acp2_micsel_pa = 0;
    }
  }    
  ret = digital_output(card,o0+0,&acp2_micsel_vhf1);

  ret = digital_input(card,i0+1,&acp2_micsel_vhf2,1);
  if (ret == 1) {
    printf("ACP2 MIC SELECTOR VHF2: %i \n",acp2_micsel_vhf2);
    if (acp2_micsel_vhf2 == 1) {
      acp2_micsel_vhf1 = 0;
      //acp2_micsel_vhf2 = 0;
      acp2_micsel_vhf3 = 0;
      acp2_micsel_hf1 = 0;
      acp2_micsel_hf2 = 0;
      acp2_micsel_flt = 0;
      acp2_micsel_svc = 0;
      acp2_micsel_pa = 0;
    }
  }    
  ret = digital_output(card,o0+1,&acp2_micsel_vhf2);
  
  ret = digital_input(card,i0+2,&acp2_micsel_vhf3,1);
  if (ret == 1) {
    printf("ACP2 MIC SELECTOR VHF3: %i \n",acp2_micsel_vhf3);
    if (acp2_micsel_vhf3 == 1) {
      acp2_micsel_vhf1 = 0;
      acp2_micsel_vhf2 = 0;
      //acp2_micsel_vhf3 = 0;
      acp2_micsel_hf1 = 0;
      acp2_micsel_hf2 = 0;
      acp2_micsel_flt = 0;
      acp2_micsel_svc = 0;
      acp2_micsel_pa = 0;
    }
  }    
  ret = digital_output(card,o0+2,&acp2_micsel_vhf3);
  
  ret = digital_input(card,i0+3,&acp2_micsel_hf1,1);
  if (ret == 1) {
    printf("ACP2 MIC SELECTOR HF1: %i \n",acp2_micsel_hf1);
    if (acp2_micsel_hf1 == 1) {
      acp2_micsel_vhf1 = 0;
      acp2_micsel_vhf2 = 0;
      acp2_micsel_vhf3 = 0;
      //acp2_micsel_hf1 = 0;
      acp2_micsel_hf2 = 0;
      acp2_micsel_flt = 0;
      acp2_micsel_svc = 0;
      acp2_micsel_pa = 0;
    }
  }    
  ret = digital_output(card,o0+3,&acp2_micsel_hf1);
  
  ret = digital_input(card,i0+4,&acp2_micsel_hf2,1);
  if (ret == 1) {
    printf("ACP2 MIC SELECTOR HF2: %i \n",acp2_micsel_hf2);
    if (acp2_micsel_hf2 == 1) {
      acp2_micsel_vhf1 = 0;
      acp2_micsel_vhf2 = 0;
      acp2_micsel_vhf3 = 0;
      acp2_micsel_hf1 = 0;
      //acp2_micsel_hf2 = 0;
      acp2_micsel_flt = 0;
      acp2_micsel_svc = 0;
      acp2_micsel_pa = 0;
    }
  }    
  ret = digital_output(card,o0+4,&acp2_micsel_hf2);
  
  ret = digital_input(card,i0+5,&acp2_micsel_flt,1);
  if (ret == 1) {
    printf("ACP2 MIC SELECTOR FLT: %i \n",acp2_micsel_flt);
    if (acp2_micsel_flt == 1) {
      acp2_micsel_vhf1 = 0;
      acp2_micsel_vhf2 = 0;
      acp2_micsel_vhf3 = 0;
      acp2_micsel_hf1 = 0;
      acp2_micsel_hf2 = 0;
      //acp2_micsel_flt = 0;
      acp2_micsel_svc = 0;
      acp2_micsel_pa = 0;
    }
  }    
  ret = digital_output(card,o0+5,&acp2_micsel_flt);
  
  ret = digital_input(card,i0+6,&acp2_micsel_svc,1);
  if (ret == 1) {
    printf("ACP2 MIC SELECTOR SVC: %i \n",acp2_micsel_svc);
    if (acp2_micsel_svc == 1) {
      acp2_micsel_vhf1 = 0;
      acp2_micsel_vhf2 = 0;
      acp2_micsel_vhf3 = 0;
      acp2_micsel_hf1 = 0;
      acp2_micsel_hf2 = 0;
      acp2_micsel_flt = 0;
      //acp2_micsel_svc = 0;
      acp2_micsel_pa = 0;
    }
  }    
  ret = digital_output(card,o0+6,&acp2_micsel_svc);
  
  ret = digital_input(card,i0+7,&acp2_micsel_pa,1);
  if (ret == 1) {
    printf("ACP2 MIC SELECTOR PA: %i \n",acp2_micsel_pa);
    if (acp2_micsel_pa == 1) {
      acp2_micsel_vhf1 = 0;
      acp2_micsel_vhf2 = 0;
      acp2_micsel_vhf3 = 0;
      acp2_micsel_hf1 = 0;
      acp2_micsel_hf2 = 0;
      acp2_micsel_flt = 0;
      acp2_micsel_svc = 0;
      //acp2_micsel_pa = 0;
    }
  }    
  ret = digital_output(card,o0+7,&acp2_micsel_pa);

  /* Other Switches */
  ret = digital_input(card,i0+8,&acp2_rt_ic,0);
  if (ret == 1) {
    printf("ACP2 R/T I/C Switch: %i \n",acp2_rt_ic);
  }    

  ret = digital_input(card,i0+9,&acp2_mask_boom,0);
  if (ret == 1) {
    printf("ACP2 MASK/BOOM Switch: %i \n",acp2_mask_boom);
  }    
  
  ret = digital_input(card,i0+10,&acp2_sel_v,0);
  if (ret == 1) {
    printf("ACP2 VBR Selector V: %i \n",acp2_sel_v);
  }    
  
  ret = digital_input(card,i0+11,&acp2_sel_b,0);
  if (ret == 1) {
    printf("ACP2 VBR Selector B: %i \n",acp2_sel_b);
  }    
  ret = digital_input(card,i0+12,&acp2_sel_r,0);
  if (ret == 1) {
    printf("ACP2 VBR Selector R: %i \n",acp2_sel_r);
  }

  acp2_sel_vbr = acp2_sel_v + acp2_sel_b*2 + acp2_sel_r*3;
  
  ret = digital_input(card,i0+13,&acp2_alt_norm,0);
  if (ret == 1) {
    printf("ACP2 ALT / NORM Switch: %i \n",acp2_alt_norm);
  }

  /* Audio Volume Potentiometers */
  ret = analog_input(card,a0+0,&acp2_vol_vhf1,0.0,1.0);
  if (ret == 1) {
    //printf("ACP2 Volume VHF1: %f \n",acp2_vol_vhf1);
  }
  ret = analog_input(card,a0+1,&acp2_vol_vhf2,0.0,1.0);
  if (ret == 1) {
    // printf("ACP2 Volume VHF2: %f \n",acp2_vol_vhf2);
  }
  ret = analog_input(card,a0+2,&acp2_vol_pa,0.0,1.0);
  if (ret == 1) {
    // printf("ACP2 Volume PA: %f \n",acp2_vol_pa);
  }
  ret = analog_input(card,a0+3,&acp2_vol_mkr,0.0,1.0);
  if (ret == 1) {
    // printf("ACP2 Volume MKR: %f \n",acp2_vol_mkr);
  }
  ret = analog_input(card,a0+4,&acp2_vol_spkr,0.0,1.0);
  if (ret == 1) {
    //printf("ACP2 Volume SPKR: %f \n",acp2_vol_spkr);
  }


  
  /*** TRANSPONDER PANEL ***/
  i0 = 64+16;
  d0 = 0;

  /* XPNDR IDENT button */
  ret = digital_input(card,i0+8,transponder_ident,0);
  if (ret == 1) {
    printf("XPNDR IDENT BUTTON: %i \n",*transponder_ident);
  }
  /* XPNDR Selection switch */
  ret = digital_input(card,i0+9,&xpndr_sel_switch,0);
  if (ret == 1) {
    printf("XPNDR SEL SWITCH: %i \n",xpndr_sel_switch);
  }
  /* XPNDR Source switch */
  ret = digital_input(card,i0+10,&xpndr_src_switch,0);
  if (ret == 1) {
    printf("XPNDR SRC SWITCH: %i \n",xpndr_src_switch);
  }
  /* XPNDR Mode Select */
  temp = 0;
  ret = digital_input(card,i0+11,&temp,0);
  if ((ret == 1) && (xpndr_mode_select != 1)) {
    xpndr_mode_select = 1;
    printf("XPNDR MODE SELECT: %i \n",xpndr_mode_select);
  }
  temp = 0;
  ret = digital_input(card,i0+12,&temp,0);
  if ((ret == 1) && (xpndr_mode_select != 2)) {
    xpndr_mode_select = 2;
    printf("XPNDR MODE SELECT: %i \n",xpndr_mode_select);
  }
  temp = 0;
  ret = digital_input(card,i0+13,&temp,0);
  if ((ret == 1) && (xpndr_mode_select != 3)) {
    xpndr_mode_select = 3;
    printf("XPNDR MODE SELECT: %i \n",xpndr_mode_select);
  }
  temp = 0;
  ret = digital_input(card,i0+14,&temp,0);
  if ((ret == 1) && (xpndr_mode_select != 4)) {
    xpndr_mode_select = 4;
    printf("XPNDR MODE SELECT: %i \n",xpndr_mode_select);
  }
  temp = 0;
  ret = digital_input(card,i0+15,&temp,0);
  if ((ret == 1) && (xpndr_mode_select != 5)) {
    xpndr_mode_select = 5;
    printf("XPNDR MODE SELECT: %i \n",xpndr_mode_select);
  }

  if ((acf_type == 2) || (acf_type == 3)) {
    float xpndr_mode_select_f = (float) xpndr_mode_select;
    ret = set_state_updnf(&xpndr_mode_select_f,transponder_mode_f,transponder_mode_up,transponder_mode_dn);
    if (ret != 0) {
      printf("XPNDR MODE: %f %f %i %i \n",
	     xpndr_mode_select_f,*transponder_mode_f,*transponder_mode_up,*transponder_mode_dn);
    }
  } else {
    *transponder_mode = xpndr_mode_select;
  }

  
  if (*transponder_code != INT_MISS) {
    /* XPNDR Encoder Digits 1's */
    updn = 0;
    ret = encoder_input(card,i0+6,i0+7,&updn,1,1);
    if (ret == 1) {
      temp = *transponder_code - (*transponder_code / 10) * 10;
      *transponder_code -= temp;
      temp += updn;
      if (temp > transponder_max) temp = transponder_min;
      if (temp < transponder_min) temp = transponder_max;
      *transponder_code += temp; 
      printf("TRANSPONDER CODE: %i\n",*transponder_code);
    }
    /* XPNDR Encoder Digits 10's */
    updn = 0;
    ret = encoder_input(card,i0+4,i0+5,&updn,1,1);
    if (ret == 1) {
      temp = (*transponder_code - (*transponder_code / 100) * 100)/10;
      *transponder_code -= 10*temp;
      temp += updn;
      if (temp > transponder_max) temp = transponder_min;
      if (temp < transponder_min) temp = transponder_max;
      *transponder_code += 10*temp; 
      printf("TRANSPONDER CODE: %i\n",*transponder_code);
    }
    /* XPNDR Encoder Digits 100's */
    updn = 0;
    ret = encoder_input(card,i0+2,i0+3,&updn,1,1);
    if (ret == 1) {
      temp = (*transponder_code - (*transponder_code / 1000) * 1000)/100;
      *transponder_code -= 100*temp;
      temp += updn;
      if (temp > transponder_max) temp = transponder_min;
      if (temp < transponder_min) temp = transponder_max;
      *transponder_code += 100*temp; 
      printf("TRANSPONDER CODE: %i\n",*transponder_code);
    }
    /* XPNDR Encoder Digits 1000's */
    updn = 0;
    ret = encoder_input(card,i0+0,i0+1,&updn,1,1);
    if (ret == 1) {
      temp = *transponder_code / 1000;
      *transponder_code -= 1000*temp;
      temp += updn;
      if (temp > transponder_max) temp = transponder_min;
      if (temp < transponder_min) temp = transponder_max;
      *transponder_code += 1000*temp; 
      printf("TRANSPONDER CODE: %i\n",*transponder_code);
    }

    /* Transponder 7 seg display is split into two parts
       since there were no 4 display outputs in a row remaining.
       The digits take place 6/7 and 14/15 of the ADF1 display unit */
    
    /* Digits 1's */
    temp = *transponder_code - (*transponder_code / 10) * 10;
    ret = display_output(card, d0+6, 1, &temp, -1, display_brightness);
    /* Digits 10's */
    temp = *transponder_code / 10 - (*transponder_code / 100) * 10;
    ret = display_output(card, d0+7, 1, &temp, -1, display_brightness);
    /* Digits 100's */
    temp = *transponder_code / 100 - (*transponder_code / 1000)*10;
    ret = display_output(card, d0+8+6, 1, &temp, -1, display_brightness);
    /* Digits 1000's */
    if (*transponder_fail != INT_MISS) {
      /* set major digit DP for transponder fail led */
      if (*lights_test == 1.0) {
	dp = 0; 
      } else {
	dp = -1 + *transponder_fail; 
      }
    } else {
      dp = -1;
    }
    temp = *transponder_code / 1000;
    ret = display_output(card, d0+8+7, 1, &temp, dp, display_brightness);

  } else {
    temp = 0;
    ret = display_output(card, d0+6, 2, &temp, -1, 0);
    ret = display_output(card, d0+8+6, 2, &temp, -1, 0);
  }


  
  /**** RUDDER TRIM PANEL ***/
  i0 = 8;
  o0 = 32;

  /* Aileron Trim Left switch 1 */
  ret = digital_input(card,i0+1,&temp,0);
  /* Aileron Trim Left switch 2 */
  ret = digital_input(card,i0+3,&temp2,0);
  /* Only activate Aileron Trim if both Switches are activated */
  if ((temp == 1) && (temp2 == 1)) {
    *aileron_trim_left = 1;
  } else {
    *aileron_trim_left = 0;
  }
  /* Aileron Trim Right switch 1 */
  ret = digital_input(card,i0+2,&temp,0);
  /* Aileron Trim Right switch 2 */
  ret = digital_input(card,i0+4,&temp2,0);
  /* Only activate Aileron Trim if both Switches are activated */
  if ((temp == 1) && (temp2 == 1)) {
    *aileron_trim_right = 1;
  } else {
    *aileron_trim_right = 0;
  }
  
  /* Rudder Trim Left switch */
  ret = digital_input(card,i0+5,rudder_trim_left,0);
  if (ret == 1) {
    printf("RUDDER TRIM LEFT SWITCH \n");
  }
  /* Rudder Trim Right switch */
  ret = digital_input(card,i0+6,rudder_trim_right,0);
  if (ret == 1) {
    printf("RUDDER TRIM RIGHT SWITCH \n");
  }

  /*  Rudder Trim Display Simulated by Servo 0 */
  float servoval = 0.0;
  if (*rudder_trim != FLT_MISS) {
    servoval = *rudder_trim * 0.82 - 0.28;
  }
  ret = servo_outputf(card,0,&servoval,trim_min,trim_max);


  
  /*** SELCAL PANEL ***/
  /* Part of WXR PANEL */
  i0 = 24;
  o0 = 32+24;

  ret = digital_input(card,i0+0,vhf1,0);
  if (ret == 1) {
    printf("SELCAL VHF1 BUTTON: %i \n",*vhf1);
  }    

  ret = digital_input(card,i0+1,vhf2,0);
  if (ret == 1) {
    printf("SELCAL VHF2 BUTTON: %i \n",*vhf2);
  }    
  
  ret = digital_input(card,i0+2,vhf3,0);
  if (ret == 1) {
    printf("SELCAL VHF3 BUTTON: %i \n",*vhf3);
  }    

  ret = digital_input(card,i0+3,hf1,0);
  if (ret == 1) {
    printf("SELCAL HF1 BUTTON: %i \n",*hf1);
  }    
  
  ret = digital_input(card,i0+4,hf2,0);
  if (ret == 1) {
    printf("SELCAL HF2 BUTTON: %i \n",*hf2);
  }    

  if (*lights_test == 1.0) {
    ret = digital_output(card,o0+0,&one);
    ret = digital_output(card,o0+1,&one);
    ret = digital_output(card,o0+2,&one);
    ret = digital_output(card,o0+3,&one);
    ret = digital_output(card,o0+4,&one);
  } else {
    ret = digital_output(card,o0+0,vhf1);
    ret = digital_output(card,o0+1,vhf2);
    ret = digital_output(card,o0+2,vhf3);
    ret = digital_output(card,o0+3,hf1);
    ret = digital_output(card,o0+4,hf2);
  }


  
  /*** WXR PANEL ***/
  /* Part of SELCAL PANEL */
  i0 = 24;
  a0 = 0;

  /* WXR Mode Selector */
  ret = digital_input(card,i0+5,&wxr_mode_test,0);
  if (ret == 1) {
    printf("WXR MODE TEST: %i \n",wxr_mode_test);
  }    
  ret = digital_input(card,i0+6,&wxr_mode_wx,0);
  if (ret == 1) {
    printf("WXR MODE WX: %i \n",wxr_mode_wx);
  }    
  ret = digital_input(card,i0+7,&wxr_mode_turb,0);
  if (ret == 1) {
    printf("WXR MODE WX/TURB: %i \n",wxr_mode_turb);
  }

  /* WXR Gain */
  ret = analog_input(card,a0+0,wxr_gain,0.0,1.0);
  if (ret == 1) {
    //    printf("WXR Gain: %f \n",*wxr_gain);
  }
  
  /* WXR Tilt */
  ret = analog_input(card,a0+1,wxr_tilt,0.0,1.0);
  if (ret == 1) {
    //    printf("WXR Tilt: %f \n",*wxr_tilt);
  }



  /*** LIGHTS PANEL ***/
  a0 = 2;

  /* Panel Brightness: HARDWARE */

  /* Flood Brightness */
  ret = analog_input(card,a0+0,&flood_brightness,0.0,1.0);
  if (ret == 1) {
    //printf("FLOOD LIGHT BRIGHTNESS: %f \n",flood_brightness);
  }
  //ret = servo_outputf(card,0,&flood_brightness,0.0,1.0);



  
  /*** DOOR PANEL ***/
  i0 = 0;
  o0 = 32;

  ret = digital_input(card,i0+0,&stab_trim_ovrd,0);
  if (ret == 1) {
    printf("STAB TRIM OVRD SWITCH: %i \n",stab_trim_ovrd);
  }    

  ret = digital_input(card,i0+1,&flt_deck_door_unlock,0);
  if (ret == 1) {
    printf("FLT DECK DOOR UNLOCKED: %i \n",flt_deck_door_unlock);
  }    
  ret = digital_input(card,i0+2,&flt_deck_door_auto,0);
  if (ret == 1) {
    printf("FLT DECK DOOR AUTO: %i \n",flt_deck_door_auto);
  }    
  ret = digital_input(card,i0+3,&flt_deck_door_deny,0);
  if (ret == 1) {
    printf("FLT DECK DOOR DENY: %i \n",flt_deck_door_deny);
  }

  /* NOT WORKING IN ZIBO MOD AS OF 2023/02/07 */
  /*
  flt_deck_door_pos = -flt_deck_door_unlock + flt_deck_door_deny; 
  float flt_deck_door_pos_f = (float) flt_deck_door_pos;
  ret = set_state_updnf(&flt_deck_door_pos_f,flt_deck_door,flt_deck_door_right,flt_deck_door_left);
  if (ret != 0) {
    printf("FLT DECK DOOR: %f %f %i %i \n",
	   flt_deck_door_pos_f,*flt_deck_door,*flt_deck_door_right,*flt_deck_door_left);
  }
  */
 
  ret = digital_outputf(card,o0+0,lock_fail);
  ret = digital_outputf(card,o0+1,auto_unlk);
  


  
  /*** FIRE MODULE ***/
  i0 = 32;
  o0 = 0;

  if ((acf_type == 1) || (acf_type == 2) || (acf_type == 3)) {
  
    /* annunciators */
    ret = digital_outputf(card,o0+0,fire_eng1_ovht_ann);
    ret = digital_outputf(card,o0+1,fire_wheelwell_ann);
    ret = digital_outputf(card,o0+2,fire_fault_ann);
    ret = digital_outputf(card,o0+3,fire_apu_det_inop_ann);
    ret = digital_outputf(card,o0+4,fire_apu_bottle_disch_ann);
    ret = digital_outputf(card,o0+5,fire_eng2_ovht_ann);
    ret = digital_outputf(card,o0+6,fire_bottle_l_disch_ann);
    ret = digital_outputf(card,o0+7,fire_bottle_r_disch_ann);
    
    /* fire handle lights */
    ret = digital_outputf(card,o0+8,fire_eng1_ann);
    ret = digital_outputf(card,o0+9,fire_apu_ann);
    ret = digital_outputf(card,o0+10,fire_eng2_ann);
    
    /* discharge indicators */
    ret = digital_outputf(card,o0+12,fire_eng1_test_ann);
    ret = digital_outputf(card,o0+13,fire_apu_test_ann);
    ret = digital_outputf(card,o0+14,fire_eng2_test_ann);

    /* Fire Handle ENG 1*/
    ret = digital_inputf(card,i0+0,&fire_eng1_pulled_input,-1);
    if (ret == 1) {
      printf("FIRE HANDLE ENG 1 PULLED: %f \n",fire_eng1_pulled_input);
    }
    ret = set_state_togglef(&fire_eng1_pulled_input, fire_eng1_pulled_status, fire_eng1_pulled);

    ret = digital_inputf(card,i0+1,&fire_eng1_rotate_l_input,0);
    if (ret == 1) {
      printf("FIRE HANDLE ENG 1 ROTATE LEFT: %f \n",fire_eng1_rotate_l_input);
    }
    ret = digital_inputf(card,i0+2,&fire_eng1_rotate_r_input,0);
    if (ret == 1) {
      printf("FIRE HANDLE ENG 1 ROTATE RIGHT: %f \n",fire_eng1_rotate_r_input);
    }
    float fire_eng1_rotate_input = -fire_eng1_rotate_l_input + fire_eng1_rotate_r_input;
    ret = set_state_updnf(&fire_eng1_rotate_input, fire_eng1_rotate_status, fire_eng1_rotate_r, fire_eng1_rotate_l);

    /* Fire Handle APU */
    ret = digital_inputf(card,i0+3,&fire_apu_pulled_input,-1);
    if (ret == 1) {
      printf("FIRE HANDLE ENG 1 PULLED: %f \n",fire_apu_pulled_input);
    }
    ret = set_state_togglef(&fire_apu_pulled_input, fire_apu_pulled_status, fire_apu_pulled);

    ret = digital_inputf(card,i0+4,&fire_apu_rotate_l_input,0);
    if (ret == 1) {
      printf("FIRE HANDLE ENG 1 ROTATE LEFT: %f \n",fire_apu_rotate_l_input);
    }
    ret = digital_inputf(card,i0+5,&fire_apu_rotate_r_input,0);
    if (ret == 1) {
      printf("FIRE HANDLE ENG 1 ROTATE RIGHT: %f \n",fire_apu_rotate_r_input);
    }
    float fire_apu_rotate_input = -fire_apu_rotate_l_input + fire_apu_rotate_r_input;
    ret = set_state_updnf(&fire_apu_rotate_input, fire_apu_rotate_status, fire_apu_rotate_r, fire_apu_rotate_l);

    /* Fire Handle ENG2 */
    ret = digital_inputf(card,i0+8,&fire_eng2_pulled_input,-1);
    if (ret == 1) {
      printf("FIRE HANDLE ENG 1 PULLED: %f \n",fire_eng2_pulled_input);
    }
    ret = set_state_togglef(&fire_eng2_pulled_input, fire_eng2_pulled_status, fire_eng2_pulled);

    ret = digital_inputf(card,i0+9,&fire_eng2_rotate_l_input,0);
    if (ret == 1) {
      printf("FIRE HANDLE ENG 1 ROTATE LEFT: %f \n",fire_eng2_rotate_l_input);
    }
    ret = digital_inputf(card,i0+10,&fire_eng2_rotate_r_input,0);
    if (ret == 1) {
      printf("FIRE HANDLE ENG 1 ROTATE RIGHT: %f \n",fire_eng2_rotate_r_input);
    }
    float fire_eng2_rotate_input = -fire_eng2_rotate_l_input + fire_eng2_rotate_r_input;
    ret = set_state_updnf(&fire_eng2_rotate_input, fire_eng2_rotate_status, fire_eng2_rotate_r, fire_eng2_rotate_l);

    /* Fire Bell Cutout Switch */
    ret = digital_input(card,i0+12,&fire_bell_cutout,0);
    if (ret == 1) {
      printf("FIRE BELL CUTOUT: %i \n",fire_bell_cutout);
    }

    /* Fire Module Switches */
    ret = digital_input(card,i0+16,&fire_eng1_ovht_det_a,0);
    if (ret == 1) {
      printf("ENG 1 OVHT DET A: %i \n",fire_eng1_ovht_det_a);
    }
    ret = digital_input(card,i0+17,&fire_eng1_ovht_det_b,0);
    if (ret == 1) {
      printf("ENG 1 OVHT DET B: %i \n",fire_eng1_ovht_det_b);
    }
    ret = digital_input(card,i0+18,fire_test_fault,0);
    if (ret == 1) {
      printf("FIRE TEST/FAULT: %i \n",*fire_test_fault);
    }
    ret = digital_input(card,i0+19,fire_test_ovht,0);
    if (ret == 1) {
      printf("FIRE TEST/OVHT: %i \n",*fire_test_ovht);
    }
    ret = digital_input(card,i0+20,&fire_eng2_ovht_det_a,0);
    if (ret == 1) {
      printf("ENG 2 OVHT DET A: %i \n",fire_eng2_ovht_det_a);
    }
    ret = digital_input(card,i0+21,&fire_eng2_ovht_det_b,0);
    if (ret == 1) {
      printf("ENG 2 OVHT DET B: %i \n",fire_eng2_ovht_det_b);
    }
    ret = digital_input(card,i0+22,fire_eng_ext_test_left,0);
    if (ret == 1) {
      printf("FIRE EXT TEST 1: %i \n",*fire_eng_ext_test_left);
    }
    ret = digital_input(card,i0+23,fire_eng_ext_test_right,0);
    if (ret == 1) {
      printf("FIRE EXT TEST 2: %i \n",*fire_eng_ext_test_right);
    }
    
  }
  
  /*** CARGO FIRE PANEL ***/
  i0 = 32+24;
  o0 = 16;

  /* TEST button */
  ret = digital_input(card,i0+0,&cargofire_test,0);
  if (ret == 1) {
    printf("CARGO FIRE TEST BUTTON: %i \n",cargofire_test);
  }  
  /* SELECT FWD rotary */
  ret = digital_input(card,i0+1,&cargofire_select_fwd_a,0);
  if (ret == 1) {
    printf("CARGO FIRE SELECT FWD A: %i \n",cargofire_select_fwd_a);
  }  
  ret = digital_input(card,i0+2,&cargofire_select_fwd_b,0);
  if (ret == 1) {
    printf("CARGO FIRE SELECT FWD B: %i \n",cargofire_select_fwd_b);
  }  
  /* SELECT AFT rotary */
  ret = digital_input(card,i0+3,&cargofire_select_aft_a,0);
  if (ret == 1) {
    printf("CARGO FIRE SELECT AFT A: %i \n",cargofire_select_aft_a);
  }  
  ret = digital_input(card,i0+4,&cargofire_select_aft_b,0);
  if (ret == 1) {
    printf("CARGO FIRE SELECT AFT B: %i \n",cargofire_select_aft_b);
  }
  /* ARM FWD button */
  ret = digital_input(card,i0+5,&cargofire_arm_fwd,1);
  if (ret == 1) {
    printf("CARGO FIRE ARM FWD: %i \n",cargofire_arm_fwd);
  }
  /* ARM AFT button */
  ret = digital_input(card,i0+6,&cargofire_arm_aft,1);
  if (ret == 1) {
    printf("CARGO FIRE ARM AFT: %i \n",cargofire_arm_aft);
  }
  /* DISCH button */
  ret = digital_input(card,i0+7,&cargofire_disch,0);
  if (ret == 1) {
    printf("CARGO FIRE DISCH: %i \n",cargofire_disch);
  }

  /* Annunciators */
  if (*lights_test == 1.0) {
    ret = digital_output(card,o0+0,&one);
    ret = digital_output(card,o0+1,&one);
    ret = digital_output(card,o0+2,&one);
    ret = digital_output(card,o0+3,&one);
    ret = digital_output(card,o0+4,&one);
    ret = digital_output(card,o0+5,&one);
    ret = digital_output(card,o0+6,&one);
    ret = digital_output(card,o0+7,&one);
  } else {
    cargofire_fault = cargofire_test;
    cargofire_test_fwd = cargofire_test;
    cargofire_test_aft = cargofire_test;
    ret = digital_output(card,o0+0,&cargofire_fault);
    ret = digital_output(card,o0+1,&cargofire_test_fwd);
    ret = digital_output(card,o0+2,&cargofire_test_aft);
    ret = digital_output(card,o0+3,&cargofire_arm_fwd);
    ret = digital_output(card,o0+4,&cargofire_arm_fwd);
    ret = digital_output(card,o0+5,&cargofire_arm_aft);
    ret = digital_output(card,o0+6,&cargofire_arm_aft);
    ret = digital_output(card,o0+7,&cargofire_disch);
  }


}



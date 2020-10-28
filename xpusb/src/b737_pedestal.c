/* This is the b737_pedestal.c code which contains a sample set-up for how to communicate with the 
   BOEING 737 pedestal (switches, LED's, displays) connected to the OpenCockpits IOCARDS USB device.

   Copyright (C) 2009 - 2013  Reto Stockli

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

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "libiocards.h"
#include "serverdata.h"
#include "b737_pedestal.h"

#define max(A,B) ((A)>(B) ? (A) : (B)) 
#define min(A,B) ((A)<(B) ? (A) : (B)) 

void b737_pedestal(void)
{

  int device;
  int card;

  /* x-plane data */
  int *nav1_freq_active = link_dataref_int("sim/cockpit/radios/nav1_freq_hz");
  int *nav2_freq_active = link_dataref_int("sim/cockpit/radios/nav2_freq_hz");
  int *com1_freq_active = link_dataref_int("sim/cockpit2/radios/actuators/com1_frequency_hz_833");
  int *com2_freq_active = link_dataref_int("sim/cockpit2/radios/actuators/com2_frequency_hz_833");
  int *adf1_freq_active = link_dataref_int("sim/cockpit/radios/adf1_freq_hz");
  int *adf2_freq_active = link_dataref_int("sim/cockpit/radios/adf2_freq_hz");
  //  int *dme_freq_active = link_dataref_int("sim/cockpit/radios/dme_freq_hz");
  int *nav1_freq_stdby = link_dataref_int("sim/cockpit/radios/nav1_stdby_freq_hz");
  int *nav2_freq_stdby = link_dataref_int("sim/cockpit/radios/nav2_stdby_freq_hz");
  int *com1_freq_stdby = link_dataref_int("sim/cockpit2/radios/actuators/com1_standby_frequency_hz_833");
  int *com2_freq_stdby = link_dataref_int("sim/cockpit2/radios/actuators/com2_standby_frequency_hz_833");
  int *adf1_freq_stdby = link_dataref_int("sim/cockpit/radios/adf1_stdby_freq_hz");
  int *adf2_freq_stdby = link_dataref_int("sim/cockpit/radios/adf2_stdby_freq_hz"); 
  //  int *dme_freq_stdby = link_dataref_int("sim/cockpit/radios/dme_stdby_freq_hz");

  int *transponder_code = link_dataref_int("sim/cockpit/radios/transponder_code");
  int *transponder_mode = link_dataref_int("sim/cockpit/radios/transponder_mode");
  int *transponder_fail = link_dataref_int("sim/operation/failures/rel_xpndr");
  int *transponder_ident = link_dataref_int("sim/cockpit/radios/transponder_id");

  int *transponder_mode_up;
  int *transponder_mode_dn;
  if ((acf_type == 2) || (acf_type == 3)) {
    transponder_mode_up = link_dataref_cmd_once("laminar/B738/knob/transponder_mode_up");
    transponder_mode_dn = link_dataref_cmd_once("laminar/B738/knob/transponder_mode_dn");
    *transponder_mode_up = 0;
    *transponder_mode_dn = 0;
  }
    
  float *rudder_trim = link_dataref_flt("sim/flightmodel/controls/rud_trim",-3);
  float *aileron_trim = link_dataref_flt("sim/flightmodel/controls/ail_trim",-4);

  /* FIRE SWITCHES ETC. (ONLY FOR x737) */
  int *fire_test; 
  int *fire_test_ovht; 
  int *fire_test_fault; 
  int *fire_eng1_ovht_det; 
  int *fire_eng2_ovht_det;
  int *fire_eng_ext_test;
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
  int *fire_eng1_rotate; 
  int *fire_eng1_rotate_l; 
  int *fire_eng1_rotate_r; 
  int *fire_apu_pulled;
  int *fire_apu_rotate; 
  int *fire_apu_rotate_l; 
  int *fire_apu_rotate_r; 
  int *fire_eng2_pulled;
  int *fire_eng2_rotate;
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

    *fire_eng1_pulled = 0;
    *fire_eng1_rotate_l = 0;
    *fire_eng1_rotate_r = 0;
    *fire_apu_pulled = 0;
    *fire_apu_rotate_l = 0;
    *fire_apu_rotate_r = 0;
    *fire_eng2_pulled = 0;
    *fire_eng2_rotate_l = 0;
    *fire_eng2_rotate_r = 0;
    
    fire_eng1_pulled_status = link_dataref_flt("laminar/B738/fire/engine01/ext_switch/pos_arm",-2); 
    fire_eng1_rotate_status = link_dataref_flt("laminar/B738/fire/engine01/ext_switch/pos_disch",-2);
    fire_apu_pulled_status = link_dataref_flt("laminar/B738/fire/apu/ext_switch/pos_arm",-2); 
    fire_apu_rotate_status = link_dataref_flt("laminar/B738/fire/apu/ext_switch/pos_disch",-2);
    fire_eng2_pulled_status = link_dataref_flt("laminar/B738/fire/engine02/ext_switch/pos_arm",-2); 
    fire_eng2_rotate_status = link_dataref_flt("laminar/B738/fire/engine02/ext_switch/pos_disch",-2);
   
    /*
    fire_eng1_ovht_det = link_dataref_int("");
    fire_eng2_ovht_det = link_dataref_int("");
    */
  } else if (acf_type == 1) {
    fire_test = link_dataref_int("x737/cockpit/fireSupPanel/fireWarnTestSw_state"); // 0: fault, 1: off, 2: ovht
    fire_eng1_ovht_det = link_dataref_int("x737/cockpit/fireSupPanel/Eng1OvhtDetSw_state"); // 0: A, 1: NORMAL, 2: B
    fire_eng2_ovht_det = link_dataref_int("x737/cockpit/fireSupPanel/Eng2OvhtDetSw_state"); // 0: A, 1: NORMAL, 2: B
    fire_eng_ext_test = link_dataref_int("x737/cockpit/fireSupPanel/fireExtTestSw_state"); //  0: '1', 1: center, 2: '2'
    fire_eng1_pulled = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle1Pulled_state"); // 0: down, 1: pulled
    fire_eng1_rotate = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle1Rotate_state"); // -1: 'L', 0: center, 1: 'R'
    fire_apu_pulled = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle2Pulled_state"); // 0: down, 1: pulled
    fire_apu_rotate = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle2Rotate_state"); // -1: 'L', 0: center, 1: 'R'
    fire_eng2_pulled = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle3Pulled_state"); // 0: down, 1: pulled
    fire_eng2_rotate = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle3Rotate_state"); // -1: 'L', 0: center, 1: 'R'
    
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
  int *vhf1 = link_dataref_cmd_once("BetterPushback/start");
  int *vhf2 = link_dataref_cmd_once("BetterPushback/stop");
  int *vhf3 = link_dataref_cmd_once("BetterPushback/disconnect");
  
  /* unimplemented datarefs (not available in X-Plane and not in x737) */
  int *hf1 = link_dataref_int("xpserver/selcal/hf1");
  int *hf2 = link_dataref_int("xpserver/selcal/hf2");
  int *wxsel = link_dataref_int("xpserver/wxradar/sel");
  int *cargofire_fwd_select = link_dataref_int("xpserver/cargofire_fwd_select");
  int *cargofire_fwd_armed = link_dataref_int("xpserver/cargofire_fwd_armed");
  int *cargofire_aft_select = link_dataref_int("xpserver/cargofire_aft_select");
  int *cargofire_aft_armed = link_dataref_int("xpserver/cargofire_aft_armed");
  int *cargofire_disch = link_dataref_int("xpserver/cargofire_disch");
  int *cargofire_test = link_dataref_int("xpserver/cargofire_test");
  int *cargofire_fault = link_dataref_int("xpserver/cargofire_fault");
  //  int *transponder_select = link_dataref_int("xpserver/transponder_select");


  int *stab_trim_mode;
  float *flt_dk_door;
  float *lock_fail;
  float *auto_unlk;
  if ((acf_type == 2) || (acf_type == 3)) {
    stab_trim_mode = link_dataref_int("xpserver/stab_trim_mode");
    flt_dk_door = link_dataref_flt("laminar/B738/toggle_switch/flt_dk_door",0);
    lock_fail = link_dataref_flt("laminar/B738/annunciator/door_lock_fail",0);
    auto_unlk = link_dataref_flt("laminar/B738/annunciator/door_auto_unlk",0);
  } else {
    stab_trim_mode = link_dataref_int("xpserver/stab_trim_mode");
    flt_dk_door = link_dataref_flt("xpserver/flt_dk_door",0);
    lock_fail = link_dataref_flt("xpserver/lock_fail",0);
    auto_unlk = link_dataref_flt("xpserver/auto_unlk",0);
  }
    
  float *radio_volume1;
  float *radio_volume2;
  if ((acf_type == 2) || (acf_type == 3)) {
    radio_volume1 = link_dataref_flt("laminar/B738/comm/audio_vol_com1",-1);
    radio_volume2 = link_dataref_flt("laminar/B738/comm/audio_vol_com2",-1);
  } else {
    radio_volume1 = link_dataref_flt("sim/cockpit2/radios/actuators/audio_volume_com1",-1);
    radio_volume2 = link_dataref_flt("sim/cockpit2/radios/actuators/audio_volume_com1",-1);
  }
  float *master_volume = link_dataref_flt("sim/operation/sound/master_volume_ratio",-1);

  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");
 
  /* local variables */
  int ret;
  float updn;
  int test;
  int test1; 
  int test2;
  int test3; 
  int tfr;
  int temp;
  int integer;
  int decimal;

  /* parameters */
  /* X-Plane frequencies are scaled by 10 kHz except for adf and dme */
  int zero = 0;
  int one = 1;
  int ten = 10;
  int nav_min = 108; // MHz
  int nav_max = 117; // MHz
  int com_min = 118; // MHz
  int com_max = 136; // MHz
  int adf_min = 100; // kHz
  int adf_max = 2000; // kHz
  int transponder_min = 0;
  int transponder_max = 7777;

  float trim_min = -1.0;
  float trim_max = 1.0;

  float flood_brightness;
  float panel_brightness;

  /***** Mastercard #0 connected to USB Expansion Card device 0 *****/
  device = 0;
  card = 0;

  /* read inputs */

  /* NAV1 test button */
  ret = digital_input(device,card,0,&test,0);
  if (ret == 1) {
    printf("NAV1 TEST BUTTON: %i \n",test);
  }

  /* NAV1 tfr button */
  ret = digital_input(device,card,1,&tfr,0);
  if (ret == 1) {
    if (tfr == 1) {
      temp = *nav1_freq_active;
      *nav1_freq_active = *nav1_freq_stdby;
      *nav1_freq_stdby = temp;
    }
  }

  /* COM1 test button */
  ret = digital_input(device,card,9,&test,0);
  if (ret == 1) {
    printf("COM1 TEST BUTTON: %i \n",test);
  }

  /* COM1 tfr button */
  ret = digital_input(device,card,10,&tfr,0);
  if (ret == 1) {
    if (tfr == 1) {
      temp = *com1_freq_active;
      *com1_freq_active = *com1_freq_stdby;
      *com1_freq_stdby = temp;
    }
  }

  /* ADF1 switch 1 */
  ret = digital_input(device,card,18,&test,0);
  if (ret == 1) {
    printf("ADF1 TONE SWITCH: %i \n",test);
  }

  /* ADF1 switch 2 */
  ret = digital_input(device,card,19,&test,0);
  if (ret == 1) {
    printf("ADF1 ANT SWITCH: %i \n",test);
  }

  /* ADF1 tfr button */
  ret = digital_input(device,card,20,&tfr,0);
  if (ret == 1) {
    if (tfr == 1) {
      temp = *adf1_freq_active;
      *adf1_freq_active = *adf1_freq_stdby;
      *adf1_freq_stdby = temp;
    }
  }

  /* SELCAL */
  ret = digital_input(device,card,56,hf2,0);
  if (ret == 1) {
    printf("SELCAL HF2 button: %i \n",*hf2);
  }
  if ((acf_type == 2) || (acf_type == 3)) {
  }
  
  ret = digital_input(device,card,57,hf1,0);
  if (ret == 1) {
    printf("SELCAL HF1 button: %i \n",*hf1);
  }

  ret = digital_input(device,card,58,vhf3,2);
  if (ret == 1) {
    printf("SELCAL VHF3 button: %i \n",*vhf3);
  }
  ret = digital_input(device,card,59,vhf2,2);
  if (ret == 1) {
    printf("SELCAL VHF2 button: %i \n",*vhf2);
  }
  ret = digital_input(device,card,60,vhf1,2);
  if (ret == 1) {
    printf("SELCAL VHF1 button: %i \n",*vhf1);
  }

  /* use WX SELECTOR for AUTOBRAKE until we have built the MIP */
  ret = digital_input(device,card,55,&test,0);
  if (ret == 1) {
    printf("WX Radar Switch: %i \n",test);
  }
  ret = digital_input(device,card,54,&test1,0);
  if (ret == 1) {
    printf("WX Radar Switch: %i \n",test1*2);
  }
  ret = digital_input(device,card,62,&test2,0);
  if (ret == 1) {
    printf("WX Radar Switch: %i \n",test2*3);
  }  
  ret = digital_input(device,card,61,&test3,0);
  if (ret == 1) {
    printf("WX Radar Switch: %i \n",test3*5);
  }
  float autobrake_pos;
  if ((acf_type == 2) || (acf_type == 3)) {
    autobrake_pos = (float) test + test1*2 + test2*3 + test3*5;
    float *autobrake = link_dataref_flt("laminar/B738/autobrake/autobrake_pos",0);
    int *autobrake_dn = link_dataref_cmd_once("laminar/B738/knob/autobrake_dn");
    int *autobrake_up = link_dataref_cmd_once("laminar/B738/knob/autobrake_up");
    ret = set_state_updnf(&autobrake_pos,autobrake,autobrake_up,autobrake_dn);
    if (ret == 1) {
      printf("AUTOBRAKE: %f \n",autobrake_pos);
    }
  }

  /* CARGO FIRE */

  /* let us map cargo fire disch/armed buttons to the clock for now */
  if ((acf_type == 2) || (acf_type == 3)) {
    int *chr_mode = link_dataref_cmd_hold("laminar/B738/push_button/chrono_cycle_capt");
    int *et_mode = link_dataref_cmd_hold("laminar/B738/push_button/chrono_capt_et_mode");
    int *et_reset = link_dataref_cmd_hold("laminar/B738/push_button/et_reset_capt");
    int *date_mode = link_dataref_cmd_hold("laminar/B738/push_button/chrono_disp_mode_capt");
    ret = digital_input(device,card,45,&test,0);
    if (ret == 1) {
      printf("Cargo Fire DISCH Button: %i \n",test);
    }
    *chr_mode = test;
    
    ret = digital_input(device,card,46,&test,0);
    if (ret == 1) {
      printf("Cargo Fire AFT ARMED Button: %i \n",test);
    }
    *et_reset = test;

    ret = digital_input(device,card,47,&test,0);
    if (ret == 1) {
      printf("Cargo Fire FWD ARMED Button: %i \n",test);
    }
    *et_mode = test;
  
    ret = digital_input(device,card,48,&test,0);
    if (ret == 1) {
      printf("Cargo Fire TEST Button: %i \n",test);
    }
    *date_mode = test;
  }


  /* let us map the cargo fire extinguisher select to the IRS select for now */
  if ((acf_type == 2) || (acf_type == 3)) {
    ret = digital_input(device,card,49,&test,0);
    if (ret == 1) {
      printf("Cargo Fire AFT Switch A: %i \n",test);
    }
    ret = digital_input(device,card,50,&test2,0);
    if (ret == 1) {
      printf("Cargo Fire AFT Switch B: %i \n",test2);
    }
    int irs_r_pos = INT_MISS;
    if ((test != INT_MISS) && (test2 != INT_MISS)) {
      irs_r_pos = (1-test) + test2;
    } 
    int *irs_r = link_dataref_int("laminar/B738/toggle_switch/irs_right");
    int *irs_r_left = link_dataref_cmd_once("laminar/B738/toggle_switch/irs_R_left");
    int *irs_r_right = link_dataref_cmd_once("laminar/B738/toggle_switch/irs_R_right");
    ret = set_state_updn(&irs_r_pos,irs_r,irs_r_right,irs_r_left);
    if (ret == 1) {
      printf("IRS R POS: %i \n",irs_r_pos);
    }

    ret = digital_input(device,card,51,&test,0);
    if (ret == 1) {
      printf("Cargo Fire FWD Switch A: %i \n",test);
    }
    ret = digital_input(device,card,52,&test2,0);
    if (ret == 1) {
      printf("Cargo Fire FWD Switch B: %i \n",test2);
    }
    int irs_l_pos = INT_MISS;
    if ((test != INT_MISS) && (test2 != INT_MISS)) {
      irs_l_pos = (1-test) + test2;
    } 
    int *irs_l = link_dataref_int("laminar/B738/toggle_switch/irs_left");
    int *irs_l_left = link_dataref_cmd_once("laminar/B738/toggle_switch/irs_L_left");
    int *irs_l_right = link_dataref_cmd_once("laminar/B738/toggle_switch/irs_L_right");
    ret = set_state_updn(&irs_l_pos,irs_l,irs_l_right,irs_l_left);
    if (ret == 1) {
      printf("IRS L POS: %i \n",irs_l_pos);
    }
  }

  /* TRANSPONDER */


  ret = digital_input(device,card,67,&test,0);
  if (ret == 1) {
    if ((acf_type == 2) || (acf_type == 3)) {
      if (*transponder_mode > test) {
	*transponder_mode_dn = 1;
      } else if (*transponder_mode < test) {
	*transponder_mode_up = 1;
      }
    } else {
      *transponder_mode = test;
    }
    printf("TRANSPONDER MODE: %i \n",test);
  }

  ret = digital_input(device,card,68,&test,0);
  if (ret == 1) {
    if ((acf_type == 2) || (acf_type == 3)) {
      if (*transponder_mode > test*2) {
	*transponder_mode_dn = 1;
      } else if (*transponder_mode < test*2) {
	*transponder_mode_up = 1;
      }
    } else {
      *transponder_mode = test*2;
    }
    printf("TRANSPONDER MODE: %i \n",test*2);
  }

  ret = digital_input(device,card,70,&test,0);
  if (ret == 1) {
    printf("TRANSPONDER SOURCE: %i \n",test+1);
  }
  ret = digital_input(device,card,69,&test,0);
  if (ret == 1) {
    printf("TRANSPONDER IDENT: %i \n",test);
  }
  ret = digital_input(device,card,71,&test,0);
  if (ret == 1) {
    printf("TRANSPONDER SEL: %i \n",test+1);
  }


  /* read encoders */

  /* NAV1 */
  updn = 0.;
  ret = mastercard_encoder(device,card,3,&updn,1.0,2,3);
  if (ret == 1) {
    if (*nav1_freq_stdby != INT_MISS) {
      integer = *nav1_freq_stdby / 100;
      decimal = *nav1_freq_stdby - integer*100;
      integer = min(max(integer + (int) updn,nav_min),nav_max);
      *nav1_freq_stdby = integer*100 + decimal;
      printf("NAV1 STDBY FREQ: %i \n",*nav1_freq_stdby);
    }
  }

  updn = 0.;
  ret = mastercard_encoder(device,card,5,&updn,5.0,2,3);
  if (ret == 1) {
    if (*nav1_freq_stdby != INT_MISS) {
      integer = *nav1_freq_stdby / 100;
      decimal = *nav1_freq_stdby - integer*100;
      decimal = (decimal + (int) updn) % 100;
      if (decimal < 0) decimal += 100;
      *nav1_freq_stdby = integer*100 + decimal;
      printf("NAV1 STDBY FREQ: %i \n",*nav1_freq_stdby);
    }
  }

  /* COM1 */
  updn = 0.;
  ret = mastercard_encoder(device,card,12,&updn,1.0,2,3);
  if (ret == 1) {
    if (*com1_freq_stdby != INT_MISS) {
      integer = *com1_freq_stdby / 1000;
      decimal = *com1_freq_stdby - integer*1000;
      integer = min(max(integer + (int) updn,com_min),com_max);
      *com1_freq_stdby = integer*1000 + decimal;
      printf("COM1 STDBY FREQ: %i \n",*com1_freq_stdby);
    }
  }
  updn = 0.;
  ret = mastercard_encoder(device,card,14,&updn,5.0,2,3);
  if (ret == 1) {
    if (*com1_freq_stdby != INT_MISS) {
      integer = *com1_freq_stdby / 1000;
      decimal = *com1_freq_stdby - integer*1000;
      decimal = (decimal + (int) updn) % 1000;
      if (decimal < 0) decimal += 1000;
      *com1_freq_stdby = integer*1000 + decimal;
      printf("COM1 STDBY FREQ: %i \n",*com1_freq_stdby);
    }
  }

  /* ADF1 */
  updn = 0.;
  ret = mastercard_encoder(device,card,22,&updn,100.0,2,3);
  if (ret == 1) {
    if (*adf1_freq_stdby != INT_MISS) {
      *adf1_freq_stdby += (int) updn;
      *adf1_freq_stdby = min(max(*adf1_freq_stdby,adf_min),adf_max);
      printf("ADF1 STDBY FREQ: %i \n",*adf1_freq_stdby);
    }
  }
  updn = 0.;
  ret = mastercard_encoder(device,card,24,&updn,1.0,2,3);
  if (ret == 1) {
    if (*adf1_freq_stdby != INT_MISS) {
      *adf1_freq_stdby += (int) updn;
      *adf1_freq_stdby = min(max(*adf1_freq_stdby,adf_min),adf_max);
      printf("ADF1 STDBY FREQ: %i \n",*adf1_freq_stdby);
    }
  }
  
  /* rudder trim */
  ret = mastercard_encoder(device,card,27,rudder_trim,-0.01,1,2);
  if (ret == 1) {
    *rudder_trim = min(max(*rudder_trim,trim_min),trim_max);
    printf("Rudder Trim: %f \n",*rudder_trim);
  }

  /* aileron trim. Both upper and lower switches have to be pressed */
  test1 = 0;
  test2 = 0;
  ret = digital_input(device,card,30,&test1,0);
  ret = digital_input(device,card,32,&test2,0);
  if ((test1 == 1) && (test2 == 1)) {
    if (*aileron_trim != FLT_MISS) {
      *aileron_trim += 0.0001;
      *aileron_trim = min(max(*aileron_trim,trim_min),trim_max);
      printf("Aileron Left Down: %f \n",*aileron_trim);
    }
  }

  test1 = 0;
  test2 = 0;
  ret = digital_input(device,card,29,&test1,0);
  ret = digital_input(device,card,31,&test2,0);
  if ((test1 == 1) && (test2 == 1)) {
    if (*aileron_trim != FLT_MISS) {
      *aileron_trim -= 0.0001;
      *aileron_trim = min(max(*aileron_trim,trim_min),trim_max);
      printf("Aileron Right Down: %f \n",*aileron_trim);
    }
  }
						
  /* Transponder */

  updn = 0.;
  ret = mastercard_encoder(device,card,63,&updn,100.0,1,2);
  if (ret == 1) {
    if (*transponder_code != INT_MISS) {
      *transponder_code += (int) updn;
      *transponder_code = min(max(*transponder_code,transponder_min),transponder_max);
      printf("TRANSPONDER CODE: %i \n",*transponder_code);
    }
  }
  updn = 0.;
  ret = mastercard_encoder(device,card,65,&updn,1.0,1,2);
  if (ret == 1) {
    if (*transponder_code != INT_MISS) {
      *transponder_code += (int) updn;
      *transponder_code = min(max(*transponder_code,transponder_min),transponder_max);
      printf("TRANSPONDER CODE: %i \n",*transponder_code);
    }
  }


  /* update outputs */

  /* background lighting: relais #1 */
  ret = digital_output(device,card,49,avionics_on);
 
  /* NAV1 decimal point */
  ret = digital_output(device,card,11,avionics_on);
  ret = digital_output(device,card,12,avionics_on);
  
  /* COM1 decimal point */
  ret = digital_output(device,card,13,avionics_on);
  ret = digital_output(device,card,14,avionics_on);
  
  /* ADF1 decimal point */
  ret = digital_output(device,card,15,avionics_on);
  ret = digital_output(device,card,16,avionics_on);
  
  /* SELCAL */
  ret = digital_output(device,card,20,hf2);
  ret = digital_output(device,card,21,hf1);
  ret = digital_output(device,card,22,vhf3);
  ret = digital_output(device,card,23,vhf2);
  ret = digital_output(device,card,24,vhf1);

  /* CARGO FIRE */
  if (*cargofire_test == 1) {
    /* CARGO FIRE extinguisher fault indicator */
    ret = digital_output(device,card,25,&one);
    ret = digital_output(device,card,26,&one);
    ret = digital_output(device,card,27,&one);
  } else {
    if (*cargofire_fault == 1) {
      ret = digital_output(device,card,25,&one);
    } else {
      ret = digital_output(device,card,25,&zero);
    }
    if (*cargofire_fwd_armed == 1) {
      ret = digital_output(device,card,26,&one);
    } else {
      ret = digital_output(device,card,26,&zero);
    }
    if (*cargofire_aft_armed == 1) {
      ret = digital_output(device,card,27,&one);
    } else {
      ret = digital_output(device,card,27,&zero);
    }
  }

  /* TRANSPONDER */
  if (*transponder_ident == 1) {
    ret = digital_output(device,card,28,&one);
  } else {
    ret = digital_output(device,card,28,&zero);
  }

  /* update displays */

  /* NAV1 */
  ret = mastercard_display(device,card,0,5, nav1_freq_active, 0);
  ret = mastercard_display(device,card,5,5, nav1_freq_stdby, 0);

  /* COM1 */
  ret = mastercard_display(device,card,10,6,com1_freq_active, 0);
  ret = mastercard_display(device,card,16,6,com1_freq_stdby, 0);

  /* ADF1 */
  temp = *adf1_freq_active * 10;
  ret = mastercard_display(device,card,22,5, &temp, 0);
  temp = *adf1_freq_stdby * 10;
  ret = mastercard_display(device,card,27,5, &temp, 0);

  /* TRANSPONDER */
  if ((*transponder_mode > 0) && (*transponder_fail != 6)) {
    ret = mastercard_display(device,card,32,4, transponder_code, 0);
  } else {
    ret = mastercard_display(device,card,32,4, &ten, 1);
  }

  if (*avionics_on != 1) {
    ret = mastercard_display(device,card,0,36, &ten, 1);
  }
  
  /***** mastercard #1 connected to USB Expansion Card device 0 *****/
  device = 0;
  card = 1;

  /* read inputs */
  if ((acf_type == 1) || (acf_type == 2) || (acf_type == 3)) {
    ret = digital_input(device,card,63,&test,0);
    if (ret == 1) {
      printf("Fire TEST OVHT Switch: %i \n",test);
      if (acf_type == 1) {
	*fire_test = 1+test;
      } else {
	*fire_test_ovht = test;
      }
    }

    ret = digital_input(device,card,64,&test,0);
    if (ret == 1) {
      printf("Fire TEST FAULT Switch: %i \n",test);
      if (acf_type == 1) {
	*fire_test = 1-test;
      } else {
	*fire_test_fault = test;
      }
    }
    
    ret = digital_input(device,card,71,&test,0);
    if (ret == 1) {
      printf("Fire Engine 1 Ext Test: %i \n",test);
      if (acf_type == 1) {
	*fire_eng_ext_test = 1-test;
      } else {
	*fire_eng_ext_test_left = test;
      }
    }

    ret = digital_input(device,card,69,&test,0);
    if (ret == 1) {
      printf("Fire Engine 2 Ext Test: %i \n",test);
      if (acf_type == 1) {
	*fire_eng_ext_test = 1+test;
      } else {
	*fire_eng_ext_test_right = test;
      }
    }

    ret = digital_input(device,card,67,&test,0);
    if (ret == 1) {
      printf("Fire Bell Cutout: %i \n",test);
    }

    /* fire pull and rotate switches for Engines 1, 2 and APU */
    ret = digital_input(device,card,55,&test,0);
    if (ret == 1) printf("Fire Engine 1 DISCH Pulled: %i \n",1-test);
    if (acf_type == 1) {
      *fire_eng1_pulled = 1-test;
    } else {
      if (((1-test) == 1) && (*fire_eng1_pulled_status < 0.01)) {
	*fire_eng1_pulled = 1;
      }
      if (((1-test) == 0) && (*fire_eng1_pulled_status >= 0.99)) {
	*fire_eng1_pulled = 1;
      }
    }

    ret = digital_input(device,card,58,&test,0);
    if (ret == 1) printf("Fire APU DISCH Pulled: %i \n",1-test);
    if (acf_type == 1) {
      *fire_apu_pulled = 1-test;
    } else {
      if (((1-test) == 1) && (*fire_apu_pulled_status < 0.01)) {
	*fire_apu_pulled = 1;
      }
      if (((1-test) == 0) && (*fire_apu_pulled_status >= 0.99)) {
	*fire_apu_pulled = 1;
      }
    }
  
    ret = digital_input(device,card,61,&test,0);
    if (ret == 1) printf("Fire Engine 2 DISCH Pulled: %i \n",1-test);
    if (acf_type == 1) {
      *fire_eng2_pulled = 1-test;
    } else {
      if (((1-test) == 1) && (*fire_eng2_pulled_status < 0.01)) {
	*fire_eng2_pulled = 1;
      }
      if (((1-test) == 0) && (*fire_eng2_pulled_status >= 0.99)) {
	*fire_eng2_pulled = 1;
      }
    }

    ret = digital_input(device,card,54,&test1,0);
    if (ret == 1) printf("Fire Engine 1 DISCH L: %i \n",test1);
    ret = digital_input(device,card,56,&test2,0);
    if (ret == 1) printf("Fire Engine 1 DISCH R: %i \n",test2);

    if (acf_type == 1) {
      *fire_eng1_rotate = -test1;
      *fire_eng1_rotate = test2;
    } else {
      test = -test1 + test2;

      if (((float) test) > (*fire_eng1_rotate_status+0.99)) {
	*fire_eng1_rotate_r = 1;
      }     
      if (((float) test) < (*fire_eng1_rotate_status-0.99)) {
	*fire_eng1_rotate_l = 1;
      }     
    }
    
    ret = digital_input(device,card,57,&test1,0);
    if (ret == 1) printf("Fire APU DISCH L: %i \n",test1);
    ret = digital_input(device,card,59,&test2,0);
    if (ret == 1) printf("Fire APU 1 DISCH R: %i \n",test2);

    if (acf_type == 1) {
      *fire_apu_rotate = -test1;
      *fire_apu_rotate = test2;
    } else {
      test = -test1 + test2;

      if (((float) test) > (*fire_apu_rotate_status+0.99)) {
	*fire_apu_rotate_r = 1;
      }     
      if (((float) test) < (*fire_apu_rotate_status-0.99)) {
	*fire_apu_rotate_l = 1;
      }     
    }
    
    ret = digital_input(device,card,60,&test1,0);
    if (ret == 1) printf("Fire Engine 2 DISCH L: %i \n",test1);
    ret = digital_input(device,card,62,&test2,0);
    if (ret == 1) printf("Fire Engine 2 DISCH R: %i \n",test2);

    if (acf_type == 1) {
      *fire_eng2_rotate = -test1;
      *fire_eng2_rotate = test2;
    } else {
      test = -test1 + test2;

      if (((float) test) > (*fire_eng2_rotate_status+0.99)) {
	*fire_eng2_rotate_r = 1;
      }     
      if (((float) test) < (*fire_eng2_rotate_status-0.99)) {
	*fire_eng2_rotate_l = 1;
      }     
    }
    
  }

  if (acf_type == 1) {

    ret = digital_input(device,card,65,&test,0);
    if (ret == 1) {
      printf("Fire Engine 1 OVHT DET Switch B: %i \n",test);
      *fire_eng1_ovht_det = 1+test;
    }

    ret = digital_input(device,card,66,&test,0);
    if (ret == 1) {
      printf("Fire Engine 1 OVHT DET Switch A: %i \n",test);
      *fire_eng1_ovht_det = 1-test;
    }

    ret = digital_input(device,card,68,&test,0);
    if (ret == 1) {
      printf("Fire Engine 2 OVHT DET Switch B: %i \n",test);
      *fire_eng2_ovht_det = 1+test;
    }
    ret = digital_input(device,card,70,&test,0);
    if (ret == 1) {
      printf("Fire Engine 2 OVHT DET Switch A: %i \n",test);
      *fire_eng2_ovht_det = 1-test;
    }

  }

  /* NAV2 test button */
  ret = digital_input(device,card,9,&test,0);
  if (ret == 1) {
    printf("NAV2 TEST BUTTON: %i \n",test);
  }

  /* NAV2 tfr button */
  ret = digital_input(device,card,10,&tfr,0);
  if (ret == 1) {
    if (tfr == 1) {
      temp = *nav2_freq_active;
      *nav2_freq_active = *nav2_freq_stdby;
      *nav2_freq_stdby = temp;
    }
  }

  /* COM2 test button */
  ret = digital_input(device,card,1,&test,0);
  if (ret == 1) {
    printf("COM2 TEST BUTTON: %i \n",test);
  }

  /* COM2 tfr button */
  ret = digital_input(device,card,2,&tfr,0);
  if (ret == 1) {
    if (tfr == 1) {
      temp = *com2_freq_active;
      *com2_freq_active = *com2_freq_stdby;
      *com2_freq_stdby = temp;
    }
  }
 
  /* ADF2 switch 1 */
  ret = digital_input(device,card,18,&test,0);
  if (ret == 1) {
    printf("ADF2 ANT SWITCH: %i \n",test);
  }

  /* ADF2 switch 2 */
  ret = digital_input(device,card,19,&test,0);
  if (ret == 1) {
    printf("ADF2 TONE SWITCH: %i \n",test);
  }

  /* ADF2 tfr button */
  ret = digital_input(device,card,20,&tfr,0);
  if (ret == 1) {
    if (tfr == 1) {
      temp = *adf2_freq_active;
      *adf2_freq_active = *adf2_freq_stdby;
      *adf2_freq_stdby = temp;
    }
  }

  /* STAB TRIM MODE */
  ret = digital_input(device,card,27,&test,0);
  if (ret == 1) {
    printf("STAB Trim Mode: %i \n",test);
    *stab_trim_mode = test;
  }

  /* Flight Deck Door */
  ret = digital_input(device,card,28,&test,0);
  if (ret == 1) {
    printf("Flight Deck Door AUTO: %i \n",test);
  }
  ret = digital_input(device,card,29,&test2,0);
  if (ret == 1) {
    printf("Flight Deck Door DENY: %i \n",test2);
  }
  if ((test != INT_MISS) && (test2 != INT_MISS)) {
    *flt_dk_door = (float) test - 1 + test2*2;
  }

  
  /* read encoders */

  /* NAV2 */
  updn = 0.;
  ret = mastercard_encoder(device,card,11,&updn,1.0,2,3);
  if (ret == 1) {
    if (*nav2_freq_stdby != INT_MISS) {
      integer = *nav2_freq_stdby / 100;
      decimal = *nav2_freq_stdby - integer*100;
      integer = min(max(integer + (int) updn,nav_min),nav_max);
      *nav2_freq_stdby = integer*100 + decimal;
      printf("NAV2 STDBY FREQ: %i \n",*nav2_freq_stdby);
    }
  }

  updn = 0.;
  ret = mastercard_encoder(device,card,13,&updn,5.0,2,3);
  if (ret == 1) {
    if (*nav2_freq_stdby != INT_MISS) {
      integer = *nav2_freq_stdby / 100;
      decimal = *nav2_freq_stdby - integer*100;
      decimal = (decimal + (int) updn) % 100;
      if (decimal < 0) decimal += 100;
      *nav2_freq_stdby = integer*100 + decimal;
      printf("NAV2 STDBY FREQ: %i \n",*nav2_freq_stdby);
    }
  }

  /* COM2 */
  updn = 0.;
  ret = mastercard_encoder(device,card,3,&updn,1.0,2,3);
  if (ret == 1) {
    if (*com2_freq_stdby != INT_MISS) {
      integer = *com2_freq_stdby / 1000;
      decimal = *com2_freq_stdby - integer*1000;
      integer = min(max(integer + (int) updn,com_min),com_max);
      *com2_freq_stdby = integer*1000 + decimal;
      printf("COM2 STDBY FREQ: %i \n",*com2_freq_stdby);
    }
  }
  updn = 0.;
  ret = mastercard_encoder(device,card,5,&updn,5.0,2,3);
  if (ret == 1) {
    if (*com2_freq_stdby != INT_MISS) {
      integer = *com2_freq_stdby / 1000;
      decimal = *com2_freq_stdby - integer*1000;
      decimal = (decimal + (int) updn) % 1000;
      if (decimal < 0) decimal += 1000;
      *com2_freq_stdby = integer*1000 + decimal;
      printf("COM2 STDBY FREQ: %i \n",*com2_freq_stdby);
    }
  }


  /* ADF2 */
  updn = 0.;
  ret = mastercard_encoder(device,card,21,&updn,100.0,2,3);
  if (ret == 1) {
    if (*adf2_freq_stdby != INT_MISS) {
      *adf2_freq_stdby += (int) updn;
      *adf2_freq_stdby = min(max(*adf2_freq_stdby,adf_min),adf_max);
      printf("ADF2 STDBY FREQ: %i \n",*adf2_freq_stdby);
    }
  }
  updn = 0.;
  ret = mastercard_encoder(device,card,23,&updn,1.0,2,3);
  if (ret == 1) {
    if (*adf2_freq_stdby != INT_MISS) {
      *adf2_freq_stdby += (int) updn;
      *adf2_freq_stdby = min(max(*adf2_freq_stdby,adf_min),adf_max);
      printf("ADF2 STDBY FREQ: %i \n",*adf2_freq_stdby);
    }
  }
 
  /* update outputs */

  /* Fire Panel LED's */
  if ((acf_type == 1) || (acf_type == 2) || (acf_type == 3)) {
    if (*fire_eng1_ovht_ann > 0.5) {
      ret = digital_output(device,card,26,&one); // ENG1 OVHT Ann
    } else {
      ret = digital_output(device,card,26,&zero); // ENG1 OVHT Ann
    }
    if (*fire_eng2_ovht_ann > 0.5) {
      ret = digital_output(device,card,31,&one); // ENG2 OVHT Ann
    } else {
      ret = digital_output(device,card,31,&zero); // ENG2 OVHT Ann
    }
    if (*fire_wheelwell_ann > 0.5) {
      ret = digital_output(device,card,27,&one); // Wheel Well Ann
    } else {
      ret = digital_output(device,card,27,&zero); // Wheel Well Ann
    }
    if (*fire_fault_ann > 0.5) {
      ret = digital_output(device,card,28,&one); // FAULT Ann
    } else {
      ret = digital_output(device,card,28,&zero); // FAULT Ann
    }
    if (*fire_apu_det_inop_ann > 0.5) {
      ret = digital_output(device,card,29,&one); // APU Det INOP Ann
    } else {
      ret = digital_output(device,card,29,&zero); // APU Det INOP Ann
    }
    if (*fire_apu_bottle_disch_ann > 0.5) {
      ret = digital_output(device,card,30,&one); // APU Bottle DISCH Ann
    } else {
      ret = digital_output(device,card,30,&zero); // APU Bottle DISCH Ann
    }
    if (*fire_bottle_l_disch_ann > 0.5) {
      ret = digital_output(device,card,32,&one); // L Bottle DISCH Ann
    } else {
      ret = digital_output(device,card,32,&zero); // L Bottle DISCH Ann
    }
    if (*fire_bottle_r_disch_ann > 0.5) {
      ret = digital_output(device,card,33,&one); // R Bottle DISCH Ann
    } else {
      ret = digital_output(device,card,33,&zero); // R Bottle DISCH Ann
    }
    if (*fire_eng1_ann > 0.5) {
      ret = digital_output(device,card,23,&one); // ENG1 Fire Ann
    } else {
      ret = digital_output(device,card,23,&zero); // ENG1 Fire Ann
    }
    if (*fire_apu_ann > 0.5) {
      ret = digital_output(device,card,24,&one); // APU Fire Ann
    } else {
      ret = digital_output(device,card,24,&zero); // APU Fire Ann
    }
    if (*fire_eng2_ann > 0.5) {
      ret = digital_output(device,card,25,&one); // ENG2 Fire Ann
    } else {
      ret = digital_output(device,card,25,&zero); // ENG2 Fire Ann
    }
    if (*fire_eng1_test_ann > 0.5) {
      ret = digital_output(device,card,34,&one); // Ext Test ENG1
    } else {
      ret = digital_output(device,card,34,&zero); // Ext Test ENG1
    }
    if (*fire_apu_test_ann > 0.5) {
      ret = digital_output(device,card,35,&one); // Ext Test APU
    } else {
      ret = digital_output(device,card,35,&zero); // Ext Test APU
    }
    if (*fire_eng2_test_ann > 0.5) {
      ret = digital_output(device,card,36,&one); // Ext Test ENG2
    } else {
      ret = digital_output(device,card,36,&zero); // Ext Test ENG2
    }
    if (*fire_eng1_test_ann > 0.5) {
      ret = digital_output(device,card,34,&one); // Ext Test ENG1
    } else {
      ret = digital_output(device,card,34,&zero); // Ext Test ENG1
    }
    if (*fire_apu_test_ann > 0.5) {
      ret = digital_output(device,card,35,&one); // Ext Test APU
    } else {
      ret = digital_output(device,card,35,&zero); // Ext Test APU
    }
    if (*fire_eng2_test_ann > 0.5) {
      ret = digital_output(device,card,36,&one); // Ext Test ENG2
    } else {
      ret = digital_output(device,card,36,&zero); // Ext Test ENG2
    }
  }
  
  /* NAV2 decimal point */
  ret = digital_output(device,card,11,avionics_on);
  ret = digital_output(device,card,12,avionics_on);
  
  /* COM2 decimal point */
  ret = digital_output(device,card,13,avionics_on);
  ret = digital_output(device,card,14,avionics_on);
  
  /* ADF2 decimal point */
  ret = digital_output(device,card,15,avionics_on);
  ret = digital_output(device,card,16,avionics_on);

  /* Flight Deck Door Lock Indicator */
  ret = digital_outputf(device,card,17,lock_fail);
  ret = digital_outputf(device,card,18,auto_unlk);

  /* update displays */

  /* NAV2 */
  ret = mastercard_display(device,card,0,5, nav2_freq_active, 0);
  ret = mastercard_display(device,card,5,5, nav2_freq_stdby, 0);

  /* COM2 */
  ret = mastercard_display(device,card,10,6,com2_freq_active, 0);
  ret = mastercard_display(device,card,16,6,com2_freq_stdby, 0);

  /* ADF2 */
  temp = *adf2_freq_active * 10;
  ret = mastercard_display(device,card,22,5, &temp, 0);
  temp = *adf2_freq_stdby * 10;
  ret = mastercard_display(device,card,27,5, &temp, 0);

  if (*avionics_on != 1) {
    ret = mastercard_display(device,card,0,36, &ten, 1);
  }


  /***** USBServo Card device 1 *****/
 

  /* read analog input #1 and #2 on USBServo card, scale it to 0-1 */  
  device = 1;
  ret = axis_input(device,0,&flood_brightness,0.0,1.0);
  if (ret == 1) {
    //    printf("flood brightness: %f \n",flood_brightness);
  }
  ret = axis_input(device,1,&panel_brightness,0.0,1.0);
  if (ret == 1) {
    //    printf("panel brightness: %f \n",panel_brightness);
  }

  ret = axis_input(device,2,radio_volume1,0.0,1.0);
  ret = axis_input(device,2,radio_volume2,0.0,1.0);
  if (ret == 1) {
    // printf("radio volume: %f \n",*radio_volume1);
  }
  ret = axis_input(device,3,master_volume,0.0,1.0);
  if (ret == 1) {
    //    printf("engine volume: %f \n",*master_volume);
  }

  /* steer servo for rudder position indicator */  
  ret = servos_output(device,0,rudder_trim,-1.0,1.0,425,750);
  
}

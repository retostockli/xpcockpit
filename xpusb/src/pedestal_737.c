/* This is the pedestal_737.c code which contains a sample set-up for how to communicate with the 
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
#include "pedestal_737.h"

#define max(A,B) ((A)>(B) ? (A) : (B)) 
#define min(A,B) ((A)<(B) ? (A) : (B)) 

void pedestal_737(void)
{

  int device;
  int card;

  //  int *status_x737 = link_dataref_int("x737/systems/afds/plugin_status");
  int *status_x737 = link_dataref_int("xpserver/status_737");

  /* x-plane data */
  int *nav1_freq_active = link_dataref_int("sim/cockpit/radios/nav1_freq_hz");
  int *nav2_freq_active = link_dataref_int("sim/cockpit/radios/nav2_freq_hz");
  int *com1_freq_active = link_dataref_int("sim/cockpit/radios/com1_freq_hz");
  int *com2_freq_active = link_dataref_int("sim/cockpit/radios/com2_freq_hz");
  int *adf1_freq_active = link_dataref_int("sim/cockpit/radios/adf1_freq_hz");
  int *adf2_freq_active = link_dataref_int("sim/cockpit/radios/adf2_freq_hz");
  //  int *dme_freq_active = link_dataref_int("sim/cockpit/radios/dme_freq_hz");
  int *nav1_freq_stdby = link_dataref_int("sim/cockpit/radios/nav1_stdby_freq_hz");
  int *nav2_freq_stdby = link_dataref_int("sim/cockpit/radios/nav2_stdby_freq_hz");
  int *com1_freq_stdby = link_dataref_int("sim/cockpit/radios/com1_stdby_freq_hz");
  int *com2_freq_stdby = link_dataref_int("sim/cockpit/radios/com2_stdby_freq_hz");
  int *adf1_freq_stdby = link_dataref_int("sim/cockpit/radios/adf1_stdby_freq_hz");
  int *adf2_freq_stdby = link_dataref_int("sim/cockpit/radios/adf2_stdby_freq_hz"); 
  //  int *dme_freq_stdby = link_dataref_int("sim/cockpit/radios/dme_stdby_freq_hz");

  int *transponder_code = link_dataref_int("sim/cockpit/radios/transponder_code");
  int *transponder_mode = link_dataref_int("sim/cockpit/radios/transponder_mode");
  int *transponder_fail = link_dataref_int("sim/operation/failures/rel_xpndr");
  int *transponder_ident = link_dataref_int("sim/cockpit/radios/transponder_id");

  int *transponder_mode_up;
  int *transponder_mode_dn;
  if ((*status_x737 == 2) || (*status_x737 == 3)) {
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
  if ((*status_x737 == 2) || (*status_x737 == 3)) {
    fire_test = link_dataref_int("laminar/B738/toggle_switch/fire_test"); // -1: fault, 0: off, 1: ovht
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
  } else if (*status_x737 == 1) {
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


  
  /* unimplemented datarefs (not available in X-Plane and not in x737) */
  int *vhf1 = link_dataref_int("xpserver/selcal/vhf1");
  int *vhf2 = link_dataref_int("xpserver/selcal/vhf2");
  int *vhf3 = link_dataref_int("xpserver/selcal/vhf3");
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
  int *stab_trim_mode = link_dataref_int("xpserver/stab_trim_mode");
  int *flt_dk_door = link_dataref_int("xpserver/flt_dk_door");

  float *radio_volume;
  if (*status_x737 == 1) {
    radio_volume = link_dataref_flt("x737/cockpit/ACP/ACP1/volumeControl01",-2);
  } else {
    radio_volume = link_dataref_flt("sim/operation/sound/radio_volume_ratio",-2);
  }
  float *master_volume = link_dataref_flt("sim/operation/sound/master_volume_ratio",-2);

  // test for lights
  int *beacon_lights_737;
  float *landing_lights_lf_737;
  float *landing_lights_rf_737;
  float *landing_lights_lr_737;
  float *landing_lights_rr_737;
  int *logo_lights_737;
  float *logo_lights_flt;
  int *position_lights_737;
  int *wheelwell_lights_737;
  float *wheelwell_lights_flt;
  float *taxi_lights_737;
  float *taxi_lights_pos;
  float *wing_lights_737;
  float *rwy_turn_lights_l_737;
  float *rwy_turn_lights_r_737;
  float *position_lights_flt;
  int *position_lights_up;
  int *position_lights_dn;

  int *beacon_lights;
  int *landing_lights;
  int *nav_lights;
  int *strobe_lights;
  int *taxi_lights;
  if ((*status_x737 == 2) || (*status_x737 == 3)) {
    //    landing_lights_lf_737 = link_dataref_flt_arr("sim/cockpit2/switches/landing_lights_switch",16,0,-1);
    //    landing_lights_rf_737 = link_dataref_flt_arr("sim/cockpit2/switches/landing_lights_switch",16,3,-1);
    landing_lights_lf_737 = link_dataref_flt("laminar/B738/switch/land_lights_left_pos",-1);
    landing_lights_rf_737 = link_dataref_flt("laminar/B738/switch/land_lights_right_pos",-1);

    rwy_turn_lights_l_737 = link_dataref_flt_arr("sim/cockpit2/switches/generic_lights_switch",128,2,-1);
    rwy_turn_lights_r_737 = link_dataref_flt_arr("sim/cockpit2/switches/generic_lights_switch",128,3,-1);

    logo_lights_flt = link_dataref_flt_arr("sim/cockpit2/switches/generic_lights_switch",128,1,-1);    
    beacon_lights = link_dataref_int("sim/cockpit/electrical/beacon_lights_on");
    nav_lights = link_dataref_int("sim/cockpit/electrical/nav_lights_on");
    position_lights_flt = link_dataref_flt("laminar/B738/toggle_switch/position_light_pos",-1);
    position_lights_up = link_dataref_cmd_once("laminar/B738/toggle_switch/position_light_up");
    position_lights_dn = link_dataref_cmd_once("laminar/B738/toggle_switch/position_light_down");
    *position_lights_up = 0;
    *position_lights_dn = 0;

    wing_lights_737 = link_dataref_flt_arr("sim/cockpit2/switches/generic_lights_switch",128,0,-1);
    wheelwell_lights_flt = link_dataref_flt_arr("sim/cockpit2/switches/generic_lights_switch",128,5,-1);

    taxi_lights = link_dataref_cmd_once("laminar/B738/toggle_switch/taxi_light_brigh_toggle");
    *taxi_lights = 0;
    taxi_lights_pos = link_dataref_flt("laminar/B738/toggle_switch/taxi_light_brightness_pos",-1);
  } else if (*status_x737 == 1) {
    beacon_lights_737 = link_dataref_int("x737/systems/exteriorLights/beaconLightSwitch");
    landing_lights_lf_737 = link_dataref_flt("x737/systems/exteriorLights/leftFixedLanLtSwitch",-2);
    landing_lights_rf_737 = link_dataref_flt("x737/systems/exteriorLights/rightFixedLanLtSwitch",-2);
    landing_lights_lr_737 = link_dataref_flt("x737/systems/exteriorLights/leftRetrLanLtSwitch",-2);
    landing_lights_rr_737 = link_dataref_flt("x737/systems/exteriorLights/rightRetrLanLtSwitch",-2);
    logo_lights_737 = link_dataref_int("x737/systems/exteriorLights/logoLightSwitch");
    position_lights_737 = link_dataref_int("x737/systems/exteriorLights/positionLightSwitch");
    wheelwell_lights_737 = link_dataref_int("x737/systems/exteriorLights/wheelWellLightsSwitch");
    taxi_lights_737 = link_dataref_flt("x737/systems/exteriorLights/taxiLightsSwitch",-2);
    wing_lights_737 = link_dataref_flt("x737/systems/exteriorLights/wingLightsSwitch",-2);
  } else {
    beacon_lights = link_dataref_int("sim/cockpit/electrical/beacon_lights_on");
    landing_lights = link_dataref_int("sim/cockpit/electrical/landing_lights_on");
    nav_lights = link_dataref_int("sim/cockpit/electrical/nav_lights_on");
    strobe_lights = link_dataref_int("sim/cockpit/electrical/strobe_lights_on");
    taxi_lights = link_dataref_int("sim/cockpit/electrical/taxi_light_on");
  }
    

  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");

  // test for tail hook (fighter, carrier)
  int *tailhook = link_dataref_int("sim/cockpit2/switches/tailhook_deploy");

 
  /* local variables */
  int ret;
  float updn;
  int test;
  int test1; 
  int test2;
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
  if (*vhf1 == INT_MISS) *vhf1 = 0;
  if (*vhf2 == INT_MISS) *vhf2 = 0;
  if (*vhf3 == INT_MISS) *vhf3 = 0;
  if (*hf1 == INT_MISS) *hf1 = 0;
  if (*hf2 == INT_MISS) *hf2 = 0;
  ret = digital_input(device,card,56,hf2,1);
  if (ret == 1) {
    printf("SELCAL HF2 button: %i \n",*hf2);
  }
  ret = digital_input(device,card,57,hf1,1);
  if (ret == 1) {
    printf("SELCAL HF1 button: %i \n",*hf1);
  }
  ret = digital_input(device,card,58,vhf3,1);
  if (ret == 1) {
    printf("SELCAL VHF3 button: %i \n",*vhf3);
  }
  ret = digital_input(device,card,59,vhf2,1);
  if (ret == 1) {
    printf("SELCAL VHF2 button: %i \n",*vhf2);
  }
  ret = digital_input(device,card,60,vhf1,1);
  if (ret == 1) {
    printf("SELCAL VHF1 button: %i \n",*vhf1);
  }

  // assign vhf/hf buttons to light switches
  if ((*status_x737 == 2) || (*status_x737 == 3)) {
    *landing_lights_lf_737 = (float) *vhf1;
    *landing_lights_rf_737 = (float) *vhf1;
    *rwy_turn_lights_l_737 = (float) *vhf2;
    *rwy_turn_lights_r_737 = (float) *vhf2;
    if (*vhf3 != *taxi_lights_pos/2) {
      *taxi_lights = 1;
    }
    *logo_lights_flt = (float) *hf2;
    *beacon_lights = *hf1;
    *nav_lights = *hf1;
    if (*hf1 > *position_lights_flt) {
      *position_lights_up = 1;
    }
    if (*hf1 < *position_lights_flt) {
      *position_lights_dn = 1;
    }

    *wing_lights_737 = (float) *hf2;
    *wheelwell_lights_flt = (float) *vhf3;
    
  } else if (*status_x737 == 1) {
    *beacon_lights_737 = *hf1;
    *landing_lights_lf_737 = (float) *vhf1;
    *landing_lights_rf_737 = (float) *vhf1;
    *landing_lights_lr_737 = (float) 2 * *vhf1;
    *landing_lights_rr_737 = (float) 2 * *vhf1;
    *logo_lights_737 = *hf2;
    *position_lights_737 = *hf1;
    *wheelwell_lights_737 = *hf2;
    *taxi_lights_737 = (float) *vhf3;
    *wing_lights_737 = (float) *hf2;
  } else {
    *beacon_lights = *hf1;
    *landing_lights = *vhf1;
    *nav_lights = *hf1;
    *strobe_lights = *hf1;
    *taxi_lights = *vhf3;
  }
  ret = digital_input(device,card,55,&test,0);
  if (ret == 1) {
    printf("WX Radar Switch: %i \n",test);
    *wxsel = test;
  }
  ret = digital_input(device,card,54,&test,0);
  if (ret == 1) {
    printf("WX Radar Switch: %i \n",test*2);
    *wxsel = test*2;
  }

  /* CARGO FIRE */
  ret = digital_input(device,card,45,&test,0);
  if (ret == 1) {
    printf("Cargo Fire DISCH Button: %i \n",test);
    *cargofire_disch = test;

    if (test == 0) {
      *cargofire_fwd_armed = test;
      *cargofire_aft_armed = test;
    }
  }
  if (*cargofire_aft_armed == INT_MISS) *cargofire_aft_armed = 0;
  ret = digital_input(device,card,46,&test,1);
  if (ret == 1) {
    printf("Cargo Fire AFT ARMED Button: %i \n",test);
    *cargofire_aft_armed = test;
  }
  if (*cargofire_fwd_armed == INT_MISS) *cargofire_fwd_armed = 0;
  ret = digital_input(device,card,47,&test,1);
  if (ret == 1) {
    printf("Cargo Fire FWD ARMED Button: %i \n",test);
    *cargofire_fwd_armed = test;
  }
  ret = digital_input(device,card,48,&test,0);
  if (ret == 1) {
    printf("Cargo Fire TEST Button: %i \n",test);
    *cargofire_test = test;
  }
  ret = digital_input(device,card,49,&test,0);
  if (ret == 1) {
    printf("Cargo Fire AFT Switch A: %i \n",test);
    *cargofire_aft_select = test;
  }
  ret = digital_input(device,card,50,&test,0);
  if (ret == 1) {
    printf("Cargo Fire AFT Switch B: %i \n",test*2);
    *cargofire_aft_select = test*2;
  }
  ret = digital_input(device,card,51,&test,0);
  if (ret == 1) {
    printf("Cargo Fire FWD Switch A: %i \n",test);
    *cargofire_fwd_select = test;
  }
  ret = digital_input(device,card,52,&test,0);
  if (ret == 1) {
    printf("Cargo Fire FWD Switch B: %i \n",test*2);
    *cargofire_fwd_select = test*2;
  }

  *tailhook = *cargofire_fwd_armed;

  /* TRANSPONDER */


  ret = digital_input(device,card,67,&test,0);
  if (ret == 1) {
    if ((*status_x737 == 2) || (*status_x737 == 3)) {
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
    if ((*status_x737 == 2) || (*status_x737 == 3)) {
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
      decimal = min(max(decimal + (int) updn,0),95);
      *nav1_freq_stdby = integer*100 + decimal;
      printf("NAV1 STDBY FREQ: %i \n",*nav1_freq_stdby);
    }
  }

  /* COM1 */
  updn = 0.;
  ret = mastercard_encoder(device,card,12,&updn,1.0,2,3);
  if (ret == 1) {
    if (*com1_freq_stdby != INT_MISS) {
      integer = *com1_freq_stdby / 100;
      decimal = *com1_freq_stdby - integer*100;
      integer = min(max(integer + (int) updn,com_min),com_max);
      *com1_freq_stdby = integer*100 + decimal;
      printf("COM1 STDBY FREQ: %i \n",*com1_freq_stdby);
    }
  }
  updn = 0.;
  ret = mastercard_encoder(device,card,14,&updn,25.0,2,3);
  if (ret == 1) {
    if (*com1_freq_stdby != INT_MISS) {
      integer = *com1_freq_stdby / 100;
      decimal = (int) (roundf(((float) ((*com1_freq_stdby - integer*100)*10))/25.0)*25.0);
      decimal = min(max(decimal + (int) updn,0),975);
      *com1_freq_stdby = integer*100 + decimal/10;
      printf("COM1 STDBY FREQ: %i \n",*com1_freq_stdby);
    }
  }

  /* ADF1 */
  updn = 0.;
  ret = mastercard_encoder(device,card,22,&updn,10.0,2,3);
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
  temp = *com1_freq_active * 10;
  ret = mastercard_display(device,card,10,6, &temp, 0);
  temp = *com1_freq_stdby * 10;
  ret = mastercard_display(device,card,16,6, &temp, 0);

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
  if ((*status_x737 == 1) || (*status_x737 == 2) || (*status_x737 == 3)) {
    ret = digital_input(device,card,63,&test,0);
    if (ret == 1) {
      printf("Fire TEST OVHT Switch: %i \n",test);
      if (*status_x737 == 1) {
	*fire_test = 1+test;
      } else {
	*fire_test_ovht = test;
      }
    }

    ret = digital_input(device,card,64,&test,0);
    if (ret == 1) {
      printf("Fire TEST FAULT Switch: %i \n",test);
      if (*status_x737 == 1) {
	*fire_test = 1-test;
      } else {
	*fire_test_fault = test;
      }
    }
    
    ret = digital_input(device,card,71,&test,0);
    if (ret == 1) {
      printf("Fire Engine 1 Ext Test: %i \n",test);
      if (*status_x737 == 1) {
	*fire_eng_ext_test = 1-test;
      } else {
	*fire_eng_ext_test_left = test;
      }
    }

    ret = digital_input(device,card,69,&test,0);
    if (ret == 1) {
      printf("Fire Engine 2 Ext Test: %i \n",test);
      if (*status_x737 == 1) {
	*fire_eng_ext_test = 1+test;
      } else {
	*fire_eng_ext_test_right = test;
      }
    }

    /* fire pull and rotate switches for Engines 1, 2 and APU */
    ret = digital_input(device,card,55,&test,0);
    if (ret == 1) printf("Fire Engine 1 DISCH Pulled: %i \n",1-test);
    if (*status_x737 == 1) {
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
    if (*status_x737 == 1) {
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
    if (*status_x737 == 1) {
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

    if (*status_x737 == 1) {
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

    if (*status_x737 == 1) {
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

    if (*status_x737 == 1) {
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

  if (*status_x737 == 1) {

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
    *flt_dk_door = test;
  }

  ret = digital_input(device,card,29,&test,0);
  if (ret == 1) {
    printf("Flight Deck Door DENY: %i \n",test);
    *flt_dk_door = test*2;
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
      decimal = min(max(decimal + (int) updn,0),95);
      *nav2_freq_stdby = integer*100 + decimal;
      printf("NAV2 STDBY FREQ: %i \n",*nav2_freq_stdby);
    }
  }

  /* COM2 */
  updn = 0.;
  ret = mastercard_encoder(device,card,3,&updn,1.0,2,3);
  if (ret == 1) {
    if (*com2_freq_stdby != INT_MISS) {
      integer = *com2_freq_stdby / 100;
      decimal = *com2_freq_stdby - integer*100;
      integer = min(max(integer + (int) updn,com_min),com_max);
      *com2_freq_stdby = integer*100 + decimal;
      printf("COM2 STDBY FREQ: %i \n",*com2_freq_stdby);
    }
  }
  updn = 0.;
  ret = mastercard_encoder(device,card,5,&updn,25.0,2,3);
  if (ret == 1) {
    if (*com2_freq_stdby != INT_MISS) {
      integer = *com2_freq_stdby / 100;
      decimal = (int) (roundf(((float) ((*com2_freq_stdby - integer*100)*10))/25.0)*25.0);
      decimal = min(max(decimal + (int) updn,0),975);
      *com2_freq_stdby = integer*100 + decimal/10;
      printf("COM2 STDBY FREQ: %i \n",*com2_freq_stdby);
    }
  }


  /* ADF2 */
  updn = 0.;
  ret = mastercard_encoder(device,card,21,&updn,10.0,2,3);
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
  if ((*status_x737 == 1) || (*status_x737 == 2) || (*status_x737 == 3)) {
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
  if (*flt_dk_door == 1) {
    ret = digital_output(device,card,18,&one);
  } else {
    ret = digital_output(device,card,18,&zero);
  }
  ret = digital_output(device,card,17,&one);

  /* update displays */

  /* NAV2 */
  ret = mastercard_display(device,card,0,5, nav2_freq_active, 0);
  ret = mastercard_display(device,card,5,5, nav2_freq_stdby, 0);

  /* COM2 */
  temp = *com2_freq_active * 10;
  ret = mastercard_display(device,card,10,6, &temp, 0);
  temp = *com2_freq_stdby * 10;
  ret = mastercard_display(device,card,16,6, &temp, 0);

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

  ret = axis_input(device,2,radio_volume,0.0,1.0);
  if (ret == 1) {
    //    printf("radio volume: %f \n",*radio_volume);
  }
  ret = axis_input(device,3,master_volume,0.0,1.0);
  if (ret == 1) {
    //    printf("engine volume: %f \n",*master_volume);
  }

  /* steer servo for rudder position indicator */
  
  ret = servos_output(device,0,rudder_trim,-2.3,3.0);
  
}

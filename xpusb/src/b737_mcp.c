/* This is the b737_mcp.c code which contains a sample set-up for how to communicate with the 
   BOEING 737 mode control panel (switches, LED's, displays) connected to the OpenCockpits IOCARDS USB device.

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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "libiocards.h"
#include "serverdata.h"
#include "b737_mcp.h"

void b737_mcp(void)
{
  
  int ret = 0;
  int device = 5;
  int card = 0;

  float *ap_altitude;  // autopilot altitude
  if ((acf_type == 2) || (acf_type == 3)) {
    ap_altitude = link_dataref_flt("laminar/B738/autopilot/mcp_alt_dial",0); 
  } else if (acf_type == 1) {
    ap_altitude = link_dataref_flt("x737/systems/afds/ALTHLD_baroalt",0);
  } else {
    ap_altitude = link_dataref_flt("sim/cockpit/autopilot/altitude",0);
  }
  float *ap_heading; // autopilot heading
  if ((acf_type == 2) || (acf_type == 3)) {
    ap_heading = link_dataref_flt("laminar/B738/autopilot/mcp_hdg_dial",0);      
  } else if (acf_type == 1) {  
    ap_heading = link_dataref_flt("x737/systems/afds/HDG_magnhdg",0);      
  } else {
    ap_heading = link_dataref_flt("sim/cockpit/autopilot/heading_mag",0);
  }
  float *ap_ias; // IAS autopilot
  if ((acf_type == 2) || (acf_type == 3)) {
    ap_ias = link_dataref_flt("laminar/B738/autopilot/mcp_speed_dial_kts_mach",0);      
    //ap_ias = link_dataref_flt("sim/cockpit2/autopilot/airspeed_dial_kts_mach",0);      
  } else if (acf_type == 1) {  
    ap_ias = link_dataref_flt("x737/systems/athr/MCPSPD_spd",0);           
  } else {
    ap_ias = link_dataref_flt("sim/cockpit/autopilot/airspeed",0);
  }

  float *ap_vspeed; // autopilot vertical speed
  if ((acf_type == 2) || (acf_type == 3)) {
    ap_vspeed = link_dataref_flt("sim/cockpit2/autopilot/vvi_dial_fpm",0);             
  } else if (acf_type == 1) {  
    ap_vspeed = link_dataref_flt("x737/systems/afds/VS_vvi",0);             
  } else {
    ap_vspeed = link_dataref_flt("sim/cockpit/autopilot/vertical_velocity",0);
  }
  
  int *ap_banklimit;  // bank limit
  if ((acf_type == 2) || (acf_type == 3)) {
    ap_banklimit = link_dataref_int("laminar/B738/rotary/autopilot/bank_angle");      
  } else {
    ap_banklimit = link_dataref_int("sim/cockpit/autopilot/heading_roll_mode");     
  }
    
  int *ap_spd_ismach; 
  if (acf_type == 1) {
    ap_spd_ismach = link_dataref_int("x737/systems/athr/MCPSPD_ismach"); // MCP speed in mach 
  } else {
    ap_spd_ismach = link_dataref_int("sim/cockpit2/autopilot/airspeed_is_mach"); // MCP speed in mach 
  }

  int *ap_cmd_a;
  int *ap_cmd_a_led;
  int *ap_cmd_b;
  int *ap_cmd_b_led;
  int *ap_cws_a;
  int *ap_cws_a_led;
  int *ap_cws_b;
  int *ap_cws_b_led;
  int *ap_fdir_a;
  int *ap_fdir_a_led;
  int *ap_fdir_b;
  int *ap_fdir_b_led;
  int *ap_engage;
  int *ap_disengage_status;
  int *ap_vs_arm;
  int *ap_vs_engage;
  int *ap_alt_hold;
  int *ap_alt_hold_led;
  int *ap_lnav;
  int *ap_lnav_led;
  int *ap_vor_loc;
  int *ap_vor_loc_led;
  int *ap_app;
  int *ap_app_led;
  int *ap_hdg_sel;
  int *ap_hdg_sel_led;
  int *ap_lvl_chg;
  int *ap_lvl_chg_led;
  int *ap_vnav;
  int *ap_vnav_led;
  int *ap_n1;
  int *ap_n1_led;
  int *ap_at_arm;
  int *ap_at_arm_status;
  int *ap_mcpspd;
  int *ap_mcpspd_led;
  if (acf_type == 3) {
    ap_cmd_a = link_dataref_cmd_hold("laminar/B738/autopilot/cmd_a_press");     // MCP CMD A mode
    ap_cmd_a_led = link_dataref_int("laminar/B738/autopilot/cmd_a_status");
    ap_cmd_b = link_dataref_cmd_hold("laminar/B738/autopilot/cmd_b_press");     // MCP CMD B mode
    ap_cmd_b_led = link_dataref_int("laminar/B738/autopilot/cmd_b_status");
    ap_cws_a = link_dataref_cmd_hold("laminar/B738/autopilot/cws_a_press");     // MCP CWS A mode
    ap_cws_a_led = link_dataref_int("laminar/B738/autopilot/cws_a_status");     // MCP CWS A mode
    ap_cws_b = link_dataref_cmd_hold("laminar/B738/autopilot/cws_b_press");     // MCP CWS B mode
    ap_cws_b_led = link_dataref_int("laminar/B738/autopilot/cws_b_status");     // MCP CWS B mode
    ap_fdir_a = link_dataref_int("laminar/B738/switches/autopilot/fd_ca");    // Flight Director mode CA (A)
    ap_fdir_a_led = link_dataref_int("laminar/B738/autopilot/master_capt_status");
    ap_fdir_b = link_dataref_int("laminar/B738/switches/autopilot/fd_fo");    // Flight Director mode FO (B)
    ap_fdir_b_led = link_dataref_int("laminar/B738/autopilot/master_fo_status");
    ap_engage = link_dataref_cmd_once("laminar/B738/autopilot/disconnect_toggle"); // AP engage toggle
    ap_disengage_status = link_dataref_int("laminar/B738/autopilot/disconnect_pos");  // Needs the status too
    ap_vs_arm = link_dataref_cmd_hold("laminar/B738/autopilot/vs_press");   // VS hold MODE (orange)
    ap_vs_engage = link_dataref_int("laminar/B738/autopilot/vs_status");  // VS hold MODE (green)
    ap_alt_hold = link_dataref_cmd_hold("laminar/B738/autopilot/alt_hld_press");  // ALT HOLD MODE
    ap_alt_hold_led = link_dataref_int("laminar/B738/autopilot/alt_hld_status");
    ap_lnav = link_dataref_cmd_hold("laminar/B738/autopilot/lnav_press");      // AP lateral NAV button
    ap_lnav_led = link_dataref_int("laminar/B738/autopilot/lnav_status");
    ap_vor_loc = link_dataref_cmd_hold("laminar/B738/autopilot/vorloc_press");   // AP VOR LOC button
    ap_vor_loc_led = link_dataref_int("laminar/B738/autopilot/vorloc_status");
    ap_app = link_dataref_cmd_hold("laminar/B738/autopilot/app_press");       // AP APP button
    ap_app_led = link_dataref_int("laminar/B738/autopilot/app_status");
    ap_hdg_sel = link_dataref_cmd_hold("laminar/B738/autopilot/hdg_sel_press");   // AP HDG SEL button
    ap_hdg_sel_led = link_dataref_int("laminar/B738/autopilot/hdg_sel_status");
    ap_lvl_chg = link_dataref_cmd_hold("laminar/B738/autopilot/lvl_chg_press");   // AP LVL CHG button
    ap_lvl_chg_led = link_dataref_int("laminar/B738/autopilot/lvl_chg_status"); 
    ap_vnav = link_dataref_cmd_hold("laminar/B738/autopilot/vnav_press");      // AP vertical NAV button
    ap_vnav_led = link_dataref_int("laminar/B738/autopilot/vnav_status1");
    ap_n1 = link_dataref_cmd_hold("laminar/B738/autopilot/n1_press"); // AP speed N1 button
    ap_n1_led = link_dataref_int("laminar/B738/autopilot/n1_status"); // 0: off, 1: armed, 2: captured
    ap_at_arm = link_dataref_cmd_once("laminar/B738/autopilot/autothrottle_arm_toggle");    // AP Autothrottle arm
    ap_at_arm_status = link_dataref_int("laminar/B738/autopilot/autothrottle_status");
    ap_mcpspd = link_dataref_cmd_hold("laminar/B738/autopilot/speed_press");     // AP mcpspd select switch
    ap_mcpspd_led = link_dataref_int("laminar/B738/autopilot/speed_status1");
  } else if (acf_type == 1) {
    ap_cmd_a = link_dataref_cmd_once("x737/mcp/CMDA_TOGGLE");     // MCP CMD A mode
    ap_cmd_a_led = link_dataref_int("x737/systems/MCP/LED_CMDA_on");
    ap_cmd_b = link_dataref_cmd_once("x737/mcp/CMDB_TOGGLE");     // MCP CMD B mode
    ap_cmd_b_led = link_dataref_int("x737/systems/MCP/LED_CMDB_on");
    ap_cws_a = link_dataref_int("xpserver/ap_cws_a");     // MCP CWS A mode
    ap_cws_a_led = link_dataref_int("xpserver/ap_cws_a_led");     // MCP CWS A mode
    ap_cws_b = link_dataref_int("xpserver/ap_cws_b");     // MCP CWS B mode
    ap_cws_b_led = link_dataref_int("xpserver/ap_cws_b_led");     // MCP CWS B mode
    ap_fdir_a = link_dataref_int("x737/systems/afds/fdA_status");    // Flight Director mode CA (A)
    ap_fdir_a_led = link_dataref_int("x737/systems/MCP/LED_FDA_MA_on");
    ap_fdir_b = link_dataref_int("x737/systems/afds/fdB_status");    // Flight Director mode FO (B)
    ap_fdir_b_led = link_dataref_int("x737/systems/MCP/LED_FDB_MA_on");
    ap_engage = link_dataref_int("x737/systems/afds/APengaged");    // AP engage/disengage mode
    ap_vs_arm = link_dataref_int("x737/systems/afds/VS_arm");   // VS hold MODE (orange)
    ap_vs_engage = link_dataref_int("x737/systems/afds/VS");  // VS hold MODE (green)
    ap_alt_hold = link_dataref_cmd_once("x737/mcp/ALTHLD_TOGGLE");  // ALT HOLD MODE
    ap_alt_hold_led = link_dataref_int("x737/systems/MCP/LED_ALTHLD_on");
    ap_lnav = link_dataref_cmd_once("x737/mcp/LNAV_TOGGLE");      // AP lateral NAV button
    ap_lnav_led = link_dataref_int("x737/systems/MCP/LED_LNAV_on");
    ap_vor_loc = link_dataref_cmd_once("x737/mcp/VORLOC_TOGGLE");   // AP VOR LOC button
    ap_vor_loc_led = link_dataref_int("x737/systems/MCP/LED_VORLOC_on");
    ap_app = link_dataref_cmd_once("x737/mcp/APP_TOGGLE");       // AP APP button
    ap_app_led = link_dataref_int("x737/systems/MCP/LED_APP_on");
    ap_hdg_sel = link_dataref_cmd_once("x737/mcp/HDGSEL_TOGGLE");
    ap_hdg_sel_led = link_dataref_int("x737/systems/MCP/LED_HDG_on");
    ap_lvl_chg = link_dataref_cmd_once("x737/mcp/LVLCHANGE_TOGGLE");   // AP LVL CHG button
    ap_lvl_chg_led = link_dataref_int("x737/systems/MCP/LED_LVLCHG_on");
    ap_vnav = link_dataref_cmd_once("x737/mcp/VNAV_TOGGLE");      // AP vertical NAV button
    ap_vnav_led = link_dataref_int("x737/systems/MCP/LED_VNAV_on");
    ap_n1 = link_dataref_cmd_once("x737/mcp/N1_MODE_TOGGLE");        // AP speed N1 button
    ap_n1_led = link_dataref_int("x737/systems/MCP/LED_N1_on");
    ap_at_arm = link_dataref_int("x737/systems/athr/athr_armed");    // AP Autothrottle arm
    ap_mcpspd = link_dataref_cmd_once("x737/mcp/MCPSPD_MODE_TOGGLE");     // AP mcpspd select switch
    ap_mcpspd_led = link_dataref_int("x737/systems/MCP/LED_MCPSPD_on");
  } else {
    ap_cmd_a = link_dataref_int("xpserver/CMD_A");     // MCP CMD A mode
    ap_cmd_a_led = link_dataref_int("xpserver/LED_CMDA_on");
    ap_cmd_b = link_dataref_int("xpserver/CMD_B");     // MCP CMD B mode
    ap_cmd_b_led = link_dataref_int("xpserver/LED_CMDB_on");
    ap_cws_a = link_dataref_cmd_hold("sim/autopilot/CWSA");     // MCP CWS A mode
    ap_cws_a_led = link_dataref_int("xpserver/ap_cws_a_led");     // MCP CWS A mode
    ap_cws_b = link_dataref_cmd_hold("sim/autopilot/CWSB");     // MCP CWS B mode
    ap_cws_b_led = link_dataref_int("xpserver/ap_cws_b_led");     // MCP CWS B mode
    ap_fdir_a = link_dataref_int("sim/cockpit2/autopilot/flight_director_mode");    // Flight Director mode CA (A)
    ap_fdir_a_led = link_dataref_int("sim/cockpit2/autopilot/flight_director_mode");
    ap_fdir_b = link_dataref_int("xpserver/fdB_status");    // Flight Director mode FO (B)
    ap_fdir_b_led = link_dataref_int("xpserver/LED_FDB_MA_on");
    ap_engage = link_dataref_int("sim/cockpit/autopilot/autopilot_mode");    // AP engage/disengage mode
    ap_vs_arm = link_dataref_cmd_once("sim/autopilot/vertical_speed");   // VS hold MODE (orange)
    ap_vs_engage = link_dataref_int("sim/cockpit2/autopilot/vvi_status");  // VS hold MODE (green)
    ap_alt_hold = link_dataref_cmd_once("sim/autopilot/altitude_hold");  // ALT HOLD MODE
    ap_alt_hold_led = link_dataref_int("sim/cockpit2/autopilot/altitude_hold_armed");
    ap_lnav = link_dataref_int("xpserver/LNAV_arm");      // AP lateral NAV button
    ap_lnav_led = link_dataref_int("xpserver/LED_LNAV_on");
    ap_vor_loc = link_dataref_cmd_once("sim/autopilot/NAV");   // AP VOR LOC button
    ap_vor_loc_led = link_dataref_int("sim/cockpit2/autopilot/hnav_armed");
    ap_app = link_dataref_cmd_once("sim/autopilot/approach");       // AP APP button
    ap_app_led = link_dataref_int("sim/cockpit2/autopilot/approach_status");
    ap_hdg_sel = link_dataref_cmd_once("sim/autopilot/heading");   // AP HDG SEL button
    ap_hdg_sel_led = link_dataref_int("sim/cockpit2/autopilot/heading_mode");
    ap_lvl_chg = link_dataref_cmd_once("sim/autopilot/level_change");   // AP LVL CHG button
    ap_lvl_chg_led = link_dataref_int("xpserver/LED_LVLCHG_on"); 
    ap_vnav = link_dataref_cmd_once("sim/autopilot/FMS");      // AP vertical NAV button
    ap_vnav_led = link_dataref_int("sim/cockpit2/autopilot/vnav_armed");
    ap_n1 = link_dataref_int("xpserver/N1_mode");        // AP speed N1 button
    ap_n1_led = link_dataref_int("xpserver/LED_N1_on"); // 0: off, 1: armed, 2: captured
    ap_at_arm = link_dataref_int("sim/cockpit2/autopilot/autothrottle_enabled");    // AP Autothrottle arm
    ap_mcpspd = link_dataref_int("xpserver/MCPSPD_mode");     // AP mcpspd select switch
    ap_mcpspd_led = link_dataref_int("xpserver/LED_MCPSPD_on");
  }
  float *ap_course1; // NAV autopilot course1
  if ((acf_type == 2) || (acf_type == 3)) {
    ap_course1 = link_dataref_flt("laminar/B738/autopilot/course_pilot",0);
    //ap_course1 = link_dataref_flt("sim/cockpit2/radios/actuators/nav1_obs_deg_mag_pilot",0);
  } else {
    ap_course1 = link_dataref_flt("sim/cockpit/radios/nav1_obs_degm",0);
  }
  float *ap_course2; // NAV autopilot course2    
  if ((acf_type == 2) || (acf_type == 3)) {
    ap_course2 = link_dataref_flt("laminar/B738/autopilot/course_copilot",0);
    //ap_course2 = link_dataref_flt("sim/cockpit2/radios/actuators/nav1_obs_deg_mag_copilot",0);
  } else {
    ap_course2 = link_dataref_flt("sim/cockpit/radios/nav2_obs_degm",0);   
  }
  
  int *master_caution_cp;
  int *master_caution_fo;
  int *fire_warn_cp;
  int *fire_warn_fo;
  float *master_caution_cp_f;
  float *master_caution_fo_f;
  float *fire_warn_cp_f;
  float *fire_warn_fo_f;
  int *master_caution_cp_light;
  int *master_caution_fo_light;
  float *master_caution_cp_light_f;
  float *master_caution_fo_light_f;
  int *fire_warn_cp_light;
  int *fire_warn_fo_light;
  float *fire_warn_cp_light_f;
  float *fire_warn_fo_light_f;
  
  int *sixpack_flt_cont;
  int *sixpack_irs;
  int *sixpack_fuel;
  int *sixpack_elec;
  int *sixpack_apu;
  int *sixpack_overheat;
  float *sixpack_flt_cont_f;
  float *sixpack_irs_f;
  float *sixpack_fuel_f;
  float *sixpack_elec_f;
  float *sixpack_apu_f;
  float *sixpack_overheat_f;
  
  int *sixpack_anti_ice;
  int *sixpack_hyd;
  int *sixpack_doors;
  int *sixpack_eng;
  int *sixpack_overhead;
  int *sixpack_air_cond;
  float *sixpack_anti_ice_f;
  float *sixpack_hyd_f;
  float *sixpack_doors_f;
  float *sixpack_eng_f;
  float *sixpack_overhead_f;
  float *sixpack_air_cond_f;
  if ((acf_type == 2) || (acf_type == 3)) {
    master_caution_cp = link_dataref_cmd_hold("laminar/B738/push_button/master_caution1");
    master_caution_fo = link_dataref_cmd_hold("laminar/B738/push_button/master_caution2");
    fire_warn_cp = link_dataref_cmd_hold("laminar/B738/push_button/fire_bell_light1");
    fire_warn_fo = link_dataref_cmd_hold("laminar/B738/push_button/fire_bell_light2");
    master_caution_cp_light_f = link_dataref_flt("laminar/B738/annunciator/master_caution_light",0);
    master_caution_fo_light_f = link_dataref_flt("laminar/B738/annunciator/master_caution_light",0);
    fire_warn_cp_light_f = link_dataref_flt("laminar/B738/annunciator/fire_bell_annun",0);
    fire_warn_fo_light_f = link_dataref_flt("laminar/B738/annunciator/fire_bell_annun",0);
    
    sixpack_flt_cont_f = link_dataref_flt("laminar/B738/annunciator/six_pack_flt_cont",0);
    sixpack_irs_f = link_dataref_flt("laminar/B738/annunciator/six_pack_irs",0);
    sixpack_fuel_f = link_dataref_flt("laminar/B738/annunciator/six_pack_fuel",0);
    sixpack_elec_f = link_dataref_flt("laminar/B738/annunciator/six_pack_elec",0);
    sixpack_apu_f = link_dataref_flt("laminar/B738/annunciator/six_pack_apu",0);
    sixpack_overheat_f = link_dataref_flt("laminar/B738/annunciator/six_pack_fire",0);
    
    sixpack_anti_ice_f = link_dataref_flt("laminar/B738/annunciator/six_pack_ice",0);
    sixpack_hyd_f = link_dataref_flt("laminar/B738/annunciator/six_pack_hyd",0);
    sixpack_doors_f = link_dataref_flt("laminar/B738/annunciator/six_pack_doors",0);
    sixpack_eng_f = link_dataref_flt("laminar/B738/annunciator/six_pack_eng",0);
    sixpack_overhead_f = link_dataref_flt("laminar/B738/annunciator/six_pack_overhead",0);
    sixpack_air_cond_f = link_dataref_flt("laminar/B738/annunciator/six_pack_air_cond",0);
    
  } else if (acf_type == 1) {
    master_caution_cp_f = link_dataref_flt("x737/cockpit/warningSys/MASTER_CAUTION_capt",0);
    master_caution_fo_f = link_dataref_flt("x737/cockpit/warningSys/MASTER_CAUTION_fo",0);
    fire_warn_cp_f = link_dataref_flt("x737/cockpit/warningSys/FIRE_WARN_capt",0);
    fire_warn_fo_f = link_dataref_flt("x737/cockpit/warningSys/FIRE_WARN_fo",0);
    master_caution_cp_light = link_dataref_int("x737/cockpit/warningSys/MASTER_CAUTION_capt_on");
    master_caution_fo_light = link_dataref_int("x737/cockpit/warningSys/MASTER_CAUTION_fo_on");
    fire_warn_cp_light = link_dataref_int("x737/cockpit/warningSys/FIRE_WARN_capt_on");
    fire_warn_fo_light = link_dataref_int("x737/cockpit/warningSys/FIRE_WARN_fo_on");
    
    sixpack_flt_cont = link_dataref_int("x737/cockpit/warningSys/sysAnnFLT_CONT");
    sixpack_irs = link_dataref_int("x737/cockpit/warningSys/sysAnnIRS");
    sixpack_fuel = link_dataref_int("x737/cockpit/warningSys/sysAnnFUEL");
    sixpack_elec = link_dataref_int("x737/cockpit/warningSys/sysAnnELEC");
    sixpack_apu = link_dataref_int("x737/cockpit/warningSys/sysAnnAPU");
    sixpack_overheat = link_dataref_int("x737/cockpit/warningSys/sysAnnOVHT_DET");
    
    sixpack_anti_ice = link_dataref_int("x737/cockpit/warningSys/sysAnnANTI_ICE");
    sixpack_hyd = link_dataref_int("x737/cockpit/warningSys/sysAnnHYD");
    sixpack_doors = link_dataref_int("x737/cockpit/warningSys/sysAnnDOORS");
    sixpack_eng = link_dataref_int("x737/cockpit/warningSys/sysAnnENG");
    sixpack_overhead = link_dataref_int("x737/cockpit/warningSys/sysAnnOVERHEAD");
    sixpack_air_cond = link_dataref_int("x737/cockpit/warningSys/sysAnnAIR_COND");
  }
  
  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");

  // gear handle (hooked up on MCP inputs temporarily)
  int *gear_handle_down;
  float *gear_handle_position;
  if ((acf_type == 2) || (acf_type == 3)) {
    gear_handle_down = link_dataref_int("xpserver/gear_handle");
    gear_handle_position = link_dataref_flt("laminar/B738/controls/gear_handle_down",-1);
  } else if (acf_type == 1) {
    gear_handle_down = link_dataref_int("xpserver/gear_handle");
    gear_handle_position = link_dataref_flt("x737/systems/landinggear/landingGearLever",-1);
  } else {
    gear_handle_down = link_dataref_int("sim/cockpit2/controls/gear_handle_down");
    gear_handle_position = link_dataref_flt("xpserver/gear_handle_position",-1);
  }

  /* local variables */
  int bank10, bank15, bank20, bank25, bank30;
  int temp = 0;
  int one = 1;
  int zero = 0;
  int ten = 10; /* blank display digits together with special value setting */

  // mastercard 1, input board # 1 (0-35)
  // mastercard 1, input board # 2 (36-71)
  
  /* read inputs */

  if ((acf_type == 1 ) || (acf_type == 3)) {
    ret = digital_input(device,card,39,&temp,0);
    if ((ret==1) && (temp==1)) printf("MASTER CAUTION (FO) \n");
    if (acf_type == 1) {
      *master_caution_fo_f = (float) temp;
    } else {
      *master_caution_fo = temp;
    }
    ret = digital_input(device,card,40,&temp,0);
    if ((ret==1) && (temp==0)) printf("FIRE WARN CP \n");
    if (acf_type == 1) {
      *fire_warn_cp_f = (float) 1-temp;
    } else {
      *fire_warn_cp = 1-temp;
    }
    ret = digital_input(device,card,41,&temp,0);
    if ((ret == 1) && (temp == 0)) printf("MASTER CAUTION CP \n");
    if (acf_type == 1) {
      *master_caution_cp_f = (float) 1-temp;
    } else {
      *master_caution_cp = 1-temp;
    }

    ret = digital_input(device,card,42,&temp,0);
    if ((ret==1) && (temp==1)) printf("FIRE WARN FO \n");
    if (acf_type == 1) {
      *fire_warn_fo_f = (float) temp;
    } else {
      *fire_warn_fo = temp;
    }
  }
  
  // gear lever (temporarily connected to MCP)
  ret = digital_input(device,card,44,&temp,0);
  if (temp == 1) {
    if (ret==1) printf("gear handle up \n");
    *gear_handle_down = 0;
    *gear_handle_position = 0.0;
  } else {
    ret = digital_input(device,card,43,&temp,0);
    if (temp == 1) {
      if (ret==1) printf("gear handle down \n");
      *gear_handle_down = 1;
      *gear_handle_position = 1.0;
    } else {
      *gear_handle_position = 0.5;
    }
  }

  ret = digital_input(device,card,3,&bank10,0);
  if (ret == 1) {
    printf("Bank Limit 10\n");
  }
  ret = digital_input(device,card,4,&bank15,0);
  if (ret == 1) {
    printf("Bank Limit 15\n");
  }
  ret = digital_input(device,card,5,&bank20,0);
  if (ret == 1) {
    printf("Bank Limit 20\n");
  }
  ret = digital_input(device,card,6,&bank25,0);
  if (ret == 1) {
    printf("Bank Limit 25\n");
  }
  ret = digital_input(device,card,7,&bank30,0);
  if (ret == 1) {
    printf("Bank Limit 30\n");
  }

 
  if ((acf_type == 1) || (acf_type == 3)) {
    ret = digital_input(device,card,20,ap_cmd_a,0);
    if (ret == 1) {
      printf("CMD A pressed\n");
    }
  } else {
    ret = digital_input(device,card,20,&temp,0);
    if ((ret == 1) && (temp == 1)){
      printf("CMD A pressed\n");
    }
  }

  ret = digital_input(device,card,21,ap_cws_a,0);
  if (ret == 1) {
    printf("CWS A pressed\n");
  }
  ret = digital_input(device,card,22,ap_cmd_b,0);
  if (ret == 1) {
    printf("CMD B pressed\n");
  }
  ret = digital_input(device,card,23,ap_cws_b,0);
  if (ret == 1) {
    printf("CWS B pressed\n");
  }
  
  /* only change ap_engage if switch has changed */
  /* safe algorithm for solenoid relais in pedestal code */
   if ((acf_type == 2) || (acf_type == 3)) {
    /* with ZIBO we can only toggle the switch, so check status as well */
     *ap_engage = 0;
    ret = digital_input(device,card,25,&temp,0);
    if (ret == 1) {
     if ((*ap_disengage_status == 1) && (temp == 1)) {
	*ap_engage = 1;
      }
      if ((*ap_disengage_status == 0) && (temp == 0)) {
	*ap_engage = 1;
      }
     }
  } else if (acf_type == 1) {
    ret = digital_input(device,card,25,&temp,0);
    if (ret == 1) {
      *ap_engage = temp;
    }
  } else {
    ret = digital_input(device,card,25,&temp,0);
    if (ret == 1) {
      if (temp == 1) {
	if (*ap_engage == 0) {
	  *ap_engage = 2;
	}
      } else {
	if (*ap_engage >= 1) {
	  *ap_engage = 0;
	}
      }   
    } 
  }
   
  ret = digital_input(device,card,26,ap_vnav,0);
  ret = digital_input(device,card,27,ap_lnav,0);

  ret = digital_input(device,card,28,ap_vor_loc,0);

  if (acf_type == 1) {
    ret = digital_input(device,card,29,ap_vs_arm,1);
    if (ret == 1) {
      if (*ap_vs_arm == 1) {
	if (*ap_vs_engage == 1) {
	  *ap_vs_arm = 0;
	  *ap_vs_engage = 0;
	} else {
	}
      } else {
	if (*ap_vs_engage == 1) {
	  *ap_vs_engage = 0;
	} else {
	  *ap_vs_arm = 1;
	  *ap_vs_engage = 1;
	}
      }
    }
  } else {
    ret = digital_input(device,card,29,ap_vs_arm,0);
  }

  
  ret = digital_input(device,card,30,ap_alt_hold,0);
  
  ret = digital_input(device,card,31,ap_app,0);
  if (ret == 1) printf("APP Button: %i \n",*ap_app);
  ret = digital_input(device,card,32,ap_hdg_sel,0);
  if (ret == 1) printf("HDG Button: %i \n",*ap_hdg_sel);
  ret = digital_input(device,card,33,ap_lvl_chg,0);
  if (ret == 1) printf("LVLCHG Button: %i \n",*ap_lvl_chg);
  ret = digital_input(device,card,34,ap_mcpspd,0);
  if (ret == 1) printf("SPD Button: %i \n",*ap_mcpspd);
  ret = digital_input(device,card,35,ap_n1,0);
  if (ret == 1) printf("N1 Button: %i \n",*ap_n1);

  /* only change at_arm if switch has changed */
  /* safe algorithm for solenoid relais in pedestal code */
  ret = digital_input(device,card,36,&temp,0);
  if (ret==1) {
    if ((acf_type == 2) || (acf_type == 3)) {
      *ap_at_arm = 0;
      if ((*ap_at_arm_status == 0) && (temp == 1)) {
	*ap_at_arm = 1;
	printf("ARMING AT\n");
      }
      if ((*ap_at_arm_status == 1) && (temp == 0)) {
	*ap_at_arm = 1;
	printf("DISARMING AT\n");
      }
    } else {
      *ap_at_arm = temp;
    }
  }
  
  ret = digital_input(device,card,37,&temp,0);
  if ((acf_type == 2) || (acf_type == 3)) {
    *ap_fdir_a = temp;
  } else if (acf_type == 1) {
    if (ret==1) {
      *ap_fdir_a = temp;
    }
  } else if (acf_type == 0) {
    if (temp==1) {
      *ap_fdir_a = 2;
    } else {
      *ap_fdir_a = 0;
    }
  } // don't do anything if acf_type is -1 (undefined)
   
  ret = digital_input(device,card,24,ap_fdir_b,0);
  /*  ret = digital_input(device,card,24,&temp,0);
  if (ret==1) {
    *ap_fdir_b = temp;
    } */

  ret = digital_input(device,card,38,ap_spd_ismach,1);
 
  /* Bank limit - 0 = auto, 1-6 = 5-30 degrees of bank */
  if ((acf_type == 2) || (acf_type == 3)) {
      *ap_banklimit = bank15 + bank20*2 + bank25*3 + bank30*4;
      // printf("banklimit: %i \n",*ap_banklimit);
  } else {
    if ((bank10+bank15+bank20+bank25+bank30) != 0) {
      *ap_banklimit = bank10*2 + bank15*3 + bank20*4 + bank25*5 + bank30*6;
      //   printf("banklimit: %i \n",*ap_banklimit);
      // *ap_banklimit = 6;
    } else {
      *ap_banklimit = 1;
      //*ap_banklimit = 6;
      //    printf("banklimit: %i \n",*ap_banklimit);
    }
  }

  /* check encoders */
  ret = mastercard_encoder(device,card,0,ap_heading,1.0,2,0);
  if ((ret==1) && (*ap_heading != FLT_MISS)) {
    if (*ap_heading < 0.0) {
      *ap_heading = 360. + *ap_heading;
    }
    if (*ap_heading >= 360.0) {
      *ap_heading = *ap_heading - 360.0;
    }
    printf("heading: %f \n",*ap_heading);
  }
  ret = mastercard_encoder(device,card,9,ap_course1,1.0,2,2);
  if ((ret==1) && (*ap_course1 != FLT_MISS)) {
    if (*ap_course1 < 0.0) *ap_course1 = 360.0;
    if (*ap_course1 > 360.0) *ap_course1 = 0.0;
    printf("course1: %f \n",*ap_course1);
  }
    
  if (*ap_spd_ismach == 1) {
    ret = mastercard_encoder(device,card,11,ap_ias,0.01,2,3);
  } else {
    ret = mastercard_encoder(device,card,11,ap_ias,1.0,2,3);
  }
  if (ret==1) printf("ias: %i %f \n",*ap_spd_ismach,*ap_ias);

  ret = mastercard_encoder(device,card,13,ap_altitude,100.0,2,3);
  if (ret==1) printf("altitude: %f \n",*ap_altitude);

  /* v/s is 50 fpm steps below abs(1000fpm) and 100 fpm above abs(1000fpm) */
  if ((*ap_vspeed < 1000.) && (*ap_vspeed > -1000.)) {
    ret = mastercard_encoder(device,card,15,ap_vspeed,-50.0,2,3);
  } else {
    ret = mastercard_encoder(device,card,15,ap_vspeed,-100.0,2,3);
  }
  if (ret==1) printf("vspeed: %f \n",*ap_vspeed);

  ret = mastercard_encoder(device,card,18,ap_course2,1.0,2,2);
  if ((ret==1) && (*ap_course2 != FLT_MISS)) {
    if (*ap_course2 < 0.0) *ap_course2 = 360.0;
    if (*ap_course2 > 360.0) *ap_course2 = 0.0;
    printf("course2: %f \n",*ap_course2);
  }
  
  // mastercard 1, output board (11-55)

  /* update outputs */
  if ((acf_type == 1) || (acf_type == 2) || (acf_type == 3)) {
    ret = digital_output(device,card,11,ap_cmd_a_led);
  } else {
    temp = *ap_cmd_a_led == 2; // CMD A is only lighted with AP engaged (=2)
    ret = digital_output(device,card,11,&temp);
  }
  ret = digital_output(device,card,12,ap_cws_a_led);
  ret = digital_output(device,card,13,ap_cmd_b_led);
  ret = digital_output(device,card,14,ap_cws_b_led);
  ret = digital_output(device,card,15,ap_vs_engage);
  ret = digital_output(device,card,16,ap_vs_arm);
  ret = digital_output(device,card,17,ap_alt_hold_led);

  ret = digital_output(device,card,18,ap_lnav_led);
  ret = digital_output(device,card,19,ap_vor_loc_led);
  ret = digital_output(device,card,20,ap_app_led);
  ret = digital_output(device,card,21,ap_hdg_sel_led);
  ret = digital_output(device,card,22,ap_lvl_chg_led);
  ret = digital_output(device,card,23,ap_vnav_led);  
  ret = digital_output(device,card,24,ap_mcpspd_led);
  
  ret = digital_output(device,card,25,ap_n1_led);
  if ((acf_type == 2) || (acf_type == 3)) {
    ret = digital_output(device,card,26,ap_at_arm_status);
  } else {
    ret = digital_output(device,card,26,ap_at_arm);
  }
  ret = digital_output(device,card,27,ap_fdir_a_led);
  ret = digital_output(device,card,28,ap_fdir_b_led);

  /* MCP backlighting: relais #1 */
  ret = digital_output(device,card,49,avionics_on);

  /* MASTER CAUTION Annunciator: relais #2 */
  if ((acf_type == 2) || (acf_type == 3)) {
    if ((*master_caution_cp_light_f == 1) || (*master_caution_fo_light_f == 1)) {
      ret = digital_output(device,card,50,&one);
    } else {
      ret = digital_output(device,card,50,&zero);
    }
    
    /* FIRE WARN Annunciator: relais #3 */
    if ((*fire_warn_cp_light_f == 1) || (*fire_warn_fo_light_f == 1)) {
      ret = digital_output(device,card,51,&one);
    } else {
      ret = digital_output(device,card,51,&zero);
    }
  }
  if (acf_type == 1) {
    if ((*master_caution_cp_light == 1) || (*master_caution_fo_light == 1)) {
      ret = digital_output(device,card,50,&one);
    } else {
      ret = digital_output(device,card,50,&zero);
    }
    
    /* FIRE WARN Annunciator: relais #3 */
    if ((*fire_warn_cp_light == 1) || (*fire_warn_fo_light == 1)) {
      ret = digital_output(device,card,51,&one);
    } else {
      ret = digital_output(device,card,51,&zero);
    }    
  }
    
  /* relais 4 UNUSED */
  
  /* MCP autothrottle solenoid: relais #5 */
  if ((acf_type == 2) || (acf_type == 3)) {
    ret = digital_output(device,card,53,ap_at_arm_status);
  } else {
    ret = digital_output(device,card,53,ap_at_arm);
  }
  
  /* MCP autopilot engaged solenoid: relais #6 */
  if ((acf_type == 2) || (acf_type == 3)) {
    temp = 1 - *ap_disengage_status;
    ret = digital_output(device,card,54,&temp);
  } else {
    ret = digital_output(device,card,54,ap_engage);
  }
    
  /* EFIS backlighting: relais #7 */
  ret = digital_output(device,card,55,avionics_on);
  
  /* SIX PACK CP/FO side */
  if ((acf_type == 2) || (acf_type == 3)) {
    ret = digital_outputf(device,card,37,sixpack_eng_f); // FO ENG
    ret = digital_outputf(device,card,38,sixpack_overhead_f); // FO OVERHEAD
    ret = digital_outputf(device,card,39,sixpack_air_cond_f); // FO AIR COND
    ret = digital_outputf(device,card,40,sixpack_anti_ice_f); // FO ANTI-ICE
    ret = digital_outputf(device,card,41,sixpack_hyd_f); // FO HYD
    ret = digital_outputf(device,card,42,sixpack_doors_f); // FO DOORS
    ret = digital_outputf(device,card,43,sixpack_flt_cont_f); // CP FLT CONT
    ret = digital_outputf(device,card,44,sixpack_irs_f); // CP IRS
    ret = digital_outputf(device,card,45,sixpack_fuel_f); // CP FUEL
    ret = digital_outputf(device,card,46,sixpack_elec_f); // CP ELEC
    ret = digital_outputf(device,card,47,sixpack_apu_f); // CP APU
    ret = digital_outputf(device,card,48,sixpack_overheat_f); // CP OVERHEAT
  } else if (acf_type == 1) {
    ret = digital_output(device,card,37,sixpack_eng); // FO ENG
    ret = digital_output(device,card,38,sixpack_overhead); // FO OVERHEAD
    ret = digital_output(device,card,39,sixpack_air_cond); // FO AIR COND
    ret = digital_output(device,card,40,sixpack_anti_ice); // FO ANTI-ICE
    ret = digital_output(device,card,41,sixpack_hyd); // FO HYD
    ret = digital_output(device,card,42,sixpack_doors); // FO DOORS
    ret = digital_output(device,card,43,sixpack_flt_cont); // CP FLT CONT
    ret = digital_output(device,card,44,sixpack_irs); // CP IRS
    ret = digital_output(device,card,45,sixpack_fuel); // CP FUEL
    ret = digital_output(device,card,46,sixpack_elec); // CP ELEC
    ret = digital_output(device,card,47,sixpack_apu); // CP APU
    ret = digital_output(device,card,48,sixpack_overheat); // CP OVERHEAT
  }
    
  /* update displays */
  temp = (int) *ap_course1;
  ret = mastercard_display(device,card,0,3, &temp, 0);
  if (*ap_spd_ismach == 1) {
    temp = (int) (*ap_ias * 100.0);
    ret = mastercard_display(device,card,6,1, &ten, 1);  // clear first digit
    ret = mastercard_display(device,card,3,3, &temp, 0); // mach spd in 3 digits
  } else {
    temp = (int) *ap_ias;
    ret = mastercard_display(device,card,3,4, &temp, 0);
  }
  temp = (int) *ap_heading;
  ret = mastercard_display(device,card,7,3, &temp, 0);
  temp = (int) *ap_altitude;
  ret = mastercard_display(device,card,10,5, &temp, 0);

  /* vspeed display is blank if we don't have v/s engaged */
  //  if ((*ap_vs_arm == 0) && (*ap_vs_engage == 0)) {
  /* Changed: only blank if no vspeed is selected */
  if (*ap_vspeed == 0.0) {
    ret = mastercard_display(device,card,16,5, &ten, 1);
  } else {
    temp = (int) *ap_vspeed;
    ret = mastercard_display(device,card,16,5, &temp, 0);
  }

  temp = (int) *ap_course2;
  ret = mastercard_display(device,card,21,3, &temp, 0);

  /* control cold and dark for displays */
  if (*avionics_on != 1) {
    ret = mastercard_display(device,card,0,24, &ten, 1);
  }
  
}

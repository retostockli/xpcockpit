/* This is the b737_mcp.c code which connects to the SISMO MCP V4

   Copyright (C) 2021 Reto Stockli

   Additions for analog axes treatment by Hans Jansen 2011
   Also several cosmetic changes and changes for Linux compilation
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

void b737_mcp(void)
{

  int ret;
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
    ap_ias = link_dataref_flt("laminar/B738/autopilot/mcp_speed_dial_kts_mach",-2);      
  } else if (acf_type == 1) {  
    ap_ias = link_dataref_flt("x737/systems/athr/MCPSPD_spd",-2);           
  } else {
    ap_ias = link_dataref_flt("sim/cockpit/autopilot/airspeed",-2);
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
    
  int *ap_spd_is_mach; 
  if (acf_type == 1) {
    ap_spd_is_mach = link_dataref_int("x737/systems/athr/MCPSPD_ismach"); // MCP speed in mach 
  } else {
    ap_spd_is_mach = link_dataref_int("sim/cockpit2/autopilot/airspeed_is_mach"); // MCP speed in mach 
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

  /* INPUTS */

  /* ENCODERS */
  ret = encoder_inputf(card, 1, 0, ap_course1, 1.0, 1);
  if ((ret==1) && (*ap_course1 != FLT_MISS)) {
    if (*ap_course1 < 0.0) *ap_course1 = 359.0;
    if (*ap_course1 > 359.0) *ap_course1 = 0.0;
    printf("course1: %f \n",*ap_course1);
  }

  /* OUTPUTS */

  /* DISPLAYS */
  ret = display_outputf(card, 0, 3, ap_course1, -1, 0);
  


  if (0) {
  
  /* link integer data like a switch in the cockpit */
  int *value = link_dataref_int("sim/cockpit/electrical/landing_lights_on");

  /* link floating point dataref with precision 10e-1 to local variable. This means
     that we only transfer the variable if changed by 0.1 or more */
  float *fvalue = link_dataref_flt("sim/flightmodel/controls/parkbrake",-3);

  /* link NAV1 Frequency to encoder value */
  int *encodervalue = link_dataref_int("sim/cockpit/radios/nav1_freq_hz");

  /* read second digital input (#1) */
  ret = digital_input(card, 1, value, 0);
  if (ret == 1) {
    /* ret is 1 only if input has changed */
    printf("Digital Input changed to: %i \n",*value);
  }

  /* read first analog input (#0) */
  ret = analog_input(card,0,fvalue,0.0,1.0);
  if (ret == 1) {
    /* ret is 1 only if analog input has changed */
    //    printf("Analog Input changed to: %f \n",*fvalue);
  }

  /* read encoder at inputs 13 and 15 */
  ret = encoder_input(card, 13, 15, encodervalue, 5, 1);
  if (ret == 1) {
    /* ret is 1 only if encoder has been turned */
    printf("Encoder changed to: %i \n",*encodervalue);
  }
  
  /* set LED connected to second output (#1) to value of above input */
  *value = 1;
  for (int i=0;i<64;i++) {
    ret = digital_output(card, i, value);
  }
  
  /* set 7 segment displays 0-5 to the 5 digit value of the encoder with a decimal point at digit 2 */
  ret = display_output(card, 0, 5, encodervalue, 2, 0);

  }
  
}

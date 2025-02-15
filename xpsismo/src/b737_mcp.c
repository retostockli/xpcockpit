/* This is the b737_mcp.c code which connects to the SISMO MCP V4

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
#include "b737_mcp.h"

int bank10;
int bank15;
int bank20;
int bank25;
int bank30;
float ap_at_arm_status_save;

void b737_mcp(void)
{
  int card = 0; /* SISMO card according to ini file */

  /* In the SISMO MCP all switches are inverted (0=on; 1=off) */
  
  int ret;
  int temp;
  int one = 1;
  int zero = 0;

  int ap_hdg_1;
  int ap_hdg_2;

  float *ap_altitude;  // autopilot altitude
  float *ap_heading; // autopilot heading
  float *ap_ias; // IAS autopilot
  float *ap_vspeed; // autopilot vertical speed
  float *ap_banklimit_pos;  // bank limit
  int *ap_banklimit;
  int *ap_banklimit_up; 
  int *ap_banklimit_dn; 
  int *ap_spd_is_mach;
  float *ap_vspeed_show;
  float *ap_ias_show;

  
  if ((acf_type == 2) || (acf_type == 3)) {
    ap_altitude = link_dataref_flt("laminar/B738/autopilot/mcp_alt_dial",0); 
  } else if (acf_type == 1) {
    ap_altitude = link_dataref_flt("x737/systems/afds/ALTHLD_baroalt",0);
  } else {
    ap_altitude = link_dataref_flt("sim/cockpit/autopilot/altitude",0);
  }

  
  if ((acf_type == 2) || (acf_type == 3)) {
    ap_heading = link_dataref_flt("laminar/B738/autopilot/mcp_hdg_dial",0);      
  } else if (acf_type == 1) {  
    ap_heading = link_dataref_flt("x737/systems/afds/HDG_magnhdg",0);      
  } else {
    ap_heading = link_dataref_flt("sim/cockpit/autopilot/heading_mag",0);
  }
  if ((acf_type == 2) || (acf_type == 3)) {
    ap_ias = link_dataref_flt("laminar/B738/autopilot/mcp_speed_dial_kts_mach",-2);      
    ap_ias_show = link_dataref_flt("laminar/B738/autopilot/show_ias",0);       
  } else if (acf_type == 1) {  
    ap_ias = link_dataref_flt("x737/systems/athr/MCPSPD_spd",-2);           
  } else {
    ap_ias = link_dataref_flt("sim/cockpit/autopilot/airspeed",-2);
  }
  
  if ((acf_type == 2) || (acf_type == 3)) {
    ap_vspeed = link_dataref_flt("sim/cockpit2/autopilot/vvi_dial_fpm",0);
    ap_vspeed_show = link_dataref_flt("laminar/B738/autopilot/vvi_dial_show",0);       
  } else if (acf_type == 1) {  
    ap_vspeed = link_dataref_flt("x737/systems/afds/VS_vvi",0);             
  } else {
    ap_vspeed = link_dataref_flt("sim/cockpit/autopilot/vertical_velocity",0);
  }
  
  if ((acf_type == 2) || (acf_type == 3)) {
    ap_banklimit_pos = link_dataref_flt("laminar/B738/autopilot/bank_angle_pos",0);      
    ap_banklimit_up = link_dataref_cmd_once("laminar/B738/autopilot/bank_angle_up");      
    ap_banklimit_dn = link_dataref_cmd_once("laminar/B738/autopilot/bank_angle_dn");      
  } else {
    ap_banklimit = link_dataref_int("sim/cockpit/autopilot/heading_roll_mode");     
  }
    
  if (acf_type == 1) {
    ap_spd_is_mach = link_dataref_int("x737/systems/athr/MCPSPD_ismach"); // MCP speed in mach 
  } else {
    ap_spd_is_mach = link_dataref_int("sim/cockpit2/autopilot/airspeed_is_mach"); // MCP speed in mach 
  }

  int *ap_cmd_a;
  float *ap_cmd_a_led;
  int *ap_cmd_b;
  float *ap_cmd_b_led;
  int *ap_cws_a;
  float *ap_cws_a_led;
  int *ap_cws_b;
  float *ap_cws_b_led;
  int *ap_fdir_a;
  int *ap_fdir_a_status;
  float *ap_fdir_a_led;
  int *ap_fdir_b;
  int *ap_fdir_b_status;
  float *ap_fdir_b_led;
  int *ap_engage;
  float *ap_disengage_status;
  int *ap_vs_arm;
  int *ap_vs_engage;
  float *ap_vs_engage_f;
  int *ap_alt_hld;
  int *ap_alt_hld_led;
  float *ap_alt_hld_led_f;
  int *ap_lnav;
  float *ap_lnav_led;
  int *ap_vor_loc;
  int *ap_vor_loc_led;
  float *ap_vor_loc_led_f;
  int *ap_app;
  int *ap_app_led;
  float *ap_app_led_f;
  int *ap_hdg_sel;
  int *ap_hdg_sel_led;
  float *ap_hdg_sel_led_f;
  int *ap_lvl_chg;
  float *ap_lvl_chg_led;
  int *ap_vnav;
  int *ap_vnav_led;
  float *ap_vnav_led_f;
  int *ap_n1;
  float *ap_n1_led;
  int *ap_at_arm;
  float *ap_at_arm_status;
  int *ap_speed;
  float *ap_speed_led;
  int *ap_speed_co;
  int *ap_speed_interv;
  int *ap_alt_interv;
  if (acf_type == 3) {
    ap_cmd_a = link_dataref_cmd_hold("laminar/B738/autopilot/cmd_a_press");     // MCP CMD A mode
    ap_cmd_a_led = link_dataref_flt("laminar/B738/autopilot/cmd_a_status",0);
    ap_cmd_b = link_dataref_cmd_hold("laminar/B738/autopilot/cmd_b_press");     // MCP CMD B mode
    ap_cmd_b_led = link_dataref_flt("laminar/B738/autopilot/cmd_b_status",0);
    ap_cws_a = link_dataref_cmd_hold("laminar/B738/autopilot/cws_a_press");     // MCP CWS A mode
    ap_cws_a_led = link_dataref_flt("laminar/B738/autopilot/cws_a_status",0);     // MCP CWS A mode
    ap_cws_b = link_dataref_cmd_hold("laminar/B738/autopilot/cws_b_press");     // MCP CWS B mode
    ap_cws_b_led = link_dataref_flt("laminar/B738/autopilot/cws_b_status",0);     // MCP CWS B mode
    ap_fdir_a = link_dataref_int("laminar/B738/switches/autopilot/fd_ca");    // Flight Director mode CA (A)
    ap_fdir_a_led = link_dataref_flt("laminar/B738/autopilot/master_capt_status",0);
    ap_fdir_b = link_dataref_int("laminar/B738/switches/autopilot/fd_fo");    // Flight Director mode FO (B)
    ap_fdir_b_led = link_dataref_flt("laminar/B738/autopilot/master_fo_status",0);
    ap_engage = link_dataref_cmd_once("laminar/B738/autopilot/disconnect_toggle"); // AP engage toggle
    ap_disengage_status = link_dataref_flt("laminar/B738/autopilot/disconnect_pos",0);  // Needs the status too
    ap_vs_arm = link_dataref_cmd_hold("laminar/B738/autopilot/vs_press");   // VS hold MODE (orange)
    ap_vs_engage_f = link_dataref_flt("laminar/B738/autopilot/vs_status",0);  // VS hold MODE (green)
    ap_alt_hld = link_dataref_cmd_hold("laminar/B738/autopilot/alt_hld_press");  // ALT HOLD MODE
    ap_alt_hld_led_f = link_dataref_flt("laminar/B738/autopilot/alt_hld_status",0);
    ap_lnav = link_dataref_cmd_hold("laminar/B738/autopilot/lnav_press");      // AP lateral NAV button
    ap_lnav_led = link_dataref_flt("laminar/B738/autopilot/lnav_status",0);
    ap_vor_loc = link_dataref_cmd_hold("laminar/B738/autopilot/vorloc_press");   // AP VOR LOC button
    ap_vor_loc_led_f = link_dataref_flt("laminar/B738/autopilot/vorloc_status",0);
    ap_app = link_dataref_cmd_hold("laminar/B738/autopilot/app_press");       // AP APP button
    ap_app_led_f = link_dataref_flt("laminar/B738/autopilot/app_status",0);
    ap_hdg_sel = link_dataref_cmd_hold("laminar/B738/autopilot/hdg_sel_press");   // AP HDG SEL button
    ap_hdg_sel_led_f = link_dataref_flt("laminar/B738/autopilot/hdg_sel_status",0);
    ap_lvl_chg = link_dataref_cmd_hold("laminar/B738/autopilot/lvl_chg_press");   // AP LVL CHG button
    ap_lvl_chg_led = link_dataref_flt("laminar/B738/autopilot/lvl_chg_status",0); 
    ap_vnav = link_dataref_cmd_hold("laminar/B738/autopilot/vnav_press");      // AP vertical NAV button
    ap_vnav_led_f = link_dataref_flt("laminar/B738/autopilot/vnav_status1",0);
    ap_n1 = link_dataref_cmd_hold("laminar/B738/autopilot/n1_press"); // AP speed N1 button
    ap_n1_led = link_dataref_flt("laminar/B738/autopilot/n1_status1",0); // 0: off, 1: armed, 2: captured
    ap_at_arm = link_dataref_cmd_once("laminar/B738/autopilot/autothrottle_arm_toggle");    // AP Autothrottle arm
    ap_at_arm_status = link_dataref_flt("laminar/B738/autopilot/autothrottle_status1",0);
    ap_speed = link_dataref_cmd_hold("laminar/B738/autopilot/speed_press");     // AP mcpspd select switch
    ap_speed_led = link_dataref_flt("laminar/B738/autopilot/speed_status1",0);
    ap_speed_co = link_dataref_cmd_hold("laminar/B738/autopilot/change_over_press");
    ap_speed_interv = link_dataref_cmd_hold("laminar/B738/autopilot/spd_interv");
    ap_alt_interv = link_dataref_cmd_hold("laminar/B738/autopilot/alt_interv");
  } else if (acf_type == 1) {
    ap_cmd_a = link_dataref_cmd_hold("x737/mcp/CMDA_TOGGLE");     // MCP CMD A mode
    ap_cmd_a_led = link_dataref_flt("x737/systems/MCP/LED_CMDA_on",0);
    ap_cmd_b = link_dataref_cmd_hold("x737/mcp/CMDB_TOGGLE");     // MCP CMD B mode
    ap_cmd_b_led = link_dataref_flt("x737/systems/MCP/LED_CMDB_on",0);
    ap_cws_a = link_dataref_int("xpserver/ap_cws_a");     // MCP CWS A mode
    ap_cws_a_led = link_dataref_flt("xpserver/ap_cws_a_led",0);     // MCP CWS A mode
    ap_cws_b = link_dataref_int("xpserver/ap_cws_b");     // MCP CWS B mode
    ap_cws_b_led = link_dataref_flt("xpserver/ap_cws_b_led",0);     // MCP CWS B mode
    ap_fdir_a = link_dataref_int("x737/systems/afds/fdA_status");    // Flight Director mode CA (A)
    ap_fdir_a_led = link_dataref_flt("x737/systems/MCP/LED_FDA_MA_on",0);
    ap_fdir_b = link_dataref_int("x737/systems/afds/fdB_status");    // Flight Director mode FO (B)
    ap_fdir_b_led = link_dataref_flt("x737/systems/MCP/LED_FDB_MA_on",0);
    ap_engage = link_dataref_int("x737/systems/afds/APengaged");    // AP engage/disengage mode
    ap_vs_arm = link_dataref_int("x737/systems/afds/VS_arm");   // VS hold MODE (orange)
    ap_vs_engage = link_dataref_int("x737/systems/afds/VS");  // VS hold MODE (green)
    ap_alt_hld = link_dataref_cmd_hold("x737/mcp/ALTHLD_TOGGLE");  // ALT HOLD MODE
    ap_alt_hld_led = link_dataref_int("x737/systems/MCP/LED_ALTHLD_on");
    ap_lnav = link_dataref_cmd_hold("x737/mcp/LNAV_TOGGLE");      // AP lateral NAV button
    ap_lnav_led = link_dataref_flt("x737/systems/MCP/LED_LNAV_on",0);
    ap_vor_loc = link_dataref_cmd_hold("x737/mcp/VORLOC_TOGGLE");   // AP VOR LOC button
    ap_vor_loc_led = link_dataref_int("x737/systems/MCP/LED_VORLOC_on");
    ap_app = link_dataref_cmd_hold("x737/mcp/APP_TOGGLE");       // AP APP button
    ap_app_led = link_dataref_int("x737/systems/MCP/LED_APP_on");
    ap_hdg_sel = link_dataref_cmd_hold("x737/mcp/HDGSEL_TOGGLE");
    ap_hdg_sel_led = link_dataref_int("x737/systems/MCP/LED_HDG_on");
    ap_lvl_chg = link_dataref_cmd_hold("x737/mcp/LVLCHANGE_TOGGLE");   // AP LVL CHG button
    ap_lvl_chg_led = link_dataref_flt("x737/systems/MCP/LED_LVLCHG_on",0);
    ap_vnav = link_dataref_cmd_hold("x737/mcp/VNAV_TOGGLE");      // AP vertical NAV button
    ap_vnav_led = link_dataref_int("x737/systems/MCP/LED_VNAV_on");
    ap_n1 = link_dataref_cmd_hold("x737/mcp/N1_MODE_TOGGLE");        // AP speed N1 button
    ap_n1_led = link_dataref_flt("x737/systems/MCP/LED_N1_on",0);
    ap_at_arm = link_dataref_int("x737/systems/athr/athr_armed");    // AP Autothrottle arm
    ap_speed = link_dataref_cmd_hold("x737/mcp/MCPSPD_MODE_TOGGLE");     // AP mcpspd select switch
    ap_speed_led = link_dataref_flt("x737/systems/MCP/LED_MCPSPD_on",0);
    ap_speed_co = link_dataref_int("xpserver/change_over_press");
    ap_speed_interv = link_dataref_int("xpserver/spd_interv");
    ap_alt_interv = link_dataref_int("xpserveralt_interv");
  } else {
    ap_cmd_a = link_dataref_int("xpserver/CMD_A");     // MCP CMD A mode
    ap_cmd_a_led = link_dataref_flt("xpserver/LED_CMDA_on",0);
    ap_cmd_b = link_dataref_int("xpserver/CMD_B");     // MCP CMD B mode
    ap_cmd_b_led = link_dataref_flt("xpserver/LED_CMDB_on",0);
    ap_cws_a = link_dataref_cmd_hold("sim/autopilot/CWSA");     // MCP CWS A mode
    ap_cws_a_led = link_dataref_flt("xpserver/ap_cws_a_led",0);     // MCP CWS A mode
    ap_cws_b = link_dataref_cmd_hold("sim/autopilot/CWSB");     // MCP CWS B mode
    ap_cws_b_led = link_dataref_flt("xpserver/ap_cws_b_led",0);     // MCP CWS B mode
    ap_fdir_a = link_dataref_int("sim/cockpit2/autopilot/flight_director_mode");    // Flight Director mode CA (A)
    ap_fdir_a_status = link_dataref_int("sim/cockpit2/autopilot/flight_director_master_pilot");
    ap_fdir_b = link_dataref_int("sim/cockpit2/autopilot/flight_director2_mode");    // Flight Director mode FO (B)
    ap_fdir_b_status = link_dataref_int("sim/cockpit2/autopilot/flight_director_master_copilot");
    ap_engage = link_dataref_int("sim/cockpit/autopilot/autopilot_mode");    // AP engage/disengage mode
    ap_vs_arm = link_dataref_cmd_hold("sim/autopilot/vertical_speed");   // VS hold MODE (orange)
    ap_vs_engage = link_dataref_int("sim/cockpit2/autopilot/vvi_status");  // VS hold MODE (green)
    ap_alt_hld = link_dataref_cmd_hold("sim/autopilot/altitude_hold");  // ALT HOLD MODE
    ap_alt_hld_led = link_dataref_int("sim/cockpit2/autopilot/altitude_hold_armed");
    ap_lnav = link_dataref_int("xpserver/LNAV_arm");      // AP lateral NAV button
    ap_lnav_led = link_dataref_flt("xpserver/LED_LNAV_on",0);
    ap_vor_loc = link_dataref_cmd_hold("sim/autopilot/NAV");   // AP VOR LOC button
    ap_vor_loc_led = link_dataref_int("sim/cockpit2/autopilot/hnav_armed");
    ap_app = link_dataref_cmd_hold("sim/autopilot/approach");       // AP APP button
    ap_app_led = link_dataref_int("sim/cockpit2/autopilot/approach_status");
    ap_hdg_sel = link_dataref_cmd_hold("sim/autopilot/heading");   // AP HDG SEL button
    ap_hdg_sel_led = link_dataref_int("sim/cockpit2/autopilot/heading_status");
    ap_lvl_chg = link_dataref_cmd_hold("sim/autopilot/level_change");   // AP LVL CHG button
    ap_lvl_chg_led = link_dataref_flt("xpserver/LED_LVLCHG_on",0); 
    ap_vnav = link_dataref_cmd_hold("sim/autopilot/FMS");      // AP vertical NAV button
    ap_vnav_led = link_dataref_int("sim/cockpit2/autopilot/vnav_armed");
    ap_n1 = link_dataref_int("xpserver/N1_mode");        // AP speed N1 button
    ap_n1_led = link_dataref_flt("xpserver/LED_N1_on",0); // 0: off, 1: armed, 2: captured
    ap_at_arm = link_dataref_int("sim/cockpit2/autopilot/autothrottle_enabled");    // AP Autothrottle arm
    ap_speed = link_dataref_int("xpserver/MCPSPD_mode");     // AP mcpspd select switch
    ap_speed_led = link_dataref_flt("xpserver/LED_MCPSPD_on",0);
    ap_speed_co = link_dataref_int("xpserver/change_over_press");
    ap_speed_interv = link_dataref_int("xpserver/spd_interv");
    ap_alt_interv = link_dataref_int("xpserveralt_interv");
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


  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");

  /* blank displays if avionics are off */
  int display_brightness = 15;
  if (*avionics_on != 1) display_brightness = 0;

  /* INPUTS */
  
  /* Flight Director Capain */
  /*
  if ((acf_type == 0) && (*ap_fdir_a != INT_MISS)) {
    *ap_fdir_a = *ap_fdir_a / 2;
  }
  */
  ret = digital_input(card,2,ap_fdir_a,0);
  if (ret==1) {
    printf("Flight Director Captain: %i \n",*ap_fdir_a);
  }
  /*
  if ((acf_type == 0) && (*ap_fdir_a != INT_MISS)) {
    *ap_fdir_a = 2* *ap_fdir_a;
    }*/

  /* Flight Director Copilot */
  /*
  if ((acf_type == 0) && (*ap_fdir_b != INT_MISS)) {
    *ap_fdir_b = *ap_fdir_b / 2;
  }
  */
  ret = digital_input(card,29,ap_fdir_b,0);
  if (ret==1) {
    printf("Flight Director First Officer: %i \n",*ap_fdir_b);
  }
  /*
  if ((acf_type == 0) && (*ap_fdir_b != INT_MISS)) {
    *ap_fdir_b = 2* *ap_fdir_b;
  }
  */
  
  /* AT ARM SWITCH */
  /* only change at_arm if switch has changed */
  /* safe algorithm for solenoid relais */
  if (interval_counter == 0) {
    ret = digital_inputf(card,3,&ap_at_arm_status_save,0);
    if (ret==1) {
      //printf("%f %i \n",*ap_at_arm_status,*ap_at_arm);
      if ((acf_type == 2) || (acf_type == 3)) {
	*ap_at_arm = 0;
	if ((*ap_at_arm_status == 0.0) && (ap_at_arm_status_save == 1.0)) {
	  *ap_at_arm = 1;
	  printf("ARMING AT\n");
	}
	if ((*ap_at_arm_status == 1.0) && (ap_at_arm_status_save == 0.0)) {
	  *ap_at_arm = 1;
	  printf("DISARMING AT\n");
	}
      } else {
	//printf("%f \n",ap_at_arm_status_save);
	//*ap_at_arm_status = ap_at_arm_status_save;
      }
    } else {
      //printf("%f %i \n",*ap_at_arm_status,ap_at_arm_status_save);
      /* trigger AT solenoid to disarm AT if software AT status changes to off 
	 and if switch is still in on position (temp==1).
	 Only trigger after command has been executed, means
	 that after *ap_at_arm is back to 0 */
      if ((acf_type == 2) || (acf_type == 3)) {
	if ((*ap_at_arm_status == 0.0) && (ap_at_arm_status_save == 1.0) && (*ap_at_arm == 0)) {
	  printf("AT was Disarmed by X-Plane\n");
	  ret = digital_output(card, 21, &one);
	} else {
	  ret = digital_output(card, 21, &zero);
	}
      } else {
	if (*ap_at_arm == 1) {
	  ret = digital_output(card, 21, &one);
	} else {
	ret = digital_output(card, 21, &zero);
	}
      }
    }
  }

  /* AP ENGAGE SWITCH */
  if ((acf_type == 2) || (acf_type == 3)) {
    /* with ZIBO we can only toggle the switch, so check status as well */
    *ap_engage = 0;
    ret = digital_inputf(card,24,ap_disengage_status,0);
    if (ret == 1) {
      if (*ap_disengage_status == 0.0) {
	printf("AP Engaged \n");
	*ap_engage = 1;
      }
      if (*ap_disengage_status == 1.0) {
	printf("AP Disengaged \n");
	*ap_engage = 1;
      }
    }
  } else if (acf_type == 1) {
    ret = digital_input(card,24,&temp,0);
    if (ret == 1) {
      *ap_engage = temp;
    }
  } else {
    ret = digital_input(card,24,&temp,0);
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

  ret = digital_input(card,4,ap_n1,0);
  if (ret == 1) printf("N1 Button: %i \n",*ap_n1);  
  ret = digital_input(card,5,ap_speed,0);
  if (ret == 1) printf("SPEED Button: %i \n",*ap_speed);
  ret = digital_input(card,7,ap_lvl_chg,0);
  if (ret == 1) printf("LVL CHG Button: %i \n",*ap_lvl_chg);
  ret = digital_input(card,10,ap_vnav,0);
  if (ret == 1) printf("VNAV Button: %i \n",*ap_vnav);
  ret = digital_input(card,11,ap_hdg_sel,0);
  if (ret == 1) printf("HDG SEL Button: %i \n",*ap_hdg_sel);
  ret = digital_input(card,15,ap_app,0);
  if (ret == 1) printf("APP Button: %i \n",*ap_app);
  ret = digital_input(card,16,ap_vor_loc,0);
  if (ret == 1) printf("VOR LOC Button: %i \n",*ap_vor_loc);
  ret = digital_input(card,17,ap_lnav,0);
  if (ret == 1) printf("LNAV Button: %i \n",*ap_lnav);
  ret = digital_input(card,18,ap_alt_hld,0);
  if (ret == 1) printf("ALT HLD Button: %i \n",*ap_alt_hld);
  if (acf_type == 1) {
    ret = digital_input(card,29,ap_vs_arm,1);
    if (ret == 1) {
      printf("V/S Button: %i \n",*ap_vs_arm);
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
  } else if ((acf_type == 2) || (acf_type == 3)) {
    ret = digital_input(card,23,ap_vs_arm,0);
    if (ret == 1) printf("V/S Button: %i \n",*ap_vs_arm);
  } else {
    ret = digital_input(card,23,ap_vs_arm,0);
    if (ret == 1) printf("V/S Button: %i \n",*ap_vs_arm);
  }

  ret = digital_input(card,26,ap_cmd_a,0);
  if (ret == 1) printf("CMD A Button: %i \n",*ap_cmd_a);
  ret = digital_input(card,28,ap_cmd_b,0);
  if (ret == 1) printf("CMD B Button: %i \n",*ap_cmd_b);
  ret = digital_input(card,25,ap_cws_a,0);
  if (ret == 1) printf("CWS A Button: %i \n",*ap_cws_a);
  ret = digital_input(card,27,ap_cws_b,0);
  if (ret == 1) printf("CWS B Button: %i \n",*ap_cws_b);

  ret = digital_input(card,6,ap_speed_co,0);
  if (ret == 1) printf("C/O Button: %i \n",*ap_speed_co);
  ret = digital_input(card,96,ap_speed_interv,0);
  if (ret == 1) printf("SPD INTV Button: %i \n",*ap_speed_interv);
  ret = digital_input(card,97,ap_alt_interv,0);
  if (ret == 1) printf("ALT INTV Button: %i \n",*ap_alt_interv);


  ret = digital_input(card,102,&bank10,0);
  if (ret == 1) {
    printf("Bank Limit 10\n");
  }
  ret = digital_input(card,101,&bank15,0);
  if (ret == 1) {
    printf("Bank Limit 15\n");
  }
  ret = digital_input(card,100,&bank20,0);
  if (ret == 1) {
    printf("Bank Limit 20\n");
  }
  ret = digital_input(card,99,&bank25,0);
  if (ret == 1) {
    printf("Bank Limit 25\n");
  }
  ret = digital_input(card,98,&bank30,0);
  if (ret == 1) {
    printf("Bank Limit 30\n");
  }
  /* Bank limit - 0 = auto, 1-6 = 5-30 degrees of bank */
  if ((acf_type == 2) || (acf_type == 3)) {
    float ap_banklimit_hw = (float) (bank15 + bank20*2 + bank25*3 + bank30*4);
    ret = set_state_updnf(&ap_banklimit_hw, ap_banklimit_pos, ap_banklimit_up, ap_banklimit_dn);
    if (ret == 1) {
      printf("banklimit: HW: %f XP: %f \n",ap_banklimit_hw,*ap_banklimit_pos);
    }
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

  
  /* ENCODERS */
  ret = encoder_inputf(card, 0, 1, ap_course1, 1.0, 1);
  if ((ret==1) && (*ap_course1 != FLT_MISS)) {
    if (*ap_course1 < 0.0) *ap_course1 = 359.0;
    if (*ap_course1 > 359.0) *ap_course1 = 0.0;
    printf("NAV1 Course: %f \n",*ap_course1);
  }
  
  ret = encoder_inputf(card, 30, 31, ap_course2, 1.0, 1);
  if ((ret==1) && (*ap_course2 != FLT_MISS)) {
    if (*ap_course2 < 0.0) *ap_course2 = 359.0;
    if (*ap_course2 > 359.0) *ap_course2 = 0.0;
    printf("NAV2 Course: %f \n",*ap_course2);
  }
  
  if (*ap_spd_is_mach == 1) {
    ret = encoder_inputf(card,8,9,ap_ias,0.01,1);
  } else {
    ret = encoder_inputf(card,8,9,ap_ias,1.0,1);
  }
  
  if (ret==1) printf("AP IAS: %f \n",*ap_ias);
  ret = encoder_inputf(card, 13, 14, ap_heading, 1.0, 1);
  if ((ret==1) && (*ap_heading != FLT_MISS)) {
    if (*ap_heading < 0.0) *ap_heading = 359.0;
    if (*ap_heading > 359.0) *ap_heading = 0.0;
    printf("AP Heading: %f \n",*ap_heading);
  }
  
  ret = encoder_inputf(card, 19, 20, ap_altitude, 100.0, 1);
  if ((ret==1) && (*ap_altitude != FLT_MISS)) {
    if (*ap_altitude < 0.0) *ap_altitude = 0.0;
    printf("AP Altitude: %f \n",*ap_altitude);
  }
  
  if ((*ap_vspeed < 1000.) && (*ap_vspeed > -1000.)) {
    ret = encoder_inputf(card, 21, 22, ap_vspeed, -50.0, 1);
  } else {
    ret = encoder_inputf(card, 21, 22, ap_vspeed, -100.0, 1);
  }
  if (ret==1) printf("AP Vertical Speed: %f \n",*ap_vspeed);

  /* OUTPUTS */
  
  /* Flight Director A LED */
  if ((acf_type == 1) || (acf_type == 2) || (acf_type == 3)) {
    if (*avionics_on == 1) {
      if (*ap_fdir_a_led == 2) {
	ret = digital_output(card,0,&one);
      } else {
	ret = digital_outputf(card,0,ap_fdir_a_led);
      }
    } else {
      ret = digital_output(card,0,&zero);
    }  
  } else {
    ret = digital_output(card,0,ap_fdir_a_status);
  }
  /* Flight Director B LED */
  if ((acf_type == 1) || (acf_type == 2) || (acf_type == 3)) {
    if (*avionics_on == 1) {
      if (*ap_fdir_b_led == 2) {
 	ret = digital_output(card,16,&one);
      } else {
	ret = digital_outputf(card,16,ap_fdir_b_led);
      }
    } else {
      ret = digital_output(card,16,&zero);
    }   
  } else {
    ret = digital_output(card,16,ap_fdir_b_status);
  }
  
  /* A/T ARM LED */
  if ((acf_type == 2) || (acf_type == 3)) {
    ret = digital_outputf(card,1,ap_at_arm_status);
  } else {
    if (*ap_at_arm == 1) {
      ret = digital_output(card,1,&one);
    } else {
      ret = digital_output(card,1,&zero);
    }
  }
  /* N1 LED */
  ret = digital_outputf(card,2,ap_n1_led);
  /* SPEED LED */
  ret = digital_outputf(card,3,ap_speed_led);
  /* LVL CHG LED */
  ret = digital_outputf(card,4,ap_lvl_chg_led);
  /* VNAV LED */
  if ((acf_type == 2) || (acf_type == 3)) {
    ret = digital_outputf(card,5,ap_vnav_led_f);
  } else {
    ret = digital_output(card,5,ap_vnav_led);
  }
  /* HDG SEL LED */
  if ((acf_type == 2) || (acf_type == 3)) {
    if (*ap_hdg_sel_led_f == 2.0) {
      ret = digital_output(card,6,&one);
    } else {
      ret = digital_outputf(card,6,ap_hdg_sel_led_f);
    }
  } else {
    if (*ap_hdg_sel_led == 2) {
      ret = digital_output(card,6,&one);
    } else {
      ret = digital_output(card,6,ap_hdg_sel_led);
    }
  }
  /* APP LED */
  if ((acf_type == 2) || (acf_type == 3)) {
    ret = digital_outputf(card,7,ap_app_led_f);
  } else {
    ret = digital_output(card,7,ap_app_led);
  }
  /* VOR LOC LED */
  if ((acf_type == 2) || (acf_type == 3)) {
    ret = digital_outputf(card,8,ap_vor_loc_led_f);
  } else {
    ret = digital_output(card,8,ap_vor_loc_led);
  }
  /* LNAV LED */
  ret = digital_outputf(card,9,ap_lnav_led);
  /* ALT HLD LED */
  if ((acf_type == 2) || (acf_type == 3)) {
    ret = digital_outputf(card,10,ap_alt_hld_led_f); 
  } else {
    ret = digital_output(card,10,ap_alt_hld_led);
  }
  /* V/S LED */
  if ((acf_type == 2) || (acf_type == 3)) {
    ret = digital_outputf(card,11,ap_vs_engage_f);
  } else {
    ret = digital_output(card,11,ap_vs_engage);
  }
  /* CWS A LED */
  ret = digital_outputf(card,12,ap_cws_a_led);
  /* CMD A LED */
  ret = digital_outputf(card,13,ap_cmd_a_led);
  /* CWS B LED */
  ret = digital_outputf(card,14,ap_cws_b_led);
  /* CMD B LED */
  ret = digital_outputf(card,15,ap_cmd_b_led);
 
  /* Switch Background Lighting LED's */ 
  ret = digital_output(card,17,avionics_on);

  /* DISPLAYS */
  ret = display_outputf(card, 0, 3, ap_course1, -1, display_brightness);
  ret = display_outputf(card, 21, 3, ap_course2, -1, display_brightness);

  int ap_spd_show = 0;
  if ((acf_type == 2) || (acf_type == 3)) {
    if (*ap_ias_show == 1.0) ap_spd_show = 1;
  } else {
    ap_spd_show = 1;
  }

  if (ap_spd_show == 1) {
    if (*ap_spd_is_mach == 1) {
      float ftemp = *ap_ias * 100.0;
      ret = display_outputf(card,3,4, ap_ias, -1, display_brightness);  // clear all digits
      ret = display_outputf(card,4,3, &ftemp, 2, display_brightness); // mach spd in two first digits
    } else {
      ret = display_outputf(card,3,4, ap_ias, -1, display_brightness);
    }
  } else {
    ret = display_outputf(card,3,4, ap_ias, -1, zero);
  }
  ret = display_outputf(card, 8, 3, ap_heading, -1, display_brightness);
  if (*ap_heading != FLT_MISS) {
    /* HDG always has leading zeros */
    if (*ap_heading < 100.0) {
      ret = display_output(card, 10, 1, &zero, -1, display_brightness);
      if (*ap_heading < 10.0) {
	ret = display_output(card, 9, 1, &zero, -1, display_brightness);
      } 
    }
  }
  ret = display_outputf(card, 11, 5, ap_altitude, -1, display_brightness);

  int ap_vs_show = 0;
  if ((acf_type == 2) || (acf_type == 3)) {
    if (*ap_vspeed_show >= 1.0) ap_vs_show = 1;
  } else {
    ap_vs_show = 1;
  }
  if (ap_vs_show == 1) {
    ret = display_outputf(card, 16, 5, ap_vspeed, -1, display_brightness);
  } else {
    ret = display_outputf(card, 16, 5, ap_vspeed, -1, zero);
  }
  
}

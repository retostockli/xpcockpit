/* This is the b737_efis.c code which connects to the SISMO EFIS Module
   call it with argument 0 for captain and 1 for copilot side

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
#include "b737_efis.h"


void b737_efis(int copilot)
{

  /* In the SISMO EFIS all switches are inverted (0=on; 1=off) */
  
  int ret;
  int temp;
  int card = 0;
  int one = 1;
  int zero = 0;

  /* input offset between copilot and captain input pins */
  int offset;
  if (copilot) {
    offset = 32;
  } else {
    offset = 0;
  }

  float *altimeter_pressure;
  if ((acf_type == 2) || (acf_type == 3)) {
    if (copilot) {
      altimeter_pressure = link_dataref_flt("laminar/B738/EFIS/baro_sel_in_hg_copilot",-2);
    } else {
      altimeter_pressure = link_dataref_flt("laminar/B738/EFIS/baro_sel_in_hg_pilot",-2);
    }
  } else {
    altimeter_pressure = link_dataref_flt("sim/cockpit/misc/barometer_setting",-2);
  }
  
  int *altimeter_pressure_unit;
  int *altimeter_pressure_unit_dn;
  int *altimeter_pressure_unit_up;
  if ((acf_type == 2) || (acf_type == 3)) {
    /* read only. 0=in, 1=hpa */
    if (copilot) {
      altimeter_pressure_unit_dn = link_dataref_cmd_once("laminar/B738/EFIS_control/fo/baro_in_hpa_dn");
      altimeter_pressure_unit_up = link_dataref_cmd_once("laminar/B738/EFIS_control/fo/baro_in_hpa_up");
      altimeter_pressure_unit = link_dataref_int("laminar/B738/EFIS_control/fo/baro_in_hpa");
    } else {
      altimeter_pressure_unit_dn = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/baro_in_hpa_dn");
      altimeter_pressure_unit_up = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/baro_in_hpa_up");
      altimeter_pressure_unit = link_dataref_int("laminar/B738/EFIS_control/capt/baro_in_hpa");
    }
    *altimeter_pressure_unit_dn = 0;
    *altimeter_pressure_unit_up = 0;
  } else if (acf_type == 1) {
    altimeter_pressure_unit = link_dataref_int("x737/systems/units/baroPressUnit");
  } else {
    altimeter_pressure_unit = link_dataref_int("xpserver/barometer_unit");   
  }
  
  //  float *navrangesel_x737 = link_dataref_flt("x737/cockpit/EFISCTRL_0/ND_RANGE_ENUM",0);
  int *navrangesel;
  if ((acf_type == 2) || (acf_type == 3)) {
    if (copilot) {
      navrangesel = link_dataref_int("laminar/B738/EFIS/fo/map_range");
    } else {
      navrangesel = link_dataref_int("laminar/B738/EFIS/capt/map_range");
    }
  } else {
    navrangesel = link_dataref_int("sim/cockpit2/EFIS/map_range");
  }

  // Get Map Mode (APP/VOR/MAP/PLN)
  int *map_mode;
  if ((acf_type == 2) || (acf_type == 3)) {
    if (copilot) {
      map_mode = link_dataref_int("laminar/B738/EFIS_control/fo/map_mode_pos");
    } else {
      map_mode = link_dataref_int("laminar/B738/EFIS_control/capt/map_mode_pos");
    }
  } else {
    map_mode = link_dataref_int("sim/cockpit2/EFIS/map_mode");
  }
  
  int *minimum_mode;
  float *altimeter_minimum;
  int *minimum_mode_dn;
  int *minimum_mode_up;
  if ((acf_type == 2) || (acf_type == 3)) {
    if (copilot) {
      minimum_mode = link_dataref_int("laminar/B738/EFIS_control/fo/minimums");
      minimum_mode_dn = link_dataref_cmd_once("laminar/B738/EFIS_control/fo/minimums_dn");
      minimum_mode_up = link_dataref_cmd_once("laminar/B738/EFIS_control/fo/minimums_up");
      altimeter_minimum = link_dataref_flt("laminar/B738/pfd/dh_copilot",0);
    } else {
      minimum_mode = link_dataref_int("laminar/B738/EFIS_control/cpt/minimums");
      minimum_mode_dn = link_dataref_cmd_once("laminar/B738/EFIS_control/cpt/minimums_dn");
      minimum_mode_up = link_dataref_cmd_once("laminar/B738/EFIS_control/cpt/minimums_up");
      altimeter_minimum = link_dataref_flt("laminar/B738/pfd/dh_pilot",0);
    }
    *minimum_mode_dn = 0;
    *minimum_mode_up = 0;
  } else {
    altimeter_minimum = link_dataref_flt("sim/cockpit/misc/radio_altimeter_minimum",0);
  }
  
  int *efis1_sel;
  int *efis1_sel_up;
  int *efis1_sel_dn;
  if ((acf_type == 2) || (acf_type == 3)) {
    if (copilot) {
      efis1_sel = link_dataref_int("laminar/B738/EFIS_control/fo/vor1_off_pos");
      efis1_sel_up = link_dataref_cmd_once("laminar/B738/EFIS_control/fo/vor1_off_up");
      efis1_sel_dn = link_dataref_cmd_once("laminar/B738/EFIS_control/fo/vor1_off_dn");
    } else {
      efis1_sel = link_dataref_int("laminar/B738/EFIS_control/capt/vor1_off_pos");
      efis1_sel_up = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/vor1_off_up");
      efis1_sel_dn = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/vor1_off_dn");
    }
    *efis1_sel_up = 0;
    *efis1_sel_dn = 0;
  } else {
    efis1_sel = link_dataref_int("sim/cockpit2/EFIS/EFIS_1_selection_pilot");
  }
    
  int *efis2_sel;
  int *efis2_sel_up;
  int *efis2_sel_dn;
  if ((acf_type == 2) || (acf_type == 3)) {
    if (copilot) {
      efis2_sel = link_dataref_int("laminar/B738/EFIS_control/fo/vor2_off_pos");
      efis2_sel_up = link_dataref_cmd_once("laminar/B738/EFIS_control/fo/vor2_off_up");
      efis2_sel_dn = link_dataref_cmd_once("laminar/B738/EFIS_control/fo/vor2_off_dn");
    } else {
      efis2_sel = link_dataref_int("laminar/B738/EFIS_control/capt/vor2_off_pos");
      efis2_sel_up = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/vor2_off_up");
      efis2_sel_dn = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/vor2_off_dn");
    }
    *efis2_sel_up = 0;
    *efis2_sel_dn = 0;
  } else {
    efis2_sel = link_dataref_int("sim/cockpit2/EFIS/EFIS_2_selection_pilot");
  }

  int *efis_wxr;
  int *efis_sta;
  int *efis_wpt;
  int *efis_apt;
  int *efis_data;
  int *efis_pos;
  int *efis_terr;
  if ((acf_type == 2) || (acf_type == 3)) {
    if (copilot) {
      efis_wxr = link_dataref_cmd_once("laminar/B738/EFIS_control/fo/push_button/wxr_press");
      efis_sta = link_dataref_cmd_once("laminar/B738/EFIS_control/fo/push_button/sta_press");
      efis_wpt = link_dataref_cmd_once("laminar/B738/EFIS_control/fo/push_button/wpt_press");
      efis_apt = link_dataref_cmd_once("laminar/B738/EFIS_control/fo/push_button/arpt_press");
      efis_data = link_dataref_cmd_once("laminar/B738/EFIS_control/fo/push_button/data_press");
      efis_pos = link_dataref_cmd_once("laminar/B738/EFIS_control/fo/push_button/pos_press");
      efis_terr = link_dataref_cmd_once("laminar/B738/EFIS_control/fo/push_button/terr_press");
    } else {
      efis_wxr = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/push_button/wxr_press");
      efis_sta = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/push_button/sta_press");
      efis_wpt = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/push_button/wpt_press");
      efis_apt = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/push_button/arpt_press");
      efis_data = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/push_button/data_press");
      efis_pos = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/push_button/pos_press");
      efis_terr = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/push_button/terr_press");
    }
    *efis_wxr = 0;
    *efis_sta = 0;
    *efis_wpt = 0;
    *efis_apt = 0;
    *efis_data = 0;
    *efis_pos = 0;
    *efis_terr = 0;
  } else if (acf_type == 1) {
    efis_wxr = link_dataref_int("x737/cockpit/EFISCTRL_0/WXR_on");
    efis_sta = link_dataref_int("x737/cockpit/EFISCTRL_0/STA_on");
    efis_wpt = link_dataref_int("x737/cockpit/EFISCTRL_0/WPT_on");
    efis_apt = link_dataref_int("x737/cockpit/EFISCTRL_0/ARPT_on");
    efis_data = link_dataref_int("x737/cockpit/EFISCTRL_0/DATA_on");
    efis_pos = link_dataref_int("x737/cockpit/EFISCTRL_0/POS_on");
    efis_terr = link_dataref_int("x737/cockpit/EFISCTRL_0/TERR_on");
  } else {
    efis_wxr = link_dataref_int("sim/cockpit/switches/EFIFS_shows_weather");
    efis_sta = link_dataref_int("sim/cockpit/switches/EFIS_shows_VORs");
    efis_wpt = link_dataref_int("sim/cockpit/switches/EFIS_shows_waypoints");
    efis_apt = link_dataref_int("sim/cockpit/switches/EFIS_shows_airports");
    efis_data = link_dataref_int("sim/cockpit/switches/EFIS_shows_NDBs");
    efis_pos = link_dataref_int("sim/cockpit/switches/EFIS_shows_tcas");
    efis_terr = link_dataref_int("xpserver/EFIS_terr");
   }

  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");


  /* INPUTS */
  
  /* NAV MAP FEATURE BUTTONS */
  ret = digital_input(card,40+offset,efis_wxr,-1);  // WXR
  ret = digital_input(card,41+offset,efis_sta,-1);  // STA
  ret = digital_input(card,42+offset,efis_wpt,-1);  // WPT
  ret = digital_input(card,43+offset,efis_apt,-1);  // ARPT
  ret = digital_input(card,44+offset,efis_data,-1);  // DATA
  ret = digital_input(card,45+offset,efis_pos,-1);  // POS
  ret = digital_input(card,46+offset,efis_terr,-1);  // TERR

  /* ENCODERS */

  /* OUTPUTS */

  /* DISPLAYS */
  
}

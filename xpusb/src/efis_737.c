/* This is the efis_737.c code which contains a sample set-up for how to communicate with the 
   BOEING 737 EFIS panel (switches, LED's, displays) connected to the OpenCockpits IOCARDS USB device.

   Copyright (C) 2016  Reto Stockli

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
#include "efis_737.h"

void efis_737(void)
{
  
  int ret = 0;
  int zero = 0;
  int temp = 0;
  int temp2 = 0;
  //float ftemp = 0.0;

  int device = 5;
  int card = 0;

  //  int *status_x737 = link_dataref_int("x737/systems/afds/plugin_status");
  int *status_x737 = link_dataref_int("xpserver/status_737");

  float *altimeter_pressure;
  if ((*status_x737 == 2) || (*status_x737 == 3)) {
    altimeter_pressure = link_dataref_flt("laminar/B738/EFIS/baro_sel_in_hg_pilot",-2);
  } else {
    altimeter_pressure = link_dataref_flt("sim/cockpit/misc/barometer_setting",-2);
  }
  
  int *altimeter_pressure_unit;
  int *altimeter_pressure_unit_dn;
  int *altimeter_pressure_unit_up;
  if ((*status_x737 == 2) || (*status_x737 == 3)) {
    /* read only. 0=in, 1=hpa */
    altimeter_pressure_unit_dn = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/baro_in_hpa_dn");
    altimeter_pressure_unit_up = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/baro_in_hpa_up");
    altimeter_pressure_unit = link_dataref_int("laminar/B738/EFIS_control/capt/baro_in_hpa");
    *altimeter_pressure_unit_dn = 0;
    *altimeter_pressure_unit_up = 0;
  } else if (*status_x737 == 1) {
    altimeter_pressure_unit = link_dataref_int("x737/systems/units/baroPressUnit");
  } else {
    altimeter_pressure_unit = link_dataref_int("xpserver/barometer_unit");   
  }
  
  //  float *navrangesel_x737 = link_dataref_flt("x737/cockpit/EFISCTRL_0/ND_RANGE_ENUM",0);
  int *navrangesel;
  if ((*status_x737 == 2) || (*status_x737 == 3)) {
    navrangesel = link_dataref_int("laminar/B738/EFIS/capt/map_range");
    //navrangesel = link_dataref_int("laminar/B738/EFIS/fo/map_range");
  } else {
    navrangesel = link_dataref_int("sim/cockpit2/EFIS/map_range");
  }

  // Get Map Mode (APP/VOR/MAP/PLN)
  int *map_mode;
  if ((*status_x737 == 2) || (*status_x737 == 3)) {
    map_mode = link_dataref_int("laminar/B738/EFIS_control/capt/map_mode_pos");
    // map_mode = link_dataref_int("laminar/B738/EFIS_control/fo/map_mode_pos");
  } else {
    map_mode = link_dataref_int("sim/cockpit2/EFIS/map_mode");
  }
  
  int *minimum_mode;
  float *altimeter_minimum;
  //  int *minimum_mode_dn;
  //  int *minimum_mode_up;
  if ((*status_x737 == 2) || (*status_x737 == 3)) {
    minimum_mode = link_dataref_int("laminar/B738/EFIS_control/cpt/minimums");
    //minimum_mode = link_dataref_int("laminar/B738/EFIS_control/fo/minimums");
    altimeter_minimum = link_dataref_flt("laminar/B738/pfd/dh_pilot",0);
    // altimeter_minimum = link_dataref_flt("laminar/B738/pfd/dh_copilot",0);
    /*
    minimum_mode_dn = link_dataref_cmd_once("laminar/B738/EFIS_control/cpt/minimums_dn");
    minimum_mode_up = link_dataref_cmd_once("laminar/B738/EFIS_control/cpt/minimums_up");
    *minimum_mode_dn = 0;
    *minimum_mode_up = 0;
    */
  } else {
    altimeter_minimum = link_dataref_flt("sim/cockpit/misc/radio_altimeter_minimum",0);
  }
  
  int *efis1_sel_capt;
  int *efis1_sel_capt_up;
  int *efis1_sel_capt_dn;
  if ((*status_x737 == 2) || (*status_x737 == 3)) {
    efis1_sel_capt = link_dataref_int("laminar/B738/EFIS_control/capt/vor1_off_pos");
    efis1_sel_capt_up = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/vor1_off_up");
    efis1_sel_capt_dn = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/vor1_off_dn");
    *efis1_sel_capt_up = 0;
    *efis1_sel_capt_dn = 0;
  } else {
    efis1_sel_capt = link_dataref_int("sim/cockpit2/EFIS/EFIS_1_selection_pilot");
  }
    
  int *efis2_sel_capt;
  int *efis2_sel_capt_up;
  int *efis2_sel_capt_dn;
  if ((*status_x737 == 2) || (*status_x737 == 3)) {
    efis2_sel_capt = link_dataref_int("laminar/B738/EFIS_control/capt/vor2_off_pos");
    efis2_sel_capt_up = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/vor2_off_up");
    efis2_sel_capt_dn = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/vor2_off_dn");
    *efis2_sel_capt_up = 0;
    *efis2_sel_capt_dn = 0;
  } else {
    efis2_sel_capt = link_dataref_int("sim/cockpit2/EFIS/EFIS_2_selection_pilot");
  }

  int *efis_wxr;
  int *efis_sta;
  int *efis_wpt;
  int *efis_apt;
  int *efis_data;
  int *efis_pos;
  int *efis_terr;
  int *efis_wxr_led;
  int *efis_sta_led;
  int *efis_wpt_led;
  int *efis_apt_led;
  int *efis_data_led;
  int *efis_pos_led;
  int *efis_terr_led;
  if ((*status_x737 == 2) || (*status_x737 == 3)) {
    efis_wxr = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/push_button/wxr_press");
    efis_sta = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/push_button/sta_press");
    efis_wpt = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/push_button/wpt_press");
    efis_apt = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/push_button/arpt_press");
    efis_data = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/push_button/data_press");
    efis_pos = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/push_button/pos_press");
    efis_terr = link_dataref_cmd_once("laminar/B738/EFIS_control/capt/push_button/terr_press");
    *efis_wxr = 0;
    *efis_sta = 0;
    *efis_wpt = 0;
    *efis_apt = 0;
    *efis_data = 0;
    *efis_pos = 0;
    *efis_terr = 0;
    efis_wxr_led = link_dataref_int("laminar/B738/EFIS/EFIS_wx_on");
    efis_sta_led = link_dataref_int("laminar/B738/EFIS/EFIS_vor_on");
    efis_wpt_led = link_dataref_int("laminar/B738/EFIS/EFIS_fix_on");
    efis_apt_led = link_dataref_int("laminar/B738/EFIS/EFIS_airport_on");
    efis_data_led = link_dataref_int("xpserver/EFIS_data");
    efis_pos_led = link_dataref_int("xpserver/EFIS_pos");
    efis_terr_led = link_dataref_int("xpserver/EFIS_terr");
  } else if (*status_x737 == 1) {
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

 
  // inputs:
  // 54: VOR2, 55: ADF2
  // 63-65: APP, VOR, MAP
  // 66: VOR1, 67: ADF1
  // 68-69: MINS Selector
  // 71: RADIO/BARO
  
  /* currently unsupported by x737 */

  // mastercard 2, input board # 2 (36-71)
  ret = digital_input(device,card,45,efis_wxr,1);  // WXR
  ret = digital_input(device,card,46,efis_sta,1);  // STA
  ret = digital_input(device,card,47,efis_wpt,1);  // WPT
  ret = digital_input(device,card,48,efis_apt,1);  // ARPT
  ret = digital_input(device,card,49,efis_data,1);  // DATA
  ret = digital_input(device,card,50,efis_pos,1);  // POS
  ret = digital_input(device,card,51,efis_terr,1);  // TERR

  ret = digital_input(device,card,67,&temp,0);
  ret = digital_input(device,card,66,&temp2,0);
  /* EFIS VOR1/OFF/ADF1 selector */
  if ((*status_x737 == 2) || (*status_x737 == 3)) {
    if (*efis1_sel_capt != INT_MISS) {
      if (*efis1_sel_capt < (temp + (1-temp2) - 1)) {
	*efis1_sel_capt_up = 1;
      }
      if (*efis1_sel_capt > (temp + (1-temp2) - 1)) {
	*efis1_sel_capt_dn = 1;
      }     
    }
  } else {
    *efis1_sel_capt = temp + (1-temp2);
  }
 
  ret = digital_input(device,card,55,&temp,0);
  ret = digital_input(device,card,54,&temp2,0);
  /* EFIS VOR2/OFF/ADF2 selector */
  if ((*status_x737 == 2) || (*status_x737 == 3)) {
    if (*efis2_sel_capt != INT_MISS) {
      if (*efis2_sel_capt < (temp + (1-temp2) - 1)) {
	*efis2_sel_capt_up = 1;
      }
      if (*efis2_sel_capt > (temp + (1-temp2) - 1)) {
	*efis2_sel_capt_dn = 1;
      }     
    }
  } else {
    *efis2_sel_capt = temp + (1-temp2);
  }
    
  
  if (*altimeter_pressure != FLT_MISS) {

    if (*altimeter_pressure_unit == 1) {
      *altimeter_pressure /= 0.029530; // in Hg -> 10 * hPa
    } else {
      *altimeter_pressure *= 100.0; // 100 * in Hg
    }
    
    ret = mastercard_encoder(device, card, 51, altimeter_pressure, -1.0, 1.0,2);
    if (ret == 1) {
      printf("Altimeter Pressure: %f \n",*altimeter_pressure);
    }
    if (*altimeter_pressure_unit == 1) {
      *altimeter_pressure *= 0.029530; // -> in Hg
    } else {
      *altimeter_pressure /= 100.0; // -> in Hg
    }

    temp = 0;
    ret = digital_input(device,card,53,&temp,1);
    if (ret == 1) {
      *altimeter_pressure = 29.92;
      printf("Altimeter Pressure: %f \n",*altimeter_pressure);
    }

    ret = digital_input(device,card,70,&temp,0);
    if (ret == 1) {
      if (temp == 0) printf("Altimeter Pressure Unit: mm Hg \n");
      if (temp == 1) printf("Altimeter Pressure Unit: hPa \n");
    }
    if ((*status_x737 == 2) || (*status_x737 == 3)) {
      if ((*altimeter_pressure_unit == 1) && (temp == 0)) {
	*altimeter_pressure_unit_dn = 1;
      }
      if ((*altimeter_pressure_unit == 0) && (temp == 1)) {
	*altimeter_pressure_unit_up = 1;
      }
    } else {
      *altimeter_pressure_unit = temp;
    }
    
  }

  if ((*status_x737 == 2) || (*status_x737 == 3)) {
    if (*minimum_mode != INT_MISS) {
      ret = digital_input(device,card,71,minimum_mode,0);
      if (ret == 1) {
	if (*minimum_mode == 0) printf("Minimums Mode: Radio \n");
	if (*minimum_mode == 1) printf("Minimums Mode: Baro \n");
      }
    }
    
    /*
      if ((*minimum_mode == 1) && (temp == 0)) {
      *minimum_mode_up = 1;
      }
      if ((*minimum_mode == 0) && (temp == 1)) {
      *minimum_mode_dn = 1;
      }
    */
  }


  if (*altimeter_minimum != FLT_MISS) {

    ret = mastercard_encoder(device, card, 68, altimeter_minimum, -10.0, 1.0,2);
    if (ret == 1) {
      printf("Radio Altimeter Minimum: %f \n",*altimeter_minimum);
    }

  }

  temp = 0;
  ret = digital_input(device,card,63,&temp,0);
  if (temp == 1) {
    *map_mode = 0;
  } else {
    temp = 0;
    ret = digital_input(device,card,64,&temp,0);
    if (temp == 1) {
      *map_mode = 1;
    } else {
      temp = 0;
      ret = digital_input(device,card,65,&temp,0);
      if (temp == 1) {
	*map_mode = 2;
      } else {
	*map_mode = 3;
      }
    }
  }
 
  temp = 0;
  ret = digital_input(device,card,56,&temp,0);
  if (temp == 1) {
    //    if (status_x737) {
    //      *navrangesel_x737 = 0.0;
    //    } else {
      *navrangesel = 0; // 5 nm
      //   }
  } else {
    temp = 0;
    ret = digital_input(device,card,57,&temp,0);
    if (temp == 1) {
      //      if (status_x737) {
      //	*navrangesel_x737 = 1.0;
      //      } else {
	*navrangesel = 1; // 10 nm
	//      }
    } else {
      temp = 0;
      ret = digital_input(device,card,58,&temp,0);
      if (temp == 1) {
	*navrangesel = 2; // 20 nm
      } else {
	temp = 0;
	ret = digital_input(device,card,59,&temp,0);
	if (temp == 1) {
	  *navrangesel = 3; // 40 nm
	} else {
	  temp = 0;
	  ret = digital_input(device,card,60,&temp,0);
	  if (temp == 1) {
	    *navrangesel = 4; // 80 nm
	  } else {
	    temp = 0;
	    ret = digital_input(device,card,61,&temp,0);
	    if (temp == 1) {
	      *navrangesel = 5; // 160 nm
	    } else {
	      temp = 0;
	      ret = digital_input(device,card,62,&temp,0);
	      if (temp == 1) {
		*navrangesel = 6; // 320 nm
	      } else {
		if (*navrangesel == 6) {
		  *navrangesel = 7; // 640 nm
		}
	      }
	    }
	  }
	}
      }
    }
  }
  
    
  // mastercard 2, output board (11-55)
  if (*avionics_on == 1) {
    if ((*status_x737 == 2) || (*status_x737 == 3)) {
      ret = digital_output(device,card,29,efis_wxr_led);
      ret = digital_output(device,card,30,efis_sta_led);
      ret = digital_output(device,card,31,efis_wpt_led);
      ret = digital_output(device,card,32,efis_apt_led);
      ret = digital_output(device,card,33,efis_data_led);
      ret = digital_output(device,card,34,efis_pos_led);
      ret = digital_output(device,card,35,efis_terr_led);
    } else {
      ret = digital_output(device,card,29,efis_wxr);
      ret = digital_output(device,card,30,efis_sta);
      ret = digital_output(device,card,31,efis_wpt);
      ret = digital_output(device,card,32,efis_apt);
      ret = digital_output(device,card,33,efis_data);
      ret = digital_output(device,card,34,efis_pos);
      ret = digital_output(device,card,35,efis_terr);
    }
  } else {
    ret = digital_output(device,card,29,&zero);
    ret = digital_output(device,card,30,&zero);
    ret = digital_output(device,card,31,&zero);
    ret = digital_output(device,card,32,&zero);
    ret = digital_output(device,card,33,&zero);
    ret = digital_output(device,card,34,&zero);
    ret = digital_output(device,card,35,&zero);
  }    

}

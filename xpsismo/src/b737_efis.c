/* This is the b737_efis.c code which connects to the SISMO EFIS Module
   call it with argument 0 for captain and 1 for copilot side

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
#include "b737_efis.h"


void b737_efis(int copilot)
{

  /* In the SISMO EFIS all switches are inverted (0=on; 1=off) */
  
  int ret;
  int temp;
  int temp2;
  int card = 0;
  //  int one = 1;
  //  int zero = 0;

  /* input offset between copilot and captain input pins */
  int offset;
  int offset1;
  if (copilot) {
    offset = 32;
    offset1 = 8;
  } else {
    offset = 0;
    offset1 = 0;
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
  int *efis_fpv;
  int *efis_mtrs;
  int *efis_rst;
  int *efis_std;
  int *efis_ctr;
  int *efis_tfc;
  if ((acf_type == 2) || (acf_type == 3)) {
    if (copilot) {
      //      efis_wxr = link_dataref_cmd_hold("laminar/B738/EFIS_control/fo/push_button/wxr_press");
      efis_sta = link_dataref_cmd_hold("laminar/B738/EFIS_control/fo/push_button/sta_press");
      efis_wpt = link_dataref_cmd_hold("laminar/B738/EFIS_control/fo/push_button/wpt_press");
      efis_apt = link_dataref_cmd_hold("laminar/B738/EFIS_control/fo/push_button/arpt_press");
      efis_data = link_dataref_cmd_hold("laminar/B738/EFIS_control/fo/push_button/data_press");
      efis_pos = link_dataref_cmd_hold("laminar/B738/EFIS_control/fo/push_button/pos_press");
      //      efis_terr = link_dataref_cmd_hold("laminar/B738/EFIS_control/fo/push_button/terr_press");
      efis_fpv = link_dataref_cmd_hold("laminar/B738/EFIS_control/fo/push_button/fpv_press");
      efis_mtrs = link_dataref_cmd_hold("laminar/B738/EFIS_control/fo/push_button/mtrs_press");
      efis_rst = link_dataref_cmd_hold("laminar/B738/EFIS_control/fo/push_button/rst_press");
      efis_std = link_dataref_cmd_hold("laminar/B738/EFIS_control/fo/push_button/std_press");
      efis_ctr = link_dataref_cmd_hold("laminar/B738/EFIS_control/fo/push_button/ctr_press");
      efis_tfc = link_dataref_cmd_hold("laminar/B738/EFIS_control/fo/push_button/tfc_press");
    } else {
      //      efis_wxr = link_dataref_cmd_hold("laminar/B738/EFIS_control/capt/push_button/wxr_press");
      efis_sta = link_dataref_cmd_hold("laminar/B738/EFIS_control/capt/push_button/sta_press");
      efis_wpt = link_dataref_cmd_hold("laminar/B738/EFIS_control/capt/push_button/wpt_press");
      efis_apt = link_dataref_cmd_hold("laminar/B738/EFIS_control/capt/push_button/arpt_press");
      efis_data = link_dataref_cmd_hold("laminar/B738/EFIS_control/capt/push_button/data_press");
      efis_pos = link_dataref_cmd_hold("laminar/B738/EFIS_control/capt/push_button/pos_press");
      //      efis_terr = link_dataref_cmd_hold("laminar/B738/EFIS_control/capt/push_button/terr_press");
      efis_fpv = link_dataref_cmd_hold("laminar/B738/EFIS_control/capt/push_button/fpv_press");
      efis_mtrs = link_dataref_cmd_hold("laminar/B738/EFIS_control/capt/push_button/mtrs_press");
      efis_rst = link_dataref_cmd_hold("laminar/B738/EFIS_control/capt/push_button/rst_press");
      efis_std = link_dataref_cmd_hold("laminar/B738/EFIS_control/capt/push_button/std_press");
      efis_ctr = link_dataref_cmd_hold("laminar/B738/EFIS_control/capt/push_button/ctr_press");
      efis_tfc = link_dataref_cmd_hold("laminar/B738/EFIS_control/capt/push_button/tfc_press");
    }
  } else if (acf_type == 1) {
    //    efis_wxr = link_dataref_int("x737/cockpit/EFISCTRL_0/WXR_on");
    efis_sta = link_dataref_int("x737/cockpit/EFISCTRL_0/STA_on");
    efis_wpt = link_dataref_int("x737/cockpit/EFISCTRL_0/WPT_on");
    efis_apt = link_dataref_int("x737/cockpit/EFISCTRL_0/ARPT_on");
    efis_data = link_dataref_int("x737/cockpit/EFISCTRL_0/DATA_on");
    efis_pos = link_dataref_int("x737/cockpit/EFISCTRL_0/POS_on");
    //    efis_terr = link_dataref_int("x737/cockpit/EFISCTRL_0/TERR_on");
    efis_fpv = link_dataref_int("xpserver/EFIS_FPV");
    efis_mtrs = link_dataref_int("xpserver/EFIS_MTRS");
    efis_rst = link_dataref_int("xpserver/EFIS_RST");
    efis_std = link_dataref_int("xpserver/EFIS_STD");
    efis_ctr = link_dataref_int("xpserver/EFIS_CTR");
    efis_tfc = link_dataref_int("xpserver/EFIS_TFC");
  } else {
    if (copilot) {
      //    efis_wxr = link_dataref_int("sim/cockpit/switches/EFIFS_shows_weather");
      efis_sta = link_dataref_int("sim/cockpit2/EFIS/EFIS_vor_on_copilot");
      efis_wpt = link_dataref_int("sim/cockpit2/EFIS/EFIS_fix_on_copilot");
      efis_apt = link_dataref_int("sim/cockpit2/EFIS/EFIS_airport_on_copilot");
      efis_data = link_dataref_int("xpserver/EFIS_DATA");
      efis_pos = link_dataref_int("sim/cockpit2/EFIS/EFIS_tcas_on_copilot");
      //efis_terr = link_dataref_int("xpser");
    } else {
      //    efis_wxr = link_dataref_int("sim/cockpit/switches/EFIFS_shows_weather");
      efis_sta = link_dataref_int("sim/cockpit2/EFIS/EFIS_vor_on");
      efis_wpt = link_dataref_int("sim/cockpit2/EFIS/EFIS_fix_on");
      efis_apt = link_dataref_int("sim/cockpit2/EFIS/EFIS_airport_on");
      efis_data = link_dataref_int("xpserver/EFIS_DATA");
      efis_pos = link_dataref_int("sim/cockpit2/EFIS/EFIS_tcas_on");
      //efis_terr = link_dataref_int("xpser");
    }
    efis_fpv = link_dataref_int("xpserver/EFIS_FPV");
    efis_mtrs = link_dataref_int("xpserver/EFIS_MTRS");
    efis_rst = link_dataref_int("xpserver/EFIS_RST");
    efis_std = link_dataref_int("xpserver/EFIS_STD");
    efis_ctr = link_dataref_int("xpserver/EFIS_CTR");
    efis_tfc = link_dataref_int("xpserver/EFIS_TFC");
   }

  if (copilot) {
    efis_wxr = link_dataref_int("xpserver/EFIS_fo_wxr");
    efis_terr = link_dataref_int("xpserver/EFIS_fo_terr");
  } else {
    efis_wxr = link_dataref_int("xpserver/EFIS_capt_wxr");
    efis_terr = link_dataref_int("xpserver/EFIS_capt_terr");
  }


  /* INPUTS */
  
  /* NAV MAP FEATURE BUTTONS */
  if (*efis_wxr == INT_MISS) *efis_wxr = 0;
  ret = digital_input(card,40+offset,efis_wxr,1);  // WXR --> toggle switch
  if (ret == 1) printf("WXR: %i \n",*efis_wxr);
  if ((acf_type == 2) || (acf_type == 3)) {
    ret = digital_input(card,41+offset,efis_sta,0);  // STA
    if (ret == 1) printf("STA: %i \n",*efis_sta);
    ret = digital_input(card,42+offset,efis_wpt,0);  // WPT
    ret = digital_input(card,43+offset,efis_apt,0);  // ARPT
    ret = digital_input(card,44+offset,efis_data,0);  // DATA
    ret = digital_input(card,45+offset,efis_pos,0);  // POS
    ret = digital_input(card,52+offset,efis_fpv,0);  // FPV
    ret = digital_input(card,53+offset,efis_mtrs,0);  // MTRS
    ret = digital_input(card,34+offset,efis_rst,0);  // RST
    ret = digital_input(card,39+offset,efis_std,0);  // STD
    ret = digital_input(card,104+offset1,efis_ctr,0);  // CTR
    ret = digital_input(card,105+offset1,efis_tfc,0);  // TFC
  } else {
    if (*efis_sta == INT_MISS) *efis_sta = 0;
    if (*efis_wpt == INT_MISS) *efis_wpt = 0;
    if (*efis_apt == INT_MISS) *efis_apt = 0;
    if (*efis_data == INT_MISS) *efis_data = 0;
    if (*efis_pos == INT_MISS) *efis_pos = 0;
    ret = digital_input(card,41+offset,efis_sta,1);  // STA
    if (ret == 1) printf("STA: %i \n",*efis_sta);
    ret = digital_input(card,42+offset,efis_wpt,1);  // WPT
    ret = digital_input(card,43+offset,efis_apt,1);  // ARPT
    ret = digital_input(card,44+offset,efis_data,1);  // DATA
    ret = digital_input(card,45+offset,efis_pos,1);  // POS
    ret = digital_input(card,52+offset,efis_fpv,1);  // FPV
    ret = digital_input(card,53+offset,efis_mtrs,1);  // MTRS
    ret = digital_input(card,34+offset,efis_rst,1);  // RST
    ret = digital_input(card,39+offset,efis_std,1);  // STD
    ret = digital_input(card,104+offset1,efis_ctr,1);  // CTR
    ret = digital_input(card,105+offset1,efis_tfc,1);  // TFC
  }
    
    
  /* Special Handling for TERR since we cycle through TERR modes */
  temp = 0;
  ret = digital_input(card,46+offset,&temp,1);  // TERR --> toggle switch
  if (*efis_terr == INT_MISS) *efis_terr = 0;
  if ((ret==1) && (temp==1)) {
    if (*efis_terr == 0) {
      *efis_terr = 1;
    } else if (*efis_terr == 1) {
      *efis_terr = 2;
    } else {
      *efis_terr = 0;
    }
    printf("EFIS TERR: %i \n",*efis_terr);
  }

  /* EFIS VOR1/OFF/ADF1 selector */
  ret = digital_input(card,32+offset,&temp,0); // VOR1
  ret = digital_input(card,33+offset,&temp2,0); // ADF1
  if ((acf_type == 2) || (acf_type == 3)) {
    if (*efis1_sel != INT_MISS) {
      if (*efis1_sel < (temp + (1-temp2) - 1)) {
	*efis1_sel_up = 1;
      }
      if (*efis1_sel > (temp + (1-temp2) - 1)) {
	*efis1_sel_dn = 1;
      }     
    }
  } else {
    *efis1_sel = temp + (1-temp2);
  }

  /* EFIS VOR2/OFF/ADF2 selector */
  ret = digital_input(card,54+offset,&temp,0); // VOR2
  ret = digital_input(card,55+offset,&temp2,0); // ADF2
  if ((acf_type == 2) || (acf_type == 3)) {
    if (*efis2_sel != INT_MISS) {
      if (*efis2_sel < (temp + (1-temp2) - 1)) {
	*efis2_sel_up = 1;
      }
      if (*efis2_sel > (temp + (1-temp2) - 1)) {
	*efis2_sel_dn = 1;
      }     
    }
  } else {
    *efis2_sel = temp + (1-temp2);
  }

  /* BARO IN/HPA */
  if (*altimeter_pressure_unit != INT_MISS) {
    if ((acf_type == 2) || (acf_type == 3)) {
      ret = digital_input(card,108+offset1,&temp,0);
      if ((*altimeter_pressure_unit == 1) && (temp == 0)) {
	printf("Altimeter Pressure Unit: in Hg \n");
	*altimeter_pressure_unit_dn = 1;
      }
      if ((*altimeter_pressure_unit == 0) && (temp == 1)) {
	*altimeter_pressure_unit_up = 1;
	printf("Altimeter Pressure Unit: hPa \n");
      }
    } else {
      ret = digital_input(card,108+offset1,altimeter_pressure_unit,0);
      if (ret == 1) {
	if (*altimeter_pressure_unit == 0) printf("Altimeter Pressure Unit: in Hg \n");
	if (*altimeter_pressure_unit == 1) printf("Altimeter Pressure Unit: hPa \n");
      }
    }
  }

  /* MINS RADIO/BARO */
  if ((acf_type == 2) || (acf_type == 3)) {
    if (*minimum_mode != INT_MISS) {
      ret = digital_input(card,106+offset1,&temp,0); // not inverse
      if ((*minimum_mode == 1) && (temp == 0)) {
	*minimum_mode_up = 1;
	printf("Minimums Mode: Radio \n");
      }
      if ((*minimum_mode == 0) && (temp == 1)) {
	*minimum_mode_dn = 1;
	printf("Minimums Mode: Baro \n");
      }
    }
  }

  /* MAP MODE SELECTOR */
  ret = digital_input(card,51+offset,&temp,0); // APP
  if (temp == 1) {
    *map_mode = 0;
  } else {
    ret = digital_input(card,50+offset,&temp,0); // VOR
    if (temp == 1) {
      *map_mode = 1;
    } else {
      ret = digital_input(card,49+offset,&temp,0); // MAP
      if (temp == 1) {
	*map_mode = 2;
      } else {
	ret = digital_input(card,48+offset,&temp,0); // PLN
	if (temp == 1) {
	  *map_mode = 3;
	}
      }
    }
  }

  /* MAP RANGE SELECTOR */
  ret = digital_input(card,63+offset,&temp,0);
  if (temp == 1) {
      *navrangesel = 0; // 5 nm
  } else {
    ret = digital_input(card,62+offset,&temp,0);
    if (temp == 1) {
	*navrangesel = 1; // 10 nm
    } else {
      ret = digital_input(card,61+offset,&temp,0);
      if (temp == 1) {
	*navrangesel = 2; // 20 nm
      } else {
	ret = digital_input(card,60+offset,&temp,0);
	if (temp == 1) {
	  *navrangesel = 3; // 40 nm
	} else {
	  ret = digital_input(card,59+offset,&temp,0);
	  if (temp == 1) {
	    *navrangesel = 4; // 80 nm
	  } else {
	    ret = digital_input(card,58+offset,&temp,0);
	    if (temp == 1) {
	      *navrangesel = 5; // 160 nm
	    } else {
	      ret = digital_input(card,57+offset,&temp,0);
	      if (temp == 1) {
		*navrangesel = 6; // 320 nm
	      } else {
		ret = digital_input(card,56+offset,&temp,0);
		if (temp == 1) {
		  *navrangesel = 7; // 640 nm
		}
	      }
	    }
	  }
	}
      }
    }
  }

  /* ENCODERS */

  /* Altimeter setting */
  if (*altimeter_pressure != FLT_MISS) {

    if (*altimeter_pressure_unit == 1) {
      *altimeter_pressure /= 0.029530; // in Hg -> 10 * hPa
    } else {
      *altimeter_pressure *= 100.0; // 100 * in Hg
    }
    
    ret = encoder_inputf(card, 37+offset, 38+offset, altimeter_pressure, 1.0, 1);
    if (ret == 1) {
      printf("Altimeter Pressure: %f \n",*altimeter_pressure);
    }
    if (*altimeter_pressure_unit == 1) {
      *altimeter_pressure *= 0.029530; // -> in Hg
    } else {
      *altimeter_pressure /= 100.0; // -> in Hg
    }
  }

  /* Minimums setting */
  if (*altimeter_minimum != FLT_MISS) {
    ret = encoder_inputf(card, 35+offset,36+offset, altimeter_minimum, 10.0, 1);
    if (*altimeter_minimum < 0.0) *altimeter_minimum = 0.0;
    if (ret == 1) {
      printf("Radio Altimeter Minimum: %f \n",*altimeter_minimum);
    }
  }

  /* OUTPUTS */

  /* DISPLAYS */
  
}

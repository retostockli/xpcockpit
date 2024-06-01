/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Copyright (C) 2001-2021 by:
  Original author:
  Damion Shelton
  Contributors (in alphabetical order):
  Reto Stockli
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  =========================================================================*/

#include <stdio.h>
#include <math.h>
#include "ogcGaugeComponent.h"
#include "ogcB737PFD.h"
#include "ogcB737PFDBackground.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC
{

  B737PFDBackground::B737PFDBackground()
  {
    printf("B737PFDBackground constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 200; // make the clip region larger to handle speed bug
    m_PhysicalSize.y = 220;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  B737PFDBackground::~B737PFDBackground()
  {

  }

  void B737PFDBackground::Render()
  {
    char buffer[30]; // temporary buffer for text
    memset(buffer,0,sizeof(buffer));
    float fontHeight = 5;
    float fontWidth = 5;
    float lineWidth = 3.0;
    float rawGlideslope;
    float rawLocalizer;

    // Call base class to setup viewport and projection
    GaugeComponent::Render();

    // Save matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // retrieve data
    bool is_captain = (this->GetArg() == 0);
    bool is_copilot = (this->GetArg() == 1);
    
    int acf_type = m_pDataSource->GetAcfType();

    float *ap_speed;
    int *ap_speed_is_mach;
    float *ap_altitude;

    if ((acf_type == 2) || (acf_type == 3)) {
      ap_speed = link_dataref_flt("laminar/B738/autopilot/mcp_speed_dial_kts_mach",0);
      ap_speed_is_mach = link_dataref_int("sim/cockpit/autopilot/airspeed_is_mach");  
      ap_altitude = link_dataref_flt("laminar/B738/autopilot/mcp_alt_dial",0); 
    } else if (acf_type == 1) {
      ap_speed = link_dataref_flt("x737/systems/athr/MCPSPD_spd",0);
      ap_speed_is_mach = link_dataref_int("x737/systems/athr/MCPSPD_ismach");  
      ap_altitude = link_dataref_flt("x737/systems/afds/ALTHLD_baroalt",0); 
    } else {
      ap_speed = link_dataref_flt("sim/cockpit/autopilot/airspeed",0);
      ap_speed_is_mach = link_dataref_int("sim/cockpit/autopilot/airspeed_is_mach");  
      ap_altitude = link_dataref_flt("sim/cockpit/autopilot/altitude",0); 
    }
    float *altimeter_pressure;
    if (is_captain) {
      altimeter_pressure = link_dataref_flt("sim/cockpit/misc/barometer_setting",-2);
    } else {
      altimeter_pressure = link_dataref_flt("sim/cockpit/misc/barometer_setting2",-2);
    }

    //  int *altimeter_pressure_unit = link_dataref_int("x737/systems/units/baroPressUnit");   
    int *altimeter_pressure_unit;
    int *altimeter_set_std;
    float *altimeter_pressure_preset;
    int *altimeter_preset_show;
    int *no_vspeed;
    int *alt_is_meters;
    if ((acf_type == 2) || (acf_type == 3)) {
      if (is_captain) {
	alt_is_meters = link_dataref_int("laminar/B738/PFD/capt/alt_mode_is_meters");
	altimeter_pressure_unit = link_dataref_int("laminar/B738/EFIS_control/capt/baro_in_hpa");
	altimeter_set_std = link_dataref_int("laminar/B738/EFIS/baro_set_std_pilot");
	altimeter_preset_show = link_dataref_int("laminar/B738/EFIS/baro_sel_pilot_show");
	altimeter_pressure_preset = link_dataref_flt("laminar/B738/EFIS/baro_sel_in_hg_pilot",-4);
      } else {
	alt_is_meters = link_dataref_int("laminar/B738/PFD/fo/alt_mode_is_meters");
	altimeter_pressure_unit = link_dataref_int("laminar/B738/EFIS_control/fo/baro_in_hpa");
	altimeter_set_std = link_dataref_int("laminar/B738/EFIS/baro_set_std_copilot");
	altimeter_preset_show = link_dataref_int("laminar/B738/EFIS/baro_sel_copilot_show");
	altimeter_pressure_preset = link_dataref_flt("laminar/B738/EFIS/baro_sel_in_hg_copilot",-4);
      }
      no_vspeed = link_dataref_int("laminar/B738/pfd/no_vspd");
    } else {
      altimeter_pressure_unit = link_dataref_int("xpserver/barometer_unit");
    }

    // angle of attack (degrees)
    float *aoa = link_dataref_flt("sim/flightmodel/position/alpha",-1);

    //float *ap_athr_armed_rec;   
    //float *ap_athr_armed;   
    //float *ap_n1_mode;   
    //float *ap_mcpspd_mode;   
    //float *ap_fmcspd_mode;

    float *ap_spd_mode;
    //float *ap_spd_mode_arm;
    float *ap_spd_mode_rec;
    float *ap_hdg_mode;   
    float *ap_hdg_mode_arm;   
    float *ap_hdg_mode_rec;   
    float *ap_alt_mode;   
    float *ap_alt_mode_arm;   
    float *ap_alt_mode_rec;   

    //float *ap_flare_mode;   
    //float *ap_rollout_mode;   

    //float *ap_retard_mode;   
    //float *ap_thrhld_mode;   
    //float *ap_lnav_armed;   
    //float *ap_vorloc_armed;   
    //float *ap_pitchspd_mode;   
    //float *ap_vvi_mode;
    //float *ap_vs_mode;   
    //float *ap_gs_armed;   
    //float *ap_gs_mode;   
    //float *ap_toga_mode;   
    //float *ap_lnav_mode;   
    //float *ap_vorloc_mode;   
    float *ap_single_ch;
    float *ap_autoland;
    if ((acf_type == 2) || (acf_type == 3)) {
      //ap_athr_armed = link_dataref_flt("laminar/B738/autopilot/autothrottle_status",0);   
      //ap_athr_armed_rec = link_dataref_flt("laminar/B738/autopilot/rec_thr2_modes",0);   

      if (is_captain) {
	ap_spd_mode     = link_dataref_flt_arr("laminar/B738/autopilot/pfd_spd_mode",2,0,0);   
	// ap_spd_mode_arm = link_dataref_flt_arr("laminar/B738/autopilot/pfd_spd_mode_arm",2,0,0);   
	ap_spd_mode_rec = link_dataref_flt_arr("laminar/B738/autopilot/rec_thr_modes",2,0,0);   
	ap_hdg_mode     = link_dataref_flt_arr("laminar/B738/autopilot/pfd_hdg_mode",2,0,0);   
	ap_hdg_mode_arm = link_dataref_flt_arr("laminar/B738/autopilot/pfd_hdg_mode_arm",2,0,0);   
	ap_hdg_mode_rec = link_dataref_flt_arr("laminar/B738/autopilot/rec_hdg_modes",2,0,0);   
	ap_alt_mode     = link_dataref_flt_arr("laminar/B738/autopilot/pfd_alt_mode",2,0,0);   
	ap_alt_mode_arm = link_dataref_flt_arr("laminar/B738/autopilot/pfd_alt_mode_arm",2,0,0);   
	ap_alt_mode_rec = link_dataref_flt_arr("laminar/B738/autopilot/rec_alt_modes",2,0,0);   
      } else {
	ap_spd_mode     = link_dataref_flt_arr("laminar/B738/autopilot/pfd_spd_mode",2,1,0);   
	// ap_spd_mode_arm = link_dataref_flt_arr("laminar/B738/autopilot/pfd_spd_mode_arm",2,1,0);   
	ap_spd_mode_rec = link_dataref_flt_arr("laminar/B738/autopilot/rec_thr_modes",2,1,0);   
	ap_hdg_mode     = link_dataref_flt_arr("laminar/B738/autopilot/pfd_hdg_mode",2,1,0);   
	ap_hdg_mode_arm = link_dataref_flt_arr("laminar/B738/autopilot/pfd_hdg_mode_arm",2,1,0);   
	ap_hdg_mode_rec = link_dataref_flt_arr("laminar/B738/autopilot/rec_hdg_modes",2,1,0);   
	ap_alt_mode     = link_dataref_flt_arr("laminar/B738/autopilot/pfd_alt_mode",2,1,0);   
	ap_alt_mode_arm = link_dataref_flt_arr("laminar/B738/autopilot/pfd_alt_mode_arm",2,1,0);   
	ap_alt_mode_rec = link_dataref_flt_arr("laminar/B738/autopilot/rec_alt_modes",2,1,0);   
      }
	  
	
      //ap_flare_mode = link_dataref_flt("laminar/B738/autopilot/flare_status",0);   
      //ap_rollout_mode = link_dataref_flt("laminar/B738/autopilot/rollout_status",0);   

      //ap_retard_mode = link_dataref_flt("laminar/B738/autopilot/retard_status",0);   
      //ap_thrhld_mode = link_dataref_flt("laminar/B738/autopilot/thr_hld_pfd",0);   
      //ap_lnav_armed = link_dataref_flt("laminar/B738/autopilot/lnav_status",0);   
      //ap_vorloc_armed = link_dataref_flt("laminar/B738/autopilot/vorloc_status",0);   
      //ap_pitchspd_mode = link_dataref_flt("xpserver/PFD_PITCHSPD_mode_on",0);   
      //ap_vvi_mode = link_dataref_flt("laminar/B738/autopilot/vvi_status_pfd",0);
      //ap_vs_mode = link_dataref_flt("laminar/B738/autopilot/vs_status",0);   
      //ap_gs_armed = link_dataref_flt("laminar/B738/autopilot/gs_armed_pfd",0);   
      //ap_gs_mode = link_dataref_flt("xpserver/PFD_GS_mode_on",0);   
      //ap_toga_mode = link_dataref_flt("xpserver/PFD_TOGA_mode_on",0);   
      //ap_lnav_mode = link_dataref_flt("laminar/B738/autopilot/pfd_vorloc_lnav",0);   
      //ap_vorloc_mode = link_dataref_flt("laminar/B738/autopilot/pfd_vorloc_lnav",0);   
      ap_single_ch = link_dataref_flt("laminar/B738/autopilot/single_ch_status",0);
      ap_autoland = link_dataref_flt("laminar/B738/autopilot/autoland_status",0);
    } else if (acf_type == 1) {
      //ap_athr_armed = link_dataref_int("x737/systems/PFD/PFD_ATHR_ARMED_on");   
      //ap_athr_armed_rec = link_dataref_int("xpserver/rec_thr2_modes",0);   
      //ap_n1_mode = link_dataref_int("x737/systems/PFD/PFD_N1_mode_on",0);   
      //ap_mcpspd_mode = link_dataref_int("x737/systems/PFD/PFD_MCPSPD_mode_on",0);   
      //ap_fmcspd_mode = link_dataref_int("x737/systems/PFD/PFD_FMCSPD_mode_on",0);   
      //ap_retard_mode = link_dataref_int("x737/systems/PFD/PFD_RETARD_mode_on",0);   
      //ap_thrhld_mode = link_dataref_int("x737/systems/PFD/PFD_THRHLD_mode_on",0);   
      //ap_lnav_armed = link_dataref_int("x737/systems/PFD/PFD_LNAVARMED_mode_on",0);   
      //ap_vorloc_armed = link_dataref_int("x737/systems/PFD/PFD_VORLOCARMED_mode_on",0);   
      //ap_pitchspd_mode = link_dataref_int("x737/systems/PFD/PFD_PITCHSPD_mode_on",0);   
      //ap_vvi_mode = link_dataref_int("x737/systems/PFD/PFD_VSARMED_mode_on",0);
      //ap_vs_mode = link_dataref_int("x737/systems/PFD/PFD_VS_mode_on",0);   
      //ap_gs_armed = link_dataref_int("x737/systems/PFD/PFD_GSARMED_mode_on",0);   
      //ap_gs_mode = link_dataref_int("x737/systems/PFD/PFD_GS_mode_on",0);   
      //ap_flare_mode = link_dataref_int("x737/systems/PFD/PFD_FLARE_mode_on",0);   
      //ap_toga_mode = link_dataref_int("x737/systems/PFD/PFD_TOGA_mode_on",0);   
      //ap_lnav_mode = link_dataref_int("x737/systems/PFD/PFD_LNAV_mode_on",0);   
      //ap_vorloc_mode = link_dataref_int("x737/systems/PFD/PFD_VORLOC_mode_on",0);   
      ap_single_ch = link_dataref_flt("x737/systems/afds/SINGLE_CH_warning",0);
      ap_autoland = link_dataref_flt("xpserver/autoland_status",0);
    } else {
      //ap_athr_armed = link_dataref_flt("xpserver/PFD_ATHR_ARMED_on");   
      //ap_athr_armed_rec = link_dataref_flt("xpserver/rec_thr2_modes");   
      //ap_thrhld_mode = link_dataref_flt("xpserver/PFD_THRHLD_mode_on");   
      //ap_lnav_armed = link_dataref_flt("xpserver/PFD_LNAVARMED_mode_on");   
      //ap_vorloc_armed = link_dataref_flt("xpserver/PFD_VORLOCARMED_mode_on");   
      //ap_pitchspd_mode = link_dataref_flt("xpserver/PFD_PITCHSPD_mode_on");   
      //ap_vvi_mode = link_dataref_flt("xpserver/PFD_VSARMED_mode_on");
      //ap_vs_mode = link_dataref_flt("xpserver/PFD_VS_mode_on");   
      //ap_gs_armed = link_dataref_flt("xpserver/PFD_GSARMED_mode_on");   
      //ap_gs_mode = link_dataref_flt("xpserver/PFD_GS_mode_on");   
      //ap_flare_mode = link_dataref_flt("xpserver/PFD_FLARE_mode_on");   
      //ap_toga_mode = link_dataref_flt("xpserver/PFD_TOGA_mode_on");   
      //ap_lnav_mode = link_dataref_flt("xpserver/PFD_LNAV_mode_on");   
      //ap_hdg_mode = link_dataref_flt("xpserver/PFD_HDG_mode_on");   
      //ap_hdg_mode_rec = link_dataref_flt("xpserver/PFD_HDG_mode_on");   
      //ap_vorloc_mode = link_dataref_flt("xpserver/PFD_VORLOC_mode_on");   
      ap_single_ch = link_dataref_flt("xpserver/SINGLE_CH_warning",0);
      ap_autoland = link_dataref_flt("xpserver/autoland_status",0);
    }
  
    //  int *ap_vnav_armed = link_dataref_int("x737/systems/afds/VNAV");   
    //  int *ap_lvlchg_armed = link_dataref_int("x737/systems/afds/LVLCHG");   

    float *fd_a_status;
    float *fd_b_status;
    float *cmd_a_status;
    float *cmd_b_status;
    float *cmd_rec;
    if ((acf_type == 2) || (acf_type == 3)) {
      fd_a_status = link_dataref_flt("laminar/B738/autopilot/master_capt_status",0);   
      fd_b_status = link_dataref_flt("laminar/B738/autopilot/master_fo_status",0);
      cmd_a_status = link_dataref_flt("laminar/B738/autopilot/cmd_a_status",0);
      cmd_b_status = link_dataref_flt("laminar/B738/autopilot/cmd_b_status",0);
      if (is_captain) {
	cmd_rec = link_dataref_flt("laminar/B738/autopilot/rec_cmd_modes",0);
      } else {
	cmd_rec = link_dataref_flt("laminar/B738/autopilot/rec_cmd_modes_fo",0);
      }
    } else if (acf_type == 1) {
      fd_a_status = link_dataref_flt("x737/systems/afds/fdA_status",0);   
      fd_b_status = link_dataref_flt("x737/systems/afds/fdB_status",0);   
      cmd_a_status = link_dataref_flt("x737/systems/afds/CMD_A",0);
      cmd_b_status = link_dataref_flt("x737/systems/afds/CMD_B",0);
      cmd_rec = link_dataref_flt("xpserver/CMD_REC",0);
    } else {
      fd_a_status = link_dataref_flt("sim/cockpit/autopilot/autopilot_mode",0);   
      fd_b_status = link_dataref_flt("sim/cockpit/autopilot/autopilot_mode",0);
      cmd_a_status = link_dataref_flt("xpserver/CMD_A",0);
      cmd_b_status = link_dataref_flt("xpserver/CMD_B",0);
      cmd_rec = link_dataref_flt("xpserver/CMD_REC",0);
    }
    
    int *nav1_CDI = link_dataref_int("sim/cockpit/radios/nav1_CDI");   
    int *nav2_CDI = link_dataref_int("sim/cockpit/radios/nav2_CDI"); 
    int *nav1_horizontal = link_dataref_int("sim/cockpit2/radios/indicators/nav1_display_horizontal");
    int *nav2_horizontal = link_dataref_int("sim/cockpit2/radios/indicators/nav2_display_horizontal");
    int *nav1_vertical = link_dataref_int("sim/cockpit2/radios/indicators/nav1_display_vertical");
    int *nav2_vertical = link_dataref_int("sim/cockpit2/radios/indicators/nav2_display_vertical");
    float *nav1_hdef = link_dataref_flt("sim/cockpit/radios/nav1_hdef_dot",-2);   
    float *nav2_hdef = link_dataref_flt("sim/cockpit/radios/nav2_hdef_dot",-2);   
    float *nav1_vdef = link_dataref_flt("sim/cockpit/radios/nav1_vdef_dot",-2);   
    float *nav2_vdef = link_dataref_flt("sim/cockpit/radios/nav2_vdef_dot",-2);   

    int *gpws = link_dataref_int("sim/cockpit2/annunciators/GPWS");
    int *windshear = link_dataref_int("sim/operation/failures/rel_wind_shear");

    float *altimeter_minimum;
    int *minimum_mode;
    if ((acf_type == 2) || (acf_type == 3)) {
      if (is_captain) {
	altimeter_minimum = link_dataref_flt("laminar/B738/pfd/dh_pilot",0);
	minimum_mode = link_dataref_int("laminar/B738/EFIS_control/cpt/minimums");
      } else {
	altimeter_minimum = link_dataref_flt("laminar/B738/pfd/dh_copilot",0);
	minimum_mode = link_dataref_int("laminar/B738/EFIS_control/fo/minimums");
      }
    } else {
      altimeter_minimum = link_dataref_flt("sim/cockpit/misc/radio_altimeter_minimum",0);
    }

    float *ias_mach;
    if (is_captain) {
      ias_mach = link_dataref_flt("sim/cockpit2/gauges/indicators/mach_pilot",-3);
    } else {
      ias_mach = link_dataref_flt("sim/cockpit2/gauges/indicators/mach_copilot",-3);
    }
    
    if (*ias_mach != FLT_MISS) {
      // 1. Plot Mach number below the speed tape
    
      glColor3ub(COLOR_WHITE);
      glLineWidth(lineWidth);
    
      m_pFontManager->SetSize(m_Font, fontWidth, fontHeight);
    
      snprintf(buffer, sizeof(buffer), "%5.3f", *ias_mach);
      m_pFontManager->Print(10,25+0, &buffer[0], m_Font);
    }

    // 2. Plot Autopilot dialed speed above the speed tape

    glColor3ub(COLOR_VIOLET);
    glLineWidth(lineWidth);

    m_pFontManager->SetSize(m_Font, 1.15*fontWidth, 1.35*fontHeight);

    if (*ap_speed != FLT_MISS) {
      if (*ap_speed_is_mach == 1) {
	snprintf(buffer, sizeof(buffer), "%03.2f", *ap_speed);
	m_pFontManager->Print(15,170+3, &buffer[0], m_Font);
      } else {
	snprintf(buffer, sizeof(buffer), "%4.0f", *ap_speed);
	m_pFontManager->Print(14,170+3, &buffer[0], m_Font);
      }
    }

    // 3. Plot dialed barometric pressure in In Hg below the altitude tape


    if ((acf_type == 2) || (acf_type == 3)) {
      if (*altimeter_pressure != FLT_MISS) {
	m_pFontManager->SetSize(m_Font, fontWidth, fontHeight);
	glColor3ub(COLOR_GREEN);
	glLineWidth(lineWidth);
	if (*altimeter_set_std == 1) {
	  strcpy(buffer, "STD");
	  m_pFontManager->Print(153,25+0, &buffer[0], m_Font);
	} else {
	  if (*altimeter_pressure_unit == 1) {
	    snprintf(buffer, sizeof(buffer), "%4.0f hPa", *altimeter_pressure/0.029530);
	    m_pFontManager->Print(150,25+0, &buffer[0], m_Font);
	  } else {
	    snprintf(buffer, sizeof(buffer), "%4.2f In", *altimeter_pressure);
	    m_pFontManager->Print(150,25+0, &buffer[0], m_Font);
	  }
	}
      }
      if (*altimeter_pressure_preset != FLT_MISS) {
	m_pFontManager->SetSize(m_Font, 0.7*fontWidth, 0.7*fontHeight);
	glColor3ub(COLOR_WHITE);
	glLineWidth(lineWidth);
	if (*altimeter_preset_show == 1) {
	  if (*altimeter_pressure_unit == 1) {
	    snprintf(buffer, sizeof(buffer), "%4.0f hPa", *altimeter_pressure_preset/0.029530);
	    m_pFontManager->Print(155,18+0, &buffer[0], m_Font);
	  } else {
	    snprintf(buffer, sizeof(buffer), "%4.2f In", *altimeter_pressure_preset);
	    m_pFontManager->Print(155,18+0, &buffer[0], m_Font);
	  }
	}
      }
    } else {
      if (*altimeter_pressure != FLT_MISS) {
	m_pFontManager->SetSize(m_Font, fontWidth, fontHeight);
	glColor3ub(COLOR_GREEN);
	glLineWidth(lineWidth);
	if (roundf(*altimeter_pressure*100) == 2992) {
	  strcpy(buffer, "STD");
	  m_pFontManager->Print(153,25+0, &buffer[0], m_Font);
	} else {
	  if (*altimeter_pressure_unit == 1) {
	    snprintf(buffer, sizeof(buffer), "%4.0f hPa", *altimeter_pressure/0.029530);
	    m_pFontManager->Print(150,25+0, &buffer[0], m_Font);
	  } else {
	    snprintf(buffer, sizeof(buffer), "%4.2f In", *altimeter_pressure);
	    m_pFontManager->Print(150,25+0, &buffer[0], m_Font);
	  }
	}
      }
    }

    // 4. Plot dialed Autopilot altitude in feet above the altitude tape

    glColor3ub(COLOR_VIOLET);
    glLineWidth(lineWidth);

    if (*ap_altitude != FLT_MISS) {
      int thousands = (int) *ap_altitude / 1000.0;
      int hundreds = (int) *ap_altitude - thousands*1000.0;
      snprintf(buffer, sizeof(buffer), "%2d", thousands);
      m_pFontManager->SetSize(m_Font, 1.15*fontWidth, 1.35*fontHeight);
      if (thousands >= 10) {
	m_pFontManager->Print(151.5,170+3, &buffer[0], m_Font);
	snprintf(buffer, sizeof(buffer), "%03d", hundreds);
	m_pFontManager->SetSize(m_Font, 1.00*fontWidth, 1.15*fontHeight);
	m_pFontManager->Print(161.5,170+3, &buffer[0], m_Font);
      } else if (thousands >= 1) {
	m_pFontManager->Print(154.0,170+3, &buffer[0], m_Font);
	snprintf(buffer, sizeof(buffer), "%03d", hundreds);
	m_pFontManager->SetSize(m_Font, 1.00*fontWidth, 1.15*fontHeight);
	m_pFontManager->Print(161.5,170+3, &buffer[0], m_Font);
      } else {
	if (hundreds > 0) {
	  snprintf(buffer, sizeof(buffer), "%03d", hundreds);
	  m_pFontManager->SetSize(m_Font, 1.00*fontWidth, 1.15*fontHeight);
	  m_pFontManager->Print(161.5,170+3, &buffer[0], m_Font);
	} else {
	  snprintf(buffer, sizeof(buffer), "%d", hundreds);
	  m_pFontManager->SetSize(m_Font, 1.00*fontWidth, 1.15*fontHeight);
	  m_pFontManager->Print(170.0,170+3, &buffer[0], m_Font);
	}
      }
      if ((acf_type == 2) || (acf_type == 3)) {
	if (*alt_is_meters == 1) {
	  int ap_altitude_meters = (int) *ap_altitude * 0.3048;
	  snprintf(buffer, sizeof(buffer), "%d", ap_altitude_meters);
	  m_pFontManager->SetSize(m_Font, 1.0*fontWidth, 1.15*fontHeight);
	  if (ap_altitude_meters >= 10000) {
	    m_pFontManager->Print(165-0.85*4.0*fontWidth,170+12, &buffer[0], m_Font);
	  } else if (ap_altitude_meters > 1000) {
	    m_pFontManager->Print(165-0.85*3.0*fontWidth,170+12, &buffer[0], m_Font);
	  } else if (ap_altitude_meters > 100) {
	    m_pFontManager->Print(165-0.85*2.0*fontWidth,170+12, &buffer[0], m_Font);
	  } else if (ap_altitude_meters > 10 ) {
	    m_pFontManager->Print(165-0.85*fontWidth,170+12, &buffer[0], m_Font);
	  } else {
	    m_pFontManager->Print(165,170+12, &buffer[0], m_Font);
	  }
	  glColor3ub(COLOR_CYAN);
	  snprintf(buffer, sizeof(buffer), "M");
	  m_pFontManager->SetSize(m_Font, 0.75*fontWidth, 0.9*fontHeight);
	  m_pFontManager->Print(170,170+12, &buffer[0], m_Font);
	}
      }
    }

    // 5. Plot MCP states above PFD 

    // Draw in gray
    glColor3ub(COLOR_GRAYBLUE);

    // Draw the background rectangle
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(40, 190+8);
    glVertex2f(40, 170+8);
    glVertex2f(140, 170+8);
    glVertex2f(140, 190+8);
    glEnd();  

    // Draw in black
    glColor3ub(COLOR_BLACK);
    glLineWidth(3.0);

    glBegin(GL_LINES);
    glVertex2f(40,180+8);
    glVertex2f(140,180+8);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(73,170+8);
    glVertex2f(73,190+8);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(106,170+8);
    glVertex2f(106,190+8);
    glEnd();

    // Draw Autopilot states 

    m_pFontManager->SetSize(m_Font, 4, 5);

    if ((acf_type == 2) || (acf_type == 3)) {
  
      // top left
      if (*ap_spd_mode >= 1) {
    
	// Green border around background
	if (*ap_spd_mode_rec == 1) {
	  glColor3ub(COLOR_GREEN);
	  glLineWidth(lineWidth);
	  glBegin(GL_LINE_LOOP);
	  glVertex2f(40,181+8);
	  glVertex2f(72,181+8);
	  glVertex2f(72,190+8);
	  glVertex2f(40,190+8);
	  glEnd();
	}
    
	// White Text on Gray
	if (*ap_spd_mode == 1) {
	  glColor3ub(COLOR_WHITE);
	  strcpy(buffer, "ARM");
	  m_pFontManager->Print(49,183+8, &buffer[0], m_Font);
	} else if (*ap_spd_mode == 2) {
	  glColor3ub(COLOR_GREEN);
	  strcpy(buffer, "N1");
	  m_pFontManager->Print(50,183+8, &buffer[0], m_Font);
	} else if (*ap_spd_mode == 3) {
	  glColor3ub(COLOR_GREEN);
	  strcpy(buffer, "MCP SPD");
	  m_pFontManager->Print(44,183+8, &buffer[0], m_Font);
	} else if (*ap_spd_mode == 4) {
	  glColor3ub(COLOR_GREEN);
	  strcpy(buffer, "FMC SPD");
	  m_pFontManager->Print(44,183+8, &buffer[0], m_Font);
	} else if (*ap_spd_mode == 5) {
	  glColor3ub(COLOR_GREEN);
	  strcpy(buffer, "SPD 5");
	  m_pFontManager->Print(44,183+8, &buffer[0], m_Font);
	} else if (*ap_spd_mode == 6) {
	  glColor3ub(COLOR_GREEN);
	  strcpy(buffer, "THR HLD");
	  m_pFontManager->Print(44,183+8, &buffer[0], m_Font);
	} else if (*ap_spd_mode == 7) {
	  glColor3ub(COLOR_GREEN);
	  strcpy(buffer, "RETARD");
	  m_pFontManager->Print(46,183+8, &buffer[0], m_Font);
	}
      }

      // top center
      if (*ap_hdg_mode >= 1) {

	glColor3ub(COLOR_GREEN);

	if (*ap_hdg_mode_rec == 1) {
	  // Green border around background
	  glLineWidth(lineWidth);
	  glBegin(GL_LINE_LOOP);
	  glVertex2f(74,181+8);
	  glVertex2f(105,181+8);
	  glVertex2f(105,190+8);
	  glVertex2f(74,190+8);
	  glEnd();
	}

	// Green Text on Gray
	if (*ap_hdg_mode == 1) {
	  strcpy(buffer, "HDG SEL");
	  m_pFontManager->Print(77,183+8, &buffer[0], m_Font);
	} else if (*ap_hdg_mode == 2) {
	  strcpy(buffer, "VOR LOC");
	  m_pFontManager->Print(77,183+8, &buffer[0], m_Font);
	} else if (*ap_hdg_mode == 3) {
	  strcpy(buffer, "LNAV");
	  m_pFontManager->Print(81,183+8, &buffer[0], m_Font);
	} else if (*ap_hdg_mode == 4) {
	  strcpy(buffer, "ROLLOUT");
	  m_pFontManager->Print(77,183+8, &buffer[0], m_Font);
	}
      }
 
      // top right
      if (*ap_alt_mode >= 1) {

	glColor3ub(COLOR_GREEN);

	// Green border around background
	if (*ap_alt_mode_rec == 1) {
	  glLineWidth(lineWidth);
	  glBegin(GL_LINE_LOOP);
	  glVertex2f(107,181+8);
	  glVertex2f(140,181+8);
	  glVertex2f(140,190+8);
	  glVertex2f(107,190+8);
	  glEnd();
	}
	
	if (*ap_alt_mode == 5) {
	  strcpy(buffer, "G/S");
	  m_pFontManager->Print(119,183+8, &buffer[0], m_Font);
	} else if (*ap_alt_mode == 1) {
	  strcpy(buffer, "V/S");
	  m_pFontManager->Print(119,183+8, &buffer[0], m_Font);
	} else if (*ap_alt_mode == 2) {
	  strcpy(buffer, "MCP SPD");
	  m_pFontManager->Print(111,183+8, &buffer[0], m_Font);
	} else if (*ap_alt_mode == 3) {
	  strcpy(buffer, "ALT ACQ");
	  m_pFontManager->Print(111,183+8, &buffer[0], m_Font);
	} else if (*ap_alt_mode == 4) {
	  strcpy(buffer, "ALT HOLD");
	  m_pFontManager->Print(110,183+8, &buffer[0], m_Font);
	} else if (*ap_alt_mode == 5) {
	  strcpy(buffer, "ALT 5");
	  m_pFontManager->Print(110,183+8, &buffer[0], m_Font);
	} else if (*ap_alt_mode == 6) {
	  strcpy(buffer, "FLARE");
	  m_pFontManager->Print(114,183+8, &buffer[0], m_Font);
	} else if (*ap_alt_mode == 7) {
	  strcpy(buffer, "ALT 7");
	  m_pFontManager->Print(114,183+8, &buffer[0], m_Font);
	} else if (*ap_alt_mode == 8) {
	  strcpy(buffer, "VNAV SPD");
	  m_pFontManager->Print(111,183+8, &buffer[0], m_Font);
	} else if (*ap_alt_mode == 9) {
	  strcpy(buffer, "VNAV PTH");
	  m_pFontManager->Print(110,183+8, &buffer[0], m_Font);
	} else if (*ap_alt_mode == 10) {
	  strcpy(buffer, "VNAV ALT");
	  m_pFontManager->Print(110,183+8, &buffer[0], m_Font);
	} else if (*ap_alt_mode == 11) {
	  strcpy(buffer, "TO/GA");
	  m_pFontManager->Print(114,183+8, &buffer[0], m_Font);
	}
      }


      /* bottom modes are white */
      glColor3ub(COLOR_WHITE);

      // bottom left
      //      if (*ap_spd_mode_arm >= 1) {

      //      }
      
      // bottom center
      if (*ap_hdg_mode_arm >= 1) {

	// White border around background
	/*
	if (*ap_lnav_armed == 2) {
	  glLineWidth(lineWidth);
	  glBegin(GL_LINE_LOOP);
	  glVertex2f(74,170+8);
	  glVertex2f(105,170+8);
	  glVertex2f(105,179+8);
	  glVertex2f(74,179+8);
	  glEnd();
	}
	*/
    
	// White Text on Gray
	/*
	strcpy(buffer, "LNAV ARM");
	m_pFontManager->Print(76,172+8, &buffer[0], m_Font);
	strcpy(buffer, "VOR LOC");
	m_pFontManager->Print(77,172+8, &buffer[0], m_Font);
	*/
	if (*ap_hdg_mode_arm == 1) {
	  strcpy(buffer, "VOR/LOC");
	  m_pFontManager->Print(76,172+8, &buffer[0], m_Font);
	} else if (*ap_hdg_mode_arm == 2) {
	  strcpy(buffer, "ROLLOUT");
	  m_pFontManager->Print(76,172+8, &buffer[0], m_Font);
	} else if (*ap_hdg_mode_arm == 3) {
	  strcpy(buffer, "LNAV");
	  m_pFontManager->Print(81,172+8, &buffer[0], m_Font);
	}

      }
      

      // bottom right
      if (*ap_alt_mode_arm >= 1) {

	// White border around background
	/*
	if (*ap_vvi_mode == 2)  {
	  glLineWidth(lineWidth);
	  glBegin(GL_LINE_LOOP);
	  glVertex2f(107,170+8);
	  glVertex2f(140,170+8);
	  glVertex2f(140,179+8);
	  glVertex2f(107,179+8);
	  glEnd();
	}
	*/

	if (*ap_alt_mode_arm == 1) {
	  strcpy(buffer, "G/S");
	  m_pFontManager->Print(119,172+8, &buffer[0], m_Font);
	} else if (*ap_alt_mode_arm == 2) {
	  strcpy(buffer, "V/S");
	  m_pFontManager->Print(119,172+8, &buffer[0], m_Font);
	} else if (*ap_alt_mode_arm == 3) {
	  strcpy(buffer, "FLARE");
	  m_pFontManager->Print(114,172+8, &buffer[0], m_Font);
	} else if (*ap_alt_mode_arm == 4) {
	  strcpy(buffer, "ALT 4");
	  m_pFontManager->Print(114,172+8, &buffer[0], m_Font);
	} else if (*ap_alt_mode_arm == 5) {
	  strcpy(buffer, "VNAV");
	  m_pFontManager->Print(116,172+8, &buffer[0], m_Font);
	}
      }

    }
  
    // Print FD status
    if (((*fd_a_status >= 1) && is_captain) || ((*fd_b_status >= 1) && is_copilot)) {
      m_pFontManager->SetSize(m_Font, fontHeight*1.2, fontWidth*1.2);
      glColor3ub(COLOR_GREEN);
   
      if (*ap_single_ch == 1) {
	glColor3ub(COLOR_YELLOW);
	strcpy(buffer, "SINGLE CH");
	m_pFontManager->Print(70,155+0, &buffer[0], m_Font);
      } else if (*ap_autoland == 1) {
	strcpy(buffer, "LAND 3");
	m_pFontManager->Print(75,155+0, &buffer[0], m_Font);
	if (*cmd_rec == 1) {
	  glLineWidth(lineWidth);
	  glBegin(GL_LINE_LOOP);
	  glVertex2f(70,152);
	  glVertex2f(110,152);
	  glVertex2f(110,163);
	  glVertex2f(70,163);
	  glEnd();
	}
      } else {
	if ((((*cmd_a_status == 1) || (*cmd_b_status == 1)) && (acf_type >= 1)) ||
	    (((*fd_a_status == 2) || (*fd_b_status == 2)) && (acf_type == 0))) {
	  strcpy(buffer, "CMD");
	  m_pFontManager->Print(80,155+0, &buffer[0], m_Font);	  
	  if (*cmd_rec == 1) {
	    glLineWidth(lineWidth);
	    glBegin(GL_LINE_LOOP);
	    glVertex2f(77,152);
	    glVertex2f(102,152);
	    glVertex2f(102,163);
	    glVertex2f(77,163);
	    glEnd();
	  }
	} else {
	  strcpy(buffer, "FD");
	  m_pFontManager->Print(84,155+0, &buffer[0], m_Font);
	  if (*cmd_rec == 1) {
	    glLineWidth(lineWidth);
	    glBegin(GL_LINE_LOOP);
	    glVertex2f(81,152);
	    glVertex2f(98,152);
	    glVertex2f(98,163);
	    glVertex2f(81,163);
	    glEnd();
	  }
	}
      }      

    }

    // Draw the glideslope and localizer displays to the right and bottom of the ADI

    // Draw glide slope to the right of the ADI

    // Where is the center of the ADI?
    
    // Where is the center of the ADI?
    float ADICenterX = (94/2) + 42;
    float ADICenterY = (98/2) + 52;
    
    // The height of the glideslope markers above and below center (+/- 1 and 2 degrees deflection)
    float glideslopeHeight = 17;
    
    // Horizontal position of the dots relative to the ADI center
    float dotsHoriz = ADICenterX + 51;
    
    // Horizontal center line
    glColor3ub(COLOR_WHITE);
    glBegin(GL_LINES);
    glVertex2f( dotsHoriz - 4, ADICenterY );
    glVertex2f( dotsHoriz + 4, ADICenterY );
    glEnd();
    
    // Draw the glideslope dots
    
    // Set up the circle
    CircleEvaluator aCircle;
    aCircle.SetRadius(2.0);
    aCircle.SetArcStartEnd(0,360);
    aCircle.SetDegreesPerPoint(10);
    
    glLineWidth(lineWidth);
    
    aCircle.SetOrigin(dotsHoriz, ADICenterY + glideslopeHeight * 2);
    glBegin(GL_LINE_LOOP);
    aCircle.Evaluate();
    glEnd();
    
    aCircle.SetOrigin(dotsHoriz, ADICenterY + glideslopeHeight);
    glBegin(GL_LINE_LOOP);
    aCircle.Evaluate();
    glEnd();
    
    aCircle.SetOrigin(dotsHoriz, ADICenterY + -1 * glideslopeHeight);
    glBegin(GL_LINE_LOOP);
    aCircle.Evaluate();
    glEnd();
    
    aCircle.SetOrigin(dotsHoriz, ADICenterY + -1 * glideslopeHeight * 2);
    glBegin(GL_LINE_LOOP);
    aCircle.Evaluate();
    glEnd();
    
    // This is the glideslope bug
    if ( ((*nav1_vertical == 1) && (*nav1_CDI == 1) && (is_captain)) || 
	 ((*nav2_vertical == 1) && (*nav2_CDI == 1) && (is_copilot)) ) 
      {
	if (*nav1_vertical == 1) {
	  rawGlideslope = *nav1_vdef;
	} else {
	  rawGlideslope = *nav2_vdef;
	}

	if (rawGlideslope < - 2.5) rawGlideslope = -2.5;
	if (rawGlideslope >   2.5) rawGlideslope = 2.5;
    
	// The vertical offset of the glideslope bug
	float glideslopePosition = ADICenterY - rawGlideslope * glideslopeHeight;

	// Todo: fill the glideslope bug when we are in glide slope
	// color is magenta
	glColor3ub(COLOR_VIOLET);
	if (fabs(rawGlideslope) < 2.49) {
	  glBegin(GL_POLYGON);
	} else {
	  glBegin(GL_LINE_LOOP);
	}
	glVertex2f( dotsHoriz - 2, glideslopePosition ); 	
	glVertex2f( dotsHoriz, glideslopePosition + 4 );
	glVertex2f( dotsHoriz + 2, glideslopePosition );
	glVertex2f( dotsHoriz, glideslopePosition - 4 );
	glEnd();	
      }

    
    // Draw the localizer
    
    // Height of localizer center in the PFD
    float localizerHeight = 48.0;
    
    // Overall localizer width
    float localizerWidth = 17.0;
    		
    // Verticalal center line
    glColor3ub(COLOR_WHITE);
    glBegin(GL_LINES);
    glVertex2f( ADICenterX, localizerHeight + 4 );
    glVertex2f( ADICenterX, localizerHeight - 4 );
    glEnd();
    
    // Draw the localizer dots
    
    // Set up the circle
    aCircle.SetRadius(2.0);
    aCircle.SetArcStartEnd(0,360);
    aCircle.SetDegreesPerPoint(10);
    
    glLineWidth(lineWidth);
    
    aCircle.SetOrigin(ADICenterX + localizerWidth * 2, localizerHeight);
    glBegin(GL_LINE_LOOP);
    aCircle.Evaluate();
    glEnd();
    
    aCircle.SetOrigin(ADICenterX + localizerWidth, localizerHeight);
    glBegin(GL_LINE_LOOP);
    aCircle.Evaluate();
    glEnd();
    
    aCircle.SetOrigin(ADICenterX + -1 * localizerWidth, localizerHeight);
    glBegin(GL_LINE_LOOP);
    aCircle.Evaluate();
    glEnd();
    
    aCircle.SetOrigin(ADICenterX + -1 * localizerWidth * 2, localizerHeight);
    glBegin(GL_LINE_LOOP);
    aCircle.Evaluate();
    glEnd();
	
    if ( ((*nav1_horizontal == 1) && (is_captain)) ||
	 ((*nav2_horizontal == 1) && (is_copilot)) ) 
      {
	if (*nav1_horizontal == 1) {
	  rawLocalizer = *nav1_hdef;
	} else {
	  rawLocalizer = *nav2_hdef;
	}

	// The horizontal offset of the localizer bug
	float localizerPosition = ADICenterX + rawLocalizer * localizerWidth;
 	// This is the localizer bug
	glColor3ub(COLOR_VIOLET);
	if (fabs(rawLocalizer) < 2.49) {
	  glBegin(GL_POLYGON);
	} else {
	  glBegin(GL_LINE_LOOP);
	}
	glVertex2f( localizerPosition - 4, localizerHeight ); 	
	glVertex2f( localizerPosition, localizerHeight + 2 );
	glVertex2f( localizerPosition + 4, localizerHeight );
	glVertex2f( localizerPosition, localizerHeight - 2 );
	glEnd();	

      }

    // draw GPWS in RED
    if (*gpws != INT_MISS) {
      if (*gpws == 1) {
	m_pFontManager->SetSize(m_Font, fontHeight*1.5, fontWidth*1.5);
	glColor3ub(COLOR_RED);
	strcpy(buffer, "PULL UP");
	m_pFontManager->Print(70,42, &buffer[0], m_Font);
      }
    }
    // draw Wind Shear Warning in RED
    if (*windshear != INT_MISS) {
      if (*windshear == 1) {
	m_pFontManager->SetSize(m_Font, fontHeight*1.5, fontWidth*1.5);
	glColor3ub(COLOR_RED);
	strcpy(buffer, "WINDSHEAR");
	m_pFontManager->Print(58,42, &buffer[0], m_Font);
      }
    }

    // draw minimum altitude (feet)
    if ((*altimeter_minimum != FLT_MISS) && (*altimeter_minimum > 0.0)) {
      m_pFontManager->SetSize(m_Font, fontWidth, fontHeight);
      glColor3ub(COLOR_GREEN);
      if ((acf_type == 2) || (acf_type == 3)) {
	if (*minimum_mode == 0) {
	  strcpy(buffer, "RADIO");
	} else {
	  strcpy(buffer, "BARO");
	}
      }
      m_pFontManager->Print(117,35, &buffer[0], m_Font);
      snprintf(buffer, sizeof(buffer), "%4.0f", *altimeter_minimum);
      m_pFontManager->Print(124,28, &buffer[0], m_Font);
    }

    if (*aoa != FLT_MISS) {
      float minDegrees = -45.0;
      float maxDegrees = 180.0;
      float R = 11.0;
      float xcircle = 130;
      float ycircle = 164;
      CircleEvaluator bCircle;
      
      m_pFontManager->SetSize(m_Font, 3.8, 3.8);
      glLineWidth(lineWidth);
      
      glColor3ub(COLOR_WHITE);
      bCircle.SetRadius(R);
      bCircle.SetArcStartEnd(minDegrees,maxDegrees);
      bCircle.SetDegreesPerPoint(10);
      bCircle.SetOrigin(xcircle,ycircle);
      glBegin(GL_LINE_STRIP);
      bCircle.Evaluate();
      glEnd();
      glColor3ub(COLOR_GREEN);
      bCircle.SetRadius(R+1);
      bCircle.SetArcStartEnd(78.0,102.0);
      bCircle.SetDegreesPerPoint(10);
      bCircle.SetOrigin(xcircle,ycircle);
      glBegin(GL_LINE_STRIP);
      bCircle.Evaluate();
      glEnd();
      glColor3ub(COLOR_RED);
      glBegin(GL_LINES);
      glVertex2f(xcircle,ycircle+R);
      glVertex2f(xcircle,ycircle+R+2);
      glEnd();

      glColor3ub(COLOR_WHITE);
      float radians;
      for (float degrees = minDegrees; degrees <= maxDegrees; degrees += 45.0) {
	radians = degrees * atan2(0.0, -1.0) / 180.;
	glBegin(GL_LINE_STRIP);
	glVertex2f(xcircle + R * sin(radians),
		   ycircle + R * cos(radians));
	glVertex2f(xcircle + (R - 2) * sin(radians),
		   ycircle + (R - 2) * cos(radians));
	glEnd();
      }
      if ((*aoa < 20.0) && (*aoa > -5.0)) {
	radians = ((20.0-*aoa)*9.0 - 45.0) * atan2(0.0, -1.0) / 180.;
	glBegin(GL_LINE_STRIP);
	glVertex2f(xcircle,
		   ycircle);
	glVertex2f(xcircle + R * sin(radians),
		   ycircle + R * cos(radians));
	glEnd();
      
	snprintf(buffer, sizeof(buffer), "%2.1f", *aoa);
	m_pFontManager->Print(xcircle-11.0,ycircle-5.5, &buffer[0], m_Font);
      } else {
	snprintf(buffer, sizeof(buffer), "0.0");
	m_pFontManager->Print(xcircle-11.0,ycircle-5.5, &buffer[0], m_Font);
      }

    }

    if ((acf_type == 2) || (acf_type == 3)) {
      // No Reference Speed indicator
      if (*no_vspeed == 1) {
	m_pFontManager->SetSize(m_Font, 4.5, 5.5);
	glColor3ub(COLOR_ORANGE);
	strcpy(buffer, "NO");
	m_pFontManager->Print(32,130, &buffer[0], m_Font); 
	strcpy(buffer, "V");
	m_pFontManager->Print(34,123, &buffer[0], m_Font); 
	strcpy(buffer, "S");
	m_pFontManager->Print(34,116, &buffer[0], m_Font); 
	strcpy(buffer, "P");
	m_pFontManager->Print(34,109, &buffer[0], m_Font); 
	strcpy(buffer, "D");
	m_pFontManager->Print(34,101, &buffer[0], m_Font); 
       }

      // NAV Text on PFD
      unsigned char *text1 = link_dataref_byte_arr("laminar/B738/pfd/cpt_nav_txt1",20,-1);
      unsigned char *text2 = link_dataref_byte_arr("laminar/B738/pfd/cpt_nav_txt2",20,-1);
      float *lnav_status = link_dataref_flt("laminar/B738/autopilot/lnav_status",0);
      float *ils_show;
      if (is_captain) {
	ils_show = link_dataref_flt("laminar/B738/pfd/ils_show",0);
      } else {
	ils_show = link_dataref_flt("laminar/B738/pfd/ils_fo_show",0);
      }
      m_pFontManager->SetSize(m_Font, 4, 4.5);
      glColor3ub(COLOR_WHITE);
      snprintf( buffer, sizeof(buffer), "%s", text1 );
      /* fix degree sign */
      for (long unsigned int i=0; i<sizeof(buffer); i++) {
	if (((int) buffer[i]) == 61) buffer[i] = (char) 176; // X737
	if (((int) buffer[i]) == 96) buffer[i] = (char) 176; // ZIBO
      }
      m_pFontManager->Print(42,170,buffer, m_Font); 
      snprintf( buffer, sizeof(buffer), "%s", text2 );
      m_pFontManager->Print(42,162,buffer, m_Font);
      if (*lnav_status == 1.0) {
	strcpy( buffer, "LNAV/VNAV");
	m_pFontManager->Print(42,154,buffer, m_Font);
      } else if (*ils_show == 1.0) {
	strcpy( buffer, "ILS");
	m_pFontManager->Print(42,154,buffer, m_Font);
      }

      // Control Panel Source Warning
      float *control_panel_source = link_dataref_flt("laminar/B738/toggle_switch/dspl_ctrl_pnl",0);
      if ((is_captain && (*control_panel_source == 1)) || (is_copilot && (*control_panel_source == -1))) {
	m_pFontManager->SetSize(m_Font, 3, 3.5);
	glColor3ub(COLOR_ORANGE);
	snprintf( buffer, sizeof(buffer), "%s", "DISPLAYS" );
	m_pFontManager->Print(150,15,buffer, m_Font);
	snprintf( buffer, sizeof(buffer), "%s", "CONTROL PANEL" );
	m_pFontManager->Print(143,9,buffer, m_Font);
      }
    }

    
    glPopMatrix();
  }

} // end namespace OpenGC

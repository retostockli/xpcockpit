/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcAirframeDataContainer.cpp,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:27:55 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2003 Damion Shelton
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "ogcAirframeDataContainer.h"

namespace OpenGC
{

AirframeDataContainer::AirframeDataContainer()
{
  this->InitializeData();
}

AirframeDataContainer::~AirframeDataContainer()
{

}

void
AirframeDataContainer
::InitializeData()
{

  // Clock and Timer

  m_Hours = 0;
  m_Minutes = 0;
  m_Seconds = 0;

  // Heading and Location

  m_Roll = 0;
  m_Pitch = 0;

  m_Mag_Heading = 0;
  m_Mag_Variation = 0;
  m_True_Heading = 0;

  m_Latitude = 0;
  m_Longitude = 0;
  
  m_Turn_Coordinator_Ball = 0;  // what is this?
  m_Turn_Rate = 0;

  // Speed

  m_IAS = 0;
  m_EAS = 0;
  m_TAS = 0;
  m_Mach = 0;

  m_Vso = 0;
  m_Vs = 0;
  m_Vfe = 0;
  m_Vno = 0;
  m_Vne = 0;

  m_Flap_Ratio = 0;

  m_Ground_Speed_MS = 0;
  m_Ground_Speed_K = 0;

  m_Vertical_Speed_MS = 0;
  m_Vertical_Speed_FPS = 0;
  m_Vertical_Speed_FPM = 0;

  // Altitude

  m_Pressure_Alt_Feet = 0;
  m_Pressure_Alt_Metres = 0;
  m_Altitude_AGL_Feet = 0;
  m_Altitude_AGL_Metres = 0;
  m_Altitude_MSL_Feet = 0;
  m_Altitude_MSL_Metres = 0;
  m_Altimeter_Pressure = 0;
  m_Altimeter_Pressure_Unit = false;

  // Control Surfaces

  m_Flaps_Deflection = 0;
  m_Commanded_Flaps_Deflection = 0;
  m_Commanded_Flaps_Deflection_Modified = false;
  m_Aileron_Deflection = 0;
  m_Commanded_Aileron_Deflection = 0;
  m_Commanded_Aileron_Deflection_Modified = false;
  m_Rudder_Deflection = 0;
  m_Commanded_Rudder_Deflection = 0;
  m_Commanded_Rudder_Deflection_Modified = false;
  m_Elevator_Deflection = 0;
  m_Commanded_Elevator_Deflection = 0;
  m_Commanded_Elevator_Deflection_Modified = false;

  // Landing Gear

  m_Nose_Gear_Position = 0;
  m_Left_Gear_Position = 0;
  m_Right_Gear_Position = 0;
  m_Gear_Down_Command = false;
  m_Gear_Down_Command_Modified = false;

  m_Left_Brake = 0;
  m_Left_Brake_Modified = false;
  m_Right_Brake = 0;
  m_Right_Brake_Modified = false;

  // Diverse

  m_Total_Air_Temperature = 0;

  // Engines

  m_EGT1 = 0;
  m_EGT2 = 0;
  m_EGT3 = 0;
  m_EGT4 = 0;

  m_Fuel_Flow_PPH1 = 0;
  m_Fuel_Flow_PPH2 = 0;
  m_Fuel_Flow_PPH3 = 0;
  m_Fuel_Flow_PPH4 = 0;

  m_Fuel_Tank_Level_Center = 0;
  m_Fuel_Tank_Capacity_Center = 0;
  m_Fuel_Tank_Level_Left = 0;
  m_Fuel_Tank_Capacity_Left = 0;
  m_Fuel_Tank_Level_Right = 0;
  m_Fuel_Tank_Capacity_Right = 0;
  m_Fuel_Tank_Capacity_Total = 1;

  m_Engine_Hydraulic_Pressure1 = 0;
  m_Engine_Hydraulic_Pressure2 = 0;
  m_Engine_Hydraulic_Pressure3 = 0;
  m_Engine_Hydraulic_Pressure4 = 0;

  m_Engine_Hydraulic_Quantity1 = 0;
  m_Engine_Hydraulic_Quantity2 = 0;
  m_Engine_Hydraulic_Quantity3 = 0;
  m_Engine_Hydraulic_Quantity4 = 0;

  m_N11 = 0;
  m_N12 = 0;
  m_N13 = 0;
  m_N14 = 0;

  m_N21 = 0;
  m_N22 = 0;
  m_N23 = 0;
  m_N24 = 0;

  m_Oil_Pressure1 = 0;
  m_Oil_Pressure2 = 0;
  m_Oil_Pressure3 = 0;
  m_Oil_Pressure4 = 0;

  m_Oil_Quantity1 = 0;
  m_Oil_Quantity2 = 0;
  m_Oil_Quantity3 = 0;
  m_Oil_Quantity4 = 0;

  m_Oil_Temp_C1 = 0;
  m_Oil_Temp_C2 = 0;
  m_Oil_Temp_C3 = 0;
  m_Oil_Temp_C4 = 0;

  m_Engine_Vibration1 = 0;
  m_Engine_Vibration2 = 0;
  m_Engine_Vibration3 = 0;
  m_Engine_Vibration4 = 0;

  // Navigation

  m_Nav1_Valid = false;
  m_Nav1_Has_DME = false;
  m_Adf1_Valid = false;
  m_Nav1_Localizer_Needle = 0;
  m_Nav1_CDI = false;
  m_Nav2_CDI = false;

  m_Nav1_Heading = 0;
  m_Nav1_Bearing = 0;
  m_Nav1_Hdef = 0;
  m_Nav1_Vdef = 0;
  m_Nav1_DME = 0;
  m_Nav1_FromTo = false;
  m_Nav1_Glideslope = false;
  m_Nav1_Horizontal = false;
  m_Nav1_Vertical = false;

  m_Adf1_Heading = 0;
  m_Adf1_Bearing = 0;
  m_Adf1_DME = 0;
  m_Adf1_FromTo = false;

  m_Nav2_Valid = false;
  m_Nav2_Has_DME = false;
  m_Adf2_Valid = false;
  m_Nav2_Localizer_Needle = 0;

  m_Nav2_Heading = 0;
  m_Nav2_Bearing = 0;
  m_Nav2_Hdef = 0;
  m_Nav2_Vdef = 0;
  m_Nav2_DME = 0;
  m_Nav2_FromTo = false;
  m_Nav2_Glideslope = false;
  m_Nav2_Horizontal = false;
  m_Nav2_Vertical = false;

  m_Adf2_Heading = 0;
  m_Adf2_Bearing = 0;
  m_Adf2_DME = 0;
  m_Adf2_FromTo = false;

  m_Nav1_OBS = 0;

  m_Nav_Range_Selector = 0;

  // Autopilot

  m_Autopilot_Engaged = false;
  m_Director_Engaged = false;
  m_Director_A_Engaged = false;
  m_Director_B_Engaged = false;
  m_Director_A_Pitch = 0;
  m_Director_A_Roll = 0;
  m_Director_B_Pitch = 0;
  m_Director_B_Roll = 0;

  m_Autopilot_Altitude = 0;
  m_Autopilot_Heading = 0;
  m_Autopilot_Speed = 0;
  m_Autopilot_Speed_Is_Mach = false;
  m_Autopilot_Vertical_Speed = 0;

  m_Cmd_Heading_Engaged = false;
  m_Cmd_Altitude_Engaged = false;
  m_Cmd_Airspeed_Engaged = false;
  m_Cmd_Approach_Engaged = false;
  m_Cmd_Level_Change = false;
  m_Cmd_Vertical_Speed_Armed = false;
  m_Cmd_Vertical_Speed_Engaged = false;
  m_Cmd_LNAV_Armed = false;
  m_Cmd_LNAV_Engaged = false;
  m_Cmd_VNAV_Engaged = false;
  m_Cmd_VORLOC_Armed = false;
  m_Cmd_VORLOC_Engaged = false;
  m_Cmd_AT_Armed = false;
  m_Cmd_AT_Speed = false;
  m_Cmd_AT_N1 = false;

  m_PFD_N1_mode = 0;
  m_PFD_MCPSPD_mode = 0;
  m_PFD_FMCSPD_mode = 0;
  m_PFD_RETARD_mode = 0;
  m_PFD_THRHLD_mode = 0;
  m_PFD_LNAVARMED_mode = 0;
  m_PFD_VORLOCARMED_mode = 0;
  m_PFD_PITCHSPD_mode = 0;
  m_PFD_ALTHLD_mode = 0;
  m_PFD_VSARMED_mode = 0;
  m_PFD_VS_mode = 0;
  m_PFD_VNAVALT_mode = 0;
  m_PFD_VNAVPATH_mode = 0;
  m_PFD_VNAVSPD_mode = 0;
  m_PFD_GSARMED_mode = 0;
  m_PFD_GS_mode = 0;
  m_PFD_FLAREARMED_mode = 0;
  m_PFD_FLARE_mode = 0;
  m_PFD_TOGA_mode = 0;
  m_PFD_LNAV_mode = 0;
  m_PFD_HDG_mode = 0;
  m_PFD_VORLOC_mode = 0;
 
  m_Cmd_Nav1_Lock_Flag = false;
}

} // end namespace OpenGC

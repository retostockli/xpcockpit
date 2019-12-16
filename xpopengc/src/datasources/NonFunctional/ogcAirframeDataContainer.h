/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcAirframeDataContainer.h,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:27:56 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2004 Damion Shelton
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef ogcAirframeDataContainer_h
#define ogcAirframeDataContainer_h

#include "ogcDataContainer.h"

extern "C" {
#include "serverdata.h"
}

namespace OpenGC
{

class AirframeDataContainer : public DataContainer
{

  //
  //--------------Clock and Timer----------------
  //

  /** (Zulu) clock hours, minutes, seconds */
  DataReadMacro(Hours, double);
  DataReadMacro(Minutes, double);
  DataReadMacro(Seconds, double);

  //
  //------------Heading and Location-------------
  //

  /** Roll in degrees around the axis of flight, right roll + */
  DataReadMacro(Roll, double);

  /** Pitch in degrees from horizontal, pitch up + */
  DataReadMacro(Pitch, double);

  /** Magnetic heading */
  DataReadMacro(Mag_Heading, double);

  /** Magnetic variation in degrees, West -
   * Convert True headings to Magnetic by subtracting this value,
   * Magnetic headings to True by adding this value.
   */
  DataReadMacro(Mag_Variation, double);

  /** True heading in degrees */
  DataReadMacro(True_Heading, double);

  /** Latitude in degrees (with fractional degrees) North +, South - */
  DataReadMacro(Latitude, double);

  /** Longitude in degrees (with fractional degrees) East +, West - */
  DataReadMacro(Longitude, double); 

  /** Turn coordinator ball position (slip and skid). -1.0 is extreme left, +1.0 is extreme right, 0 is balanced. */
  DataReadMacro(Turn_Coordinator_Ball, double);
  
  /** Turn Rate (for turn coordinator). 0=level, -1.0 = 2min Left, +1.0 = 2min Right */
  DataReadMacro(Turn_Rate, double);

  //
  //--------------Speed--------------- 
  //

  /** Indicated airspeed in knots */
  DataReadMacro(IAS, double);

  /** Equivalent airspeed */
  DataReadMacro(EAS, double);
  
  /** True airspeed in knots */
  DataReadMacro(TAS, double);

  /** Minimum Stall Speed during Landing in knots */
  DataReadMacro(Vso, double);

  /** Minimum Stall Speed in knots */
  DataReadMacro(Vs, double);

  /** Maximum Flap extended Speed in knots */
  DataReadMacro(Vfe, double);

  /** Maximum Operation Speed in knots */
  DataReadMacro(Vno, double);

  /** Maximum Never Extend Speed in knots */
  DataReadMacro(Vne, double);

  /** Flap extension ratio (0-1) */
  DataReadMacro(Flap_Ratio, double);

  /** Mach number */
  DataReadMacro(Mach, double);

  /** Ground speed in meters/sec */
  DataReadMacro(Ground_Speed_MS, double);

  /** Ground speed in knots */
  DataReadMacro(Ground_Speed_K, double);

  /** Rate of climb or descent, in m/s */
  DataReadMacro(Vertical_Speed_MS, double);

  /** Rate of climb or descent, in feet per second */
  DataReadMacro(Vertical_Speed_FPS, double);

  /** Rate of climb or descent, in feet per minute */
  DataReadMacro(Vertical_Speed_FPM, double);

  //
  //------------Altitude---------------
  //

  /** Pressure altitude in feet */
  DataReadMacro(Pressure_Alt_Feet, double);

  /** Pressure altitude in metres */
  DataReadMacro(Pressure_Alt_Metres, double);

  /** Altitude in feet above ground level */
  DataReadMacro(Altitude_AGL_Feet, double);

  /** Altitude in metres above ground level */
  DataReadMacro(Altitude_AGL_Metres, double);

  /** Altitude in feet above sea level */
  DataReadMacro(Altitude_MSL_Feet, double);

  /** Altitude in metres above sea level */
  DataReadMacro(Altitude_MSL_Metres, double);
  
  /** Barometric altimeter reference pressure in inches of mercury */
  DataReadMacro(Altimeter_Pressure, double);

  /** Displayed Barometric altimeter reference pressure Unit: 0: InHg 1: hPa */
  DataReadMacro(Altimeter_Pressure_Unit, bool);

  //
  //--------------Control Surfaces------------------
  //

  /** Flaps deflection, 0 = full retraction, 1 = full extension.
    * Note that the interpretation of this data varies on a 
    * per aircraft basis
    */
  DataReadMacro(Flaps_Deflection, double);

  /** The flaps setting commanded by the pilot, different
    * than the actual deflection while the flaps are in transit.
    */
  DataReadWriteMacro(Commanded_Flaps_Deflection, double);
  
  /** Aileron deflection, -1 = max left, 0 level, +1 max right */
  DataReadMacro(Aileron_Deflection, double);

  /** The aileron setting commanded by the pilot, different
    * than the actual deflection while the ailerons are in transit.
    */
  DataReadWriteMacro(Commanded_Aileron_Deflection, double);
  
  /** Actual rudder deflection scaled from -1 (full left) to +1 (full right) */
  DataReadMacro(Rudder_Deflection, double);

  /** The rudder setting commanded by the pilot, different
    * than the actual deflection while the rudder is in transit.
    */
  DataReadWriteMacro(Commanded_Rudder_Deflection, double);

  /** Actual elevator deflection, -1 max dive, +1 max climb */
  DataReadMacro(Elevator_Deflection, double);

  /** The rudder setting commanded by the pilot, different
    * than the actual deflection while the rudder is in transit.
    */
  DataReadWriteMacro(Commanded_Elevator_Deflection, double);

  //
  //--------------Landing gear------------------
  //

  /** Nose gear position, 0 = full up, 1 = full down */
  DataReadMacro(Nose_Gear_Position, double);
  
  /** Left main gear position, 0 = full up, 1 = full down */
  DataReadMacro(Left_Gear_Position, double);

  /** Right main gear position, 0 = full up, 1 = full down */
  DataReadMacro(Right_Gear_Position, double);

  /** Commanded gear position, false = up, true = down */
  DataReadWriteMacro(Gear_Down_Command, bool);

  /** Amount of left brake, 0 = none, 1 = max brake */
  DataReadWriteMacro(Left_Brake, double);

  /** Amount of right brake, 0 = none, 1 = max brake */
  DataReadWriteMacro(Right_Brake, double);

  //
  //--------------Engines------------------
  //
  /** EGT -> maybe set to array, but how?*/
  DataReadWriteMacro(EGT1,double);
  DataReadWriteMacro(EGT2,double);
  DataReadWriteMacro(EGT3,double);
  DataReadWriteMacro(EGT4,double);

  DataReadWriteMacro(Fuel_Flow_PPH1,double);
  DataReadWriteMacro(Fuel_Flow_PPH2,double);
  DataReadWriteMacro(Fuel_Flow_PPH3,double);
  DataReadWriteMacro(Fuel_Flow_PPH4,double);

  DataReadWriteMacro(Fuel_Tank_Level_Center,double);
  DataReadWriteMacro(Fuel_Tank_Capacity_Center,double);
  DataReadWriteMacro(Fuel_Tank_Level_Left,double);
  DataReadWriteMacro(Fuel_Tank_Capacity_Left,double);
  DataReadWriteMacro(Fuel_Tank_Level_Right,double);
  DataReadWriteMacro(Fuel_Tank_Capacity_Right,double);
  DataReadWriteMacro(Fuel_Tank_Capacity_Total,double);

  DataReadWriteMacro(Engine_Hydraulic_Pressure1,double);
  DataReadWriteMacro(Engine_Hydraulic_Pressure2,double);
  DataReadWriteMacro(Engine_Hydraulic_Pressure3,double);
  DataReadWriteMacro(Engine_Hydraulic_Pressure4,double);

  DataReadWriteMacro(Engine_Hydraulic_Quantity1,double);
  DataReadWriteMacro(Engine_Hydraulic_Quantity2,double);
  DataReadWriteMacro(Engine_Hydraulic_Quantity3,double);
  DataReadWriteMacro(Engine_Hydraulic_Quantity4,double);

  DataReadWriteMacro(N11,double);
  DataReadWriteMacro(N12,double);
  DataReadWriteMacro(N13,double);
  DataReadWriteMacro(N14,double);

  DataReadWriteMacro(N21,double);
  DataReadWriteMacro(N22,double);
  DataReadWriteMacro(N23,double);
  DataReadWriteMacro(N24,double);

  DataReadWriteMacro(Oil_Pressure1,double);
  DataReadWriteMacro(Oil_Pressure2,double);
  DataReadWriteMacro(Oil_Pressure3,double);
  DataReadWriteMacro(Oil_Pressure4,double);

  DataReadWriteMacro(Oil_Quantity1,double);
  DataReadWriteMacro(Oil_Quantity2,double);
  DataReadWriteMacro(Oil_Quantity3,double);
  DataReadWriteMacro(Oil_Quantity4,double);

  DataReadWriteMacro(Oil_Temp_C1,double);
  DataReadWriteMacro(Oil_Temp_C2,double);
  DataReadWriteMacro(Oil_Temp_C3,double);
  DataReadWriteMacro(Oil_Temp_C4,double);

  DataReadWriteMacro(Engine_Vibration1,double);
  DataReadWriteMacro(Engine_Vibration2,double);
  DataReadWriteMacro(Engine_Vibration3,double);
  DataReadWriteMacro(Engine_Vibration4,double);

  //
  //--------------Diverse------------------
  //
  /** Air Temperature */
  DataReadWriteMacro(Total_Air_Temperature, double);


  //
  //--------------Navigation------------------
  //
  DataReadWriteMacro(Nav1_Valid, bool);
  DataReadWriteMacro(Nav1_Has_DME, bool);
  DataReadWriteMacro(Adf1_Valid, bool);
  DataReadWriteMacro(Nav1_Localizer_Needle, double);
  DataReadWriteMacro(Nav1_CDI,bool);
  DataReadWriteMacro(Nav2_CDI,bool)

  DataReadWriteMacro(Nav1_Heading, double);
  DataReadWriteMacro(Nav1_Bearing, double);
  DataReadWriteMacro(Nav1_Hdef, double);
  DataReadWriteMacro(Nav1_Vdef, double);
  DataReadWriteMacro(Nav1_DME, double);
  DataReadWriteMacro(Nav1_FromTo, bool);
  DataReadWriteMacro(Nav1_Glideslope, bool);
  DataReadWriteMacro(Nav1_Horizontal, bool);
  DataReadWriteMacro(Nav1_Vertical, bool);

  DataReadWriteMacro(Adf1_Heading, double);
  DataReadWriteMacro(Adf1_Bearing, double);
  DataReadWriteMacro(Adf1_DME, double);
  DataReadWriteMacro(Adf1_FromTo, bool);

  DataReadWriteMacro(Nav2_Valid, bool);
  DataReadWriteMacro(Nav2_Has_DME, bool);
  DataReadWriteMacro(Adf2_Valid, bool);
  DataReadWriteMacro(Nav2_Localizer_Needle, double);

  DataReadWriteMacro(Nav2_Heading, double);
  DataReadWriteMacro(Nav2_Bearing, double);
  DataReadWriteMacro(Nav2_Hdef, double);
  DataReadWriteMacro(Nav2_Vdef, double);
  DataReadWriteMacro(Nav2_DME, double);
  DataReadWriteMacro(Nav2_FromTo, bool);
  DataReadWriteMacro(Nav2_Glideslope, bool);
  DataReadWriteMacro(Nav2_Horizontal, bool);
  DataReadWriteMacro(Nav2_Vertical, bool);

  DataReadWriteMacro(Adf2_Heading, double);
  DataReadWriteMacro(Adf2_Bearing, double);
  DataReadWriteMacro(Adf2_DME, double);
  DataReadWriteMacro(Adf2_FromTo, bool);

  DataReadWriteMacro(Nav1_OBS, double);

  DataReadWriteMacro(Nav_Range_Selector, int);


  //
  //--------------Autopilot------------------
  //
  
  DataReadWriteMacro(Autopilot_Engaged, bool);
  DataReadWriteMacro(Director_Engaged, bool);
  DataReadWriteMacro(Director_A_Engaged, bool);
  DataReadWriteMacro(Director_B_Engaged, bool);
  DataReadWriteMacro(Director_A_Pitch, double);
  DataReadWriteMacro(Director_A_Roll, double);
  DataReadWriteMacro(Director_B_Pitch, double);
  DataReadWriteMacro(Director_B_Roll, double);

  DataReadWriteMacro(Autopilot_Altitude, double);
  DataReadWriteMacro(Autopilot_Heading, double);
  DataReadWriteMacro(Autopilot_Speed, double);
  DataReadWriteMacro(Autopilot_Speed_Is_Mach, bool);
  DataReadWriteMacro(Autopilot_Vertical_Speed, double);

  DataReadWriteMacro(Cmd_Heading_Engaged, bool);
  DataReadWriteMacro(Cmd_Altitude_Engaged, bool);
  DataReadWriteMacro(Cmd_Airspeed_Engaged, bool);
  DataReadWriteMacro(Cmd_Approach_Engaged, bool);
  DataReadWriteMacro(Cmd_Level_Change, bool);
  DataReadWriteMacro(Cmd_Vertical_Speed_Armed, bool);
  DataReadWriteMacro(Cmd_Vertical_Speed_Engaged, bool);
  DataReadWriteMacro(Cmd_LNAV_Armed, bool);
  DataReadWriteMacro(Cmd_LNAV_Engaged, bool);
  DataReadWriteMacro(Cmd_VNAV_Engaged, bool);
  DataReadWriteMacro(Cmd_VORLOC_Armed, bool);
  DataReadWriteMacro(Cmd_VORLOC_Engaged, bool);
  DataReadWriteMacro(Cmd_AT_Armed, bool);
  DataReadWriteMacro(Cmd_AT_Speed, bool);
  DataReadWriteMacro(Cmd_AT_N1, bool);

  DataReadWriteMacro(PFD_N1_mode, int);
  DataReadWriteMacro(PFD_MCPSPD_mode, int);
  DataReadWriteMacro(PFD_FMCSPD_mode, int);
  DataReadWriteMacro(PFD_RETARD_mode, int);
  DataReadWriteMacro(PFD_THRHLD_mode, int);
  DataReadWriteMacro(PFD_LNAVARMED_mode, int);
  DataReadWriteMacro(PFD_VORLOCARMED_mode, int);
  DataReadWriteMacro(PFD_PITCHSPD_mode, int);
  DataReadWriteMacro(PFD_ALTHLD_mode, int);
  DataReadWriteMacro(PFD_VSARMED_mode, int);
  DataReadWriteMacro(PFD_VS_mode, int);
  DataReadWriteMacro(PFD_VNAVALT_mode, int);
  DataReadWriteMacro(PFD_VNAVPATH_mode, int);
  DataReadWriteMacro(PFD_VNAVSPD_mode, int);
  DataReadWriteMacro(PFD_GSARMED_mode, int);
  DataReadWriteMacro(PFD_GS_mode, int);
  DataReadWriteMacro(PFD_FLAREARMED_mode, int);
  DataReadWriteMacro(PFD_FLARE_mode, int);
  DataReadWriteMacro(PFD_TOGA_mode, int);
  DataReadWriteMacro(PFD_LNAV_mode, int);
  DataReadWriteMacro(PFD_HDG_mode, int);
  DataReadWriteMacro(PFD_VORLOC_mode, int);
  
  DataReadWriteMacro(Cmd_Nav1_Lock_Flag, bool);
  

public:
  
  void InitializeData();

  AirframeDataContainer();
  virtual ~AirframeDataContainer();

protected:

};

} // end namespace OpenGC
#endif

/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcFDSDataSource.cpp,v $

  Copyright (C) 2001-2 by:
    Original author:
      Damion Shelton
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2004/10/14 19:27:57 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
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
#include "ogcDataSource.h"
#include "ogcFDSDataSource.h"

// First the code for the "real" version on Win32
#ifdef _WIN32

#include "FDSConnection.h"

namespace OpenGC
{

FDSDataSource::FDSDataSource()
{
  printf("oFDSDataSource constructed\n");

  // Initialize the data
  this->InitializeData();

  // Open the FDS connection
  m_pFDSConnection = new CFDSConnection();

  // Try to open the FDS connection
  if( m_pFDSConnection->Open() )
  {
    m_ValidConnection = true;
  }
  else
  {
    m_ValidConnection = false;
    delete m_pFDSConnection;
  }
}

FDSDataSource::~FDSDataSource()
{
  // Close out the FDS connection
  m_pFDSConnection->Close();
  delete m_pFDSConnection;
}

void FDSDataSource::OnIdle()
{
  DWORD dwResult;

  //---------------------Basic State Variables-------------------

  long _LatitudeAircraftLow;
  m_pFDSConnection->Read(0x0560, 4, &_LatitudeAircraftLow);

  long _LatitudeAircraftHigh;
  m_pFDSConnection->Read(0x0564, 4, &_LatitudeAircraftHigh);

  long _LongitudeAircraftLow;
  m_pFDSConnection->Read(0x0568, 4, &_LongitudeAircraftLow);
  
  long _LongitudeAircraftHigh;
  m_pFDSConnection->Read(0x056C, 4, &_LongitudeAircraftHigh);

  long _bank;
  m_pFDSConnection->Read(0x57c, 4, &_bank);

  long _pitch;
  m_pFDSConnection->Read(0x578, 4, &_pitch);

  long _tHeading;
  m_pFDSConnection->Read(0x580, 4, &_tHeading);

  short _MagneticVariation;
  m_pFDSConnection->Read(0x2A0, 2, &_MagneticVariation);
  
  long _tas;
  m_pFDSConnection->Read(0x2b8, 4, &_tas);
  
  long _ias;
  m_pFDSConnection->Read(0x2bc, 4, &_ias);

  long _Ground_Speed_K;
  m_pFDSConnection->Read(0x2b4, 4, &_Ground_Speed_K);

  long _fracMeters;
  m_pFDSConnection->Read(0x570, 4, &_fracMeters);
  
  long _unitMeters;
  m_pFDSConnection->Read(0x574, 4, &_unitMeters);

  long _Vertical_Speed_MS;
  m_pFDSConnection->Read(0x2c8, 4, &_Vertical_Speed_MS);

  long _Flaps_Deflection_Radians;
  m_pFDSConnection->Read(0x0bdc, 4, &_Flaps_Deflection_Radians);

  long _Flaps_TrueDeflection_Radians;
  m_pFDSConnection->Read(0x0BE0, 4, &_Flaps_TrueDeflection_Radians);

  long _Nose_Gear;
  m_pFDSConnection->Read(0x0bec, 4, &_Nose_Gear);

  long _Right_Gear;
  m_pFDSConnection->Read(0x0bf0, 4, &_Right_Gear);

  long _Left_Gear;
  m_pFDSConnection->Read(0x0bf4, 4, &_Left_Gear);

  unsigned short _Altimeter_Pressure;
  m_pFDSConnection->Read(0x0F48, 2, &_Altimeter_Pressure);

  unsigned short _Mach_Speed;
  m_pFDSConnection->Read(0x11C6, 2, &_Mach_Speed);

  short _TAT;
  m_pFDSConnection->Read(0x11D0, 2, &_TAT);


  //------------------------Nav Radio--------------------------

  unsigned short _Nav1_Freq;
  m_pFDSConnection->Read(0x0350, 2, &_Nav1_Freq);

  short _Nav1_Radial;
  m_pFDSConnection->Read(0x0c50, 2, &_Nav1_Radial);

  char _Nav1LocalizerNeedle;
  m_pFDSConnection->Read(0xc48, 1, &_Nav1LocalizerNeedle);

  short _Nav1_OBS;
  m_pFDSConnection->Read(0x0c4e, 2, &_Nav1_OBS);

  bool _Nav1GlideslopeAlive;
  m_pFDSConnection->Read(0xc4c, 1, &_Nav1GlideslopeAlive);

  char _Nav1GlideslopeNeedle;
  m_pFDSConnection->Read(0xc49, 1, &_Nav1GlideslopeNeedle);

  unsigned short _Nav2_Freq;
  m_pFDSConnection->Read(0x0352, 2, &_Nav2_Freq);

  short _Nav2_Radial;
  m_pFDSConnection->Read(0x0c60, 2, &_Nav2_Radial);

  char _Nav2LocalizerNeedle;
  m_pFDSConnection->Read(0xc59, 1, &_Nav2LocalizerNeedle);

  short _Nav2_OBS;
  m_pFDSConnection->Read(0x0c4e, 2, &_Nav2_OBS);

  short _ADF_Heading;
  m_pFDSConnection->Read(0x0c5e, 2, &_ADF_Heading);

  short Rstatus;
  m_pFDSConnection->Read(0x3300, 2, &Rstatus);

  //---------------------------ILS--------------------------------

  short _ILSInverseRunwayHeading;
  m_pFDSConnection->Read(0x0870, 2, &_ILSInverseRunwayHeading);
  
  short _ILSGlideslopeInclination;
  m_pFDSConnection->Read(0x0872, 2, &_ILSGlideslopeInclination);

  //------------------------Fuel----------------------------------

  long _Fuel_Centre_Level;
  m_pFDSConnection->Read(0x0B74, 4, &_Fuel_Centre_Level);

  long _Fuel_Left_Level;
  m_pFDSConnection->Read(0x0B7C, 4, &_Fuel_Left_Level);

  long _Fuel_Right_Level;
  m_pFDSConnection->Read(0x0B94, 4, &_Fuel_Right_Level);

  long _Fuel_Centre_Capacity;
  m_pFDSConnection->Read(0x0B78, 4, &_Fuel_Centre_Capacity);

  long _Fuel_Left_Capacity;
  m_pFDSConnection->Read(0x0B80, 4, &_Fuel_Left_Capacity);

  long _Fuel_Right_Capacity;
  m_pFDSConnection->Read(0x0B98, 4, &_Fuel_Right_Capacity);

  //------------------------Autopilot-----------------------------

  short _dirActive;
  m_pFDSConnection->Read(0x2ee0, 2, &_dirActive);

  double _dirPitch;
  m_pFDSConnection->Read(0x2ee8, 8, &_dirPitch);

  double _dirBank;
  m_pFDSConnection->Read(0x2ef0, 8, &_dirBank);

  short _Ref_Airspeed_Knots;
  m_pFDSConnection->Read(0x07E2, 4, &_Ref_Airspeed_Knots);


  //-------------------------------------------------------------
  short _Total_Air_Temperature;
  m_pFDSConnection->Read(0x11D0, 2, &_Total_Air_Temperature);

  //---------------------- Fuel Tanks----------------------------
  long _Fuel_Tank_Capacity_Center;
  m_pFDSConnection->Read(0x0B78, 4, &_Fuel_Tank_Capacity_Center);

  long _Fuel_Tank_Capacity_Left;
  m_pFDSConnection->Read(0x0B80, 4, &_Fuel_Tank_Capacity_Left);

  long _Fuel_Tank_Capacity_Right;
  m_pFDSConnection->Read(0x0B98, 4, &_Fuel_Tank_Capacity_Right);


  long _Fuel_Tank_Level_Center;
  m_pFDSConnection->Read(0x0B74, 4, &_Fuel_Tank_Level_Center);

  long _Fuel_Tank_Level_Left;
  m_pFDSConnection->Read(0x0B7C, 4, &_Fuel_Tank_Level_Left);

  long _Fuel_Tank_Level_Right;
  m_pFDSConnection->Read(0x0B94, 4, &_Fuel_Tank_Level_Right);

  //-------------------------Engines-----------------------------

  //-------------------------Engine 1----------------------------
  double _Engine1_N1;
  m_pFDSConnection->Read(0x2010, 8, &_Engine1_N1);

  double _Engine1_N2;
  m_pFDSConnection->Read(0x2018, 8, &_Engine1_N2);

  double _Engine1_FF_PPH;
  m_pFDSConnection->Read(0x2020, 8, &_Engine1_FF_PPH);

  double _Engine1_EPR;
  m_pFDSConnection->Read(0x2030, 8, &_Engine1_EPR);

  short _Engine1_EGT;
  m_pFDSConnection->Read(0x8be, 2, &_Engine1_EGT);

  // mdf
  char pp1[2];
  m_pFDSConnection->Read(0x08BA, 2, pp1);

  char pp2[2];
  m_pFDSConnection->Read(0x08B8, 2, pp2);

  short _Engine1_Oil_Pressure;
  m_pFDSConnection->Read(0x08BA, 2, &_Engine1_Oil_Pressure);

  short _Engine1_Oil_Temp;
  m_pFDSConnection->Read(0x08B8, 2, &_Engine1_Oil_Temp);

  short _Engine1_Oil_Quantity;
  m_pFDSConnection->Read(0x08D0, 2, &_Engine1_Oil_Quantity);

  long _Engine1_Vibration;
  m_pFDSConnection->Read(0x08D4, 4, &_Engine1_Vibration);

  long _Engine1_Hydraulic_Pressure;
  m_pFDSConnection->Read(0x08D8, 4, &_Engine1_Hydraulic_Pressure);

  long _Engine1_Hydraulic_Quantity;
  m_pFDSConnection->Read(0x08DC, 4, &_Engine1_Hydraulic_Quantity);

  //-------------------------Engine 2----------------------------
  double _Engine2_N1;
  m_pFDSConnection->Read(0x2110, 8, &_Engine2_N1);
  
  double _Engine2_N2;
  m_pFDSConnection->Read(0x2118, 8, &_Engine2_N2);

  double _Engine2_FF_PPH;
  m_pFDSConnection->Read(0x2120, 8, &_Engine2_FF_PPH);

  double _Engine2_EPR;
  m_pFDSConnection->Read(0x2130, 8, &_Engine2_EPR);

  short _Engine2_EGT;
  m_pFDSConnection->Read(0x8be, 2, &_Engine2_EGT);


  // mdf
  char pp3[2];
  m_pFDSConnection->Read(0x0952, 2, pp3);

  char pp4[2];
  m_pFDSConnection->Read(0x0950, 2, pp4);


  short _Engine2_Oil_Pressure;
  m_pFDSConnection->Read(0x0952, 2, &_Engine2_Oil_Pressure);

  short _Engine2_Oil_Temp;
  m_pFDSConnection->Read(0x0950, 2, &_Engine2_Oil_Temp);

  short _Engine2_Oil_Quantity;
  m_pFDSConnection->Read(0x0968, 2, &_Engine2_Oil_Quantity);

  long _Engine2_Vibration;
  m_pFDSConnection->Read(0x096C, 4, &_Engine2_Vibration);

  long _Engine2_Hydraulic_Pressure;
  m_pFDSConnection->Read(0x0970, 4, &_Engine2_Hydraulic_Pressure);

  long _Engine2_Hydraulic_Quantity;
  m_pFDSConnection->Read(0x0974, 4, &_Engine2_Hydraulic_Quantity);

  short autoPilotHeading;
  m_pFDSConnection->Read(0x7CC, 2, &autoPilotHeading);

  short adf1Heading;
  m_pFDSConnection->Read(0x0C6A, 2, &adf1Heading);

  m_pFDSConnection->Read(0x3044, 25, Adf1_Name);
  //--------------------------------------------------------------
   
  // Grab the requested data from FDS
  m_pFDSConnection->Process();

  //----------------Convert the data to a meaningful form------------------
  double _fLatitudeAircraft = _LatitudeAircraftHigh*90./10001750.;
  _fLatitudeAircraft += (_LatitudeAircraftLow*90./10001750)/(65536.*65536.);
  //Latitude = degreesToRadians(_fLatitudeAircraft);
  Latitude = _fLatitudeAircraft;

  double _fLongitudeAircraft = _LongitudeAircraftHigh*360./(65536.*65536.);
  _fLongitudeAircraft += (_LongitudeAircraftLow*360/(65536.*65536.))/(65536.*65536.);//./(65536.*65536.)
  //Longitude = degreesToRadians(_fLongitudeAircraft);
  Longitude = _fLongitudeAircraft;
  
  //Normalise autopilot heading
  Autopilot_Course_Heading= (int)autoPilotHeading * 360.0F / 65536.0F;
  if (Autopilot_Course_Heading> 360) {
  	while (Autopilot_Course_Heading> 360)
		Autopilot_Course_Heading-= 360.0F;
  }
  if (Autopilot_Course_Heading< 0) {
	while (Autopilot_Course_Heading< 0)
			Autopilot_Course_Heading+= 360.0F;
  }
  //ADF
	Adf1_Heading= (int)adf1Heading * 360.0F / 65536.0F;
	if (Adf1_Heading> 360) {
		while (Adf1_Heading> 360)
			Adf1_Heading-= 360.0F;
	}
	if (Adf1_Heading< 0) {
		while (Adf1_Heading< 0)
			Adf1_Heading+= 360.0F;
	}

  Bank = -360.0*( (double) _bank )/(65536.0*65536.0);
  Pitch = -360.0*( (double) _pitch )/(65536.0*65536.0);
  
  True_Heading = 360.0*((double)_tHeading)/(65536.0*65536.0);
  if (True_Heading < 0)
    True_Heading += 360;
  
  Mag_Variation = 360.0/65536.0*(double)_MagneticVariation;
  
  Mag_Heading = True_Heading - Mag_Variation;
  Mag_Heading = WrapHeading(Mag_Heading);
  
  Fuel_Tank_Level_Center = _Fuel_Centre_Level / 128.0 / 65536.0;
  Fuel_Tank_Level_Left = _Fuel_Left_Level / 128.0 / 65536.0;
  Fuel_Tank_Level_Right = _Fuel_Right_Level / 128.0 / 65536.0;

  Fuel_Tank_Capacity_Center = _Fuel_Centre_Capacity * 6.699219;
  Fuel_Tank_Capacity_Left = _Fuel_Left_Capacity * 6.699219;
  Fuel_Tank_Capacity_Right = _Fuel_Right_Capacity * 6.699219;

  Total_Air_Temperature = _TAT / 256.0;

  TAS = (double)_tas/128;
  IAS = (double)_ias/128;
  Ground_Speed_K = (double) _Ground_Speed_K / 65536 * 1.943844;
  Mach = _Mach_Speed / 20480.0;

  // Compute altitude
  // Note that we use the meters and fractional meters data, rather than
  // the barometric altitude in feet as computed by FDS (which seems
  // to update rather slowly)
  unsigned long fractionalMeters = (unsigned long)_fracMeters;
  long unitMeters = _unitMeters;

  // 4294967296 is 2^32, because the fractional unit is scaled to maximum
  // possible magnitude of an unsigned long int
  Barometric_Alt_Metres = (double) unitMeters + (double) fractionalMeters / 4294967296;
  Barometric_Alt_Feet = Barometric_Alt_Metres * 3.28084;

  Altimeter_Pressure = _Altimeter_Pressure / 16.0;

  Vertical_Speed_MS = (double) _Vertical_Speed_MS / 256;
  Vertical_Speed_FPM = (double) _Vertical_Speed_MS * 60 * 3.28084 / 256;

  Flaps_Deflection = (double) _Flaps_Deflection_Radians / 16383;
  Flaps_TrueDeflection = (double)  _Flaps_TrueDeflection_Radians / 16383;

  Nose_Gear = (double) _Nose_Gear / 16383;
  Right_Gear = (double) _Right_Gear / 16383;
  Left_Gear = (double) _Left_Gear / 16383;

  // Convert Nav1 frequency
  unsigned short nav1DigitA = _Nav1_Freq;
  unsigned short nav1DigitB = _Nav1_Freq;
  unsigned short nav1DigitC = _Nav1_Freq;
  unsigned short nav1DigitD = _Nav1_Freq;

  nav1DigitA = (nav1DigitA & 0xF000) >> 12;
  nav1DigitB = (nav1DigitB & 0x0F00) >> 8;
  nav1DigitC = (nav1DigitC & 0x00F0) >> 4;
  nav1DigitD = (nav1DigitD & 0x000F);

  Nav1_Freq = 100 + (float)nav1DigitA * 10 + (float)nav1DigitB * 1
    + (float)nav1DigitC * 0.1 + (float)nav1DigitD * 0.01;

  // Convert Nav2 frequency
  unsigned short nav2DigitA = _Nav2_Freq;
  unsigned short nav2DigitB = _Nav2_Freq;
  unsigned short nav2DigitC = _Nav2_Freq;
  unsigned short nav2DigitD = _Nav2_Freq;

  nav2DigitA = (nav2DigitA & 0xF000) >> 12;
  nav2DigitB = (nav2DigitB & 0x0F00) >> 8;
  nav2DigitC = (nav2DigitC & 0x00F0) >> 4;
  nav2DigitD = (nav2DigitD & 0x000F);

  Nav2_Freq = 100 + (float)nav2DigitA * 10 + (float)nav2DigitB * 1
    + (float)nav2DigitC * 0.1 + (float)nav2DigitD * 0.01;

  // Now compute the nav headings
  Nav1_Radial = (double) _Nav1_Radial * 360 / 65536;
  if (Nav1_Radial < 0)
    Nav1_Radial += 360.0;
  Nav1_OBS = (double) _Nav1_OBS;
  Nav1_Bearing = WrapHeading(WrapHeading(Nav1_Radial + 180.0) - Mag_Heading + 360.0);
  Nav1_Glideslope_Needle = (double) _Nav1GlideslopeNeedle / -128;
  Nav1_Localizer_Needle = (double) _Nav1LocalizerNeedle / 128;

  Nav2_Radial = (double) _Nav2_Radial * 360 / 65536;
  if (Nav2_Radial < 0)
    Nav2_Radial += 360.0;
  Nav2_OBS = (double) _Nav2_OBS;
  Nav2_Bearing = WrapHeading(WrapHeading(Nav2_Radial + 180.0) - Mag_Heading + 360.0);
  Nav2_Localizer_Needle = (double) _Nav2LocalizerNeedle / 128;

  ILS_Runway_Heading = (double) _ILSInverseRunwayHeading * 360 / 65536;
  ILS_Glideslope_Inclination = (double) _ILSGlideslopeInclination * 360 / 65536;
  ILS_Glideslope_Alive = _Nav1GlideslopeAlive;
  
  Director_Engaged = (bool) _dirActive;
  Director_Bank = -1*( (double) _dirBank );
  Director_Pitch = -1*( (double) _dirPitch );

  // statuses
  Nav1_Valid = (Rstatus / 2) % 2;
  Nav2_Valid = (Rstatus / 4) % 2;
  Adf1_Valid = (Rstatus / 8) % 2;
  Nav1_Has_DME = (Rstatus / 16) % 2;
  Nav2_Has_DME = (Rstatus / 32) % 2;
  //AP_Nav1_Radial_Aquired = (Rstatus / 64) % 2;
  //AP_ILS_Loc_Aquired = (Rstatus / 128) % 2;
  //AP_ILS_GS_Aquired = (Rstatus / 256) % 2;

  // autopilot 01110110
  Ref_Airspeed_Knots = (double)_Ref_Airspeed_Knots;

  // engine stuff
  N1[0] = _Engine1_N1;
  N2[0] = _Engine1_N2;
  EPR[0] = _Engine1_EPR;
  EGT[0] = 860 * (double)_Engine1_EGT/16384 * 9/5 + 32;
  Fuel_Flow_PPH[0] = _Engine1_FF_PPH;

  N1[1] = _Engine2_N1;
  N2[1] = _Engine2_N2;;
  EPR[1] = _Engine2_EPR;
  EGT[1] = 860 * (double)_Engine2_EGT/16384 * 9/5 + 32;
  Fuel_Flow_PPH[1] = _Engine2_FF_PPH;

  // mdf
  float _Engine1_OilPressure = ((BYTE)pp1[1] << 8) + (BYTE)pp1[0];
  float _Engine1_OilTemp = ((BYTE)pp2[1] << 8) + (BYTE)pp2[0];
  float _Engine2_OilPressure = ((BYTE)pp3[1] << 8) + (BYTE)pp3[0];
  float _Engine2_OilTemp = ((BYTE)pp4[1] << 8) + (BYTE)pp4[0];

  Oil_Pressure[0] = _Engine1_OilPressure / 16384 * 55;
  Oil_Pressure[1] = _Engine2_OilPressure / 16384 * 55;

  Oil_Temp_C[0] = _Engine1_OilTemp / 16384 * 140;
  Oil_Temp_C[1] = _Engine2_OilTemp / 16384 * 140;

  Oil_Pressure[0] = (double)_Engine1_Oil_Pressure / 16384 * 55;
  Oil_Pressure[1] = (double)_Engine2_Oil_Pressure / 16384 * 55;

  Oil_Temp_C[0] = (double)_Engine1_Oil_Temp / 16384 * 140;
  Oil_Temp_C[1] = (double)_Engine2_Oil_Temp / 16384 * 140;

  Oil_Quantity[0] = (double)_Engine1_Oil_Quantity / 16384 * 100;
  Oil_Quantity[1] = (double)_Engine2_Oil_Quantity / 16384 * 100;

  Engine_Vibration[0] = (double)_Engine1_Vibration / 16384 * 5;
  Engine_Vibration[1] = (double)_Engine2_Vibration / 16384 * 5;

  Engine_Hydraulic_Pressure[0] = (double)_Engine1_Hydraulic_Pressure / 16384 * 4;
  Engine_Hydraulic_Pressure[1] = (double)_Engine2_Hydraulic_Pressure / 16384 * 4;

  Engine_Hydraulic_Quantity[0] = (double)_Engine1_Hydraulic_Quantity / 16384 * 100;
  Engine_Hydraulic_Quantity[1] = (double)_Engine2_Hydraulic_Quantity / 16384 * 100;


  // fuel tank stuff
  Fuel_Tank_Capacity_Center = (double)_Fuel_Tank_Capacity_Center;
  Fuel_Tank_Capacity_Left = (double)_Fuel_Tank_Capacity_Left;
  Fuel_Tank_Capacity_Right = (double)_Fuel_Tank_Capacity_Right;

  Fuel_Tank_Level_Center = (double)_Fuel_Tank_Level_Center / 128 / 65536;
  Fuel_Tank_Level_Left = (double)_Fuel_Tank_Level_Left / 128 / 65536;
  Fuel_Tank_Level_Right = (double)_Fuel_Tank_Level_Right / 128 / 65536;

  // TAT
  Total_Air_Temperature = (int)_Total_Air_Temperature / 256;
}

double FDSDataSource::WrapHeading(double rawHead)
{
  if(rawHead < 0)
    return 360 - rawHead;

  if(rawHead >= 360)
    return rawHead - 360;

  return rawHead;

}

#else // We're not on a Win32 platform

namespace OpenGC
{

FDSDataSource::FDSDataSource()
{
  // Initialize the data
  this->InitializeData();

  m_ValidConnection = false;
}

FDSDataSource::~FDSDataSource()
{
}

void FDSDataSource::OnIdle()
{
}

#endif

} // end namespace OpenGC

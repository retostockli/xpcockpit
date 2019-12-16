/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcFSUIPCDerivedDataSource.cpp,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:00 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2003 Damion Shelton
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <stdio.h>
#include <math.h>
#include "ogcFSUIPCDerivedDataSource.h"

// First the code for the "real" version on Win32
#ifdef _WIN32

namespace OpenGC
{

FSUIPCDerivedDataSource::FSUIPCDerivedDataSource(bool connectionType)
{
  printf("oFSUIPCDerivedDataSource constructed\n");

  m_ConnectionType = connectionType;
  m_ValidConnection = false;
  m_FDSConnection = 0;

  //--------Initialization for FSUIPC-------------
  if(m_ConnectionType == 0)
  {
    char *pszErrors[] =
    {  "Okay",
      "Attempt to Open when already Open",
      "Cannot link to FSUIPC or WideClient",
      "Failed to Register common message with Windows",
      "Failed to create Atom for mapping filename",
      "Failed to create a file mapping object",
      "Failed to open a view to the file map",
      "Incorrect version of FSUIPC, or not FSUIPC",
      "Sim is not version requested",
      "Call cannot execute, link not Open",
      "Call cannot execute: no requests accumulated",
      "IPC timed out all retries",
      "IPC sendmessage failed all retries",
      "IPC request contains bad data",
      "Maybe running on WideClient, but FS not running on Server, or wrong FSUIPC",
      "Read or Write request cannot be added, memory for Process is full",
    };

    DWORD dwResult;

    // Try to open the FSUIPC connection
    if (FSUIPC_Open(SIM_ANY, &dwResult))
    {
      m_ValidConnection = true;
    }
    else
    {
      m_ValidConnection = false;
    }
  }
  else // Using an FDS connection
  {
    // Open the FDS connection
    m_FDSConnection = new CFDSConnection();

    // Try to open the FDS connection
    if( m_FDSConnection->Open() )
    {
      m_ValidConnection = true;
    }
    else
    {
      m_ValidConnection = false;
      delete m_FDSConnection;
    }
  }
}

FSUIPCDerivedDataSource::~FSUIPCDerivedDataSource()
{
  if(m_ConnectionType == 0)
  {
    // Close out the FSUIPC connection
    FSUIPC_Close();
  }
  else
  {
    // Close out the FDS connection
    m_FDSConnection->Close();
    delete m_FDSConnection;
  }
}

void
FSUIPCDerivedDataSource::
Read(DWORD offset, DWORD size, void *data)
{
  if(m_ConnectionType == 0)
  {
    DWORD dwResult;
    FSUIPC_Read(offset, size, data, &dwResult); 
  }
  else
  {
    m_FDSConnection->Read(offset, size, data);
  }
}

void
FSUIPCDerivedDataSource::
Process()
{
  if(m_ConnectionType == 0)
  {
    DWORD dwResult;
    FSUIPC_Process(&dwResult);
  }
  else
  {
    m_FDSConnection->Process();
  }
}

void
FSUIPCDerivedDataSource
::OnIdle()
{
  // Some paperwork involved with FSUIPC
  if(m_ConnectionType == 0)
  {
    DWORD dwResult;

    // We always try & open the connection if it's down.
    // This means you can start OpenGC before FSUIPC.
    if(!m_ValidConnection)
    {
    // Try to open the FSUIPC connection
      if (FSUIPC_Open(SIM_ANY, &dwResult))
      {
        m_ValidConnection = true;
      }
      else
        return;
    }
  }

  //
  //-------------Airframe data--------------
  //

  long _roll;
  this->Read(0x57c, 4, &_roll);

  long _pitch;
  this->Read(0x578, 4, &_pitch);

  long _tHeading;
  this->Read(0x580, 4, &_tHeading);

  short _MagneticVariation;
  this->Read(0x2A0, 2, &_MagneticVariation);

  long _LatitudeAircraftLow;
  this->Read(0x0560, 4, &_LatitudeAircraftLow);

  long _LatitudeAircraftHigh;
  this->Read(0x0564, 4, &_LatitudeAircraftHigh);

  long _LongitudeAircraftLow;
  this->Read(0x0568, 4, &_LongitudeAircraftLow);
  
  long _LongitudeAircraftHigh;
  this->Read(0x056C, 4, &_LongitudeAircraftHigh);
  
  long _ias;
  this->Read(0x2bc, 4, &_ias);

  long _tas;
  this->Read(0x2b8, 4, &_tas);

  unsigned short _Mach_Speed;
  this->Read(0x11C6, 2, &_Mach_Speed);

  long _Ground_Speed_K;
  this->Read(0x2b4, 4, &_Ground_Speed_K);

  long _Vertical_Speed_MS;
  this->Read(0x2c8, 4, &_Vertical_Speed_MS);

  long _fracMeters;
  this->Read(0x570, 4, &_fracMeters);
  
  long _unitMeters;
  this->Read(0x574, 4, &_unitMeters);

  long _Flaps_Deflection_Radians;
  this->Read(0x0bdc, 4, &_Flaps_Deflection_Radians);

  long _Flaps_TrueDeflection_Radians;
  this->Read(0x0BE0, 4, &_Flaps_TrueDeflection_Radians);

  long _Nose_Gear;
  this->Read(0x0bec, 4, &_Nose_Gear);

  long _Right_Gear;
  this->Read(0x0bf0, 4, &_Right_Gear);

  long _Left_Gear;
  this->Read(0x0bf4, 4, &_Left_Gear);

  unsigned short _Altimeter_Pressure;
  this->Read(0x0F48, 2, &_Altimeter_Pressure);

  short _TAT;
  this->Read(0x11D0, 2, &_TAT);


  //------------------------Nav Radio--------------------------

  unsigned short _Nav1_Freq;
  this->Read(0x0350, 2, &_Nav1_Freq);

  short _Nav1_Radial;
  this->Read(0x0c50, 2, &_Nav1_Radial);

  char _Nav1LocalizerNeedle;
  this->Read(0xc48, 1, &_Nav1LocalizerNeedle);

  short _Nav1_OBS;
  this->Read(0x0c4e, 2, &_Nav1_OBS);

  bool _Nav1GlideslopeAlive;
  this->Read(0xc4c, 1, &_Nav1GlideslopeAlive);

  char _Nav1GlideslopeNeedle;
  this->Read(0xc49, 1, &_Nav1GlideslopeNeedle);

  unsigned short _Nav2_Freq;
  this->Read(0x0352, 2, &_Nav2_Freq);

  short _Nav2_Radial;
  this->Read(0x0c60, 2, &_Nav2_Radial);

  char _Nav2LocalizerNeedle;
  this->Read(0xc59, 1, &_Nav2LocalizerNeedle);

  short _Nav2_OBS;
  this->Read(0x0c4e, 2, &_Nav2_OBS);

  short _ADF_Heading;
  this->Read(0x0c5e, 2, &_ADF_Heading);

  short Rstatus;
  this->Read(0x3300, 2, &Rstatus);

  //---------------------------ILS--------------------------------

  short _ILSInverseRunwayHeading;
  this->Read(0x0870, 2, &_ILSInverseRunwayHeading);
  
  short _ILSGlideslopeInclination;
  this->Read(0x0872, 2, &_ILSGlideslopeInclination);

  //------------------------Fuel----------------------------------

  long _Fuel_Centre_Level;
  this->Read(0x0B74, 4, &_Fuel_Centre_Level);

  long _Fuel_Left_Level;
  this->Read(0x0B7C, 4, &_Fuel_Left_Level);

  long _Fuel_Right_Level;
  this->Read(0x0B94, 4, &_Fuel_Right_Level);

  long _Fuel_Centre_Capacity;
  this->Read(0x0B78, 4, &_Fuel_Centre_Capacity);

  long _Fuel_Left_Capacity;
  this->Read(0x0B80, 4, &_Fuel_Left_Capacity);

  long _Fuel_Right_Capacity;
  this->Read(0x0B98, 4, &_Fuel_Right_Capacity);

  //------------------------Autopilot-----------------------------

  short _dirActive;
  this->Read(0x2ee0, 2, &_dirActive);

  double _dirPitch;
  this->Read(0x2ee8, 8, &_dirPitch);

  double _dirBank;
  this->Read(0x2ef0, 8, &_dirBank);

  short _Ref_Airspeed_Knots;
  this->Read(0x07E2, 4, &_Ref_Airspeed_Knots);


  //-------------------------------------------------------------
  short _Total_Air_Temperature;
  this->Read(0x11D0, 2, &_Total_Air_Temperature);

  //---------------------- Fuel Tanks----------------------------
  long _Fuel_Tank_Capacity_Center;
  this->Read(0x0B78, 4, &_Fuel_Tank_Capacity_Center);

  long _Fuel_Tank_Capacity_Left;
  this->Read(0x0B80, 4, &_Fuel_Tank_Capacity_Left);

  long _Fuel_Tank_Capacity_Right;
  this->Read(0x0B98, 4, &_Fuel_Tank_Capacity_Right);

  long _Fuel_Tank_Level_Center;
  this->Read(0x0B74, 4, &_Fuel_Tank_Level_Center);

  long _Fuel_Tank_Level_Left;
  this->Read(0x0B7C, 4, &_Fuel_Tank_Level_Left);

  long _Fuel_Tank_Level_Right;
  this->Read(0x0B94, 4, &_Fuel_Tank_Level_Right);

  //-------------------------Engines-----------------------------

  //-------------------------Engine 1----------------------------
  double _Engine1_N1;
  this->Read(0x2010, 8, &_Engine1_N1);

  double _Engine1_N2;
  this->Read(0x2018, 8, &_Engine1_N2);

  double _Engine1_FF_PPH;
  this->Read(0x2020, 8, &_Engine1_FF_PPH);

  double _Engine1_EPR;
  this->Read(0x2030, 8, &_Engine1_EPR);

  short _Engine1_EGT;
  this->Read(0x8be, 2, &_Engine1_EGT);

  // mdf
  char pp1[2];
  this->Read(0x08BA, 2, pp1);

  char pp2[2];
  this->Read(0x08B8, 2, pp2);

  short _Engine1_Oil_Pressure;
  this->Read(0x08BA, 2, &_Engine1_Oil_Pressure);

  short _Engine1_Oil_Temp;
  this->Read(0x08B8, 2, &_Engine1_Oil_Temp);

  short _Engine1_Oil_Quantity;
  this->Read(0x08D0, 2, &_Engine1_Oil_Quantity);

  long _Engine1_Vibration;
  this->Read(0x08D4, 4, &_Engine1_Vibration);

  long _Engine1_Hydraulic_Pressure;
  this->Read(0x08D8, 4, &_Engine1_Hydraulic_Pressure);

  long _Engine1_Hydraulic_Quantity;
  this->Read(0x08DC, 4, &_Engine1_Hydraulic_Quantity);

  //-------------------------Engine 2----------------------------
  double _Engine2_N1;
  this->Read(0x2110, 8, &_Engine2_N1);
  
  double _Engine2_N2;
  this->Read(0x2118, 8, &_Engine2_N2);

  double _Engine2_FF_PPH;
  this->Read(0x2120, 8, &_Engine2_FF_PPH);

  double _Engine2_EPR;
  this->Read(0x2130, 8, &_Engine2_EPR);

  short _Engine2_EGT;
  this->Read(0x8be, 2, &_Engine2_EGT);


  // mdf
  char pp3[2];
  this->Read(0x0952, 2, pp3);

  char pp4[2];
  this->Read(0x0950, 2, pp4);

  short _Engine2_Oil_Pressure;
  this->Read(0x0952, 2, &_Engine2_Oil_Pressure);

  short _Engine2_Oil_Temp;
  this->Read(0x0950, 2, &_Engine2_Oil_Temp);

  short _Engine2_Oil_Quantity;
  this->Read(0x0968, 2, &_Engine2_Oil_Quantity);

  long _Engine2_Vibration;
  this->Read(0x096C, 4, &_Engine2_Vibration);

  long _Engine2_Hydraulic_Pressure;
  this->Read(0x0970, 4, &_Engine2_Hydraulic_Pressure);

  long _Engine2_Hydraulic_Quantity;
  this->Read(0x0974, 4, &_Engine2_Hydraulic_Quantity);

  short autoPilotHeading;
  this->Read(0x7CC, 2, &autoPilotHeading);

  short adf1Heading;
  this->Read(0x0C6A, 2, &adf1Heading);

  //--------------------------------------------------------------
   
  // Grab the requested data from the sim
  this->Process();

  
  
  
  //----------------Convert the data to a meaningful form--------------

  //
  //-------------Airframe data--------------
  //
  
  double Roll = -360.0*( (double) _roll )/(65536.0*65536.0);
  this->GetAirframe()->InternalSetRoll(Roll);

  double Pitch = -360.0*( (double) _pitch )/(65536.0*65536.0);
  this->GetAirframe()->InternalSetPitch(Pitch);
  
  double True_Heading = 360.0*((double)_tHeading)/(65536.0*65536.0);
  if (True_Heading < 0)
    True_Heading += 360;
  this->GetAirframe()->InternalSetTrue_Heading(True_Heading);
  
  double Mag_Variation = 360.0/65536.0*(double)_MagneticVariation;
  this->GetAirframe()->InternalSetMag_Variation(Mag_Variation);
  
  double Mag_Heading = True_Heading - Mag_Variation;
  Mag_Heading = WrapHeading(Mag_Heading);
  this->GetAirframe()->InternalSetMag_Heading(Mag_Heading);

  double _fLatitudeAircraft = _LatitudeAircraftHigh*90./10001750.0;
  _fLatitudeAircraft += (_LatitudeAircraftLow*90./10001750)/(65536.*65536.);
  this->GetAirframe()->InternalSetLatitude(_fLatitudeAircraft);

  double _fLongitudeAircraft = _LongitudeAircraftHigh*360./(65536.*65536.);
  _fLongitudeAircraft += (_LongitudeAircraftLow*360/(65536.*65536.))/(65536.*65536.);
  this->GetAirframe()->InternalSetLongitude(_fLongitudeAircraft);

  this->GetAirframe()->InternalSetIAS( (double)_ias/128 );

  this->GetAirframe()->InternalSetTAS( (double)_tas/128 );
  
  this->GetAirframe()->InternalSetMach ( _Mach_Speed / 20480.0 );

  this->GetAirframe()->InternalSetGround_Speed_K (( double) _Ground_Speed_K / 65536 * 1.943844);
  

  // Compute altitude
  // Note that we use the meters and fractional meters data, rather than
  // the barometric altitude in feet (which seems to update rather slowly)
  unsigned long fractionalMeters = (unsigned long)_fracMeters;
  long unitMeters = _unitMeters;

  // 4294967296 is 2^32, because the fractional unit is scaled to maximum
  // possible magnitude of an unsigned long int
  double Pressure_Alt_Metres = (double) unitMeters + (double) fractionalMeters / 4294967296;
  this->GetAirframe()->InternalSetPressure_Alt_Metres(Pressure_Alt_Metres);
  
  this->GetAirframe()->InternalSetPressure_Alt_Feet(Pressure_Alt_Metres * 3.28084);

  this->GetAirframe()->InternalSetAltimeter_Pressure(_Altimeter_Pressure / 16.0);

  this->GetAirframe()->InternalSetVertical_Speed_MS( (double) _Vertical_Speed_MS / 256 );
  
  this->GetAirframe()->InternalSetVertical_Speed_FPM( (double) _Vertical_Speed_MS * 60 * 3.28084 / 256 );

  /*
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
  
  Fuel_Tank_Level_Center = _Fuel_Centre_Level / 128.0 / 65536.0;
  Fuel_Tank_Level_Left = _Fuel_Left_Level / 128.0 / 65536.0;
  Fuel_Tank_Level_Right = _Fuel_Right_Level / 128.0 / 65536.0;

  Fuel_Tank_Capacity_Center = _Fuel_Centre_Capacity * 6.699219;
  Fuel_Tank_Capacity_Left = _Fuel_Left_Capacity * 6.699219;
  Fuel_Tank_Capacity_Right = _Fuel_Right_Capacity * 6.699219;

  Total_Air_Temperature = _TAT / 256.0;

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
  */
}

double FSUIPCDerivedDataSource::WrapHeading(double rawHead)
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

FSUIPCDerivedDataSource::FSUIPCDerivedDataSource(bool connectionType)
{
  m_ValidConnection = false;
}

FSUIPCDerivedDataSource::~FSUIPCDerivedDataSource()
{
}

void FSUIPCDerivedDataSource::OnIdle()
{
}

#endif

} // end namespace OpenGC

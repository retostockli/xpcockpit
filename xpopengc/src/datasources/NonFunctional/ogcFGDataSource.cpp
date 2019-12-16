/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcFGDataSource.cpp,v $

  Copyright (C) 2001-2 by:
    Original author:
      John Wojnaroski
    Contributors (in alphabetical order):
      Damion Shelton

  Last modification:
    Date:      $Date: 2004/10/14 19:27:58 $
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

/*
  Sets up the data class structure and methods for receiving a UDP data
  packet transmitted by the simulation program. Uses the Plib sockets
  The port number may be changed if required.

  This program acts as a "server" receiving data from the flight simulation
*/

#include <stdio.h>
#include <iostream>
#include "ogcFGDataSource.h"
//#include <simgear/constants.h>

namespace OpenGC
{

// Constructor
FGDataSource::FGDataSource()
{ 
  printf("FGDataSource constructed\n");

  // Initialize the data, but we're going to delay opening the sockets until all the other
  // data files and command line arguments have been read
  this->InitializeData();

  // Set FlightGear buffer length is
  m_BufferLength = sizeof(FGData);

  printf("Size of FGData is %i\n", sizeof(FGData) );

  // Create the buffer
  m_Buffer = new char[m_BufferLength];

  // Temp buffer used to extract message
  m_TempMsg = new char[m_BufferLength];

  // hard code the port number
  m_ReceivePort = 5800;
}

FGDataSource::~FGDataSource()
{
  delete m_Buffer;
  delete m_TempMsg;
}

bool FGDataSource::Open()
{
  // The host who's sending data
  char m_Host[256] = "";

  // Must call this before any other net stuff
  netInit(0,0);

  // We'll assume the connection is valid until proved otherwise
  m_ValidConnection = true;

  // Try to open a socket
  if ( ! m_Socket.open( false ) )
  {
    printf("FlightGear data source: error opening socket\n");
	  m_ValidConnection = false;
    return false;
  }

  m_Socket.setBlocking( false );

  if ( m_Socket.bind( m_Host, m_ReceivePort ) == -1 )
  {
	  printf("FlightGear data source: error binding to port %d\n", m_ReceivePort);
	  m_ValidConnection = false;
    return false;
  }

  return true;
}

bool FGDataSource::GetData()
{
  if (!m_ValidConnection)
    return false;

  // Length of the message received from Flightgear
  int receivedLength = 0;

  // Actual length after flushing accumulated messages
  int finalReceivedLength = 0;

  // The code in the "do" block flushes the buffer so that only
  // the most recent message remains. This eliminates the build-up
  // of old messages in the network buffer (which we don't directly
  // control)
  do
  {
    receivedLength = m_Socket.recv(m_TempMsg, m_BufferLength, 0);

    if(receivedLength >= 0)
    {
      for(int i = 0; i < m_BufferLength; i++)
      {
        m_Buffer[i] = m_TempMsg[i];
      }

      finalReceivedLength = receivedLength;
    }
  }while(receivedLength >=0);
  
  // At this point, m_Buffer[] contains the most recent message
  if(finalReceivedLength>0)
  {
    //memcpy( &m_FDM, &m_Buffer, m_BufferLength );
    m_FDM = (FGData*) m_Buffer;
    //printf("Received a flightgear packet of length %i\n", finalReceivedLength);
    return true;
  }
  else
  {
    //printf("Uh-oh, final received length in Flightgear was 0\n");
    return false;
  }
}

void FGDataSource::Close()
{
  // dummy function for now
}

void FGDataSource::OnIdle()
{
  #define SGD_RAD_TO_DEGREES 57.29578

  // Abort if the connection isn't valid
  if( !m_ValidConnection )
    return;

  // Input data from the LAN socket
  if ( !GetData() )
    return;

  //printf("Size of FGData object %i\n", sizeof(FGData) );

  // We made it! There must be data to process

  // keep in sync with FG changes
  // Version_ID = m_FDM->version_id;
  // printf("Received version is %i\n", m_FDM->version_id);


  // NOTE: We have to transfer the FG Datasource to the newly developed OpenGC Airframe Data Container
  // This goes like this:
  // Previous :
  //  Latitude = m_FDM->latitude;
  // New:
  this->GetAirframe()->InternalSetLatitude(m_FDM->latitude);

  // Now continue for the data elements below:
  Longitude = m_FDM->longitude;
  Mag_Variation= m_FDM->magvar * SGD_RAD_TO_DEGREES;
    
  Bank = m_FDM->bank;
  Pitch = m_FDM->pitch;
 
  True_Heading = m_FDM->heading;
  Mag_Heading = m_FDM->heading - Mag_Variation;
  if (Mag_Heading < 0.0 )
    Mag_Heading += 359.9;

  Barometric_Alt_Feet = m_FDM->altitude;
  Radio_Alt_Feet = m_FDM->altitude_agl;
  
  IAS = m_FDM->v_kcas; // calibrated airspeed
  Mach = m_FDM->mach;
  //EAS = m_FDM->v_eas; // equivalent airspeed
  //TAS = EAS * (1.0 + (Barometric_Alt_Feet * 0.000017));
  //= TAS - (cos((Mag_Heading - wind_direction)/57.29456) * wind_velocity);
  
  Vertical_Speed_FPS = m_FDM->vvi; // FlightGear units are feet per second
  Vertical_Speed_FPM = Vertical_Speed_FPS * 60.0;

  //Ground_Speed_K = m_FDM->groundspeed * 3600 * SG_FEET_TO_METER * SG_METER_TO_NM;;
  Ground_Speed_K = m_FDM->groundspeed * 3600 * 3.2808399 * 1852;
    
  // Get euler angles and rates
  Alpha = m_FDM->alpha;
  Alpha_Dot = m_FDM->alpha_dot;
  Beta = m_FDM->beta;
  Beta_Dot = m_FDM->beta_dot;
  Phi_Dot = m_FDM->phi_dot;
  Psi_Dot = m_FDM->psi_dot;
  Theta_Dot = m_FDM->theta_dot;  

  // get the position of the control surfaces
  Flaps_Deflection = m_FDM->flaps;      
  Elevator_Deflection = m_FDM->elevator;
  Right_Aileron_Deflection = m_FDM->right_aileron;
  Left_Aileron_Deflection = m_FDM->left_aileron;
  Rudder_Deflection = m_FDM->rudder;
        
  // get the position of the engine controls
  Throttle[0] = m_FDM->throttle[0];
  Throttle[1] = m_FDM->throttle[1];
  Mixture[0] = m_FDM->mixture[0];
  Mixture[1] = m_FDM->mixture[1];
  Prop_Advance[1] = m_FDM->prop_advance[1];
  Prop_Advance[0] = m_FDM->prop_advance[0];

  // For single and twin engine aircraft, use the first and second elements of each set
  N1[0] = m_FDM->n1_turbine[0];
  N1[1] = m_FDM->n1_turbine[1];
  N1[2] = m_FDM->n1_turbine[2];
  N1[3] = m_FDM->n1_turbine[3];

  N2[0] = m_FDM->n2_turbine[0];
  N2[1] = m_FDM->n2_turbine[1];
  N2[2] = m_FDM->n2_turbine[2];
  N2[3] = m_FDM->n2_turbine[3];

  EGT[0] = m_FDM->egt[0];
  EGT[1] = m_FDM->egt[1];
  EGT[2] = m_FDM->egt[2];
  EGT[3] = m_FDM->egt[3];  

  EPR[0] = m_FDM->epr[0];
  EPR[1] = m_FDM->epr[1];
  EPR[2] = m_FDM->epr[2];
  EPR[3] = m_FDM->epr[3];

  Fuel_Flow_PPH[0] = m_FDM->fuel_flow[0];
  Fuel_Flow_PPH[1] = m_FDM->fuel_flow[1];  
  Fuel_Flow_PPH[2] = m_FDM->fuel_flow[2];
  Fuel_Flow_PPH[3] = m_FDM->fuel_flow[3];
  
  Oil_Pressure[0] = m_FDM->oil_pressure[0];
  Oil_Pressure[1] = m_FDM->oil_pressure[1];
  Oil_Pressure[2] = m_FDM->oil_pressure[2];
  Oil_Pressure[3] = m_FDM->oil_pressure[3];
  
  Nose_Gear = m_FDM->gear_nose;
  Left_Gear = m_FDM->gear_left;
  Right_Gear = m_FDM->gear_right;
  Left_Body_Gear = m_FDM->gear_left_rear;
  Right_Body_Gear = m_FDM->gear_right_rear;
}

} // end namespace OpenGC

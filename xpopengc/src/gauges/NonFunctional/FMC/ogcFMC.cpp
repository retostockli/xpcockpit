/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcFMC.cpp,v $

  Copyright (C) 2001-2 by:
    Original author:
      John Wojnaroski
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2003/04/18 06:00:26 $
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
This program is currently designed to interface with the FlightGear simulation via a TCP/IP
connection over a LAN. The specific control gains and functions are currently set to work with
the YASIM 747 model modified. These may or maynot work with other flight models and 
class aircraft models running in the JSBSim flight dynamics model. In order to use and setup 
properly you need to know somethings about LANs and sockets.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ogcFMC.h"
#include <Navigation/ogcNavigate.h>
#include <DataSources/ogcDataSource.h>
#include <iostream>

#define DOWN	TRUE;
#define UP		FALSE;

double	trim, fpa;
double max_bank;
double prop_loc;

ogcDataSource* exec_pData;

int			tcp_sockfd = 0;
int			udp_sockfd = 0;
int			result, servlen;
struct sockaddr_in	tcp_serv_addr, tcp_cli_addr;
struct sockaddr		*p_serv_addr;	


ogcFMC::ogcFMC()
{
  exec_pData = m_pDataSource;
 
  exec_pData->FMC_State = 0; // Everything is off
  ctrls.version = 6; // this is the current version supported by FG

// Initialize all the control variables and commands
  ctrls.rudder = 0.0;
  ctrls.elevator = 0.0;
  ctrls.aileron = 0.0;
  ctrls.elevator_trim = 0.0;
  ctrls.num_engines = 4;
  ctrls.num_wheels = 3;
  ctrls.flaps = 0.0;
  ctrls.throttle[0] = ctrls.throttle[1] = 0.0;
  ctrls.mixture[0] = ctrls.mixture[1] = 1.0;
  ctrls.prop_advance[0] = ctrls.prop_advance[1] = 1.0;
  ctrls.brake[0] = ctrls.brake[1] = ctrls.brake[2] = 0.0;
  ctrls.Gear = DOWN;

  exec_pData->Cmd_Vertical_Speed_FPS = 0.0;
  exec_pData->Rudder_Trim = 0.0;
  trim = 0.0;
  fpa = 0.0;
  max_bank = 15.0;

  printf("ogcFMC executive constructed\n");
}

ogcFMC::~ogcFMC()
{
}

void ogcFMC::FMC_throttle_ctrl( float throttle )
{	
	if ( exec_pData->Cmd_Airspeed_Hold ) {
		double err_sum;
		//exec_pData->AFDS_state = (exec_pData->AFDS_state & 0xF0FF) | 0x0500;
		exec_pData->Cmd_Airspeed_Knots = exec_pData->Ref_Airspeed_Knots;
		double speed_err = exec_pData->Cmd_Airspeed_Knots - exec_pData->IAS;
		err_sum += speed_err;
		if ( err_sum > 2000.0 ) err_sum = 2000.0;
		if ( err_sum < -2000.0 ) err_sum = -2000.0;
		//double int_adj = err_sum * 0.005;
		//double prop_adj = 0.5 + speed_err * 0.10;
		double int_adj, prop_adj;
		exec_pData->Ref_Throttle[0] = 0.9 * (0.5 + speed_err * 0.10) + 0.1 * (err_sum * 0.005);
		
// Provide speed control in any state if SPD active mode	
		exec_pData->Cmd_Throttle[0] = exec_pData->Ref_Throttle[0];
		exec_pData->Cmd_Throttle[1] = exec_pData->Ref_Throttle[0];
		exec_pData->Cmd_Throttle[2] = exec_pData->Ref_Throttle[0];
		exec_pData->Cmd_Throttle[3] = exec_pData->Ref_Throttle[0];	
		} else {
		exec_pData->Cmd_Throttle[0] = 0.4 - throttle;
		exec_pData->Cmd_Throttle[1] = 0.4 - throttle;
		exec_pData->Cmd_Throttle[2] = 0.4 - throttle;
		exec_pData->Cmd_Throttle[3] = 0.4 - throttle;
		}
		// Okay, we have command throttle settings. Later we might change these to
		// reference settings to handle other modes like VNAV with THR HOLD
}

void ogcFMC::Render()
{
// Turn on the FMC and start processing
float *axis = exec_pData->update_joystick();
// This is a test mode for debugging only
	if ( exec_pData->FMC_State == 1 ) {
		pNav->update();
		}
// Okay, looks like we want to fly		
	if ( exec_pData->FMC_State == 2 ) init();
	
// FMC computes reference control values.these become commands and override any 
// manual inputs if AFDS mode is active as selected by MCP
	pNav->update();
  FMC_autopilot();
	FMC_throttle_ctrl( axis[3] );
	
// This is the minimum flight mode. All controls are manual with no SAS or A/P
	if ( exec_pData->FMC_State == 3 ) {
		pNav->update();
	
		// This is the manual mode with no SAS 
		ctrls.elevator = -axis[1];
		ctrls.aileron = axis[0];
		}
	// Okay at State 4 we'll turn on some stab aug capability
	if ( exec_pData->FMC_State == 4 ) {
  	ctrls.aileron = axis[0];
  	ctrls.elevator = -axis[1] + exec_pData->Theta_Dot * 0.01;
  	}
  	// This is the attitude rate mode	
  if ( exec_pData->FMC_State == 5 ) {
  	if (( axis[0] < -0.010 ) || ( axis[0] > 0.010 )) 
			exec_pData->Cmd_Bank += axis[0];
		if (( axis[1] < -0.010 ) || ( axis[1] > 0.010 )) 
			exec_pData->Cmd_Pitch += axis[1];
  	}	
  	
  // Okay the autopilot is on let George drive the control input
	if ( exec_pData->FMC_State == 6 ) {
		exec_pData->Cmd_Bank = exec_pData->Ref_Bank;
		exec_pData->Cmd_Pitch = exec_pData->Ref_Pitch;		
		}				

	if ( exec_pData->FMC_State > 2 ) send_control();
}

// Check for LAN socket connections and initialize TCP sockets if connected
void ogcFMC::init()
{
  // Test for UDP packet on LAN. If true configure TCP connection
  //printf("Version is %d\n", exec_pData->Version_ID);
  if ( exec_pData->Version_ID == 3 ) tcp_sockfd = open_tcp_socket();
}

// Functions to initilaize and open tcp/udp sockets. Open TCP only if a UDP packet 
// indicates that the flight sim is active
int ogcFMC::open_tcp_socket(void)
{
  servlen = sizeof( tcp_serv_addr );	
  bzero((char *) &tcp_serv_addr, servlen);
  tcp_serv_addr.sin_family = AF_INET;
  tcp_serv_addr.sin_addr.s_addr = inet_addr(exec_pData->Flt_Sim_Address);
  tcp_serv_addr.sin_port = htons(exec_pData->TCP_Port);

  if ((tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
		printf("Failed to obtain a socket\n");
		return( 0 );
		}
  if (connect(tcp_sockfd, (struct sockaddr *) &tcp_serv_addr, sizeof(tcp_serv_addr)) < 0 ) 
		{
		printf("Failed to connect to server\n");
		return( 0 );
		}
  printf("Connected to TCP server\n");
  exec_pData->FMC_State = 3;
  return tcp_sockfd;
}

// Calculate command inputs if any autopilot is engaged, called from control method
void ogcFMC::FMC_autopilot()
{
double delta_hdg;
double	alt_err;
double total_err_loc;
double int_err;
 
// Now check each commanded mode that's active and calculate control commands
if (exec_pData->Cmd_Altitude_Hold) {
		exec_pData->Cmd_Altitude_Feet = exec_pData->Ref_Altitude_Feet;
		alt_err = exec_pData->Cmd_Altitude_Feet - exec_pData->Barometric_Alt_Feet;
		// set the climb rate to a default value
		if ( alt_err > 0.0 ) exec_pData->Cmd_Vertical_Speed_FPS = 10.0;
  				else exec_pData->Cmd_Vertical_Speed_FPS = -5.0;				
  							
  	if ( fabs(alt_err * 0.50) <  fabs( exec_pData->Cmd_Vertical_Speed_FPS) ) {
					exec_pData->Cmd_Vertical_Speed_Hold = false;
					exec_pData->Vert_Spd_Tag = 0;
					exec_pData->Cmd_Vertical_Speed_FPS = alt_err * 0.50;
					exec_pData->AFDS_state = (exec_pData->AFDS_state & 0xFFF0) | 0x0008;
					}
		}							
 				
	if ( exec_pData->Cmd_Vertical_Speed_Hold ) {
		exec_pData->AFDS_state = (exec_pData->AFDS_state & 0xFFF0) | 0x0007;
  	exec_pData->Cmd_Vertical_Speed_FPS = exec_pData->Ref_Vertical_Speed * 0.016667;
  	trim = exec_pData->Cmd_Vertical_Speed_FPS - exec_pData->Vertical_Speed_FPS;	
  	trim = 0.0;	
		}
		
	// avoid divide by zero
		if (exec_pData->TAS > 30.0) {
			exec_pData->Ref_Pitch = exec_pData->Alpha * 57.29578 +
			(asin( exec_pData->Cmd_Vertical_Speed_FPS / (exec_pData->TAS * 1.66667)) ) * 57.29578;
			}
	// *********
	// OKAY!! We have a reference pitch angle
	// **************
		
	if ( exec_pData->Mag_Heading >= 360.0 ) exec_pData->Mag_Heading = exec_pData->Mag_Heading - 360.0;
  if ( exec_pData->Mag_Heading < 0.0 ) exec_pData->Mag_Heading = exec_pData->Mag_Heading + 360.0;
	
	// Checking for a localizer mode to calculate a reference heading
	if ( exec_pData->Localizer_Hold ) {
		// check if valid ILS and the localizer has been captured 
		// you might want to consider setting heading hold mode true at this point
		if ( exec_pData->Nav1_Valid == 2) {
			double diff = exec_pData->Mag_Heading - exec_pData->Nav1_Radial;
			if ( fabs( diff ) < 45.0 ) {			
				if ( fabs(exec_pData->Nav1_Course_Indicator) < 15.0 ) {
					//max_bank = 10.0;
					exec_pData->AFDS_armed = exec_pData->AFDS_armed & 0xFF0F;
					exec_pData->AFDS_state = (exec_pData->AFDS_state & 0xFF0F) | 0x0050;
				// now calculate the course error and set all to inbound course
					double err_sum_loc;
					err_sum_loc += exec_pData->Nav1_Course_Indicator;
					if ( err_sum_loc > 2000.0 ) err_sum_loc = 2000.0;
					if ( err_sum_loc < -2000.0 ) err_sum_loc = -2000.0;
					double int_loc = err_sum_loc * 0.005;
					prop_loc = exec_pData->Nav1_Course_Indicator * 3.0;
					total_err_loc = 0.95 * prop_loc + 0.05 * int_loc;
					exec_pData->Ref_Heading = exec_pData->Nav1_Radial + total_err_loc;
//printf("SUM: %f Int: %f Prop: %f Tot: %f CDI: %f\n", err_sum_loc, int_loc, prop_loc,
					//total_err_loc, exec_pData->Nav1_Course_Indicator );						
					}
			}
		}
	}
	
	if ( exec_pData->Cmd_Heading_Hold ) {
		exec_pData->Cmd_Heading = exec_pData->Ref_Heading;
	// Calculate if a change in heading is required
		exec_pData->Cmd_Heading = exec_pData->Ref_Heading;
		delta_hdg = exec_pData->Mag_Heading - exec_pData->Cmd_Heading;
  // determine shortest direction to turn
  	if ( delta_hdg > 180.0 ) delta_hdg -= 360.0;
  	if ( delta_hdg < -180.0 ) delta_hdg += 360.0;
  // let's assume we are inside max bank degrees of command heading
  	exec_pData->Ref_Bank = -delta_hdg;
  // if not set the max bank limit
  	if ( delta_hdg < -max_bank ) // turn right
			{
			exec_pData->Ref_Bank = max_bank;	
			}
  	else if ( delta_hdg > max_bank) // turn left
			{
			exec_pData->Ref_Bank = -max_bank;	
			}
	}
	// *********
	// OKAY!! We have a reference bank angle
	// **************
	
	// Compute bank and pitch steering commands. 
	exec_pData->Director_Bank = exec_pData->Ref_Bank - exec_pData->Bank;
	exec_pData->Director_Pitch = exec_pData->Ref_Pitch - exec_pData->Pitch; 
}

// Send controls and commands to flight sim program
void ogcFMC::send_control()
{
	// If state 5 or 6 convert pitch and bank commands into surface controls
	if ( exec_pData->FMC_State > 4) {
	
// Calculate the command for elevator
  ctrls.elevator =  exec_pData->Elevator_Deflection +
 	(exec_pData->Pitch - exec_pData->Cmd_Pitch) * 0.006152 + (exec_pData->Theta_Dot * 0.5773)
			+ (exec_pData->Alpha_Dot * 0.00152) - (trim * 0.0065);
	
// Now determine amount of aileron to apply
  ctrls.aileron = (exec_pData->Cmd_Bank - exec_pData->Bank) * 0.0754 - (exec_pData->Phi_Dot * 0.736);
	} 	
// Apply rudder as required to maintain coordinated flight (ie beta = 0)
 // Treat this as a SAS that is always "on"	We'll add rudder trim later		
	ctrls.rudder = (0.006 * exec_pData->Beta_Dot) + (0.017 * exec_pData->Beta);	
	
	ctrls.throttle[0] = exec_pData->Cmd_Throttle[0];
  ctrls.throttle[1] = exec_pData->Cmd_Throttle[1];
  ctrls.throttle[2] = exec_pData->Cmd_Throttle[2];
  ctrls.throttle[3] = exec_pData->Cmd_Throttle[3];
  
  ctrls.flaps = exec_pData->Commanded_Flaps_Deflection;
  ctrls.Gear = exec_pData->Gear_Handle;
  ctrls.mixture[0] = ctrls.mixture[1] = 0.9;
	ctrls.prop_advance[0] = ctrls.prop_advance[1] = 0.85;			

	length = sizeof( ctrls );
	if (sendto(tcp_sockfd, (char *) (& ctrls), length, 0, (struct sockaddr *) &tcp_serv_addr, servlen ) != length) {
				exit( -1 );
				}
}

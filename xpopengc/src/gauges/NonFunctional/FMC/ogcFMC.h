/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcFMC.h,v $

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

#include <Base/ogcGauge.h>

class ogcFGControls
{
public:
   // Flight Control surface/engine parameters to send back to the simulation

    int version;		         // increment when data values change

    // Controls
    double aileron;		         // -1 ... 1
    double elevator;		         // -1 ... 1
    double elevator_trim;	         // -1 ... 1
    double rudder;		         // -1 ... 1
    double flaps;		         //  0 ... 1
    
    int num_engines;
    int	magnetos[4];
    bool	starter[4];
    double throttle[4];     //  0 ... 1
    double mixture[4];      //  0 ... 1
    double prop_advance[4]; //  0 ... 1
    
    int num_wheels;
    double brake[3];         //  0 ... 1
    
    bool	 Gear;

    // Other interesting/useful values
    double hground;		         // ground elevation (meters)
    double magvar;		// local mag variation
    int speedup;		// integer speedup
   
};

class ogcFMC : public ogcGauge
{

    ogcFGControls ctrls;

    int length;

public:
	ogcFMC();
	~ogcFMC();

  	void Render();

	// Check for a LAN connection and run test for a TCP server
  	void init(void);

	// Open a TCP connection with the flight sim
		int open_tcp_socket(void);
		
	// Throttle control
		void FMC_throttle_ctrl(float throttle);

	//  Get an update of current position
		void navigate();

	// If any autopilot is engaged calculate control commands for selected mode
		void FMC_autopilot();
 
	// Send controls and commands to all sim engines
		void send_control();
};

extern ogcFMC * pFMC;

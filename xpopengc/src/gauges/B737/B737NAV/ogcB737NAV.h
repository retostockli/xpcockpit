/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737NAV.h,v $

  Copyright (C) 2001-2 by:
    Original author:
      John Wojnaroski
    Contributors (in alphabetical order):
      Reto Stockli

  Last modification:
    Date:      $Date: 2003/04/18 06:00:45 $
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
This file defines the class/structure of the UDP packet that sends
the simulation data created by FlightGear to the glass displays. It
is required to "sync" the data types contained in the packet
*/

#ifndef ogcB737NAV_h
#define ogcB737NAV_h

#include <ogcGauge.h>
#include "../ogcBoeingColors.h"

namespace OpenGC
{

class B737NAV : public Gauge  
{
public:

  B737NAV();
  virtual ~B737NAV();

  void Render();

  bool GetMapCenter(void) {return m_MapCenter;}
  int GetMapMode(void) {return m_MapMode;}
  float GetMapRange(void) {return m_MapRange;}
  double GetMapCtrLon(void) {return m_MapCtrLon;}
  double GetMapCtrLat(void) {return m_MapCtrLat;}
  void SetMapCtrLon(double lon) {m_MapCtrLon=lon;}
  void SetMapCtrLat(double lat) {m_MapCtrLat=lat;}
  float GetMapHeading(void) {return m_MapHeading;}
 
 protected:

  // The font number
  int m_Font;

  bool m_MapCenter; // NAV Display Centered (true) or Expanded (false)?
  int m_MapMode; // NAV Display Mode: 0: APP, 1: VOR, 2: MAP, 3: PLN
  float m_MapRange; // NAV Display Range in [nm]
  double m_MapCtrLon; // NAV Display Center Longitude [deg E]
  double m_MapCtrLat; // NAV Display Center Latitude [deg N]
  float m_MapHeading; // True Heading of MAP (=0 or N in Plan Mode)
  
};

} // end namespace OpenGC

#endif

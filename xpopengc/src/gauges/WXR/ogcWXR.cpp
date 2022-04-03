/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Copyright (C) 2015 by:
    Original author:
      John Wojnaroski
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

/*
Base panel for defining the navigation displays. There is currenlty only
a slingle MAP mode (the MAP MODE EXPANDED) implemented.
*/

#include <stdio.h>
#include <math.h>

#include "ogcGLHeaders.h"
#include "ogcGaugeComponent.h"
#include "WXR/ogcWXR.h"
#include "WXR/ogcDrawWXR.h"
#include "WXR/ogcDrawWXRInfo.h"

namespace OpenGC
{

WXR::WXR()
{
  printf("WXR constructed\n");
        
  // We want to draw an outline or not?
  this->SetGaugeOutline(true);

  // Initialize Map Type Variables valid for all Subcomponents
  m_MapRange = 20;
  m_MapCtrLon = FLT_MISS;
  m_MapCtrLat = FLT_MISS;
  
  m_Font = m_pFontManager->LoadDefaultFont();
 
  // Specify our physical size and position
  m_PhysicalPosition.x = 0;
  m_PhysicalPosition.y = 0;
  
  m_PhysicalSize.x = 200;
  m_PhysicalSize.y = 160;

  /* Create Gauge Component that renders the WXR Image */
  DrawWXR* pDrawWXR = new DrawWXR();
  pDrawWXR->SetParentRenderObject(this);
  pDrawWXR->SetPosition(m_PhysicalPosition.x,m_PhysicalPosition.y);
  pDrawWXR->SetSize(m_PhysicalSize.x,m_PhysicalSize.y);
  pDrawWXR->SetWXRGauge(this);
  this->AddGaugeComponent(pDrawWXR);

  /* Create Gauge Component that renders the Circles and other Info */
  DrawWXRInfo* pDrawWXRInfo = new DrawWXRInfo();
  pDrawWXRInfo->SetParentRenderObject(this);
  pDrawWXRInfo->SetPosition(m_PhysicalPosition.x,m_PhysicalPosition.y);
  pDrawWXRInfo->SetSize(m_PhysicalSize.x,m_PhysicalSize.y);
  pDrawWXRInfo->SetWXRGauge(this);
  this->AddGaugeComponent(pDrawWXRInfo);

  /* Create Gauge Component that does XY */
  /* ... */

}

WXR::~WXR()
{
  // Destruction handled by base class
}

void WXR::Render()
{

  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");

  /* only display something if the battery bus is powered and avionics are running */
  if (*avionics_on == 1) {
     
    double *aircraftLat = link_dataref_dbl("sim/flightmodel/position/latitude",-4);
    double *aircraftLon = link_dataref_dbl("sim/flightmodel/position/longitude",-4);
    float *heading_mag = link_dataref_flt("sim/flightmodel/position/magpsi",-1);
    float *track_mag = link_dataref_flt("sim/cockpit2/gauges/indicators/ground_track_mag_pilot",-1);
    float *magnetic_variation = link_dataref_flt("sim/flightmodel/position/magnetic_variation",-1);
    int *rangesel = link_dataref_int("sim/cockpit2/EFIS/map_range");

    m_MapCtrLon = *aircraftLon;
    m_MapCtrLat = *aircraftLat;
    m_MapHeading = *track_mag - *magnetic_variation; // map shows TRACK MAG, but all symbols are on TRACK TRUE

    //printf("%i \n",*rangesel);

    /* WXR Range Selector through EFIS knob */
    if (*rangesel == 0) {
      m_MapRange = 5.0;
    } else if (*rangesel == 1) {
      m_MapRange = 10.0;
    } else if (*rangesel == 2) {
      m_MapRange = 20.0;
    } else if (*rangesel == 3) {
      m_MapRange = 40.0;
    } else if (*rangesel == 4) {
      m_MapRange = 80.0;
    } else if (*rangesel == 5) {
      m_MapRange = 160.0;
    } else if (*rangesel == 6) {
      m_MapRange = 320.0;
    } else if (*rangesel == 7) {
      m_MapRange = 640.0;
    } else {
      m_MapRange = 40.0;
    }

    /* for some acf map range is double this range */
    m_MapRange *= 2.0;
    
    // Call base class Render once all the new NAV states have been set
    Gauge::Render();

  } // Display powered?
  
}

} // end namespace OpenGC


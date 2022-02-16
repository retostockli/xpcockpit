/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737NAV.cpp,v $

  Copyright (C) 2015 by:
    Original author:
      John Wojnaroski
    Contributors (in alphabetical order):
      Reto Stockli

  Last modification:
    Date:      $Date: 2015/09/11 $
    Version:   $Revision: $
    Author:    $Author: stockli $
  
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
#include "B737/B737NAV/ogcB737NAV.h"
#include "B737/B737NAV/ogcB737NAVDrawStatic.h"
#include "B737/B737NAV/ogcB737NAVDrawDEM.h"
#include "B737/B737NAV/ogcB737NAVDrawWXR.h"
#include "B737/B737NAV/ogcB737NAVDrawStations.h"
#include "B737/B737NAV/ogcB737NAVDrawFMC.h"
#include "B737/B737NAV/ogcB737NAVDrawTCAS.h"
#include "B737/B737NAV/ogcB737NAVMapModeExpanded.h"
#include "B737/B737NAV/ogcB737NAVMapModePlan.h"

// The components listed below are planned...
//#include "ogcB737NAVMapModeCenter.h"
//#include "ogcB737NAVApproachModeExpanded.h"
//#include "ogcB737NAVApproachModeCenter.h"
//#include "ogcB737NAVVORModeExpanded.h"
//#include "ogcB737NAVVORModeCenter.h"
//#include "ogcB737NAVPlanMode.h"

namespace OpenGC
{

B737NAV::B737NAV()
{
  printf("B737NAV constructed\n");
        
  // We want to draw an outline
  this->SetGaugeOutline(true);

  // Initialize Map Type Variables valid for all Subcomponents
  m_MapCenter = false;
  m_MapMode = 1;
  m_MapRange = 20;

  m_MapCtrLon = FLT_MISS;
  m_MapCtrLat = FLT_MISS;
  
  m_Font = m_pFontManager->LoadDefaultFont();
 
  // Specify our physical size and position
  m_PhysicalPosition.x = 0;
  m_PhysicalPosition.y = 0;
  
  m_PhysicalSize.x = 200;
  m_PhysicalSize.y = 200;

  B737NAVDrawDEM* pDrawDEM = new B737NAVDrawDEM();
  pDrawDEM->SetParentRenderObject(this);
  pDrawDEM->SetPosition(m_PhysicalPosition.x,m_PhysicalPosition.y);
  pDrawDEM->SetSize(m_PhysicalSize.x,m_PhysicalSize.y);
  pDrawDEM->SetNAVGauge(this);
  this->AddGaugeComponent(pDrawDEM);

  B737NAVDrawWXR* pDrawWXR = new B737NAVDrawWXR();
  pDrawWXR->SetParentRenderObject(this);
  pDrawWXR->SetPosition(m_PhysicalPosition.x,m_PhysicalPosition.y);
  pDrawWXR->SetSize(m_PhysicalSize.x,m_PhysicalSize.y);
  pDrawWXR->SetNAVGauge(this);
  this->AddGaugeComponent(pDrawWXR);

  B737NAVDrawStations* pDrawStat = new B737NAVDrawStations();
  pDrawStat->SetParentRenderObject(this);
  pDrawStat->SetPosition(m_PhysicalPosition.x,m_PhysicalPosition.y);
  pDrawStat->SetSize(m_PhysicalSize.x,m_PhysicalSize.y);
  pDrawStat->SetNAVGauge(this);
  this->AddGaugeComponent(pDrawStat);

  B737NAVDrawFMC* pDrawFMC = new B737NAVDrawFMC();
  pDrawFMC->SetParentRenderObject(this);
  pDrawFMC->SetPosition(m_PhysicalPosition.x,m_PhysicalPosition.y);
  pDrawFMC->SetSize(m_PhysicalSize.x,m_PhysicalSize.y);
  pDrawFMC->SetNAVGauge(this);
  this->AddGaugeComponent(pDrawFMC);
  
  B737NAVDrawTCAS* pDrawTCAS = new B737NAVDrawTCAS();
  pDrawTCAS->SetParentRenderObject(this);
  pDrawTCAS->SetPosition(m_PhysicalPosition.x,m_PhysicalPosition.y);
  pDrawTCAS->SetSize(m_PhysicalSize.x,m_PhysicalSize.y);
  pDrawTCAS->SetNAVGauge(this);
  this->AddGaugeComponent(pDrawTCAS);

  B737NAVMapModeExpanded* pMapExp = new B737NAVMapModeExpanded();
  pMapExp->SetParentRenderObject(this);
  pMapExp->SetPosition(m_PhysicalPosition.x,m_PhysicalPosition.y);
  pMapExp->SetSize(m_PhysicalSize.x,m_PhysicalSize.y);
  pMapExp->SetNAVGauge(this);
  this->AddGaugeComponent(pMapExp);

  B737NAVMapModePlan* pMapPlan = new B737NAVMapModePlan();
  pMapPlan->SetParentRenderObject(this);
  pMapPlan->SetPosition(m_PhysicalPosition.x,m_PhysicalPosition.y);
  pMapPlan->SetSize(m_PhysicalSize.x,m_PhysicalSize.y);
  pMapPlan->SetNAVGauge(this);
  this->AddGaugeComponent(pMapPlan);

  B737NAVDrawStatic* pDrawStatic = new B737NAVDrawStatic();
  pDrawStatic->SetParentRenderObject(this);
  pDrawStatic->SetPosition(m_PhysicalPosition.x,m_PhysicalPosition.y);
  pDrawStatic->SetSize(m_PhysicalSize.x,m_PhysicalSize.y);
  pDrawStatic->SetNAVGauge(this);
  this->AddGaugeComponent(pDrawStatic);

}

B737NAV::~B737NAV()
{
  // Destruction handled by base class
}

void B737NAV::Render()
{
  
  int acf_type = m_pDataSource->GetAcfType();

  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");
  int *irs_mode;
  if ((acf_type == 2) || (acf_type == 3)) {
    irs_mode = link_dataref_int("laminar/B738/irs/irs_mode");
  } else {
    irs_mode = link_dataref_int("xpserver/irs_mode");
    *irs_mode = 2;
  }
 
  if (*avionics_on == 1) {
     
    double *aircraftLat = link_dataref_dbl("sim/flightmodel/position/latitude",-4);
    double *aircraftLon = link_dataref_dbl("sim/flightmodel/position/longitude",-4);
    float *heading_mag = link_dataref_flt("sim/flightmodel/position/magpsi",-1);
    float *track_mag = link_dataref_flt("sim/cockpit2/gauges/indicators/ground_track_mag_pilot",-1);
    float *magnetic_variation = link_dataref_flt("sim/flightmodel/position/magnetic_variation",-1);

    bool is_captain = (this->GetArg() == 0);

    // Get Map Mode (APP/VOR/MAP/PLN)
    int *map_mode;
    if ((acf_type == 2) || (acf_type == 3)) {
      if (is_captain) {
	map_mode = link_dataref_int("laminar/B738/EFIS_control/capt/map_mode_pos");
      } else {
	map_mode = link_dataref_int("laminar/B738/EFIS_control/fo/map_mode_pos");
      }
    } else {
      map_mode = link_dataref_int("sim/cockpit2/EFIS/map_mode");
    }

    if (*map_mode != INT_MISS) {
      m_MapMode = *map_mode;
    }

    if (m_MapMode != 3) {
      SetMapCtrLon(*aircraftLon);
      SetMapCtrLat(*aircraftLat);
      m_MapHeading = *track_mag - *magnetic_variation; // map shows TRACK MAG, but all symbols are on TRACK TRUE
      // m_MapHeading = *heading_mag - *magnetic_variation; // map shows TRACK MAG, but all symbols are on TRACK TRUE
    } else {
      // set throught DrawFMC gauge component when reading center FMS waypoint
      m_MapHeading = 0.0;
    }

    // if IRS is not aligned disable map display
    if (*irs_mode != 2) m_MapHeading = FLT_MISS;
    
    //    printf("%f %f \n",m_MapCtrLon,m_MapCtrLat);
    
    // Get Map Centered vs. Expanded Mode
    int *map_expanded;
    if ((acf_type == 2) || (acf_type == 3)) {
      if (is_captain) {
	map_expanded = link_dataref_int("laminar/B738/EFIS_control/capt/exp_map");
      } else {
	map_expanded = link_dataref_int("laminar/B738/EFIS_control/fo/exp_map");
      }
      if (*map_expanded != INT_MISS) {
	if (*map_expanded == 1) {
	  m_MapCenter = false;
	} else {
	  m_MapCenter = true;
	}
      }
      if (*map_mode == 3) {
	m_MapCenter = true;
      }
    } else {
      // leave default (Expanded map)
    }

    //    printf("%i %i %i \n",m_MapMode,m_MapCenter,*map_mode);
    
    // Get Navigation Map Range Selector Position and determine maximum displayable range
    int *rangesel;
    if (acf_type == 3) {
      if (is_captain) {
	rangesel = link_dataref_int("laminar/B738/EFIS/capt/map_range");
      } else {
	rangesel = link_dataref_int("laminar/B738/EFIS/fo/map_range");
      }
    } else {
      rangesel = link_dataref_int("sim/cockpit2/EFIS/map_range");
    }

    //printf("%i \n",*rangesel);
     
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
    if (acf_type != 3) {
      /* map range starts at 10 nm for regular ACF */
      m_MapRange *= 2;
    }

    /* Centered Mode: forward range is only half of the selected range */
    if ((m_MapCenter) && (m_MapMode != 3)) {
      m_MapRange /= 2;
    }

    // Call base class Render once all the new NAV states have been set
    Gauge::Render();

  } // Display powered?
  
}

} // end namespace OpenGC


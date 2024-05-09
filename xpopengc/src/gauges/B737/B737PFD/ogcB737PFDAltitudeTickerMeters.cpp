/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
 
  Copyright (C) 2001-2021 by:
  Original author:
  Damion Shelton
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

#include <stdio.h>

#include "ogcGaugeComponent.h"
#include "ogcB737PFD.h"
#include "ogcB737PFDAltitudeTickerMeters.h"

namespace OpenGC
{

  B737PFDAltitudeTickerMeters::B737PFDAltitudeTickerMeters()
  {
    printf("B737PFDAltitudeTickerMeters constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 24.5;
    m_PhysicalSize.y = 8;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  B737PFDAltitudeTickerMeters::~B737PFDAltitudeTickerMeters()
  {

  }

  void B737PFDAltitudeTickerMeters::Render()
  {
    // Call base class to setup viewport and projection
    GaugeComponent::Render();

    int acf_type = m_pDataSource->GetAcfType();
    bool is_captain = (this->GetArg() == 0);
    
    float lineWidth = 1.5;
    float fontHeight = 4.5;
    float fontWidth = 4.25;

    if ((acf_type == 2) || (acf_type == 3)) {
      
      int *alt_is_meters;
      if (is_captain) {
	alt_is_meters = link_dataref_int("laminar/B738/PFD/capt/alt_mode_is_meters");
      } else {
	alt_is_meters = link_dataref_int("laminar/B738/PFD/fo/alt_mode_is_meters");
      }
 
      if (*alt_is_meters == 1) {

	// Draw black background
	glLineWidth(lineWidth);
	glColor3ub(COLOR_BLACK);
	glRectd(0.0,0.0,24.5,8.0);
	glEnd();

	// White border around background
	glLineWidth(lineWidth);
	glColor3ub(COLOR_WHITE);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0.0,0.0);
	glVertex2f(24.5,0.0);
	glVertex2f(24.5,8.0);
	glVertex2f(0.0,8.0);
	glEnd();

	// Get the barometric altitude (feet)
	float *pressure_altitude;
	if (is_captain) {
	  pressure_altitude = link_dataref_flt("sim/cockpit2/gauges/indicators/altitude_ft_pilot",0);
	} else {
	  pressure_altitude = link_dataref_flt("sim/cockpit2/gauges/indicators/altitude_ft_copilot",0);
	}

	if (*pressure_altitude != FLT_MISS) {
 
	  char buffer[12];
	  memset(buffer,0,sizeof(buffer));

	  int alt_meters = (int) *pressure_altitude * 0.3048;
	  
	  float x = 16.0;
	  float y = 1.5;
	  
	  glColor3ub(COLOR_WHITE);
 	  m_pFontManager->SetSize(m_Font, 1.0*fontWidth, 1.15*fontHeight);
	  if (alt_meters >= 10000) {
	    snprintf(buffer, sizeof(buffer), "%d", alt_meters);
	    m_pFontManager->Print(x-0.85*4.0*fontWidth,y, &buffer[0], m_Font);
	  } else if (alt_meters > 1000) {
	    snprintf(buffer, sizeof(buffer), "%d", alt_meters);
	    m_pFontManager->Print(x-0.85*3.0*fontWidth,y, &buffer[0], m_Font);
	  } else {
	    snprintf(buffer, sizeof(buffer), "%03d", alt_meters);
	    m_pFontManager->Print(x-0.85*2.0*fontWidth,y, &buffer[0], m_Font);
	  }

	  glColor3ub(COLOR_CYAN);
	  snprintf(buffer, sizeof(buffer), "M");
	  m_pFontManager->SetSize(m_Font, 0.75*fontWidth, 0.9*fontHeight);
	  m_pFontManager->Print(x+4.0,y, &buffer[0], m_Font);

	}

      } // altitude is displayed in meters

    } // ZIBO?
    
  }

} // end namespace OpenGC

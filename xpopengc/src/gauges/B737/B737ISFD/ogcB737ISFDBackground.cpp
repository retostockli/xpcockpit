/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Copyright (C) 2021 by:
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
#include <math.h>
#include "ogcGaugeComponent.h"
#include "ogcB737ISFD.h"
#include "ogcB737ISFDBackground.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC
{

  B737ISFDBackground::B737ISFDBackground()
  {
    printf("B737ISFDBackground constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 62; 
    m_PhysicalSize.y = 13;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  B737ISFDBackground::~B737ISFDBackground()
  {

  }

  void B737ISFDBackground::Render()
  {

    // Call base class to setup viewport and projection
    GaugeComponent::Render();

    int acf_type = m_pDataSource->GetAcfType();

    char buffer[12]; // temporary buffer for text
    memset(buffer,0,sizeof(buffer));
    
    float fontHeight = 6;
    float fontWidth = 5;
    float *stdby_pressure;
    if ((acf_type == 2) || (acf_type == 3)) {
      stdby_pressure = link_dataref_flt("laminar/B738/gauges/standby_altimeter_baro",-2);
    } else {
      stdby_pressure = link_dataref_flt("sim/cockpit/misc/barometer_setting",-2);
    }

    float *stdby_pressure_mode;
    if ((acf_type == 2) || (acf_type == 3)) {
      stdby_pressure_mode = link_dataref_flt("laminar/B738/gauges/standby_alt_mode",0);
    } else {
      stdby_pressure_mode = link_dataref_flt("sim/cockpit/misc/barometer_setting",0);
    }    
    float *stdby_std_mode;
    if ((acf_type == 2) || (acf_type == 3)) {
      stdby_std_mode = link_dataref_flt("laminar/B738/gauges/standby_alt_std_mode",0);
    } else {
      stdby_std_mode = link_dataref_flt("sim/cockpit/misc/barometer_setting",0);
    }    
    float *isfd_horizontal;
    if ((acf_type == 2) || (acf_type == 3)) {
      isfd_horizontal = link_dataref_flt("laminar/B738/gauges/standby_app_horz",0);
    } else {
      isfd_horizontal = link_dataref_flt("xpserver/isfd/app_horizontal",0);
    }
    float *isfd_vertical;
    if ((acf_type == 2) || (acf_type == 3)) {
      isfd_vertical = link_dataref_flt("laminar/B738/gauges/standby_app_vert",0);
    } else {
      isfd_vertical = link_dataref_flt("xpserver/isfd/app_vertical",0);
    }
    
    // Save matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glColor3ub(COLOR_GREEN);
    m_pFontManager->SetSize(m_Font, fontWidth, fontHeight);

    // Plot ISFD Mode
    if ((*isfd_horizontal==1) && (*isfd_vertical==1)) {
      strcpy(buffer, "APP");
      m_pFontManager->Print(m_PhysicalSize.x*0.03,m_PhysicalSize.y*0.2, &buffer[0], m_Font);
    } else if (*isfd_horizontal==1) {
      strcpy(buffer, "BCRS");
      m_pFontManager->Print(m_PhysicalSize.x*0.03,m_PhysicalSize.y*0.2, &buffer[0], m_Font);
    }
    
    // Plot dialed barometric pressure in In Hg
    bool is_std = false;
    if ((acf_type == 2) || (acf_type == 3)) {
      if (*stdby_std_mode == 1.0) is_std = true;
    } else {
      if (roundf(*stdby_pressure*100) == 2992) is_std = true;
    }
      

    if (*stdby_pressure != FLT_MISS) {
      if (is_std) {
	strcpy(buffer, "STD");
	m_pFontManager->Print(m_PhysicalSize.x*0.77,m_PhysicalSize.y*0.2, &buffer[0], m_Font);
      } else {
	m_pFontManager->SetSize(m_Font, fontWidth*0.9, fontHeight*0.9);
	if (*stdby_pressure_mode == 1) {
	  snprintf(buffer, sizeof(buffer), "%4.0f HPA", *stdby_pressure/0.029530);
	  m_pFontManager->Print(m_PhysicalSize.x*0.51,m_PhysicalSize.y*0.2, &buffer[0], m_Font);
	} else {
	  snprintf(buffer, sizeof(buffer), "%4.2f IN", *stdby_pressure);
	  m_pFontManager->Print(m_PhysicalSize.x*0.57,m_PhysicalSize.y*0.2, &buffer[0], m_Font);
	}
      }
    }

    glPopMatrix();
  }

} // end namespace OpenGC

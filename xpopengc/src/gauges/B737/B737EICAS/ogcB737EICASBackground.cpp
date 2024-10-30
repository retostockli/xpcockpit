/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737EICASBackground.cpp,v $

  Copyright (C) 2002-2015 by:
  Original author:
  Michael DeFeyter
  Contributors (in alphabetical order):
  Reto Stockli

  Last modification:
  Date:      $Date: 2015/10/06 $
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

#include "B737/B737EICAS/ogcB737EICAS.h"
#include "B737/B737EICAS/ogcB737EICASBackground.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace OpenGC
{

  B737EICASBackground::B737EICASBackground()
  {
    printf("B737EICASBackground constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 201;
    m_PhysicalSize.y = 201;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

  }

  B737EICASBackground::~B737EICASBackground()
  {

  }

  void B737EICASBackground::Render()
  {
    GaugeComponent::Render();

    float *time = link_dataref_flt("sim/time/framerate_period",-3);
    float *tat = link_dataref_flt("sim/weather/aircraft/temperature_ambient_deg_c",-1);
    int *oilpress1 = link_dataref_int_arr("sim/cockpit/warnings/annunciators/oil_pressure_low",8,0);
    int *oilpress2 = link_dataref_int_arr("sim/cockpit/warnings/annunciators/oil_pressure_low",8,1);

    int acf_type = m_pDataSource->GetAcfType();

    float *valve1;
    float *valve2;
    float *filter1;
    float *filter2;
    int *n1_mode;
    if (acf_type == 1) {
      /* these are only available for the x737 */
      valve1 = link_dataref_flt("x737/ovh/fuelPanel/engValve1_annunc",-1);
      valve2 = link_dataref_flt("x737/ovh/fuelPanel/engValve2_annunc",-1);
      filter1 = link_dataref_flt("x737/ovh/fuelPanel/filterBypass1_annunc",-1);
      filter2 = link_dataref_flt("x737/ovh/fuelPanel/filterBypass2_annunc",-1);
    } else if (acf_type == 3) {
      valve1 = link_dataref_flt("laminar/B738/engine/start_valve1",-1);
      valve2 = link_dataref_flt("laminar/B738/engine/start_valve2",-1);
      filter1 = link_dataref_flt("laminar/B738/annunciator/bypass_filter_1",-1);
      filter2 = link_dataref_flt("laminar/B738/annunciator/bypass_filter_2",-1);
      n1_mode = link_dataref_int("laminar/B738/FMS/N1_mode");
    }
      
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glEnable(GL_LINE_SMOOTH);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

    glLineWidth(3.0);
    glColor3ub(COLOR_LIGHTBLUE);

    glBegin(GL_LINE_STRIP);
    glVertex2f(116, 165);
    glVertex2f(116, 5);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(118, 52);
    glVertex2f(195, 52);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(114, 40);
    glVertex2f(5, 40);
    glEnd();

    m_pFontManager->SetSize(m_Font, 5, 5);

    m_pFontManager->Print(52, 145, "N1", m_Font);
    m_pFontManager->Print(52, 75, "N2", m_Font);
    m_pFontManager->Print(50, 110, "EGT", m_Font);
    m_pFontManager->Print(147, 96, "OIL T", m_Font);
    m_pFontManager->Print(147, 131, "OIL P", m_Font);
    m_pFontManager->Print(146, 87, "OIL Q %", m_Font);
    m_pFontManager->Print(148, 14, "HYD P", m_Font);
    m_pFontManager->Print(150, 44, "A", m_Font);
    m_pFontManager->Print(160, 44, "B", m_Font);
    m_pFontManager->Print(143, 6, "HYD Q %", m_Font);
    m_pFontManager->Print(152, 54, "VIB", m_Font);
    m_pFontManager->Print(47, 49, "FF/FU", m_Font);
    m_pFontManager->Print(41, 42, "KG x 1000", m_Font);
    m_pFontManager->Print(44, 5, "FUEL KG", m_Font);

	
    m_pFontManager->SetSize(m_Font,6,6);
    char buf[50];
 
    if (*time != FLT_MISS) {
      int fps = (int) 1.0 / *time;
      if (fps > 0) {
	snprintf(buf, sizeof(buf), "%i", fps);
      }
     glColor3ub(COLOR_WHITE);
     m_pFontManager->Print(5, 190, buf, m_Font);
    }

    if (acf_type == 3) {
      if (*n1_mode != INT_MISS) {
	if (*n1_mode == 0) {
	  sprintf(buf, "MAN");
	} else if (*n1_mode == 1) {
	  sprintf(buf, "TO");
	} else if (*n1_mode == 2) {
	  sprintf(buf, "TO 1");
	} else if (*n1_mode == 3) {
	  sprintf(buf, "TO 2");
	} else if (*n1_mode == 4) {
	  sprintf(buf, "D-TO");
	} else if (*n1_mode == 5) {
	  sprintf(buf, "D-TO 1");
	} else if (*n1_mode == 6) {
	  sprintf(buf, "D-TO 2");
	} else if (*n1_mode == 7) {
	  sprintf(buf, "CLB");
	} else if (*n1_mode == 8) {
	  sprintf(buf, "CLB 1");
	} else if (*n1_mode == 9) {
	  sprintf(buf, "CLB 2");
	} else if (*n1_mode == 10) {
	  sprintf(buf, "CRZ");
	} else if (*n1_mode == 11) {
	  sprintf(buf, "G/A");
	} else if (*n1_mode == 12) {
	  sprintf(buf, "CON");
	} else {
	  sprintf(buf, "---");
	}
	glColor3ub(COLOR_GREEN);
	m_pFontManager->Print(35, 190, buf, m_Font);

      }
    }
   
    glColor3ub(COLOR_LIGHTBLUE);
    m_pFontManager->SetSize(m_Font, 5, 5);
    m_pFontManager->Print(75, 190, "TAT", m_Font);
    if (*tat != FLT_MISS) {
      int itat = (int) *tat;
      if (itat > 0) {
	snprintf(buf, sizeof(buf), "+%i c", itat);
      } else {
	snprintf(buf, sizeof(buf), "%i c", itat);
      }
      glColor3ub(COLOR_GREEN);
      m_pFontManager->SetSize(m_Font,6,6);
      m_pFontManager->Print(90, 190, buf, m_Font);
    }

    // warning annunciators
    m_pFontManager->SetSize(m_Font,3.0,3.0);

    if ((acf_type == 1) || (acf_type == 3)) {
      if (*valve1 == 1.0) {
	glColor3ub(COLOR_YELLOW);
      } else {
	glColor3ub(COLOR_GRAY15);
      }
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glBegin(GL_POLYGON);
      glVertex2f(120, 194.5);
      glVertex2f(155, 194.5);
      glVertex2f(155, 185.5);
      glVertex2f(120, 185.5);
      glEnd();;
      if (*valve1 == 1.0) {
	glColor3ub(COLOR_BLACK);
	m_pFontManager->Print(124, 191,"START VALVE", m_Font);
	m_pFontManager->Print(132, 186,"OPEN", m_Font);
      }

      if (*valve2 == 1.0) {
	glColor3ub(COLOR_YELLOW);
      } else {
	glColor3ub(COLOR_GRAY15);
      }
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glBegin(GL_POLYGON);
      glVertex2f(120+40, 194.5);
      glVertex2f(155+40, 194.5);
      glVertex2f(155+40, 185.5);
      glVertex2f(120+40, 185.5);
      glEnd();;
      if (*valve2 == 1.0) {
	glColor3ub(COLOR_BLACK);
	m_pFontManager->Print(124+40, 191,"START VALVE", m_Font);
	m_pFontManager->Print(132+40, 186,"OPEN", m_Font);
      }
      
      if (*filter1 == 1.0) {
	glColor3ub(COLOR_YELLOW);
      } else {
	glColor3ub(COLOR_GRAY15);
      }
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glBegin(GL_POLYGON);
      glVertex2f(120, 194.5-10);
      glVertex2f(155, 194.5-10);
      glVertex2f(155, 185.5-10);
      glVertex2f(120, 185.5-10);
      glEnd();;
      if (*filter1 == 1.0) {
	glColor3ub(COLOR_BLACK);
	m_pFontManager->Print(126, 191-10,"OIL FILTER", m_Font);
	m_pFontManager->Print(130, 186-10,"BYPASS", m_Font);
      }

      if (*filter2 == 1.0) {
	glColor3ub(COLOR_YELLOW);
      } else {
	glColor3ub(COLOR_GRAY15);
      }
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glBegin(GL_POLYGON);
      glVertex2f(120+40, 194.5-10);
      glVertex2f(155+40, 194.5-10);
      glVertex2f(155+40, 185.5-10);
      glVertex2f(120+40, 185.5-10);
      glEnd();;
      if (*filter2 == 1.0) {
	glColor3ub(COLOR_BLACK);
	m_pFontManager->Print(126+40, 191-10,"OIL FILTER", m_Font);
	m_pFontManager->Print(130+40, 186-10,"BYPASS", m_Font);
      }
    }

    if (*oilpress1 == 1) {
      glColor3ub(COLOR_YELLOW);
    } else {
      glColor3ub(COLOR_GRAY15);
    }
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(120, 194.5-20);
    glVertex2f(155, 194.5-20);
    glVertex2f(155, 185.5-20);
    glVertex2f(120, 185.5-20);
    glEnd();;
    if (*oilpress1 == 1) {
      glColor3ub(COLOR_BLACK);
      m_pFontManager->Print(129, 191-20,"LOW OIL", m_Font);
      m_pFontManager->Print(127, 186-20,"PRESSURE", m_Font);
    }
    
    if (*oilpress2 == 1) {
      glColor3ub(COLOR_YELLOW);
    } else {
      glColor3ub(COLOR_GRAY15);
    }
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(120+40, 194.5-20);
    glVertex2f(155+40, 194.5-20);
    glVertex2f(155+40, 185.5-20);
    glVertex2f(120+40, 185.5-20);
    glEnd();;
    if (*oilpress2 == 1) {
      glColor3ub(COLOR_BLACK);
      m_pFontManager->Print(129+40, 191-20,"LOW OIL", m_Font);
      m_pFontManager->Print(127+40, 186-20,"PRESSURE", m_Font);
    }

    glPopMatrix();
  }

}

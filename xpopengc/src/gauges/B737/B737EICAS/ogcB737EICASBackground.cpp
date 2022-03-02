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

    float *tat = link_dataref_flt("sim/weather/temperature_ambient_c",-1);
    int *oilpress1 = link_dataref_int_arr("sim/cockpit/warnings/annunciators/oil_pressure_low",8,0);
    int *oilpress2 = link_dataref_int_arr("sim/cockpit/warnings/annunciators/oil_pressure_low",8,1);

    int acf_type = m_pDataSource->GetAcfType();

    float *valve1;
    float *valve2;
    float *filter1;
    float *filter2;
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

	
    m_pFontManager->Print(80, 190, "TAT", m_Font);
    m_pFontManager->SetSize(m_Font,6,6);
    glColor3ub(COLOR_WHITE);
    char buf[50];

    if (*tat != FLT_MISS) {
      snprintf(buf, sizeof(buf), "%.01f c", *tat);
      m_pFontManager->Print(95, 190, buf, m_Font);
    }

    // warning annunciators
    m_pFontManager->SetSize(m_Font,3.0,3.0);

    if ((acf_type == 1) || (acf_type == 3)) {
      if (*valve1 == 1.0) {
	glColor3ub(COLOR_YELLOW);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glBegin(GL_POLYGON);
	glVertex2f(120, 194.5);
	glVertex2f(155, 194.5);
	glVertex2f(155, 185.5);
	glVertex2f(120, 185.5);
	glEnd();;
	glColor3ub(COLOR_BLACK);
	m_pFontManager->Print(124, 191,"START VALVE", m_Font);
	m_pFontManager->Print(132, 186,"OPEN", m_Font);
      }
      if (*valve2 == 1.0) {
	glColor3ub(COLOR_YELLOW);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glBegin(GL_POLYGON);
	glVertex2f(120+40, 194.5);
	glVertex2f(155+40, 194.5);
	glVertex2f(155+40, 185.5);
	glVertex2f(120+40, 185.5);
	glEnd();;
	glColor3ub(COLOR_BLACK);
	m_pFontManager->Print(124+40, 191,"START VALVE", m_Font);
	m_pFontManager->Print(132+40, 186,"OPEN", m_Font);
      }
      
      if (*filter1 == 1.0) {
	glColor3ub(COLOR_YELLOW);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glBegin(GL_POLYGON);
	glVertex2f(120, 194.5-10);
	glVertex2f(155, 194.5-10);
	glVertex2f(155, 185.5-10);
	glVertex2f(120, 185.5-10);
	glEnd();;
	glColor3ub(COLOR_BLACK);
	m_pFontManager->Print(126, 191-10,"OIL FILTER", m_Font);
	m_pFontManager->Print(130, 186-10,"BYPASS", m_Font);
      }
      if (*filter2 == 1.0) {
	glColor3ub(COLOR_YELLOW);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glBegin(GL_POLYGON);
	glVertex2f(120+40, 194.5-10);
	glVertex2f(155+40, 194.5-10);
	glVertex2f(155+40, 185.5-10);
	glVertex2f(120+40, 185.5-10);
	glEnd();;
	glColor3ub(COLOR_BLACK);
	m_pFontManager->Print(126+40, 191-10,"OIL FILTER", m_Font);
	m_pFontManager->Print(130+40, 186-10,"BYPASS", m_Font);
      }
    }

    if (*oilpress1 == 1) {
      glColor3ub(COLOR_YELLOW);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glBegin(GL_POLYGON);
      glVertex2f(120, 194.5-20);
      glVertex2f(155, 194.5-20);
      glVertex2f(155, 185.5-20);
      glVertex2f(120, 185.5-20);
      glEnd();;
      glColor3ub(COLOR_BLACK);
      m_pFontManager->Print(129, 191-20,"LOW OIL", m_Font);
      m_pFontManager->Print(127, 186-20,"PRESSURE", m_Font);
    }
    if (*oilpress2 == 1) {
      glColor3ub(COLOR_YELLOW);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glBegin(GL_POLYGON);
      glVertex2f(120+40, 194.5-20);
      glVertex2f(155+40, 194.5-20);
      glVertex2f(155+40, 185.5-20);
      glVertex2f(120+40, 185.5-20);
      glEnd();;
      glColor3ub(COLOR_BLACK);
      m_pFontManager->Print(129+40, 191-20,"LOW OIL", m_Font);
      m_pFontManager->Print(127+40, 186-20,"PRESSURE", m_Font);
    }

    glPopMatrix();
  }

}

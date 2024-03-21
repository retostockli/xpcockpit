/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737LOWERDU.cpp,v $

  Copyright (C) 2024 by:
    Original author:
      Reto Stockli
    Contributors (in alphabetical order):
  
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


#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#include "ogcGLHeaders.h"
#include "ogcGaugeComponent.h"
#include "B737/B737LOWERDU/ogcB737LOWERDU.h"
#include "B737/B737LOWERDU/ogcB737LOWERDUSys.h"

namespace OpenGC
{

B737LOWERDU::B737LOWERDU()
{

  printf("B737LOWERDU constructed\n");
	
  // We want to draw an outline
  this->SetGaugeOutline(true);

  //  m_Font = m_pFontManager->LoadDefaultFont();
  m_Font = m_pFontManager->LoadFont((char*) "CockpitMCDU.ttf");

  // Specify our physical size and position
  m_PhysicalPosition.x = 0;
  m_PhysicalPosition.y = 0;
  
  m_PhysicalSize.x = 200;
  m_PhysicalSize.y = 200;

  B737LOWERDUSys* pB737LOWERDUSys = new B737LOWERDUSys();
  pB737LOWERDUSys->SetParentRenderObject(this);
  pB737LOWERDUSys->SetPosition(0,0);
  this->AddGaugeComponent(pB737LOWERDUSys);
  
}

B737LOWERDU::~B737LOWERDU()
{
  // Destruction handled by base class
}

void B737LOWERDU::Render()
{

  int rotate = this->GetArg();
 
  int acf_type = m_pDataSource->GetAcfType();
  
  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");
  //if ((*avionics_on == 1) && (acf_type >= 1)) {
    char buffer[10];

    // after drawing the background, draw components
    Gauge::Render();

    //} else {

    Gauge::ResetGaugeCoordinateSystem();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    glTranslatef(m_PhysicalSize.x/2, m_PhysicalSize.y/2, 0.0);
    glRotatef(rotate, 0, 0, 1);
    glTranslatef(-m_PhysicalSize.x/2, -m_PhysicalSize.y/2, 0.0);
    // Draw green rectangle in upper left of gauge to show it is loaded (red if X-plane is not yet connected)
    if (*avionics_on == 0) {
      // Valid dataref: Connection to X-Plane intact
      glColor3ub(COLOR_GREEN);
    } else {
      // Likely missing dataref: Connection to X-Plane not yet done
      glColor3ub(COLOR_RED);
    }
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(5,m_PhysicalSize.y-5);
    glVertex2f(10,m_PhysicalSize.y-5);
    glVertex2f(10,m_PhysicalSize.y-10);
    glVertex2f(5,m_PhysicalSize.y-10);
    glEnd();  
    //} // Display powered?

    glPopMatrix();

}

} // end namespace OpenGC


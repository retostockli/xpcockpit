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
#include <math.h>

#include "ogcB737PFDArtificialHorizon.h"
#include "ogcB737PFDAltitudeTicker.h"
#include "ogcB737PFDAltitudeTickerMeters.h"
#include "ogcB737PFDAltitudeTape.h"
#include "ogcB737PFDSpeedTape.h"
#include "ogcB737PFDSpeedTicker.h"
#include "ogcB737PFDHeadingIndicator.h"
#include "ogcB737PFDVSI.h"
#include "ogcB737PFD.h"
#include "ogcB737PFDBackground.h"

namespace OpenGC
{

B737PFD::B737PFD()
{
  printf("B737PFD constructed\n");
 
  m_PhysicalSize.x = 200;
  m_PhysicalSize.y = 200;
  
  // We need a font to draw the ILS display
  m_Font = m_pFontManager->LoadDefaultFont();
  
  // We want to draw an outline
  this->SetGaugeOutline(false);

  // Create a heading indicator
  B737PFDHeadingIndicator* pHeadingIndicator = new B737PFDHeadingIndicator();
  pHeadingIndicator->SetParentRenderObject(this);
  pHeadingIndicator->SetPosition(29,5);
  this->AddGaugeComponent(pHeadingIndicator);

  // Create an artficial horizon
  B737PFDArtificialHorizon* pHorizon = new B737PFDArtificialHorizon();
  pHorizon->SetParentRenderObject(this);
  pHorizon->SetPosition(42,52);
  this->AddGaugeComponent(pHorizon);

  // Create an altitude tape
  B737PFDAltitudeTape* pAltTape = new B737PFDAltitudeTape();
  pAltTape->SetParentRenderObject(this);
  pAltTape->SetPosition(150,32);
  this->AddGaugeComponent(pAltTape);

  // Create a VSI
  B737PFDVSI* pVSI = new B737PFDVSI();
  pVSI->SetParentRenderObject(this);
  pVSI->SetPosition(182,44);
  this->AddGaugeComponent(pVSI);

  // Create an altitude ticker
  B737PFDAltitudeTicker* pAltTicker = new B737PFDAltitudeTicker();
  pAltTicker->SetParentRenderObject(this);
  pAltTicker->SetPosition(157,90);
  this->AddGaugeComponent(pAltTicker);

  // Create an altitude ticker (Meters)
  B737PFDAltitudeTickerMeters* pAltTickerMeters = new B737PFDAltitudeTickerMeters();
  pAltTickerMeters->SetParentRenderObject(this);
  pAltTickerMeters->SetPosition(160.5,108);
  this->AddGaugeComponent(pAltTickerMeters);

  // Create a speed tape
  B737PFDSpeedTape* pSpeedTape = new B737PFDSpeedTape();
  pSpeedTape->SetParentRenderObject(this);
  pSpeedTape->SetPosition(8,32);
  this->AddGaugeComponent(pSpeedTape);

  // Create a speed ticker
  B737PFDSpeedTicker* pSpeedTicker = new B737PFDSpeedTicker();
  pSpeedTicker->SetParentRenderObject(this);
  pSpeedTicker->SetPosition(7,90);
  this->AddGaugeComponent(pSpeedTicker);

  // Create all other messages and indicators on the PDF
  B737PFDBackground* pBackground = new B737PFDBackground();
  pBackground->SetParentRenderObject(this);
  pBackground->SetPosition(0,0);
  this->AddGaugeComponent(pBackground);

  

}

B737PFD::~B737PFD()
{
  // Destruction handled by base class
}

void B737PFD::Render()
{
  
  // printf("%i \n",this->GetArg());

  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");

  if (*avionics_on == 1) {
    Gauge::Render();
  } else {
    // Draw green rectangle in upper left of gauge to show it is loaded (red if X-plane is not yet connected)
    Gauge::ResetGaugeCoordinateSystem();
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
  }

  float fps = GetFPS();
  char buffer[5];

  if (fps != FLT_MISS) {
    glColor3ub(COLOR_RED);
    snprintf( buffer, sizeof(buffer), "%i",(int) round(fps) );
    this->m_pFontManager->SetSize(m_Font, 5, 5);
    this->m_pFontManager->Print(m_PhysicalSize.x-15, m_PhysicalSize.y-10, &buffer[0], m_Font);    
  }
  
}

} // end namespace OpenGC


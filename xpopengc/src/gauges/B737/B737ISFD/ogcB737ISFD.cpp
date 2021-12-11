/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Copyright (C) 2021  by:
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

#include "ogcB737ISFDArtificialHorizon.h"
#include "ogcB737ISFDAltitudeTicker.h"
#include "ogcB737ISFDAltitudeTape.h"
#include "ogcB737ISFDSpeedTape.h"
#include "ogcB737ISFDSpeedTicker.h"
#include "ogcB737ISFDHeadingIndicator.h"
#include "ogcB737ISFD.h"
#include "ogcB737ISFDBackground.h"

namespace OpenGC
{

B737ISFD::B737ISFD()
{
  printf("B737ISFD constructed\n");
 
  m_PhysicalSize.x = 100;
  m_PhysicalSize.y = 100;
  
  // We need a font to draw the ILS display
  m_Font = m_pFontManager->LoadDefaultFont();
  
  // We want to draw an outline
  this->SetGaugeOutline(true);

  // Create a heading indicator

  B737ISFDHeadingIndicator* pHeadingIndicator = new B737ISFDHeadingIndicator();
  pHeadingIndicator->SetParentRenderObject(this);
  pHeadingIndicator->SetPosition(17,0);
  this->AddGaugeComponent(pHeadingIndicator);

  // Create an artficial horizon
  B737ISFDArtificialHorizon* pHorizon = new B737ISFDArtificialHorizon();
  pHorizon->SetParentRenderObject(this);
  pHorizon->SetPosition(17,15);
  this->AddGaugeComponent(pHorizon);

  // Create an altitude tape
  B737ISFDAltitudeTape* pAltTape = new B737ISFDAltitudeTape();
  pAltTape->SetParentRenderObject(this);
  pAltTape->SetPosition(80,0);
  this->AddGaugeComponent(pAltTape);

  // Create an altitude ticker
  B737ISFDAltitudeTicker* pAltTicker = new B737ISFDAltitudeTicker();
  pAltTicker->SetParentRenderObject(this);
  pAltTicker->SetPosition(74,39);
  this->AddGaugeComponent(pAltTicker);
  
  // Create a speed tape
  B737ISFDSpeedTape* pSpeedTape = new B737ISFDSpeedTape();
  pSpeedTape->SetParentRenderObject(this);
  pSpeedTape->SetPosition(0,0);
  this->AddGaugeComponent(pSpeedTape);


  // Create a speed ticker
  B737ISFDSpeedTicker* pSpeedTicker = new B737ISFDSpeedTicker();
  pSpeedTicker->SetParentRenderObject(this);
  pSpeedTicker->SetPosition(0,39);
  this->AddGaugeComponent(pSpeedTicker);

  // Create all other messages and indicators on the ISFD
  B737ISFDBackground* pBackground = new B737ISFDBackground();
  pBackground->SetParentRenderObject(this);
  pBackground->SetPosition(17,87);
  this->AddGaugeComponent(pBackground);
  
}

B737ISFD::~B737ISFD()
{
  // Destruction handled by base class
}

void B737ISFD::Render()
{

  //  printf("%i \n",this->GetArg());
  
  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");
  if (*avionics_on == 1) {
    Gauge::Render();
  }
  
}

} // end namespace OpenGC


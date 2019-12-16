/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737PFD.cpp,v $

  Copyright (C) 2001-2015 by:
  Original author:
  Damion Shelton
  Contributors (in alphabetical order):
  Reto Stockli

  Last modification:
  Date:      $Date: 2015/11/24 $
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

#include <stdio.h>

#include "ogcB737ArtificialHorizon.h"
#include "ogcB737AltitudeTicker.h"
#include "ogcB737AltitudeTape.h"
#include "ogcB737SpeedTape.h"
#include "ogcB737SpeedTicker.h"
#include "ogcB737PFDHeadingIndicator.h"
#include "ogcB737VSI.h"
#include "ogcB737PFD.h"
#include "ogcB737MachNumber.h"
//#include "ogcB737GradientADI.h"

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
  this->SetGaugeOutline(true);

  // Create a heading indicator
  B737PFDHeadingIndicator* pHeadingIndicator = new B737PFDHeadingIndicator();
  pHeadingIndicator->SetParentRenderObject(this);
  pHeadingIndicator->SetPosition(29,5);
  this->AddGaugeComponent(pHeadingIndicator);

  // Create an artficial horizon
  B737ArtificialHorizon* pHorizon = new B737ArtificialHorizon();
  //B737GradientADI* pHorizon = new B737GradientADI();
  pHorizon->SetParentRenderObject(this);
  pHorizon->SetPosition(42,52);
  this->AddGaugeComponent(pHorizon);

  // Create an altitude tape
  B737AltitudeTape* pAltTape = new B737AltitudeTape();
  pAltTape->SetParentRenderObject(this);
  pAltTape->SetPosition(150,32);
  this->AddGaugeComponent(pAltTape);

  // Create an altitude ticker
  B737AltitudeTicker* pAltTicker = new B737AltitudeTicker();
  pAltTicker->SetParentRenderObject(this);
  pAltTicker->SetPosition(157,90);
  this->AddGaugeComponent(pAltTicker);

  // Create a VSI
  B737VSI* pVSI = new B737VSI();
  pVSI->SetParentRenderObject(this);
  pVSI->SetPosition(182,44);
  this->AddGaugeComponent(pVSI);

  // Create a speed tape
  B737SpeedTape* pSpeedTape = new B737SpeedTape();
  pSpeedTape->SetParentRenderObject(this);
  pSpeedTape->SetPosition(8,32);
  this->AddGaugeComponent(pSpeedTape);

  // Create a speed ticker
  B737SpeedTicker* pSpeedTicker = new B737SpeedTicker();
  pSpeedTicker->SetParentRenderObject(this);
  pSpeedTicker->SetPosition(7,90);
  this->AddGaugeComponent(pSpeedTicker);

  // Create all other messages and indicators on the PDF
  B737MachNumber* pMachNumber = new B737MachNumber();
  pMachNumber->SetParentRenderObject(this);
  pMachNumber->SetPosition(0,0);
  this->AddGaugeComponent(pMachNumber);

  

}

B737PFD::~B737PFD()
{
  // Destruction handled by base class
}

void B737PFD::Render()
{

  //  printf("%i \n",this->GetArg());
  
  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");
    if (*avionics_on == 1) {
      Gauge::Render();
    }
}

} // end namespace OpenGC


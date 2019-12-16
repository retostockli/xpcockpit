/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737PFD.cpp,v $

  Copyright (C) 2001-2 by:
    Original author:
      Michael DeFeyter
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2003/06/12 23:03:32 $
    Version:   $Revision: 1.2 $
    Author:    $Author: damion $
  
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

#include "B737/B737PFDSA/ogcB737PFDSA.h"
#include "B737/B737PFDSA/ogcB737PFDAttitude.h"
#include "B737/B737PFDSA/ogcB737PFDSpeedIndicator.h"
#include "B737/B737PFDSA/ogcB737PFDNav.h"
#include "B737/B737PFDSA/ogcB737PFDCompas.h"
#include "B737/B737PFDSA/ogcB737PFDBackground.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace OpenGC
{

B737PFDSA::B737PFDSA()
{
	printf("B737/B737PFDSA constructed\n");

	m_PhysicalSize.x = 201;
	m_PhysicalSize.y = 201;

	B737PFDAttitude* attitude = new B737PFDAttitude();
	attitude->SetParentRenderObject(this);
	attitude->SetPosition(99.5, 78);
	this->AddGaugeComponent(attitude);

	B737PFDSpeedIndicator* speedIndicator = new B737PFDSpeedIndicator();
	speedIndicator->SetParentRenderObject(this);
	speedIndicator->SetPosition(10, 90);
	this->AddGaugeComponent(speedIndicator);

	B737PFDNav* nav = new B737PFDNav();
	nav->SetParentRenderObject(this);
	nav->SetPosition(102, -38);
	nav->setBottomHide(38);
	this->AddGaugeComponent(nav);

	B737PFDCompas* compas = new B737PFDCompas();
	compas->SetParentRenderObject(this);
	compas->SetPosition(10, 0);
	this->AddGaugeComponent(compas);

	B737PFDBackground* background = new B737PFDBackground();
	background->SetParentRenderObject(this);
	background->SetPosition(0, 0);
	this->AddGaugeComponent(background);
}

B737PFDSA::~B737PFDSA()
{

}

//////////////////////////////////////////////////////////////////////

void B737PFDSA::Render()
{
    int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");
    if (*avionics_on == 1) {
	Gauge::Render();
    }
}

}

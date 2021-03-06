/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcBoeing737VerticalSpeedDigital.cpp,v $

  Copyright (C) 2001-2 by:
    Original author:
      Jurgen Roeland (Jurgen.Roeland@AISYstems.be)
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

#include "ogcBoeing737VerticalSpeedDigital.h"
#include "737/ogcBoeing737VerticalSpeedDigitalComponent.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace OpenGC
{

Boeing737VerticalSpeedDigital::Boeing737VerticalSpeedDigital()
{
	printf("Boeing737VerticalSpeedDigital\n");

	m_PhysicalSize.x = 200;
	m_PhysicalSize.y = 200;

	Boeing737VerticalSpeedDigitalComponent* FlC2 = new Boeing737VerticalSpeedDigitalComponent();
	FlC2->SetParentRenderObject(this);
	FlC2->SetPosition(0, 0);
	this->AddGaugeComponent(FlC2);
}

Boeing737VerticalSpeedDigital::~Boeing737VerticalSpeedDigital()
{

}

void Boeing737VerticalSpeedDigital::Render()
{
	Gauge::Render();
}

}

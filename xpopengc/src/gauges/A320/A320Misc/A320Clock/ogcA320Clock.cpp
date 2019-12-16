/*=============================================================================

  This is the ogcA320Clock.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Main-Panel Clock ===

  Created:
    Date:   2011-11-14
    Author: Hans Jansen

  Copyright (C) 2011-2016 Hans Jansen (hansjansen@users.sourceforge.net)
  and/or                  Reto Stöckli (stockli@users.sourceforge.net)

  This program is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>. 

===============================================================================

  The OpenGC subproject has been derived from:
    OpenGC - The Open Source Glass Cockpit Project
    Copyright (c) 2001-2003 Damion Shelton

=============================================================================*/

#include <stdio.h>

#include "ogcDataSource.h"

#include "ogcA320Clock.h"
#include "ogcA320ClockComponent.h"

namespace OpenGC
{

A320Clock::A320Clock() {

    if (verbosity > 0) printf ("A320Clock - constructing\n");

    // Specify our physical size
    m_PhysicalSize.x = 170;
    m_PhysicalSize.y = 220;

    // We do not want to draw an outline
    this->SetGaugeOutline (false);

    // The face of the Clock
    A320ClockComponent* pComp = new A320ClockComponent ();
    pComp->SetParentRenderObject (this);
    pComp->SetPosition (0, 0);
    pComp->SetSize (m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent (pComp);

    if (verbosity > 1) printf ("A320Clock - constructed\n");
  }

  A320Clock::~A320Clock () {}

  void A320Clock::Render () {
    Gauge::Render();

    if (verbosity > 1)
    {
      printf ("A320BrkTripleInd - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320BrkTripleInd -    pixel position: %i %i\n", m_PixelPosition.x,    m_PixelPosition.y);
      printf ("A320BrkTripleInd -     physical size: %f %f\n", m_PhysicalSize.x,     m_PhysicalSize.y);
      printf ("A320BrkTripleInd -        pixel size: %i %i\n", m_PixelSize.x,        m_PixelSize.y);
    }

  }

} // end namespace OpenGC

/*=============================================================================

  This is the ogcBasicClock.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  Created:
    Date:   2015-05-05
    Author: Hans Jansen

  Copyright (C) 2011-2015 Hans Jansen (hansjansen@users.sourceforge.net)
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
#include <math.h>

#include "ogcGaugeComponent.h"
#include "BasicClock/ogcBasicClock.h"
#include "BasicClock/ogcBasicClockComponent.h"

namespace OpenGC
{

  /** The Subwindows */
  BasicClockComponent* pComp;

  BasicClock::BasicClock()
  {
    if (verbosity > 0) printf("BasicClock - constructing\n");

    // Specify our physical size and position
    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;
    
    m_PhysicalSize.x = 400;
    m_PhysicalSize.y = 400;
    
    // We want to draw an outline
    this->SetGaugeOutline(true);

    // The dial of the Clock
    pComp = new BasicClockComponent();
    pComp->SetParentRenderObject(this);
    pComp->SetPosition(m_PhysicalPosition.x,m_PhysicalPosition.y);
    pComp->SetSize(m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent(pComp);

    if (verbosity > 1) printf("BasicClock - constructed\n");
  }

  BasicClock::~BasicClock()
  {
  }

  void BasicClock::Render()
  {
    Gauge::Render();

    if (verbosity > 2)
    {
      printf ("BasicClock - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("BasicClock -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("BasicClock -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("BasicClock -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }
  }

} // end namespace OpenGC

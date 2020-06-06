/*=============================================================================

  This is the ogcB737Clock.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === B737 style Main-Panel Clock ===

  Created:
    Date:        2011-11-14
    Author:      Hans Jansen
    Last change: 2020-01-22

  Copyright (C) 2011-2020 Hans Jansen (hansjansen@users.sourceforge.net)
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

#include "ogcB737Clock.h"
#include "ogcB737ClockComponent.h"

namespace OpenGC {

  B737Clock::B737Clock() {
    if (verbosity > 0) printf ("B737Clock - constructing\n");

    // Specify our physical size
    m_PhysicalSize.x = 80;
    m_PhysicalSize.y = 80;

    // We do not want to draw an outline
    this->SetGaugeOutline (false);

    // The face of the Clock
    B737ClockComponent* clkComp = new B737ClockComponent ();
    clkComp->SetParentRenderObject (this);
    clkComp->SetPosition (0, 0);
    clkComp->SetSize (m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent (clkComp);

    if (verbosity > 1) printf ("B737Clock - constructed\n");
  }

  B737Clock::~B737Clock () {
  // Destruction handled by base class
  }

  void B737Clock::Render () {

    int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");
    if (*avionics_on == 1) {

      Gauge::Render();

    }
    
  } // end Render ()

} // end namespace OpenGC

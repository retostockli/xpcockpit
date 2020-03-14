/*=============================================================================

  This is the ogcA320StbyAlt.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Standby Altitude Indicator ===

  Created:
    Date:   2018-05-03
    Author: Hans Jansen
    Last change: 2020-01-23

  Copyright (C) 2018-2020 Hans Jansen (hansjansen@users.sourceforge.net)
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

=============================================================================

  The OpenGC subproject has been derived from:
    OpenGC - The Open Source Glass Cockpit Project
    Copyright (c) 2001-2003 Damion Shelton

=============================================================================*/

#include <stdio.h>
#include <math.h>

#include "ogcA320StbyAlt.h"
#include "ogcA320StbyAltComponent.h"

namespace OpenGC {

  /** The Subwindows */
  A320StbyAltComponent* altComp;

  A320StbyAlt::A320StbyAlt () {
  
    if (verbosity > 0) printf ("A320StbyAlt - constructing\n");

    // Specify our physical size
    m_PhysicalSize.x = 170;
    m_PhysicalSize.y = 170;

    // We want to draw an outline
    this->SetGaugeOutline (true);

    // A Component of the Gauge
    altComp = new A320StbyAltComponent ();
    altComp->SetParentRenderObject (this);
    altComp->SetPosition (0, 0);
    altComp->SetSize (m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent (altComp);

    if (verbosity > 1) printf ("A320StbyAlt - constructed\n");
  }

  A320StbyAlt::~A320StbyAlt () {}

  void A320StbyAlt::Render () {
  
    Gauge::Render ();

    if (verbosity > 1) {
      printf ("A320StbyAlt - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320StbyAlt -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320StbyAlt -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320StbyAlt -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

  } // end Render()

} // end namespace OpenGC

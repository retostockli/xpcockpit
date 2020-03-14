/*=============================================================================

  This is the ogcA320StbyISIS.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Integrated Standby Instrument System ===

  Created:
    Date:   2018-05-03
    Author: Hans Jansen
    Last change: 2020-01-24

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

#include "ogcA320StbyISIS.h"
#include "ogcA320StbyISISComponent.h"

namespace OpenGC {

  /** The Subwindows */
  A320StbyISISComponent* isisComp;

  A320StbyISIS::A320StbyISIS () {
      if (verbosity > 0) printf ("A320StbyISIS - constructing\n");

    // Specify our physical size
    m_PhysicalSize.x = 220;
    m_PhysicalSize.y = 220;

    // We want to draw an outline
    this->SetGaugeOutline (true);

    // A Component of the Gauge
    isisComp = new A320StbyISISComponent ();
    isisComp->SetParentRenderObject (this);
    isisComp->SetPosition (0, 0);
    isisComp->SetSize (m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent (isisComp);

    if (verbosity > 1) printf ("A320StbyISIS - constructed\n");
  }

  A320StbyISIS::~A320StbyISIS (){}

  void A320StbyISIS::Render () {
    Gauge::Render ();

    if (verbosity > 1) {
      printf ("A320StbyISIS - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320StbyISIS -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320StbyISIS -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320StbyISIS -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

  }

} // end namespace OpenGC

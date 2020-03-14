/*=============================================================================

  This is the ogcA320StbyAtt.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Standby Attitude Indicator ===

  Created:
    Date:   2018-05-03
    Author: Hans Jansen
    last change: 2020-01-24

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

#include "ogcA320StbyAtt.h"
#include "ogcA320StbyAttComponent.h"

namespace OpenGC {

  /** The Subwindows */
  A320StbyAttComponent* attComp;

  A320StbyAtt::A320StbyAtt () {
  
    if (verbosity > 0) printf ("A320StbyAtt - constructing\n");

    // Specify our physical size
    m_PhysicalSize.x = 220;
    m_PhysicalSize.y = 220;

    // We want to draw an outline
    this->SetGaugeOutline (true);

    // A Component of the Gauge
    attComp = new A320StbyAttComponent ();
    attComp->SetParentRenderObject (this);
    attComp->SetPosition (0, 0);
    attComp->SetSize (m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent (attComp);

    if (verbosity > 1) printf ("A320StbyAtt - constructed\n");
  }

  A320StbyAtt::~A320StbyAtt () {}

  void A320StbyAtt::Render () {
  
    Gauge::Render ();

    if (verbosity > 1) {
      printf ("A320StbyAtt - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320StbyAtt -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320StbyAtt -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320StbyAtt -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

  } // end Render()

} // end namespace OpenGC

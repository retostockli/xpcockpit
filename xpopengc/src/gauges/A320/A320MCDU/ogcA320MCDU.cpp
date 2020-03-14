/*=============================================================================

  This is the ogcA320MCDU.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Multi-Purpose Control and Display Unit ===
  === (display part only; keyboard input is done via the USBIoCards module) ===

  Created:
    Date:        2015-06-14
    Author:      Hans Jansen
    Last change: 2020-02-06

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

/*=============================================================================

  This instrument may use following (..) datarefs from QPAC A320 Basic V2.02:
  (actual usage still to be determined...)

  //AirbusFBW/...

=============================================================================*/

#include <stdio.h>

#include "ogcDataSource.h"
#include "ogcCircleEvaluator.h"

#include "ogcA320MCDU.h"
#include "ogcA320MCDUWidget.h"

namespace OpenGC
{

  /** The Subwindow */
  A320MCDUWidget* pMCDUWidget;

  A320MCDU::A320MCDU () {
    if (verbosity > 1) printf ("A320MCDU constructing\n");

    // Specify our physical size and rotation
    m_PhysicalSize.x = 1024; // should be 800 on normal monitor (Sony overdraws!)
    m_PhysicalSize.y = 768;

    // We do not want to draw an outline
    this->SetGaugeOutline (false);

    // Create the widget
    pMCDUWidget = new A320MCDUWidget ();
    pMCDUWidget->SetParentRenderObject (this);
    pMCDUWidget->SetPosition (0, 0);
    pMCDUWidget->SetSize (m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent (pMCDUWidget);

    if (verbosity > 0) printf("A320MCDU - constructed\n");
  }

  A320MCDU::~A320MCDU () {}

  void A320MCDU::Render () {
    Gauge::Render ();

    if (verbosity > 2) {
      printf ("A320MCDU - Physical position: x %f, y %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320MCDU - Pixel position:    x %i, y %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320MCDU - Physical size:     x %f, y %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320MCDU - Pixel size:        x %i, y %i\n", m_PixelSize.x, m_PixelSize.y);
    }

  } // end Render()

} // end namespace OpenGC

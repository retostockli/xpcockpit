/*=============================================================================

  This is the A320SD.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style System Display ===

  Created:
    Date:        2011-11-14
    Author:      Hans Jansen
    Last change: 2020-02-02

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

#include "ogcA320SD.h"
#include "ogcA320SDWidget.h"

namespace OpenGC
{

  bool CdStateSd = true;
  bool coldDarkSd () { return CdStateSd; }

  /** The Subwindow */
  A320SDWidget* SDWidget;

  A320SD::A320SD () {
    if (verbosity > 1) printf ("A320SD constructing\n");

    // Specify our physical size and rotation
    m_PhysicalSize.x = 460;
    m_PhysicalSize.y = 460;

    // We do not want to draw an outline
    this->SetGaugeOutline (false);

    // Create the widget
    SDWidget = new A320SDWidget ();
    SDWidget->SetParentRenderObject (this);
    SDWidget->SetPosition (0, 0);
    SDWidget->SetSize (m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent (SDWidget);

    if (verbosity > 0) printf("A320SD - constructed\n");
  }

  A320SD::~A320SD () {}

  void A320SD::SetRotation (int rot) {
    SDWidget->SetRotation (rot);
  }

  void A320SD::Render () {
    Gauge::Render ();

    if (verbosity > 2)
    {
      printf ("A320SD - Physical position: x %f, y %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320SD - Pixel position:    x %i, y %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320SD - Physical size:     x %f, y %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320SD - Pixel size:        x %i, y %i\n", m_PixelSize.x, m_PixelSize.y);
    }

  } // end Render()

} // end namespace OpenGC

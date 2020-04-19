/*=============================================================================

  This is the ogcA320EWDFuel.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Engine/Warning Display - Fuel-On-Board Indicator ===

  Created:
    Date:        2011-11-14
    Author:      Hans Jansen
    last change: 2020-02-06
    (see ogcSkeletonGauge.cpp for more details)

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

#include <math.h>

#include "ogcA320EWD.h"
#include "ogcA320EWDFuel.h"

namespace OpenGC {

  A320EWDFuel::A320EWDFuel () {
    printf("A320EWDFuel constructed\n");

    m_Font = m_pFontManager->LoadFont((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  A320EWDFuel::~A320EWDFuel () {}

  void A320EWDFuel::Render () {

    bool coldAndDark = true;

    GaugeComponent::Render ();

    // The datarefs we want to use on this instrument

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    float totFOB = 0;
    float *tot_fuel = link_dataref_flt ("sim/flightmodel/weight/m_fuel_total", 0);
    if (*tot_fuel != FLT_MISS) totFOB = *tot_fuel;

    if (!coldAndDark) {

      // The instrument display
      char buffer[32];
      glColor3ub (COLOR_GREEN);
      m_pFontManager->SetSize (m_Font, 14, 14);
      sprintf (buffer, "%5i", ((int) (totFOB + 10) / 20) * 20);
      m_pFontManager->Print ( 50,   5, buffer, m_Font); // total FOB

    } // End if (!coldAndDark)

  } // End Render ()

}

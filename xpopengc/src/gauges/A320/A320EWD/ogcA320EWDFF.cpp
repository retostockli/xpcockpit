/*=============================================================================

  This is the ogcA320EWDFF.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Engine/Warning Display - Fuel Flow Indicator ===

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
#include "ogcA320EWDFF.h"

namespace OpenGC {

  A320EWDFF::A320EWDFF ()   {
    printf("A320EWDFF constructed\n");

    m_Font = m_pFontManager->LoadFont((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    m_Engine = 1;
  }

  A320EWDFF::~A320EWDFF () {}

  void A320EWDFF::setEngine (int engine)   {
    m_Engine = engine;
  }

  void A320EWDFF::Render () {

    bool coldAndDark = true;
    char buffer[32];

    GaugeComponent::Render ();

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    // The datarefs we want to use on this instrument
    float engFF = 0;
    float *eng_ff = link_dataref_flt_arr ("sim/cockpit2/engine/indicators/fuel_flow_kg_sec",8,-1,-4);
      if (*eng_ff != FLT_MISS) engFF = eng_ff[m_Engine-1] * 3600;

    if (!coldAndDark) {

      // The instrument display
      glColor3ub (COLOR_GREEN);
      m_pFontManager->SetSize (m_Font, 14, 14);
      sprintf(buffer, "%4i", ((int) ((engFF + 9) / 10) * 10));
      m_pFontManager->Print (0, 3, buffer, m_Font); // Eng 1 FF

    } // End if (!coldAndDark)

  } // End Render()

}

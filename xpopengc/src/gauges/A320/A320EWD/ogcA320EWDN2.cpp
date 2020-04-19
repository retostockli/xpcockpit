/*=============================================================================

  This is the ogcA320EWDN2.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Engine/Warning Display - N2 Indicator ===

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
#include "ogcA320EWDN2.h"

namespace OpenGC {

  A320EWDN2::A320EWDN2 () {
    printf( "A320EWDN2 constructed\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    m_Engine = 1;
  }

  A320EWDN2::~A320EWDN2 () {}

  void A320EWDN2::setEngine (int engine) {
    m_Engine = engine;
  }

  void A320EWDN2::Render () {

    bool coldAndDark = true;
    char buffer[32];

    GaugeComponent::Render ();

    // The datarefs we want to use on this instrument

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    float engN1a = 0;
    float *eng_n1a = link_dataref_flt_arr ("sim/flightmodel/engine/ENGN_N1_",8,-1,-2);
    if (*eng_n1a != FLT_MISS) engN1a = eng_n1a[m_Engine-1];
    float engN2 = 0;
    float *eng_n2 = link_dataref_flt_arr ("sim/flightmodel/engine/ENGN_N2_",8,-1,-2);
    if (*eng_n2 != FLT_MISS) engN2 = eng_n2[m_Engine-1];

    if (!coldAndDark) {

      // The instrument display
      if (engN2 < 3.4) {
        glLineWidth (2);
        glColor3ub (COLOR_BLACK);
        glBegin (GL_POLYGON);
          glVertex2f( 0.0, 0.0 );
          glVertex2f( 0.0, m_PhysicalSize.y );
          glVertex2f( m_PhysicalSize.x, m_PhysicalSize.y );
          glVertex2f( m_PhysicalSize.x, 0.0 );
        glEnd ();

        m_pFontManager->SetSize (m_Font, 14, 14);
        glColor3ub (COLOR_AMBER);
        m_pFontManager->Print (10, 3, "XX",   m_Font);

      } else {
        m_pFontManager->SetSize (m_Font, 14, 14);
        glColor3ub (COLOR_GREEN);
        sprintf (buffer, "%4.1f", engN2);
        m_pFontManager->Print (0, 3, buffer, m_Font);
      } // End engN2 Low

    } // End if (!coldAndDark)

  } // End Render()

}

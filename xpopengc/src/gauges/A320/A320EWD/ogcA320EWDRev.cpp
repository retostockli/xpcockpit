/*=============================================================================

  This is the ogcA320EWDRev.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Engine/Warning Display - Reverser Indicator ===

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
#include "ogcA320EWDRev.h"

namespace OpenGC {

  A320EWDRev::A320EWDRev () {
    printf ("A320EWDRev constructed\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    m_Engine = 1;
  }

  A320EWDRev::~A320EWDRev () {}

  void A320EWDRev::setEngine (int engine) {
    m_Engine = engine;
  }

  void A320EWDRev::Render () {

    bool coldAndDark = true;

    GaugeComponent::Render ();
    // The datarefs we want to use on this instrument

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    int engFadecState = 0;
    int *eng_fadec_state = link_dataref_int_arr ("AirbusFBW/FADECStateArray", 4, -1);
    if (*eng_fadec_state != INT_MISS) engFadecState = eng_fadec_state[m_Engine-1];
    int engRevSw = 0;
    int *eng_rev_sw = link_dataref_int_arr ("AirbusFBW/ENGRevArray", 4, -1);
    if (*eng_rev_sw != INT_MISS) engRevSw = eng_rev_sw[m_Engine-1];

    if (!coldAndDark) {

      // The instrument display
      if (engFadecState != 0) {
        switch (engRevSw) {
          case 1: // Reverser in transit
            glColor3ub (COLOR_AMBER);
            glLineWidth (2);
            glBegin (GL_LINE_LOOP);
              glVertex2d ( 1,  1);
              glVertex2d ( 1, 15);
              glVertex2d (31, 15);
              glVertex2d (31,  1);
            glEnd ();
            m_pFontManager->SetSize (m_Font, 10, 10);
            m_pFontManager->Print (4, 3, "REV", m_Font);
            break;
          case 2: // Reverser open
            glColor3ub (COLOR_GREEN);
            glLineWidth (2);
            glBegin (GL_LINE_LOOP);
              glVertex2d  (1,  1);
              glVertex2d ( 1, 15);
              glVertex2d (31, 15);
              glVertex2d (31,  1);
            glEnd ();
            m_pFontManager->SetSize (m_Font, 10, 10);
            m_pFontManager->Print (4, 3, "REV", m_Font);
            break;
          default:
            break;
        } // End switch ()

      } // End FADEC On

    } // End if (!coldAndDark)

  } // End Render()

}

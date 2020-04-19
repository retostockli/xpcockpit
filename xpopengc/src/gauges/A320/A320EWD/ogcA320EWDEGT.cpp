/*=============================================================================

  This is the ogcA320EWDEGT.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Engine/Warning Display - EGT Dial ===

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
#include "ogcA320EWDEGT.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320EWDEGT::A320EWDEGT () {
    printf ("A320EWDEGT constructed\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    m_Engine = 0;
  }

  A320EWDEGT::~A320EWDEGT () {}

  void A320EWDEGT::setEngine (int engine) {
    m_Engine = engine;
  }

  void A320EWDEGT::Render () {

    bool coldAndDark = true;
    CircleEvaluator aCircle;
    int value = 0;
    char buffer[32];

    GaugeComponent::Render ();

    // The datarefs we want to use on this instrument

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    int engFadecState = 0;
    int *eng_fadec_state = link_dataref_int_arr ("AirbusFBW/FADECStateArray", 4, -1);
    if (*eng_fadec_state != INT_MISS) engFadecState = eng_fadec_state[m_Engine-1];
    float engEgt = 0;
    float *eng_egt = link_dataref_flt_arr ("sim/cockpit2/engine/indicators/EGT_deg_C",8,-1,0);
    if (*eng_egt != FLT_MISS) engEgt = eng_egt[m_Engine-1];

    if (!coldAndDark) {

      // The instrument display

      if (engFadecState == 0) { // FADEC off
        glColor3ub (COLOR_BLACK);
        glLineWidth (2);
        glBegin (GL_POLYGON); // ( EPR/EGT dials )
          glVertex2d (0, 0);
          glVertex2d (0, 50);
          glVertex2d (120, 50);
          glVertex2d (120, 0);
        glEnd ();

        aCircle.SetOrigin (50, 50);
        aCircle.SetRadius (45);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (270, 90);
        glLineWidth (3);
        glColor3ub (COLOR_AMBER);
        glBegin (GL_LINE_STRIP);
          aCircle.Evaluate ();
        glEnd ();

        m_pFontManager->SetSize (m_Font, 14, 14);
        glColor3ub (COLOR_AMBER);
        m_pFontManager->Print (40, 0, "XX", m_Font); // EGT
      } else { // FADEC on
        if (engEgt != FLT_MISS) {
          aCircle.SetOrigin (50, 0);
          aCircle.SetRadius (45);
          aCircle.SetDegreesPerPoint (5);
          aCircle.SetArcStartEnd (270, 60);
          glLineWidth (3);
          glColor3ub (COLOR_WHITE);
          glBegin (GL_LINE_STRIP);
            aCircle.Evaluate ();
          glEnd ();
          aCircle.SetRadius (43);
          aCircle.SetArcStartEnd (60, 90);
          glLineWidth (7);
          glColor3ub (COLOR_RED);
          glBegin (GL_LINE_STRIP);
            aCircle.Evaluate ();
          glEnd ();
          aCircle.SetRadius (43);
          aCircle.SetArcStartEnd (63, 87);
          glLineWidth (2);
          glColor3ub (COLOR_BLACK);
          glBegin (GL_LINE_STRIP);
            aCircle.Evaluate ();
          glEnd ();
          glColor3ub (COLOR_WHITE);
          glLineWidth (2);
          glPushMatrix ();
            glTranslated (50, 0, 0);
            glBegin (GL_LINES);
              glVertex2d (0, 35);
              glVertex2d (0, 45);
            glEnd ();
            glRotated (90, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2d (0, 35);
              glVertex2d (0, 45);
            glEnd ();
          glPopMatrix ();

          glColor3ub (COLOR_GREEN);
          glLineWidth (3);
          glPushMatrix ();
            glTranslated (50, 0, 0);
            glRotated (90, 0, 0, 1);
            value = (engEgt * 150 / 700);
            if (value > 150) value = 150;
            glRotated (-value, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2d (0, 25);
              glVertex2d (0, 45);
            glEnd ();
          glPopMatrix ();

          m_pFontManager->SetSize (m_Font, 14, 14);
          sprintf (buffer, "%4.0f", engEgt);
          m_pFontManager->Print (35, 3, buffer, m_Font);
        }

      } // End FADEC Off

    } // End if (!coldAndDark)

  } // End Render()

} // end namespace OpenGC

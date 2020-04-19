/*=============================================================================

  This is the ogcA320EWDEPR.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Engine/Warning Display - EPR Dial ===

  Created:
    Date:        2011-11-14
    Author:      Hans Jansen
    last change: 2020-02-06
    (see ogcSkeletonGauge.cpp for more details)

  Copyright (C) 2011-2020 Hans Jansen (hansjansen@users.sourceforge.net)
  and/or                  Reto Stöckli (stockli@users.sourceforge.net)

  Copyright (C) 2011-2016 Hans Jansen (hansjansen@users.sourceforge.net)
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
#include "ogcA320EWDEPR.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320EWDEPR::A320EWDEPR () {
    printf ("A320EWDEPR constructed\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    m_Engine = 0;
  }

  A320EWDEPR::~A320EWDEPR () {}

  void A320EWDEPR::setEngine (int engine) {
    m_Engine = engine;
  }

  void A320EWDEPR::Render ()  {

    bool coldAndDark = true;
    CircleEvaluator aCircle;
    int value = 0, rot = 0;
    char buffer[32];

    GaugeComponent::Render ();

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    // The datarefs we want to use on this instrument
    int engFadecState = 0;
    int *eng_fadec_state = link_dataref_int_arr ("AirbusFBW/FADECStateArray", 4, -1);
    if (*eng_fadec_state != INT_MISS) engFadecState = eng_fadec_state[m_Engine-1];
    float engEpr = 0;
    float *eng_epr = link_dataref_flt_arr ("AirbusFBW/ENGEPRArray",4, -1, -4);
    if (*eng_epr != FLT_MISS) engEpr = eng_epr[m_Engine-1];
    int engRevSw = 0;
    int *eng_rev_sw = link_dataref_int_arr ("AirbusFBW/ENGRevArray", 4, -1);
    if (*eng_rev_sw != INT_MISS) engRevSw = eng_rev_sw[m_Engine-1];

    if (!coldAndDark) {

      // The instrument display
      if (engFadecState == 0) {
        glColor3ub (COLOR_BLACK);
        glLineWidth (2);
        glBegin (GL_POLYGON); // ( EPR/EGT dials )
          glVertex2d (0, 0);
          glVertex2d (0, 100);
          glVertex2d (120, 100);
          glVertex2d (120, 0);
        glEnd ();
        glBegin (GL_POLYGON); // ( EPR Numerals )
          glVertex2d (120,  0);
          glVertex2d (120, 31);
          glVertex2d (136, 31);
          glVertex2d (136,  0);
        glEnd ();

        aCircle.SetOrigin (60, 40);
        aCircle.SetRadius (55);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (230, 95);
        glLineWidth (3);
        glColor3ub (COLOR_AMBER);
        glBegin(GL_LINE_STRIP);
          aCircle.Evaluate ();
        glEnd();

        m_pFontManager->SetSize (m_Font, 14, 14);
        glColor3ub (COLOR_AMBER);
        m_pFontManager->Print (50, 40, "XX", m_Font); // REV
        m_pFontManager->Print (90, 10, "XX", m_Font); // EPR
      } else { // FADEC on
        m_pFontManager->SetSize (m_Font, 10, 12);
        aCircle.SetOrigin (60, 40);
        aCircle.SetRadius (55);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (230, 95);
        glLineWidth (3);
        glColor3ub (COLOR_WHITE);
        glBegin(GL_LINE_STRIP);
          aCircle.Evaluate ();
        glEnd();
        glColor3ub (COLOR_WHITE);
        glLineWidth (2);
        glPushMatrix ();
          glTranslated (60, 40, 0);
          glRotated (-90, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2d (0, 45);
            glVertex2d (0, 55);
          glEnd ();
          for (rot = 0; rot < 3; rot++) {
            glRotated (70, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2d (0, 45);
              glVertex2d (0, 55);
            glEnd ();
          }
        glPopMatrix ();
        m_pFontManager->Print (75, 34, "1.6", m_Font);
        m_pFontManager->Print (62, 68, "1.4", m_Font);
        m_pFontManager->Print (28, 56, "1.2", m_Font);
        m_pFontManager->Print (26, 14, "1", m_Font);
        glLineWidth (2);
        glBegin (GL_LINE_LOOP);
          glVertex2d ( 50,  5);
          glVertex2d ( 50, 30);
          glVertex2d (135, 30);
          glVertex2d (135,  5);
        glEnd ();
        aCircle.SetOrigin (60, 40);
        aCircle.SetRadius (55);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (230, 95);
        glLineWidth (3);
        glColor3ub (COLOR_WHITE);
        glBegin(GL_LINE_STRIP);
          aCircle.Evaluate ();
        glEnd();
        glColor3ub (COLOR_WHITE);
        glLineWidth (2);
        glPushMatrix ();
         glTranslated (60, 40, 0);
         glRotated (-90, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2d (0, 45);
            glVertex2d (0, 55);
          glEnd ();
          for (rot = 0; rot < 3; rot++) {
            glRotated (70, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2d (0, 45);
              glVertex2d (0, 55);
            glEnd ();
          }
        glPopMatrix ();
        m_pFontManager->Print (75, 34, "1.6", m_Font);
        m_pFontManager->Print (62, 68, "1.4", m_Font);
        m_pFontManager->Print (28, 56, "1.2", m_Font);
        m_pFontManager->Print (26, 14, "1", m_Font);
        glLineWidth (2);
        glBegin (GL_LINE_LOOP);
          glVertex2d ( 50,  5);
          glVertex2d ( 50, 30);
          glVertex2d (135, 30);
          glVertex2d (135,  5);
        glEnd ();

        glColor3ub (COLOR_GREEN);
        glLineWidth (4);
        glPushMatrix ();
          glTranslated (60, 40, 0);
          glRotated (120, 0, 0, 1);
          value = (engEpr * 1000 - 1000) * 210 / 600;
          if (value < 0) value = 0; if (value > 210) value = 210;
          glRotated (-value, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2d (0,  5);
            glVertex2d (0, 55);
          glEnd ();
        glPopMatrix ();

        glColor3ub (COLOR_GREEN);
        m_pFontManager->SetSize (m_Font, 16, 16);
        sprintf (buffer, "%1.3f", engEpr);
        m_pFontManager->Print (60, 10, buffer, m_Font);
      } // End FADEC Off

    } // End if (!coldAndDark)

  } // End Render()

}

/*=============================================================================

  This is the ogcA320EWDN1.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Engine/Warning Display - N1 Dial ===

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
#include "ogcA320EWDN1.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320EWDN1::A320EWDN1 () {
    printf("A320EWDN1 constructed\n");

    m_Font = m_pFontManager->LoadFont((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    m_Engine = 1;
  }

  A320EWDN1::~A320EWDN1 () {}

  void A320EWDN1::setEngine (int engine) {
    m_Engine = engine;
  }

  void A320EWDN1::Render () {

    bool coldAndDark = true;
    CircleEvaluator aCircle;
    int rot = 0, value = 0;
    char buffer[32];

    GaugeComponent::Render ();

    // The datarefs we want to use on this instrument

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    float engN1a = 0;
    float *eng_n1a = link_dataref_flt_arr ("sim/flightmodel/engine/ENGN_N1_",8,-1,-2);
    if (*eng_n1a != FLT_MISS) engN1a = eng_n1a[m_Engine-1];
    float engN1 = 0;
    float *eng_n1 = link_dataref_flt_arr ("sim/cockpit2/engine/indicators/N1_percent",8,-1,0);
    if (*eng_n1 != FLT_MISS) engN1 = eng_n1[m_Engine-1];

    if (!coldAndDark) {
      // The instrument display
      if (engN1a < 3.4) { // i.e. N1 low...
   
        glLineWidth (2);
        glColor3ub (COLOR_BLACK);
        glBegin (GL_POLYGON);
          glVertex2d (  0,   0);
          glVertex2d (  0, 100);
          glVertex2d (120, 100);
          glVertex2d (120,   0);
        glEnd ();

        aCircle.SetOrigin (60, 40);
        aCircle.SetRadius (55);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (230, 90);
        glLineWidth (3);
        glColor3ub (COLOR_AMBER);
        glBegin (GL_LINE_STRIP);
          aCircle.Evaluate ();
        glEnd ();

        m_pFontManager->SetSize(m_Font, 14, 14);
        glColor3ub (COLOR_AMBER);
        m_pFontManager->Print (90, 20, "XX", m_Font);

      } else {
        glColor3ub (COLOR_WHITE);
        aCircle.SetOrigin (60, 40);
        aCircle.SetRadius (55);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (230, 60);
        glLineWidth (3);
        glColor3ub (COLOR_WHITE);
        glBegin (GL_LINE_STRIP);
          aCircle.Evaluate ();
        glEnd ();
        aCircle.SetRadius (53);
        aCircle.SetArcStartEnd (60, 90);
        glLineWidth (7);
        glColor3ub (COLOR_RED);
        glBegin (GL_LINE_STRIP);
          aCircle.Evaluate ();
        glEnd ();
        aCircle.SetRadius (53);
        glLineWidth (2);
        aCircle.SetArcStartEnd (63, 87);
        glColor3ub (COLOR_BLACK);
        glBegin (GL_LINE_STRIP);
          aCircle.Evaluate ();
        glEnd ();
        glColor3ub (COLOR_WHITE);
        glLineWidth (2);
        glPushMatrix ();
          glTranslated (60, 40, 0);
          glRotated (-60, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2d (0, 45);
            glVertex2d (0, 55);
          glEnd ();
          for (rot = 0; rot < 3; rot++) {
            glRotated (60, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2d (0, 45);
              glVertex2d (0, 55);
            glEnd ();
          }
        glPopMatrix ();
        glColor3ub (COLOR_WHITE);
        m_pFontManager->Print (25, 52, "4", m_Font);
        m_pFontManager->Print (75, 52, "10", m_Font);

      glColor3ub (COLOR_GREEN);
        glLineWidth (4);
        glPushMatrix ();
          glTranslated (60, 40, 0);
          glRotated (120, 0, 0, 1);
          value = (engN1 - 20) * 180 / 80;
          if (value < 0) value = 0; if (value > 210) value = 210;
          glRotated (-value, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2d (0,  0);
            glVertex2d (0, 55);
          glEnd ();
        glPopMatrix ();

       glColor3ub (COLOR_GREEN);
       sprintf (buffer, "%4.1f", engN1);
        m_pFontManager->Print (70, 20, buffer, m_Font);
      } // End N1 low

    } // End if (!coldAndDark)

  } // End Render()

}

/*=============================================================================

  This is the ogcA320NDIls.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Navigation Display ===
       (based on Reto's ogcB737NAV stuff)

  Created:
    Date:   2011-11-14
    Author: Hans Jansen
    last change: 2020-02-01
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

#include <stdio.h>
#include <math.h>

#include "ogcGaugeComponent.h"
#include "ogcA320ND.h"
#include "ogcA320NDIls.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320NDIls::A320NDIls () {
    if (verbosity > 0) printf ("ogcA320NDIls - constructing\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("ogcA320NDIls - constructed\n");
  }

  A320NDIls::~A320NDIls () {}

  static char buff[80];

  // The variables defined in the A320NDWidget
  extern bool avionicsOn;
  extern int headingValid, ndPage, ndRange;
  extern int ilsVDefVis, ilsHDefVis;

  extern float hdgMag, gndSpd, airSpd, wndDirT, wndDirM, wndSpd, chronoCapt;
  extern float crsIls, hdefIls, vdefIls, freqIls;
  extern float wptDist, wptCrs;
  extern unsigned char *ils_name;

  void A320NDIls::Render () {

    bool coldAndDark = true;
    CircleEvaluator aCircle;

    // define map center position
    int mapSize    = m_PhysicalSize.x * 0.5;  // half of total component size (just for name simplification <- square gauge!)
    int mapCenterX = mapSize * 1.0, mapCenterY = mapSize *1.0; // defines arc center (now resp. 250 and 250)
    int mapCircle  = mapSize * 0.64; // radius of the outer range circle (now 160)
    int mapCircle1 = mapSize * 0.32; // radius of the inner range circle (now  80)
    int mapRange;

    // Call base class to setup for size and position
    GaugeComponent::Render ();

    float fontSize = 12;

    if (verbosity > 2) {
      printf ("A320NDIls - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320NDIls -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320NDIls -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320NDIls -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    if ((coldAndDark == 0) && (avionicsOn == 1) && (ndPage == 0)) {
      glPushMatrix ();
        glColor3ub (COLOR_WHITE);
        m_pFontManager->SetSize (m_Font, 18, 18);
        m_pFontManager->Print (220, 470, "ILS", m_Font );

        // The ILS data
        m_pFontManager->SetSize(m_Font, 14, 14);
        glColor3ub (COLOR_WHITE);
        m_pFontManager->Print (370, 480, "ILS2", m_Font);
        m_pFontManager->Print (408, 458, "CRS", m_Font);
        glColor3ub (COLOR_MAGENTA);
        sprintf (buff, "%6.2f", freqIls);
        m_pFontManager->Print (425, 480, buff, m_Font);
        sprintf (buff, "%03d", (int) crsIls);
        m_pFontManager->Print (450, 458, buff, m_Font);
        sprintf (buff, "%s", ils_name);
        m_pFontManager->Print (435, 436, buff, m_Font);
        glColor3ub (COLOR_CYAN);
        m_pFontManager->Print (483, 458, "\260", m_Font); // the "degree" sign

        if (headingValid == 1) {

          // The Deviation Scales and Indicators
          glPushMatrix (); // ILS Glidepath
            glColor3ub (COLOR_WHITE);
            glTranslated (480, 250, 0);
            aCircle.SetRadius (4);
            aCircle.SetDegreesPerPoint (5);
            aCircle.SetOrigin (  0,  100); aCircle.SetArcStartEnd (0, 360); glBegin (GL_TRIANGLE_FAN); aCircle.Evaluate (); glEnd();
            aCircle.SetOrigin (  0,   50); aCircle.SetArcStartEnd (0, 360); glBegin (GL_TRIANGLE_FAN); aCircle.Evaluate (); glEnd();
            aCircle.SetOrigin (  0,  -50); aCircle.SetArcStartEnd (0, 360); glBegin (GL_TRIANGLE_FAN); aCircle.Evaluate (); glEnd();
            aCircle.SetOrigin (  0, -100); aCircle.SetArcStartEnd (0, 360); glBegin (GL_TRIANGLE_FAN); aCircle.Evaluate (); glEnd();
            glColor3ub (COLOR_YELLOW);
            glLineWidth (3);
            glBegin (GL_LINES);
              glVertex2d ( -10,   0); glVertex2d (  10,   0);
            glEnd ();
            if (ilsVDefVis == 1) {
              glTranslated ( 0, -50 * vdefIls, 0);
              glColor3ub (COLOR_MAGENTA);
              glBegin (GL_LINE_STRIP); // (glidepath)
                glVertex2d (  10,    0); glVertex2d (   0,    5);
                glVertex2d ( -10,    0); glVertex2d (   0,   -5);
                glVertex2d (  10,    0);
              glEnd ();
            }
          glPopMatrix ();

          glPushMatrix (); // ILS localizer radial
            glTranslated (250, 250, 0);
            glRotatef (hdgMag - crsIls, 0, 0, 1);
            glColor3ub (COLOR_WHITE);
            aCircle.SetRadius (4);
            aCircle.SetDegreesPerPoint (5);
            aCircle.SetOrigin (-100,   0); aCircle.SetArcStartEnd (0, 360); glBegin (GL_TRIANGLE_FAN); aCircle.Evaluate (); glEnd();
            aCircle.SetOrigin ( -50,   0); aCircle.SetArcStartEnd (0, 360); glBegin (GL_TRIANGLE_FAN); aCircle.Evaluate (); glEnd();
            aCircle.SetOrigin (  50,   0); aCircle.SetArcStartEnd (0, 360); glBegin (GL_TRIANGLE_FAN); aCircle.Evaluate (); glEnd();
            aCircle.SetOrigin ( 100,   0); aCircle.SetArcStartEnd (0, 360); glBegin (GL_TRIANGLE_FAN); aCircle.Evaluate (); glEnd();
            glColor3ub (COLOR_MAGENTA);
            glLineWidth (2);
            glBegin (GL_LINES); // (localizer)
              glVertex2d (   0, -160); glVertex2d (   0, -100);
              glVertex2d (   0,  100); glVertex2d (   0,  160);
              glVertex2d ( -20,  140); glVertex2d (  20,  140);
            glEnd ();
            if (ilsHDefVis == 1) {
              glPushMatrix (); // (localizer)
                glTranslated (50 * hdefIls,   0,   0);
                glBegin (GL_LINES);
                  glVertex2d (   0, -100); glVertex2d (   0,  100);
                glEnd ();
              glPopMatrix ();
            }
          glPopMatrix ();

          // The Plane Symbol
          glPushMatrix ();
            glLineWidth (4);
            glColor3ub (COLOR_YELLOW);
            glTranslated (250, 250, 0);
            glBegin (GL_LINES);
              glVertex2d (-25,   0); glVertex2d ( 25 ,  0);
              glVertex2d (-10, -30); glVertex2d ( 10, -30);
              glVertex2d (  0,  15); glVertex2d (  0, -35);
            glEnd ();
          glPopMatrix ();

        } // end of (headingValid == 1)

      glPopMatrix ();
    
    } // end if (! coldAndDark ... ())

  } // end Render()

} // end namespace OpenGC

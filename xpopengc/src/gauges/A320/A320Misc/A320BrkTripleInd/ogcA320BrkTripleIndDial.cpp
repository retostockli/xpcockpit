/*=============================================================================

  This is the ogcA320BrkTripleIndDial.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Triple Brake Indicator: the Dials ===

  Created:
    Date:   2015-06-14
    Author: Hans Jansen
    Last change: 2020-01-22

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
#include "ogcA320BrkTripleIndDial.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320BrkTripleIndDial::A320BrkTripleIndDial () {
    if (verbosity > 0) printf("ogcA320BrkTripleIndDial - constructing\n");

    m_Font = m_pFontManager->LoadFont((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 2) printf("ogcA320BrkTripleIndDial - constructed\n");
  }

  A320BrkTripleIndDial::~A320BrkTripleIndDial () {}

  void A320BrkTripleIndDial::Render () {

    // Call base class to setup for size and position
    GaugeComponent::Render ();

    // For drawing circles
    CircleEvaluator aCircle;

    float fontSize = 20;

    // Request the datarefs we want to use
    // Note: these seem to be inactive!
    float *brkAccu  = link_dataref_flt ("AirbusFBW/BrakeAccu", -2);
    if (*brkAccu == FLT_MISS) *brkAccu = 0.0;
    float *brkLeft  = link_dataref_flt ("AirbusFBW/TotLeftBrake", -2);
    if (*brkLeft == FLT_MISS) *brkLeft = 0.0;
    float *brkRight = link_dataref_flt ("AirbusFBW/TotRightBrake", -2);
    if (*brkRight == FLT_MISS) *brkRight = 0.0;
    float ndlRot = 0.0;

    if (verbosity > 1) {
      printf ("A320BrkTripleIndDial - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320BrkTripleIndDial -    pixel position: %i %i\n", m_PixelPosition.x,    m_PixelPosition.y);
      printf ("A320BrkTripleIndDial -     physical size: %f %f\n", m_PhysicalSize.x,     m_PhysicalSize.y);
      printf ("A320BrkTripleIndDial -        pixel size: %i %i\n", m_PixelSize.x,        m_PixelSize.y);
    }

    double partSize = m_PhysicalSize.x;	// defines total component size (just for name simplification) (always square!)
    double partCenter = m_PhysicalSize.x / 2;	// defines component center

    m_pFontManager->SetSize (m_Font, fontSize, fontSize);

    glPushMatrix ();
    glTranslated (partCenter, partCenter, 0);

    // The rim of the dials window
    glColor3ub (100, 100, 120); // lighter Gray-blue
    glBegin (GL_POLYGON);
      glVertex2f (-partSize / 2,     -partSize * 3 / 8);
      glVertex2f (-partSize / 2,      partSize * 3 / 8);
      glVertex2f (-partSize * 3 / 8,  partSize / 2);
      glVertex2f (partSize * 3 / 8,  partSize / 2);
      glVertex2f (partSize / 2,      partSize * 3 / 8);
      glVertex2f (partSize / 2,     -partSize * 3 / 8);
      glVertex2f (partSize * 3 / 8, -partSize / 2);
      glVertex2f (-partSize * 3 / 8, -partSize / 2);
    glEnd ();

    // The background of the dials window
    glColor3ub (10, 10, 20); // darker Gray-blue
    aCircle.SetOrigin (0, 0);
    aCircle.SetRadius (partSize * 0.93 / 2);
    aCircle.SetDegreesPerPoint (5);
    aCircle.SetArcStartEnd (0.0, 360.0);
    glBegin(GL_TRIANGLE_FAN);
      glVertex2f(0,0);
      aCircle.Evaluate();
      glVertex2f(0,0);
    glEnd();

    for (int i = 0; i < 3; i++) {
      glPushMatrix ();
      glRotated (i * 120.0, 0, 0, 1);
      glTranslated (0, partCenter * 0.65, 0);

      // The individual dials
      switch (i) {
        case 0: // Brake Accu dial
          glPushMatrix ();
          glColor3ub (0, 255, 0); // Green - safe area
          aCircle.SetOrigin (0, 0);
          aCircle.SetRadius (partSize * 108 / 450);
          aCircle.SetDegreesPerPoint (10);
          aCircle.SetArcStartEnd (140.0, 155.0);
          glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            aCircle.Evaluate();
            glVertex2f(0,0);
          glEnd();
          glColor3ub (255, 255, 255); // White - the tick marks
          glRotated (-48.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f (0, 0);
            glVertex2f (0, -partSize * 110 / 450);
          glEnd ();
          glRotated (96.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f (0, 0);
            glVertex2f (0, -partSize * 110 / 450);
          glEnd ();
          glPopMatrix ();
        break;
        case 1: // Left Brake dial
          glPushMatrix ();
          glColor3ub (0, 255, 0); // Green - safe area
          aCircle.SetOrigin (0, 0);
          aCircle.SetRadius (partSize * 108 / 450);
          aCircle.SetDegreesPerPoint (10);
          aCircle.SetArcStartEnd (180.0, 228.0);
          glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            aCircle.Evaluate();
            glVertex2f(0,0);
          glEnd();
          glColor3ub (255, 255, 255); // White - the tick marks
          glRotated (-60.0, 0, 0, 1);
          for (int j = 0; j < 9; j++) {
            glRotated (12.0, 0, 0, 1);
            if (j == 5 || j == 7) {
              glBegin (GL_LINES);
                glVertex2f (0, 0);
                glVertex2f (0, -partSize * 100 / 450);
              glEnd ();
            }
            if (j == 0 || j == 2 || j == 4 || j == 6 || j == 8) { 
              glBegin (GL_LINES);
                glVertex2f (0, 0);
                glVertex2f (0, -partSize * 110 / 450);
              glEnd ();
            }
          } // end of tick marks rotation
          glPopMatrix ();
        break;
        case 2: // Right Brake dial
          glPushMatrix ();
          glColor3ub (0, 255, 0); // Green - safe area
          aCircle.SetOrigin (0, 0);
          aCircle.SetRadius (partSize * 108 / 450);
          aCircle.SetDegreesPerPoint (10);
          aCircle.SetArcStartEnd (132.0, 180.0);
          glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            aCircle.Evaluate();
            glVertex2f(0,0);
          glEnd();
          glColor3ub (255, 255, 255); // White - the tick marks
          glRotated (-60.0, 0, 0, 1);
          for (int j = 0; j < 9; j++) {
            glRotated (12.0, 0, 0, 1);
            if (j == 1 || j == 3) {
              glBegin (GL_LINES);
                glVertex2f (0, 0);
                glVertex2f (0, -partSize * 100 / 450);
              glEnd ();
            }
            if (j == 0 || j == 2 || j == 4 || j == 6 || j == 8) { 
              glBegin (GL_LINES);
                glVertex2f (0, 0);
                glVertex2f (0, -partSize * 110 / 450);
              glEnd ();
            }
          } // end of tick marks rotation
          glPopMatrix ();
        break;
      } // end switch (i)

      // the dials themselves
      glColor3ub (128, 128, 128); // Gray - large segment rim
      aCircle.SetOrigin (0, 0);
      aCircle.SetRadius (partSize * 90 / 450);
      aCircle.SetDegreesPerPoint (10);
      aCircle.SetArcStartEnd (120.0, 240.0);
      glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0,0);
        aCircle.Evaluate();
        glVertex2f(0,0);
      glEnd();
      glColor3ub (0, 0, 0); // Black - large segment fill
      aCircle.SetOrigin (0, 0);
      aCircle.SetRadius (partSize * 88 / 450);
      aCircle.SetDegreesPerPoint (10);
      aCircle.SetArcStartEnd (122.0, 238.0);
      glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0,0);
        aCircle.Evaluate();
        glVertex2f(0,0);
      glEnd();
      glColor3ub (128, 128, 128); // Gray - small segment rim
      aCircle.SetOrigin (0, 0);
      aCircle.SetRadius (partSize * 40 / 450);
      aCircle.SetDegreesPerPoint (10);
      aCircle.SetArcStartEnd (-100.0, 100.0);
      glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0,0);
        aCircle.Evaluate();
        glVertex2f(0,0);
      glEnd();
      glColor3ub (0, 0, 0); // Black - small segment fill
      aCircle.SetOrigin (0, 0);
      aCircle.SetRadius (partSize * 38 / 450);
      aCircle.SetDegreesPerPoint (10);
      aCircle.SetArcStartEnd (-100.0, 100.0);
      glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0,0);
        aCircle.Evaluate();
        glVertex2f(0,0);
      glEnd();

      // the needles
      glPushMatrix ();
      switch (i) {
        case 0:
          ndlRot = *brkAccu * 80 + 132;
        break;
        case 1:
          if (*brkLeft <= 0.1) ndlRot = *brkLeft * 480 + 132;
          else ndlRot = *brkLeft * 96 + 132;
        break;
        case 2:
          if (*brkRight <= 0.1) ndlRot = -(*brkRight * 480 + 132);
          else ndlRot = -(*brkRight * 96 + 132);
        break;
      } // end 
      glRotated (ndlRot, 0, 0, 1);
      glColor3ub (255, 255, 255); // White
      glBegin (GL_POLYGON);
        glVertex2f (-5, 0);
        glVertex2f (-5, partSize * 78 / 450);
        glVertex2f ( 0, partSize * 88 / 450);
        glVertex2f  (5, partSize * 78 / 450);
        glVertex2f ( 5, 0);
      glEnd ();
      glColor3ub (192, 192, 192); // Light Grey
      glBegin (GL_POLYGON);
        glVertex2f (-3, 0);
        glVertex2f (-3, partSize * 60 / 450);
        glVertex2f ( 0, partSize * 70 / 450);
        glVertex2f ( 3, partSize * 60 / 450);
        glVertex2f ( 3, 0);
      glEnd ();
      glPopMatrix ();

      // the needle centers
      glColor3ub (100, 100, 120); // lighter Gray-blue
      aCircle.SetOrigin (0, 0);
      aCircle.SetRadius (partSize * 30 / 450);
      aCircle.SetDegreesPerPoint (10);
      aCircle.SetArcStartEnd (0.0, 360.0);
      glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0,0);
        aCircle.Evaluate();
        glVertex2f(0,0);
      glEnd();
      glColor3ub (50,50,60); // darker Gray-blue
      aCircle.SetOrigin (0, 0);
      aCircle.SetRadius (partSize * 20 / 450);
      aCircle.SetDegreesPerPoint (10);
      aCircle.SetArcStartEnd (0.0, 360.0);
      glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0,0);
        aCircle.Evaluate();
        glVertex2f(0,0);
      glEnd();

      glPopMatrix ();
    } // end of dials rotation

    glPopMatrix (); // end of translation

    // The texts
    int d_h  = (int) fontSize;          // height of a single line
    int d_f  = (int) (fontSize * 0.25); // space between text lines
    int d_w  = (int) fontSize;          // width of a single character (non-proportional font!)

    glColor3ub (255, 255, 255); // White
    m_pFontManager->Print(partCenter - 6   * d_w, partCenter + 9   * (d_h - d_f), "ACCU       PRESS", m_Font);
    m_pFontManager->Print(partCenter - 5.3 * d_w, partCenter + 4   * (d_h - d_f),   "0           4",  m_Font);
    m_pFontManager->Print(partCenter - 6.5 * d_w, partCenter + 2   * (d_h - d_f), "3              3", m_Font);
    m_pFontManager->Print(partCenter - 1.6 * d_w, partCenter - 2   * (d_h - d_f),      "1  1",        m_Font);
    m_pFontManager->Print(partCenter - d_w / 2,   partCenter - 5   * (d_h + d_f),        "0",         m_Font);
    m_pFontManager->Print(partCenter - 2.5 * d_w, partCenter - 6.3 * (d_h + d_f),      "BRAKES",      m_Font);
    glColor3ub (0, 128, 255); // Light Blue
    m_pFontManager->Print(partCenter - 4 * d_w,   partCenter - 7.6 * (d_h + d_f),    "PSI x 1000",    m_Font);

  } // end Render()

} // end namespace OpenGC

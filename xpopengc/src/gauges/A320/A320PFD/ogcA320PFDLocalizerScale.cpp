/*=============================================================================

  This is the ogcA320PFDLocalizerScale.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Primary Flight Display: the Localizer Scale ===

  Created:
    Date:        2011-11-14
    Author:      Hans Jansen
    last change: 2020-02-04
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

=============================================================================

  The following datarefs are used by this instrument:
v	AirbusFBW/ILSonCapt	int
v	AirbusFBW/LOConCapt	int
v	AirbusFBW/LOCValCapt	float
v	AirbusFBW/NPAValid	int

=============================================================================*/

#include <stdio.h>

#include "ogcGaugeComponent.h"
#include "ogcA320PFD.h"
#include "ogcA320PFDLocalizerScale.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320PFDLocalizerScale::A320PFDLocalizerScale () {
    if (verbosity > 0) printf ("A320PFDLocalizerScale - constructing\n");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("A320PFDLocalizerScale - constructed\n");
  }

  A320PFDLocalizerScale::~A320PFDLocalizerScale () {}

  void A320PFDLocalizerScale::Render () {

    bool ColdAndDark = true;
    CircleEvaluator aCircle;

    GaugeComponent::Render ();

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) ColdAndDark = true; else ColdAndDark = (*cold_and_dark != 0) ? true : false;

    // LS Scales visible?
    int* lsValid = link_dataref_int ("AirbusFBW/ILSonCapt");

    // NPA valid?
    int* npaValid = link_dataref_int ("AirbusFBW/NPAValid");

    // LOC valid?
    int* locValid = link_dataref_int ("AirbusFBW/LOConCapt");

    // LOC value
    int locValue;
    float* loc_value = link_dataref_flt ("AirbusFBW/LOCvalCapt", 0);
    if (*loc_value != FLT_MISS) locValue = (int) (*loc_value * 40); else locValue = 0;

    if (verbosity > 2) {
      printf ("A320PFDLocalizerScale - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320PFDLocalizerScale -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320PFDLocalizerScale -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320PFDLocalizerScale -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    if (!ColdAndDark) {

      double partWidth = m_PhysicalSize.x;
      double partHeight = m_PhysicalSize.y;
      double refPosH = partWidth / 2;
      double refPosV = partHeight / 2;

      if (*lsValid == 1) {
        // draw a background

        // Draw the reference line
        glColor3ub (COLOR_YELLOW);
        glLineWidth (3.0);
        glBegin (GL_LINE_STRIP);
          glVertex2f (refPosH, partHeight - 5);
          glVertex2f (refPosH, 5);
        glEnd ();

        // Tick marks are spaced every 10 degrees horizontally
        // The tick spacing represents how far apart they are in physical units
        double tickSpacing = m_PhysicalSize.x / 6;
        double tickHeight = 16;

        if (*npaValid == 1) {
          // draw the LDEV background marks
          glColor3ub (COLOR_WHITE);

          glBegin (GL_LINE_STRIP);
            glVertex2f (refPosH+tickSpacing, partHeight - 10);
            glVertex2f (refPosH+tickSpacing, 10);
          glEnd ();
          glBegin (GL_LINE_STRIP);
            glVertex2f (refPosH+tickSpacing * 2, partHeight - 10);
            glVertex2f (refPosH+tickSpacing * 2, 10);
          glEnd ();
          glBegin (GL_LINE_STRIP);
            glVertex2f (refPosH-tickSpacing, partHeight - 10);
            glVertex2f (refPosH-tickSpacing, 10);
          glEnd ();
          glBegin (GL_LINE_STRIP);
            glVertex2f (refPosH-tickSpacing * 2, partHeight - 10);
            glVertex2f (refPosH-tickSpacing * 2, 10);
          glEnd ();
        } else {
          // draw the LOC background marks
          glColor3ub (COLOR_WHITE);

          aCircle.SetOrigin (refPosH + tickSpacing, refPosV);
          aCircle.SetRadius (3);
          aCircle.SetDegreesPerPoint (15);
          aCircle.SetArcStartEnd (0.0, 360.0);
          glBegin (GL_TRIANGLE_FAN);
            glVertex2f (refPosH + tickSpacing, refPosV);
            aCircle.Evaluate ();
            glVertex2f (refPosH + tickSpacing, refPosV);
          glEnd ();

          aCircle.SetOrigin (refPosH + tickSpacing * 2, refPosV);
          aCircle.SetRadius (3);
          aCircle.SetDegreesPerPoint (15);
          aCircle.SetArcStartEnd (0.0, 360.0);
          glBegin (GL_TRIANGLE_FAN);
            glVertex2f (refPosH + tickSpacing * 2, refPosV);
            aCircle.Evaluate ();
            glVertex2f (refPosH + tickSpacing * 2, refPosV);
          glEnd ();

          aCircle.SetOrigin (refPosH - tickSpacing, refPosV);
          aCircle.SetRadius (3);
          aCircle.SetDegreesPerPoint (15);
          aCircle.SetArcStartEnd (0.0, 360.0);
          glBegin (GL_TRIANGLE_FAN);
            glVertex2f (refPosH - tickSpacing, refPosV);
            aCircle.Evaluate ();
            glVertex2f (refPosH - tickSpacing, refPosV);
          glEnd ();

          aCircle.SetOrigin (refPosH - tickSpacing * 2, refPosV);
          aCircle.SetRadius (3);
          aCircle.SetDegreesPerPoint (15);
          aCircle.SetArcStartEnd (0.0, 360.0);
          glBegin (GL_TRIANGLE_FAN);
            glVertex2f (refPosH - tickSpacing * 2, refPosV);
            aCircle.Evaluate ();
            glVertex2f (refPosH - tickSpacing * 2, refPosV);
          glEnd ();
        } // (else) if (*npaValid == 1)
      } // if (*lsValid == 1)

      if (*locValid == 1) {
        // draw the LOC indicator
        if (*npaValid == 1) {
          // draw the NPA bug
          glColor3ub (COLOR_MAGENTA);
          glLineWidth (2.0);
          glBegin (GL_LINE_LOOP);
            glVertex2f (locValue + refPosH - 2, refPosV-12);
            glVertex2f (locValue + refPosH - 4, refPosV-10);
            glVertex2f (locValue + refPosH - 4, refPosV+10);
            glVertex2f (locValue + refPosH - 2, refPosV+12);
            glVertex2f (locValue + refPosH + 2, refPosV+12);
            glVertex2f (locValue + refPosH + 4, refPosV+10);
            glVertex2f (locValue + refPosH + 4, refPosV-10);
            glVertex2f (locValue + refPosH + 2, refPosV-12);
          glEnd ();
        } else {
          // draw the ILS bug
          glColor3ub (COLOR_MAGENTA);
          glLineWidth (2.0);
          glBegin (GL_LINE_LOOP);
            glVertex2f (locValue + refPosH, refPosV - 12);
            glVertex2f (locValue + refPosH - 6, refPosV);
            glVertex2f (locValue + refPosH, refPosV + 12);
            glVertex2f (locValue + refPosH + 6, refPosV);
          glEnd ();
        }

      } // if (*locValid == 1)

    } // end if (!ColdAndDark)

  } // end Render()

} // end namespace OpenGC

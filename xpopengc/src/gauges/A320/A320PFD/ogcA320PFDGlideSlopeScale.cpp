/*=============================================================================

  This is the ogcA320PFDGlideSlopeScale.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Primary Flight Display: the Glide-Slope Scale ===

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
v	AirbusFBW/GSonCapt	int
v	AirbusFBW/GSvalCapt	float
v	AirbusFBW/ILSonCapt	int
v	AirbusFBW/NPAValid	int

=============================================================================*/

#include <stdio.h>

#include "ogcGaugeComponent.h"
#include "ogcA320PFD.h"
#include "ogcA320PFDGlideSlopeScale.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320PFDGlideSlopeScale::A320PFDGlideSlopeScale () {
    if (verbosity > 0) printf ("A320PFDGlideSlopeScale - constructing\n");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("A320PFDGlideSlopeScale - constructed\n");
  }

  A320PFDGlideSlopeScale::~A320PFDGlideSlopeScale () {}

  void A320PFDGlideSlopeScale::Render () {

    bool ColdAndDark = true;
    CircleEvaluator aCircle;

    GaugeComponent::Render ();

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) ColdAndDark = true; else ColdAndDark = (*cold_and_dark != 0) ? true : false;

    // LS Scales visible?
    int* lsValid = link_dataref_int("AirbusFBW/ILSonCapt");

    // NPA valid?
    int* npaValid = link_dataref_int("AirbusFBW/NPAValid");

    // G/S valid?
    int* gsValid = link_dataref_int("AirbusFBW/GSonCapt");

    // G/S value
    float gsValue;
    float* gs_value = link_dataref_flt("AirbusFBW/GSvalCapt", 0);
    if (*gs_value != FLT_MISS) gsValue = (*gs_value * -40); else gsValue = 0;

    if (verbosity > 2)
    {
      printf ("A320PFDGlideSlopeScale - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320PFDGlideSlopeScale -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320PFDGlideSlopeScale -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320PFDGlideSlopeScale -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    double partWidth = m_PhysicalSize.x;
    double partHeight = m_PhysicalSize.y;
    double refPosH = partWidth / 2;
    double refPosV = partHeight / 2;

    if (!ColdAndDark) {

    if (*lsValid == 1) {
      // draw a background

      // Draw the reference line
      glColor3ub (COLOR_YELLOW);
      glLineWidth (3.0);
      glBegin (GL_LINE_STRIP);
        glVertex2f (partWidth - 5, refPosV);
        glVertex2f (5, refPosV);
      glEnd ();

     // Tick marks are spaced every 200 feet vertically
      // The tick spacing represents how far apart they are in physical units
      double tickSpacing = m_PhysicalSize.y / 6;
      double tickWidth = 16;

      if (*npaValid == 1) {
        // draw the VDEV background marks
        glColor3ub (COLOR_WHITE);

        glBegin (GL_LINE_STRIP);
          glVertex2f (partWidth - 10, refPosV + tickSpacing);
          glVertex2f (10, refPosV + tickSpacing);
        glEnd ();
        glBegin (GL_LINE_STRIP);
          glVertex2f (partWidth - 10, refPosV + tickSpacing * 2);
          glVertex2f (10, refPosV + tickSpacing * 2);
        glEnd ();
        glBegin (GL_LINE_STRIP);
          glVertex2f (partWidth - 10, refPosV - tickSpacing);
          glVertex2f (10, refPosV - tickSpacing);
        glEnd ();
        glBegin (GL_LINE_STRIP);
          glVertex2f (partWidth - 10, refPosV - tickSpacing * 2);
          glVertex2f (10, refPosV - tickSpacing * 2);
        glEnd ();
      } else {
        // draw the G/S background marks
        glColor3ub (COLOR_WHITE);

        aCircle.SetOrigin (refPosH, refPosV + tickSpacing);
        aCircle.SetRadius (3);
        aCircle.SetDegreesPerPoint (15);
        aCircle.SetArcStartEnd (0.0, 360.0);
        glBegin (GL_TRIANGLE_FAN);
          glVertex2f (refPosH + 3, refPosV + tickSpacing);
          glVertex2f (refPosH, refPosV + tickSpacing + 3);
          aCircle.Evaluate ();
          glVertex2f (refPosH, refPosV + tickSpacing);
        glEnd ();
        aCircle.SetOrigin (refPosH, refPosV + tickSpacing * 2);
        aCircle.SetRadius (3);
        aCircle.SetDegreesPerPoint (15);
        aCircle.SetArcStartEnd (0.0, 360.0);
        glBegin (GL_TRIANGLE_FAN);
          glVertex2f (refPosH + 3, refPosV + tickSpacing * 2);
          glVertex2f (refPosH, refPosV + tickSpacing * 2 + 3);
          aCircle.Evaluate ();
          glVertex2f (refPosH, refPosV + tickSpacing * 2);
        glEnd ();
        aCircle.SetOrigin (refPosH, refPosV - tickSpacing);
        aCircle.SetRadius (3);
        aCircle.SetDegreesPerPoint (15);
        aCircle.SetArcStartEnd (0.0, 360.0);
        glBegin (GL_TRIANGLE_FAN);
          glVertex2f (refPosH + 3, refPosV - tickSpacing);
          glVertex2f (refPosH, refPosV - tickSpacing + 3);
          aCircle.Evaluate ();
          glVertex2f (refPosH, refPosV - tickSpacing);
        glEnd ();
        aCircle.SetOrigin (refPosH, refPosV - tickSpacing * 2);
        aCircle.SetRadius (3);
        aCircle.SetDegreesPerPoint (15);
        aCircle.SetArcStartEnd (0.0, 360.0);
        glBegin (GL_TRIANGLE_FAN);
          glVertex2f (refPosH + 3, refPosV - tickSpacing * 2);
          glVertex2f (refPosH, refPosV - tickSpacing * 2 + 3);
          aCircle.Evaluate ();
          glVertex2f (refPosH, refPosV - tickSpacing * 2);
        glEnd();
      } // if (*npaValid == 1)
    } // if (*lsValid == 1)
    if (*gsValid == 1) {
      // draw the G/S indicator
      if (*npaValid == 1) {
        // draw the NPA bug
        glColor3ub (COLOR_MAGENTA);
        glLineWidth (2.0);
        glBegin (GL_LINE_LOOP);
          glVertex2f (refPosH - 12, gsValue+refPosV - 2);
          glVertex2f (refPosH - 10, gsValue+refPosV - 4);
          glVertex2f (refPosH + 10, gsValue+refPosV - 4);
          glVertex2f (refPosH + 12, gsValue+refPosV - 2);
          glVertex2f (refPosH + 12, gsValue+refPosV + 2);
          glVertex2f (refPosH + 10, gsValue+refPosV + 4);
          glVertex2f (refPosH - 10, gsValue+refPosV + 4);
          glVertex2f (refPosH - 12, gsValue+refPosV + 2);
        glEnd ();
      } else {
        if (gsValue >= -200) {
          // draw the ILS bug
          glColor3ub (COLOR_MAGENTA);
          glLineWidth (2.0);
          glBegin (GL_LINE_LOOP);
            glVertex2f (refPosH - 12, gsValue + refPosV);
            glVertex2f (refPosH, gsValue + refPosV - 6);
            glVertex2f (refPosH + 12, gsValue + refPosV);
            glVertex2f (refPosH, gsValue + refPosV + 6);
          glEnd ();
        }
      } // if (*npaValid == 1)
    } // if (*gsValid == 1)

    } // end if ((!ColdAndDark())

  } // end Render()

} // end namespace OpenGC

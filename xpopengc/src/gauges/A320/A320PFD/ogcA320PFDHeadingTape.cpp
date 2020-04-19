/*=============================================================================

  This is the ogcA320PFDHeadingTape.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Primary Flight Display: the Heading Tape ===

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
v	sim/cockpit/autopilot/heading_mag	float
v	AirbusFBW/APHDG_Capt			float
v	AirbusFBW/CaptHDGValid			int
v	AirbusFBW/Drift_Capt			float
v	AirbusFBW/HDG_Capt			float
v	AirbusFBW/HDGdashed			int
v	AirbusFBW/ILSCourseDev			float
v	AirbusFBW/ILSCrs			float

=============================================================================*/

#include <stdio.h>
#include <math.h>

#include "ogcGaugeComponent.h"
#include "ogcA320PFD.h"
#include "ogcA320PFDHeadingTape.h"

namespace OpenGC {

  A320PFDHeadingTape::A320PFDHeadingTape () {
    if (verbosity > 0) printf ("A320PFDHeadingTape - constructing\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("A320PFDHeadingTape - constructed\n");
  }

  A320PFDHeadingTape::~A320PFDHeadingTape () {}

  float hdgCapt, hdgDev, fcuHdg, apHdg, ilsCrs, ilsDev;

  void A320PFDHeadingTape::Render () {

    bool ColdAndDark = true;

    GaugeComponent::Render ();

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) ColdAndDark = true; else ColdAndDark = (*cold_and_dark != 0) ? true : false;

    // Heading indicators valid?
    int* hdgValid = link_dataref_int ("AirbusFBW/CaptHDGValid");

    // FCU-heading dashed?
    int* hdgDashed = link_dataref_int ("AirbusFBW/HDGdashed");

    // current heading
    float *hdg_capt = link_dataref_flt ("AirbusFBW/HDGCapt", -1);
    if (*hdg_capt != FLT_MISS) hdgCapt = *hdg_capt; else hdgCapt = 0;

    // heading deviation
    float *hdg_dev = link_dataref_flt ("AirbusFBW/Drift_Capt", 0);
    if (*hdg_dev != FLT_MISS) hdgDev = *hdg_dev; else hdgDev = 0;

    // autopilot-commanded heading
    float *ap_heading = link_dataref_flt ("sim/cockpit/autopilot/heading_mag", -1);
    if (*ap_heading != FLT_MISS) fcuHdg = *ap_heading; else fcuHdg = 0;

    // autopilot-commanded heading deviation
    float *ap_hdg = link_dataref_flt ("AirbusFBW/APHDG_Capt", -1);
    if (*ap_hdg != FLT_MISS) apHdg = *ap_hdg; else apHdg = 0;

    // ILS course
    float *ils_crs = link_dataref_flt ("AirbusFBW/ILSCrs", -1);
    if (*ils_crs != FLT_MISS) ilsCrs = *ils_crs; else ilsCrs = 0;

    // ILS course deviation
    float *ils_dev = link_dataref_flt ("AirbusFBW/ILSCourseDev", -1);
    if (*ils_dev != FLT_MISS) ilsDev = *ils_dev; else ilsDev = 0;

    if (verbosity > 2) {
      printf ("A320PFDHeadingTape - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320PFDHeadingTape -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320PFDHeadingTape -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320PFDHeadingTape -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    if (!ColdAndDark) {

      double partWidth = m_PhysicalSize.x;
      double partHeight = m_PhysicalSize.y - 20;
      double refPos = m_PhysicalSize.x / 2;

      double fontWidth = 12;
      double fontHeight = 12;

      glPushMatrix ();

      // Draw the background rectangle
      glColor3ub (COLOR_AIRBUSGRAY);
      glBegin (GL_POLYGON);
        glVertex2f (0, 0);
        glVertex2f (0, partHeight);
        glVertex2f (partWidth, partHeight);
        glVertex2f (partWidth, 0);
      glEnd ();
      // Draw the boundary lines
      glColor3ub (COLOR_GRAY75);
      glLineWidth (2.0);
      glBegin (GL_LINE_STRIP);
        glVertex2f (-partWidth, -partHeight);
        glVertex2f (-partWidth, partHeight);
        glVertex2f (partWidth, partHeight);
        glVertex2f (partWidth, -partHeight);
      glEnd ();

      if (*hdgValid != 1) {
        m_pFontManager->SetSize (m_Font, 1.5 * fontWidth, 1.5 * fontHeight);
        glColor3ub (COLOR_RED);
        m_pFontManager->Print (refPos - 1.5 * fontWidth, 1.5 * fontHeight, "HDG", m_Font);
      } else {
        // Draw the reference line (outside the clipping region!)
        glColor3ub (COLOR_YELLOW);
        glLineWidth (3.0);
        glBegin (GL_LINE_STRIP);
          glVertex2f (refPos, partHeight-30);
          glVertex2f (refPos, partHeight + 20);
        glEnd ();

        // Tick marks are spaced every 5 and 10 degrees horizontally
        // The tick spacing represents how far apart they are in physical units
        double tickSpacing = m_PhysicalSize.x / 12;
        double tickHeight = 16;

        m_pFontManager->SetSize (m_Font, fontHeight, fontWidth);

        char buffer[16];
        int nextHigherHdg = (int) (hdgCapt / 5) * 5;

        // The horizontal offset is how far in physical units the next higher 5's
        // heading is to the right of the reference line. For headings divisible by 5,
        //  this is 0. I.e. 120, 125, 130 etc. are all aligned with the reference line
        float horOffset = nextHigherHdg - hdgCapt;
        // Horizontal location of the tick mark
        double tickLocation = 0;

        glColor3ub (COLOR_WHITE);
        glLineWidth (2.0);

        double i = 0; // counter
        int tickHeading; // Heading represented by tick mark
        int charHdg; // Heading for computing text digits

        // Draw ticks right from the center
        for (i = 0; i <= ((m_PhysicalSize.x / 2) / tickSpacing); i++) {
          tickHeading = nextHigherHdg + (int) (i * 5);
          tickLocation = refPos + (i + 0.2 * horOffset) * tickSpacing;
          if ((tickHeading % 10) == 0) {
            if ((tickHeading % 30) == 0 || (tickHeading == 0)) {
              glBegin (GL_LINES);
                glVertex2f (tickLocation, partHeight);
                glVertex2f (tickLocation, partHeight - tickHeight * 1.5);
              glEnd ();
            } else {
              glBegin (GL_LINES);
                glVertex2f (tickLocation, partHeight);
                glVertex2f (tickLocation, partHeight - tickHeight);
              glEnd ();
            }

            double textLocation = tickLocation - fontWidth + 3;
            if (tickHeading >= 360) {
              sprintf ( buffer, "%02i", (int) (tickHeading - 360) / 10);
            } else {
              sprintf ( buffer, "%02i", (int) (tickHeading) / 10);
            }
            m_pFontManager->Print (textLocation, 5, buffer, m_Font);
          } else {
            glBegin (GL_LINES);
              glVertex2f (tickLocation, partHeight);
              glVertex2f (tickLocation, partHeight - tickHeight / 2);
            glEnd ();
          }
        }

        // Draw ticks left from the center
        for (i = 1; i <= ((m_PhysicalSize.x/2) / tickSpacing); i++) {
          tickHeading = nextHigherHdg - (int) (i * 5);
          tickLocation = refPos + (-i + 0.2 * horOffset) * tickSpacing;

          if ((tickHeading % 10) == 0) {
            if ((tickHeading % 30) == 0 || (tickHeading == 0)) {
              glBegin (GL_LINES);
                glVertex2f (tickLocation, partHeight);
                glVertex2f (tickLocation, partHeight - tickHeight * 1.5);
              glEnd ();
            } else {
              glBegin (GL_LINES);
                glVertex2f (tickLocation, partHeight);
                glVertex2f (tickLocation, partHeight - tickHeight);
              glEnd ();
            }

            double textLocation = tickLocation - fontWidth;
            if (tickHeading < 0) {
              sprintf ( buffer, "%02i", (int) (tickHeading + 360) / 10);
            } else {
              sprintf ( buffer, "%02i", (int) (tickHeading) / 10);
            }
            m_pFontManager->Print (textLocation, 5, buffer, m_Font);
          } else {
            glBegin (GL_LINES);
              glVertex2f (tickLocation, partHeight);
              glVertex2f (tickLocation, partHeight - tickHeight / 2);
            glEnd ();
          }
        }

        // The FCU Heading target
        if (*hdgDashed == 0) {
          if ( fabs (apHdg) <= 25 ) {
            // draw FCU Heading target if within the Heading tape range
            double fcuHdgLocation = refPos + double (apHdg) * tickSpacing / 5;
            glLineWidth (3.0);
            glColor3ub (COLOR_CYAN);
            glBegin (GL_LINE_LOOP);
              glVertex2f (fcuHdgLocation, partHeight + 2);
              glVertex2f (fcuHdgLocation + 10, partHeight + 20);
              glVertex2f (fcuHdgLocation - 10, partHeight + 20);
            glEnd ();
          } else {
            // draw numeric FCU Heading target if outside tape range
            glColor3ub (COLOR_CYAN);
            sprintf ( buffer, "%03i", (int) fcuHdg);
            if (apHdg < -25) {
              m_pFontManager->Print (20, partHeight + 5, buffer, m_Font);
            } else  {
              if (apHdg > 25) {
                m_pFontManager->Print (partWidth - 50, partHeight + 5, buffer, m_Font);
              }
            }
          }
        } // if (*hdgDashed == 0)

        // Mask off the left and right ends of the tape
        glColor3ub (COLOR_BLACK);
        glBegin (GL_POLYGON);
          glVertex2f (0, 0);
          glVertex2f (0, partHeight + 1);
          glVertex2f (20, partHeight + 1);
          glVertex2f (20, 0);
        glEnd ();
        glBegin (GL_POLYGON);
          glVertex2f (partWidth - 20, 0);
          glVertex2f (partWidth - 20, partHeight + 1);
          glVertex2f (partWidth - 0, partHeight + 1);
          glVertex2f (partWidth - 0, 0);
        glEnd ();

        // The ILS Heading target
        if (ilsDev >= -900) {
          if (abs (ilsDev) <= 25) {
            // draw ILS Heading target if within the Heading tape range
            double ilsDevLocation = refPos + double (ilsDev) * tickSpacing / 5;
            glLineWidth (3.0);
            glColor3ub (COLOR_MAGENTA);
            glBegin (GL_LINE_STRIP);
              glVertex2f (ilsDevLocation, 2);
              glVertex2f (ilsDevLocation, 30);
              glVertex2f (ilsDevLocation, 10);
              glVertex2f (ilsDevLocation + 6, 10);
              glVertex2f (ilsDevLocation - 6, 10);
            glEnd ();
          } else {
            // draw numeric FCU Heading target if outside tape range
            if (ilsDev < -25) {
              glLineWidth (2.0);
              glColor3ub (COLOR_BLACK);
              glBegin (GL_POLYGON);
                glVertex2f (0, 21);
                glVertex2f (0, 41);
                glVertex2f (3 * fontWidth+ 2, 41);
                glVertex2f (3 * fontWidth+ 2, 21);
              glEnd ();
              glColor3ub (COLOR_WHITE);
              glBegin (GL_LINE_LOOP);
                glVertex2f (0, 21);
                glVertex2f (0, 41);
                glVertex2f (3 * fontWidth+ 2, 41);
                glVertex2f (3 * fontWidth+ 2, 21);
              glEnd ();
              glColor3ub (COLOR_MAGENTA);
              sprintf ( buffer, "%03i", (int) ilsCrs);
              m_pFontManager->Print (4, 25, buffer, m_Font);
            } else {
              if (ilsDev > 25)  {
                glLineWidth (2.0);
                glColor3ub (COLOR_BLACK);
                glBegin (GL_POLYGON);
                  glVertex2f (partWidth - 40, 21);
                  glVertex2f (partWidth - 40, 41);
                  glVertex2f (partWidth - 40 + 3 * fontWidth+ 2, 41);
                  glVertex2f (partWidth - 40 + 3 * fontWidth+ 2, 21);
                glEnd ();
                glColor3ub (COLOR_WHITE);
                glBegin (GL_LINE_LOOP);
                  glVertex2f (partWidth - 40, 21);
                  glVertex2f (partWidth - 40, 41);
                  glVertex2f (partWidth - 40 + 3 * fontWidth+ 2, 41);
                  glVertex2f (partWidth - 40 + 3 * fontWidth+ 2, 21);
                glEnd ();
                glColor3ub (COLOR_MAGENTA);
                sprintf ( buffer, "%03i", (int) ilsCrs);
                m_pFontManager->Print (partWidth - 36, 25, buffer, m_Font);
              } // if (ilsDev > 25)
            } // if (ilsDev < -25)
          } // if (abs (ilsDev) <= 25)
        } // if (ilsDev >= -900)

        // The  TRK marker
        if (abs (hdgDev) <= 25) {
          // draw ILS Heading target if within the Heading tape range
          double hdgDevLocation = refPos + double (hdgDev) * tickSpacing / 5;
          glLineWidth (3.0);
          glColor3ub (COLOR_GREEN);
          glBegin (GL_LINE_LOOP);
            glVertex2f (hdgDevLocation, partHeight - 2);
            glVertex2f (hdgDevLocation + 6, partHeight - 12);
            glVertex2f (hdgDevLocation, partHeight - 22);
            glVertex2f (hdgDevLocation - 6, partHeight - 12);
          glEnd ();
        } // if (abs (hdgDev) <= 25)

      } // (else) if (*hdgValid != 1)
      glPopMatrix ();

    } // end if (!ColdAndDark)

  } // end Render()

} // end namespace OpenGC

/*=============================================================================

  This is the ogcA320PFDVSI.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Primary Flight Display: the Vertical Speed Indicator ===

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

=============================================================================

  The OpenGC subproject has been derived from:
    OpenGC - The Open Source Glass Cockpit Project
    Copyright (c) 2001-2003 Damion Shelton

=============================================================================

  The following datarefs are used by this instrument:
v	AirbusFBW/CaptALTValid				int
v	qpac_airbus/pfdoutputs/captain/vertical_speed	float

=============================================================================*/

#include <stdio.h>
#include <math.h>

#include "ogcGaugeComponent.h"
#include "ogcA320PFD.h"
#include "ogcA320PFDVSI.h"

namespace OpenGC {

  A320PFDVSI::A320PFDVSI () {
    if (verbosity > 0) printf ("A320PFDVSI - constructing\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("A320PFDVSI - constructed\n");
  }

  A320PFDVSI::~A320PFDVSI () {}

  double A320PFDVSI::VSpeedToNeedle (double vspd) {

    double NeedleCenter = 0.5 * m_PhysicalSize.y;
    double MaxNeedleDeflection = 0.5 * m_PhysicalSize.y;
    double needleTip;
  
    if (vspd >= 0) {
      if (vspd>6000) vspd = 6000;
      needleTip = NeedleCenter - 3.0 + (1-exp(-3 * vspd / 6000)) * MaxNeedleDeflection;
    } else {
      vspd = fabs(vspd);
      if (vspd>6000) vspd = 6000;
      needleTip = NeedleCenter - 3.0  - (1-exp(-3 * vspd / 6000)) * MaxNeedleDeflection;
    }

    return needleTip;
  }

  float vertSpd;

  void A320PFDVSI::Render () {

    bool ColdAndDark = true;

    GaugeComponent::Render();

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) ColdAndDark = true; else ColdAndDark = (*cold_and_dark != 0) ? true : false;

    // Altitude indicators valid?
    int* altValid = link_dataref_int ("AirbusFBW/CaptALTValid");

    float *vert_spd = link_dataref_flt ("qpac_airbus/pfdoutputs/captain/vertical_speed",-1);
    if (*vert_spd != FLT_MISS) vertSpd = (int) *vert_spd; else vertSpd = 0;

    if (verbosity > 2)
    {
      printf ("A320PFDVSI - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320PFDVSI -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320PFDVSI -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320PFDVSI -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    if (!ColdAndDark) {

      double partWidth = m_PhysicalSize.x;	// defines total component size (just for name simplification)
      double partHeight = m_PhysicalSize.y;

      double fontHeight = 8;
      double fontWidth = 8;
      double refPos = partHeight / 2.0; // vertical position of the center reference line

      // Background of the scale
      glColor3ub (COLOR_AIRBUSGRAY);
      glBegin (GL_POLYGON);
        glVertex2f (0, 0);
        glVertex2f (0, partHeight);
        glVertex2f (0.2 * partWidth, partHeight);
        glVertex2f (2 * partWidth, 0.5 * partHeight);
        glVertex2f (0.2 * partWidth, 0);
      glEnd ();

      if (*altValid != 1) {
        m_pFontManager->SetSize(m_Font, 1.5 * 12, 1.5 * 10);
        glColor3ub (COLOR_RED);
        m_pFontManager->Print (5, refPos + 1.5 * 12, "V", m_Font);
        m_pFontManager->Print (5, refPos, "/", m_Font);
        m_pFontManager->Print (5, refPos - 1.5 * 12, "S", m_Font);
      } else {
        m_pFontManager->SetSize(m_Font, 9.0, 9.0);

        // Display the fixed background lines
        glColor3ub (COLOR_YELLOW);
        glLineWidth (2.0);
        glBegin (GL_LINES);
          glVertex2f (0, refPos);
          glVertex2f (0.3*partWidth, refPos);
        glEnd ();
        glColor3ub (COLOR_WHITE);
        m_pFontManager->Print ( 0, VSpeedToNeedle(5000.0),"-",m_Font);
        m_pFontManager->Print ( 0, VSpeedToNeedle(2000.0),"-2",m_Font);
        m_pFontManager->Print ( 0, VSpeedToNeedle(1000.0),"-1",m_Font);
        m_pFontManager->Print ( 0, VSpeedToNeedle(-1000.0),"-1",m_Font);
        m_pFontManager->Print ( 0, VSpeedToNeedle(-2000.0),"-2",m_Font);
        m_pFontManager->Print ( 0, VSpeedToNeedle(-5000.0),"-",m_Font);

         // if vertical > +- 200 fpm display digital readout
        char buffer[16];
        glColor3ub (COLOR_GREEN);
        if ( vertSpd < -199 ) {
          glColor3ub (COLOR_BLACK);
          glRecti (0, 8, partWidth, 20);
          sprintf (buffer, "%2i", (int) abs (vertSpd / 100));
          glColor3ub (COLOR_GREEN);
          m_pFontManager->Print ( 0, 10 , &buffer[0], m_Font);
        } else {
          if ( vertSpd > 199 ) {
            glColor3ub (COLOR_BLACK);
            glRecti (0, partHeight-20, partWidth, partHeight-8);
            sprintf (buffer, "%2i", (int) (vertSpd / 100));
            glColor3ub (COLOR_GREEN);
            m_pFontManager->Print ( 0, partHeight-18 , &buffer[0], m_Font);
          }
        }

        glLineWidth (2.0);

        // Draw the angled line that indicates climb rate
        glColor3ub (COLOR_GREEN);
        glBegin (GL_LINES);
          glVertex2f ( 0, VSpeedToNeedle(vertSpd) + 3.0);
          glVertex2f ( 2 * partWidth, partHeight*0.5 - 2.0);
        glEnd ();

        // Mask off the invisibe part of the scale and pointer
        glColor3ub (COLOR_BLACK);
        glBegin (GL_POLYGON);
          glVertex2f (0.7 * partWidth, 0);
          glVertex2f (0.7 * partWidth, partHeight);
          glVertex2f (partWidth, partHeight);
          glVertex2f (partWidth, 0);
        glEnd ();

      } // end if (*altValid != 1)

    } // end if (!ColdAndDark)

  } // end Render()

} // end namespace OpenGC

/*=============================================================================

  This is the ogcA320PFDBackground.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Primary Flight Display: Background ===

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
	-none-

=============================================================================*/

#include "ogcGaugeComponent.h"
#include "ogcA320PFD.h"
#include "ogcA320PFDBackground.h"

namespace OpenGC {

  A320PFDBackground::A320PFDBackground () {
    if (verbosity > 0) printf ("A320PFDBackground - constructing\n");

    m_Font = m_pFontManager->LoadFont((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("A320PFDBackground - constructed\n");
  }

  A320PFDBackground::~A320PFDBackground () {}

  void A320PFDBackground::Render () {

    bool coldDarkPfd = true;

    GaugeComponent::Render ();

    if (verbosity > 2)
    {
      printf ("A320PFDBackground - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320PFDBackground -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320PFDBackground -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320PFDBackground -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    if (!coldDarkPfd) {
      glPushMatrix ();

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_dark_pfd = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_dark_pfd == INT_MISS) coldDarkPfd = true; else coldDarkPfd = (*cold_dark_pfd != 0) ? true : false;

      // Translate to the center of the ATT component
      glTranslated (200,235,0);

        //----------------The bank angle markings----------------
        // (these are otherwise partially obscured by the ATT mask)

        // Left side bank markings
        glPushMatrix ();

          // Draw in yellow
          glColor3ub(COLOR_YELLOW);
          glLineWidth (2.0);

          // Draw the center detent
          glBegin (GL_LINE_LOOP);
            glVertex2f (00,125);
            glVertex2f (-8,137);
            glVertex2f (8,137);
          glEnd ();

          // Left side bank markings

          // Draw in white
          glColor3ub(COLOR_WHITE);

          glRotated (10.0,0,0,1);
          glBegin (GL_LINE_STRIP);
            glVertex2f (-2,125);
            glVertex2f (-2,135);
            glVertex2f (2,135);
            glVertex2f (2,125);
          glEnd ();

          glRotated (10.0,0,0,1);
          glBegin (GL_LINE_STRIP);
            glVertex2f (-2,125);
            glVertex2f (-2,135);
            glVertex2f (2,135);
            glVertex2f (2,125);
          glEnd ();

          glRotated (10.0,0,0,1);
          glBegin (GL_LINE_STRIP);
            glVertex2f (-2,125);
            glVertex2f (-2,145);
            glVertex2f (2,145);
            glVertex2f (2,125);
          glEnd ();

          glRotated (15.0,0,0,1);
          glBegin (GL_LINES);
            glVertex2f (0,125);
            glVertex2f (0,135);
          glEnd ();

        glPopMatrix ();

        // Right side bank markings
        glColor3ub(COLOR_WHITE);

        glPushMatrix ();

          glRotated (-10.0,0,0,1);
          glBegin (GL_LINE_STRIP);
            glVertex2f (-2,125);
            glVertex2f (-2,135);
            glVertex2f (2,135);
            glVertex2f (2,125);
          glEnd ();
  
          glRotated (-10.0,0,0,1);
          glBegin (GL_LINE_STRIP);
            glVertex2f (-2,125);
            glVertex2f (-2,135);
            glVertex2f (2,135);
            glVertex2f (2,125);
          glEnd ();

          glRotated (-10.0,0,0,1);
          glBegin (GL_LINE_STRIP);
            glVertex2f (-2,125);
            glVertex2f (-2,145);
            glVertex2f (2,145);
            glVertex2f (2,125);
          glEnd ();

          glRotated (-15.0,0,0,1);
          glBegin (GL_LINES);
            glVertex2f (0,125);
            glVertex2f (0,135);
          glEnd ();

        glPopMatrix ();

        //----------------End Bank Markings----------------

      glPopMatrix ();

    } // end if ((!coldDarkPfd)

  } // end Render()

} // end namespace OpenGC

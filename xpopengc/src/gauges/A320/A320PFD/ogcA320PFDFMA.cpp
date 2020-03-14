/*=============================================================================

  This is the ogcA320PFDFMA.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Primary Flight Display: the Flight Mode Annunciator ===

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

===============================================================================

  The following datarefs are used by this instrument:
v	AirbusFBW/FMA1b
v	AirbusFBW/FMA1g
v	AirbusFBW/FMA1w
v	AirbusFBW/FMA2b
v	AirbusFBW/FMA2m
v	AirbusFBW/FMA2w
v	AirbusFBW/FMA3a
v	AirbusFBW/FMA3b
v	AirbusFBW/FMA3w
v	AirbusFBW/FMAAPFDBoxing
v	AirbusFBW/FMAATHRboxing
v	AirbusFBW/FMAATHRModeBox
v	AirbusFBW/FMAAPRightArmedBox
v	AirbusFBW/FMAAPLeftArmedBox

=============================================================================*/

#include "ogcGaugeComponent.h"
#include "ogcA320PFD.h"
#include "ogcA320PFDFMA.h"

namespace OpenGC
{

  A320PFDFMA::A320PFDFMA () {
    if (verbosity > 0) printf ("A320PFDFMA - constructing\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("A320PFDFMA - constructed\n");
  }

  A320PFDFMA::~A320PFDFMA () {}

  void A320PFDFMA::Render () {

    bool coldDarkPfd = true;

    GaugeComponent::Render ();

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_dark_pfd = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_dark_pfd == INT_MISS) coldDarkPfd = true; else coldDarkPfd = (*cold_dark_pfd != 0) ? true : false;

    if (!coldDarkPfd) {

      unsigned char *fma_1b = link_dataref_byte_arr ("AirbusFBW/FMA1b", 37, -1); if (strlen ((char*)fma_1b) > 37) fma_1b[37] = '\0';
      unsigned char *fma_1g = link_dataref_byte_arr ("AirbusFBW/FMA1g", 37, -1); if (strlen ((char*)fma_1g) > 37) fma_1g[37] = '\0';
      unsigned char *fma_1w = link_dataref_byte_arr ("AirbusFBW/FMA1w", 37, -1); if (strlen ((char*)fma_1w) > 37) fma_1w[37] = '\0';
      unsigned char *fma_2b = link_dataref_byte_arr ("AirbusFBW/FMA2b", 37, -1); if (strlen ((char*)fma_2b) > 37) fma_2b[37] = '\0';
      unsigned char *fma_2m = link_dataref_byte_arr ("AirbusFBW/FMA2m", 37, -1); if (strlen ((char*)fma_2m) > 37) fma_2m[37] = '\0';
      unsigned char *fma_2w = link_dataref_byte_arr ("AirbusFBW/FMA2w", 37, -1); if (strlen ((char*)fma_2w) > 37) fma_2w[37] = '\0';
      unsigned char *fma_3a = link_dataref_byte_arr ("AirbusFBW/FMA3a", 37, -1); if (strlen ((char*)fma_3a) > 37) fma_3a[37] = '\0';
      unsigned char *fma_3b = link_dataref_byte_arr ("AirbusFBW/FMA3b", 37, -1); if (strlen ((char*)fma_3b) > 37) fma_3b[37] = '\0';
      unsigned char *fma_3w = link_dataref_byte_arr ("AirbusFBW/FMA3w", 37, -1); if (strlen ((char*)fma_3w) > 37) fma_3w[37] = '\0';
      int *fma_pfd_box = link_dataref_int ("AirbusFBW/FMAAPFDboxing");
      int *fma_athr_box = link_dataref_int ("AirbusFBW/FMAATHRboxing");
      int *fma_athr_m_box = link_dataref_int ("AirbusFBW/FMAATHRModeBox");
      int *fma_r_armed_box = link_dataref_int ("AirbusFBW/FMAAPRightArmedBox");
      int *fma_l_armed_box = link_dataref_int ("AirbusFBW/FMAAPLeftArmedBox");

      if (verbosity > 2) {
        printf ("A320PFDFMA - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
        printf ("A320PFDFMA -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
        printf ("A320PFDFMA -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
        printf ("A320PFDFMA -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
      }

      glLineWidth (2.0);
      glColor3ub (COLOR_WHITE);

      // The FMA vertical separators
      glBegin (GL_LINE_STRIP);
        glVertex2f (88,  5);
        glVertex2f (88, 70);
      glEnd ();
      glBegin (GL_LINE_STRIP);
        glVertex2f (88 * 3,  5);
        glVertex2f (88 * 3, 70);
      glEnd ();
      glBegin (GL_LINE_STRIP);
        glVertex2f (88*4,  5);
        glVertex2f (88*4, 70);
      glEnd ();

      // FMA texts

      m_pFontManager->SetSize (m_Font, 14, 14);
      glColor3ub (COLOR_GREEN);
      m_pFontManager->Print (3, 50, (const char*) fma_1g, m_Font);

      glColor3ub (COLOR_CYAN);
      m_pFontManager->Print (3, 50, (const char*) fma_1b, m_Font);
      m_pFontManager->Print (3, 30, (const char*) fma_2b, m_Font);
      m_pFontManager->Print (3, 10, (const char*) fma_3b, m_Font);

      glColor3ub (COLOR_WHITE);
      m_pFontManager->Print (3, 50, (const char*) fma_1w, m_Font);
      m_pFontManager->Print (3, 30, (const char*) fma_2w, m_Font);
      m_pFontManager->Print (3, 10, (const char*) fma_3w, m_Font);
//      m_pFontManager->Print (3, 50, (const char*) "ABCDEFGHIJKLMNOPQRSTUVWXYZ.:,;(*!?')", m_Font);
//      m_pFontManager->Print (3, 50, (const char*) "ABCDEFGHIJKLMNOPQRSTUVWXYZ.: ;(*  ')", m_Font); // three glyphs not supported...
//      m_pFontManager->Print (3, 30, (const char*) "abcdefghijklmnopqrstuvwxyz0123456789", m_Font);
      m_pFontManager->Print (3, 10, (const char*) fma_3w, m_Font);

      glColor3ub (COLOR_MAGENTA);
      m_pFontManager->Print (3, 30, (const char*) fma_2m, m_Font);

      glColor3ub (COLOR_AMBER);
      m_pFontManager->Print (3, 10, (const char*) fma_3a, m_Font);

      // Various boxes

      glPushMatrix();
        glLineWidth (2);

        if ((*fma_athr_box != INT_MISS) && (*fma_athr_box != 0)) {
          switch (*fma_athr_box) {
            case 1:
              glColor3ub (COLOR_WHITE);
              glBegin (GL_LINE_LOOP);
                glVertex2f (1, 27);
                glVertex2f (1, 67);
                glVertex2f (86, 67);
                glVertex2f (86, 27);
              glEnd ();
              break;
            case 2:
              glColor3ub (COLOR_YELLOW);
              glBegin (GL_LINE_LOOP);
                glVertex2f (1, 27);
                glVertex2f (1, 67);
                glVertex2f (86, 67);
                glVertex2f (86, 27);
              glEnd ();
              break;
            case 3:
              glColor3ub (COLOR_YELLOW);
              glBegin (GL_LINE_LOOP);
                glVertex2f (1, 47);
                glVertex2f (1, 67);
                glVertex2f (86, 67);
                glVertex2f (86, 47);
              glEnd ();
              break;
            default:
              break;
          }
        }

        if (*fma_pfd_box != INT_MISS) {
          glColor3ub (COLOR_WHITE);
          switch (*fma_pfd_box) {
            case 0:
              glBegin (GL_LINE_STRIP);
                glVertex2f (88 * 2,  5);
                glVertex2f (88 * 2, 70);
              glEnd ();
              break;
            case 1:
              glBegin (GL_LINE_STRIP);
                glVertex2f (88 * 2, 25);
                glVertex2f (88 * 2, 70);
              glEnd ();
              break;
            case 2:
              glBegin (GL_LINE_STRIP);
                glVertex2f (88 * 2,  5);
                glVertex2f (88 * 2, 70);
              glEnd ();
              glBegin (GL_LINE_LOOP);
                glVertex2f (88 + 3, 47);
                glVertex2f (88 + 3, 67);
                glVertex2f (88 * 2-3, 67);
                glVertex2f (88 * 2-3, 47);
              glEnd ();
              break;
            case 3:
              glBegin (GL_LINE_STRIP);
                glVertex2f (88 * 2, 25);
                glVertex2f (88 * 2, 70);
              glEnd ();
              glBegin (GL_LINE_LOOP);
                glVertex2f (88 + 3, 47);
                glVertex2f (88 + 3, 67);
                glVertex2f (88 * 2-3, 67);
                glVertex2f (88 * 2-3, 47);
              glEnd ();
              break;
            case 4:
              glBegin (GL_LINE_STRIP);
                glVertex2f (88 * 2,  5);
                glVertex2f (88 * 2, 70);
              glEnd ();
              glBegin (GL_LINE_LOOP);
                glVertex2f (88 * 2 + 3, 47);
                glVertex2f (88 * 2 + 3, 67);
                glVertex2f (88 * 3 - 3, 67);
                glVertex2f (88 * 3 - 3, 47);
              glEnd ();
              break;
            case 5:
              glBegin (GL_LINE_STRIP);
                glVertex2f (88 * 2, 25);
                glVertex2f (88 * 2, 70);
              glEnd ();
              glBegin (GL_LINE_LOOP);
                glVertex2f (88 * 2 + 3, 47);
                glVertex2f (88 * 2 + 3, 67);
                glVertex2f (88 * 3 - 3, 67);
                glVertex2f (88 * 3 - 3, 47);
              glEnd ();
              break;
            case 6:
              glBegin (GL_LINE_STRIP);
                glVertex2f (88 * 2,  5);
                glVertex2f (88 * 2, 70);
              glEnd ();
              glBegin (GL_LINE_LOOP);
                glVertex2f (88 + 3, 47);
                glVertex2f (88 + 3, 67);
                glVertex2f (88 * 2 - 3, 67);
                glVertex2f (88 * 2 - 3, 47);
              glEnd ();
              glBegin (GL_LINE_LOOP);
                glVertex2f (88 * 2 + 3, 47);
                glVertex2f (88 * 2 + 3, 67);
                glVertex2f (88 * 3 - 3, 67);
                glVertex2f (88 * 3 - 3, 47);
              glEnd ();
              break;
            case 7:
              glBegin (GL_LINE_STRIP);
                glVertex2f (88 * 2, 25);
                glVertex2f (88 * 2, 70);
              glEnd ();
              glBegin (GL_LINE_LOOP);
                glVertex2f (88 + 3, 47);
                glVertex2f (88 + 3, 67);
                glVertex2f (88 * 2 - 3, 67);
                glVertex2f (88 * 2 - 3, 47);
              glEnd ();
              glBegin (GL_LINE_LOOP);
                glVertex2f (88 * 2 + 3, 47);
                glVertex2f (88 * 2 + 3, 67);
                glVertex2f (88 * 3 - 3, 67);
                glVertex2f (88 * 3 - 3, 47);
              glEnd ();
              break;
            case 8:
              break;
            case 9:
              glBegin (GL_LINE_LOOP);
                glVertex2f (88 + 3, 47);
                glVertex2f (88 + 3, 67);
                glVertex2f (88 * 3 - 3, 67);
                glVertex2f (88 * 3 - 3, 47);
              glEnd ();
              break;
            default:
              break;
          }
        }

        glColor3ub (COLOR_WHITE);

        if (*fma_athr_m_box == 1) {
          // show arm-box for A/THR
          glBegin (GL_LINE_LOOP);
            glVertex2f (3, 47);
            glVertex2f (3, 67);
            glVertex2f (86, 67);
            glVertex2f (86, 47);
          glEnd ();
        }

        if (*fma_r_armed_box == 1) {
          // show arm-box for hor nav
          glBegin (GL_LINE_LOOP);
            glVertex2f (88 * 2 + 3, 27);
            glVertex2f (88 * 2 + 3, 47);
            glVertex2f (88 * 3 - 3, 47);
            glVertex2f (88 * 3 - 3, 27);
          glEnd ();
        }

        if (*fma_l_armed_box == 1) {
          // show arm-box for vert nav
          glBegin (GL_LINE_LOOP);
            glVertex2f (88 + 3, 27);
            glVertex2f (88 + 3, 47);
            glVertex2f (88 * 2-3, 47);
            glVertex2f (88 * 2-3, 27);
          glEnd ();
        }

      glPopMatrix();

    } // end if (!coldDarkPfd)

  } // end Render ()

} // end namespace OpenGC

/*=============================================================================

  This is the ogcA320PFDAltitudeTicker.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Primary Flight Display: the Altitude Ticker ===

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
v	sim/cockpit2/gauges/indicators/altitude_ft_pilot	float	(not found in the 2D panel...)
v	AirbusFBW/CaptALTValid					int

=============================================================================*/

#include <stdio.h>

#include "ogcGaugeComponent.h"
#include "ogcA320PFD.h"
#include "ogcA320PFDAltitudeTicker.h"

namespace OpenGC {

  A320PFDAltitudeTicker::A320PFDAltitudeTicker () {
    if (verbosity > 0) printf ("A320PFDAltitudeTicker - constructing\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("A320PFDAltitudeTicker - constructed\n");
  }

  A320PFDAltitudeTicker::~A320PFDAltitudeTicker () {}

  void A320PFDAltitudeTicker::Render () {

    bool coldDarkPfd = true;

    // Call base class to setup viewport and projection
    GaugeComponent::Render ();

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_dark_pfd = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_dark_pfd == INT_MISS) coldDarkPfd = true; else coldDarkPfd = (*cold_dark_pfd != 0) ? true : false;

    // Altitude indicators valid?
    int* altValid = link_dataref_int ("AirbusFBW/CaptALTValid");

    // current altitude (feet)
    int alt;
    float *pressAltFeet = link_dataref_flt ("sim/cockpit2/gauges/indicators/altitude_ft_pilot" ,0);
    if (*pressAltFeet != FLT_MISS) alt = (long) *pressAltFeet; else alt = 0;

    if (verbosity > 2) {
      printf ("A320PFDAltitudeTicker - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320PFDAltitudeTicker -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320PFDAltitudeTicker -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320PFDAltitudeTicker -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    if (!coldDarkPfd) {

    if (*altValid != 1) {
      // don't even think of drawing this thing!
    } else {
      long test;

      double partWidth = m_PhysicalSize.x;
      double partHeight = m_PhysicalSize.y;

      double bigFontHeight = 15;
      double bigFontWidth = 14;
      double littleFontHeight = 13;
      double littleFontWidth = 9;
      double textx = 6;
      double texty = (partHeight-bigFontHeight) * 0.5;
      double textyy = texty - (bigFontHeight - littleFontHeight) / 2;
      double textBorder = partWidth - 2;
      double textSep = textBorder - 2 - 2 * littleFontWidth;
      m_pFontManager->SetSize (m_Font, bigFontWidth, bigFontHeight);

      // Draw black background
      glColor3ub (COLOR_BLACK);
      // Rectangular part
      glBegin (GL_POLYGON);
        glVertex2f (10, partHeight*0.75);
        glVertex2f (textSep, partHeight*0.75);
        glVertex2f (textSep, partHeight*0.25);
        glVertex2f (10, partHeight*0.25);
      glEnd ();
      glBegin (GL_POLYGON);
        glVertex2f (textSep, partHeight);
        glVertex2f (textBorder, partHeight);
        glVertex2f (textBorder, 0);
        glVertex2f (textSep, 0);
      glEnd ();

      char buffer[24];

      // Draw text in green
      glColor3ub (COLOR_GREEN);
      sprintf (buffer, "%03i", abs(alt) / 100);
      m_pFontManager->Print (textx + 8, texty, buffer, m_Font);

      // The 10's and 1's are drawn in a smaller size
      m_pFontManager->SetSize (m_Font, littleFontWidth, littleFontHeight);
  
      // The 10's and 1's position (which is always 0) scroll based on altitude
      // Note that the tens digit is always a multiple of 2
      int middle_ten = (abs(alt) % 100 / 10);

      bool roundupnine = false;

      switch (middle_ten)
      {
        case 1:
          middle_ten = 2;
          break;
        case 3:
          middle_ten = 4;
          break;
        case 5:
          middle_ten = 6;
          break;
        case 7:
          middle_ten = 8;
          break;
        case 9:
          middle_ten = 0;
          roundupnine = true;
          break;
      }

      // Figure out the translation for the tens and ones position
      double vertTranslation;
      if (middle_ten != 0)
        vertTranslation = (middle_ten * 10 - (double)(abs (alt) % 100)) / 10 * littleFontHeight;
      else {
        if (roundupnine==true)
          vertTranslation = (100 - (double)(abs(alt)%100)) / 10 * littleFontHeight;
        else
          vertTranslation = (0 - (double)(abs(alt)%100)) / 10 * littleFontHeight;
      }
      glTranslated (0, vertTranslation, 0);

      // Now figure out the digits above and below
      int top_ten = (middle_ten + 2) % 10;
      int bottom_ten = (middle_ten - 2 + 10) % 10;

      // Set the horizontal displacement of the two digits
      int onesx = textBorder - littleFontWidth;
      int tensx = onesx - littleFontWidth;

      // Display all of the digits
      sprintf (buffer, "%i", top_ten);
      m_pFontManager->Print (tensx, textyy + 2 * littleFontHeight, &buffer[0], m_Font);
      m_pFontManager->Print (onesx, textyy + 2 * littleFontHeight, "0", m_Font);

      sprintf (buffer, "%i", middle_ten);
      m_pFontManager->Print (tensx, textyy, &buffer[0], m_Font);
      m_pFontManager->Print (onesx, textyy, "0", m_Font);

      sprintf (buffer, "%i", bottom_ten);
      m_pFontManager->Print (tensx, textyy + -2 * littleFontHeight, &buffer[0], m_Font);
      m_pFontManager->Print (onesx, textyy + -2 * littleFontHeight, "0", m_Font);

      glTranslated (0, -vertTranslation, 0);

      // Yellow border around background
      glColor3ub (COLOR_YELLOW);
      glLineWidth(3.0);
      glBegin (GL_LINE_STRIP);
        glVertex2f (10, partHeight * 0.75);
        glVertex2f (textSep, partHeight * 0.75);
        glVertex2f (textSep, partHeight);
        glVertex2f (textBorder, partHeight);
        glVertex2f (textBorder, 0);
        glVertex2f (textSep, 0);
        glVertex2f (textSep, partHeight * 0.25);
        glVertex2f (10, partHeight * 0.25);
      glEnd ();
    }

    } // end if ((!coldDarkPfd)

  } // end Render()

} // end namespace OpenGC

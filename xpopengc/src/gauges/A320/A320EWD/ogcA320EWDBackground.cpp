/*=============================================================================

  This is the ogcA320EWDBackground.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Engine/Warning Display Background ===

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
#include "ogcA320EWDBackground.h"

namespace OpenGC {

  A320EWDBackground::A320EWDBackground () {
    if (verbosity > 1) printf ( "A320EWDBackground - constructing\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 460;
    m_PhysicalSize.y =  50;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 0) printf ("A320EWDBackground - constructed\n");
  }

  A320EWDBackground::~A320EWDBackground () {}

  void A320EWDBackground::Render () {

    bool coldAndDark = true;

    GaugeComponent::Render ();

    if (verbosity > 2) {
      printf ("A320EWDBackground - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320EWDBackground -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320EWDBackground -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320EWDBackground -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    // The datarefs we want to use on this instrument (This one is not used anywhere ??

    int ewdStartMode = 0;
    int *ewd_start_mode = link_dataref_int ("AirbusFBW/EWDStartMode");
    if (*ewd_start_mode != INT_MISS) ewdStartMode = *ewd_start_mode;

    if (!coldAndDark) {

//===============================Faking flight phase===========================
/*
      int apPhase = -1; char apBuffer[32];
      int *ap_flight_phase = link_dataref_int ("AirbusFBW/APPhase");
      if (*ap_flight_phase != INT_MISS) apPhase = *ap_flight_phase;
      m_pFontManager->SetSize (m_Font, 12, 12);
      sprintf (apBuffer, "         AP Phase: % 2i ", apPhase);
      glColor3ub (COLOR_YELLOW);
      m_pFontManager->Print (60,  50, apBuffer, m_Font);
      int ecamFlightPhase = -1; char ecamBuffer[32];
      int *ecam_flight_phase = link_dataref_int ("AirbusFBW/ECAMFlightPhase");
      if (*ecam_flight_phase != INT_MISS) ecamFlightPhase = *ecam_flight_phase;
      m_pFontManager->SetSize (m_Font, 12, 12);
      sprintf (ecamBuffer, "ECAM Flight Phase: % 2i ", ecamFlightPhase);
      glColor3ub (COLOR_YELLOW);
      m_pFontManager->Print (60,  30, ecamBuffer, m_Font);
      int qpacFlightPhase = -1; char qpacBuffer[32];
      int *qpac_flight_phase = link_dataref_int ("AirbusFBW/QPACFlightPhase");
      if (*qpac_flight_phase != INT_MISS) qpacFlightPhase = *qpac_flight_phase;
      m_pFontManager->SetSize (m_Font, 12, 12);
      sprintf (qpacBuffer, "QPAC Flight Phase: % 2i ", qpacFlightPhase);
      glColor3ub (COLOR_YELLOW);
      m_pFontManager->Print (60,  10, qpacBuffer, m_Font);
*/
//=============================End Faking flight phase=========================

      // The fixed background lines

      glLineWidth (3);
      glColor3ub (COLOR_WHITE);
      glBegin (GL_LINE_STRIP);
        glVertex2d ( 10, 150);
        glVertex2d (270, 150);
      glEnd ();
      glBegin (GL_LINE_STRIP);
        glVertex2d (310, 150);
        glVertex2d (450, 150);
      glEnd ();
      glBegin (GL_LINE_STRIP);
        glVertex2d (290, 140);
        glVertex2d (290,  10);
      glEnd ();

      // The fixed background texts

      m_pFontManager->SetSize (m_Font, 12, 12);
      glColor3ub (COLOR_WHITE);
      m_pFontManager->Print (220, 400, "EPR",  m_Font);
      m_pFontManager->Print ( 25, 335, "FF",   m_Font);
      m_pFontManager->Print (222, 335, "EGT",  m_Font);
      m_pFontManager->Print (380, 335, "FF",   m_Font);
      m_pFontManager->Print (227, 270, "N1" ,  m_Font);
      m_pFontManager->Print ( 40, 250, "N2" ,  m_Font);
      m_pFontManager->Print (400, 250, "N2" ,  m_Font);
      m_pFontManager->Print ( 10, 160, "FOB:", m_Font);
      glColor3ub (COLOR_CYAN);
      m_pFontManager->Print ( 55, 335, "KG/H", m_Font);
      m_pFontManager->Print (410, 335, "KG/H", m_Font);
      m_pFontManager->Print (225, 320, "*C",   m_Font);
      m_pFontManager->Print (230, 255, "%",    m_Font);
      m_pFontManager->Print ( 70, 250, "%",    m_Font);
      m_pFontManager->Print (430, 250, "%",    m_Font);
      m_pFontManager->Print (120, 160, "KG",   m_Font);

    } // End if (!coldAndDark)

  } // End Render()

}

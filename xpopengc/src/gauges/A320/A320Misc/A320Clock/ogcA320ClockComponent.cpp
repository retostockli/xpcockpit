/*=============================================================================

  This is the ogcA320ClockComponent.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Main-Panel Clock ===

  Created:
    Date:        2015-06-14
    Author:      Hans Jansen
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
#include "ogcA320Clock.h"
#include "ogcA320ClockComponent.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320ClockComponent::A320ClockComponent () {
    if (verbosity > 0) printf("ogcA320ClockComponent - constructing\n");

    m_Font = m_pFontManager->LoadFont((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 2) printf ("ogcA320ClockComponent - constructed\n");
  }

  A320ClockComponent::~A320ClockComponent () {}

  // The variables for the elapsed-time display
  int etState = 0, etVal = 0, etStart = 0, etStop = 0;
  
  void A320ClockComponent::Render () {

    bool coldAndDark = true;
    char buff[36];

    // Call base class to setup for size and position
    GaugeComponent::Render ();

    // Request the datarefs we want to use

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

//    int *cy = link_dataref_int ("sim/cockpit2/clock_timer/current_year");        // not defined in sim & not used here
    int *cm = link_dataref_int ("sim/cockpit2/clock_timer/current_month");
    int *cd = link_dataref_int ("sim/cockpit2/clock_timer/current_day");
    int *zth = link_dataref_int ("sim/cockpit2/clock_timer/zulu_time_hours");
    int *ztm = link_dataref_int ("sim/cockpit2/clock_timer/zulu_time_minutes");
    int *zts = link_dataref_int ("sim/cockpit2/clock_timer/zulu_time_seconds");
    int *ds = link_dataref_int ("sim/cockpit2/clock_timer/date_is_showing");
//    int *tr = link_dataref_int ("sim/cockpit2/clock_timer/timer_running");       // not used here
//    int *eth = link_dataref_int ("sim/cockpit2/clock_timer/elapsed_time_hours"); // not used here
    int *etm = link_dataref_int ("sim/cockpit2/clock_timer/elapsed_time_minutes");
    int *ets = link_dataref_int ("sim/cockpit2/clock_timer/elapsed_time_seconds");
    float *trt = link_dataref_flt ("sim/time/total_running_time_sec", 0);
    float *engn1 = link_dataref_flt_arr ("sim/cockpit2/engine/indicators/N1_percent", 8, -1, 0);

    if (verbosity > 1) {
      printf ("A320ClockComponent - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320ClockComponent -    pixel position: %i %i\n", m_PixelPosition.x,    m_PixelPosition.y);
      printf ("A320ClockComponent -     physical size: %f %f\n", m_PhysicalSize.x,     m_PhysicalSize.y);
      printf ("A320ClockComponent -        pixel size: %i %i\n", m_PixelSize.x,        m_PixelSize.y);
    }

    if (!coldAndDark) {
      // The borders of the displays
      glColor3ub (COLOR_CYAN);
      glBegin (GL_LINE_STRIP);
        glVertex2f ( 35, 190); glVertex2f (130, 190); glVertex2f (130, 155); glVertex2f ( 35, 155); glVertex2f ( 35, 190);
      glEnd ();
      glBegin (GL_LINE_STRIP);
        glVertex2f ( 20, 120); glVertex2f (150, 120); glVertex2f (150,  85); glVertex2f ( 20,  85); glVertex2f ( 20, 120);
      glEnd ();
      glBegin (GL_LINE_STRIP);
        glVertex2f ( 35,  50); glVertex2f (130,  50); glVertex2f (130,  15); glVertex2f ( 35,  15); glVertex2f ( 35,  50);
      glEnd ();

      // The texts
      glColor3ub (COLOR_CYAN);
      m_pFontManager->SetSize (m_Font, 12, 12);
      m_pFontManager->Print ( 55, 200, "CHRONO",      m_Font);
      m_pFontManager->Print ( 30, 130, "DATE / TIME", m_Font);
      m_pFontManager->Print ( 75,  60, "ET",          m_Font);

      // The displays
      glColor3ub (COLOR_YELLOW);
      if (*ds == 0) {										// normal time display
        m_pFontManager->SetSize (m_Font, 16, 16);
	sprintf (buff, "%02d", *zts); m_pFontManager->Print (113,  95, buff, m_Font);
        m_pFontManager->SetSize (m_Font, 18, 18);
	sprintf (buff, "%02d:%02d", *zth, *ztm); m_pFontManager->Print ( 33,  93, buff, m_Font);
      } else {											// date display
        m_pFontManager->SetSize (m_Font, 16, 16);
	m_pFontManager->Print (113,  95, "  ", m_Font);
        m_pFontManager->SetSize (m_Font, 18, 18);
	sprintf (buff, "%02d:%02d", *cm, *cd); m_pFontManager->Print ( 33,  93, buff, m_Font);
      }
      // chrono display
      glColor3ub (COLOR_YELLOW);
      m_pFontManager->SetSize (m_Font, 18, 18);
      sprintf (buff, "%02d:%02d", *etm, *ets); m_pFontManager->Print ( 47, 163, buff, m_Font);

      // flight-time display
      glColor3ub (COLOR_YELLOW);
      etVal = (int) *trt;
      switch (etState) {
        case 0:
          m_pFontManager->Print ( 47,  23, "     ", m_Font);
          if (engn1[0] < 20 && engn1[1] < 20) etStart = etVal;    // before engine start
          else etState = 1;                                       // engine(s) starting
          break;
        case 1:
          sprintf (buff, "%02d:%02d", (int) (((etVal - etStart) + etStop) / 3600), (int) ((((etVal - etStart) + etStop) / 60) % 60));
          m_pFontManager->Print ( 47,  23, buff, m_Font);
          if (engn1[0] < 20 && engn1[1] < 20) {                   // engines have been shut down
            etStop += (etVal - etStart);
            etState = 2;
          }
          break;
        case 2:
          sprintf (buff, "%02d:%02d", (int) etStop / 3600, (int) etStop / 60 % 60);
          m_pFontManager->Print ( 47,  23, buff, m_Font);
          if (engn1[0] > 20 || engn1[1] > 20) {                   // engines have been restarted: continue counting!
            etStart = etVal;
            etState = 1;
          }
          break;
      } // end switch (etState)

    } // end if (! coldAndDark)

  } // end Render()

} // end namespace OpenGC

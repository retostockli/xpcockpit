/*=============================================================================

  This is the ogcA320EWDSlatsFlaps.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Engine/Warning Display - Slats/Flaps Indicator ===

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
#include "ogcA320EWDSlatsFlaps.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320EWDSlatsFlaps::A320EWDSlatsFlaps () {
    printf ("A320EWDSlatsFlaps constructed\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  A320EWDSlatsFlaps::~A320EWDSlatsFlaps () {}

  int slatReq = 0; float slatReqPos[] = { 0.000, 0.600, 0.800, 1.000 };
  int flapReq = 0; float flapReqPos[] = { 0.000, 0.250, 0.375, 0.500, 1.000 };

  void A320EWDSlatsFlaps::Render () {

    bool coldAndDark = true;
    CircleEvaluator aCircle;

    GaugeComponent::Render ();

    // The datarefs we want to use on this instrument

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    int *flap_req_pos = link_dataref_int ("AirbusFBW/FlapRequestPos");
    if ((*flap_req_pos != INT_MISS) && (*flap_req_pos >= 0)) { // can be -1, but that is an illegal value to work with...
      flapReq = *flap_req_pos;
//      flapReqPos = (float) flapReq * 0.20; // range 0 ... 5; normalized to 0.0 ... 1.0
    }

    int *slat_req_pos = link_dataref_int ("AirbusFBW/SlatRequestPos");
    if ((*slat_req_pos != INT_MISS) && (*slat_req_pos >= 0)) { // see remark on flap_req_pos
      slatReq = *slat_req_pos;
//      slatReqPos = (float) slatReq * 0.25; // range 0 ... 4; normalized to 0.0 ... 1.0
    }
    float slatRatio = 0;
    float *slat_rat  = link_dataref_flt ("sim/flightmodel/controls/slatrat", -2);
    if (*slat_rat != FLT_MISS) slatRatio = *slat_rat;

    float flapRatio = 0;
    float *flap_rat  = link_dataref_flt ("sim/flightmodel/controls/wing2l_fla1def", -2);
    if (*flap_rat != FLT_MISS) flapRatio = *flap_rat / 40; // funny dataref... why not "sim/flightmodel/controls/flaprat" ??

    if (!coldAndDark) {

      // The instrument display

      int sFHx =        90, sFHy = 39; // the slats home position
      int fFHx = sFHx + 10, fFHy = sFHy; // the flaps home position
      float sRPPx = 70 * 0.85; // horizontal and vertical offset units for the slats symbol
      float sRPPy = 70 * 0.30;
      float fRPPx = 90 * 0.95; // horizontal and vertical offset units for the flaps symbol
      float fRPPy = 90 * 0.25;

      glLineWidth (2);
      glColor3ub (COLOR_WHITE);
      glBegin (GL_LINE_LOOP); // slats/flaps central marker
        glVertex2d (sFHx - 5, sFHy - 10);
        glVertex2d (sFHx    , sFHy     );
        glVertex2d (fFHx    , fFHy     );
        glVertex2d (fFHx + 5, fFHy - 10);
      glEnd ();

      if ((slatRatio > 0.01) || (flapRatio > 0.01)) {
        glLineWidth (3);
        glColor3ub (COLOR_WHITE);
        aCircle.SetRadius (2);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (0, 360);
        aCircle.SetOrigin (sFHx - 10 - 0.6 * sRPPx, sFHy - 8 - 0.6 * sRPPy); // slats dots
        glBegin (GL_LINE_STRIP);
          aCircle.Evaluate ();
        glEnd ();
        aCircle.SetOrigin (sFHx - 10 - 0.8 * sRPPx, sFHy - 8 - 0.8 * sRPPy);
        glBegin (GL_LINE_STRIP);
          aCircle.Evaluate ();
        glEnd ();
        aCircle.SetOrigin (sFHx - 10 - 1.0 * sRPPx, sFHy - 8 - 1.0 * sRPPy);
        glBegin (GL_LINE_STRIP);
          aCircle.Evaluate ();
        glEnd ();
        m_pFontManager->Print (sFHx - 58, sFHy - 10, "S", m_Font);
        glLineWidth (3);
        aCircle.SetOrigin (fFHx + 10 + 0.250 * fRPPx, fFHy - 8 - 0.250 * fRPPy); // flaps dots
        glBegin (GL_LINE_STRIP);
          aCircle.Evaluate ();
        glEnd ();
        aCircle.SetOrigin (fFHx + 10 + 0.375 * fRPPx, fFHy - 8 - 0.375 * fRPPy);
        glBegin (GL_LINE_STRIP);
          aCircle.Evaluate ();
        glEnd ();
        aCircle.SetOrigin (fFHx + 10 + 0.500 * fRPPx, fFHy - 8 - 0.500 * fRPPy);
        glBegin (GL_LINE_STRIP);
          aCircle.Evaluate ();
        glEnd ();
        aCircle.SetOrigin (fFHx + 10 + 1.000 * fRPPx, fFHy - 8 - 1.000 * fRPPy);
        glBegin (GL_LINE_STRIP);
          aCircle.Evaluate ();
        glEnd ();
        m_pFontManager->Print (fFHx + 58, fFHy - 10, "F", m_Font);
      }

      // The Slats - requested position

      float srpx = sFHx - slatReqPos[slatReq] * sRPPx;
      float srpy = sFHy - slatReqPos[slatReq] * sRPPy;

      glLineWidth (2);
      glColor3ub (COLOR_CYAN);
      glBegin (GL_LINES);
        glVertex2f (sFHx, sFHy);
        glVertex2f (srpx, srpy);
      glEnd ();
      glBegin (GL_LINE_LOOP);
        glVertex2d (srpx     , srpy     );
        glVertex2d (srpx -  5, srpy - 10);
        glVertex2d (srpx - 20, srpy - 15);
        glVertex2d (srpx - 15, srpy -  5);
      glEnd ();

      // The Slats - actual position

      float sapx = sFHx - slatRatio * sRPPx;
      float sapy = sFHy - slatRatio * sRPPy;

      glLineWidth (2);
      glColor3ub (COLOR_GREEN);
      glBegin (GL_LINES);
        glVertex2f (sFHx, sFHy);
        glVertex2f (sapx, sapy);
      glEnd ();
      glBegin (GL_LINE_LOOP);
        glVertex2d (sapx     , sapy     );
        glVertex2d (sapx -  5, sapy - 10);
        glVertex2d (sapx - 20, sapy - 15);
        glVertex2d (sapx - 15, sapy -  5);
      glEnd ();

      // The Flaps - requested position

      float frpx = fFHx + flapReqPos[flapReq] * fRPPx;
      float frpy = fFHy - flapReqPos[flapReq] * fRPPy;

      glLineWidth (2);
      glColor3ub (COLOR_CYAN);
      glBegin (GL_LINES);
        glVertex2f (fFHx, fFHy);
        glVertex2f (frpx, frpy);
      glEnd ();
      glBegin (GL_LINE_LOOP);
        glVertex2d (frpx     , frpy     );
        glVertex2d (frpx + 15, frpy -  4);
        glVertex2d (frpx + 20, frpy - 14);
        glVertex2d (frpx +  5, frpy -  9);
      glEnd ();

      // The Flaps - actual position

      float fapx = fFHx + flapRatio * fRPPx;
      float fapy = fFHy - flapRatio * fRPPy;

      glLineWidth (2);
      glColor3ub (COLOR_GREEN);
      glBegin (GL_LINES);
        glVertex2f (fFHx, fFHy);
        glVertex2f (fapx, fapy);
      glEnd ();
      glBegin (GL_LINE_LOOP);
        glVertex2d (fapx     , fapy     );
        glVertex2d (fapx + 15, fapy -  4);
        glVertex2d (fapx + 20, fapy - 14);
        glVertex2d (fapx +  5, fapy -  9);
      glEnd ();

      // The Flaps-Handle annunciator

      if ((slatRatio > 0.01) || (flapRatio > 0.01)) {
        m_pFontManager->SetSize (m_Font, 12, 12);
        glColor3ub (COLOR_GREEN);
        switch (flapReq) {
          case 0:
            m_pFontManager->Print (90, 0, "0",    m_Font);
            break;
          case 1:
            m_pFontManager->Print (80, 0, "1+F",  m_Font); // "1" only if not on ground!
            break;
          case 2:
            m_pFontManager->Print (90, 0, "2",    m_Font);
            break;
          case 3:
            m_pFontManager->Print (90, 0, "3",    m_Font);
            break;
          case 4:
            m_pFontManager->Print (80, 0, "FULL", m_Font);
            break;
          default:
            break;
        } // end switch ()

      } // end if ()

    } // End if (!ColdAndDark)

  } // End Render()

} // end namespace OpenGC

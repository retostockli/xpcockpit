/*=============================================================================

  This is the ogcA320NDPlan.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Navigation Display ===
       (based on Reto's ogcB737NAV stuff)

  Created:
    Date:   2011-11-14
    Author: Hans Jansen
    last change: 2020-02-01
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

#include <stdio.h>
#include <math.h>

#include "ogcGaugeComponent.h"
#include "ogcA320ND.h"
#include "ogcA320NDPlan.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320NDPlan::A320NDPlan () {
    if (verbosity > 0) printf("ogcA320NDPlan - constructing\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf("ogcA320NDPlan - constructed\n");
  }

  A320NDPlan::~A320NDPlan () {}

  static char buff[80];

  // The variables defined in the A320NDWidget
  extern  bool avionicsOn;
  extern   int ndPage,        ndRange;
  extern float ndRanges[][4];
  extern   int ndRange;
  extern   int headingValid,  mapAvail,      oppsideMsg;
  extern   int utcHours,      utcMinutes,    utcSeconds;
  extern   int headingDev;
  extern   int efis1SelPilot, efis2SelPilot;
  extern   int ilsVDefVis,    ilsHDefVis;

  extern float hdgMag,   hdgTrue,  gndSpd,   airSpd,   wndDirT,  wndDirM,  wndSpd,  chronoCapt;
  extern float brgNav1,  brgNav2,  brgAdf1,  brgAdf2;
  extern float distNav1, distNav2, distAdf1, distAdf2;
  extern float crsNav1,  hdefNav1, vdefNav1, freqNav1;
  extern float crsIls,   hdefIls,  vdefIls,  freqIls;
  extern float tcasHdg,  tcasAlt,  tcasDist;
  extern float gndSpd,   airSpd,   wndDirT,  wndDirM,  wndSpd,   chronoCapt;

  extern double acLat,   acLon;

  extern unsigned char *adf1_name, *adf2_name, *ils_name, *nav1_name, *nav2_name;
  extern unsigned char *fp_dept_name, *fp_dest_name, *fp_alt_name, *fp_wp_id;

  extern unsigned char *toWptId;
  extern float wptDist, wptCrs;
  extern int   wptHrs,  wptMins;

  extern int fpNoWp, fpToWp;
 
  //  extern int *nav_shows_wxr;
  //  extern int *nav_shows_acf;
  extern int *nav_shows_apt;
  extern int *nav_shows_cst;
  extern int *nav_shows_fix;
  extern int *nav_shows_vor;
  extern int *nav_shows_ndb;

  extern double dtor;      // degree to radian
  // extern double rtod;   //radian to degree
  // extern double mtok;   // nautical mile to kilometer
  // extern double ktom;   // kilometer to nautical mile

  void A320NDPlan::Render () {

    bool coldAndDark = true;
    CircleEvaluator aCircle;

    // Call base class to setup for size and position
    GaugeComponent::Render ();

    float fontSize = 12;

    // (Re-)Define the datarefs we want to use (just a few examples here)

    if (verbosity > 2) {
      printf ("A320NDPlan - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320NDPlan -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320NDPlan -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320NDPlan -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    double partSize = m_PhysicalSize.x; // defines total component size (just for name simplification) (square gauge!)
    double partCenter = partSize / 2;   // defines component center

    m_pFontManager->SetSize (m_Font, 1.5 * fontSize, 1.5 * fontSize);

    if ((!coldAndDark) && (avionicsOn == 1) &&  (ndPage == 4)) {

      glPushMatrix ();

        // do any rendering here
        glColor3ub (COLOR_WHITE);
        m_pFontManager->Print ( 220, 470, "PLAN", m_Font );
        m_pFontManager->SetSize (m_Font, fontSize, fontSize);
//        glColor3ub (COLOR_GREEN);

        // The Compass Points
        glColor3ub (COLOR_WHITE);
        glLineWidth (2);
        glBegin (GL_TRIANGLES);
          glVertex2d (244, 402); glVertex2d (249, 414); glVertex2d (254, 402); // N
          glVertex2d (244, 100); glVertex2d (249,  88); glVertex2d (254, 100); // S
          glVertex2d ( 89, 250); glVertex2d ( 99, 256); glVertex2d ( 99, 244); // W
          glVertex2d (412, 250); glVertex2d (402, 256); glVertex2d (402, 244); // E
        glEnd ();
        m_pFontManager->SetSize (m_Font, 14, 14);
        glColor3ub (COLOR_WHITE);
        m_pFontManager->Print (244, 384, "N", m_Font);
        m_pFontManager->Print (244, 104, "S", m_Font);
        m_pFontManager->Print (103, 244, "W", m_Font);
        m_pFontManager->Print (388, 244, "E", m_Font);

        // The Plane Symbol
        if (headingValid == 1) {
          glPushMatrix ();
            glLineWidth (5);
            glColor3ub (COLOR_YELLOW);
            glTranslated (250, 250, 0);
            glRotated (hdgMag * -1, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2d (-25,   0); glVertex2d ( 25 ,  0);
              glVertex2d (-10, -30); glVertex2d ( 10, -30);
              glVertex2d (  0,  15); glVertex2d (  0, -35);
            glEnd ();
          glPopMatrix ();
        }

        // Mask off the Navaids displays
        glColor3ub (COLOR_BLACK);
        glBegin(GL_POLYGON);
          glVertex2d (  0,   0);
          glVertex2d (  0,  85);
          glVertex2d (500,  85);
          glVertex2d (500,   0);
        glEnd();

      glPopMatrix ();

/* Test code for flight plan ... does not seem to work!
int lnBase = 0;
glColor3ub(COLOR_WHITE);
m_pFontManager->SetSize (m_Font, 10, 10);
sprintf(buff, "Dept: -%5s- dest: -%5s- alt: -%5s-", fp_dept_name, fp_dest_name, fp_alt_name);
m_pFontManager->Print ( 20,  lnBase, buff, m_Font); lnBase += 14;
sprintf(buff, "ToWPT: %5s dist: %1.1f crs: %03.0f time: %02i:%02i", toWptId, wptDist, wptCrs, wptHrs, wptMins);
m_pFontManager->Print ( 20,  lnBase, buff, m_Font); lnBase += 14;
sprintf(buff, "fpNoWpt: %02i fpToWp: %02i", fpNoWp, fpToWp);
m_pFontManager->Print ( 20,  lnBase, buff, m_Font); lnBase += 14;
/* end of fp test code */

    } // end if (coldAndDark)
    
  } // end of Render() ...

} // end namespace OpenGC

/*=============================================================================

  This is the ogcA320NDNav.cpp file, part of the OpenGC subproject
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
#include "ogcA320NDNav.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320NDNav::A320NDNav () {
    if (verbosity > 0) printf ("ogcA320NDNav - constructing\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("ogcA320NDNav - constructed\n");
  }

  A320NDNav::~A320NDNav () {}

  // The variables defined in the A320NDWidget
  extern bool avionicsOn;
  extern int ndPage, ndRange;
  extern int headingValid, mapAvail, oppsideMsg;
  extern int utcHours, utcMinutes, utcSeconds;
  extern int headingDev;
  extern int efis1SelPilot, efis2SelPilot;
  extern int ilsVDefVis, ilsHDefVis;

  //  extern int *nav_shows_wxr;
  //  extern int *nav_shows_acf;
  extern int *nav_shows_apt;
  extern int *nav_shows_cst;
  extern int *nav_shows_fix;
  extern int *nav_shows_vor;
  extern int *nav_shows_ndb;

  extern float hdgMag, hdgTrue, gndSpd, airSpd, wndDirT, wndDirM, wndSpd, chronoCapt;
  extern float brgNav1, brgNav2, brgAdf1, brgAdf2;
  extern float distNav1, distNav2, distAdf1, distAdf2;
  extern float crsNav1, hdefNav1, vdefNav1, freqNav1;
  extern float crsIls, hdefIls, vdefIls, freqIls;
  extern float tcasHdg, tcasAlt, tcasDist;
  extern float wptDist, wptCrs;

  extern double acLat, acLon;

  extern unsigned char *adf1_name, *adf2_name, *ils_name, *nav1_name, *nav2_name;

  static char buff[80];

  extern int efis1SelPilot, efis2SelPilot;
  extern float hdgMag, gndSpd, airSpd, wndDirT, wndDirM, wndSpd, chronoCapt;
  extern float brgNav1, brgNav2, brgAdf1, brgAdf2;
  extern float crsNav1, hdefNav1, vdefNav1, freqNav1;
  extern float crsIls, hdefIls, vdefIls, freqIls;
  extern float wptDist, wptCrs;
  extern float ndRanges[][4];
  extern int ndRange;

  extern double dtor;      // degree to radian
  // extern double rtod;   //radian to degree
  // extern double mtok;   // nautical mile to kilometer
  // extern double ktom;   // kilometer to nautical mile

  void A320NDNav::Render () {

    bool coldAndDark = true;
    CircleEvaluator aCircle;

    // define map center position
    int mapSize = m_PhysicalSize.x * 0.5;  // half of total component size (just for name simplification <- square gauge!)
    int mapCenterX = mapSize * 1.0, mapCenterY = mapSize *1.0; // defines arc center (now resp. 250 and 250)
    int mapCircle  = mapSize * 0.64; // radius of the outer range circle (now 160)
    int mapCircle1 = mapSize * 0.32; // radius of the inner range circle (now  80)
    int mapRange;
    float symbolSize = mapSize * 0.04; // size of the symbols in the display

    // Call base class to setup for size and position
    GaugeComponent::Render ();

    float fontSize = 12;

    if (verbosity > 2)
    {
      printf ("A320NDNav - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320NDNav -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320NDNav -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320NDNav -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    if ((!coldAndDark) && (avionicsOn == 1) &&  (ndPage == 2)) {

      glPushMatrix ();
        glColor3ub (COLOR_WHITE);
        m_pFontManager->SetSize (m_Font, 18, 18);
        m_pFontManager->Print ( 220, 470, "NAV", m_Font );

        if ((headingValid == 1) && (mapAvail == 1)) {
          // Mapping the environment <= BEGIN =============

          // The input coordinates are in lon/lat, so we have to rotate against true heading
          // despite the NAV display is showing mag heading
          if (hdgTrue != FLT_MISS) {

            // Shift center and rotate about heading
            glMatrixMode (GL_MODELVIEW);
            glPushMatrix ();
            if (ndPage == 3) glTranslatef (mapCenterX, mapCenterY, 0.0); // ARC page: (250, 100)
            else glTranslatef (mapCenterX, mapCenterX, 0.0); // other pages: (250, 250)
            if (ndPage != 4) glRotatef (hdgTrue, 0, 0, 1); // NOT on PLAN page!

            // valid coordinates?
            if ((acLon >= -180.0) && (acLon <= 180.0) && (acLat >= -90.0) && (acLat <= 90.0)) {

              // Set up circle for small symbols
              CircleEvaluator aCircle;
              aCircle.SetArcStartEnd (0, 360);
              aCircle.SetDegreesPerPoint (10);

              double northing;
              double easting;
              double northing2;
              double easting2;
              double lon;
              double lat;
              float xPos;
              float yPos;
              float xPosR;
              float yPosR;
              float xPos2;
              float yPos2;
              float sideL;
              float sideR;
              double acLon2;
              double acLat2;
              mapRange = ndRanges[ndRange][3] / 2;

              // calculate approximate lon/lat range to search for given selected map range
              int nlat = ceil (mapRange * 1852.0 / (2.0 * 3.141592 * 6378137.0) * 360.0);
              int nlon;
              if (fabs (acLat) < 89.5) {
                nlon = ceil (mapRange * 1852.0 / (2.0 * 3.141592 * 6378137.0) * 360.0 / cos (acLat * 0.0174533));
              } else nlon = 180; // north/south pole: span full longitude range

              for (int la = -nlat; la <= nlat; la++) {
                for (int lo = -nlon; lo <= nlon; lo++) {

                  acLon2 = acLon + (double) lo;
                  acLat2 = acLat + (double) la;

                  // only display Fixes and Navaids with a map range <= 40 nm
                  if (ndRange < 3) {
                    if (*nav_shows_fix == 1) {

                      //----------Fixes-----------

                      GeographicObjectList::iterator fixIt;
                      GeographicHash* pFixHash = m_pNavDatabase->GetFixHash ();
                      std::list<OpenGC::GeographicObject*>* pFixList = pFixHash->GetListAtLatLon (acLat2, acLon2);

                      for (fixIt = pFixList->begin (); fixIt != pFixList->end (); ++fixIt) {

                        lon = (*fixIt)->GetDegreeLon ();
                        lat = (*fixIt)->GetDegreeLat ();

                        // convert to azimuthal equidistant coordinates with acf in center
                        lonlat2gnomonic (&lon, &lat, &easting, &northing, &acLon, &acLat);

                        // Compute physical position relative to acf center on screen
                        yPos = -northing / 1852.0 / mapRange * mapCircle;
                        xPos = easting / 1852.0 / mapRange * mapCircle;

                        // Only draw the nav if it's visible within the rendering area
                        if (sqrt (xPos * xPos + yPos * yPos) < mapCircle) {

                          // calculate physical position on heading rotated nav map
                          float rotateRad = -1.0 * hdgTrue * dtor;

                          xPosR = cos (rotateRad) * xPos + sin (rotateRad) * yPos;
                          yPosR = -sin (rotateRad) * xPos + cos (rotateRad) * yPos;

                          sideL = (xPosR + 0.95 * mapRange) * (-0.25 * mapRange - 0.75 * mapRange) -
                              (yPosR - 0.75 * mapRange) * (-0.15 * mapRange + 0.75 * mapRange);
                          sideR = (xPosR - 0.95 * mapRange) * (-0.25 * mapRange - 0.75 * mapRange) -
                              (yPosR - 0.75 * mapRange) * (0.15 * mapRange - 0.75 * mapRange);

                          // Only draw the nav if it is in the upper section of the screen
                          //  if ((sideL <= 0.0) && (sideR >= 0.0)) {

                          // We're going to be clever and "derotate" each label
                          // by the heading, so that everything appears upright despite
                          // the global rotation. This makes all of the labels appear at
                          // 0,0 in the local coordinate system
                          glPushMatrix ();
                          
                          glTranslatef (xPos, yPos, 0.0);
                          glRotatef (-1.0 * hdgTrue, 0, 0, 1);

                          // Fix: magenta diamond
                          glColor3ub (COLOR_MAGENTA);
                          glLineWidth (2.0);
                          glBegin (GL_LINE_LOOP);
                            glVertex2f ( 0, -4);
                            glVertex2f ( 4,  0);
                            glVertex2f ( 0,  4);
                            glVertex2f (-4,  0);
                          glEnd ();

                          m_pFontManager->SetSize (m_Font, 10, 10); // <= (m_Font, 0.65*fontSize, 0.65*fontSize);
                          m_pFontManager->Print (8, 6, (*fixIt)->GetIdentification ().c_str(), m_Font);
                          glPopMatrix ();

                        //} // end if ((sideL <= 0.0) ...
                        } // end if (sqrt (xPos * xPos ...
                      } // end for (fixIt = ...
                    } // end if (*nav_shows_fix

                    // only display Navaids with a map range <= 40 nm
                    if ((*nav_shows_ndb == 1) || (*nav_shows_vor == 1)) {

                      //----------Navaids-----------
                      GeographicObjectList::iterator navIt;
                      GeographicHash* pNavHash = m_pNavDatabase->GetNavaidHash ();
                      std::list<OpenGC::GeographicObject*>* pNavList = pNavHash->GetListAtLatLon (acLat2, acLon2);

                      for (navIt = pNavList->begin (); navIt != pNavList->end (); ++navIt) {

                        lon = (*navIt)->GetDegreeLon ();
                        lat = (*navIt)->GetDegreeLat ();

                        // convert to azimuthal equidistant coordinates with acf in center
                        lonlat2gnomonic (&lon, &lat, &easting, &northing, &acLon, &acLat);

                        // Compute physical position relative to acf center on screen
                        yPos = -northing / 1852.0 / mapRange * mapCircle;
                        xPos = easting / 1852.0 / mapRange * mapCircle;

                        // Only draw the nav if it's visible within the rendering area
                        if (sqrt (xPos * xPos + yPos * yPos) < mapCircle) {

                          // calculate physical position on heading rotated nav map
                          float rotateRad = -1.0 * hdgTrue * dtor;

                          xPosR = cos (rotateRad) * xPos + sin (rotateRad) * yPos;
                          yPosR = -sin (rotateRad) * xPos + cos (rotateRad) * yPos;

                          sideL = (xPosR + 0.95 * mapRange) * (-0.25 * mapRange - 0.75 * mapRange) -
                              (yPosR - 0.75 * mapRange) * (-0.15 * mapRange + 0.75 * mapRange);
                          sideR = (xPosR - 0.95 * mapRange) * (-0.25 * mapRange - 0.75 * mapRange) -
                              (yPosR - 0.75 * mapRange) * (0.15 * mapRange - 0.75 * mapRange);

                          // Only draw the nav if it is in the upper section of the screen
                          //  if ((sideL <= 0.0) && (sideR >= 0.0)) {

                            int type = (*navIt)->GetNavaidType ();

                            if (((*nav_shows_ndb == 1) &&  (type == 2)) ||
                                ((*nav_shows_vor == 1) && ((type == 3)  || (type == 12) || (type == 13)))) {

                              // We're going to be clever and "derotate" each label
                              // by the heading, so that everything appears upright despite
                              // the global rotation. This makes all of the labels appear at
                              // 0,0 in the local coordinate system
                              glPushMatrix ();

                      //        if (ndPage != 4) { // NOT on PLAN page!
                                  glTranslatef (xPos, yPos, 0.0);
                                  glRotatef (-1.0 * hdgTrue, 0, 0, 1);
                      //        }

                              glColor3ub (COLOR_MAGENTA);
                              glLineWidth (2.0);
                              if (type == 2) {
                                // NDB: magenta triangle
                                glBegin (GL_LINE_LOOP);
                                  glVertex2f ( -5, -5);
                                  glVertex2f (  0,  5);
                                  glVertex2f (  5, -5);
                                glEnd ();
                              } else if (type == 3) {
                                // VOR: magenta hexagon (should be: + sign)
                                float ss3 = 0.65 * symbolSize;
                                glBegin (GL_LINE_LOOP);
                                  glVertex2f (-0.5 * ss3, -0.866 * ss3);
                                  glVertex2f (0.5 * ss3, -0.866 * ss3);
                                  glVertex2f (ss3, 0.0);
                                  glVertex2f (0.5 * ss3, 0.866 * ss3);
                                  glVertex2f (-0.5 * ss3, 0.866 * ss3);
                                  glVertex2f (-ss3, 0.0);
                                glEnd ();
/*
                                // VOR: magenta + sign
                                glBegin (GL_LINES);
                                  glVertex2f (-8,  0); glVertex2f ( 8, 0);
                                  glVertex2f ( 0, -8); glVertex2f ( 0, 8);
                                glEnd ();
*/
                              } else {
                              // DME or DME part of VOR/VORTAC: magenta hexagon with boxes (should be: circle, with outside lines)
                                float ss4 = 0.65 * symbolSize;
                                glBegin (GL_LINE_LOOP);
                                  glVertex2f (-0.5 * ss4, -0.866 * ss4);
                                  glVertex2f (0.5 * ss4, -0.866 * ss4);
                                  glVertex2f (ss4, 0.0);
                                  glVertex2f (0.5 * ss4, 0.866 * ss4);
                                  glVertex2f (-0.5 * ss4, 0.866 * ss4);
                                  glVertex2f (-ss4, 0.0);
                                glEnd ();
                                glBegin (GL_LINE_STRIP);
                                  glVertex2f (-0.5 * ss4, -0.866 * ss4);
                                  glVertex2f (-0.5 * ss4, -1.732 * ss4);
                                  glVertex2f (0.5 * ss4, -1.732 * ss4);
                                glVertex2f (0.5 * ss4, -0.866 * ss4);
                                glEnd ();
                                glBegin (GL_LINE_STRIP);
                                  glVertex2f (ss4, 0.0);
                                  glVertex2f (1.75 * ss4, 0.433 * ss4);
                                  glVertex2f (1.25 * ss4, 1.366 * ss4);
                                  glVertex2f (0.5 * ss4, 0.866 * ss4);
                                glEnd ();
                                glBegin (GL_LINE_STRIP);
                                  glVertex2f (-0.5 * ss4, 0.866 * ss4);
                                  glVertex2f (-1.25 * ss4, 1.366 * ss4);
                                  glVertex2f (-1.75 * ss4, 0.433 * ss4);
                                  glVertex2f (-ss4, 0.0);
                                glEnd ();
/*
                                // DME or DME part of VOR/VORTAC: magenta circle, with outside lines
                                aCircle.SetRadius (6);
                                aCircle.SetOrigin (0.0, 0.0);
                                glBegin (GL_LINE_LOOP);
                                  aCircle.Evaluate ();
                                glEnd ();
                                glBegin (GL_LINES);
                                  glVertex2f (-10,  0); glVertex2f (-6,   0);
                                  glVertex2f (  6,  0); glVertex2f (10,   0);
                                  glVertex2f (  0, 10); glVertex2f ( 0,   6);
                                  glVertex2f (  0, -6); glVertex2f ( 0, -10);
                                glEnd ();
*/
                              }

                              m_pFontManager->SetSize (m_Font, 8.5, 8.5); // <= (m_Font, 0.65*fontSize, 0.65*fontSize);
                              m_pFontManager->Print (symbolSize * 1.25, -symbolSize * 1.75, (*navIt)->GetIdentification ().c_str(), m_Font);
                            glPopMatrix ();
                          } // endif (((type == 2) ...
                        //  } // end  if ((sideL <= 0.0) ...
                      } // end if (sqrt (xPos * xPos ...
                    } // end for (navIt = ...
                  } // end if ((*nav_shows_ndb ...
                } // end if (ndRange < 3) ...

                //----------Airports-----------
                  
                if (*nav_shows_apt == 1) {
                  GeographicObjectList::iterator aptIt;
                  GeographicHash* pAptHash = m_pNavDatabase->GetAirportHash ();
                  std::list<OpenGC::GeographicObject*>* pAptList = pAptHash->GetListAtLatLon (acLat2, acLon2);
                    
                  for (aptIt = pAptList->begin (); aptIt != pAptList->end (); ++aptIt) {
                    
                    lon = (*aptIt)->GetDegreeLon ();
                    lat = (*aptIt)->GetDegreeLat ();

                    // convert to azimuthal equidistant coordinates with acf in center
                    lonlat2gnomonic (&lon, &lat, &easting, &northing, &acLon, &acLat);

                    // Compute physical position relative to acf center on screen
                    yPos = -northing / 1852.0 / mapRange * mapCircle;
                    xPos = easting / 1852.0 / mapRange * mapCircle;
/*
printf ("APT: %s lon %f lat %f northing %f, easting %f, mapRange %d, xPos %f, yPos %f\n", 
(*aptIt)->GetIdentification ().c_str (), lon, lat, northing, easting, mapRange, xPos, yPos);
/**/
                    // Only draw the airport if it's visible within the rendering area
                    if (sqrt (xPos*xPos + yPos*yPos) < mapCircle) {

                      // calculate physical position on heading rotated nav map
                      float rotateRad = -1.0 * hdgTrue * dtor;

                      xPosR = cos (rotateRad) * xPos + sin (rotateRad) * yPos;
                      yPosR = -sin (rotateRad) * xPos + cos (rotateRad) * yPos;

                      sideL = (xPosR + 0.95 * mapRange) * (-0.25 * mapRange - 0.75 * mapRange) -
                          (yPosR - 0.75 * mapRange) * (-0.15 * mapRange + 0.75 * mapRange);
                      sideR = (xPosR - 0.95 * mapRange) * (-0.25 * mapRange - 0.75 * mapRange) -
                          (yPosR - 0.75 * mapRange) * (0.15 * mapRange - 0.75 * mapRange);

                      // We're going to be clever and "derotate" each label
                      // by the heading, so that everything appears upright despite
                      // the global rotation. This makes all of the labels appear at
                      // 0,0 in the local coordinate system
                      glPushMatrix ();

                      glTranslatef (xPos, yPos, 0.0);
                      glRotatef (-1.0 * hdgTrue, 0, 0, 1);

                      // APT: magenta star
                      glColor3ub (COLOR_MAGENTA);
                      glLineWidth (2.0);
                      glBegin (GL_LINES);
                        glVertex2d ( 0, -8); glVertex2d ( 0,  8);
                        glVertex2d (-8, -0); glVertex2d ( 8,  0);
                        glVertex2d (-6, -6); glVertex2d ( 6,  6);
                        glVertex2d (-6,  6); glVertex2d ( 6, -6);
                      glEnd ();

                      m_pFontManager->SetSize (m_Font, 10, 10); // <= (m_Font, 0.65*fontSize, 0.65*fontSize);
                      m_pFontManager->Print (8, 6, (*aptIt)->GetIdentification ().c_str(), m_Font);
                      
                      glPopMatrix ();

                    } // end if (sqrt (xPos*xPos ...
                  } // end for (aptIt =  ...
                } // end if (*nav_shows_apt
              } // end for (int lo = -nlon; ... (loop through longitude range around acf)
            } // end for (int la = -nlat; ... (loop through latitude range around acf)
if (0 == 3) {              
              // plot TCAS of AI aircraft (NOTE: tcas_on always == 1 ??)
              for (int i = 0; i < 20; i++) {
                if (((tcasDist + i) > 0.0) && ((tcasHdg + i) != FLT_MISS) && ((tcasAlt + i) != FLT_MISS)) {

                  float distancemeters = (tcasDist + i);
                  float altitudemeters = (tcasAlt + i);
                  float rotateRad = ((tcasHdg + i) + hdgTrue) * dtor;

                  xPos = distancemeters * sin (rotateRad) / 1852.0 / mapRange;
                  yPos = distancemeters * cos (rotateRad) / 1852.0 / mapRange;
//printf ("%i %f %f %f \n",i,distancemeters,(tcasHdg+i),altitudemeters*3.28084/100);

                  glPushMatrix ();

                  glTranslatef (xPos, yPos, 0.0);
                  glRotatef (-1.0 * hdgTrue, 0, 0, 1);
                  glLineWidth (2.0);
                  float ss2 = 0.65 * symbolSize;
                  if (fabs (altitudemeters)*3.28084 < 300.) {
                    // Resolution Advisory (red filled square)
                    glColor3ub (COLOR_RED);
                    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
                    glBegin (GL_POLYGON);
                      glVertex2f (-ss2, -ss2);
                      glVertex2f (ss2, -ss2);
                      glVertex2f (ss2, ss2);
                      glVertex2f (-ss2, ss2);
                    glEnd ();
                  } else if (fabs (altitudemeters)*3.28084 < 900.) {
                    // traffic advisory (yellow filled circle)
                    glColor3ub (COLOR_YELLOW);
                    CircleEvaluator tCircle;
                    tCircle.SetDegreesPerPoint (20);
                    tCircle.SetArcStartEnd (0, 360);
                    tCircle.SetRadius (ss2);
                    tCircle.SetOrigin (0, 0);
                    glBegin (GL_POLYGON);
                      tCircle.Evaluate ();
                    glEnd ();
                  } else {
                    // other: white diamond
                    glColor3ub (COLOR_WHITE);
                    glPolygonMode (GL_FRONT, GL_LINE);
                    glBegin (GL_POLYGON);
                      glVertex2f (0, -ss2);
                      glVertex2f (ss2, 0);
                      glVertex2f (0, ss2);
                      glVertex2f (-ss2, 0);
                    glEnd ();
                    glPolygonMode (GL_FRONT, GL_FILL);
                  }

                  if (fabs (altitudemeters * 3.28084 / 100) < 100) {
                    if (altitudemeters > 0) {
                      m_pFontManager->SetSize (m_Font, 8, 8); // <= ( m_Font, 0.6*fontSize, 0.6*fontSize );
                      sprintf (buff, "+%i", (int) (fabs (altitudemeters)*3.28084 / 100));
              //        m_pFontManager->Print (-0.9*fontSize,ss2+0.05*fontSize, &buff[0], m_Font);
                      m_pFontManager->Print (-0.9 * 13, ss2 + 0.05 * 13, &buff[0], m_Font);
                    } else {
                      m_pFontManager->SetSize (m_Font, 8, 8); // <= ( m_Font, 0.6*fontSize, 0.6*fontSize );
                      sprintf (buff, "-%i", (int) (fabs (altitudemeters)*3.28084 / 100));
              //        m_pFontManager->Print (-0.9*fontSize,-ss2-0.65*fontSize, &buff[0], m_Font);
                      m_pFontManager->Print (-0.9 * 13, -ss2 - 0.65 * 13, &buff[0], m_Font);
                    }
                  }
                  glPopMatrix ();
                } // end if (((tcasDist + i) ...

              } // end for (int i = 0) ... (plot TCAS of AI aircraft)
} // end if (0 == 3) ...              
              // plot TCAS of Multiplayer aircraft...

            } // end if if ((acLon >= -180.0) ... (valid acf coordinates)

            /* end of down-shifted coordinate system */
            glPopMatrix ();

          } // end if (hdgTrue != ...

        // end of environment mapping
        } // end if ((headingValid == 1) &&(mapAvail == 1)) ...

        if (mapAvail == 0) {
          glColor3ub (COLOR_RED);
          m_pFontManager->SetSize (m_Font, 24, 24);
          m_pFontManager->Print (130, 300, "MAP NOT AVAIL", m_Font);
        }

      glPopMatrix ();

      // The track line
      glPushMatrix ();
        glLineWidth (1);
        glColor3ub (COLOR_GREEN);
        glTranslated (250, 250, 0);
//        glRotated (0.0, 0, 0, 1); // ? ever needed ?
        glBegin (GL_LINES);
          glVertex2d (  0,   0); glVertex2d (  0, 160);
        glEnd ();
      glPopMatrix ();

      // The Plane Symbol
      glPushMatrix ();
        glLineWidth (4);
        glColor3ub (COLOR_YELLOW);
        glTranslated (250, 250, 0);
        glBegin (GL_LINES);
          glVertex2d (-25,   0); glVertex2d ( 25 ,  0);
          glVertex2d (-10, -30); glVertex2d ( 10, -30);
          glVertex2d (  0,  15); glVertex2d (  0, -35);
        glEnd ();
      glPopMatrix ();

    } // end if (!coldAndDark )

  } // end of Render() ...

} // end namespace OpenGC

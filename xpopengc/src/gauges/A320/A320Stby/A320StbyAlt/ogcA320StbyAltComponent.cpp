/*=============================================================================

  This is the ogcA320StbyAltComponent.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Standby Altitude Indicator ===

  Created:
    Date:   2018-05-03
    Author: Hans Jansen
    Last change: 2020-04-06

  Copyright (C) 2018-2020 Hans Jansen (hansjansen@users.sourceforge.net)
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

=============================================================================*/

#include <stdio.h>
#include <math.h>

#include "ogcGaugeComponent.h"
#include "ogcA320StbyAlt.h"
#include "ogcA320StbyAltComponent.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320StbyAltComponent::A320StbyAltComponent () {
      if (verbosity > 0) printf ("A320StbyAltComponent - constructing\n");

    m_Font = m_pFontManager->LoadDefaultFont ();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("A320StbyAltComponent - constructed\n");
  }

  A320StbyAltComponent::~A320StbyAltComponent () {}

  void A320StbyAltComponent::Render () {
  
    bool coldAndDark = true;
    CircleEvaluator aCircle;
    float fontSize = 8;
    char buffer[8];

    // Call base class to setup for size and position
    GaugeComponent::Render ();

    // Request the datarefs we want to use

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    // Barometer setting (for ISIS)
    float baroSetting;
//    float *baro_setting = link_dataref_flt("sim/cockpit2/gauges/actuators/barometer_setting_in_hg_pilot",-2);
    float *baro_setting = link_dataref_flt("AirbusFBW/ISIBaroSetting",-2);
    if (*baro_setting != FLT_MISS) baroSetting = *baro_setting; else baroSetting = 0;
    // Current altitude (for ISIS)
    float altCapt;
    float *alt_capt = link_dataref_flt ("sim/cockpit2/gauges/indicators/altitude_ft_pilot", 0);
    if (*alt_capt != FLT_MISS) altCapt = *alt_capt; else altCapt = 0.0;

      if (verbosity > 1) {
        printf ("A320StbyAltComponent - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
        printf ("A320StbyAltComponent -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
        printf ("A320StbyAltComponent -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
        printf ("A320StbyAltComponent -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
      }

    if (!coldAndDark) {

      double partSize = m_PhysicalSize.x;	// defines total component size (just for name simplification) (square gauge!)
      double partCenter = partSize / 2;		// defines component center

      m_pFontManager->SetSize (m_Font, fontSize, fontSize);

      glPushMatrix ();
        // fill the background
        glColor3ub (COLOR_BLACK);

        // draw the frame
        glColor3ub (COLOR_GRAY25);
        glBegin (GL_POLYGON);
          glVertex2f (  0,       35);
          glVertex2f (  0,      135);
          glVertex2f ( 35, partSize);
          glVertex2f (135, partSize);
          glVertex2f (partSize, 135);
          glVertex2f (partSize,  35);
          glVertex2f (135,        0);
          glVertex2f ( 35,        0);
        glEnd();

        // Translate to the center of the component
        glTranslated (partCenter, partCenter, 0);
        glLineWidth (2);

        // draw the rim of the glass
        glPushMatrix();
          glColor3ub (COLOR_BLACK);
          aCircle.SetOrigin (0, 0);
          aCircle.SetDegreesPerPoint (5);
          aCircle.SetArcStartEnd (0.0, 360.0);
          aCircle.SetRadius (75);
          glBegin (GL_TRIANGLE_FAN);
            aCircle.Evaluate ();
          glEnd ();
          glColor3ub (COLOR_GRAY25);
          aCircle.SetRadius (70);
          glBegin (GL_TRIANGLE_FAN);
            aCircle.Evaluate ();
          glEnd ();    
        glPopMatrix ();

        // draw the screws in the frame
        int angles[8] = { 23, 67, 113, 157, 203, 247, 293, 337 };
        for (int i = 0; i < 8; i++) {
          glPushMatrix ();
            glRotatef (angles[i], 0, 0, 1);
            glTranslated (0, 82, 0);
            glColor3ub (COLOR_GRAY25);
            aCircle.SetOrigin (0, 0);
            aCircle.SetDegreesPerPoint (6);
            aCircle.SetArcStartEnd (0.0, 360.0);
            aCircle.SetRadius (6);
            glBegin (GL_TRIANGLE_FAN);
              aCircle.Evaluate ();
            glEnd ();
            glColor3ub (COLOR_BLACK);
            glBegin (GL_LINES);
              glVertex2f (-5.0, 0.0); glVertex2f (5.0, 0.0);
            glEnd ();
          glPopMatrix  ();
        }
      
        // draw the dial ticks
        glColor3ub (COLOR_WHITE);
        glPushMatrix ();
          for (int i = 0; i< 1000; i = i + 20) { // short ticks every 20 ft
            if (i != 500) {
              glBegin (GL_LINES);
                glVertex2f (0.0, 55.0); glVertex2f (0.0, 65.0);
              glEnd ();
            }
            glRotatef (-7.2, 0, 0, 1);
          }
        glPopMatrix ();
        glPushMatrix ();
          for (int i = 0; i< 1000; i = i + 100) { // long ticks every 100 ft
            if (i != 500) {
              glBegin (GL_LINES);
                glVertex2f (0.0, 55.0); glVertex2f (0.0, 70.0);
              glEnd ();
            }
            glRotatef (-36, 0, 0, 1);
          }
        glPopMatrix ();
      
        // draw the dial texts (note: still translated to center of instrument!)
        m_pFontManager->Print (  -9, -20, "ALT", m_Font);
        m_pFontManager->Print (  -3,  43, "0", m_Font);
        m_pFontManager->Print (  26,  34, "1", m_Font);
        m_pFontManager->Print (  43,  11, "2", m_Font);
        m_pFontManager->Print (  43, -18, "3", m_Font);
        m_pFontManager->Print (  26, -42, "4", m_Font);
        m_pFontManager->Print (  -3, -65, "5", m_Font);
        m_pFontManager->Print ( -33, -42, "6", m_Font);
        m_pFontManager->Print ( -49, -18, "7", m_Font);
        m_pFontManager->Print ( -49,  11, "8", m_Font);
        m_pFontManager->Print ( -33,  34, "9", m_Font);

        // draw the barometer setting
        m_pFontManager->SetSize (m_Font, fontSize-2, fontSize-2);
        m_pFontManager->Print (-6, -35, "hPa", m_Font);
        glBegin (GL_LINE_LOOP);
          glVertex2f (-12, -38); glVertex2f (15, -38); glVertex2f (15, -47); glVertex2f (-12, -47);
        glEnd ();
        sprintf (buffer, "%4i", (int) (baroSetting * 1013 / 29.92)); // always display hPa!
        m_pFontManager->Print (-9, -45, buffer, m_Font);

        // draw the numeric altitude display
        glBegin (GL_LINE_LOOP);
          glVertex2f (-20, 10); glVertex2f ( 0, 10); glVertex2f ( 0, 27); glVertex2f (-20, 27);
        glEnd ();
        glBegin (GL_LINE_LOOP);
          glVertex2f (  0, 11); glVertex2f (24, 11); glVertex2f (24, 26); glVertex2f (  0, 26);
        glEnd ();
        m_pFontManager->SetSize (m_Font, fontSize+2, fontSize+2);
        sprintf (buffer, "%02.0f", floor (altCapt / 1000));
        m_pFontManager->Print (-18, 14, buffer, m_Font);
        m_pFontManager->SetSize (m_Font, fontSize, fontSize);
        sprintf (buffer, "%03.0f", ((altCapt / 1000) - (floor (altCapt / 1000))) * 1000);
        m_pFontManager->Print (  2, 15, buffer, m_Font);

        // draw the needle(s)
        glPushMatrix ();
          glRotatef (-altCapt * 3.6 / 100, 0, 0, 1);
          glColor3ub (COLOR_BLACK);
          glBegin(GL_POLYGON);
            glVertex2f(-1,-20);
            glVertex2f(-1,  0);
            glVertex2f( 1,  0);
            glVertex2f( 1,-20);
          glEnd();
          glBegin(GL_POLYGON);
            glVertex2f(-1,  0);
            glVertex2f(-1, 20);
            glVertex2f( 1, 20);
            glVertex2f( 1,  0);
          glEnd();
          glColor3ub (COLOR_WHITE);
          glBegin(GL_POLYGON);
            glVertex2f(-1, 20);
            glVertex2f(-1, 40);
            glVertex2f( 1, 40);
            glVertex2f( 1, 20);
          glEnd();
          glBegin(GL_POLYGON);
            glVertex2f(-2, 40);
            glVertex2f(-2, 50);
            glVertex2f( 0, 60);
            glVertex2f( 2, 50);
            glVertex2f( 2, 40);
          glEnd();
          aCircle.SetDegreesPerPoint (5);
          aCircle.SetArcStartEnd (0.0, 360.0);
          aCircle.SetRadius (7);
          aCircle.SetOrigin (0, 0);
          glBegin (GL_TRIANGLE_FAN);
            aCircle.Evaluate ();
          glEnd ();
          glColor3ub (COLOR_BLACK);
          aCircle.SetRadius (6);
          aCircle.SetOrigin (0, 0);
          glBegin (GL_TRIANGLE_FAN);
            aCircle.Evaluate ();
          glEnd ();
        glPopMatrix ();

      glPopMatrix ();

    } // end if (! coldAndDark)

  } // end Render ()

} // end namespace OpenGC


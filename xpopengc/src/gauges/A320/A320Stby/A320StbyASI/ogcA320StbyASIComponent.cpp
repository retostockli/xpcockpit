/*=============================================================================

  This is the ogcA320StbyASIComponent.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Standby AirSpeed Indicator ===

  Created:
    Date:   2018-05-03
    Author: Hans Jansen
    last change: 2020-04-06

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
#include "ogcA320StbyASI.h"
#include "ogcA320StbyASIComponent.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320StbyASIComponent::A320StbyASIComponent () {
    if (verbosity > 0) printf ("A320StbyASIComponent - constructing\n");

    m_Font = m_pFontManager->LoadDefaultFont ();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("A320StbyASIComponent - constructed\n");
  }

  A320StbyASIComponent::~A320StbyASIComponent () {}

  // calculate the needle rotation angle
    /*
      ias	angle	step
      <50	   0	 0
       60	  10	 1
      200	 150	 1
      350	 270	0.8
      450	 340	0.7
    */
  float convertAirspeed (float speed) {
    float angle = 0.0;
    if (speed > 50) {
      if (speed < 200) angle = speed - 50;
      else if (speed < 350) angle = 150 + ((speed - 200) * 0.8);
      else if (speed <= 450) angle = 270 + ((speed - 350) * 0.7);
      else angle = 340;
    }
    return angle;
  }

  void A320StbyASIComponent::Render () {

    bool coldAndDark = true;
  
    // Call base class to setup for size and position
    GaugeComponent::Render ();

    // For drawing circles
    CircleEvaluator aCircle;
    float fontSize = 8;

      // Request the datarefs we want to use

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    // Current airspeed
    float iasCapt;
    float *ias_capt = link_dataref_flt ("AirbusFBW/IASCapt", -1);
    if (*ias_capt != FLT_MISS) iasCapt = *ias_capt; else iasCapt = 0.0;
    float iasAngle = convertAirspeed (iasCapt);

    if (verbosity > 1) {
      printf ("A320StbyASIComponent - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320StbyASIComponent -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320StbyASIComponent -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320StbyASIComponent -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
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
          glRotatef (-10, 0, 0, 1);
          for (int i = 10; i< 150; i = i + 10) { // 10 - 145 deg: 60 - 195 kts
            glBegin (GL_LINES);
              glVertex2f (0.0, 55.0); glVertex2f (0.0, 70.0);
            glEnd ();
            glRotatef (-5, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2f (0.0, 55.0); glVertex2f (0.0, 65.0);
            glEnd ();
            glRotatef (-5, 0, 0, 1);
          }
          for (int i = 160; i< 190; i = i + 8) { // 150 - 185 deg: 200 - 245 kts
            glBegin (GL_LINES);
              glVertex2f (0.0, 55.0); glVertex2f (0.0, 70.0);
            glEnd ();
            glRotatef (-4, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2f (0.0, 55.0); glVertex2f (0.0, 65.0);
            glEnd ();
            glRotatef (-4, 0, 0, 1);
          }
          for (int i = 190; i< 270; i = i + 8) { // 190 - 265 deg: 250 - 345 kts
            glBegin (GL_LINES);
              glVertex2f (0.0, 55.0); glVertex2f (0.0, 70.0);
            glEnd ();
            glRotatef (-8, 0, 0, 1);
          }
          for (int i = 270; i< 350; i = i + 7) { // 270 - 340 deg: 350 - 450 kts
            glBegin (GL_LINES);
              glVertex2f (0.0, 55.0); glVertex2f (0.0, 70.0);
            glEnd ();
            glRotatef (-7, 0, 0, 1);
          }
        glPopMatrix ();
      
        // draw the dial texts (note: still translated to center of instrument!)
        m_pFontManager->Print (  -8,  15, "IAS", m_Font);
        m_pFontManager->Print (  -5,  43, " 60", m_Font);
        m_pFontManager->Print (  10,  37, " 80", m_Font);
        m_pFontManager->Print (  20,  26, "100", m_Font);
        m_pFontManager->Print (  29,  12, "120", m_Font);
        m_pFontManager->Print (  32,  -4, "140", m_Font);
        m_pFontManager->Print (  29, -20, "160", m_Font);
        m_pFontManager->Print (  22, -34, "180", m_Font);
        m_pFontManager->Print (   8, -46, "200", m_Font);
        m_pFontManager->Print ( -16, -51, "250", m_Font);
        m_pFontManager->Print ( -41, -35, "300", m_Font);
        m_pFontManager->Print ( -52,  -4, "350", m_Font);
        m_pFontManager->Print ( -45,  24, "400", m_Font);
        m_pFontManager->Print ( -24,  40, "450", m_Font);
   
        // draw the needle
        glPushMatrix ();
          glRotatef (-iasAngle, 0, 0, 1);
          glColor3ub (COLOR_BLACK);
          glBegin(GL_POLYGON);
            glVertex2f(-3,-20);
            glVertex2f(-3, 20);
            glVertex2f( 3, 20);
            glVertex2f( 3,-20);
          glEnd();
          aCircle.SetDegreesPerPoint (5);
          aCircle.SetArcStartEnd (0.0, 360.0);
          aCircle.SetRadius (8);
          aCircle.SetOrigin (0, 0);
          glBegin (GL_TRIANGLE_FAN);
            aCircle.Evaluate ();
          glEnd ();
          aCircle.SetRadius (6);
          aCircle.SetOrigin (0, -20);
          glBegin (GL_TRIANGLE_FAN);
            aCircle.Evaluate ();
          glEnd ();
          glColor3ub (COLOR_WHITE);
          glBegin(GL_POLYGON);
            glVertex2f(-3, 20);
            glVertex2f(-3, 50);
            glVertex2f( 0, 60);
            glVertex2f( 3, 50);
            glVertex2f( 3, 20);
          glEnd();
        glPopMatrix ();
    
      glPopMatrix ();

    } // end if (! coldAndDark)

  } // end Render ()

} // end namespace OpenGC


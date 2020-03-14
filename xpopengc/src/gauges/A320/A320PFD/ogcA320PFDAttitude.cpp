/*=============================================================================

  This is the ogcA320PFDAttitude.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Primary Flight Display: the Attitude Indicator ===

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
	AirbusFBW/CaptATTValid					int
	sim/flightmodel/misc/h_ind				float
	sim/flightmodel/position/theta				float
	sim/flightmodel/position/phi				float
	sim/cockpit2/autopilot/flight_director_pitch_deg	float
	sim/cockpit2/autopilot/flight_director_roll_deg		float

  Note that this subgauge is incomplete! The QPAC aircraft panel does
	not have any data on this part; so we still have to determine
	things like the following:
	Altitude_AGL_Feet ?
	Flight_Director_Engaged ?
	Nav1_Localizer_Needle ?
	Nav1_Valid ?
  ... but for the time being, it is usable!

=============================================================================*/

#include "ogcGaugeComponent.h"
#include "ogcA320PFD.h"
#include "ogcA320PFDAttitude.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320PFDAttitude::A320PFDAttitude () {
    if (verbosity > 0) printf ("A320PFDAttitude - constructing\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("A320PFDAttitude - constructed\n");
  }

  A320PFDAttitude::~A320PFDAttitude () {}

  float alt, pitch, roll, apPitch, apRoll;

  void A320PFDAttitude::Render() {

    bool coldDarkPfd = true;
    CircleEvaluator aCircle;

    GaugeComponent::Render();

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_dark_pfd = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_dark_pfd == INT_MISS) coldDarkPfd = true; else coldDarkPfd = (*cold_dark_pfd != 0) ? true : false;

    // Attitude indicator valid?
    int* attValid = link_dataref_int ("AirbusFBW/CaptATTValid");

    // current altitude (feet)
    float *pressAltFeet = link_dataref_flt ("sim/flightmodel/misc/h_ind", 0);
    if (*pressAltFeet != FLT_MISS) alt = (long) *pressAltFeet; else alt = 0;

    // current pitch
    float *cur_pitch = link_dataref_flt ("sim/flightmodel/position/theta", 0);
    if (*cur_pitch != FLT_MISS) pitch = *cur_pitch; else pitch = 0;

    // current bank
    float *cur_roll = link_dataref_flt ("sim/flightmodel/position/phi", 0);
    if (*cur_roll != FLT_MISS) roll = *cur_roll; else roll = 0;

    // autopilot-commanded pitch
    float *ap_pitch = link_dataref_flt ("sim/cockpit2/autopilot/flight_director_pitch_deg", 0);
    if (*ap_pitch != FLT_MISS) apPitch = (int) *ap_pitch; else apPitch = 0;

    // autopilot-commanded roll
    float *ap_roll = link_dataref_flt ("sim/cockpit2/autopilot/flight_director_roll_deg", 0);
    if (*ap_roll != FLT_MISS) apRoll = (int) *ap_roll; else apRoll = 0;

    if (verbosity > 2) {
      printf ("A320PFDAttitude - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320PFDAttitude -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320PFDAttitude -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320PFDAttitude -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

      if (!coldDarkPfd) {

      double halfWidth = m_PhysicalSize.x * 0.5;
      double halfHeight = m_PhysicalSize.y * 0.5;

      double fontWidth = 10;
      double fontHeight = 10;

      if (*attValid != 1) {
        m_pFontManager->SetSize (m_Font, 2 * fontWidth, 2 * fontHeight);
        glColor3ub (COLOR_RED);
        m_pFontManager->Print (halfWidth, halfHeight, "ATT", m_Font);
      } else {
        // Translate to the center of the component
        glTranslated (halfWidth, halfHeight, 0);

        glPushMatrix ();
          // Rotate based on the bank
          glRotated (roll, 0, 0, 1);

          // Translate in the direction of the rotation based
          // on the pitch. On the A320, a pitch of 1 degree = 4 mm
          glTranslated (0, pitch * -4.0, 0);

          //-------------------Gauge Background------------------
          // It's drawn oversize to allow for pitch and bank (was 300 px each, now 100 for testing)

          // The "ground" rectangle
          // Remember, the coordinate system is now centered in the gauge component
          glColor3ub (COLOR_AIRBUSGROUND);
          glBegin (GL_POLYGON);
            glVertex2f (-200, -200);
            glVertex2f (-200, 0);
            glVertex2f (200, 0);
            glVertex2f (200, -200);
          glEnd ();

          // The "sky" rectangle
          // Remember, the coordinate system is now centered in the gauge component
          glColor3ub (COLOR_AIRBUSSKY);
          glBegin (GL_POLYGON);
            glVertex2f (-200, 0);
            glVertex2f (-200, 200);
            glVertex2f (200, 200);
            glVertex2f (200, 0);
          glEnd ();

          //-----------------The pitch markings------------------

          // Draw in white
          glColor3ub (COLOR_WHITE);
          // Specify line width
          glLineWidth (1.0);
          // The size for all pitch text
          m_pFontManager->SetSize (m_Font, fontWidth, fontHeight);

          glBegin (GL_LINES);
            // The dividing line between sky and ground
            glVertex2f (-100, 0);
            glVertex2f (100, 0);

            // +2.5 degrees
            glVertex2f (-5, 10);
            glVertex2f (5, 10);

            // +5.0 degrees
            glVertex2f (-10, 20);
            glVertex2f (10, 20);

            // +7.5 degrees
            glVertex2f (-5, 30);
            glVertex2f (5, 30);

            // +10.0 degrees
            glVertex2f (-20, 40);
            glVertex2f (20, 40);

            // +12.5 degrees
            glVertex2f (-5, 50);
            glVertex2f (5, 50);

            // +15.0 degrees
            glVertex2f (-10, 60);
            glVertex2f (10, 60);

            // +17.5 degrees
            glVertex2f (-5, 70);
            glVertex2f (5, 70);

            // +20.0 degrees
            glVertex2f (-20, 80);
            glVertex2f (20, 80);

            // +22.5 degrees
            glVertex2f (-5, 90);
            glVertex2f (5, 90);

            // +25.0 degrees
            glVertex2f (-10, 100);
            glVertex2f (10, 100);

            // +27.5 degrees
            glVertex2f (-5, 110);
            glVertex2f (5, 110);

            // +30.0 degrees
            glVertex2f (-20, 120);
            glVertex2f (20, 120);

            glColor3ub (COLOR_GREEN);

            glVertex2f (-20, 122);
            glVertex2f (-10, 122);
            glVertex2f (-20, 118);
            glVertex2f (-10, 118);
            glVertex2f (20, 122);
            glVertex2f (10, 122);
            glVertex2f (20, 118);
            glVertex2f (10, 118);

            glColor3ub (COLOR_WHITE);

            // -2.5 degrees
            glVertex2f (-5, -10);
            glVertex2f (5, -10);

            // -5.0 degrees
            glVertex2f (-10, -20);
            glVertex2f (10, -20);

            // -7.5 degrees
            glVertex2f (-5, -30);
            glVertex2f (5, -30);

            // -10.0 degrees
            glVertex2f (-20, -40);
            glVertex2f (20, -40);

            // -12.5 degrees
            glVertex2f (-5, -50);
            glVertex2f (5, -50);

            // -15.0 degrees
            glVertex2f (-10, -60);
            glVertex2f (10, -60);

            glColor3ub (COLOR_GREEN);

            glVertex2f (-20, -62);
            glVertex2f (-10, -62);
            glVertex2f (-20, -58);
            glVertex2f (-10, -58);
            glVertex2f (20, -62);
            glVertex2f (10, -62);
            glVertex2f (20, -58);
            glVertex2f (10, -58);

            glColor3ub (COLOR_WHITE);

            // -17.5 degrees
            glVertex2f (-5, -70);
            glVertex2f (5, -70);

            // -20.0 degrees
            glVertex2f (-20, -80);
            glVertex2f (20, -80);

            // -22.5 degrees
            glVertex2f (-5, -90);
            glVertex2f (5, -90);

            // -25.0 degrees
            glVertex2f (-10, -100);
            glVertex2f (10, -100);

            // -27.5 degrees
            glVertex2f (-5, -110);
            glVertex2f (5, -110);

            // -30.0 degrees
            glVertex2f (-20, -120);
            glVertex2f (20, -120);
          glEnd ();

          // +10
          m_pFontManager->Print (-40, 36.0, "10", m_Font);
          m_pFontManager->Print (23.0, 36.0, "10", m_Font);

          // -10
          m_pFontManager->Print (-40, -44.0, "10", m_Font);
          m_pFontManager->Print (23.0, -44.0, "10", m_Font);

          // +20
          m_pFontManager->Print (-40, 76.0, "20", m_Font);
          m_pFontManager->Print (23.0, 76.0, "20", m_Font);

          // -20
          m_pFontManager->Print (-40, -84.0, "20", m_Font);
          m_pFontManager->Print (23.0, -84.0, "20", m_Font);

        glPopMatrix ();

        //-----The mask around the attitude ball-------

        glPushMatrix ();
          // Draw in black
          glColor3ub (COLOR_BLACK);

          // top half
          aCircle.SetOrigin (0, 0);
          aCircle.SetRadius (124);
          aCircle.SetDegreesPerPoint (5);

          aCircle.SetArcStartEnd (300.0, 360.0);
          glBegin (GL_TRIANGLE_FAN);
            glVertex2f (-halfWidth, halfHeight);
            glVertex2f (-halfWidth, halfHeight-65);
            aCircle.Evaluate ();
            glVertex2f (halfWidth, halfHeight);
          glEnd ();

          aCircle.SetArcStartEnd (0.0, 60.0);
          glBegin (GL_TRIANGLE_FAN);
            glVertex2f (halfWidth, halfHeight);
            glVertex2f (halfWidth, halfHeight);
            aCircle.Evaluate ();
            glVertex2f (halfWidth, halfHeight-65);
          glEnd ();

          // bottom half
          aCircle.SetArcStartEnd (120.0, 180.0);
          glBegin (GL_TRIANGLE_FAN);
            glVertex2f (halfWidth, -halfHeight);
            glVertex2f (halfWidth, -halfHeight+65);
            aCircle.Evaluate ();
            glVertex2f (halfWidth, -halfHeight);
          glEnd ();

          aCircle.SetArcStartEnd (180.0, 240.0);
          glBegin (GL_TRIANGLE_FAN);
            glVertex2f (-halfWidth, -halfHeight);
            glVertex2f (halfWidth, -halfHeight);
            aCircle.Evaluate ();
            glVertex2f (-halfWidth, -halfHeight+65);
          glEnd ();

          // left part
          glBegin (GL_POLYGON);
            glVertex2f (-halfWidth, -halfHeight);
            glVertex2f (-halfWidth, halfHeight);
            glVertex2f (-halfWidth + 15, halfHeight);
            glVertex2f (-halfWidth + 15, -halfHeight);
          glEnd ();

          // right part
          glBegin (GL_POLYGON);
            glVertex2f (halfWidth, -halfHeight);
            glVertex2f (halfWidth, halfHeight);
            glVertex2f (halfWidth - 15, halfHeight);
            glVertex2f (halfWidth - 15, -halfHeight);
          glEnd ();

        glPopMatrix ();

        // Finished the ball; now the fixed markings

        //----------------The bank angle markings----------------

        glPushMatrix ();
          // Draw in yellow
          glColor3ub (COLOR_YELLOW);
          glLineWidth (2.0);

          // Draw the center detent
          glBegin (GL_LINE_LOOP);
            glVertex2f (00, 125);
            glVertex2f (-8, 137);
            glVertex2f (8, 137);
          glEnd ();

          // Left side bank markings

          // Draw in white
          glColor3ub (COLOR_WHITE);

          glRotated (10.0, 0, 0, 1);
          glBegin (GL_LINE_STRIP);
            glVertex2f (-2, 125);
            glVertex2f (-2, 135);
            glVertex2f (2, 135);
            glVertex2f (2, 125);
          glEnd ();

          glRotated (10.0, 0, 0, 1);
          glBegin (GL_LINE_STRIP);
            glVertex2f (-2, 125);
            glVertex2f (-2, 135);
            glVertex2f (2, 135);
            glVertex2f (2, 125);
          glEnd ();

          glRotated (10.0, 0, 0, 1);
          glBegin (GL_LINE_STRIP);
            glVertex2f (-2, 125);
            glVertex2f (-2, 145);
            glVertex2f (2, 145);
            glVertex2f (2, 125);
          glEnd ();

          glRotated (15.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f (0, 125);
            glVertex2f (0, 135);
          glEnd ();

          glColor3ub (COLOR_GREEN);
          glRotated (21.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f (-2, 115);
            glVertex2f (-2, 125);
            glVertex2f (2, 116);
            glVertex2f (2, 126);
          glEnd ();
        glPopMatrix ();

        // Right side bank markings
        glColor3ub (COLOR_WHITE);

        glPushMatrix ();
          glRotated (-10.0, 0, 0, 1);
          glBegin (GL_LINE_STRIP);
            glVertex2f (-2, 125);
            glVertex2f (-2, 135);
            glVertex2f (2, 135);
            glVertex2f (2, 125);
          glEnd ();
  
          glRotated (-10.0, 0, 0, 1);
          glBegin (GL_LINE_STRIP);
            glVertex2f (-2, 125);
            glVertex2f (-2, 135);
            glVertex2f (2, 135);
            glVertex2f (2, 125);
          glEnd ();

          glRotated (-10.0, 0, 0, 1);
          glBegin (GL_LINE_STRIP);
            glVertex2f (-2, 125);
            glVertex2f (-2, 145);
            glVertex2f (2, 145);
            glVertex2f (2, 125);
          glEnd ();

          glRotated (-15.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f (0, 125);
            glVertex2f (0, 135);
          glEnd ();

          glColor3ub (COLOR_GREEN);
          glRotated (-21.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f (-2, 116);
            glVertex2f (-2, 126);
            glVertex2f (2, 115);
            glVertex2f (2, 125);
          glEnd ();
        glPopMatrix ();

        //----------------End Bank Markings----------------

        //----------------Bank Indicator----------------

        glPushMatrix ();
          // Rotate based on the bank
          glRotated (roll, 0, 0, 1);

          // Draw in yellow
          glColor3ub (COLOR_YELLOW);
          // Specify line width
          glLineWidth (2.0);

          glBegin (GL_LINE_LOOP); // the bank indicator
            glVertex2f (-8, 112);
            glVertex2f (0, 124);
            glVertex2f (8, 112);
          glEnd ();

          glBegin (GL_LINE_LOOP); // the slip indicator
            glVertex2f (-8, 111);
            glVertex2f (-10, 105);
            glVertex2f (10, 105);
            glVertex2f (8, 111);
          glEnd ();
        glPopMatrix ();

        //--------------End bank indicator------------

        //----------------Flight Director----------------
//        if (m_pDataSource->GetAirframe ()->GetDirector_Engaged ())
        {
          glPushMatrix ();

            // Draw in green
            glColor3ub (COLOR_GREEN);
            glLineWidth (3.0);
    
            glPushMatrix ();
              glTranslated (0, apPitch * 4.0, 0);
              glBegin (GL_LINES);
                glVertex2f (-40, 0);
                glVertex2f (40, 0);
              glEnd ();
            glPopMatrix ();
    
            glPushMatrix ();
              glTranslated (apRoll - roll, 0, 0);
              glTranslated (apRoll, 0, 0);
              glBegin (GL_LINES);
                glVertex2f (0, -40);
                glVertex2f (0, 40);
              glEnd ();
            glPopMatrix ();

          glPopMatrix ();
        }

/*
        // Draw the runway indicator only if the radio altitude is 
        // less than 30 (?? hj) feet and the localizer is alive
        // NOTE: this is incomplete... this was copied from the 737/777 PFD // hj
//        if ((m_pDataSource->GetAirframe ()->GetNav1_Valid ()) && (m_pDataSource->GetAirframe ()->GetAltitude_AGL_Feet ()<30))
        {
          glPushMatrix ();
            glColor3ub (COLOR_GREEN);
            glLineWidth (2.4);
            float val = m_pDataSource->GetAirframe ()->GetPressure_Alt_Feet ();
            if (val > 400) val = 400;
//            printf ("Pressure_Alt_Feet %f\n",val);
            float tx = m_pDataSource->GetAirframe ()->GetNav1_Localizer_Needle () * 36;
            glTranslated (tx, 0, 0);
            glBegin (GL_LINE_LOOP);
              glVertex2f (0, -12);
              glVertex2f (-3, -15);
              glVertex2f (-3, -35);
              glVertex2f (3, -35);
              glVertex2f (3, -15);
            glEnd ();
          glPopMatrix ();
        }
/**/

        //----------------Attitude Indicator----------------

        glPushMatrix ();
          // The center axis indicator
          // Black background
          glColor3ub (COLOR_BLACK);
          glBegin (GL_POLYGON);
            glVertex2f (3, 3);
            glVertex2f (3, -3);
            glVertex2f (-3, -3);
            glVertex2f (-3, 3);
          glEnd ();
          // Yellow lines
          glColor3ub (COLOR_YELLOW);
          glLineWidth (2.0);
          glBegin (GL_LINE_LOOP);
            glVertex2f (3, 3);
            glVertex2f (3, -3);
            glVertex2f (-3, -3);
            glVertex2f (-3, 3);
          glEnd ();

          // The left part
          // Black background
          glColor3ub (COLOR_BLACK);
          glBegin (GL_POLYGON);
            glVertex2f (-104, 3);
            glVertex2f (-46, 3);
            glVertex2f (-46, -3);
            glVertex2f (-104, -3);
          glEnd ();
          glBegin (GL_POLYGON);
            glVertex2f (-46, 3);
            glVertex2f (-46, -12);
            glVertex2f (-52, -12);
            glVertex2f (-52, 3);
          glEnd ();
          // Yellow lines
          glColor3ub (COLOR_YELLOW);
          glLineWidth (2.0);
          glBegin (GL_LINE_LOOP);
            glVertex2f (-104, 3);
            glVertex2f (-46, 3);
            glVertex2f (-46, -12);
            glVertex2f (-52, -12);
            glVertex2f (-52, -3);
            glVertex2f (-104, -3);
          glEnd ();

          // The right part
          // Black background
          glColor3ub (COLOR_BLACK);
          glBegin (GL_POLYGON);
            glVertex2f (104, 3);
            glVertex2f (46, 3);
            glVertex2f (46, -3);
            glVertex2f (104, -3);
          glEnd ();
          glBegin (GL_POLYGON);
            glVertex2f (46, 3);
            glVertex2f (46, -12);
            glVertex2f (52, -12);
            glVertex2f (52, 3);
          glEnd ();
          // Yellow lines
          glColor3ub (COLOR_YELLOW);
          glLineWidth (2.0);
          glBegin (GL_LINE_LOOP);
            glVertex2f (104, 3);
            glVertex2f (46, 3);
            glVertex2f (46, -12);
            glVertex2f (52, -12);
            glVertex2f (52, -3);
            glVertex2f (104, -3);
          glEnd ();
        glPopMatrix ();

      } // end if (*attValid ...)

    } // end if ((!coldDarkPfd)

  } // end Render()

} // end namespace OpenGC

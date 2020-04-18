/*=============================================================================

  This is the ogcA320StbyAttComponent.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Main-Panel Clock ===

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

===========================================================================*/

#include <stdio.h>
#include <math.h>

#include "ogcGaugeComponent.h"
#include "ogcA320StbyAtt.h"
#include "ogcA320StbyAttComponent.h"
#include "ogcCircleEvaluator.h"

#define COLOR_ISISGROUND 194, 146, 44
#define COLOR_ISISSKY 27, 109, 158

namespace OpenGC {

  A320StbyAttComponent::A320StbyAttComponent () {
      if (verbosity > 0) printf ("A320StbyAttComponent - constructing\n");

    m_Font = m_pFontManager->LoadDefaultFont ();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("A320StbyAttComponent - constructed\n");
  }

  A320StbyAttComponent::~A320StbyAttComponent () {}

  void A320StbyAttComponent::Render () {

    bool coldDarkAtt = true;
    CircleEvaluator aCircle;
    float fontSize = 12;

    float ias, alt, pitch, roll;

    // Call base class to setup for size and position
    GaugeComponent::Render ();

      // Request the datarefs we want to use

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldDarkAtt = true; else coldDarkAtt = (*cold_and_dark != 0) ? true : false;

    // Current airspeed = sim/cockpit2/gauges/indicators/airspeed_kts_pilot (AirbusFBW/IASCapt)
    // note: only needed for ISIS!
//    float *ias_capt = link_dataref_flt ("sim/cockpit2/gauges/indicators/airspeed_kts_pilot", -1);
//    if (*ias_capt != FLT_MISS) ias = *ias_capt; else ias = 0;
//ias = 0; // during development!

    // current altitude (feet) = sim/cockpit2/gauges/indicators/altitude_ft_pilot (im/flightmodel/misc/h_ind)
    // note: only needed for ISIS!
//    float *pressAltFeet = link_dataref_flt ("sim/cockpit2/gauges/indicators/altitude_ft_pilot", 0);
//    if (*pressAltFeet != FLT_MISS) alt = (long) *pressAltFeet; else alt = 0;
//alt = 0; // during development!

    // current pitch = sim/cockpit2/gauges/indicators/pitch_electric_deg_pilot (sim/flightmodel/position/theta)
    float *cur_pitch = link_dataref_flt ("sim/cockpit2/gauges/indicators/pitch_electric_deg_pilot", 0);
    if (*cur_pitch != FLT_MISS) pitch = *cur_pitch; else pitch = 0;

    // current bank = sim/cockpit2/gauges/indicators/roll_electric_deg_pilot (sim/flightmodel/position/phi)
    float *cur_roll = link_dataref_flt ("sim/cockpit2/gauges/indicators/roll_electric_deg_pilot", 0);
    if (*cur_roll != FLT_MISS) roll = *cur_roll; else roll = 0;

    if (!coldDarkAtt) {

      if (verbosity > 1) {
        printf ("A320StbyAttComponent - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
        printf ("A320StbyAttComponent -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
        printf ("A320StbyAttComponent -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
        printf ("A320StbyAttComponent -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
      }

      double partSize = m_PhysicalSize.x;	// defines total component size (just for name simplification) (square gauge!)
      double halfSize = partSize / 2;		// defines component center

      double fontWidth = 10;
      double fontHeight = 10;

      m_pFontManager->SetSize (m_Font, fontSize, fontSize);

      glPushMatrix ();
        // fill the background
        glColor3ub (COLOR_BLACK);

        // Translate to the center of the component
        glTranslated (halfSize, halfSize, 0);

        //-------------Gauge Background (the "ball")-----------
        // It's drawn oversize to allow for pitch and bank
        // Remember, the coordinate system is now centered in the gauge component

        glPushMatrix ();
          // Rotate based on the bank
          glRotated (roll, 0, 0, 1);

          // Translate in the direction of the rotation based on the pitch.
          // On the A320, a pitch of 1 degree = 4 mm
          glTranslated (0, pitch * -4.0, 0);

          // The "ground" rectangle
          glColor3ub (COLOR_ISISGROUND);
          glBegin (GL_POLYGON);
            glVertex2f (-90, -165);
            glVertex2f (-90, 0);
            glVertex2f (90, 0);
            glVertex2f (90, -165);
          glEnd ();

          // The "sky" rectangle
          glColor3ub (COLOR_ISISSKY);
          glBegin (GL_POLYGON);
            glVertex2f (-90, 0);
            glVertex2f (-90, 165);
            glVertex2f (90, 165);
            glVertex2f (90, 0);
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
            glVertex2f (-65, 0);
            glVertex2f (65, 0);

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

          m_pFontManager->Print (-40,  36.0, "10", m_Font);
          m_pFontManager->Print (-40, -44.0, "10", m_Font);
          m_pFontManager->Print (-40,  76.0, "20", m_Font);
          m_pFontManager->Print (-40, -84.0, "20", m_Font);

        //----------------The bank angle markings----------------

          glLineWidth (2.0);

          // Draw the center pointer
          glBegin (GL_POLYGON);
            glVertex2f ( 0, 60);
            glVertex2f (-6, 50);
            glVertex2f ( 6, 50);
          glEnd ();

          // Top curve
          glColor3ub (COLOR_WHITE);

          glRotated (-25.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f (-5, 50);
            glVertex2f ( 5, 50);
          glEnd ();
          glRotated (10.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f (-5, 50);
            glVertex2f ( 5, 50);
          glEnd ();
          glRotated (10.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f (-5, 50);
            glVertex2f ( 5, 50);
          glEnd ();
          glRotated (10.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f (-5, 50);
            glVertex2f ( 5, 50);
          glEnd ();
          glRotated (10.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f (-5, 50);
            glVertex2f ( 5, 50);
          glEnd ();
          glRotated (10.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f (-5, 50);
            glVertex2f ( 5, 50);
          glEnd ();
          glRotated (-25.0, 0, 0, 1);

          // Left side bank markings

          glRotated (10.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f ( 0, 50);
            glVertex2f ( 0, 60);
          glEnd ();

          glRotated (10.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f ( 0, 50);
            glVertex2f ( 0, 60);
          glEnd ();

          glRotated (10.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f ( 0, 50);
            glVertex2f ( 0, 60);
          glEnd ();

          glRotated (15.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f ( 0, 50);
            glVertex2f ( 0, 60);
          glEnd ();

          glRotated (15.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f ( 0, 50);
            glVertex2f ( 0, 60);
          glEnd ();
          glRotated (-60.0, 0, 0, 1);

          // Right side bank markings
          glColor3ub (COLOR_WHITE);

          glRotated (-10.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f ( 0, 50);
            glVertex2f ( 0, 60);
          glEnd ();

          glRotated (-10.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f ( 0, 50);
            glVertex2f ( 0, 60);
          glEnd ();

          glRotated (-10.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f ( 0, 50);
            glVertex2f ( 0, 60);
          glEnd ();

          glRotated (-15.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f ( 0, 50);
            glVertex2f ( 0, 60);
          glEnd ();

          glRotated (-15.0, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2f ( 0, 50);
            glVertex2f ( 0, 60);
          glEnd ();
        glPopMatrix ();

        //----------------End Bank Markings----------------

        // Finished the ball; now the fixed markings

        glPushMatrix ();
          // Draw in yellow
          glColor3ub (COLOR_YELLOW);
          glLineWidth (2.0);

          // The center detent
          glBegin (GL_POLYGON);
            glVertex2f ( 0, 58);
            glVertex2f (-6, 65);
            glVertex2f ( 6, 65);
          glEnd ();

        glPopMatrix ();

        //-----The mask around the attitude ball-------

        glPushMatrix ();
          // Draw in black
          glColor3ub (COLOR_BLACK);

          // top part
          glBegin (GL_POLYGON);
            glVertex2f (-halfSize, -halfSize);
            glVertex2f (-halfSize, -halfSize + 45);
            glVertex2f (halfSize, -halfSize + 45);
            glVertex2f (halfSize, -halfSize);
          glEnd ();

          // bottom part
          glBegin (GL_POLYGON);
            glVertex2f (-halfSize, halfSize);
            glVertex2f (-halfSize, halfSize - 45);
            glVertex2f (halfSize, halfSize - 45);
            glVertex2f (halfSize, halfSize);
          glEnd ();

          // left part
          glBegin (GL_POLYGON);
            glVertex2f (-halfSize, -halfSize);
            glVertex2f (-halfSize, halfSize);
            glVertex2f (-halfSize + 45, halfSize);
            glVertex2f (-halfSize + 45, -halfSize);
          glEnd ();

          // right part
          glBegin (GL_POLYGON);
            glVertex2f (halfSize, -halfSize);
            glVertex2f (halfSize, halfSize);
            glVertex2f (halfSize - 45, halfSize);
            glVertex2f (halfSize - 45, -halfSize);
          glEnd ();

        glPopMatrix ();

        //----------------Attitude Indicator----------------

        glPushMatrix ();
          // The center axis indicator
          // Black background
          glColor3ub (COLOR_BLACK);
          glBegin (GL_POLYGON);
            glVertex2f ( 3,  3);
            glVertex2f ( 3, -3);
            glVertex2f (-3, -3);
            glVertex2f (-3,  3);
          glEnd ();
          // Yellow lines
          glColor3ub (COLOR_YELLOW);
          glLineWidth (2.0);
          glBegin (GL_LINE_LOOP);
            glVertex2f ( 3,  3);
            glVertex2f ( 3, -3);
            glVertex2f (-3, -3);
            glVertex2f (-3,  3);
          glEnd ();

          // The left part
          // Black background
          glColor3ub (COLOR_BLACK);
          glBegin (GL_POLYGON);
            glVertex2f (-65,  3);
            glVertex2f (-20,  3);
            glVertex2f (-20, -3);
            glVertex2f (-65, -3);
          glEnd ();
          glBegin (GL_POLYGON);
            glVertex2f (-20,   3);
            glVertex2f (-20, -10);
            glVertex2f (-26, -10);
            glVertex2f (-26,   3);
          glEnd ();
          // Yellow lines
          glColor3ub (COLOR_YELLOW);
          glLineWidth (2.0);
          glBegin (GL_LINE_LOOP);
            glVertex2f (-65,   3);
            glVertex2f (-20,   3);
            glVertex2f (-20, -10);
            glVertex2f (-26, -10);
            glVertex2f (-26,  -3);
            glVertex2f (-65,  -3);
          glEnd ();

          // The right part
          // Black background
          glColor3ub (COLOR_BLACK);
          glBegin (GL_POLYGON);
            glVertex2f (65,  3);
            glVertex2f (20,  3);
            glVertex2f (20, -3);
            glVertex2f (65, -3);
          glEnd ();
          glBegin (GL_POLYGON);
            glVertex2f (20,   3);
            glVertex2f (20, -10);
            glVertex2f (26, -10);
            glVertex2f (26,   3);
          glEnd ();
          // Yellow lines
          glColor3ub (COLOR_YELLOW);
          glLineWidth (2.0);
          glBegin (GL_LINE_LOOP);
            glVertex2f (65,   3);
            glVertex2f (20,   3);
            glVertex2f (20, -10);
            glVertex2f (26, -10);
            glVertex2f (26,  -3);
            glVertex2f (65,  -3);
          glEnd ();
        glPopMatrix ();

        // End of the "ball"

        // Airspeed and altitude indicators can be inserted here for ISIS...

      glPopMatrix ();

    } // end if (! coldDarkAtt)

  } // end Render ()

} // end namespace OpenGC


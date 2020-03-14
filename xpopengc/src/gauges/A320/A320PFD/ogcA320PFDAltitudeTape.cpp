/*=============================================================================

  This is the ogcA320PFDAltitudeTape.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Primary Flight Display: the Altitude Tape ===

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
v	sim/cockpit/autopilot/altitude
v	sim/cockpit2/gauges/indicators/altitude_ft_pilot (not found in the 2D panel...)
v	sim/cockpit2/gauges/actuators/barometer_setting_in_hg_pilot
v	AirbusFBW/ALTCapt_M		float
v	AirbusFBW/AltitudeTargetIsFL	int
v	AirbusFBW/ALTisCstr		int
v	AirbusFBW/ALTPointerPos		float
v	AirbusFBW/BaroStdCapt		int
v	AirbusFBW/BaroUnitCapt		int
v	AirbusFBW/CaptALTValid		int
v	AirbusFBW/ConstraintAlt		float
v	AirbusFBW/HideBaroCapt		int
v	AirbusFBW/MetricAlt		int

=============================================================================*/

#include <stdio.h>
#include <math.h>

#include "ogcGaugeComponent.h"
#include "ogcA320PFD.h"
#include "ogcA320PFDAltitudeTape.h"

namespace OpenGC {

  A320PFDAltitudeTape::A320PFDAltitudeTape () {
    if (verbosity > 0) printf ("A320PFDAltitudeTape - constructing\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("A320PFDAltitudeTape - constructed\n");
  }

  A320PFDAltitudeTape::~A320PFDAltitudeTape () {}

  float altCapt, cstrAlt, altPPos, fcuAlt, fcuAltM, altCaptM, baroSetting;

  void A320PFDAltitudeTape::Render () {

    bool coldDarkPfd = true;

    GaugeComponent::Render();

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_dark_pfd = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_dark_pfd == INT_MISS) coldDarkPfd = true; else coldDarkPfd = (*cold_dark_pfd != 0) ? true : false;

    // current altitude (feet)
    float *alt_capt = link_dataref_flt ("sim/cockpit2/gauges/indicators/altitude_ft_pilot", 0);
    if (*alt_capt != FLT_MISS) altCapt = (long) (*alt_capt + 0.5); else altCapt = 0;

    // Constrained altitude
    float *cstr_alt  = link_dataref_flt ("AirbusFBW/ConstraintAlt", 0);
    if (*cstr_alt != FLT_MISS) cstrAlt = (long) *cstr_alt; else cstrAlt = 0;

    // Altitude indicators valid?
    int *altValid = link_dataref_int ("AirbusFBW/CaptALTValid");

    // Altitude as Flight Level?
    int *altTgtFl = link_dataref_int ("AirbusFBW/AltitudeTargetIsFL");

    // Altitude Managed?
    int *altMgd = link_dataref_int ("AirbusFBW/ALTisCstr");

    // position of altitude pointer on PFD scale
    float *alt_p_pos = link_dataref_flt ("AirbusFBW/ALTPointerPos", 0);
    if (*alt_p_pos != FLT_MISS) altPPos = (long) *alt_p_pos; else altPPos = 0;

    // Autopilot altitude (feet)
    float *fcu_alt = link_dataref_flt ("sim/cockpit/autopilot/altitude",0);
    if (*fcu_alt != FLT_MISS) fcuAlt = (long) *fcu_alt; else fcuAlt = 0;

    // Barometer settings to be shown?
    int *hideBaro = link_dataref_int ("AirbusFBW/HideBaroCapt");

    // Barometer STD indicator?
    int *baroStd = link_dataref_int ("AirbusFBW/BaroStdCapt");

    // Barometer Unit?
    int *baroUnit = link_dataref_int ("AirbusFBW/BaroUnitCapt");

    // Barometer setting?
    float *baro_setting = link_dataref_flt ("sim/cockpit2/gauges/actuators/barometer_setting_in_hg_pilot", -2);
    if (*baro_setting != FLT_MISS) baroSetting = *baro_setting; else baroSetting = 0;

    // Metric altitude indications
    int *metricAlt = link_dataref_int ("AirbusFBW/MetricAlt");

    // Metric FCU altitude
    float *fcu_alt_m = link_dataref_flt ("AirbusFBW/FCUALT_M", 0);
    if (*fcu_alt_m != FLT_MISS) fcuAltM = *fcu_alt_m; else fcuAltM = 0;

    // Metric Captain's altitude
    float *alt_capt_m = link_dataref_flt ("AirbusFBW/ALTCapt_M", 0);
    if (*alt_capt_m != FLT_MISS) altCaptM = *alt_capt_m; else altCaptM = 0;

    if (verbosity > 2) {
      printf ("A320AltitudeTape - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320AltitudeTape -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320AltitudeTape -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320AltitudeTape -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    if (!coldDarkPfd) {

      glPushMatrix ();

        double partWidth = m_PhysicalSize.x;	// defines total component size (just for name simplification)
        double partHeight = m_PhysicalSize.y;
        double indent_x = (m_PhysicalSize.x - 20); // defines region of altitude tape
        double tapeOrigin = 20;
        double tapeHeight = partHeight - 40;
        double tapeTop = tapeHeight + tapeOrigin;

        double fontWidth = 10;
        double fontHeight = 12;
        double fontIndent = 10;
        double refPos = partHeight / 2.0; // vertical position of the center reference line

        // Draw the background rectangle
        glColor3ub (COLOR_AIRBUSGRAY);
        glBegin (GL_POLYGON);
          glVertex2f (20, tapeOrigin);
          glVertex2f (20, tapeTop);
          glVertex2f (indent_x, tapeTop);
          glVertex2f (indent_x, tapeOrigin);
        glEnd();
        // Draw the boundary lines
        glColor3ub (COLOR_GRAY75);
        glLineWidth(2.0);
        glBegin (GL_LINE_STRIP);
          glVertex2f (partWidth, tapeOrigin);
          glVertex2f (20, tapeOrigin);
          glVertex2f (indent_x, tapeOrigin);
          glVertex2f (indent_x, tapeTop);
          glVertex2f (20, tapeTop);
          glVertex2f (partWidth, tapeTop);
        glEnd();

        if (*altValid != 1) {
          m_pFontManager->SetSize (m_Font, 1.5 * fontWidth, 1.5 * fontHeight);
          glColor3ub (COLOR_RED);
          m_pFontManager->Print (2 * fontIndent + 2, refPos, "ALT", m_Font);
        } else {
          // Tick marks are spaced every 100 ft. vertically
          // The tick spacing represents how far apart they are in physical units
          double tickSpacing = (partHeight - 20) / 12;
          double tickWidth = 10;

          m_pFontManager->SetSize (m_Font, fontWidth, fontHeight);

          char buffer[16];
          int nextHigherAlt = ((int) (altCapt / 100)) * 100; // (altCapt+90)/100? then next if statement not needed
          if (nextHigherAlt < altCapt) nextHigherAlt += 100;

          // The vertical offset is how high in physical units the next higher 100's
          // altitude is above the reference line
          float vertOffset = (nextHigherAlt - altCapt) / 100 * tickSpacing;

          double tickLocation = 0;	// Vertical location of the tick mark
          double texty = 0;		// Vertical location of the speed text
          int i = 0;			// counter
          int tickAlt;			// altitude represented by tick mark

          glColor3ub (COLOR_WHITE);
          glLineWidth(2.0);

          // Draw ticks up from the center
          for (i = 0; i <= ((m_PhysicalSize.y-40) / (2 * tickSpacing)); i++) {
            tickAlt = nextHigherAlt + i * 100;
            tickLocation = refPos + i * tickSpacing + vertOffset;
            texty = tickLocation - fontHeight / 2;
            if ( (tickAlt % 500) == 0) {
              glBegin (GL_LINES);
                glVertex2f (indent_x - tickWidth, tickLocation);
                glVertex2f (indent_x, tickLocation);
              glEnd();
              sprintf ( buffer, "%03i", abs(tickAlt / 100) );
              m_pFontManager->Print (fontIndent + 12, texty, &buffer[0], m_Font);
            } else {
              glBegin (GL_LINES);
                glVertex2f (indent_x - tickWidth, tickLocation);
                glVertex2f (indent_x, tickLocation);
              glEnd();
            }
          }

          // Draw ticks down from the center
          for (i = 1; i <= ((m_PhysicalSize.y-40) / (2 * tickSpacing)); i++) {
            tickAlt = nextHigherAlt - i * 100;
            tickLocation = refPos - ( (i-1) * tickSpacing) - (tickSpacing - vertOffset);
            texty = tickLocation - fontHeight / 2;
            if ( (tickAlt % 500) == 0 ) {
              glBegin (GL_LINES);
                glVertex2f (indent_x - tickWidth, tickLocation);
                glVertex2f (indent_x, tickLocation);
              glEnd ();
              sprintf (buffer, "%03i", abs(tickAlt / 100));
              m_pFontManager->Print (fontIndent + 12, texty, buffer, m_Font);
            } else {
              glBegin (GL_LINES);
                glVertex2f (indent_x - tickWidth, tickLocation);
                glVertex2f (indent_x, tickLocation);
              glEnd ();
            }
          }

        // draw FCU dialed (or managed) altitude if within the altitude tape range
        int apAlt = (int) fcuAlt;
        if (fcuAlt >= 0) {
          glLineWidth (2.0);
          if (*altMgd == 0) {
            glColor3ub (COLOR_CYAN);
          } else {
            glColor3ub (COLOR_MAGENTA);
            apAlt = cstrAlt;
          }
          if ((fabs (altPPos) < 580) && (fcuAlt > 0)) {
            double fcuAltLocation = double (refPos) + double (apAlt - altCapt) * tickSpacing / 100;
            glBegin (GL_LINE_STRIP);
              glVertex2f (30, fcuAltLocation-tickSpacing * 0.5);
              glVertex2f (30, fcuAltLocation-tickSpacing * 0.75);
              glVertex2f (10, fcuAltLocation-tickSpacing * 0.75);
              glVertex2f (10, fcuAltLocation-tickSpacing * 0.25);
              glVertex2f (15, fcuAltLocation);
              glVertex2f (10, fcuAltLocation+tickSpacing * 0.25);
              glVertex2f (10, fcuAltLocation+tickSpacing * 0.75);
              glVertex2f (30, fcuAltLocation+tickSpacing * 0.75);
              glVertex2f (30, fcuAltLocation+tickSpacing * 0.5);
            glEnd();
          } else {
            // draw numeric FCU altitude target if outside tape range // fcuAlt, cstrAlt
            if (*altTgtFl == 1) {
              sprintf ( buffer, "FL%i", (int)apAlt/100);
            } else {
              sprintf ( buffer, "%i", apAlt);
            }
        
            if (altPPos > 580) {
              m_pFontManager->Print (fontIndent + fontWidth, partHeight - 16, buffer, m_Font);
            } else if (altPPos < -580) {
              m_pFontManager->Print (fontIndent + fontWidth, 4, buffer, m_Font);
            }
          }
        }
      }
    glPopMatrix ();

    } // end if (!coldDarkPfd)

  } // end Render()

} // end namespace OpenGC

/*=============================================================================

  This is the ogcA320PFDSpeedTape.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Primary Flight Display: the Speed Tape ===

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

===============================================================================

  The OpenGC subproject has been derived from:
    OpenGC - The Open Source Glass Cockpit Project
    Copyright (c) 2001-2003 Damion Shelton

=============================================================================

  The following datarefs are used by this instrument:
v	sim/cockpit/autopilot/airspeed		float
v	sim/cockpit/autopilot/airspeed_is_mach	int
v	AirbusFBW/AlphaProt			float
v	AirbusFBW/AlphaMax			float
v	AirbusFBW/APSPD_Capt			float
v	AirbusFBW/CaptIASValid			int
v	AirbusFBW/IASCapt			float
v	AirbusFBW/IASTrendCapt			float
v	AirbusFBW/SPDdashed			int
v	AirbusFBW/VGreenDot			float
v	AirbusFBW/V1				float
v	AirbusFBW/V1Value			float
v	AirbusFBW/VF				float
v	AirbusFBW/VFENext			float
v	AirbusFBW/VLS				float
v	AirbusFBW/VMO				float
v	AirbusFBW/VR				float
v	AirbusFBW/VS				float

=============================================================================*/

#include <stdio.h>
#include <math.h>

#include "ogcGaugeComponent.h"
#include "ogcA320PFD.h"
#include "ogcA320PFDSpeedTape.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {
  A320PFDSpeedTape::A320PFDSpeedTape () {
    if (verbosity > 0) printf ("A320PFDSpeedTape - constructing\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("A320PFDSpeedTape - constructed\n");
  }

  A320PFDSpeedTape::~A320PFDSpeedTape () {}

  float iasCapt, alphaProt, alphaMax, vMO, apSpd, apSpdCapt;
  float iasTrend, v1Spd, v1Offset, vfSpd, vfnSpd, vgrSpd, vLs, vrSpd, vsSpd;

  void A320PFDSpeedTape::Render () {

    bool ColdAndDark = true;
    CircleEvaluator aCircle;

    GaugeComponent::Render ();

    // Request the datarefs we need

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) ColdAndDark = true; else ColdAndDark = (*cold_and_dark != 0) ? true : false;

    // Speed indicators valid?
    int* spdValid = link_dataref_int ("AirbusFBW/CaptIASValid");

    // Speed visible?
    int* spdDashed = link_dataref_int ("AirbusFBW/SPDdashed");

    // Speed as Mach?
    int* spdIsMach = link_dataref_int ("sim/cockpit/autopilot/airspeed_is_mach");

    // Current airspeed
    float *ias_capt = link_dataref_flt ("AirbusFBW/IASCapt", -1);
    if (*ias_capt != FLT_MISS) iasCapt = *ias_capt; else iasCapt = 0;

    // Alpha Protection speed
    float *alpha_prot = link_dataref_flt ("AirbusFBW/AlphaProt", -1);
    if (*alpha_prot != FLT_MISS) alphaProt = *alpha_prot; else alphaProt = 0;

    // Minimum speed
    float *alpha_max = link_dataref_flt ("AirbusFBW/AlphaMax", -1);
    if (*alpha_max != FLT_MISS) alphaMax = *alpha_max; else alphaMax = 0;

    // Maximum speed
    float *v_mo = link_dataref_flt ("AirbusFBW/VMO",-1);
    if (*v_mo != FLT_MISS) vMO = *v_mo; else vMO = 0;

    // Autopilot speed (knots)
    float *ap_speed = link_dataref_flt ("sim/cockpit/autopilot/airspeed", -1);
    if (*ap_speed != FLT_MISS) apSpd = *ap_speed; else apSpd = 0;

    // Autopilot selected speed or deviation (?)
    float *ap_spd_capt = link_dataref_flt ("AirbusFBW/APSPD_Capt", -1);
    if (*ap_spd_capt != FLT_MISS) apSpdCapt = *ap_spd_capt; else apSpdCapt = 0;

    // The trend in Airspeed
    float *ias_trend = link_dataref_flt ("AirbusFBW/IASTrendCapt", -1);
    if (*ias_trend != FLT_MISS) iasTrend = *ias_trend * 100; else iasTrend = 0;

    // The go/nogo speed
    float *v1_value = link_dataref_flt ("AirbusFBW/V1Value", -1);
    if (*v1_value != FLT_MISS) v1Spd = *v1_value; else v1Spd = 0;

    // Various speed bugs
    float *v1 = link_dataref_flt ("AirbusFBW/V1", -1);
    if (*v1 != FLT_MISS) v1Offset = *v1; else v1Offset = 0;

    float *vF = link_dataref_flt ("qpac_airbus/pfdoutputs/general/VF_value", -1);
    if (*vF != FLT_MISS) vfSpd = *vF; else vfSpd = 0;

    float *vFENext = link_dataref_flt ("qpac_airbus/pfdoutputs/general/VFENext_value", -1);
    if (*vFENext != FLT_MISS) vfnSpd = *vFENext; else vfnSpd = 0;

    float *vGreenDot = link_dataref_flt ("qpac_airbus/pfdoutputs/general/VGreenDot_value", -1);
    if (*vGreenDot != FLT_MISS) vgrSpd = *vGreenDot; else vgrSpd = 0;

    float *v_ls = link_dataref_flt ("qpac_airbus/pfdoutputs/general/VLS_value", -1);
    if (*v_ls != FLT_MISS) vLs = *v_ls; else vLs = 0;

    float *vR = link_dataref_flt ("qpac_airbus/pfdoutputs/general/VR_value", -1);
    if (*vR != FLT_MISS) vrSpd = *vR; else vrSpd = 0;

    float *vS = link_dataref_flt ("qpac_airbus/pfdoutputs/general/VS_value", -1);
    if (*vS != FLT_MISS) vsSpd = *vS; else vsSpd = 0;

    if (verbosity > 2)
    {
      printf ("A320PFDSpeedTape - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320PFDSpeedTape -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320PFDSpeedTape -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320PFDSpeedTape -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    if (!ColdAndDark) {

      double partWidth = m_PhysicalSize.x;	    // defines total component size (just for name simplification)
      double partHeight = m_PhysicalSize.y;
      double refPos = partHeight / 2;             // vertical position of the center reference line
      double tapeIndent = m_PhysicalSize.x - 30;	// defines region of speed tape
      double tapeBottom = 20;
      double tapeHeight = partHeight - 2 * tapeBottom;
      double tapeTop = tapeHeight + tapeBottom;

      double fontWidth = 10;
      double fontHeight = 12;
      double fontIndent = 2;

      glPushMatrix ();

      // The speed tape doesn't display speeds < 30 or > 999
      if (iasCapt < 30.0) iasCapt = 30;
      else if (iasCapt > 999.0) iasCapt = 999;

      // Draw the background rectangle
      glColor3ub (COLOR_AIRBUSGRAY);
      glBegin (GL_POLYGON);
        glVertex2f (0, tapeBottom);
        glVertex2f (0, tapeTop);
        glVertex2f (tapeIndent, tapeTop);
        glVertex2f (tapeIndent, tapeBottom);
      glEnd ();
      // Draw the boundary lines
      glColor3ub (COLOR_GRAY75);
      glLineWidth (2.0);
      glBegin (GL_LINE_STRIP);
        glVertex2f (partWidth, tapeBottom);
        glVertex2f (0, tapeBottom);
        glVertex2f (tapeIndent, tapeBottom);
        glVertex2f (tapeIndent, tapeTop);
        glVertex2f (0, tapeTop);
        glVertex2f (partWidth, tapeTop);
      glEnd ();

      if (*spdValid != 1) {
        m_pFontManager->SetSize (m_Font, 1.5 * fontWidth, 1.5 * fontHeight);
        glColor3ub (COLOR_RED);
        m_pFontManager->Print (fontIndent / 2, refPos, "SPD", m_Font);
      } else {
        // Draw the reference line and triangle
        glColor3ub (COLOR_YELLOW);
        glLineWidth (2.0);
        glBegin (GL_LINE_STRIP);
          glVertex2f (0, tapeBottom + tapeHeight / 2);
          glVertex2f (tapeIndent, tapeBottom + tapeHeight / 2);
        glEnd ();
        glBegin (GL_POLYGON);
          glVertex2f (tapeIndent, tapeBottom + tapeHeight/2);
          glVertex2f (tapeIndent + ((partWidth - tapeIndent)/2), (tapeBottom + tapeHeight / 2) + 10);
          glVertex2f (tapeIndent + ((partWidth - tapeIndent)/2), (tapeBottom + tapeHeight / 2) -10);
        glEnd ();

        // Tick marks are spaced every 10 kts vertically
        // The tick spacing represents how far apart they are in physical units
        double tickSpacing = (tapeTop - 20) / 10;
        double tickWidth = 10;

        m_pFontManager->SetSize (m_Font, fontWidth, fontHeight);
        char buffer[16];
        int nextHigherIAS = (((int) (iasCapt + 9)) / 10) * 10;

        // The vertical offset is how high in physical units the next higher 10's
        // airspeed is above the arrow. For airspeeds divisible by 10, this is 0.
        // I.e. 120, 130 etc. are all aligned with the arrow
        float vertOffset = (nextHigherIAS - iasCapt) / 10 * tickSpacing;

        double tickLocation = 0; // Vertical location of the tick mark
        double texty = 0; // Vertical location of the speed text
        int i = 0; // counter
        int tickSpeed; // speed represented by tick mark

        glLineWidth (2.0);
        glColor3ub (COLOR_WHITE);

        // Draw ticks up from the center
        for (i = 0; i <= ((tapeHeight / 2 - 20) / tickSpacing); i++)  {
          tickSpeed = nextHigherIAS + i * 10;
          tickLocation = refPos + i * tickSpacing + vertOffset;
          glBegin (GL_LINES);
            glVertex2f (tapeIndent - tickWidth, tickLocation);
            glVertex2f (tapeIndent, tickLocation);
          glEnd ();
          if (((tickSpeed % 20) == 0)) { // && (tickLocation < (tapeTop - fontHeight / 2)))
            texty = tickLocation - fontHeight / 2;
            sprintf ( buffer, "%03i", tickSpeed);
            m_pFontManager->Print (fontIndent, texty, &buffer[0], m_Font);
          }
        }

        // Draw ticks down from the center
        for (i = 0; i <= ((tapeHeight / 2 - 20) / (tickSpacing)); i++) {
          tickSpeed = (int) nextHigherIAS - (i + 1) * 10;
          // Only draw ticks if they represent speeds >= 30
          if ( tickSpeed >= 30) {
            tickLocation = refPos - ( i * tickSpacing) - (tickSpacing - vertOffset);
            glBegin (GL_LINES);
              glVertex2f (tapeIndent - tickWidth, tickLocation);
              glVertex2f (tapeIndent, tickLocation);
            glEnd ();
            if (((tickSpeed % 20) == 0)) { // && (tickLocation > (tapeBottom + fontHeight / 2)))
              texty = tickLocation - fontHeight / 2;
              sprintf ( buffer, "%03i", tickSpeed);
              m_pFontManager->Print (fontIndent, texty, &buffer[0], m_Font);
            }
          }
        }

        // Mask off excess drawing from outside top & bottom of the tape
        glColor3ub (COLOR_BLACK);
        glBegin (GL_POLYGON);
          glVertex2f (0, tapeTop); glVertex2f (0, tapeTop + 20); glVertex2f (partWidth, tapeTop + 20); glVertex2f (partWidth, tapeTop);
        glEnd ();
        glBegin (GL_POLYGON);
          glVertex2f (0, tapeBottom); glVertex2f (partWidth, tapeBottom); glVertex2f (partWidth, tapeBottom - 20); glVertex2f (0, tapeBottom - 20);
        glEnd ();

        // The autopilot-commanded speed
        sprintf ( buffer, "%03i", (int) apSpd);

        if (spdDashed == 0)  {
          glColor3ub (COLOR_CYAN);
        } else {
          glColor3ub (COLOR_MAGENTA);
        }

        // draw numeric FCU speed value if outside speed tape range
        if ((apSpdCapt <= 0) && (apSpd >= 100)) {
          m_pFontManager->Print (fontIndent + fontWidth, 4, buffer, m_Font);
        } else {
          if ((*spdIsMach <= 0) && (apSpdCapt >= 84)) {
            m_pFontManager->Print (fontIndent + fontWidth, partHeight - 16, buffer, m_Font);
          } else {
            // draw FCU speed symbol if within tape range (and >= 30!) (left-pointing open triangle)
            if (apSpd >= 30) {
              if (fabs (apSpd - iasCapt) <= 40) {
                double fcuSpeedLocation = refPos + double (apSpd - iasCapt) * tickSpacing / 10;
                glLineWidth (3.0);
                glBegin (GL_LINE_LOOP);
                  glVertex2f (tapeIndent, fcuSpeedLocation);
                  glVertex2f (tapeIndent + ((partWidth-tapeIndent)/2), fcuSpeedLocation + 10);
                  glVertex2f (tapeIndent + ((partWidth-tapeIndent)/2), fcuSpeedLocation - 10);
                glEnd ();
              }
            }
          }
        }

        // The warning tapes

        double topLocation, blockLocation;

        // VLS
        if ((vLs > tapeBottom) && (iasCapt > vLs)) {
          topLocation = refPos + double (vLs - iasCapt) * tickSpacing / 10;
          if (topLocation > tapeTop) topLocation = tapeTop;
          glColor3ub (COLOR_AMBER);
          glLineWidth (2.0);
          glBegin (GL_POLYGON);
            glVertex2f (tapeIndent, tapeBottom);
            glVertex2f (tapeIndent, topLocation);
            glVertex2f (tapeIndent + ((partWidth-tapeIndent)/3), topLocation);
            glVertex2f (tapeIndent + ((partWidth-tapeIndent)/3), tapeBottom);
          glEnd ();
          glColor3ub (COLOR_BLACK);
          glLineWidth (2.0);
          glBegin (GL_POLYGON);
            glVertex2f (tapeIndent, tapeBottom);
            glVertex2f (tapeIndent, topLocation - 3);
            glVertex2f (tapeIndent + ((partWidth-tapeIndent)/3) - 3, topLocation - 3);
            glVertex2f (tapeIndent + ((partWidth-tapeIndent)/3) - 3, tapeBottom);
          glEnd ();
        }

        // AlphaProt
        if ((alphaProt > tapeBottom) && (iasCapt > alphaProt)) {
          topLocation = refPos + double (alphaProt - iasCapt) * tickSpacing / 10;
          if (topLocation > tapeTop) topLocation = tapeTop;
          glColor3ub (COLOR_AMBER);
          glLineWidth (2.0);
          glBegin (GL_POLYGON);
            glVertex2f (tapeIndent, tapeBottom);
            glVertex2f (tapeIndent, topLocation);
            glVertex2f (tapeIndent + ((partWidth-tapeIndent) / 3), topLocation);
            glVertex2f (tapeIndent + ((partWidth-tapeIndent) / 3), tapeBottom);
          glEnd ();
          glColor3ub (COLOR_BLACK);
          glLineWidth (2.0);
          int i;
          for (i = 0; i  < 10; i++) {
            blockLocation = topLocation - 10 * i - 10;
            if (blockLocation > tapeBottom) {
              glBegin (GL_POLYGON);
                glVertex2f (tapeIndent, blockLocation);
                glVertex2f (tapeIndent, blockLocation + 5);
                glVertex2f (tapeIndent + ((partWidth - tapeIndent)/3) - 3, blockLocation + 5);
                glVertex2f (tapeIndent + ((partWidth - tapeIndent)/3) - 3, blockLocation);
              glEnd ();
            }
          }
        }

        // AlphaMax
        if ((alphaMax > tapeBottom) && (iasCapt > alphaMax)) {
          topLocation = refPos + double (alphaMax - iasCapt) * tickSpacing / 10;
          if (topLocation > tapeTop) topLocation = tapeTop;
          glColor3ub (COLOR_RED);
          glLineWidth (2.0);
          glBegin (GL_POLYGON);
            glVertex2f (tapeIndent, tapeBottom);
            glVertex2f (tapeIndent, topLocation);
            glVertex2f (tapeIndent + ((partWidth - tapeIndent) / 3), topLocation);
            glVertex2f (tapeIndent + ((partWidth - tapeIndent) / 3), tapeBottom);
          glEnd ();
        }

        // VMO
        if ((vMO > 0) && (iasCapt < vMO)) {
          topLocation = refPos + double (vMO - iasCapt) * tickSpacing / 10;
          if (topLocation < tapeBottom) topLocation = tapeBottom;
          glColor3ub (COLOR_AMBER);
          glLineWidth (2.0);
          glBegin (GL_POLYGON);
            glVertex2f (tapeIndent, tapeTop);
            glVertex2f (tapeIndent, topLocation);
            glVertex2f (tapeIndent + ((partWidth - tapeIndent) / 3), topLocation);
            glVertex2f (tapeIndent + ((partWidth - tapeIndent) / 3), tapeTop);
          glEnd ();
          glColor3ub (COLOR_BLACK);
          glLineWidth (2.0);
          int i  = 0;
          for (i = 0; i < 15; i++)  {
            blockLocation = topLocation + 10 * i + 10;
            if (blockLocation < tapeTop) {
              glBegin (GL_POLYGON);
                glVertex2f (tapeIndent, blockLocation);
                glVertex2f (tapeIndent, blockLocation - 5);
                glVertex2f (tapeIndent + ((partWidth-tapeIndent) / 3) - 3, blockLocation - 5);
                glVertex2f (tapeIndent + ((partWidth-tapeIndent) / 3) - 3, blockLocation);
              glEnd ();
            }
          }
        }

        // draw V1 speed symbol if within the speed tape range (and >= 30!)
        if (v1Spd > 30  && (abs (v1Spd - iasCapt) <= 40)) {
          float v1SpdLocation = refPos + (v1Spd - iasCapt) * tickSpacing / 10 - fontHeight/2;
          glLineWidth (3.0);
          glColor3ub (COLOR_CYAN);
          m_pFontManager->Print (tapeIndent + 3, v1SpdLocation, "1", m_Font);
        } else if ((v1Spd - iasCapt) > 40) {
          // draw numeric V1 speed value if outside tape range
          glColor3ub (COLOR_CYAN);
          sprintf ( buffer, "%03i", (int) v1Spd);
          m_pFontManager->Print (tapeIndent + 3, partHeight - 35, buffer, m_Font);
        }

        // draw VR speed symbol if within the speed tape range
        if (vrSpd > 30 && (abs (vrSpd - iasCapt) <= 40)) {
          float vrSpdLocation = refPos + (vrSpd - iasCapt) * tickSpacing / 10;
          glLineWidth (2.0);
          glColor3ub (COLOR_CYAN);
          aCircle.SetOrigin (tapeIndent, vrSpdLocation);
          aCircle.SetRadius (5);
          aCircle.SetDegreesPerPoint (5);
          aCircle.SetArcStartEnd (  0.0, 360.0);
          glBegin (GL_LINE_STRIP);
            aCircle.Evaluate ();
          glEnd ();
        }

        // draw GreenDot speed symbol if within the speed tape range (and >= 30!)
        if (vgrSpd > 30 && ((vgrSpd - iasCapt) <= 40)) {
          float vgrSpdLocation = refPos + (vgrSpd - iasCapt) * tickSpacing / 10;
          glColor3ub (COLOR_GREEN);
          glLineWidth (2.0);
          aCircle.SetOrigin (tapeIndent, vgrSpdLocation);
          aCircle.SetRadius (3);
          aCircle.SetDegreesPerPoint (5);
          aCircle.SetArcStartEnd (  0.0, 360.0);
          glBegin (GL_LINE_STRIP);
            aCircle.Evaluate ();
          glEnd ();
        }

        // draw VF speed symbol if within the speed tape range (and >= 30!)
        if (vfSpd > 30 && ((vfSpd - iasCapt) <= 40)) {
          double vfSpdLocation = refPos + double (vfSpd - iasCapt) * tickSpacing / 10 - fontHeight / 2;
          glLineWidth (3.0);
          glColor3ub (COLOR_MAGENTA);
          m_pFontManager->Print (tapeIndent+3, vfSpdLocation, "F", m_Font);
        }

        // draw VS speed symbol if within the speed tape range (and >= 30!)
        if (vsSpd >= 30 && ((vsSpd - iasCapt) <= 40)) {
          double vsSpdLocation = refPos + double (vsSpd - iasCapt) * tickSpacing / 10 - fontHeight / 2;
          glLineWidth (3.0);
          glColor3ub (COLOR_GREEN);
          glBegin (GL_LINES);
            glVertex2f (tapeIndent - 3, vsSpdLocation);
            glVertex2f (tapeIndent, vsSpdLocation);
          glEnd ();
          glColor3ub (COLOR_MAGENTA);
          m_pFontManager->Print (tapeIndent+3, vsSpdLocation, "S", m_Font);
        }

        // draw VFENext speed symbol if within the speed tape range (and >= 30!)
        if (vfnSpd > 30 && (abs (vfnSpd - iasCapt) <= 40)) {
          double vfnSpdLocation = refPos + double (vfnSpd - iasCapt) * tickSpacing / 10 - fontHeight / 2;
          glColor3ub (COLOR_MAGENTA);
          m_pFontManager->Print (tapeIndent-fontWidth, vfnSpdLocation, "=", m_Font);
        }

        // draw speed trend arrow if within the speed tape range (and >= 2)
        int spdTrend = (iasTrend + 5) / 10;
        if ((abs (spdTrend) > 2) && (abs (spdTrend) <= 40)) {
          double spdTrendLocation = refPos + spdTrend * tickSpacing / 10;
          glLineWidth (2.0);
          glColor3ub (COLOR_WHITE);
          if (spdTrend > 0) {
            glBegin (GL_LINE_STRIP);
              glVertex2f (tapeIndent - 10, refPos);
              glVertex2f (tapeIndent - 10, spdTrendLocation);
              glVertex2f (tapeIndent - 15, spdTrendLocation - 10);
              glVertex2f (tapeIndent - 10, spdTrendLocation);
              glVertex2f (tapeIndent -  5, spdTrendLocation - 10);
            glEnd ();
          } else {
            glBegin (GL_LINE_STRIP);
              glVertex2f (tapeIndent-10, refPos);
              glVertex2f (tapeIndent-10, spdTrendLocation);
              glVertex2f (tapeIndent-15, spdTrendLocation+10);
              glVertex2f (tapeIndent-10, spdTrendLocation);
              glVertex2f (tapeIndent-5, spdTrendLocation+10);
            glEnd ();
          }
        }
      }

      glPopMatrix ();

    } // end if (!ColdAndDark)

  } // end Render()

} // end namespace OpenGC

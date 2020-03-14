/*=============================================================================

  This is the ogcA320PFDMiscInfo.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Primary Flight Display: Miscellaneous Info ===

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
v	sim/cockpit2/radios/indicators/inner_marker_lit		int
v	sim/cockpit2/radios/indicators/middle_marker_lit	int
v	sim/cockpit2/radios/indicators/outer_marker_lit		int
v	sim/cockpit2/gauges/actuators/barometer_setting_in_hg_pilot	float
v	AirbusFBW/ALTCapt_M		float
v	AirbusFBW/ALTisCstr		int
v	AirbusFBW/BaroStdCapt		int
v	AirbusFBW/BaroUnitCapt		int
v	AirbusFBW/CaptALTValid		int
v	AirbusFBW/FCUALT_M		float
v	AirbusFBW/HideBaroCapt		int
v	AirbusFBW/MetricAlt		int
v	AirbusFBW/ILSonCapt		int
v	AirbusFBW/ILS1			byte[8]
v	AirbusFBW/ILS2			byte[8]
v	AirbusFBW/ILS3			byte[8]
v	AirbusFBW/ShowMachCapt		int
v	AirbusFBW/MachCapt		float

=============================================================================*/

#include "ogcGaugeComponent.h"
#include "ogcA320PFD.h"
#include "ogcA320PFDMiscInfo.h"

namespace OpenGC {

  A320PFDMiscInfo::A320PFDMiscInfo () {
    if (verbosity > 0) printf ("A320PFDMiscInfo - constructing\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("A320PFDMiscInfo - constructed\n");
  }

  A320PFDMiscInfo::~A320PFDMiscInfo () {}

  void A320PFDMiscInfo::Render () {

    bool ColdAndDark = true;
    char buffer[12];

    GaugeComponent::Render ();

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) ColdAndDark = true; else ColdAndDark = (*cold_and_dark != 0) ? true : false;

    // Markers
    int *im_lit = link_dataref_int ("sim/cockpit2/radios/indicators/inner_marker_lit");
    int *mm_lit = link_dataref_int ("sim/cockpit2/radios/indicators/middle_marker_lit");
    int *om_lit = link_dataref_int ("sim/cockpit2/radios/indicators/outer_marker_lit");

    // Altitude indicators valid?
    int *altValid = link_dataref_int ("AirbusFBW/CaptALTValid");

    // Barometer settings to be shown?
    int *hideBaro = link_dataref_int ("AirbusFBW/HideBaroCapt");

    // Barometer STD indicator?
    int *baroStd = link_dataref_int ("AirbusFBW/BaroStdCapt");

    // Barometer Unit?
    int *baroUnit = link_dataref_int ("AirbusFBW/BaroUnitCapt");

    // Barometer setting?
    float baroSetting;
    float *baro_setting = link_dataref_flt ("sim/cockpit2/gauges/actuators/barometer_setting_in_hg_pilot",- 2);
    if (*baro_setting != FLT_MISS) baroSetting = *baro_setting; else baroSetting = 0;

    // Metric altitude indications
    int *metricAlt = link_dataref_int ("AirbusFBW/MetricAlt");

    // altitude is managed?
    int *altMgd = link_dataref_int ("AirbusFBW/ALTisCstr");

    // Metric altitude
    int altCaptM = 0;
    float *alt_capt_m = link_dataref_flt ("AirbusFBW/ALTCapt_M", 0);
    if (*alt_capt_m != FLT_MISS) altCaptM = *alt_capt_m;

    // Metric FCU altitude
    int fcuAltM = 0;
    float *fcu_alt_m = link_dataref_flt ("AirbusFBW/FCUALT_M", 0);
    if (*fcu_alt_m != FLT_MISS) fcuAltM = *fcu_alt_m;

    // ILS info
    int* lsValid = link_dataref_int ("AirbusFBW/ILSonCapt");
    unsigned char *ils_1 = link_dataref_byte_arr("AirbusFBW/ILS1", 8, -1); if (strlen ((char*)ils_1) > 8) ils_1[8] = '\0';
    unsigned char *ils_2 = link_dataref_byte_arr("AirbusFBW/ILS2", 8, -1); if (strlen ((char*)ils_2) > 8) ils_2[8] = '\0';
    unsigned char *ils_3 = link_dataref_byte_arr("AirbusFBW/ILS3", 8, -1); if (strlen ((char*)ils_3) > 8) ils_3[8] = '\0';

    // Mach number
    int *machShown = link_dataref_int ("AirbusFBW/ShowMachCapt");
    int machNum = 0;
    float *mach_number = link_dataref_flt ("AirbusFBW/MachCapt", 0);
    if (*mach_number != FLT_MISS) machNum = (int) (*mach_number * 100);

    if (verbosity > 2)
    {
      printf ("A320PFDMiscInfo - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320PFDMiscInfo -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320PFDMiscInfo -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320PFDMiscInfo -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    if (!ColdAndDark) {

      if (*altValid == 1) {
        m_pFontManager->SetSize(m_Font, 14, 14);

        // Radio Markers stuff
        if (*im_lit == 1) {
          glColor3ub (COLOR_WHITE);
          m_pFontManager->Print (320, 80, "IM", m_Font);
        }
        if (*mm_lit == 1) {
          glColor3ub (COLOR_YELLOW);
          m_pFontManager->Print (320, 80, "MM", m_Font);
        }
        if (*om_lit == 1) {
          glColor3ub (COLOR_CYAN);
          m_pFontManager->Print (320, 80, "OM", m_Font);
        }

        // Barometer setting stuff
        if (*hideBaro != 0) {
          if (*baroStd == 1) {
            glLineWidth (2.0);
            glColor3ub (COLOR_YELLOW);
            glBegin (GL_LINE_LOOP);
              glVertex2f (387, 67);
              glVertex2f (427, 67);
              glVertex2f (427, 87);
              glVertex2f (387, 87);
            glEnd ();
            glColor3ub (COLOR_CYAN);
            m_pFontManager->Print (390, 70, "STD", m_Font);
          } else {
            glColor3ub (COLOR_WHITE);
            m_pFontManager->Print (360, 70, "QNH", m_Font);
            glColor3ub (COLOR_CYAN);
            if (*baroUnit != 1) {
              sprintf (buffer, "%2.2f", baroSetting); // inHg
              m_pFontManager->Print (400, 70, &buffer[0], m_Font);
            } else {
              sprintf (buffer, "%4i", (int) (baroSetting*1013/29.92)); // hPa
              m_pFontManager->Print (400, 70, &buffer[0], m_Font);
            }
          }
        } // end if (*hideBaro != 0)

        // Metric Altitude stuff
        if (*metricAlt == 1) {
          // show metric alt capt in blue w/yellow box (below QNH/STD field)
          glLineWidth (2.0);
          glColor3ub (COLOR_YELLOW);
          glBegin (GL_LINE_LOOP);
            glVertex2f (360, 37);
            glVertex2f (440, 37);
            glVertex2f (440, 55);
            glVertex2f (360, 55);
          glEnd ();
          glColor3ub (COLOR_CYAN);
          sprintf (buffer, "%5i M", altCaptM); // inHg
          m_pFontManager->Print (365, 40, &buffer[0], m_Font);
          // show metric FCU-commanded altitude
          if (*altMgd == 1) {
             glColor3ub (COLOR_MAGENTA);
          } else {
            glColor3ub (COLOR_CYAN);
          }
          sprintf (buffer, "%5i M", fcuAltM); // inHg
          m_pFontManager->Print (290, 355, &buffer[0], m_Font);
        } // end if (*metricAlt == 1)

      } // end if (*altValid == 1)

      if (*lsValid == 1) {
        m_pFontManager->SetSize(m_Font, 12, 12);
        glColor3ub (COLOR_MAGENTA);
        m_pFontManager->Print (10, 46, (const char*) ils_1, m_Font); // ILS ID
        m_pFontManager->Print (10, 30, (const char*) ils_2, m_Font); // ILS course
        m_pFontManager->Print (10, 14, (const char*) ils_3, m_Font); // ILS Distance
      }

      if (*machShown == 1) {
        m_pFontManager->SetSize (m_Font, 14, 14);
        glColor3ub (COLOR_MAGENTA);
        sprintf (buffer, ".%02i", machNum); // inHg
        m_pFontManager->Print (25, 70, &buffer[0], m_Font);
      }

    } // end if (!ColdAndDark)

  } // end Render()

} // end namespace OpenGC

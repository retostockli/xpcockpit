/*=============================================================================

  This is the ogcA320StbyRMIComponent.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Standby Radio magnetic Indicator ===

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
#include "ogcA320StbyRMI.h"
#include "ogcA320StbyRMIComponent.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320StbyRMIComponent::A320StbyRMIComponent () {  
    if (verbosity > 0) printf ("A320StbyRMIComponent - constructing\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("A320StbyRMIComponent - constructed\n");
  }

  A320StbyRMIComponent::~A320StbyRMIComponent () {}
  
  int efis1Select, efis2Select;
  float brngNav1,  brngNav2,  brngAdf1,  brngAdf2;
  float dstNav1, dstNav2, dstAdf1, dstAdf2;
  float hdngMag;
  char buf[36];

  void A320StbyRMIComponent::Render () {

    CircleEvaluator aCircle;
    float fontSize = 12;

    // Call base class to setup for size and position
    GaugeComponent::Render ();

    bool coldDarkRmi = true;

    // Request the datarefs we want to use

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldDarkRmi = true; else coldDarkRmi = (*cold_and_dark != 0) ? true : false;

    int *nav1_has_dme = link_dataref_int ("sim/cockpit2/radios/indicators/nav1_has_dme");
    int *nav2_has_dme = link_dataref_int ("sim/cockpit2/radios/indicators/nav2_has_dme");
    int *adf1_has_dme = link_dataref_int ("sim/cockpit2/radios/indicators/adf1_has_dme");
    int *adf2_has_dme = link_dataref_int ("sim/cockpit2/radios/indicators/adf2_has_dme");
    float *nav1_dme_distance_nm = link_dataref_flt ("sim/cockpit2/radios/indicators/nav1_dme_distance_nm",-1);
    if (*nav1_dme_distance_nm != FLT_MISS) dstNav1 = *nav1_dme_distance_nm; else dstNav1 = 0;
    float *nav2_dme_distance_nm = link_dataref_flt ("sim/cockpit2/radios/indicators/nav2_dme_distance_nm",-1);
    if (*nav2_dme_distance_nm != FLT_MISS) dstNav2 = *nav2_dme_distance_nm; else dstNav2 = 0;
    float *adf1_dme_distance_nm = link_dataref_flt ("sim/cockpit2/radios/indicators/adf1_dme_distance_nm",-1);
    if (*adf1_dme_distance_nm != FLT_MISS) dstAdf1 = *adf1_dme_distance_nm; else dstAdf1 = 0;
    float *adf2_dme_distance_nm = link_dataref_flt ("sim/cockpit2/radios/indicators/adf2_dme_distance_nm",-1);
    if (*adf2_dme_distance_nm != FLT_MISS) dstAdf2 = *adf2_dme_distance_nm; else dstAdf2 = 0;
    float *nav1_bearing = link_dataref_flt ("sim/cockpit2/radios/indicators/nav1_relative_bearing_deg",-1);
    if (*nav1_bearing != FLT_MISS) brngNav1 = *nav1_bearing; else brngNav1 = 0;
    float *nav2_bearing = link_dataref_flt ("sim/cockpit2/radios/indicators/nav2_relative_bearing_deg",-1);
    if (*nav2_bearing != FLT_MISS) brngNav2 = *nav2_bearing; else brngNav2 = 0;
    float *adf1_bearing = link_dataref_flt ("sim/cockpit2/radios/indicators/adf1_relative_bearing_deg",-1);
    if (*adf1_bearing != FLT_MISS) brngAdf1 = *adf1_bearing; else brngAdf1 = 0;
    float *adf2_bearing = link_dataref_flt ("sim/cockpit2/radios/indicators/adf2_relative_bearing_deg",-1);
    if (*adf2_bearing != FLT_MISS) brngAdf2 = *adf2_bearing; else brngAdf2 = 0;
    int *efis1_select = link_dataref_int ("sim/cockpit2/EFIS/EFIS_1_selection_pilot"); // 0=ADF, 1=off, 2=VOR
    if (*efis1_select != FLT_MISS) efis1Select = *efis1_select; else efis1Select = 1;
    int *efis2_select = link_dataref_int ("sim/cockpit2/EFIS/EFIS_2_selection_pilot"); // 0=ADF, 1=off, 2=VOR
    if (*efis2_select != FLT_MISS) efis2Select = *efis2_select; else efis2Select = 1;
    float *heading_mag = link_dataref_flt ("sim/cockpit2/gauges/indicators/compass_heading_deg_mag",-1);
    if (*heading_mag != FLT_MISS) hdngMag = *heading_mag; else hdngMag = 0;
  if (verbosity > 1) {

    printf ("A320StbyRMIComponent - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
    printf ("A320StbyRMIComponent -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
    printf ("A320StbyRMIComponent -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
    printf ("A320StbyRMIComponent -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
  }

  if (!coldDarkRmi) {

    double partSize = m_PhysicalSize.x;	// defines total component size (just for name simplification) (square gauge!)
    double halfSize = partSize / 2;		// defines component center

    m_pFontManager->SetSize (m_Font, fontSize, fontSize);

    double cardSize = halfSize - 30;

      // do any rendering here

      // Fixed texts
      m_pFontManager->SetSize (m_Font, 11, 11);
      glColor3ub (COLOR_WHITE);
      m_pFontManager->Print (10, partSize - 4 * fontSize, "DME1", m_Font);
      m_pFontManager->Print (partSize - 4 * fontSize - 5, partSize - 4 * fontSize, "DME2", m_Font);

      // Draw at compass card center
      glPushMatrix ();
        glTranslated (halfSize, halfSize - 10, 0);
        glPushMatrix (); // Start of compass card
          glRotatef (hdngMag, 0, 0, 1); // Rotate to plane heading
          glColor3ub (COLOR_GRAY75);
          glLineWidth (1);
          for (int i = 0; i < 36; i++) {
            glBegin (GL_LINES);
              glVertex2d (0, cardSize); glVertex2d (0, cardSize - 10);
            glEnd ();
            glRotatef (-10, 0, 0, 1);
          }
          glRotatef (-5, 0, 0, 1);
          for (int i = 0; i < 36; i++) {
            glBegin (GL_LINES);
              glVertex2d (0, cardSize); glVertex2d (0, cardSize - 5);
            glEnd ();
            glRotatef (-10, 0, 0, 1);
          }
          glRotatef (5, 0, 0, 1);
          m_pFontManager->SetSize (m_Font, 10, 10);
          m_pFontManager->Print (-fontSize / 3, cardSize - 22, "0", m_Font);
          glRotatef (-30, 0, 0, 1);
          m_pFontManager->Print (-fontSize / 3, cardSize - 22, "3", m_Font);
          glRotatef (-30, 0, 0, 1);
          m_pFontManager->Print (-fontSize / 3, cardSize - 22, "6", m_Font);
          glRotatef (-30, 0, 0, 1);
          m_pFontManager->Print (-fontSize / 3, cardSize - 22, "9", m_Font);
          glRotatef (-30, 0, 0, 1);
          m_pFontManager->Print (-fontSize / 1.5, cardSize - 22, "12", m_Font);
          glRotatef (-30, 0, 0, 1);
          m_pFontManager->Print (-fontSize / 1.5, cardSize - 22, "15", m_Font);
          glRotatef (-30, 0, 0, 1);
          m_pFontManager->Print (-fontSize / 1.5, cardSize - 22, "18", m_Font);
          glRotatef (-30, 0, 0, 1);
          m_pFontManager->Print (-fontSize / 1.5, cardSize - 22, "21", m_Font);
          glRotatef (-30, 0, 0, 1);
          m_pFontManager->Print (-fontSize / 1.5, cardSize - 22, "24", m_Font);
          glRotatef (-30, 0, 0, 1);
          m_pFontManager->Print (-fontSize / 1.5, cardSize - 22, "27", m_Font);
          glRotatef (-30, 0, 0, 1);
          m_pFontManager->Print (-fontSize / 1.5, cardSize - 22, "30", m_Font);
          glRotatef (-30, 0, 0, 1);
          m_pFontManager->Print (-fontSize / 1.5, cardSize - 22, "33", m_Font);
        glPopMatrix (); // Back to heading 000; end of card

        // The NavAids arrows (we are still at card center!)

        // The VOR1/ADF1 Arrow
        glPushMatrix ();
          glLineWidth (1);
          glColor3ub (COLOR_YELLOW);
          if (efis1Select == 2) glRotatef (-brngNav1, 0, 0, 1); // VOR1 Bearing
          else if (efis1Select == 0) glRotatef (-brngAdf1, 0, 0, 1); // ADF1 Bearing
          glBegin (GL_LINE_STRIP);
            glVertex2d (0, cardSize - 35); glVertex2d (-5, cardSize -45);
            glVertex2d (-5, cardSize - 45); glVertex2d (0, cardSize - 15);
            glVertex2d (0, cardSize - 15); glVertex2d (5, cardSize - 45);
            glVertex2d (5, cardSize - 45); glVertex2d (0, cardSize - 35);
            glVertex2d (0, cardSize - 35); glVertex2d (0, -cardSize + 15);
          glEnd ();
        glPopMatrix ();

        // The VOR2/ADF2 Arrow
        glPushMatrix ();
          glLineWidth (1);
          glColor3ub (COLOR_WHITE);
          if (efis2Select == 2) glRotatef (-brngNav2, 0, 0, 1); // VOR1 Bearing
          else if (efis2Select == 2) glRotatef (-brngAdf2, 0, 0, 1); // ADF1 Bearing
          glBegin (GL_LINE_STRIP);
            glVertex2d (-5, cardSize - 35); glVertex2d (-10, cardSize -45);
            glVertex2d (-10, cardSize - 45); glVertex2d (0, cardSize - 15);
            glVertex2d (0, cardSize - 15); glVertex2d (10, cardSize - 45);
            glVertex2d (10, cardSize - 45); glVertex2d (5, cardSize - 35);
            glVertex2d (5, cardSize - 35); glVertex2d (5, -cardSize + 15);
            glVertex2d (5, -cardSize + 15); glVertex2d (-5, -cardSize + 15);
            glVertex2d (-5, -cardSize + 15); glVertex2d (-5, cardSize - 15);
          glEnd ();
        glPopMatrix ();

        // The needle center cover
        glColor3ub (COLOR_GRAY25);
        aCircle.SetOrigin (0, 0);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetRadius (15);
        aCircle.SetArcStartEnd (0, 360); glBegin (GL_POLYGON); aCircle.Evaluate (); glEnd ();

      glPopMatrix (); // Back to origin; end of arrows

      // End of compass card (Again centered on origin)

      // The numeric value of the DME distances
      glLineWidth (2);
      m_pFontManager->SetSize (m_Font, 14, 14);
      glColor3ub (COLOR_WHITE);
      if (efis1Select == 2) {
        sprintf (buf, "%05.1f", dstNav1);
        m_pFontManager->Print (20, partSize - 2 * fontSize, buf, m_Font);
      }
      if (efis1Select == 0) {
        sprintf (buf, "%05.1f", dstAdf1);
        m_pFontManager->Print (20, partSize - 2 * fontSize, buf, m_Font);
      }
      if (efis1Select == 2) {
        sprintf (buf, "%05.1f", dstNav2);
        m_pFontManager->Print (partSize - 10 - 5 * fontSize, partSize - 2 * fontSize, buf, m_Font);
      }
      if (efis1Select == 0) {
        sprintf (buf, "%05.1f", dstAdf1);
        m_pFontManager->Print (partSize - 10 - 5 * fontSize, partSize - 2 * fontSize, buf, m_Font);
      }

    } // end if (! coldDarkRmi)

  } // end Render ()

} // end namespace OpenGC


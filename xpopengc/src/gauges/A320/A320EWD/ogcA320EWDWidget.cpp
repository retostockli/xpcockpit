/*=============================================================================

  This is the ogcA320EWDWidget.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Engine/Warning Display Widget ===

  Created:
    Date:        2011-11-14
    Author:      Hans Jansen
    last change: 2020-02-06
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

/*===========================================================================
 * Note: this module is not used; it is only a proof-of-concept!
 * ==========================================================================*/

#include <math.h>
#include "ogcA320EWDWidget.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320EWDWidget::A320EWDWidget () {
    if (verbosity > 1) printf("A320EWDWidget - constructing\n");

    m_Font = m_pFontManager->LoadFont((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 0) printf("A320EWDWidget - constructed\n");
  }

  A320EWDWidget::~A320EWDWidget () {}

  int slatReq = 0; float slatReqPos[] = { 0.000, 0.600, 0.800, 1.000 };
  int flapReq = 0; float flapReqPos[] = { 0.000, 0.250, 0.375, 0.500, 1.000 };

  void A320EWDWidget::Render () {

    bool coldAndDark = true;
    CircleEvaluator aCircle;

    GaugeComponent::Render ();

    if (verbosity > 2) {
      printf ("A320EWDWidget - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320EWDWidget -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320EWDWidget -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320EWDWidget -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    // The datarefs we want to use on this instrument

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    float eng1Epr = 0, eng2Epr = 0;
    float *eng_epr = link_dataref_flt_arr ("AirbusFBW/ENGEPRArray",4,-1,0);
    if (*eng_epr != FLT_MISS) { eng1Epr = eng_epr[0]; eng2Epr = eng_epr[1]; };

    int eng1RevSw = 0, eng2RevSw = 0;
    int *eng_rev_sw = link_dataref_int_arr ("AirbusFBW/EngRevArray", 4, -1);
    if (*eng_rev_sw != INT_MISS) { eng1RevSw = eng_rev_sw[0]; eng2RevSw = eng_rev_sw[1]; };

    int ewdStartMode = 0;
    int *ewd_start_mode = link_dataref_int ("AirbusFBW/EWDStartMode"); // ?? what is this used for ??
    if (*ewd_start_mode != INT_MISS) ewdStartMode = *ewd_start_mode;

    int eng1FadecState = 0, eng2FadecState = 0;
    int *eng_fadec_state = link_dataref_int_arr ("AirbusFBW/FADECStateArray", 4, -1);
    if (*eng_fadec_state != INT_MISS) { eng1FadecState = eng_fadec_state[0]; eng2FadecState = eng_fadec_state[1]; };

    int *flap_req_pos = link_dataref_int ("AirbusFBW/FlapRequestPos");
    if ((*flap_req_pos != INT_MISS) && (*flap_req_pos >= 0)) // can be -1, but that is an illegal value to work with...
    {
      flapReq = *flap_req_pos;
//      flapReqPos = (float) flapReq * 0.20; // range 0 ... 5; normalized to 0.0 ... 1.0
    }

    int *slat_req_pos = link_dataref_int ("AirbusFBW/SlatRequestPos");
    if ((*slat_req_pos != INT_MISS) && (*slat_req_pos >= 0)) // see remark on flap_req_pos
    {
      slatReq = *slat_req_pos;
//      slatReqPos = (float) slatReq * 0.25; // range 0 ... 4; normalized to 0.0 ... 1.0
    }

    float eng1Egt = 0, eng2Egt = 0;
    float *eng_egt = link_dataref_flt_arr ("sim/cockpit2/engine/indicators/EGT_deg_C",8,-1,0);
    if (*eng_egt != FLT_MISS) { eng1Egt = eng_egt[0]; eng2Egt = eng_egt[1]; };

    float eng1FF = 0, eng2FF = 0;
    float *eng_ff = link_dataref_flt_arr ("sim/cockpit2/engine/indicators/fuel_flow_kg_sec",8,-1,0);
    if (*eng_ff != FLT_MISS) { eng1FF = eng_ff[0]; eng2FF = eng_ff[1]; };

    float eng1N1 = 0, eng2N1 = 0;
    float *eng_n1 = link_dataref_flt_arr ("sim/cockpit2/engine/indicators/N1_percent",8,-1,0); if (*eng_n1 != FLT_MISS)
    if (*eng_n1 != FLT_MISS) { eng1N1 = eng_n1[0]; eng2N1 = eng_n1[1]; };

    float eng1N2 = 0, eng2N2 = 0;
    float *eng_n2 = link_dataref_flt_arr ("sim/cockpit2/engine/indicators/N2_percent",8,-1,0);
    if (*eng_n2 != FLT_MISS) { eng1N2 = eng_n2[0]; eng2N2 = eng_n2[1]; };

    float slatRatio = 0;
    float *slat_rat  = link_dataref_flt ("sim/flightmodel/controls/slatrat", -2);
    if (*slat_rat != FLT_MISS) slatRatio = *slat_rat;

    float flapRatio = 0;
    float *flap_rat  = link_dataref_flt ("sim/flightmodel/controls/wing2l_fla1def", -2);
    if (*flap_rat != FLT_MISS) flapRatio = *flap_rat / 40; // funny dataref... why not "sim/flightmodel/controls/flaprat" ??

    float eng1N1a = 0, eng2N1a = 0;
    float *eng_n1a = link_dataref_flt_arr ("sim/flightmodel/engine/ENGN_N1_",8,-1,0);
    if (*eng_n1a != FLT_MISS) { eng1N1a = eng_n1a[0]; eng2N1a = eng_n1a[1]; };

    float eng1N2a = 0, eng2N2a = 0;
    float *eng_n2a = link_dataref_flt_arr ("sim/flightmodel/engine/ENGN_N2_",8,-1,0);
    if (*eng_n2a != FLT_MISS) { eng1N2a = eng_n2a[0]; eng2N2a = eng_n2a[1]; };

    float totFOB = 0;
    float *tot_fuel = link_dataref_flt ("sim/flightmodel/weight/m_fuel_total", 0);
    if (*tot_fuel != FLT_MISS) totFOB = *tot_fuel;

    unsigned char *ewd_1a = link_dataref_byte_arr ("AirbusFBW/EWD1aText", 36, -1); if (strlen((char*)ewd_1a) > 36) ewd_1a[36] = '\0';
    unsigned char *ewd_1b = link_dataref_byte_arr ("AirbusFBW/EWD1bText", 36, -1); if (strlen((char*)ewd_1b) > 36) ewd_1b[36] = '\0';
    unsigned char *ewd_1g = link_dataref_byte_arr ("AirbusFBW/EWD1gText", 36, -1); if (strlen((char*)ewd_1g) > 36) ewd_1g[36] = '\0';
    unsigned char *ewd_1r = link_dataref_byte_arr ("AirbusFBW/EWD1rText", 36, -1); if (strlen((char*)ewd_1r) > 36) ewd_1r[36] = '\0';
    unsigned char *ewd_1w = link_dataref_byte_arr ("AirbusFBW/EWD1wText", 36, -1); if (strlen((char*)ewd_1w) > 36) ewd_1w[36] = '\0';
    unsigned char *ewd_2a = link_dataref_byte_arr ("AirbusFBW/EWD2aText", 36, -1); if (strlen((char*)ewd_2a) > 36) ewd_2a[36] = '\0';
    unsigned char *ewd_2b = link_dataref_byte_arr ("AirbusFBW/EWD2bText", 36, -1); if (strlen((char*)ewd_2b) > 36) ewd_2b[36] = '\0';
    unsigned char *ewd_2g = link_dataref_byte_arr ("AirbusFBW/EWD2gText", 36, -1); if (strlen((char*)ewd_2g) > 36) ewd_2g[36] = '\0';
    unsigned char *ewd_2r = link_dataref_byte_arr ("AirbusFBW/EWD2rText", 36, -1); if (strlen((char*)ewd_2r) > 36) ewd_2r[36] = '\0';
    unsigned char *ewd_2w = link_dataref_byte_arr ("AirbusFBW/EWD2wText", 36, -1); if (strlen((char*)ewd_2w) > 36) ewd_2w[36] = '\0';
    unsigned char *ewd_3a = link_dataref_byte_arr ("AirbusFBW/EWD3aText", 36, -1); if (strlen((char*)ewd_3a) > 36) ewd_3a[36] = '\0';
    unsigned char *ewd_3b = link_dataref_byte_arr ("AirbusFBW/EWD3bText", 36, -1); if (strlen((char*)ewd_3b) > 36) ewd_3b[36] = '\0';
    unsigned char *ewd_3g = link_dataref_byte_arr ("AirbusFBW/EWD3gText", 36, -1); if (strlen((char*)ewd_3g) > 36) ewd_3g[36] = '\0';
    unsigned char *ewd_3r = link_dataref_byte_arr ("AirbusFBW/EWD3rText", 36, -1); if (strlen((char*)ewd_3r) > 36) ewd_3r[36] = '\0';
    unsigned char *ewd_3w = link_dataref_byte_arr ("AirbusFBW/EWD3wText", 36, -1); if (strlen((char*)ewd_3w) > 36) ewd_3w[36] = '\0';
    unsigned char *ewd_4a = link_dataref_byte_arr ("AirbusFBW/EWD4aText", 36, -1); if (strlen((char*)ewd_4a) > 36) ewd_4a[36] = '\0';
    unsigned char *ewd_4b = link_dataref_byte_arr ("AirbusFBW/EWD4bText", 36, -1); if (strlen((char*)ewd_4b) > 36) ewd_4b[36] = '\0';
    unsigned char *ewd_4g = link_dataref_byte_arr ("AirbusFBW/EWD4gText", 36, -1); if (strlen((char*)ewd_4g) > 36) ewd_4g[36] = '\0';
    unsigned char *ewd_4r = link_dataref_byte_arr ("AirbusFBW/EWD4rText", 36, -1); if (strlen((char*)ewd_4r) > 36) ewd_4r[36] = '\0';
    unsigned char *ewd_4w = link_dataref_byte_arr ("AirbusFBW/EWD4wText", 36, -1); if (strlen((char*)ewd_4w) > 36) ewd_4w[36] = '\0';
    unsigned char *ewd_5a = link_dataref_byte_arr ("AirbusFBW/EWD5aText", 36, -1); if (strlen((char*)ewd_5a) > 36) ewd_5a[36] = '\0';
    unsigned char *ewd_5b = link_dataref_byte_arr ("AirbusFBW/EWD5bText", 36, -1); if (strlen((char*)ewd_5b) > 36) ewd_5b[36] = '\0';
    unsigned char *ewd_5g = link_dataref_byte_arr ("AirbusFBW/EWD5gText", 36, -1); if (strlen((char*)ewd_5g) > 36) ewd_5g[36] = '\0';
    unsigned char *ewd_5r = link_dataref_byte_arr ("AirbusFBW/EWD5rText", 36, -1); if (strlen((char*)ewd_5r) > 36) ewd_5r[36] = '\0';
    unsigned char *ewd_5w = link_dataref_byte_arr ("AirbusFBW/EWD5wText", 36, -1); if (strlen((char*)ewd_5w) > 36) ewd_5w[36] = '\0';
    unsigned char *ewd_6a = link_dataref_byte_arr ("AirbusFBW/EWD6aText", 36, -1); if (strlen((char*)ewd_6a) > 36) ewd_6a[36] = '\0';
    unsigned char *ewd_6b = link_dataref_byte_arr ("AirbusFBW/EWD6bText", 36, -1); if (strlen((char*)ewd_6b) > 36) ewd_6b[36] = '\0';
    unsigned char *ewd_6g = link_dataref_byte_arr ("AirbusFBW/EWD6gText", 36, -1); if (strlen((char*)ewd_6g) > 36) ewd_6g[36] = '\0';
    unsigned char *ewd_6r = link_dataref_byte_arr ("AirbusFBW/EWD6rText", 36, -1); if (strlen((char*)ewd_6r) > 36) ewd_6r[36] = '\0';
    unsigned char *ewd_6w = link_dataref_byte_arr ("AirbusFBW/EWD6wText", 36, -1); if (strlen((char*)ewd_6w) > 36) ewd_6w[36] = '\0';
    unsigned char *ewd_7a = link_dataref_byte_arr ("AirbusFBW/EWD7aText", 36, -1); if (strlen((char*)ewd_7a) > 36) ewd_7a[36] = '\0';
    unsigned char *ewd_7b = link_dataref_byte_arr ("AirbusFBW/EWD7bText", 36, -1); if (strlen((char*)ewd_7b) > 36) ewd_7b[36] = '\0';
    unsigned char *ewd_7g = link_dataref_byte_arr ("AirbusFBW/EWD7gText", 36, -1); if (strlen((char*)ewd_7g) > 36) ewd_7g[36] = '\0';
    unsigned char *ewd_7r = link_dataref_byte_arr ("AirbusFBW/EWD7rText", 36, -1); if (strlen((char*)ewd_7r) > 36) ewd_7r[36] = '\0';
    unsigned char *ewd_7w = link_dataref_byte_arr ("AirbusFBW/EWD7wText", 36, -1); if (strlen((char*)ewd_7w) > 36) ewd_7w[36] = '\0';

    if (!coldAndDark) {
      glPushMatrix ();

      // The fixed background lines

      glLineWidth (3);
      glColor3ub (COLOR_WHITE);
      glBegin (GL_LINE_STRIP);
        glVertex2d ( 10, 150);
        glVertex2d (270, 150);
      glEnd ();
      glBegin (GL_LINE_STRIP);
        glVertex2d (310, 150);
        glVertex2d (450, 150);
      glEnd ();
      glBegin (GL_LINE_STRIP);
        glVertex2d (290, 140);
        glVertex2d (290,  10);
      glEnd ();

      // Dials and Markers

      int rot = 0;
      m_pFontManager->SetSize (m_Font, 10, 12);

      // Eng 1 EPR
      glColor3ub (COLOR_WHITE);
      aCircle.SetOrigin (160, 400);
      aCircle.SetRadius (55);
      aCircle.SetDegreesPerPoint (5);
      aCircle.SetArcStartEnd (230, 95);
      aCircle.Evaluate ();
      glColor3ub (COLOR_BLACK);
      aCircle.SetRadius (53);
      aCircle.Evaluate ();
      glColor3ub (COLOR_WHITE);
      glLineWidth (2);
      glPushMatrix ();
        glTranslated (160, 400, 0);
        glRotated (-90, 0, 0, 1);
        glBegin (GL_LINES);
          glVertex2d (0, 45);
          glVertex2d (0, 55);
        glEnd ();
        for (rot = 0; rot < 3; rot++) {
          glRotated (70, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2d (0, 45);
            glVertex2d (0, 55);
          glEnd ();
        }
      glPopMatrix ();
      m_pFontManager->Print (175, 394, "1.6", m_Font);
      m_pFontManager->Print (162, 428, "1.4", m_Font);
      m_pFontManager->Print (128, 416, "1.2", m_Font);
      m_pFontManager->Print (126, 374, "1", m_Font);
      glLineWidth (2);
      glBegin (GL_LINE_LOOP);
        glVertex2d (150, 365);
        glVertex2d (150, 390);
        glVertex2d (235, 390);
        glVertex2d (235, 365);
      glEnd ();

      // Eng 1 EGT
      glColor3ub (COLOR_WHITE);
      aCircle.SetOrigin (160, 300);
      aCircle.SetRadius (45);
      aCircle.SetDegreesPerPoint (5);
      aCircle.SetArcStartEnd (270, 60);
      aCircle.Evaluate ();
      glColor3ub (COLOR_BLACK);
      aCircle.SetRadius (43);
      aCircle.Evaluate ();
      glColor3ub (COLOR_RED);
      aCircle.SetRadius (48);
      aCircle.SetArcStartEnd (60, 90);
      aCircle.Evaluate ();
      glColor3ub (COLOR_BLACK);
      aCircle.SetRadius (42);
      aCircle.Evaluate ();
      glColor3ub (COLOR_WHITE);
      glLineWidth (2);
      glPushMatrix ();
        glTranslated (160, 300, 0);
        glBegin (GL_LINES);
          glVertex2d (0, 35);
          glVertex2d (0, 45);
        glEnd ();
        glRotated (90, 0, 0, 1);
        glBegin (GL_LINES);
          glVertex2d (0, 35);
          glVertex2d (0, 45);
        glEnd ();
       glPopMatrix ();

        // Eng 1 N1
        glColor3ub (COLOR_WHITE);
        aCircle.SetOrigin (160, 240);
        aCircle.SetRadius (55);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (230, 60);
        aCircle.Evaluate ();
        glColor3ub (COLOR_BLACK);
        aCircle.SetRadius (53);
        aCircle.Evaluate ();
        glColor3ub (COLOR_RED);
        aCircle.SetRadius (58);
        aCircle.SetArcStartEnd (60, 90);
        aCircle.Evaluate ();
        glColor3ub (COLOR_BLACK);
        aCircle.SetRadius (52);
        aCircle.Evaluate ();
        glColor3ub (COLOR_WHITE);
        glLineWidth (2);
        glPushMatrix ();
          glTranslated (160, 240, 0);
          glRotated (-60, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2d (0, 45);
            glVertex2d (0, 55);
          glEnd ();
          for (rot = 0; rot < 3; rot++) {
            glRotated (60, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2d (0, 45);
              glVertex2d (0, 55);
            glEnd ();
          }
        glPopMatrix ();
        glColor3ub (COLOR_WHITE);
        m_pFontManager->Print (124, 252, "4", m_Font);
        m_pFontManager->Print (180, 252, "10", m_Font);

        // Eng 2 EPR
        glColor3ub (COLOR_WHITE);
        aCircle.SetOrigin (310, 400);
        aCircle.SetRadius (55);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (230, 95);
        aCircle.Evaluate ();
        glColor3ub (COLOR_BLACK);
        aCircle.SetRadius (53);
        aCircle.Evaluate ();
        glColor3ub (COLOR_WHITE);
        glLineWidth (2);
        glPushMatrix ();
          glTranslated (310, 400, 0);
          glRotated (-90, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2d (0, 45);
            glVertex2d (0, 55);
          glEnd ();
          for (rot = 0; rot < 3; rot++) {
            glRotated (70, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2d (0, 45);
              glVertex2d (0, 55);
            glEnd ();
          }
        glPopMatrix ();
        m_pFontManager->Print (325, 394, "1.6", m_Font);
        m_pFontManager->Print (312, 428, "1.4", m_Font);
        m_pFontManager->Print (278, 416, "1.2", m_Font);
        m_pFontManager->Print (276, 374, "1", m_Font);
        glLineWidth (2);
        glBegin (GL_LINE_LOOP);
          glVertex2d (300, 365);
          glVertex2d (300, 390);
          glVertex2d (385, 390);
          glVertex2d (385, 365);
        glEnd ();

        // Eng 2 EGT
        glColor3ub (COLOR_WHITE);
        aCircle.SetOrigin (310, 300);
        aCircle.SetRadius (45);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (270, 90);
        aCircle.Evaluate ();
        glColor3ub (COLOR_BLACK);
        aCircle.SetRadius (43);
        aCircle.Evaluate ();
        glColor3ub (COLOR_RED);
        aCircle.SetRadius (48);
        aCircle.SetArcStartEnd (60, 90);
        aCircle.Evaluate ();
        glColor3ub (COLOR_BLACK);
        aCircle.SetRadius (42);
        aCircle.Evaluate ();
        glColor3ub (COLOR_WHITE);
        glLineWidth (2);
        glPushMatrix ();
          glTranslated (310, 300, 0);
          glBegin (GL_LINES);
            glVertex2d (0, 35);
            glVertex2d (0, 45);
          glEnd ();
          glRotated (90, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2d (0, 35);
            glVertex2d (0, 45);
          glEnd ();
        glPopMatrix ();

        // Eng 2 N1
        glColor3ub (COLOR_WHITE);
        aCircle.SetOrigin (310, 240);
        aCircle.SetRadius (55);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (230, 60);
        aCircle.Evaluate ();
        glColor3ub (COLOR_BLACK);
        aCircle.SetRadius (53);
        aCircle.Evaluate ();
        glColor3ub (COLOR_RED);
        aCircle.SetRadius (58);
        aCircle.SetArcStartEnd (60, 90);
        aCircle.Evaluate ();
        glColor3ub (COLOR_BLACK);
        aCircle.SetRadius (52);
        aCircle.Evaluate ();
        glColor3ub (COLOR_WHITE);
        glLineWidth (2);
        glPushMatrix ();
          glTranslated (310, 240, 0);
          glRotated (-60, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2d (0, 45);
            glVertex2d (0, 55);
          glEnd ();
          for (rot = 0; rot < 3; rot++) {
            glRotated (60, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2d (0, 45);
              glVertex2d (0, 55);
            glEnd ();
          }
        glPopMatrix ();
        glColor3ub (COLOR_WHITE);
        m_pFontManager->Print (274, 252, "4", m_Font);
        m_pFontManager->Print (330, 252, "10", m_Font);

        // Other background stuff

        m_pFontManager->SetSize (m_Font, 12, 12);
        glColor3ub (COLOR_WHITE);
        m_pFontManager->Print (220, 400, "EPR",  m_Font);
        m_pFontManager->Print ( 25, 335, "FF",   m_Font);
        m_pFontManager->Print (222, 335, "EGT",  m_Font);
        m_pFontManager->Print (380, 335, "FF",   m_Font);
        m_pFontManager->Print (227, 270, "N1" ,  m_Font);
        m_pFontManager->Print ( 40, 250, "N2" ,  m_Font);
        m_pFontManager->Print (400, 250, "N2" ,  m_Font);
        m_pFontManager->Print ( 10, 160, "FOB:", m_Font);
        glColor3ub (COLOR_CYAN);
        m_pFontManager->Print ( 55, 335, "KG/H", m_Font);
        m_pFontManager->Print (410, 335, "KG/H", m_Font);
        m_pFontManager->Print (225, 320, "*C",   m_Font);
        m_pFontManager->Print (230, 255, "%",    m_Font);
        m_pFontManager->Print ( 70, 250, "%",    m_Font);
        m_pFontManager->Print (430, 250, "%",    m_Font);
        m_pFontManager->Print (120, 160, "KG",   m_Font);

        // Needles and text fields

        int value = 0;

        glColor3ub (COLOR_GREEN);
        glLineWidth (4);
        // Eng 1 EPR	
        glPushMatrix ();
          glTranslated (160, 400, 0);
          glRotated (120, 0, 0, 1);
          value = (eng1Epr * 1000 - 1000) * 210 / 600;
          if (value < 0) value = 0; if (value > 210) value = 210;
          glRotated (-value, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2d (0,  5);
            glVertex2d (0, 55);
          glEnd ();
        glPopMatrix ();
        // Eng 2 EPR	
        glPushMatrix ();
          glTranslated (310, 400, 0);
          glRotated (120, 0, 0, 1);
          value = (eng2Epr * 1000 - 1000) * 210 / 600;
          if (value < 0) value = 0; if (value > 210) value = 210;
          glRotated (-value, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2d (0,  5);
            glVertex2d (0, 55);
          glEnd ();
        glPopMatrix ();
        // Eng 1 EGT	
        glLineWidth (3);
        glPushMatrix ();
          glTranslated (160, 300, 0);
          glRotated (90, 0, 0, 1);
          value = (eng1Egt * 150 / 700);
          if (value > 150) value = 150;
          glRotated (-value, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2d (0, 25);
            glVertex2d (0, 45);
          glEnd ();
        glPopMatrix ();
        // Eng 2 EGT	
        glPushMatrix ();
          glTranslated (310, 300, 0);
          glRotated (90, 0, 0, 1);
          value = (eng2Egt * 150 / 700);
          if (value > 150) value = 150;
          glRotated (-value, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2d (0, 25);
            glVertex2d (0, 45);
          glEnd ();
        glPopMatrix ();
        // Eng 1 N1	
        glLineWidth (4);
        glPushMatrix ();
          glTranslated (160, 240, 0);
          glRotated (120, 0, 0, 1);
          value = (eng1N1 - 20) * 180 / 80;
          if (value < 0) value = 0; if (value > 210) value = 210;
          glRotated (-value, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2d (0,  0);
            glVertex2d (0, 55);
          glEnd ();
        glPopMatrix ();
        // Eng 2 N1	
        glPushMatrix ();
          glTranslated (310, 240, 0);
          glRotated (120, 0, 0, 1);
          value = (eng2N1 - 20) * 180 / 80;
          if (value < 0) value = 0; if (value > 210) value = 210;
          glRotated (-value, 0, 0, 1);
          glBegin (GL_LINES);
            glVertex2d (0,  0);
            glVertex2d (0, 55);
          glEnd ();
        glPopMatrix ();

        switch (eng1RevSw) {
          case 1: // Eng 1 Reverser in transit
            glColor3ub (COLOR_AMBER);
            glLineWidth (2);
            glBegin (GL_LINE_LOOP);
              glVertex2d (163, 409);
              glVertex2d (163, 423);
              glVertex2d (192, 423);
              glVertex2d (192, 409);
            glEnd ();
            m_pFontManager->SetSize (m_Font, 10, 10);
            m_pFontManager->Print (165, 411, "REV", m_Font);
            break;
          case 2: // Eng 1 Reverser open
            glColor3ub (COLOR_GREEN);
            glLineWidth (2);
            glBegin (GL_LINE_LOOP);
              glVertex2d (163, 409);
              glVertex2d (163, 423);
              glVertex2d (192, 423);
              glVertex2d (192, 409);
            glEnd ();
            m_pFontManager->SetSize (m_Font, 10, 10);
            m_pFontManager->Print (165, 411, "REV", m_Font);
            break;
          default:
            break;
        }

        switch (eng2RevSw) {
          case 1: // Eng 2 Reverser in transit
            glColor3ub (COLOR_AMBER);
            glLineWidth (2);
            glBegin (GL_LINE_LOOP);
              glVertex2d (313, 409);
              glVertex2d (313, 423);
              glVertex2d (342, 423);
              glVertex2d (342, 409);
            glEnd ();
            m_pFontManager->SetSize (m_Font, 10, 10);
            m_pFontManager->Print (315, 411, "REV", m_Font);
            break;
          case 2: // Eng 2 Reverser open
            glColor3ub (COLOR_GREEN);
            glLineWidth (2);
            glBegin (GL_LINE_LOOP);
              glVertex2d (313, 409);
              glVertex2d (313, 423);
              glVertex2d (342, 423);
              glVertex2d (342, 409);
            glEnd ();
          m_pFontManager->SetSize (m_Font, 10, 10);
          m_pFontManager->Print (315, 411, "REV", m_Font);
          break;
        }

        char buffer[32];
        glColor3ub (COLOR_GREEN);
        m_pFontManager->SetSize (m_Font, 16, 16);
        sprintf(buffer, "%1.3f", eng1Epr);
        m_pFontManager->Print (160, 370, buffer, m_Font); // Eng 1 EPR
        sprintf(buffer, "%1.3f", eng2Epr);
        m_pFontManager->Print (310, 370, buffer, m_Font); // Eng 2 EPR
        m_pFontManager->SetSize (m_Font, 14, 14);
        sprintf(buffer, "%4i", (int) eng1FF);
        m_pFontManager->Print ( 40, 303, buffer, m_Font); // Eng 1 FF
        sprintf(buffer, "%4i", (int) eng2FF);
        m_pFontManager->Print (395, 303, buffer, m_Font); // Eng 2 FF
        sprintf(buffer, "%5i", (int) totFOB);
        m_pFontManager->Print ( 55, 160, buffer, m_Font); // FOB
        m_pFontManager->SetSize (m_Font, 12, 12);
        sprintf(buffer, "%4i", (int) eng1Egt);
        m_pFontManager->Print (138, 303, buffer, m_Font); // Eng 1 EGT
        sprintf(buffer, "%4i", (int) eng2Egt);
        m_pFontManager->Print (288, 303, buffer, m_Font); // Eng 2 EGT
        sprintf(buffer, "%4.1f", eng1N1);
        m_pFontManager->Print (180, 220, buffer, m_Font); // Eng 1 N1
        sprintf(buffer, "%4.1f", eng2N1);
        m_pFontManager->Print (330, 220, buffer, m_Font); // Eng 2 N1
        sprintf(buffer, "%4.1f", eng1N2);
        m_pFontManager->Print ( 40, 223, buffer, m_Font); // Eng 1 N2
        sprintf(buffer, "%4.1f", eng2N2);
        m_pFontManager->Print (400, 223, buffer, m_Font); // Eng 2 N2

        // The slats and flaps indicators

        int sFHx = 310,       sFHy =  193; // the slats home position
        int fFHx = sFHx + 10, fFHy = sFHy; // the flaps home position
        float sRPPx = 70 * 0.85; // horizontal and vertical offset units for the slats symbol
        float sRPPy = 70 * 0.30;
        float fRPPx = 90 * 0.95; // horizontal and vertical offset units for the flaps symbol
        float fRPPy = 90 * 0.25;
/*
        glLineWidth (1);
        glColor3ub (COLOR_WHITE);
        glBegin (GL_LINE_LOOP); // box for testing only
          glVertex2d (230, 160);
          glVertex2d (230, 200);
          glVertex2d (440, 200);
          glVertex2d (440, 160);
        glEnd ();
/*
        m_pFontManager->SetSize (m_Font, 12, 12);
        glColor3ub (COLOR_MAGENTA); // displays for testing only
        sprintf(buffer, "% 3i", slatReq);
        m_pFontManager->Print (300, 125, buffer, m_Font);
        sprintf(buffer, "% 3i", flapReq);
        m_pFontManager->Print (370, 125, buffer, m_Font);
        glColor3ub (COLOR_CYAN);
        sprintf(buffer, "% 6.2f", slatReqPos[slatReq]);
        m_pFontManager->Print (300, 105, buffer, m_Font);
        sprintf(buffer, "% 6.2f", flapReqPos[flapReq]);
        m_pFontManager->Print (370, 105, buffer, m_Font);
        glColor3ub (COLOR_GREEN);
        sprintf(buffer, "% 6.2f", slatRatio);
        m_pFontManager->Print (300,  85, buffer, m_Font);
        sprintf(buffer, "% 6.2f", flapRatio);
        m_pFontManager->Print (370,  85, buffer, m_Font);
/**/
        glLineWidth (2);
        glColor3ub (COLOR_WHITE);
        glBegin (GL_LINE_LOOP); // slats/flaps central marker
          glVertex2d (sFHx - 5, sFHy - 10);
          glVertex2d (sFHx    , sFHy     );
          glVertex2d (fFHx    , fFHy     );
          glVertex2d (fFHx + 5, fFHy - 10);
        glEnd ();

        if ((slatRatio > 0.01) || (flapRatio > 0.01)) {
          glColor3ub (COLOR_WHITE);
          aCircle.SetRadius (2);
          aCircle.SetDegreesPerPoint (5);
          aCircle.SetArcStartEnd (0, 360);
          aCircle.SetOrigin (sFHx - 10 - 0.6 * sRPPx, sFHy - 8 - 0.6 * sRPPy); // slats dots
          aCircle.Evaluate ();
          aCircle.SetOrigin (sFHx - 10 - 0.8 * sRPPx, sFHy - 8 - 0.8 * sRPPy);
          aCircle.Evaluate ();
          aCircle.SetOrigin (sFHx - 10 - 1.0 * sRPPx, sFHy - 8 - 1.0 * sRPPy);
          aCircle.Evaluate ();
          m_pFontManager->Print (sFHx - 58, sFHy - 10, "S", m_Font);
          aCircle.SetOrigin (fFHx + 10 + 0.250 * fRPPx, fFHy - 8 - 0.250 * fRPPy); // flaps dots
          aCircle.Evaluate ();
          aCircle.SetOrigin (fFHx + 10 + 0.375 * fRPPx, fFHy - 8 - 0.375 * fRPPy);
          aCircle.Evaluate ();
          aCircle.SetOrigin (fFHx + 10 + 0.500 * fRPPx, fFHy - 8 - 0.500 * fRPPy);
          aCircle.Evaluate ();
          aCircle.SetOrigin (fFHx + 10 + 1.000 * fRPPx, fFHy - 8 - 1.000 * fRPPy);
          aCircle.Evaluate ();
          m_pFontManager->Print (fFHx + 58, fFHy - 10, "F", m_Font);
//      }

        // The Slats - requested position

        float srpx = sFHx - slatReqPos[slatReq] * sRPPx;
        float srpy = sFHy - slatReqPos[slatReq] * sRPPy;

        glLineWidth (2);
        glColor3ub (COLOR_CYAN);
        glBegin (GL_LINES);
          glVertex2f (sFHx, sFHy);
          glVertex2f (srpx, srpy);
        glEnd();
        glBegin (GL_LINE_LOOP);
          glVertex2d (srpx     , srpy     );
          glVertex2d (srpx -  5, srpy - 10);
          glVertex2d (srpx - 20, srpy - 15);
          glVertex2d (srpx - 15, srpy -  5);
        glEnd ();

        // The Slats - actual position

        float sapx = sFHx - slatRatio * sRPPx;
        float sapy = sFHy - slatRatio * sRPPy;

        glLineWidth (2);
        glColor3ub (COLOR_GREEN);
        glBegin (GL_LINES);
          glVertex2f (sFHx, sFHy);
          glVertex2f (sapx, sapy);
        glEnd();
        glBegin (GL_LINE_LOOP);
          glVertex2d (sapx     , sapy     );
          glVertex2d (sapx -  5, sapy - 10);
          glVertex2d (sapx - 20, sapy - 15);
          glVertex2d (sapx - 15, sapy -  5);
        glEnd ();

        // The Flaps - requested position

        float frpx = fFHx + flapReqPos[flapReq] * fRPPx;
        float frpy = fFHy - flapReqPos[flapReq] * fRPPy;

        glLineWidth (2);
        glColor3ub (COLOR_CYAN);
        glBegin (GL_LINES);
          glVertex2f (fFHx, fFHy);
          glVertex2f (frpx, frpy);
        glEnd();
        glBegin (GL_LINE_LOOP);
          glVertex2d (frpx     , frpy     );
          glVertex2d (frpx + 15, frpy -  4);
          glVertex2d (frpx + 20, frpy - 14);
          glVertex2d (frpx +  5, frpy -  9);
        glEnd ();

        // The Flaps - actual position

        float fapx = fFHx + flapRatio * fRPPx;
        float fapy = fFHy - flapRatio * fRPPy;

        glLineWidth (2);
        glColor3ub (COLOR_GREEN);
        glBegin (GL_LINES);
          glVertex2f (fFHx, fFHy);
          glVertex2f (fapx, fapy);
        glEnd();
        glBegin (GL_LINE_LOOP);
          glVertex2d (fapx     , fapy     );
          glVertex2d (fapx + 15, fapy -  4);
          glVertex2d (fapx + 20, fapy - 14);
          glVertex2d (fapx +  5, fapy -  9);
        glEnd ();

        // The Flaps-Handle annunciator

        m_pFontManager->SetSize (m_Font, 12, 12);
        glColor3ub (COLOR_GREEN);
        switch (flapReq) {
          case 0:
            m_pFontManager->Print (310, 160, "0",    m_Font);
            break;
          case 1:
            m_pFontManager->Print (300, 160, "1+F",  m_Font); // "1" only if not on ground!
            break;
          case 2:
            m_pFontManager->Print (310, 160, "2",    m_Font);
            break;
          case 3:
            m_pFontManager->Print (310, 160, "3",    m_Font);
            break;
          case 4:
            m_pFontManager->Print (300, 160, "FULL", m_Font);
            break;
          default:
            break;
        }

      }
/**/
      // Various masks
/**/
      // Eng 1 FADEC Off
      if (eng1FadecState == 0) {
        glColor3ub (COLOR_BLACK);
        glLineWidth (2);
        glBegin (GL_POLYGON);
          glVertex2d (  0, 195);
          glVertex2d (  0, 245);
          glVertex2d (100, 245);
          glVertex2d (100, 195);
        glEnd ();
        glBegin (GL_POLYGON);
          glVertex2d (  0, 296);
          glVertex2d (  0, 325);
          glVertex2d (100, 325);
          glVertex2d (100, 296);
        glEnd ();
        glBegin (GL_POLYGON);
          glVertex2d (100, 195);
          glVertex2d (100, 460);
          glVertex2d (220, 460);
          glVertex2d (220, 195);
        glEnd ();
        glBegin (GL_POLYGON);
          glVertex2d (220, 364);
          glVertex2d (220, 391);
          glVertex2d (236, 391);
          glVertex2d (236, 364);
        glEnd ();

        glColor3ub (COLOR_AMBER);
        aCircle.SetOrigin (160, 400);
        aCircle.SetRadius (55);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (230, 95);
        aCircle.Evaluate ();
        glColor3ub (COLOR_BLACK);
        aCircle.SetRadius (53);
        aCircle.Evaluate ();

        glColor3ub (COLOR_AMBER);
        aCircle.SetOrigin (160, 300);
        aCircle.SetRadius (45);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (270, 90);
        aCircle.Evaluate ();
        glColor3ub (COLOR_BLACK);
        aCircle.SetRadius (43);
        aCircle.Evaluate ();

        glColor3ub (COLOR_AMBER);
        aCircle.SetOrigin (160, 240);
        aCircle.SetRadius (55);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (230, 90);
        aCircle.Evaluate ();
        glColor3ub (COLOR_BLACK);
        aCircle.SetRadius (53);
        aCircle.Evaluate ();

        m_pFontManager->SetSize (m_Font, 14, 14);
        glColor3ub (COLOR_AMBER);
        m_pFontManager->Print (150, 400, "XX", m_Font);
        m_pFontManager->Print (190, 370, "XX", m_Font); // EPR
        m_pFontManager->Print (150, 300, "XX", m_Font); // EGT
        m_pFontManager->Print (190, 220, "XX", m_Font); // N1
        m_pFontManager->Print ( 50, 303, "XX", m_Font); // FF
        m_pFontManager->Print ( 50, 223, "XX", m_Font); // N2

      } // End Eng 1 FADEC Off

      // Eng 1 N1 low
      if (eng1N1a < 3.4) {
        glColor3ub (COLOR_BLACK);
        glLineWidth (2);
        glBegin (GL_POLYGON);
          glVertex2d (100, 195);
          glVertex2d (100, 296);
          glVertex2d (220, 296);
          glVertex2d (220, 195);
        glEnd ();

        glColor3ub (COLOR_AMBER);
        aCircle.SetOrigin (160, 240);
        aCircle.SetRadius (55);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (230, 90);
        aCircle.Evaluate ();
        glColor3ub (COLOR_BLACK);
        aCircle.SetRadius (53);
        aCircle.Evaluate ();

        m_pFontManager->SetSize (m_Font, 14, 14);
        glColor3ub (COLOR_AMBER);
        m_pFontManager->Print (190, 220, "XX", m_Font); // N1

      } // End Eng 1 N1 low

      // Eng 1 N2 low
      if(eng1N2a < 3.4) {
        glColor3ub (COLOR_BLACK);
        glLineWidth (2);
        glBegin (GL_POLYGON);
          glVertex2d (  0, 195);
          glVertex2d (  0, 245);
          glVertex2d (100, 245);
          glVertex2d (100, 195);
        glEnd ();

        m_pFontManager->SetSize (m_Font, 14, 14);
        glColor3ub (COLOR_AMBER);
        m_pFontManager->Print ( 50, 223, "X2",   m_Font);
      } // End Eng 1 N2 low

      // Eng 2 FADEC Off
      if (eng2FadecState == 0) {
        glColor3ub (COLOR_BLACK);
        glLineWidth (2);
        glBegin (GL_POLYGON);
          glVertex2d (370, 195);
          glVertex2d (370, 245);
          glVertex2d (460, 245);
          glVertex2d (460, 195);
        glEnd ();
        glBegin (GL_POLYGON);
          glVertex2d (370, 296);
          glVertex2d (370, 325);
          glVertex2d (460, 325);
          glVertex2d (460, 296);
        glEnd ();
        glBegin (GL_POLYGON);
          glVertex2d (250, 195);
          glVertex2d (250, 460);
          glVertex2d (370, 460);
          glVertex2d (370, 195);
        glEnd ();
        glBegin (GL_POLYGON);
          glVertex2d (370, 364);
          glVertex2d (370, 391);
          glVertex2d (386, 391);
          glVertex2d (386, 364);
        glEnd ();

        glColor3ub (COLOR_AMBER);
        aCircle.SetOrigin (310, 400);
        aCircle.SetRadius (55);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (230, 95);
        aCircle.Evaluate ();
        glColor3ub (COLOR_BLACK);
        aCircle.SetRadius (53);
        aCircle.Evaluate ();

        glColor3ub (COLOR_AMBER);
        aCircle.SetOrigin (310, 300);
        aCircle.SetRadius (45);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (270, 90);
        aCircle.Evaluate ();
        glColor3ub (COLOR_BLACK);
        aCircle.SetRadius (43);
        aCircle.Evaluate ();

        glColor3ub (COLOR_AMBER);
        aCircle.SetOrigin (310, 240);
        aCircle.SetRadius (55);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (230, 90);
        aCircle.Evaluate ();
        glColor3ub (COLOR_BLACK);
        aCircle.SetRadius (53);
        aCircle.Evaluate ();

        m_pFontManager->SetSize (m_Font, 14, 14);
        glColor3ub (COLOR_AMBER);
        m_pFontManager->Print (300, 400, "XX", m_Font);
        m_pFontManager->Print (342, 370, "XX", m_Font); // EPR
        m_pFontManager->Print (300, 300, "XX", m_Font); // EGT
        m_pFontManager->Print (345, 220, "XX", m_Font); // N1
        m_pFontManager->Print (410, 303, "XX", m_Font); // FF
        m_pFontManager->Print (410, 223, "XX", m_Font); // N2
      } // End Eng 2 FADEC Off

      // Eng 2 N1 low
      if (eng2N1a < 3.4) {
        glColor3ub (COLOR_BLACK);
        glLineWidth (2);
        glBegin (GL_POLYGON);
          glVertex2d (250, 195);
          glVertex2d (250, 296);
          glVertex2d (370, 296);
          glVertex2d (370, 195);
        glEnd ();

        glColor3ub (COLOR_AMBER);
        aCircle.SetOrigin (310, 240);
        aCircle.SetRadius (55);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (230, 90);
        aCircle.Evaluate ();
        glColor3ub (COLOR_BLACK);
        aCircle.SetRadius (53);
        aCircle.Evaluate ();

        m_pFontManager->SetSize (m_Font, 14, 14);
        glColor3ub (COLOR_AMBER);
        m_pFontManager->Print (345, 220, "XX", m_Font); // N1
      } // End Eng 2 N1 low

      // Eng 2 N2 low
      if (eng2N2a < 3.4) {
        glColor3ub (COLOR_BLACK);
        glLineWidth (2);
        glBegin (GL_POLYGON);
          glVertex2d (370, 195);
          glVertex2d (370, 245);
          glVertex2d (460, 245);
          glVertex2d (460, 195);
        glEnd ();

        m_pFontManager->SetSize (m_Font, 14, 14);
        glColor3ub (COLOR_AMBER);
        m_pFontManager->Print ( 410, 223, "XX",   m_Font);
      } // End Eng 2 N1 low
/**/
    // The annunciator lines

      m_pFontManager->SetSize (m_Font, 15, 15);
//      glColor3ub (COLOR_GREEN); // test lines only!
//      m_pFontManager->Print (10, 124, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line 1 test
//      m_pFontManager->Print (10, 105, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line 2 test
//      m_pFontManager->Print (10,  86, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line 3 test
//      m_pFontManager->Print (10,  67, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line 4 test
//      m_pFontManager->Print (10,  48, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line 5 test
//      m_pFontManager->Print (10,  29, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line 6 test
//      m_pFontManager->Print (10,  10, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line 7 test

      glColor3ub (COLOR_AMBER);
      m_pFontManager->Print (10, 124, (const char*) ewd_1a, m_Font); // Line 1 amber
      m_pFontManager->Print (10, 105, (const char*) ewd_2a, m_Font); // Line 2 amber
      m_pFontManager->Print (10,  86, (const char*) ewd_3a, m_Font); // Line 3 amber
      m_pFontManager->Print (10,  67, (const char*) ewd_4a, m_Font); // Line 4 amber
      m_pFontManager->Print (10,  48, (const char*) ewd_5a, m_Font); // Line 5 amber
      m_pFontManager->Print (10,  29, (const char*) ewd_6a, m_Font); // Line 6 amber
      m_pFontManager->Print (10,  10, (const char*) ewd_7a, m_Font); // Line 7 amber
      glColor3ub (COLOR_CYAN);
      m_pFontManager->Print (10, 124, (const char*) ewd_1b, m_Font); // Line 1 blue
      m_pFontManager->Print (10, 105, (const char*) ewd_2b, m_Font); // Line 2 blue
      m_pFontManager->Print (10,  86, (const char*) ewd_3b, m_Font); // Line 3 blue
      m_pFontManager->Print (10,  67, (const char*) ewd_4b, m_Font); // Line 4 blue
      m_pFontManager->Print (10,  48, (const char*) ewd_5b, m_Font); // Line 5 blue
      m_pFontManager->Print (10,  29, (const char*) ewd_6b, m_Font); // Line 6 blue
      m_pFontManager->Print (10,  10, (const char*) ewd_7b, m_Font); // Line 7 blue
      glColor3ub (COLOR_GREEN);
      m_pFontManager->Print (10, 124, (const char*) ewd_1g, m_Font); // Line 1 green
      m_pFontManager->Print (10, 105, (const char*) ewd_2g, m_Font); // Line 2 green
      m_pFontManager->Print (10,  86, (const char*) ewd_3g, m_Font); // Line 3 green
      m_pFontManager->Print (10,  67, (const char*) ewd_4g, m_Font); // Line 4 green
      m_pFontManager->Print (10,  48, (const char*) ewd_5g, m_Font); // Line 5 green
      m_pFontManager->Print (10,  29, (const char*) ewd_6g, m_Font); // Line 6 green
      m_pFontManager->Print (10,  10, (const char*) ewd_7g, m_Font); // Line 7 green
      glColor3ub (COLOR_RED);
      m_pFontManager->Print (10, 124, (const char*) ewd_1r, m_Font); // Line 1 red
      m_pFontManager->Print (10, 105, (const char*) ewd_2r, m_Font); // Line 2 red
      m_pFontManager->Print (10,  86, (const char*) ewd_3r, m_Font); // Line 3 red
      m_pFontManager->Print (10,  67, (const char*) ewd_4r, m_Font); // Line 4 red
      m_pFontManager->Print (10,  48, (const char*) ewd_5r, m_Font); // Line 5 red
      m_pFontManager->Print (10,  29, (const char*) ewd_6r, m_Font); // Line 6 red
      m_pFontManager->Print (10,  10, (const char*) ewd_7r, m_Font); // Line 7 red
      glColor3ub (COLOR_WHITE);
      m_pFontManager->Print (10, 124, (const char*) ewd_1w, m_Font); // Line 1 white
      m_pFontManager->Print (10, 105, (const char*) ewd_2w, m_Font); // Line 2 white
      m_pFontManager->Print (10,  86, (const char*) ewd_3w, m_Font); // Line 3 white
      m_pFontManager->Print (10,  67, (const char*) ewd_4w, m_Font); // Line 4 white
      m_pFontManager->Print (10,  48, (const char*) ewd_5w, m_Font); // Line 5 white
      m_pFontManager->Print (10,  29, (const char*) ewd_6w, m_Font); // Line 6 white
      m_pFontManager->Print (10,  10, (const char*) ewd_7w, m_Font); // Line 7 white

      glPopMatrix ();

    } // end if (! coldAndDark)

  } // end Render ()

}

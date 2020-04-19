/*=============================================================================

  This is the ogcA320MCDUWidget.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Multi-Purpose Control / Display Unit Widget ===

  Created:
    Date:        2015-06-14
    Author:      Hans Jansen
    Last change: 2020-02-06

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

#include <math.h>
#include "ogcA320MCDU.h"
#include "ogcA320MCDUWidget.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320MCDUWidget::A320MCDUWidget () {
    if (verbosity > 1) printf ("A320MCDUWidget - constructing\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitMCDU.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 0) printf("A320MCDUWidget - constructed\n");
  }

  A320MCDUWidget::~A320MCDUWidget () {}

  // Draw text from a spec-line
  void A320MCDUWidget::drawSpec (int fWidth, int fHeight, int xPos, int yPos, char* line) {
    char specAmber[25] = "                        ", specCyan[25] = "                        ",
         specGreen[25] = "                        ", specWhite[25] = "                        ";
    for (int i = strlen(line); i >= 0; i--) {
      switch (line[i - 1]) { // AB01CD23E4567
        case 'A':
          specCyan[i - 1] = '[';
          break;
        case 'B':
          specCyan[i - 1] = ']';
          break;
        case '0':
          specCyan[i - 1] = '!';
          break;
        case '1':
          specCyan[i - 1] = '"';
          break;
        case 'C':
          specWhite[i - 1] = '#';
          break;
        case 'D':
          specWhite[i - 1] = '$';
          break;
        case '2':
          specWhite[i - 1] = '!';
          break;
        case '3':
          specWhite[i - 1] = '"';
          break;
        case 'E':
          specAmber[i - 1] = '|';
          break;
        case '4':
          specAmber[i - 1] = '!';
          break;
        case '5':
          specAmber[i - 1] = '"';
          break;
        case '6':
          specGreen[i - 1] = '!';
          break;
        case '7':
          specGreen[i - 1] = '"';
          break;
      } // end switch()
      specAmber[i] = '\0'; specCyan[i] = '\0'; specGreen[i] = '\0'; specWhite[i] = '\0';
      m_pFontManager->SetSize (m_Font, fWidth, fHeight);
      glColor3ub (COLOR_AMBER);
      m_pFontManager->Print (xPos, yPos, specAmber, m_Font);
      glColor3ub (COLOR_CYAN);
      m_pFontManager->Print (xPos, yPos, specCyan,  m_Font);
      glColor3ub (COLOR_GREEN);
      m_pFontManager->Print (xPos, yPos, specGreen, m_Font);
      glColor3ub (COLOR_WHITE);
      m_pFontManager->Print (xPos, yPos, specWhite, m_Font);
    } // end for()
  } // end drawSpec

  void A320MCDUWidget::Render () {

    bool coldAndDark = true;
    CircleEvaluator aCircle;

    GaugeComponent::Render ();

    // The datarefs we want to use on this instrument

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;
    // The selector for left/right MCDU
    // Note: this dataref is defined by the a320_mcdu_keys.c module.
    int *current_mcdu = link_dataref_int ("xpserver/current_mcdu");
    if (*current_mcdu == INT_MISS) *current_mcdu = 0; // value 2 for testing!

//    unsigned char *ewd_1a = link_dataref_byte_arr ("AirbusFBW/EWD1aText", 36, -1); if (strlen ((char*)ewd_1a) > 36) ewd_1a[36] = '\0';
    unsigned char *mcdu1_title_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1titleb", 25, -1);
    unsigned char *mcdu1_title_green = link_dataref_byte_arr ("AirbusFBW/MCDU1titleg", 25, -1);
    unsigned char *mcdu1_title_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1titles", 25, -1);
    unsigned char *mcdu1_title_white = link_dataref_byte_arr ("AirbusFBW/MCDU1titlew", 25, -1);
    unsigned char *mcdu1_title_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1titley", 25, -1);
    unsigned char *mcdu1_stitle_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1stitleb", 25, -1);
    unsigned char *mcdu1_stitle_green = link_dataref_byte_arr ("AirbusFBW/MCDU1stitleg", 25, -1);
    unsigned char *mcdu1_stitle_white = link_dataref_byte_arr ("AirbusFBW/MCDU1stitlew", 25, -1);
    unsigned char *mcdu1_stitle_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1stitley", 25, -1);
    unsigned char *mcdu1_label1_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1label1a", 25, -1);
    unsigned char *mcdu1_label1_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1label1b", 25, -1);
    unsigned char *mcdu1_label1_green = link_dataref_byte_arr ("AirbusFBW/MCDU1label1g", 25, -1);
    unsigned char *mcdu1_label1_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1label1m", 25, -1);
    unsigned char *mcdu1_label1_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1label1s", 25, -1);
    unsigned char *mcdu1_label1_white = link_dataref_byte_arr ("AirbusFBW/MCDU1label1w", 25, -1);
    unsigned char *mcdu1_label1_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1label1y", 25, -1);
    unsigned char *mcdu1_content1_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1cont1a", 25, -1);
    unsigned char *mcdu1_content1_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1cont1b", 25, -1);
    unsigned char *mcdu1_content1_green = link_dataref_byte_arr ("AirbusFBW/MCDU1cont1g", 25, -1);
    unsigned char *mcdu1_content1_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1cont1m", 25, -1);
    unsigned char *mcdu1_content1_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1cont1s", 25, -1);
    unsigned char *mcdu1_content1_white = link_dataref_byte_arr ("AirbusFBW/MCDU1cont1w", 25, -1);
    unsigned char *mcdu1_content1_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1cont1y", 25, -1);
    unsigned char *mcdu1_scontent1_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1scont1a", 25, -1);
    unsigned char *mcdu1_scontent1_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1scont1b", 25, -1);
    unsigned char *mcdu1_scontent1_green = link_dataref_byte_arr ("AirbusFBW/MCDU1scont1g", 25, -1);
    unsigned char *mcdu1_scontent1_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1scont1m", 25, -1);
    unsigned char *mcdu1_scontent1_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1scont1s", 25, -1);
    unsigned char *mcdu1_scontent1_white = link_dataref_byte_arr ("AirbusFBW/MCDU1scont1w", 25, -1);
    unsigned char *mcdu1_scontent1_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1scont1y", 25, -1);
    unsigned char *mcdu1_label2_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1label2a", 25, -1);
    unsigned char *mcdu1_label2_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1label2b", 25, -1);
    unsigned char *mcdu1_label2_green = link_dataref_byte_arr ("AirbusFBW/MCDU1label2g", 25, -1);
    unsigned char *mcdu1_label2_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1label2m", 25, -1);
    unsigned char *mcdu1_label2_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1label2s", 25, -1);
    unsigned char *mcdu1_label2_white = link_dataref_byte_arr ("AirbusFBW/MCDU1label2w", 25, -1);
    unsigned char *mcdu1_label2_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1label2y", 25, -1);
    unsigned char *mcdu1_content2_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1cont2a", 25, -1);
    unsigned char *mcdu1_content2_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1cont2b", 25, -1);
    unsigned char *mcdu1_content2_green = link_dataref_byte_arr ("AirbusFBW/MCDU1cont2g", 25, -1);
    unsigned char *mcdu1_content2_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1cont2m", 25, -1);
    unsigned char *mcdu1_content2_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1cont2s", 25, -1);
    unsigned char *mcdu1_content2_white = link_dataref_byte_arr ("AirbusFBW/MCDU1cont2w", 25, -1);
    unsigned char *mcdu1_content2_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1cont2y", 25, -1);
    unsigned char *mcdu1_scontent2_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1scont2a", 25, -1);
    unsigned char *mcdu1_scontent2_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1scont2b", 25, -1);
    unsigned char *mcdu1_scontent2_green = link_dataref_byte_arr ("AirbusFBW/MCDU1scont2g", 25, -1);
    unsigned char *mcdu1_scontent2_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1scont2m", 25, -1);
    unsigned char *mcdu1_scontent2_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1scont2s", 25, -1);
    unsigned char *mcdu1_scontent2_white = link_dataref_byte_arr ("AirbusFBW/MCDU1scont2w", 25, -1);
    unsigned char *mcdu1_scontent2_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1scont2y", 25, -1);
    unsigned char *mcdu1_label3_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1label3a", 25, -1);
    unsigned char *mcdu1_label3_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1label3b", 25, -1);
    unsigned char *mcdu1_label3_green = link_dataref_byte_arr ("AirbusFBW/MCDU1label3g", 25, -1);
    unsigned char *mcdu1_label3_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1label3m", 25, -1);
    unsigned char *mcdu1_label3_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1label3s", 25, -1);
    unsigned char *mcdu1_label3_white = link_dataref_byte_arr ("AirbusFBW/MCDU1label3w", 25, -1);
    unsigned char *mcdu1_label3_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1label3y", 25, -1);
    unsigned char *mcdu1_content3_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1cont3a", 25, -1);
    unsigned char *mcdu1_content3_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1cont3b", 25, -1);
    unsigned char *mcdu1_content3_green = link_dataref_byte_arr ("AirbusFBW/MCDU1cont3g", 25, -1);
    unsigned char *mcdu1_content3_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1cont3m", 25, -1);
    unsigned char *mcdu1_content3_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1cont3s", 25, -1);
    unsigned char *mcdu1_content3_white = link_dataref_byte_arr ("AirbusFBW/MCDU1cont3w", 25, -1);
    unsigned char *mcdu1_content3_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1cont3y", 25, -1);
    unsigned char *mcdu1_scontent3_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1scont3a", 25, -1);
    unsigned char *mcdu1_scontent3_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1scont3b", 25, -1);
    unsigned char *mcdu1_scontent3_green = link_dataref_byte_arr ("AirbusFBW/MCDU1scont3g", 25, -1);
    unsigned char *mcdu1_scontent3_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1scont3m", 25, -1);
    unsigned char *mcdu1_scontent3_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1scont3s", 25, -1);
    unsigned char *mcdu1_scontent3_white = link_dataref_byte_arr ("AirbusFBW/MCDU1scont3w", 25, -1);
    unsigned char *mcdu1_scontent3_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1scont3y", 25, -1);
    unsigned char *mcdu1_label4_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1label4a", 25, -1);
    unsigned char *mcdu1_label4_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1label4b", 25, -1);
    unsigned char *mcdu1_label4_green = link_dataref_byte_arr ("AirbusFBW/MCDU1label4g", 25, -1);
    unsigned char *mcdu1_label4_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1label4m", 25, -1);
    unsigned char *mcdu1_label4_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1label4s", 25, -1);
    unsigned char *mcdu1_label4_white = link_dataref_byte_arr ("AirbusFBW/MCDU1label4w", 25, -1);
    unsigned char *mcdu1_label4_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1label4y", 25, -1);
    unsigned char *mcdu1_content4_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1cont4a", 25, -1);
    unsigned char *mcdu1_content4_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1cont4b", 25, -1);
    unsigned char *mcdu1_content4_green = link_dataref_byte_arr ("AirbusFBW/MCDU1cont4g", 25, -1);
    unsigned char *mcdu1_content4_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1cont4m", 25, -1);
    unsigned char *mcdu1_content4_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1cont4s", 25, -1);
    unsigned char *mcdu1_content4_white = link_dataref_byte_arr ("AirbusFBW/MCDU1cont4w", 25, -1);
    unsigned char *mcdu1_content4_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1cont4y", 25, -1);
    unsigned char *mcdu1_scontent4_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1scont4a", 25, -1);
    unsigned char *mcdu1_scontent4_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1scont4b", 25, -1);
    unsigned char *mcdu1_scontent4_green = link_dataref_byte_arr ("AirbusFBW/MCDU1scont4g", 25, -1);
    unsigned char *mcdu1_scontent4_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1scont4m", 25, -1);
    unsigned char *mcdu1_scontent4_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1scont4s", 25, -1);
    unsigned char *mcdu1_scontent4_white = link_dataref_byte_arr ("AirbusFBW/MCDU1scont4w", 25, -1);
    unsigned char *mcdu1_scontent4_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1scont4y", 25, -1);
    unsigned char *mcdu1_label5_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1label5a", 25, -1);
    unsigned char *mcdu1_label5_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1label5b", 25, -1);
    unsigned char *mcdu1_label5_green = link_dataref_byte_arr ("AirbusFBW/MCDU1label5g", 25, -1);
    unsigned char *mcdu1_label5_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1label5m", 25, -1);
    unsigned char *mcdu1_label5_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1label5s", 25, -1);
    unsigned char *mcdu1_label5_white = link_dataref_byte_arr ("AirbusFBW/MCDU1label5w", 25, -1);
    unsigned char *mcdu1_label5_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1label5y", 25, -1);
    unsigned char *mcdu1_content5_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1cont5a", 25, -1);
    unsigned char *mcdu1_content5_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1cont5b", 25, -1);
    unsigned char *mcdu1_content5_green = link_dataref_byte_arr ("AirbusFBW/MCDU1cont5g", 25, -1);
    unsigned char *mcdu1_content5_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1cont5m", 25, -1);
    unsigned char *mcdu1_content5_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1cont5s", 25, -1);
    unsigned char *mcdu1_content5_white = link_dataref_byte_arr ("AirbusFBW/MCDU1cont5w", 25, -1);
    unsigned char *mcdu1_content5_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1cont5y", 25, -1);
    unsigned char *mcdu1_scontent5_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1scont5a", 25, -1);
    unsigned char *mcdu1_scontent5_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1scont5b", 25, -1);
    unsigned char *mcdu1_scontent5_green = link_dataref_byte_arr ("AirbusFBW/MCDU1scont5g", 25, -1);
    unsigned char *mcdu1_scontent5_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1scont5m", 25, -1);
    unsigned char *mcdu1_scontent5_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1scont5s", 25, -1);
    unsigned char *mcdu1_scontent5_white = link_dataref_byte_arr ("AirbusFBW/MCDU1scont5w", 25, -1);
    unsigned char *mcdu1_scontent5_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1scont5y", 25, -1);
    unsigned char *mcdu1_label6_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1label6a", 25, -1);
    unsigned char *mcdu1_label6_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1label6b", 25, -1);
    unsigned char *mcdu1_label6_green = link_dataref_byte_arr ("AirbusFBW/MCDU1label6g", 25, -1);
    unsigned char *mcdu1_label6_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1label6m", 25, -1);
    unsigned char *mcdu1_label6_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1label6s", 25, -1);
    unsigned char *mcdu1_label6_white = link_dataref_byte_arr ("AirbusFBW/MCDU1label6w", 25, -1);
    unsigned char *mcdu1_label6_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1label6y", 25, -1);
    unsigned char *mcdu1_content6_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1cont6a", 25, -1);
    unsigned char *mcdu1_content6_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1cont6b", 25, -1);
    unsigned char *mcdu1_content6_green = link_dataref_byte_arr ("AirbusFBW/MCDU1cont6g", 25, -1);
    unsigned char *mcdu1_content6_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1cont6m", 25, -1);
    unsigned char *mcdu1_content6_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1cont6s", 25, -1);
    unsigned char *mcdu1_content6_white = link_dataref_byte_arr ("AirbusFBW/MCDU1cont6w", 25, -1);
    unsigned char *mcdu1_content6_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1cont6y", 25, -1);
    unsigned char *mcdu1_scontent6_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1scont6a", 25, -1);
    unsigned char *mcdu1_scontent6_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU1scont6b", 25, -1);
    unsigned char *mcdu1_scontent6_green = link_dataref_byte_arr ("AirbusFBW/MCDU1scont6g", 25, -1);
    unsigned char *mcdu1_scontent6_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU1scont6m", 25, -1);
    unsigned char *mcdu1_scontent6_spec = link_dataref_byte_arr ("AirbusFBW/MCDU1scont6s", 25, -1);
    unsigned char *mcdu1_scontent6_white = link_dataref_byte_arr ("AirbusFBW/MCDU1scont6w", 25, -1);
    unsigned char *mcdu1_scontent6_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU1scont6y", 25, -1);
    unsigned char *mcdu1_scrpad_amber = link_dataref_byte_arr ("AirbusFBW/MCDU1spa", 25, -1);
    unsigned char *mcdu1_scrpad_white = link_dataref_byte_arr ("AirbusFBW/MCDU1spw", 25, -1);
    unsigned char *mcdu2_title_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2titleb", 25, -1);
    unsigned char *mcdu2_title_green = link_dataref_byte_arr ("AirbusFBW/MCDU2titleg", 25, -1);
    unsigned char *mcdu2_title_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2titles", 25, -1);
    unsigned char *mcdu2_title_white = link_dataref_byte_arr ("AirbusFBW/MCDU2titlew", 25, -1);
    unsigned char *mcdu2_title_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2titley", 25, -1);
    unsigned char *mcdu2_stitle_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2stitleb", 25, -1);
    unsigned char *mcdu2_stitle_green = link_dataref_byte_arr ("AirbusFBW/MCDU2stitleg", 25, -1);
    unsigned char *mcdu2_stitle_white = link_dataref_byte_arr ("AirbusFBW/MCDU2stitlew", 25, -1);
    unsigned char *mcdu2_stitle_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2stitley", 25, -1);
    unsigned char *mcdu2_label1_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2label1a", 25, -1);
    unsigned char *mcdu2_label1_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2label1b", 25, -1);
    unsigned char *mcdu2_label1_green = link_dataref_byte_arr ("AirbusFBW/MCDU2label1g", 25, -1);
    unsigned char *mcdu2_label1_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2label1m", 25, -1);
    unsigned char *mcdu2_label1_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2label1s", 25, -1);
    unsigned char *mcdu2_label1_white = link_dataref_byte_arr ("AirbusFBW/MCDU2label1w", 25, -1);
    unsigned char *mcdu2_label1_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2label1y", 25, -1);
    unsigned char *mcdu2_content1_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2cont1a", 25, -1);
    unsigned char *mcdu2_content1_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2cont1b", 25, -1);
    unsigned char *mcdu2_content1_green = link_dataref_byte_arr ("AirbusFBW/MCDU2cont1g", 25, -1);
    unsigned char *mcdu2_content1_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2cont1m", 25, -1);
    unsigned char *mcdu2_content1_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2cont1s", 25, -1);
    unsigned char *mcdu2_content1_white = link_dataref_byte_arr ("AirbusFBW/MCDU2cont1w", 25, -1);
    unsigned char *mcdu2_content1_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2cont1y", 25, -1);
    unsigned char *mcdu2_scontent1_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2scont1a", 25, -1);
    unsigned char *mcdu2_scontent1_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2scont1b", 25, -1);
    unsigned char *mcdu2_scontent1_green = link_dataref_byte_arr ("AirbusFBW/MCDU2scont1g", 25, -1);
    unsigned char *mcdu2_scontent1_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2scont1m", 25, -1);
    unsigned char *mcdu2_scontent1_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2scont1s", 25, -1);
    unsigned char *mcdu2_scontent1_white = link_dataref_byte_arr ("AirbusFBW/MCDU2scont1w", 25, -1);
    unsigned char *mcdu2_scontent1_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2scont1y", 25, -1);
    unsigned char *mcdu2_label2_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2label2a", 25, -1);
    unsigned char *mcdu2_label2_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2label2b", 25, -1);
    unsigned char *mcdu2_label2_green = link_dataref_byte_arr ("AirbusFBW/MCDU2label2g", 25, -1);
    unsigned char *mcdu2_label2_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2label2m", 25, -1);
    unsigned char *mcdu2_label2_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2label2s", 25, -1);
    unsigned char *mcdu2_label2_white = link_dataref_byte_arr ("AirbusFBW/MCDU2label2w", 25, -1);
    unsigned char *mcdu2_label2_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2label2y", 25, -1);
    unsigned char *mcdu2_content2_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2cont2a", 25, -1);
    unsigned char *mcdu2_content2_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2cont2b", 25, -1);
    unsigned char *mcdu2_content2_green = link_dataref_byte_arr ("AirbusFBW/MCDU2cont2g", 25, -1);
    unsigned char *mcdu2_content2_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2cont2m", 25, -1);
    unsigned char *mcdu2_content2_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2cont2s", 25, -1);
    unsigned char *mcdu2_content2_white = link_dataref_byte_arr ("AirbusFBW/MCDU2cont2w", 25, -1);
    unsigned char *mcdu2_content2_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2cont2y", 25, -1);
    unsigned char *mcdu2_scontent2_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2scont2a", 25, -1);
    unsigned char *mcdu2_scontent2_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2scont2b", 25, -1);
    unsigned char *mcdu2_scontent2_green = link_dataref_byte_arr ("AirbusFBW/MCDU2scont2g", 25, -1);
    unsigned char *mcdu2_scontent2_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2scont2m", 25, -1);
    unsigned char *mcdu2_scontent2_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2scont2s", 25, -1);
    unsigned char *mcdu2_scontent2_white = link_dataref_byte_arr ("AirbusFBW/MCDU2scont2w", 25, -1);
    unsigned char *mcdu2_scontent2_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2scont2y", 25, -1);
    unsigned char *mcdu2_label3_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2label3a", 25, -1);
    unsigned char *mcdu2_label3_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2label3b", 25, -1);
    unsigned char *mcdu2_label3_green = link_dataref_byte_arr ("AirbusFBW/MCDU2label3g", 25, -1);
    unsigned char *mcdu2_label3_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2label3m", 25, -1);
    unsigned char *mcdu2_label3_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2label3s", 25, -1);
    unsigned char *mcdu2_label3_white = link_dataref_byte_arr ("AirbusFBW/MCDU2label3w", 25, -1);
    unsigned char *mcdu2_label3_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2label3y", 25, -1);
    unsigned char *mcdu2_content3_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2cont3a", 25, -1);
    unsigned char *mcdu2_content3_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2cont3b", 25, -1);
    unsigned char *mcdu2_content3_green = link_dataref_byte_arr ("AirbusFBW/MCDU2cont3g", 25, -1);
    unsigned char *mcdu2_content3_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2cont3m", 25, -1);
    unsigned char *mcdu2_content3_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2cont3s", 25, -1);
    unsigned char *mcdu2_content3_white = link_dataref_byte_arr ("AirbusFBW/MCDU2cont3w", 25, -1);
    unsigned char *mcdu2_content3_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2cont3y", 25, -1);
    unsigned char *mcdu2_scontent3_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2scont3a", 25, -1);
    unsigned char *mcdu2_scontent3_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2scont3b", 25, -1);
    unsigned char *mcdu2_scontent3_green = link_dataref_byte_arr ("AirbusFBW/MCDU2scont3g", 25, -1);
    unsigned char *mcdu2_scontent3_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2scont3m", 25, -1);
    unsigned char *mcdu2_scontent3_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2scont3s", 25, -1);
    unsigned char *mcdu2_scontent3_white = link_dataref_byte_arr ("AirbusFBW/MCDU2scont3w", 25, -1);
    unsigned char *mcdu2_scontent3_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2scont3y", 25, -1);
    unsigned char *mcdu2_label4_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2label4a", 25, -1);
    unsigned char *mcdu2_label4_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2label4b", 25, -1);
    unsigned char *mcdu2_label4_green = link_dataref_byte_arr ("AirbusFBW/MCDU2label4g", 25, -1);
    unsigned char *mcdu2_label4_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2label4m", 25, -1);
    unsigned char *mcdu2_label4_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2label4s", 25, -1);
    unsigned char *mcdu2_label4_white = link_dataref_byte_arr ("AirbusFBW/MCDU2label4w", 25, -1);
    unsigned char *mcdu2_label4_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2label4y", 25, -1);
    unsigned char *mcdu2_content4_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2cont4a", 25, -1);
    unsigned char *mcdu2_content4_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2cont4b", 25, -1);
    unsigned char *mcdu2_content4_green = link_dataref_byte_arr ("AirbusFBW/MCDU2cont4g", 25, -1);
    unsigned char *mcdu2_content4_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2cont4m", 25, -1);
    unsigned char *mcdu2_content4_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2cont4s", 25, -1);
    unsigned char *mcdu2_content4_white = link_dataref_byte_arr ("AirbusFBW/MCDU2cont4w", 25, -1);
    unsigned char *mcdu2_content4_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2cont4y", 25, -1);
    unsigned char *mcdu2_scontent4_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2scont4a", 25, -1);
    unsigned char *mcdu2_scontent4_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2scont4b", 25, -1);
    unsigned char *mcdu2_scontent4_green = link_dataref_byte_arr ("AirbusFBW/MCDU2scont4g", 25, -1);
    unsigned char *mcdu2_scontent4_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2scont4m", 25, -1);
    unsigned char *mcdu2_scontent4_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2scont4s", 25, -1);
    unsigned char *mcdu2_scontent4_white = link_dataref_byte_arr ("AirbusFBW/MCDU2scont4w", 25, -1);
    unsigned char *mcdu2_scontent4_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2scont4y", 25, -1);
    unsigned char *mcdu2_label5_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2label5a", 25, -1);
    unsigned char *mcdu2_label5_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2label5b", 25, -1);
    unsigned char *mcdu2_label5_green = link_dataref_byte_arr ("AirbusFBW/MCDU2label5g", 25, -1);
    unsigned char *mcdu2_label5_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2label5m", 25, -1);
    unsigned char *mcdu2_label5_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2label5s", 25, -1);
    unsigned char *mcdu2_label5_white = link_dataref_byte_arr ("AirbusFBW/MCDU2label5w", 25, -1);
    unsigned char *mcdu2_label5_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2label5y", 25, -1);
    unsigned char *mcdu2_content5_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2cont5a", 25, -1);
    unsigned char *mcdu2_content5_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2cont5b", 25, -1);
    unsigned char *mcdu2_content5_green = link_dataref_byte_arr ("AirbusFBW/MCDU2cont5g", 25, -1);
    unsigned char *mcdu2_content5_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2cont5m", 25, -1);
    unsigned char *mcdu2_content5_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2cont5s", 25, -1);
    unsigned char *mcdu2_content5_white = link_dataref_byte_arr ("AirbusFBW/MCDU2cont5w", 25, -1);
    unsigned char *mcdu2_content5_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2cont5y", 25, -1);
    unsigned char *mcdu2_scontent5_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2scont5a", 25, -1);
    unsigned char *mcdu2_scontent5_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2scont5b", 25, -1);
    unsigned char *mcdu2_scontent5_green = link_dataref_byte_arr ("AirbusFBW/MCDU2scont5g", 25, -1);
    unsigned char *mcdu2_scontent5_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2scont5m", 25, -1);
    unsigned char *mcdu2_scontent5_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2scont5s", 25, -1);
    unsigned char *mcdu2_scontent5_white = link_dataref_byte_arr ("AirbusFBW/MCDU2scont5w", 25, -1);
    unsigned char *mcdu2_scontent5_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2scont5y", 25, -1);
    unsigned char *mcdu2_label6_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2label6a", 25, -1);
    unsigned char *mcdu2_label6_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2label6b", 25, -1);
    unsigned char *mcdu2_label6_green = link_dataref_byte_arr ("AirbusFBW/MCDU2label6g", 25, -1);
    unsigned char *mcdu2_label6_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2label6m", 25, -1);
    unsigned char *mcdu2_label6_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2label6s", 25, -1);
    unsigned char *mcdu2_label6_white = link_dataref_byte_arr ("AirbusFBW/MCDU2label6w", 25, -1);
    unsigned char *mcdu2_label6_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2label6y", 25, -1);
    unsigned char *mcdu2_content6_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2cont6a", 25, -1);
    unsigned char *mcdu2_content6_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2cont6b", 25, -1);
    unsigned char *mcdu2_content6_green = link_dataref_byte_arr ("AirbusFBW/MCDU2cont6g", 25, -1);
    unsigned char *mcdu2_content6_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2cont6m", 25, -1);
    unsigned char *mcdu2_content6_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2cont6s", 25, -1);
    unsigned char *mcdu2_content6_white = link_dataref_byte_arr ("AirbusFBW/MCDU2cont6w", 25, -1);
    unsigned char *mcdu2_content6_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2cont6y", 25, -1);
    unsigned char *mcdu2_scontent6_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2scont6a", 25, -1);
    unsigned char *mcdu2_scontent6_cyan = link_dataref_byte_arr ("AirbusFBW/MCDU2scont6b", 25, -1);
    unsigned char *mcdu2_scontent6_green = link_dataref_byte_arr ("AirbusFBW/MCDU2scont6g", 25, -1);
    unsigned char *mcdu2_scontent6_magenta = link_dataref_byte_arr ("AirbusFBW/MCDU2scont6m", 25, -1);
    unsigned char *mcdu2_scontent6_spec = link_dataref_byte_arr ("AirbusFBW/MCDU2scont6s", 25, -1);
    unsigned char *mcdu2_scontent6_white = link_dataref_byte_arr ("AirbusFBW/MCDU2scont6w", 25, -1);
    unsigned char *mcdu2_scontent6_yellow = link_dataref_byte_arr ("AirbusFBW/MCDU2scont6y", 25, -1);
    unsigned char *mcdu2_scrpad_amber = link_dataref_byte_arr ("AirbusFBW/MCDU2spa", 25, -1);
    unsigned char *mcdu2_scrpad_white = link_dataref_byte_arr ("AirbusFBW/MCDU2spw", 25, -1);

      if (verbosity > 2) {
        printf ("A320MCDUWidget - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
        printf ("A320MCDUWidget -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
        printf ("A320MCDUWidget -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
        printf ("A320MCDUWidget -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
      }

    if (!coldAndDark) {

      glPushMatrix ();

      // The text lines

      int fWidth = 38, fLHeight = 30, fSHeight = 24, xPos = 10, // fWidth should be 40 on normal monitor (Sony overdraws!)
          yPosT  = 556, yPosL1 = 514, yPosC1 = 472, yPosL2 = 430, yPosC2 = 388, yPosL3 = 346, yPosC3 = 304,
          yPosL4 = 262, yPosC4 = 220, yPosL5 = 178, yPosC5 = 136, yPosL6 =  94, yPosC6 =  52, yPosS  =  10;

      switch (*current_mcdu) {
        case 2: // testing only...
          glColor3ub (COLOR_WHITE);
          m_pFontManager->SetSize (m_Font, fWidth, fSHeight);
          m_pFontManager->Print (xPos, yPosL1, "[.......label 1........]", m_Font); // Line 1 label test
          m_pFontManager->Print (xPos, yPosL2, "[.......label 2........]", m_Font); // Line 2 label test
//          m_pFontManager->Print (xPos, yPosL3, "[.......label 3........]", m_Font); // Line 3 label test
          m_pFontManager->Print (xPos, yPosL4, "[.......label 4........]", m_Font); // Line 4 label test
          m_pFontManager->Print (xPos, yPosL5, "[.......label 5........]", m_Font); // Line 5 label test
          m_pFontManager->Print (xPos, yPosL6, "[.......label 6........]", m_Font); // Line 6 label test
          m_pFontManager->SetSize (m_Font, fWidth, fLHeight);
          m_pFontManager->Print (xPos, yPosT,  "(........title.........)", m_Font); // Title test
//          m_pFontManager->Print (xPos, yPosC1, "(......contents 1......)", m_Font); // Line 1 contents test
          m_pFontManager->Print (xPos, yPosC2, "(......contents 2......)", m_Font); // Line 2 contents test
          m_pFontManager->Print (xPos, yPosC3, "(......contents 3......)", m_Font); // Line 3 contents test
          m_pFontManager->Print (xPos, yPosC4, "(......contents 4......)", m_Font); // Line 4 contents test
          m_pFontManager->Print (xPos, yPosC5, "(......contents 5......)", m_Font); // Line 5 contents test
          m_pFontManager->Print (xPos, yPosC6, "(......contents 6......)", m_Font); // Line 6 contents test
          m_pFontManager->Print (xPos, yPosS,  "(......scratchpad......)", m_Font); // Scratchpad test
          drawSpec (fWidth, fLHeight, xPos, yPosC1, (char*) "     AB01CD23E4567");
          drawSpec (fWidth, fSHeight, xPos, yPosL3, (char*) "     AB01CD23E4567");
          break;
        case 0: // left MCDU
          // all white lines
          glColor3ub (COLOR_WHITE);
          m_pFontManager->SetSize (m_Font, fWidth, fSHeight);
          m_pFontManager->Print (xPos, yPosT,  (const char*) mcdu1_stitle_white, m_Font); // Small title
          m_pFontManager->Print (xPos, yPosL1, (const char*) mcdu1_label1_white, m_Font); // Line 1 label
          m_pFontManager->Print (xPos, yPosL2, (const char*) mcdu1_label2_white, m_Font); // Line 2 label
          m_pFontManager->Print (xPos, yPosL3, (const char*) mcdu1_label3_white, m_Font); // Line 3 label
          m_pFontManager->Print (xPos, yPosL4, (const char*) mcdu1_label4_white, m_Font); // Line 4 label
          m_pFontManager->Print (xPos, yPosL5, (const char*) mcdu1_label5_white, m_Font); // Line 5 label
          m_pFontManager->Print (xPos, yPosL6, (const char*) mcdu1_label6_white, m_Font); // Line 6 label
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu1_scontent1_white, m_Font); // Line 1 small contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu1_scontent2_white, m_Font); // Line 2 small contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu1_scontent3_white, m_Font); // Line 3 small contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu1_scontent4_white, m_Font); // Line 4 small contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu1_scontent5_white, m_Font); // Line 5 small contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu1_scontent6_white, m_Font); // Line 6 small contents
          m_pFontManager->SetSize (m_Font, fWidth, fLHeight);
          m_pFontManager->Print (xPos, yPosT,  (const char*) mcdu1_title_white, m_Font); // Title
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu1_content1_white, m_Font); // Line 1 contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu1_content2_white, m_Font); // Line 2 contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu1_content3_white, m_Font); // Line 3 contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu1_content4_white, m_Font); // Line 4 contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu1_content5_white, m_Font); // Line 5 contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu1_content6_white, m_Font); // Line 6 contents
          m_pFontManager->Print (xPos, yPosS,  (const char*) mcdu1_scrpad_white, m_Font); // Scratchpad

          // all amber lines
          glColor3ub (COLOR_AMBER);
          m_pFontManager->SetSize (m_Font, fWidth, fSHeight);
          m_pFontManager->Print (xPos, yPosL1, (const char*) mcdu1_label1_amber, m_Font); // Line 1 label
          m_pFontManager->Print (xPos, yPosL2, (const char*) mcdu1_label2_amber, m_Font); // Line 2 label
          m_pFontManager->Print (xPos, yPosL3, (const char*) mcdu1_label3_amber, m_Font); // Line 3 label
          m_pFontManager->Print (xPos, yPosL4, (const char*) mcdu1_label4_amber, m_Font); // Line 4 label
          m_pFontManager->Print (xPos, yPosL5, (const char*) mcdu1_label5_amber, m_Font); // Line 5 label
          m_pFontManager->Print (xPos, yPosL6, (const char*) mcdu1_label6_amber, m_Font); // Line 6 label
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu1_scontent1_amber, m_Font); // Line 1 small contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu1_scontent2_amber, m_Font); // Line 2 small contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu1_scontent3_amber, m_Font); // Line 3 small contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu1_scontent4_amber, m_Font); // Line 4 small contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu1_scontent5_amber, m_Font); // Line 5 small contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu1_scontent6_amber, m_Font); // Line 6 small contents
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu1_content1_amber, m_Font); // Line 1 contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu1_content2_amber, m_Font); // Line 2 contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu1_content3_amber, m_Font); // Line 3 contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu1_content4_amber, m_Font); // Line 4 contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu1_content5_amber, m_Font); // Line 5 contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu1_content6_amber, m_Font); // Line 6 contents
          m_pFontManager->Print (xPos, yPosS,  (const char*) mcdu1_scrpad_amber, m_Font); // Scratchpad

          // all cyan lines
          glColor3ub (COLOR_CYAN);
          m_pFontManager->SetSize (m_Font, fWidth, fSHeight);
          m_pFontManager->Print (xPos, yPosT,  (const char*) mcdu1_stitle_cyan, m_Font); // Small title
          m_pFontManager->Print (xPos, yPosL1, (const char*) mcdu1_label1_cyan, m_Font); // Line 1 label
          m_pFontManager->Print (xPos, yPosL2, (const char*) mcdu1_label2_cyan, m_Font); // Line 2 label
          m_pFontManager->Print (xPos, yPosL3, (const char*) mcdu1_label3_cyan, m_Font); // Line 3 label
          m_pFontManager->Print (xPos, yPosL4, (const char*) mcdu1_label4_cyan, m_Font); // Line 4 label
          m_pFontManager->Print (xPos, yPosL5, (const char*) mcdu1_label5_cyan, m_Font); // Line 5 label
          m_pFontManager->Print (xPos, yPosL6, (const char*) mcdu1_label6_cyan, m_Font); // Line 6 label
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu1_scontent1_cyan, m_Font); // Line 1 small contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu1_scontent2_cyan, m_Font); // Line 2 small contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu1_scontent3_cyan, m_Font); // Line 3 small contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu1_scontent4_cyan, m_Font); // Line 4 small contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu1_scontent5_cyan, m_Font); // Line 5 small contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu1_scontent6_cyan, m_Font); // Line 6 small contents
          m_pFontManager->SetSize (m_Font, fWidth, fLHeight);
          m_pFontManager->Print (xPos, yPosT,  (const char*) mcdu1_title_cyan, m_Font); // Title
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu1_content1_cyan, m_Font); // Line 1 contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu1_content2_cyan, m_Font); // Line 2 contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu1_content3_cyan, m_Font); // Line 3 contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu1_content4_cyan, m_Font); // Line 4 contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu1_content5_cyan, m_Font); // Line 5 contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu1_content6_cyan, m_Font); // Line 6 contents

          // all green lines
          glColor3ub (COLOR_GREEN);
          m_pFontManager->SetSize (m_Font, fWidth, fSHeight);
          m_pFontManager->Print (xPos, yPosT,  (const char*) mcdu1_stitle_green, m_Font); // Small title
          m_pFontManager->Print (xPos, yPosL1, (const char*) mcdu1_label1_green, m_Font); // Line 1 label
          m_pFontManager->Print (xPos, yPosL2, (const char*) mcdu1_label2_green, m_Font); // Line 2 label
          m_pFontManager->Print (xPos, yPosL3, (const char*) mcdu1_label3_green, m_Font); // Line 3 label
          m_pFontManager->Print (xPos, yPosL4, (const char*) mcdu1_label4_green, m_Font); // Line 4 label
          m_pFontManager->Print (xPos, yPosL5, (const char*) mcdu1_label5_green, m_Font); // Line 5 label
          m_pFontManager->Print (xPos, yPosL6, (const char*) mcdu1_label6_green, m_Font); // Line 6 label
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu1_scontent1_green, m_Font); // Line 1 small contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu1_scontent2_green, m_Font); // Line 2 small contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu1_scontent3_green, m_Font); // Line 3 small contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu1_scontent4_green, m_Font); // Line 4 small contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu1_scontent5_green, m_Font); // Line 5 small contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu1_scontent6_green, m_Font); // Line 6 small contents
          m_pFontManager->SetSize (m_Font, fWidth, fLHeight);
          m_pFontManager->Print (xPos, yPosT,  (const char*) mcdu1_title_green, m_Font); // Title
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu1_content1_green, m_Font); // Line 1 contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu1_content2_green, m_Font); // Line 2 contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu1_content3_green, m_Font); // Line 3 contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu1_content4_green, m_Font); // Line 4 contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu1_content5_green, m_Font); // Line 5 contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu1_content6_green, m_Font); // Line 6 contents

          // all magenta lines
          glColor3ub (COLOR_MAGENTA);
          m_pFontManager->SetSize (m_Font, fWidth, fSHeight);
          m_pFontManager->Print (xPos, yPosL1, (const char*) mcdu1_label1_magenta, m_Font); // Line 1 label
          m_pFontManager->Print (xPos, yPosL2, (const char*) mcdu1_label2_magenta, m_Font); // Line 2 label
          m_pFontManager->Print (xPos, yPosL3, (const char*) mcdu1_label3_magenta, m_Font); // Line 3 label
          m_pFontManager->Print (xPos, yPosL4, (const char*) mcdu1_label4_magenta, m_Font); // Line 4 label
          m_pFontManager->Print (xPos, yPosL5, (const char*) mcdu1_label5_magenta, m_Font); // Line 5 label
          m_pFontManager->Print (xPos, yPosL6, (const char*) mcdu1_label6_magenta, m_Font); // Line 6 label
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu1_scontent1_magenta, m_Font); // Line 1 small contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu1_scontent2_magenta, m_Font); // Line 2 small contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu1_scontent3_magenta, m_Font); // Line 3 small contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu1_scontent4_magenta, m_Font); // Line 4 small contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu1_scontent5_magenta, m_Font); // Line 5 small contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu1_scontent6_magenta, m_Font); // Line 6 small contents
          m_pFontManager->SetSize (m_Font, fWidth, fLHeight);
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu1_content1_magenta, m_Font); // Line 1 contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu1_content2_magenta, m_Font); // Line 2 contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu1_content3_magenta, m_Font); // Line 3 contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu1_content4_magenta, m_Font); // Line 4 contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu1_content5_magenta, m_Font); // Line 5 contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu1_content6_magenta, m_Font); // Line 6 contents

          // all yellow lines
          glColor3ub (COLOR_YELLOW);
          m_pFontManager->SetSize (m_Font, fWidth, fSHeight);
          m_pFontManager->Print (xPos, yPosT,  (const char*) mcdu1_stitle_yellow, m_Font); // Small title
          m_pFontManager->Print (xPos, yPosL1, (const char*) mcdu1_label1_yellow, m_Font); // Line 1 label
          m_pFontManager->Print (xPos, yPosL2, (const char*) mcdu1_label2_yellow, m_Font); // Line 2 label
          m_pFontManager->Print (xPos, yPosL3, (const char*) mcdu1_label3_yellow, m_Font); // Line 3 label
          m_pFontManager->Print (xPos, yPosL4, (const char*) mcdu1_label4_yellow, m_Font); // Line 4 label
          m_pFontManager->Print (xPos, yPosL5, (const char*) mcdu1_label5_yellow, m_Font); // Line 5 label
          m_pFontManager->Print (xPos, yPosL6, (const char*) mcdu1_label6_yellow, m_Font); // Line 6 label
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu1_scontent1_yellow, m_Font); // Line 1 small contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu1_scontent2_yellow, m_Font); // Line 2 small contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu1_scontent3_yellow, m_Font); // Line 3 small contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu1_scontent4_yellow, m_Font); // Line 4 small contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu1_scontent5_yellow, m_Font); // Line 5 small contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu1_scontent6_yellow, m_Font); // Line 6 small contents
          m_pFontManager->SetSize (m_Font, fWidth, fLHeight);
          m_pFontManager->Print (xPos, yPosT,  (const char*) mcdu1_title_yellow, m_Font); // Title
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu1_content1_yellow, m_Font); // Line 1 contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu1_content2_yellow, m_Font); // Line 2 contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu1_content3_yellow, m_Font); // Line 3 contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu1_content4_yellow, m_Font); // Line 4 contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu1_content5_yellow, m_Font); // Line 5 contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu1_content6_yellow, m_Font); // Line 6 contents

          // and all special characters
          drawSpec (fWidth, fLHeight, xPos, yPosT,  (char *) mcdu1_title_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosL1, (char *) mcdu1_label1_spec);
          drawSpec (fWidth, fLHeight, xPos, yPosC1, (char *) mcdu1_content1_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosC1, (char *) mcdu1_scontent1_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosL2, (char *) mcdu1_label2_spec);
          drawSpec (fWidth, fLHeight, xPos, yPosC2, (char *) mcdu1_content2_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosC2, (char *) mcdu1_scontent2_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosL3, (char *) mcdu1_label3_spec);
          drawSpec (fWidth, fLHeight, xPos, yPosC3, (char *) mcdu1_content3_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosC3, (char *) mcdu1_scontent3_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosL4, (char *) mcdu1_label4_spec);
          drawSpec (fWidth, fLHeight, xPos, yPosC4, (char *) mcdu1_content4_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosC4, (char *) mcdu1_scontent4_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosL5, (char *) mcdu1_label5_spec);
          drawSpec (fWidth, fLHeight, xPos, yPosC5, (char *) mcdu1_content5_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosC5, (char *) mcdu1_scontent5_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosL6, (char *) mcdu1_label6_spec);
          drawSpec (fWidth, fLHeight, xPos, yPosC6, (char *) mcdu1_content6_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosC6, (char *) mcdu1_scontent6_spec);
          break;
        case 1: // right MCDU
          // all white lines
          glColor3ub (COLOR_WHITE);
          m_pFontManager->SetSize (m_Font, fWidth, fSHeight);
          m_pFontManager->Print (xPos, yPosT,  (const char*) mcdu2_stitle_white, m_Font); // Small title
          m_pFontManager->Print (xPos, yPosL1, (const char*) mcdu2_label1_white, m_Font); // Line 1 label
          m_pFontManager->Print (xPos, yPosL2, (const char*) mcdu2_label2_white, m_Font); // Line 2 label
          m_pFontManager->Print (xPos, yPosL3, (const char*) mcdu2_label3_white, m_Font); // Line 3 label
          m_pFontManager->Print (xPos, yPosL4, (const char*) mcdu2_label4_white, m_Font); // Line 4 label
          m_pFontManager->Print (xPos, yPosL5, (const char*) mcdu2_label5_white, m_Font); // Line 5 label
          m_pFontManager->Print (xPos, yPosL6, (const char*) mcdu2_label6_white, m_Font); // Line 6 label
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu2_scontent1_white, m_Font); // Line 1 small contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu2_scontent2_white, m_Font); // Line 2 small contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu2_scontent3_white, m_Font); // Line 3 small contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu2_scontent4_white, m_Font); // Line 4 small contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu2_scontent5_white, m_Font); // Line 5 small contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu2_scontent6_white, m_Font); // Line 6 small contents
          m_pFontManager->SetSize (m_Font, fWidth, fLHeight);
          m_pFontManager->Print (xPos, yPosT,  (const char*) mcdu2_title_white, m_Font); // Title
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu2_content1_white, m_Font); // Line 1 contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu2_content2_white, m_Font); // Line 2 contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu2_content3_white, m_Font); // Line 3 contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu2_content4_white, m_Font); // Line 4 contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu2_content5_white, m_Font); // Line 5 contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu2_content6_white, m_Font); // Line 6 contents
          m_pFontManager->Print (xPos, yPosS,  (const char*) mcdu2_scrpad_white, m_Font); // Scratchpad

          // all amber lines
          glColor3ub (COLOR_AMBER);
          m_pFontManager->SetSize (m_Font, fWidth, fSHeight);
          m_pFontManager->Print (xPos, yPosL1, (const char*) mcdu2_label1_amber, m_Font); // Line 1 label
          m_pFontManager->Print (xPos, yPosL2, (const char*) mcdu2_label2_amber, m_Font); // Line 2 label
          m_pFontManager->Print (xPos, yPosL3, (const char*) mcdu2_label3_amber, m_Font); // Line 3 label
          m_pFontManager->Print (xPos, yPosL4, (const char*) mcdu2_label4_amber, m_Font); // Line 4 label
          m_pFontManager->Print (xPos, yPosL5, (const char*) mcdu2_label5_amber, m_Font); // Line 5 label
          m_pFontManager->Print (xPos, yPosL6, (const char*) mcdu2_label6_amber, m_Font); // Line 6 label
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu2_scontent1_amber, m_Font); // Line 1 small contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu2_scontent2_amber, m_Font); // Line 2 small contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu2_scontent3_amber, m_Font); // Line 3 small contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu2_scontent4_amber, m_Font); // Line 4 small contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu2_scontent5_amber, m_Font); // Line 5 small contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu2_scontent6_amber, m_Font); // Line 6 small contents
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu2_content1_amber, m_Font); // Line 1 contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu2_content2_amber, m_Font); // Line 2 contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu2_content3_amber, m_Font); // Line 3 contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu2_content4_amber, m_Font); // Line 4 contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu2_content5_amber, m_Font); // Line 5 contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu2_content6_amber, m_Font); // Line 6 contents
          m_pFontManager->Print (xPos, yPosS,  (const char*) mcdu2_scrpad_amber, m_Font); // Scratchpad

          // all cyan lines
          glColor3ub (COLOR_CYAN);
          m_pFontManager->SetSize (m_Font, fWidth, fSHeight);
          m_pFontManager->Print (xPos, yPosT,  (const char*) mcdu2_stitle_cyan, m_Font); // Small title
          m_pFontManager->Print (xPos, yPosL1, (const char*) mcdu2_label1_cyan, m_Font); // Line 1 label
          m_pFontManager->Print (xPos, yPosL2, (const char*) mcdu2_label2_cyan, m_Font); // Line 2 label
          m_pFontManager->Print (xPos, yPosL3, (const char*) mcdu2_label3_cyan, m_Font); // Line 3 label
          m_pFontManager->Print (xPos, yPosL4, (const char*) mcdu2_label4_cyan, m_Font); // Line 4 label
          m_pFontManager->Print (xPos, yPosL5, (const char*) mcdu2_label5_cyan, m_Font); // Line 5 label
          m_pFontManager->Print (xPos, yPosL6, (const char*) mcdu2_label6_cyan, m_Font); // Line 6 label
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu2_scontent1_cyan, m_Font); // Line 1 small contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu2_scontent2_cyan, m_Font); // Line 2 small contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu2_scontent3_cyan, m_Font); // Line 3 small contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu2_scontent4_cyan, m_Font); // Line 4 small contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu2_scontent5_cyan, m_Font); // Line 5 small contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu2_scontent6_cyan, m_Font); // Line 6 small contents
          m_pFontManager->SetSize (m_Font, fWidth, fLHeight);
          m_pFontManager->Print (xPos, yPosT,  (const char*) mcdu2_title_cyan, m_Font); // Title
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu2_content1_cyan, m_Font); // Line 1 contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu2_content2_cyan, m_Font); // Line 2 contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu2_content3_cyan, m_Font); // Line 3 contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu2_content4_cyan, m_Font); // Line 4 contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu2_content5_cyan, m_Font); // Line 5 contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu2_content6_cyan, m_Font); // Line 6 contents

          // all green lines
          glColor3ub (COLOR_GREEN);
          m_pFontManager->SetSize (m_Font, fWidth, fSHeight);
          m_pFontManager->Print (xPos, yPosT,  (const char*) mcdu2_stitle_green, m_Font); // Small title
          m_pFontManager->Print (xPos, yPosL1, (const char*) mcdu2_label1_green, m_Font); // Line 1 label
          m_pFontManager->Print (xPos, yPosL2, (const char*) mcdu2_label2_green, m_Font); // Line 2 label
          m_pFontManager->Print (xPos, yPosL3, (const char*) mcdu2_label3_green, m_Font); // Line 3 label
          m_pFontManager->Print (xPos, yPosL4, (const char*) mcdu2_label4_green, m_Font); // Line 4 label
          m_pFontManager->Print (xPos, yPosL5, (const char*) mcdu2_label5_green, m_Font); // Line 5 label
          m_pFontManager->Print (xPos, yPosL6, (const char*) mcdu2_label6_green, m_Font); // Line 6 label
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu2_scontent1_green, m_Font); // Line 1 small contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu2_scontent2_green, m_Font); // Line 2 small contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu2_scontent3_green, m_Font); // Line 3 small contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu2_scontent4_green, m_Font); // Line 4 small contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu2_scontent5_green, m_Font); // Line 5 small contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu2_scontent6_green, m_Font); // Line 6 small contents
          m_pFontManager->SetSize (m_Font, fWidth, fLHeight);
          m_pFontManager->Print (xPos, yPosT,  (const char*) mcdu2_title_green, m_Font); // Title
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu2_content1_green, m_Font); // Line 1 contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu2_content2_green, m_Font); // Line 2 contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu2_content3_green, m_Font); // Line 3 contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu2_content4_green, m_Font); // Line 4 contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu2_content5_green, m_Font); // Line 5 contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu2_content6_green, m_Font); // Line 6 contents

          // all magenta lines
          glColor3ub (COLOR_MAGENTA);
          m_pFontManager->SetSize (m_Font, fWidth, fSHeight);
          m_pFontManager->Print (xPos, yPosL1, (const char*) mcdu2_label1_magenta, m_Font); // Line 1 label
          m_pFontManager->Print (xPos, yPosL2, (const char*) mcdu2_label2_magenta, m_Font); // Line 2 label
          m_pFontManager->Print (xPos, yPosL3, (const char*) mcdu2_label3_magenta, m_Font); // Line 3 label
          m_pFontManager->Print (xPos, yPosL4, (const char*) mcdu2_label4_magenta, m_Font); // Line 4 label
          m_pFontManager->Print (xPos, yPosL5, (const char*) mcdu2_label5_magenta, m_Font); // Line 5 label
          m_pFontManager->Print (xPos, yPosL6, (const char*) mcdu2_label6_magenta, m_Font); // Line 6 label
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu2_scontent1_magenta, m_Font); // Line 1 small contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu2_scontent2_magenta, m_Font); // Line 2 small contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu2_scontent3_magenta, m_Font); // Line 3 small contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu2_scontent4_magenta, m_Font); // Line 4 small contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu2_scontent5_magenta, m_Font); // Line 5 small contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu2_scontent6_magenta, m_Font); // Line 6 small contents
          m_pFontManager->SetSize (m_Font, fWidth, fLHeight);
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu2_content1_magenta, m_Font); // Line 1 contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu2_content2_magenta, m_Font); // Line 2 contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu2_content3_magenta, m_Font); // Line 3 contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu2_content4_magenta, m_Font); // Line 4 contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu2_content5_magenta, m_Font); // Line 5 contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu2_content6_magenta, m_Font); // Line 6 contents

          // all yellow lines
          glColor3ub (COLOR_YELLOW);
          m_pFontManager->SetSize (m_Font, fWidth, fSHeight);
          m_pFontManager->Print (xPos, yPosT,  (const char*) mcdu2_stitle_yellow, m_Font); // Small title
          m_pFontManager->Print (xPos, yPosL1, (const char*) mcdu2_label1_yellow, m_Font); // Line 1 label
          m_pFontManager->Print (xPos, yPosL2, (const char*) mcdu2_label2_yellow, m_Font); // Line 2 label
          m_pFontManager->Print (xPos, yPosL3, (const char*) mcdu2_label3_yellow, m_Font); // Line 3 label
          m_pFontManager->Print (xPos, yPosL4, (const char*) mcdu2_label4_yellow, m_Font); // Line 4 label
          m_pFontManager->Print (xPos, yPosL5, (const char*) mcdu2_label5_yellow, m_Font); // Line 5 label
          m_pFontManager->Print (xPos, yPosL6, (const char*) mcdu2_label6_yellow, m_Font); // Line 6 label
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu2_scontent1_yellow, m_Font); // Line 1 small contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu2_scontent2_yellow, m_Font); // Line 2 small contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu2_scontent3_yellow, m_Font); // Line 3 small contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu2_scontent4_yellow, m_Font); // Line 4 small contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu2_scontent5_yellow, m_Font); // Line 5 small contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu2_scontent6_yellow, m_Font); // Line 6 small contents
          m_pFontManager->SetSize (m_Font, fWidth, fLHeight);
          m_pFontManager->Print (xPos, yPosT,  (const char*) mcdu2_title_yellow, m_Font); // Title
          m_pFontManager->Print (xPos, yPosC1, (const char*) mcdu2_content1_yellow, m_Font); // Line 1 contents
          m_pFontManager->Print (xPos, yPosC2, (const char*) mcdu2_content2_yellow, m_Font); // Line 2 contents
          m_pFontManager->Print (xPos, yPosC3, (const char*) mcdu2_content3_yellow, m_Font); // Line 3 contents
          m_pFontManager->Print (xPos, yPosC4, (const char*) mcdu2_content4_yellow, m_Font); // Line 4 contents
          m_pFontManager->Print (xPos, yPosC5, (const char*) mcdu2_content5_yellow, m_Font); // Line 5 contents
          m_pFontManager->Print (xPos, yPosC6, (const char*) mcdu2_content6_yellow, m_Font); // Line 6 contents

          // and all special characters
          drawSpec (fWidth, fLHeight, xPos, yPosT,  (char *) mcdu2_title_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosL1, (char *) mcdu2_label1_spec);
          drawSpec (fWidth, fLHeight, xPos, yPosC1, (char *) mcdu2_content1_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosC1, (char *) mcdu2_scontent1_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosL2, (char *) mcdu2_label2_spec);
          drawSpec (fWidth, fLHeight, xPos, yPosC2, (char *) mcdu2_content2_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosC2, (char *) mcdu2_scontent2_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosL3, (char *) mcdu2_label3_spec);
          drawSpec (fWidth, fLHeight, xPos, yPosC3, (char *) mcdu2_content3_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosC3, (char *) mcdu2_scontent3_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosL4, (char *) mcdu2_label4_spec);
          drawSpec (fWidth, fLHeight, xPos, yPosC4, (char *) mcdu2_content4_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosC4, (char *) mcdu2_scontent4_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosL5, (char *) mcdu2_label5_spec);
          drawSpec (fWidth, fLHeight, xPos, yPosC5, (char *) mcdu2_content5_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosC5, (char *) mcdu2_scontent5_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosL6, (char *) mcdu2_label6_spec);
          drawSpec (fWidth, fLHeight, xPos, yPosC6, (char *) mcdu2_content6_spec);
          drawSpec (fWidth, fSHeight, xPos, yPosC6, (char *) mcdu2_scontent6_spec);
          break;
        }

      glPopMatrix ();

    } // end if (! coldAndDark)

  } // end Render()

}

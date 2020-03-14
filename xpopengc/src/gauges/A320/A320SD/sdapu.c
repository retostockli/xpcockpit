/*=============================================================================

  This is the sdapu.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style System Display Widget ===

  This file contains the code for the SD subpage  #6: SD_APU
  (included in the A320SDWidget's Render() function)

  Created:
    Date:        2011-11-14
    Author:      Hans Jansen
    Last change: 2020-02-02

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

/* inlined code: SD APU Page */

          float rotation;

          m_pFontManager->SetSize (m_Font, 14, 14);
          m_pFontManager->Print (212, 440, "APU", m_Font);
          glLineWidth (3);
          glColor3ub (COLOR_WHITE);
          glBegin (GL_LINES); // title underlining
            glVertex2d (212, 435);
            glVertex2d (247, 435);
          glEnd ();
          glBegin (GL_LINE_STRIP); // divider line
            glVertex2d ( 60, 305);
            glVertex2d ( 60, 315);
            glVertex2d (400, 315);
            glVertex2d (400, 305);
          glEnd ();

          // The APU GEN box
          glBegin (GL_LINE_LOOP); // APU GEN box
            glVertex2d ( 75, 325);
            glVertex2d ( 75, 405);
            glVertex2d (150, 405);
            glVertex2d (150, 325);
          glEnd ();
          if (apuMaster == 1) {
            m_pFontManager->SetSize (m_Font, 12, 12);
            if (apuN <= 95) {
              glColor3ub (COLOR_GREEN);
              m_pFontManager->Print ( 80, 368, "  0", m_Font);
              glColor3ub (COLOR_AMBER);
              m_pFontManager->Print ( 80, 349, "  0", m_Font);
              m_pFontManager->Print ( 80, 330, "  0", m_Font);
            } else {
              glColor3ub (COLOR_GREEN);
              m_pFontManager->Print ( 80, 368, "  0", m_Font);
              m_pFontManager->Print ( 80, 349, "115", m_Font);
              m_pFontManager->Print ( 80, 330, "400", m_Font);
              glColor3ub (COLOR_WHITE);
              // when extPwr becomes 0, display 6% (green)
              // (these are not available from the sim, nor from the plane, but % changes with load!)
            }
            m_pFontManager->SetSize (m_Font, 11, 12);
            m_pFontManager->Print ( 78, 388, "APU GEN", m_Font);
          }

          // The BLEED box
          glLineWidth (3);
          glColor3ub (COLOR_WHITE);
          glBegin (GL_LINE_LOOP);
            glVertex2d (325, 325);
            glVertex2d (325, 365);
            glVertex2d (385, 365);
            glVertex2d (385, 325);
          glEnd ();
          if (apuMaster == 1) {
            glColor3ub (COLOR_GREEN);
            m_pFontManager->SetSize (m_Font, 12, 12);
            if (apuN <= 95) {
              m_pFontManager->Print (330, 330, " 0", m_Font);
            } else {
              m_pFontManager->Print (330, 330, "41", m_Font); // does this ever change? value not available from sim...
            }
          }

          // Fixed texts
          m_pFontManager->SetSize (m_Font, 11, 12);
          m_pFontManager->Print (330, 348, "BLEED", m_Font);
          glColor3ub (COLOR_CYAN);
          m_pFontManager->SetSize (m_Font, 10, 12);
          m_pFontManager->Print (130, 368, "%", m_Font);
          m_pFontManager->Print (130, 349, "V", m_Font);
          m_pFontManager->Print (130, 330, "Hz", m_Font);
          m_pFontManager->Print (355, 330, "PSI", m_Font);

          switch (apuBldValve) {
            case 0:
              aCircle.SetOrigin (355, 395);
              aCircle.SetRadius (12);
              aCircle.SetDegreesPerPoint (5);
              aCircle.SetArcStartEnd (0, 360);
              glLineWidth (3);
              glColor3ub (COLOR_GREEN);
              glBegin (GL_LINE_STRIP);
                aCircle.Evaluate ();
              glEnd ();
              glColor3ub (COLOR_GREEN);
              glLineWidth (3);
              glBegin (GL_LINES);
                glVertex2d (345, 395);
                glVertex2d (365, 395);
                glVertex2d (355, 365);
                glVertex2d (355, 385);
              glEnd ();
              break;
            case 1:
              aCircle.SetOrigin (355, 395);
              aCircle.SetRadius (12);
              aCircle.SetDegreesPerPoint (5);
              aCircle.SetArcStartEnd (0, 360);
              glLineWidth (3);
              glColor3ub (COLOR_GREEN);
              glBegin (GL_LINE_STRIP);
                aCircle.Evaluate ();
              glEnd ();
              glColor3ub (COLOR_GREEN);
              glLineWidth (3);
              glBegin (GL_LINES);
                glVertex2d (355, 365);
                glVertex2d (355, 405);
              glEnd ();
              break;
            case 2:
              aCircle.SetOrigin (355, 395);
              aCircle.SetRadius (12);
              aCircle.SetDegreesPerPoint (5);
              aCircle.SetArcStartEnd (0, 360);
              glLineWidth (3);
              glColor3ub (COLOR_AMBER);
              glBegin (GL_LINE_STRIP);
                aCircle.Evaluate ();
              glEnd ();
              glColor3ub (COLOR_AMBER);
              glLineWidth (3);
              glBegin (GL_LINES);
                glVertex2d (345, 395);
                glVertex2d (365, 395);
                glVertex2d (355, 365);
                glVertex2d (355, 385);
              glEnd ();
              break;
            case 3:
              glColor3ub (COLOR_AMBER);
              aCircle.SetOrigin (355, 395);
              aCircle.SetRadius (12);
              aCircle.SetDegreesPerPoint (5);
              aCircle.SetArcStartEnd (0, 360);
              glLineWidth (3);
              glColor3ub (COLOR_AMBER);
              glBegin (GL_LINE_STRIP);
                aCircle.Evaluate ();
              glEnd ();
              glColor3ub (COLOR_AMBER);
              glLineWidth (3);
              glBegin (GL_LINES);
                glVertex2d (355, 365);
                glVertex2d (355, 405);
              glEnd ();
              break;
            }
          if (apuMaster == 1) {
            glColor3ub (COLOR_GREEN);
            m_pFontManager->SetSize (m_Font, 12, 12);
            m_pFontManager->Print (300, 200, "FLAP OPEN", m_Font);
          }

          // APU N dial
          aCircle.SetOrigin (150, 240);
          aCircle.SetRadius (50);
          aCircle.SetDegreesPerPoint (5);
          aCircle.SetArcStartEnd (240, 40);
            glLineWidth (3);
            glColor3ub (COLOR_WHITE);
            glBegin (GL_LINE_STRIP);
              aCircle.Evaluate ();
            glEnd ();
          aCircle.SetRadius (49);
          aCircle.SetArcStartEnd (40, 60);
            glLineWidth (7);
            glColor3ub (COLOR_RED);
            glBegin (GL_LINE_STRIP);
              aCircle.Evaluate ();
            glEnd ();
          aCircle.SetRadius (49);
          aCircle.SetArcStartEnd (43, 57);
            glLineWidth (2);
            glColor3ub (COLOR_BLACK);
            glBegin (GL_LINE_STRIP);
              aCircle.Evaluate ();
            glEnd ();
          glPushMatrix();
            glLineWidth (3);
            glColor3ub (COLOR_WHITE);
            glTranslated (150, 240, 0);
            glRotated (120, 0, 0, 1);
            glBegin (GL_LINE_STRIP);
              glVertex2d (  0, 45);
              glVertex2d (  0, 50);
            glEnd ();
            glRotated (-160, 0, 0, 1);
            glBegin (GL_LINE_STRIP);
              glVertex2d (  0, 50);
              glVertex2d (  0, 60);
            glEnd ();
          glPopMatrix();
          m_pFontManager->SetSize (m_Font, 10, 12);
          m_pFontManager->Print (210, 270, "N", m_Font);
          m_pFontManager->SetSize (m_Font, 8, 8);
          m_pFontManager->Print (115, 215, "0", m_Font);
          m_pFontManager->Print (165, 265, "10", m_Font);
          glColor3ub (COLOR_CYAN);
          m_pFontManager->SetSize (m_Font, 10, 12);
          m_pFontManager->Print (210, 250, "%", m_Font);
          if (apuAvail == 1) {
            glColor3ub (COLOR_GREEN);
            m_pFontManager->SetSize (m_Font, 14, 14);
            m_pFontManager->Print (200, 388, "AVAIL", m_Font);
          }
          // the numeric value
          m_pFontManager->SetSize (m_Font, 12, 12);
          sprintf (buffer, "%3.0f", apuN);
          m_pFontManager->Print (160, 220, buffer, m_Font); // APU N
          // the needle
          glPushMatrix();
            glLineWidth (3);
            glColor3ub (COLOR_GREEN);
            glTranslated (150, 240, 0);
            glRotated (120 - 1.60 * apuN, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2d (  0,  0);
              glVertex2d (  0, 43);
            glEnd ();
          glPopMatrix();

          // APU EGT dial
          aCircle.SetOrigin (150, 140);
          aCircle.SetRadius (50);
          aCircle.SetDegreesPerPoint (5);
          aCircle.SetArcStartEnd (240, 30);
            glLineWidth (3);
            glColor3ub (COLOR_WHITE);
            glBegin (GL_LINE_STRIP);
              aCircle.Evaluate ();
            glEnd ();
          aCircle.SetRadius (49);
          aCircle.SetArcStartEnd (30, 45);
          glLineWidth (7);
          glColor3ub (COLOR_RED);
          glBegin (GL_LINE_STRIP);
            aCircle.Evaluate ();
          glEnd ();
          aCircle.SetRadius (49);
          aCircle.SetArcStartEnd (33, 42);
          glLineWidth (2);
          glColor3ub (COLOR_BLACK);
          glBegin (GL_LINE_STRIP);
            aCircle.Evaluate ();
          glEnd ();
          glPushMatrix();
            glLineWidth (3);
            glColor3ub (COLOR_WHITE);
            glTranslated (150, 140, 0);
            glRotated (120, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2d (  0, 45);
              glVertex2d (  0, 50);
            glEnd ();
            glRotated (-45, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2d (  0, 45);
              glVertex2d (  0, 50);
            glEnd ();
            glRotated (-35, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2d (  0, 45);
              glVertex2d (  0, 50);
            glEnd ();
            glRotated (-35, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2d (  0, 45);
              glVertex2d (  0, 50);
            glEnd ();
            glRotated (-35, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2d (  0, 45);
              glVertex2d (  0, 50);
            glEnd ();
            glRotated (-15, 0, 0, 1);
            glColor3ub (COLOR_RED);
            glBegin (GL_LINES);
              glVertex2d (  0, 45);
              glVertex2d (  0, 50);
            glEnd ();
          glPopMatrix();
          glColor3ub (COLOR_WHITE);
          m_pFontManager->Print (210, 170, "EGT", m_Font);
          m_pFontManager->SetSize (m_Font, 8, 8);
          m_pFontManager->Print (115, 115, "3", m_Font);
          m_pFontManager->Print (120, 160, "7", m_Font);
          m_pFontManager->Print (165, 165, "10", m_Font);
          glColor3ub (COLOR_CYAN);
          m_pFontManager->SetSize (m_Font, 10, 12);
          m_pFontManager->Print (210, 150, ";", m_Font);
          // the numeric value
          m_pFontManager->SetSize (m_Font, 12, 12);
          sprintf (buffer, "%4.0f", ((int) (apuEgt / 10)) * 10.0);
          m_pFontManager->Print (160, 120, buffer, m_Font); // APU EGT
          // the needle (NOTE: non-linear scale!)
          glPushMatrix();
          rotation = 120;
            glLineWidth (3);
            glColor3ub (COLOR_GREEN);
            glTranslated (150, 140, 0);
            if (apuEgt > 300) {
              if (apuEgt < 500) {
                rotation -= ((apuEgt -  300) * 0.225);
              } else if (apuEgt <  700) {
                rotation -=  45 + ((apuEgt -  500) * 0.175);
              } else if (apuEgt <  900) {
                rotation -=  80 + ((apuEgt -  700) * 0.175);
              } else if (apuEgt < 1000) {
                rotation -= 115 + ((apuEgt -  900) * 0.350);
              } else if (apuEgt <= 1200) {
                rotation -= 150 + ((apuEgt - 1000) * 0.075);
              } else rotation -= 165;
            }
            glRotated (rotation, 0, 0, 1);
            glBegin (GL_LINES);
              glVertex2d (  0, 0);
              glVertex2d (  0, 43);
            glEnd ();
          glPopMatrix();

/* end of inlined code */


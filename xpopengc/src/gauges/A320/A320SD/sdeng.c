/*=============================================================================

  This is the sdeng.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style System Display Widget ===

  This file contains the code for the SD subpage  #0: SD_ENG
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

/* inlined code: SD ENG Page */

	m_pFontManager->SetSize(m_Font, 14, 14);
	m_pFontManager->Print ( 35, 440, "ENGINE", m_Font);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES); // title underlining
	  glVertex2d ( 35, 435);
	  glVertex2d (105, 435);
	glEnd ();
	// Fixed texts and lines
	m_pFontManager->SetSize(m_Font, 14, 14);
	m_pFontManager->Print (198, 402, "F.USED", m_Font);
	m_pFontManager->Print (212, 358, "OIL", m_Font);
	m_pFontManager->Print (212, 206, "VIB", m_Font);
	m_pFontManager->Print (218, 186, "N1", m_Font);
	m_pFontManager->Print (218, 166, "N2", m_Font);
//	m_pFontManager->Print (212, 130, "NAC", m_Font);
	glColor3ub (COLOR_CYAN);
	m_pFontManager->Print (218, 384, "KG", m_Font);
	m_pFontManager->Print (218, 330, "QT", m_Font);
	m_pFontManager->Print (212, 274, "PSI", m_Font);
	m_pFontManager->Print (222, 250, ";", m_Font);
//	m_pFontManager->Print (222, 110, ";", m_Font);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES);
	  glVertex2d (180, 390);
	  glVertex2d (210, 390);
	  glVertex2d (250, 390);
	  glVertex2d (280, 390);
	  glVertex2d (180, 256);
	  glVertex2d (210, 256);
	  glVertex2d (250, 256);
	  glVertex2d (280, 256);
	  glVertex2d (180, 192);
	  glVertex2d (210, 192);
	  glVertex2d (250, 192);
	  glVertex2d (280, 192);
	  glVertex2d (180, 172);
	  glVertex2d (210, 172);
	  glVertex2d (250, 172);
	  glVertex2d (280, 172);
//	  glVertex2d (190, 126);
//	  glVertex2d (210, 126);
//	  glVertex2d (250, 126);
//	  glVertex2d (270, 126);
	glEnd ();
	// "permanent" data
	eng1FuelUsed = 0; // NOTE: Fuel_Used data not available in sim or plane!
	if (eng1FuelUsed < 100000) pPos1 = 111; if (eng2FuelUsed < 100000) pPos2 = 291;
	if (eng1FuelUsed < 10000)  pPos1 = 106; if (eng2FuelUsed < 10000)  pPos2 = 286;
	if (eng1FuelUsed < 1000)   pPos1 = 100; if (eng2FuelUsed < 1000)   pPos2 = 280;
	if (eng1FuelUsed < 100)    pPos1 =  95; if (eng2FuelUsed < 100)    pPos2 = 275;
	if (eng1FuelUsed < 10)     pPos1 =  89; if (eng2FuelUsed < 10)     pPos2 = 269;
	glColor3ub (COLOR_GREEN);
	m_pFontManager->SetSize(m_Font, 14, 14);
	sprintf(buffer, "%5.0f", eng1FuelUsed);
	m_pFontManager->Print (pPos1, 384, buffer, m_Font); // Eng 1 used fuel
	sprintf(buffer, "%5.0f", eng2FuelUsed);
	m_pFontManager->Print (pPos2, 384, buffer, m_Font); // Eng 2 used fuel
	if (eng1FadecNoPower >= 1) {
	  // paint eng1OilQty
	  aCircle.SetOrigin (140, 330); // Eng 1 Oil Qty
	  aCircle.SetDegreesPerPoint (5);
	  aCircle.SetRadius (42);
	  aCircle.SetArcStartEnd (-90, 90);
	  glLineWidth (3);
	  glColor3ub (COLOR_WHITE);
	  glBegin(GL_LINE_STRIP);
	    aCircle.Evaluate ();
	  glEnd();
	  glLineWidth (3);
	  glColor3ub (COLOR_WHITE);
	  glBegin (GL_LINES);
	    glVertex2d ( 98, 330);
	    glVertex2d (103, 330);
	    glVertex2d (140, 372);
	    glVertex2d (140, 367);
	    glVertex2d (177, 330);
	    glVertex2d (182, 330);
	  glEnd ();
	  // the needle
	  glPushMatrix();
	    glLineWidth (3);
	    glColor3ub (COLOR_GREEN);
	    glTranslated (140, 330, 0);
	    glRotated (90 - 9 * eng1OilQty, 0, 0, 1);
	    glBegin (GL_LINES);
	      glVertex2d (  0, 0);
	      glVertex2d (  0, 45);
	    glEnd ();
		glColor3ub (COLOR_BLACK);
	    glBegin (GL_LINES);
	      glVertex2d (  0, 0);
	      glVertex2d (  0, 25);
	    glEnd ();
	  glPopMatrix();
	  // the numeric value
	  glColor3ub (COLOR_GREEN);
	  m_pFontManager->SetSize(m_Font, 14, 14);
	  sprintf(buffer, "%4.1f", eng1OilQty);
	  m_pFontManager->Print (117, 330, buffer, m_Font);
	  // paint eng1OilPressure
	  aCircle.SetOrigin (140, 274); // Eng 1 Oil Press
	  aCircle.SetDegreesPerPoint (5);
	  aCircle.SetRadius (41);
	  aCircle.SetArcStartEnd (-90, -64);
	  glLineWidth (7);
	  glColor3ub (COLOR_RED);
	  glBegin(GL_LINE_STRIP);
		aCircle.Evaluate ();
	  glEnd();
	  aCircle.SetRadius (41);
	  aCircle.SetArcStartEnd (-93, -61);
	  glLineWidth (2);
	  glColor3ub (COLOR_BLACK);
	  glBegin(GL_LINE_STRIP);
		aCircle.Evaluate ();
	  glEnd();
	  aCircle.SetRadius (42);
	  aCircle.SetArcStartEnd (-65, 90);
	  glLineWidth (3);
	  glColor3ub (COLOR_WHITE);
	  glBegin(GL_LINE_STRIP);
		aCircle.Evaluate ();
	  glEnd();
	  glLineWidth (3);
	  glColor3ub (COLOR_WHITE);
	  glBegin (GL_LINES);
	    glVertex2d (140, 316);
	    glVertex2d (140, 311);
	    glVertex2d (177, 274);
	    glVertex2d (182, 274);
	  glEnd ();
	  // the needle
	  // Color of needle and of numeric indicator:
	    // if value < 60 : RED
	    // if value < 80 : AMBER
	    // if value >= 80 : GREEN
	  // if value == 200 needle vertical!
	  glPushMatrix();
	    glLineWidth (3);
	    if (eng1OilPressure > 80) glColor3ub (COLOR_GREEN);
	    else if (eng1OilPressure > 60) glColor3ub (COLOR_AMBER);
	    else glColor3ub (COLOR_RED);
	    glTranslated (140, 274, 0);
	    glRotated (90 - 0.45 * eng1OilPressure, 0, 0, 1); // (0.45 == 180 / 400)
	    glBegin (GL_LINES);
	      glVertex2d (  0, 0);
	      glVertex2d (  0, 45);
	    glEnd ();
		glColor3ub (COLOR_BLACK);
	    glBegin (GL_LINES);
	      glVertex2d (  0, 0);
	      glVertex2d (  0, 25);
	    glEnd ();
	  glPopMatrix();
	  // the numeric value
	  if (eng1OilPressure > 80) glColor3ub (COLOR_GREEN);
	  else if (eng1OilPressure > 60) glColor3ub (COLOR_AMBER);
	  else glColor3ub (COLOR_RED);
	  m_pFontManager->SetSize(m_Font, 14, 14);
	  sprintf(buffer, "%4.0f", eng1OilPressure);
	  m_pFontManager->Print (112, 274, buffer, m_Font);
	  // the "numeric only" indicators
	  glColor3ub (COLOR_GREEN);
	  sprintf(buffer, "%4.0f", eng1OilTemp);
	  m_pFontManager->Print (112, 250, buffer, m_Font);
//	  sprintf(buffer, "%4.1f", eng1N1Vib); // NOTE: not available in sim or plane!
	  sprintf(buffer, "%4.1f", 0.0);
	  m_pFontManager->Print (112, 186, buffer, m_Font);
//	  sprintf(buffer, "%4.1f", eng1N2Vib); // NOTE: not available in sim or plane!
	  sprintf(buffer, "%4.1f", 0.0);
	  m_pFontManager->Print (112, 166, buffer, m_Font);
	} else { // (! eng1FadecNoPower >= 1) 
	  // paint eng1FADECNoPower (image SD_ENG_NoFADECPower)
	  // paint eng1OilQty mask
	  glLineWidth (3);
	  aCircle.SetOrigin (140, 330); // Eng 1 Oil Qty
	  aCircle.SetDegreesPerPoint (5);
	  aCircle.SetRadius (42);
	  aCircle.SetArcStartEnd (-90, 90);
	  glColor3ub (COLOR_AMBER);
	  glBegin(GL_LINE_STRIP);
		aCircle.Evaluate ();
	  glEnd();
	  // paint eng1OilPressure mask
	  aCircle.SetOrigin (140, 274); // Eng 1 Oil Press
	  aCircle.SetDegreesPerPoint (5);
	  aCircle.SetRadius (42);
	  aCircle.SetArcStartEnd (-90, 90);
	  glLineWidth (3);
	  glColor3ub (COLOR_AMBER);
	  glBegin(GL_LINE_STRIP);
		aCircle.Evaluate ();
	  glEnd();
	  // paint eng1NacTemp mask
	  aCircle.SetOrigin (140, 110); // Eng 1 Nac Tmp
	  aCircle.SetDegreesPerPoint (5);
	  aCircle.SetRadius (42);
	  aCircle.SetArcStartEnd (-90, 70);
	  glLineWidth (3);
	  glColor3ub (COLOR_AMBER);
	  glBegin(GL_LINE_STRIP);
		aCircle.Evaluate ();
	  glEnd();
	  glColor3ub (COLOR_AMBER);
	  m_pFontManager->SetSize(m_Font, 14, 14);
	  m_pFontManager->Print (129, 330, "XX", m_Font);
	  m_pFontManager->Print (129, 274, "XX", m_Font);
	  m_pFontManager->Print (129, 250, "XX", m_Font);
	  m_pFontManager->Print (129, 186, "XX", m_Font);
	  m_pFontManager->Print (129, 166, "XX", m_Font);
	  m_pFontManager->Print (129, 110, "XX", m_Font);
	} // end (eng1FadecNoPower >= 1)
	if (eng2FadecNoPower >= 1) {
	  // paint eng2OilQty
	  aCircle.SetOrigin (320, 330); // Eng 2 Oil Qty
	  aCircle.SetDegreesPerPoint (5);
	  aCircle.SetRadius (42);
	  aCircle.SetArcStartEnd (-90, 90);
	  glLineWidth (3);
	  glColor3ub (COLOR_WHITE);
	  glBegin(GL_LINE_STRIP);
		aCircle.Evaluate ();
	  glEnd();
	  glColor3ub (COLOR_WHITE);
	  glLineWidth (3);
	  glBegin (GL_LINES);
	    glVertex2d (278, 330);
	    glVertex2d (283, 330);
	    glVertex2d (320, 372);
	    glVertex2d (320, 367);
	    glVertex2d (357, 330);
	    glVertex2d (362, 330);
	  glEnd ();
	  // the needle
	  glPushMatrix();
	    glLineWidth (3);
	    glColor3ub (COLOR_GREEN);
	    glTranslated (320, 330, 0);
	    glRotated (90 - 9 * eng2OilQty, 0, 0, 1);
	    glBegin (GL_LINES);
	      glVertex2d (  0, 0);
	      glVertex2d (  0, 45);
	    glEnd ();
		glColor3ub (COLOR_BLACK);
	    glBegin (GL_LINES);
	      glVertex2d (  0, 0);
	      glVertex2d (  0, 25);
	    glEnd ();
	  glPopMatrix();
	  // the numeric value
	  glColor3ub (COLOR_GREEN);
	  m_pFontManager->SetSize(m_Font, 14, 14);
	  sprintf(buffer, "%4.1f", eng2OilQty);
	  m_pFontManager->Print (297, 330, buffer, m_Font);
	  // paint eng2OilPressure
	  aCircle.SetOrigin (320, 274); // Eng 2 Oil Press
	  aCircle.SetDegreesPerPoint (5);
	  aCircle.SetRadius (41);
	  aCircle.SetArcStartEnd (-90, -64);
	  glLineWidth (7);
	  glColor3ub (COLOR_RED);
	  glBegin(GL_LINE_STRIP);
		aCircle.Evaluate ();
	  glEnd();
	  aCircle.SetRadius (41);
	  aCircle.SetArcStartEnd (-93, -61);
	  glLineWidth (2);
	  glColor3ub (COLOR_BLACK);
	  glBegin(GL_LINE_STRIP);
		aCircle.Evaluate ();
	  glEnd();
	  aCircle.SetRadius (42);
	  aCircle.SetArcStartEnd (-65, 90);
	  glLineWidth (3);
	  glColor3ub (COLOR_WHITE);
	  glBegin(GL_LINE_STRIP);
		aCircle.Evaluate ();
	  glEnd();
	  glLineWidth (3);
	  glColor3ub (COLOR_WHITE);
	  glBegin (GL_LINES);
	    glVertex2d (320, 316);
	    glVertex2d (320, 311);
	    glVertex2d (362, 274);
	    glVertex2d (357, 274);
	  glEnd ();
	  // the needle
	  glPushMatrix();
	    glLineWidth (3);
	    if (eng2OilPressure > 80) glColor3ub (COLOR_GREEN);
	    else if (eng2OilPressure > 60) glColor3ub (COLOR_AMBER);
	    else glColor3ub (COLOR_RED);
	    glTranslated (320, 274, 0);
	    glRotated (90 - 0.45 * eng2OilPressure, 0, 0, 1);
	    glBegin (GL_LINES);
	      glVertex2d (  0, 0);
	      glVertex2d (  0, 45);
	    glEnd ();
		glColor3ub (COLOR_BLACK);
	    glBegin (GL_LINES);
	      glVertex2d (  0, 0);
	      glVertex2d (  0, 25);
	    glEnd ();
	  glPopMatrix();
	  // the numeric value
	  if (eng2OilPressure > 80) glColor3ub (COLOR_GREEN);
	  else if (eng2OilPressure > 60) glColor3ub (COLOR_AMBER);
	  else glColor3ub (COLOR_RED);
	  m_pFontManager->SetSize(m_Font, 14, 14);
	  sprintf(buffer, "%4.0f", eng2OilPressure);
	  m_pFontManager->Print (292, 274, buffer, m_Font);
	  // the "numeric only" indicators
	  glColor3ub (COLOR_GREEN);
	  sprintf(buffer, "%4.0f", eng2OilTemp);
	  m_pFontManager->Print (292, 250, buffer, m_Font);
//	  sprintf(buffer, "%4.1f", eng2N1Vib); // NOTE: not available in sim or plane!
	  sprintf(buffer, "%4.1f", 0.0);
	  m_pFontManager->Print (292, 186, buffer, m_Font);
//	  sprintf(buffer, "%4.1f", eng2N2Vib); // NOTE: not available in sim or plane!
	  sprintf(buffer, "%4.1f", 0.0);
	  m_pFontManager->Print (292, 166, buffer, m_Font);
	} else { // (! eng2FadecNoPower >= 1)
	  // paint eng2FADECNoPower (image SD_ENG_NoFADECPower)
	  // paint eng2OilQty mask
	  aCircle.SetOrigin (320, 330); // Eng 2 Oil Qty
	  aCircle.SetDegreesPerPoint (5);
	  aCircle.SetRadius (42);
	  aCircle.SetArcStartEnd (-90, 90);
	  glLineWidth (3);
	  glColor3ub (COLOR_AMBER);
	  glBegin(GL_LINE_STRIP);
		aCircle.Evaluate ();
	  glEnd();
	  // paint eng2OilPressure mask
	  aCircle.SetOrigin (320, 274); // Eng 2 Oil Press
	  aCircle.SetDegreesPerPoint (5);
	  aCircle.SetRadius (42);
	  aCircle.SetArcStartEnd (-90, 90);
	  glLineWidth (3);
	  glColor3ub (COLOR_AMBER);
	  glBegin(GL_LINE_STRIP);
	    aCircle.Evaluate ();
	  glEnd();
	  // paint eng2NacTemp mask
	  aCircle.SetOrigin (320, 110); // Eng 2 Nac Tmp
	  aCircle.SetDegreesPerPoint (5);
	  aCircle.SetRadius (42);
	  aCircle.SetArcStartEnd (-90, 70);
	  glLineWidth (3);
	  glColor3ub (COLOR_AMBER);
	  glBegin(GL_LINE_STRIP);
	    aCircle.Evaluate ();
	  glEnd();
	  glColor3ub (COLOR_AMBER);
	  m_pFontManager->SetSize(m_Font, 14, 14);
	  m_pFontManager->Print (309, 330, "XX", m_Font);
	  m_pFontManager->Print (309, 274, "XX", m_Font);
	  m_pFontManager->Print (309, 250, "XX", m_Font);
	  m_pFontManager->Print (309, 186, "XX", m_Font);
	  m_pFontManager->Print (309, 166, "XX", m_Font);
	  m_pFontManager->Print (309, 110, "XX", m_Font);
	} // end (eng2FadecNoPower >= 1)
	if (engStartMode <= 0) {
	  // paint startBkgnd image-1 (image SD_ENG_StartBkgnd)
	  glLineWidth (3);
	  glColor3ub (COLOR_WHITE);
	  glBegin (GL_LINES);
	    glVertex2d (190, 126);
	    glVertex2d (210, 126);
	    glVertex2d (250, 126);
	    glVertex2d (270, 126);
	  glEnd ();
	  m_pFontManager->Print (212, 130, "NAC", m_Font);
	  glColor3ub (COLOR_CYAN);
	  m_pFontManager->Print (222, 110, ";", m_Font);
	  if (eng1FadecNoPower >= 1) {
	    // paint eng1NacTemp
	    aCircle.SetOrigin (140, 110); // Eng 1 Nac Tmp
	    aCircle.SetDegreesPerPoint (5);
	    aCircle.SetRadius (42);
	    aCircle.SetArcStartEnd (-90, 70);
	  glLineWidth (3);
	  glColor3ub (COLOR_WHITE);
	  glBegin(GL_LINE_STRIP);
	    aCircle.Evaluate ();
	  glEnd();
	    glLineWidth (3);
	    glColor3ub (COLOR_WHITE);
	    glBegin (GL_LINES);
	      glVertex2d ( 98, 110);
	      glVertex2d (103, 110);
	      glVertex2d (140, 152);
	      glVertex2d (140, 147);
	      glVertex2d (179, 125);
	      glVertex2d (175, 123);
	    glEnd ();
	    // the needle
	    // if value < 100 no indication;
	    // if value == 300 needle vertical
	    // max value ca. 450
	    glPushMatrix();
	      glLineWidth (3);
	      glColor3ub (COLOR_GREEN);
	      glTranslated (140, 110, 0);
	      if (eng1NacTemp > 100) {
	        glRotated (90 - 0.45 * (eng1NacTemp - 100), 0, 0, 1); // (0.45 == 180 / 400)
	      } else {
	        glRotated (90, 0, 0, 1);
	      }
	      glBegin (GL_LINES);
	        glVertex2d (  0, 0);
	        glVertex2d (  0, 45);
	      glEnd ();
	        glColor3ub (COLOR_BLACK);
	      glBegin (GL_LINES);
	        glVertex2d (  0, 0);
	        glVertex2d (  0, 25);
	      glEnd ();
	    glPopMatrix();
	    // the numeric value
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->SetSize(m_Font, 14, 14);
	    sprintf(buffer, "%4.0f", eng1NacTemp);
	    m_pFontManager->Print (112, 110, buffer, m_Font);
	  }
	  if (eng2FadecNoPower >= 1) {
	    // paint eng2NacTemp
	    aCircle.SetOrigin (320, 110); // Eng 2 Nac Tmp
	    aCircle.SetDegreesPerPoint (5);
	    aCircle.SetRadius (42);
	    aCircle.SetArcStartEnd (-90, 70);
	    glLineWidth (3);
	    glColor3ub (COLOR_WHITE);
	    glBegin(GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd();
	    glLineWidth (3);
	    glColor3ub (COLOR_WHITE);
	    glBegin (GL_LINES);
	      glVertex2d (278, 110);
	      glVertex2d (283, 110);
	      glVertex2d (320, 152);
	      glVertex2d (320, 147);
	      glVertex2d (359, 125);
	      glVertex2d (355, 123);
	    glEnd ();
	    // the needle
	    glPushMatrix();
	      glLineWidth (3);
	      glColor3ub (COLOR_GREEN);
	      glTranslated (320, 110, 0);
	      if (eng2NacTemp > 100) {
	        glRotated (90 - 0.45 * (eng2NacTemp - 100), 0, 0, 1); // (0.45 == 180 / 400)
	      } else {
	        glRotated (90, 0, 0, 1);
	      }
	      glBegin (GL_LINES);
	        glVertex2d (  0, 0);
	        glVertex2d (  0, 45);
	      glEnd ();
	        glColor3ub (COLOR_BLACK);
	      glBegin (GL_LINES);
	        glVertex2d (  0, 0);
	        glVertex2d (  0, 25);
	      glEnd ();
	    glPopMatrix();
	    // the numeric value
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->SetSize(m_Font, 14, 14);
	    sprintf(buffer, "%4.0f", eng2NacTemp);
	    m_pFontManager->Print (292, 110, buffer, m_Font);
	  }
	} else { // i.e. (engStartMode >= 1)
	  // paint startBkgnd image-2 (image SD_ENG_StartBkgnd)
	  m_pFontManager->SetSize(m_Font, 14, 14);
	  glColor3ub (COLOR_WHITE);
	  m_pFontManager->Print (212, 130, "IGN", m_Font);
	  glColor3ub (COLOR_CYAN);
	  m_pFontManager->Print (212,  80, "PSI", m_Font);
	  // eng1StartValve (image ENGStartValve)
	  if (eng1BleedPress > 35) glColor3ub (COLOR_GREEN);
	  else glColor3ub (COLOR_AMBER);
	  // bleed pressure 
	  m_pFontManager->SetSize(m_Font, 14, 14);
	  sprintf(buffer, "%3.0f", (eng1BleedPress * 41 / 56)); // sim/plane gives 56 instead of 41 !?!
	  m_pFontManager->Print (117, 80, buffer, m_Font);
	  // starter valve
	  aCircle.SetOrigin (140, 120);
	  aCircle.SetDegreesPerPoint (5);
	  aCircle.SetRadius (15);
	  aCircle.SetArcStartEnd (0, 360);
	  glLineWidth (3);
	  glBegin(GL_LINE_STRIP);
	    aCircle.Evaluate ();
	  glEnd();
	  if (eng1BleedPress > 35) glColor3ub (COLOR_GREEN);
	  else glColor3ub (COLOR_AMBER);
	  glLineWidth (3);
	  glBegin (GL_LINES);
	    glVertex2d (140, 135);
	    glVertex2d (140, 140);
	    glVertex2d (140, 105);
	    glVertex2d (140, 100);
	  glEnd ();
	  if ((eng1StartValve % 2) == 0) {
	  // paint the horizontal "closed" valve line
	    glBegin (GL_LINES);
	      glVertex2d (125, 120);
	      glVertex2d (155, 120);
	    glEnd ();
	  } else {
	    // paint the vertical "open" valve line
	    glBegin (GL_LINES);
	      glVertex2d (140, 135);
	      glVertex2d (140, 100);
	    glEnd ();
	    // ignition system designator
	    sprintf(buffer, "%s", "A "); // NOTE: not available in plane; should be variable
	    m_pFontManager->Print (130, 145, buffer, m_Font);
	  }
	  // paint eng2StartValve (image ENGStartValve)
	  if (eng2BleedPress > 35) glColor3ub (COLOR_GREEN);
	  else glColor3ub (COLOR_AMBER);
	  // bleed pressure 
	  m_pFontManager->SetSize(m_Font, 14, 14);
	  sprintf(buffer, "%3.0f", (eng2BleedPress * 41 / 56)); // sim/plane gives 56 instead of 41 !?!
	  m_pFontManager->Print (297, 80, buffer, m_Font);
	  // starter valve
	  aCircle.SetOrigin (320, 120);
	  aCircle.SetDegreesPerPoint (5);
	  aCircle.SetRadius (15);
	  aCircle.SetArcStartEnd (0, 360);
	  glLineWidth (3);
	  glBegin(GL_LINE_STRIP);
	    aCircle.Evaluate ();
	  glEnd();
	  if (eng2BleedPress > 35) glColor3ub (COLOR_GREEN);
	  else glColor3ub (COLOR_AMBER);
	  glLineWidth (3);
	  glBegin (GL_LINES);
	    glVertex2d (320, 135);
	    glVertex2d (320, 140);
	    glVertex2d (320, 105);
	    glVertex2d (320, 100);
	  glEnd ();
	  if ((eng2StartValve % 2) == 0) {
	    // paint the horizontal "closed" valve line
	    glBegin (GL_LINES);
	      glVertex2d (305, 120);
	      glVertex2d (335, 120);
	    glEnd ();
	  } else {
	    // paint the vertical "open" valve line
	    glBegin (GL_LINES);
	      glVertex2d (320, 135);
	      glVertex2d (320, 100);
	    glEnd ();
	    // ignition system designator
	    sprintf(buffer, "%s", "A "); // NOTE: not available in plane; should be variable
	    sprintf(buffer, "%s", " B"); // NOTE: not available in plane; should be variable
	    m_pFontManager->Print (310, 145, buffer, m_Font);
	  }
	} // end (engStartMode >= 1)

/* end of inlined code */


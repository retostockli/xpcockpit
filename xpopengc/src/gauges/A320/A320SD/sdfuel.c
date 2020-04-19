/*=============================================================================

  This is the sdfuel.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style System Display Widget ===

  This file contains the code for the SD subpage  #5: SD_FUEL
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

/* inlined code: SD FUEL Page */

	m_pFontManager->SetSize(m_Font, 14, 14);
	m_pFontManager->Print ( 15, 440, "FUEL", m_Font);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES); // title underlining
	  glVertex2d ( 15, 435);
	  glVertex2d ( 60, 435);
	glEnd ();

	// The fixed background stuff

	glLineWidth (2);
	// Shapes
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP);
	  glVertex2d ( 94, 252); glVertex2d ( 15, 240); glVertex2d ( 15, 190);
	  glVertex2d (445, 190); glVertex2d (445, 240); glVertex2d (366, 252);
	glEnd ();
	glBegin (GL_LINE_STRIP);
	  glVertex2d (156, 262); glVertex2d (167, 264); glVertex2d (195, 264);
	glEnd ();
	glBegin (GL_LINE_STRIP);
	  glVertex2d (265, 264); glVertex2d (290, 264); glVertex2d (304, 262);
	glEnd ();
	glBegin (GL_LINES);
	  glVertex2d ( 80, 190); glVertex2d ( 80, 250);
	  glVertex2d (180, 190); glVertex2d (180, 264);
	  glVertex2d (225, 264); glVertex2d (235, 264);
	  glVertex2d (280, 190); glVertex2d (280, 264);
	  glVertex2d (380, 190); glVertex2d (380, 250);
	glEnd ();
	glBegin (GL_LINE_STRIP);
	  glVertex2d ( 12, 108); glVertex2d (144, 108); glVertex2d (144,  86); glVertex2d ( 12,  86); glVertex2d ( 12, 108);
	glEnd ();

	// Straight Lines
	glBegin (GL_LINES);
	  glVertex2d (124, 420); glVertex2d (194, 430);
	  glVertex2d (265, 430); glVertex2d (335, 420);
	glEnd ();
	glColor3ub (COLOR_GREEN);
	glBegin (GL_LINES);
	  glVertex2d (110, 325); glVertex2d (215, 325);
	  glVertex2d (245, 325); glVertex2d (350, 325);
	  glVertex2d (110, 380); glVertex2d (110, 369);
	  glVertex2d (350, 380); glVertex2d (350, 369);
	  glVertex2d (110, 345); glVertex2d (110, 270);
	  glVertex2d (350, 345); glVertex2d (350, 270);
	  glVertex2d (210, 325); glVertex2d (210, 270);
	  glVertex2d (250, 325); glVertex2d (250, 270);
	glEnd ();
	glBegin (GL_LINE_STRIP);
	  glVertex2d (110, 290); glVertex2d (140, 290); glVertex2d (140, 270);
	glEnd ();
	glBegin (GL_LINE_STRIP);
	  glVertex2d (320, 270); glVertex2d (320, 290); glVertex2d (350, 290);
	glEnd ();

	// Variable lines

	// Texts
	m_pFontManager->SetSize(m_Font, 12, 12);
	glColor3ub (COLOR_WHITE);
	m_pFontManager->Print (105, 410, "1", m_Font);
	m_pFontManager->Print (345, 410, "2", m_Font);
	m_pFontManager->SetSize(m_Font, 12, 12);
	m_pFontManager->Print (200, 425, "F.USED", m_Font);
	m_pFontManager->Print (215, 405, "1+2", m_Font);
	m_pFontManager->Print ( 10, 145, "F.FLOW:", m_Font);
	m_pFontManager->Print ( 17, 125, "1+2:", m_Font);
	m_pFontManager->Print ( 17,  90, "FOB:", m_Font);
	glColor3ub (COLOR_CYAN);
	m_pFontManager->Print (220, 365, "KG", m_Font);
	m_pFontManager->Print ( 80, 165, ";", m_Font); // degr C
	m_pFontManager->Print (365, 165, ";", m_Font);
	m_pFontManager->Print (120, 125, "KG/MIN", m_Font);
	m_pFontManager->Print (120,  90, "KG", m_Font);

	// The valves

//	AirbusFBW/ENGFuelLPValveArray	int[4] 4 pos (image SD_LPFuelValve) 110, 355; 350, 355;
//eng1LPValve = 0;
	aCircle.SetRadius (14);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (0, 360);
	glLineWidth (3);
	aCircle.SetOrigin (110, 357);
	switch (eng1LPValve) {
	  case 0:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd();
	    glBegin (GL_LINES);
	      glVertex2d (101, 348); glVertex2d (119, 366);
	    glEnd ();
	  break;
	  case 1:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd();
	    glBegin (GL_LINES);
	      glVertex2d ( 98, 357); glVertex2d (122, 357);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd();
	    glBegin (GL_LINES);
	      glVertex2d (110, 345); glVertex2d (110, 369);
	    glEnd ();
	  break;
	  case 3:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd();
	    glBegin (GL_LINES);
	      glVertex2d (110, 345); glVertex2d (110, 369);
	    glEnd ();
	  break;
	}

//eng2LPValve = 3;
	aCircle.SetRadius (14);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (0, 360);
	glLineWidth (3);
	aCircle.SetOrigin (350, 357);
	switch (eng2LPValve) {
	  case 0:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd();
	    glBegin (GL_LINES);
	      glVertex2d (341, 348); glVertex2d (359, 366);
	    glEnd ();
	  break;
	  case 1:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd();
	    glBegin (GL_LINES);
	      glVertex2d (338, 357); glVertex2d (362, 357);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd();
	    glBegin (GL_LINES);
	      glVertex2d (350, 345); glVertex2d (350, 369);
	    glEnd ();
	  break;
	  case 3:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd();
	    glBegin (GL_LINES);
	      glVertex2d (350, 345); glVertex2d (350, 369);
	    glEnd ();
	  break;
	}

	aCircle.SetRadius (14);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (0, 360);
	glLineWidth (3);
	aCircle.SetOrigin (230, 325);
	switch (fuelXFV) {
	  case 0:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd();
	    glBegin (GL_LINES);
	      glVertex2d (221, 314); glVertex2d (239, 334);
	    glEnd ();
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd();
	    glBegin (GL_LINES);
	      glVertex2d (230, 337); glVertex2d (230, 313);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd();
	    glBegin (GL_LINES);
	      glVertex2d (218, 325); glVertex2d (242, 325);
	    glEnd ();
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd();
	    glBegin (GL_LINES);
	      glVertex2d (230, 337); glVertex2d (230, 313);
	    glEnd ();
	  break;
	  case 4:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd();
	    glBegin (GL_LINES);
	      glVertex2d (218, 325); glVertex2d (242, 325);
	    glEnd ();
	  break;
	}

	glLineWidth (2);
	switch (fuelLeftPump1) {
	  case 0:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d ( 94, 250); glVertex2d ( 94, 270); glVertex2d (124, 270); glVertex2d (124, 250); glVertex2d ( 94, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d ( 94, 260); glVertex2d (124, 260);
	    glEnd ();
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d ( 94, 250); glVertex2d ( 94, 270); glVertex2d (124, 270); glVertex2d (124, 250); glVertex2d ( 94, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (109, 250); glVertex2d (109, 270);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d ( 94, 250); glVertex2d ( 94, 270); glVertex2d (124, 270); glVertex2d (124, 250); glVertex2d ( 94, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d ( 94, 260); glVertex2d (124, 260);
	    glEnd ();
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d ( 94, 250); glVertex2d ( 94, 270); glVertex2d (124, 270); glVertex2d (124, 250); glVertex2d ( 94, 250);
	    glEnd ();
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    m_pFontManager->Print (100, 254, "LO", m_Font);
	  break;
	}

	glLineWidth (2);
	switch (fuelLeftPump2) {
	  case 0:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (126, 250); glVertex2d (126, 270); glVertex2d (156, 270); glVertex2d (156, 250); glVertex2d (126, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (126, 260); glVertex2d (156, 260);
	    glEnd ();
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (126, 250); glVertex2d (126, 270); glVertex2d (156, 270); glVertex2d (156, 250); glVertex2d (126, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (141, 250); glVertex2d (141, 270);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (126, 250); glVertex2d (126, 270); glVertex2d (156, 270); glVertex2d (156, 250); glVertex2d (126, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (126, 260); glVertex2d (156, 260);
	    glEnd ();
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (126, 250); glVertex2d (126, 270); glVertex2d (156, 270); glVertex2d (156, 250); glVertex2d (126, 250);
	    glEnd ();
	    m_pFontManager->Print (132, 254, "LO", m_Font);
	  break;
	}

	glLineWidth (2);
	switch (fuelRightPump1) {
	  case 0:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (304, 250); glVertex2d (304, 270); glVertex2d (334, 270); glVertex2d (334, 250); glVertex2d (304, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (304, 260); glVertex2d (334, 260);
	    glEnd ();
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (304, 250); glVertex2d (304, 270); glVertex2d (334, 270); glVertex2d (334, 250); glVertex2d (304, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (319, 250); glVertex2d (319, 270);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (304, 250); glVertex2d (304, 270); glVertex2d (334, 270); glVertex2d (334, 250); glVertex2d (304, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (304, 260); glVertex2d (334, 260);
	    glEnd ();
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (304, 250); glVertex2d (304, 270); glVertex2d (334, 270); glVertex2d (334, 250); glVertex2d (304, 250);
	    glEnd ();
	    m_pFontManager->Print (310, 254, "LO", m_Font);
	  break;
	}

	glLineWidth (2);
	switch (fuelRightPump2) {
	  case 0:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (336, 250); glVertex2d (336, 270); glVertex2d (366, 270); glVertex2d (366, 250); glVertex2d (336, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (336, 260); glVertex2d (366, 260);
	    glEnd ();
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (336, 250); glVertex2d (336, 270); glVertex2d (366, 270); glVertex2d (366, 250); glVertex2d (336, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (351, 250); glVertex2d (351, 270);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (336, 250); glVertex2d (336, 270); glVertex2d (366, 270); glVertex2d (366, 250); glVertex2d (336, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (336, 260); glVertex2d (366, 260);
	    glEnd ();
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (336, 250); glVertex2d (336, 270); glVertex2d (366, 270); glVertex2d (366, 250); glVertex2d (336, 250);
	    glEnd ();
	    m_pFontManager->Print (342, 254, "LO", m_Font);
	  break;
	}

	glLineWidth (2);
	switch (fuelCenterPump1) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (195, 250); glVertex2d (195, 270); glVertex2d (225, 270); glVertex2d (225, 250); glVertex2d (195, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (195, 260); glVertex2d (225, 260);
	    glEnd ();
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (195, 250); glVertex2d (195, 270); glVertex2d (225, 270); glVertex2d (225, 250); glVertex2d (195, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (210, 250); glVertex2d (210, 270);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (195, 250); glVertex2d (195, 270); glVertex2d (225, 270); glVertex2d (225, 250); glVertex2d (195, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (195, 260); glVertex2d (225, 260);
	    glEnd ();
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (195, 250); glVertex2d (195, 270); glVertex2d (225, 270); glVertex2d (225, 250); glVertex2d (195, 250);
	    glEnd ();
	    m_pFontManager->Print (201, 254, "LO", m_Font);
	  break;
	}

	glLineWidth (2);
	switch (fuelCenterPump2) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (235, 250); glVertex2d (235, 270); glVertex2d (265, 270); glVertex2d (265, 250); glVertex2d (235, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (235, 260); glVertex2d (265, 260);
	    glEnd ();
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (235, 250); glVertex2d (235, 270); glVertex2d (265, 270); glVertex2d (265, 250); glVertex2d (235, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (250, 250); glVertex2d (250, 270);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (235, 250); glVertex2d (235, 270); glVertex2d (265, 270); glVertex2d (265, 250); glVertex2d (235, 250);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (235, 260); glVertex2d (265, 260);
	    glEnd ();
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d (235, 250); glVertex2d (235, 270); glVertex2d (265, 270); glVertex2d (265, 250); glVertex2d (235, 250);
	    glEnd ();
	    m_pFontManager->Print (241, 254, "LO", m_Font);
	  break;
	}

	glColor3ub (COLOR_GREEN);
	glLineWidth (3);
	if ((fuelTVLeft1 > 0) || (fuelTVLeft2 > 0)) {
	  glBegin (GL_LINE_STRIP);
	    glVertex2d ( 75, 215); glVertex2d ( 65, 210); glVertex2d ( 65, 220); glVertex2d ( 75, 215);
	  glEnd ();
	}

	glColor3ub (COLOR_GREEN);
	glLineWidth (3);
	if ((fuelTVRight1 > 0) || (fuelTVRight2 > 0)) {
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (385, 215); glVertex2d (395, 210); glVertex2d (395, 220); glVertex2d (385, 215);
	  glEnd ();
	}

	// The numeric value displays

	glColor3ub (COLOR_GREEN);
	sprintf(buffer, "%4.0f", 0.0);
	m_pFontManager->Print ( 90, 385, buffer, m_Font); // fuel used
	sprintf(buffer, "%4.0f", 0.0);
	m_pFontManager->Print (210, 385, buffer, m_Font);
	sprintf(buffer, "%4.0f", 0.0);
	m_pFontManager->Print (330, 385, buffer, m_Font);
	sprintf(buffer, "%4.0f", fuelQuant1);
	m_pFontManager->Print ( 25, 209, buffer, m_Font); // fuel available
	sprintf(buffer, "%4.0f", fuelQuant2);
	m_pFontManager->Print (105, 209, buffer, m_Font);
	sprintf(buffer, "%4.0f", fuelQuant3);
	m_pFontManager->Print (210, 209, buffer, m_Font);
	sprintf(buffer, "%4.0f", fuelQuant4);
	m_pFontManager->Print (315, 209, buffer, m_Font);
	sprintf(buffer, "%4.0f", fuelQuant5);
	m_pFontManager->Print (400, 209, buffer, m_Font);
	sprintf(buffer, "%4.0f", fuelFlow * 60);
	m_pFontManager->Print ( 75, 125, buffer, m_Font); // fuel flow (in kg/min!)
	sprintf(buffer, "%4.0f", fuelQuant);
	m_pFontManager->Print ( 75,  90, buffer, m_Font); // FOB

/* end of inlined code */


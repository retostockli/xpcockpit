/*=============================================================================

  This is the sdhyd.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style System Display Widget ===

  This file contains the code for the SD subpage  #4: SD_HYD
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

/* inlined code: SD HYD Page */

	m_pFontManager->SetSize(m_Font, 14, 14);
	m_pFontManager->Print (130, 440, "HYD", m_Font);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES); // title underlining
	  glVertex2d (130, 435);
	  glVertex2d (166, 435);
	glEnd ();

	// fixed labels
	m_pFontManager->SetSize(m_Font, 12, 12);
	glColor3ub (COLOR_CYAN);
	m_pFontManager->Print (140, 360, "PSI", m_Font);
	m_pFontManager->Print (295, 360, "PSI", m_Font);
	m_pFontManager->SetSize(m_Font, 14, 14);

	// The PTU Circuit
	if (ptuMode ==0) glColor3ub (COLOR_WHITE);
	else glColor3ub (COLOR_GREEN);
	m_pFontManager->SetSize(m_Font, 12, 12);
	m_pFontManager->Print (300, 320, "PTU", m_Font); // label
	m_pFontManager->SetSize(m_Font, 14, 14);
	sprintf(buffer, "%i", ptuMode);
	m_pFontManager->Print (300, 300, buffer, m_Font);
	glLineWidth (2);
	if (ptuMode ==0) glColor3ub (COLOR_AMBER);
	else glColor3ub (COLOR_GREEN);
	if (ptuMode > 1) {
	  glBegin (GL_LINES);      // hor line : left connector
	    glVertex2d ( 75, 326);
	    glVertex2d (158, 326);
	  glEnd ();
	  glBegin (GL_LINES);      // hor line : right connector
	    glVertex2d (341, 326);
	    glVertex2d (380, 326);
	  glEnd ();
	}
	glBegin (GL_LINES);      // hor line center connector left
	  glVertex2d (167, 326);
	  glVertex2d (215, 326);
	glEnd ();
	aCircle.SetOrigin (230, 326); // curve
	aCircle.SetRadius (15);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd ( 90, 270);
	glLineWidth (2);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	glBegin (GL_LINES);      // hor line center connector right
	  glVertex2d (245, 326);
	  glVertex2d (288, 326);
	glEnd ();
	if ((ptuMode < 3)) {
	  glBegin (GL_LINE_STRIP); // left-pointing arrow left
	    glVertex2d (167, 320);
	    glVertex2d (158, 326);
	    glVertex2d (167, 332);
	    glVertex2d (167, 320);
	  glEnd ();
	  glBegin (GL_LINE_STRIP); // left-pointing arrow PTU left
	    glVertex2d (297, 320);
	    glVertex2d (288, 326);
	    glVertex2d (297, 332);
	    glVertex2d (297, 320);
	  glEnd ();
	} else { // ptuMode == 3
	  glBegin (GL_LINE_STRIP); // right-pointing arrow left
	    glVertex2d (158, 320);
	    glVertex2d (167, 326);
	    glVertex2d (158, 332);
	    glVertex2d (158, 320);
	  glEnd ();
	  glBegin (GL_LINE_STRIP); // right-pointing arrow PTU left
	    glVertex2d (288, 320);
	    glVertex2d (297, 326);
	    glVertex2d (288, 332);
	    glVertex2d (288, 320);
	  glEnd ();
	}
	if (ptuMode == 2) {
	  glBegin (GL_LINE_STRIP); // left-pointing arrow PTU right
	    glVertex2d (341, 320);
	    glVertex2d (332, 326);
	    glVertex2d (341, 332);
	    glVertex2d (341, 320);
	  glEnd ();
	} else {
	  glBegin (GL_LINE_STRIP); // right-pointing arrow PTU right
	    glVertex2d (332, 320);
	    glVertex2d (341, 326);
	    glVertex2d (332, 332);
	    glVertex2d (332, 320);
	  glEnd ();
	}

	// The green circuit

	// The label and value
	if (hydPressGreen < 1450) glColor3ub (COLOR_AMBER);
	else glColor3ub (COLOR_GREEN);
	m_pFontManager->Print ( 50, 405, "GREEN", m_Font); // label
	glLineWidth (2);
	glBegin (GL_LINE_STRIP); // up-pointing arrow
	  glVertex2d ( 75, 391);
	  glVertex2d ( 80, 391);
	  glVertex2d ( 75, 400);
	  glVertex2d ( 70, 391);
	  glVertex2d ( 75, 391);
	glEnd ();
	glBegin (GL_LINES);      // vert line arrow - value
	  glVertex2d ( 75, 391);
	  glVertex2d ( 75, 378);
	glEnd ();
	m_pFontManager->SetSize(m_Font, 14, 14);
	sprintf(buffer, "%4.0f", hydPressGreen); // pressure value
	if (hydPressGreen < 10.0) m_pFontManager->Print ( 64, 360, "00", m_Font);
	else if (hydPressGreen < 100.0) m_pFontManager->Print ( 41, 360, buffer, m_Font);
	else if (hydPressGreen < 1000.0) m_pFontManager->Print ( 46, 360, buffer, m_Font);
	else m_pFontManager->Print ( 53, 360, buffer, m_Font);
	glLineWidth (2);
	glBegin (GL_LINES);
	  glVertex2d ( 75, 355); // vert line value - PTU Connector
	  glVertex2d ( 75, 326);
	glEnd ();
	// the pump indicator
	if (hydPumpGreen == 1) glColor3ub (COLOR_GREEN);
	else glColor3ub (COLOR_AMBER);
	glBegin (GL_LINES);
	  glVertex2d ( 75, 326); // vert line PTU Connector - box
	  glVertex2d ( 75, 270);
	glEnd ();
	glBegin (GL_LINE_STRIP); // box
	  glVertex2d ( 55, 270);
	  glVertex2d ( 95, 270);
	  glVertex2d ( 95, 245);
	  glVertex2d ( 55, 245);
	  glVertex2d ( 55, 270);
	glEnd ();
	if (hydPumpGreen == 0) {
	  glBegin (GL_LINES);
	    glVertex2d ( 55, 257); // pump inactive indicator
	    glVertex2d ( 95, 257);
	  glEnd ();
	} else if (hydPumpGreen == 1) {
 	  glBegin (GL_LINES);
	    glVertex2d ( 75, 245); // pump OK indicator
	    glVertex2d ( 75, 270);
	  glEnd ();
	} else { // hydPumpGreen == 2
	  m_pFontManager->Print ( 65, 250, "LO", m_Font); // box value
	}
	if (hydPressGreen < 2900) glColor3ub (COLOR_AMBER);
	else glColor3ub (COLOR_WHITE);
	m_pFontManager->Print (105, 240, "1", m_Font); // engine label
	// The (fire) shutoff valve
	glLineWidth (2);
	if (eng1FireExt < 2) glColor3ub (COLOR_GREEN); // values 0, 1
	else glColor3ub (COLOR_AMBER);                 // values 2, 3
	glBegin (GL_LINES);      // vert line box - valve
	  glVertex2d ( 75, 245);
	  glVertex2d ( 75, 225);
	glEnd ();
	aCircle.SetOrigin ( 75, 210); // valve
	aCircle.SetRadius (15);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (  0, 360);
	glLineWidth (2);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	if ((eng1FireExt % 2) == 0) { // values 0, 2
	  glBegin (GL_LINES);      // vert line : valve open
	    glVertex2d ( 75, 225);
	    glVertex2d ( 75, 195);
	  glEnd ();
	} else {
	  glBegin (GL_LINES);      // hor line : valve closed
	    glVertex2d ( 60, 210);
	    glVertex2d ( 90, 210);
	  glEnd ();
	}
	// The fluid quantity indicator
	glColor3ub (COLOR_GREEN);
	glBegin (GL_LINES);      // vert line valve - green block
	  glVertex2d ( 75, 195);
	  glVertex2d ( 75, 180);
	glEnd ();
	glBegin (GL_POLYGON);    // green block
	  glVertex2d ( 75, 180);
	  glVertex2d ( 80, 180);
	  glVertex2d ( 80, 150);
	  glVertex2d ( 75, 150);
	glEnd ();
	glBegin (GL_LINE_STRIP); // green block outline
	  glVertex2d ( 75, 180);
	  glVertex2d ( 80, 180);
	  glVertex2d ( 80, 150);
	  glVertex2d ( 75, 150);
	  glVertex2d ( 75, 180);
	glEnd ();
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES);      // vert line
	  glVertex2d ( 75, 150);
	  glVertex2d ( 75, 120);
	glEnd ();
	glColor3ub (COLOR_AMBER);
	glBegin (GL_POLYGON);    // amber block
	  glVertex2d ( 76, 120);
	  glVertex2d ( 80, 120);
	  glVertex2d ( 80,  80);
	  glVertex2d ( 76,  80);
	glEnd ();
	glBegin (GL_LINE_STRIP); // amber block outline
	  glVertex2d ( 76, 120);
	  glVertex2d ( 80, 120);
	  glVertex2d ( 80,  80);
	  glVertex2d ( 76,  80);
	  glVertex2d ( 76, 120);
	glEnd ();
	// The quantity indicator
	glColor3ub (COLOR_GREEN);
	glBegin (GL_POLYGON);    // amber block
	  glVertex2d ( 65, 165);
	  glVertex2d ( 74, 165);
	  glVertex2d ( 74,  80);
	  glVertex2d ( 65,  80);
	glEnd ();
	glBegin (GL_LINE_STRIP); // amber block outline
	  glVertex2d ( 65, 175);
	  glVertex2d ( 74, 165);
	  glVertex2d ( 74,  80);
	  glVertex2d ( 65,  80);
	  glVertex2d ( 65, 165);
	  glVertex2d ( 74, 165);
	glEnd ();

	// The blue circuit

	if (hydPressBlue < 1450) glColor3ub (COLOR_AMBER);
	else  glColor3ub (COLOR_GREEN);
	m_pFontManager->Print (207, 405, "BLUE", m_Font); // label
	glLineWidth (2);
	glBegin (GL_LINE_STRIP); // up-pointing arrow
	  glVertex2d (230, 391);
	  glVertex2d (235, 391);
	  glVertex2d (230, 400);
	  glVertex2d (225, 391);
	  glVertex2d (230, 391);
	glEnd ();
	glBegin (GL_LINES);     // vert line arrow - value
	  glVertex2d (230, 391);
	  glVertex2d (230, 378);
	glEnd ();
	m_pFontManager->SetSize(m_Font, 14, 14);
	sprintf(buffer, "%4.0f", hydPressBlue); // pressure value
	if (hydPressBlue < 10.0) m_pFontManager->Print (219, 360, "00", m_Font);
	else if (hydPressBlue < 100.0) m_pFontManager->Print (196, 360, buffer, m_Font);
	else if (hydPressBlue < 1000.0) m_pFontManager->Print (201, 360, buffer, m_Font);
	else m_pFontManager->Print (208, 360, buffer, m_Font);
	glLineWidth (2);
	glBegin (GL_LINES); // vert line value - ratConn
	  glVertex2d (230, 355);
	  glVertex2d (230, 290);
	glEnd ();
	// the pump indicator
	if (hydPumpBlue == 1) glColor3ub (COLOR_GREEN);
	else glColor3ub (COLOR_AMBER);
	glBegin (GL_LINES); // vert line ratConn - box
	  glVertex2d (230, 290);
	  glVertex2d (230, 250);
	glEnd ();
	glBegin (GL_LINE_STRIP); // box
	  glVertex2d (210, 250);
	  glVertex2d (250, 250);
	  glVertex2d (250, 225);
	  glVertex2d (210, 225);
	  glVertex2d (210, 250);
	glEnd ();
	if (hydPumpBlue == 0) {
	  glBegin (GL_LINES);
	    glVertex2d (210, 237); // pump inactive indicator
	    glVertex2d (250, 237);
	  glEnd ();
	} else if (hydPumpBlue == 1) {
 	  glBegin (GL_LINES);
	    glVertex2d (230, 225); // pump OK indicator
	    glVertex2d (230, 250);
	  glEnd ();
	} else { // hydPumpBlue == 2
	  m_pFontManager->Print (220, 230, "LO", m_Font); // box value
	}
	glLineWidth (2);
	glColor3ub (COLOR_GREEN);
	glBegin (GL_LINES);      // vert line box - green block
	  glVertex2d (230, 225);
	  glVertex2d (230, 180);
	glEnd ();
	glBegin (GL_POLYGON);    // green block
	  glVertex2d (230, 180);
	  glVertex2d (235, 180);
	  glVertex2d (235, 150);
	  glVertex2d (230, 150);
	glEnd ();
	glBegin (GL_LINE_STRIP); // green block outline
	  glVertex2d (230, 180);
	  glVertex2d (235, 180);
	  glVertex2d (235, 150);
	  glVertex2d (230, 150);
	  glVertex2d (230, 180);
	glEnd ();
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES);      // vert line
	  glVertex2d (230, 150);
	  glVertex2d (230, 120);
	glEnd ();
	glColor3ub (COLOR_AMBER);
	glBegin (GL_POLYGON);    // amber block
	  glVertex2d (231, 120);
	  glVertex2d (235, 120);
	  glVertex2d (235,  80);
	  glVertex2d (231,  80);
	glEnd ();
	glBegin (GL_LINE_STRIP); // amber block outline
	  glVertex2d (231, 120);
	  glVertex2d (235, 120);
	  glVertex2d (235,  80);
	  glVertex2d (230,  80);
	  glVertex2d (231, 120);
	glEnd ();
	// The quantity indicator
	glColor3ub (COLOR_GREEN);
	glBegin (GL_POLYGON);    // amber block
	  glVertex2d (220, 165);
	  glVertex2d (229, 165);
	  glVertex2d (229,  80);
	  glVertex2d (220,  80);
	glEnd ();
	glBegin (GL_LINE_STRIP); // amber block outline
	  glVertex2d (220, 175);
	  glVertex2d (229, 165);
	  glVertex2d (229,  80);
	  glVertex2d (220,  80);
	  glVertex2d (220, 165);
	  glVertex2d (229, 165);
	glEnd ();

	// The yellow circuit

	// The label and value
	if (hydPressYellow < 1450) glColor3ub (COLOR_AMBER);
	else glColor3ub (COLOR_GREEN);
	m_pFontManager->Print (345, 405, "YELLOW", m_Font); // label
	glLineWidth (2);
	glBegin (GL_LINE_STRIP); // up-pointing arrow
	  glVertex2d (380, 391);
	  glVertex2d (385, 391);
	  glVertex2d (380, 400);
	  glVertex2d (375, 391);
	  glVertex2d (380, 391);
	glEnd ();
	glBegin (GL_LINES);     // vert line arrow - value
	  glVertex2d (380, 391);
	  glVertex2d (380, 378);
	glEnd ();
	m_pFontManager->SetSize(m_Font, 14, 14);
	sprintf(buffer, "%4.0f", hydPressYellow); // pressure value
	if (hydPressYellow < 10.0) m_pFontManager->Print (369, 360, "00", m_Font);
	else if (hydPressYellow < 100.0) m_pFontManager->Print (346, 360, buffer, m_Font);
	else if (hydPressYellow < 1000.0) m_pFontManager->Print (351, 360, buffer, m_Font);
	else m_pFontManager->Print (358, 360, buffer, m_Font);
	glLineWidth (2);
	glBegin (GL_LINES);
	  glVertex2d (380, 355); // vert line value - PTU Connector
	  glVertex2d (380, 326);
	glEnd ();
	if (yElecMode == 1) glColor3ub (COLOR_GREEN);
	else glColor3ub (COLOR_AMBER);
	glBegin (GL_LINES); // vert line PTU Connector - yElecConn
	  glVertex2d (380, 326);
	  glVertex2d (380, 290);
	glEnd ();
	// the pump indicator
	if (hydPumpYellow == 1) glColor3ub (COLOR_GREEN);
	else glColor3ub (COLOR_AMBER);
	glBegin (GL_LINES);      // vert line yElecConn - box
	  glVertex2d (380, 290);
	  glVertex2d (380, 270);
	glEnd ();
	glBegin (GL_LINE_STRIP); // box
	  glVertex2d (360, 270);
	  glVertex2d (400, 270);
	  glVertex2d (400, 245);
	  glVertex2d (360, 245);
	  glVertex2d (360, 270);
	glEnd ();
	if (hydPumpYellow == 0) {
	  glBegin (GL_LINES);
	    glVertex2d (360, 257); // pump inactive indicator
	    glVertex2d (400, 257);
	  glEnd ();
	} else if (hydPumpYellow == 1) {
 	  glBegin (GL_LINES);
	    glVertex2d (380, 245); // pump OK indicator
	    glVertex2d (380, 270);
	  glEnd ();
	} else { // hydPumpYellow == 2
	  m_pFontManager->Print (370, 250, "LO", m_Font); // box value
	}
	if (hydPressYellow < 2900) glColor3ub (COLOR_AMBER);
	else glColor3ub (COLOR_WHITE);
	m_pFontManager->Print (335, 240, "2", m_Font); // engine label
	glLineWidth (2);
	// The (fire) shutoff valve
	glLineWidth (2);
	if (eng1FireExt < 2) glColor3ub (COLOR_GREEN); // values 0, 1
	else glColor3ub (COLOR_AMBER);                 // values 2, 3

	glBegin (GL_LINES);       // vert line box - valve
	  glVertex2d (380, 245);
	  glVertex2d (380, 225);
	glEnd ();
	if (eng2FireExt < 2) glColor3ub (COLOR_GREEN); // values 0, 1
	else glColor3ub (COLOR_AMBER);                 // values 2, 3
	aCircle.SetOrigin (380, 210); // valve
	aCircle.SetRadius (15);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (  0, 360);
	glLineWidth (2);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	if ((eng2FireExt % 2) == 0) { // values 0, 2
	  glBegin (GL_LINES);         // vert line : valve open
	    glVertex2d (380, 225);
	    glVertex2d (380, 195);
	  glEnd ();
	} else {                     // values 1, 3
	  glBegin (GL_LINES);        // hor line : valve closed
	    glVertex2d (365, 210);
	    glVertex2d (395, 210);
	  glEnd ();
	}
	// The fluid quantity indicator
	glColor3ub (COLOR_GREEN);
	glBegin (GL_LINES);      // vert line valve - green block
	  glVertex2d (380, 195);
	  glVertex2d (380, 180);
	glEnd ();
	glColor3ub (COLOR_GREEN);
	glBegin (GL_POLYGON);   // green block
	  glVertex2d (380, 180);
	  glVertex2d (385, 180);
	  glVertex2d (385, 150);
	  glVertex2d (380, 150);
	glEnd ();
	glBegin (GL_LINE_STRIP); // green block outline
	  glVertex2d (380, 180);
	  glVertex2d (385, 180);
	  glVertex2d (385, 150);
	  glVertex2d (380, 150);
	  glVertex2d (380, 180);
	glEnd ();
	glColor3ub (COLOR_WHITE);      // vert line
	glBegin (GL_LINES);
	  glVertex2d (380, 150);
	  glVertex2d (380, 120);
	glEnd ();
	glColor3ub (COLOR_AMBER);
	glBegin (GL_POLYGON);    // amber block
	  glVertex2d (381, 120);
	  glVertex2d (385, 120);
	  glVertex2d (385,  80);
	  glVertex2d (381,  80);
	glEnd ();
	glBegin (GL_LINE_STRIP); // amber block outline
	  glVertex2d (381, 120);
	  glVertex2d (385, 120);
	  glVertex2d (385,  80);
	  glVertex2d (381,  80);
	  glVertex2d (381, 120);
	glEnd ();
	// The quantity indicator
	glColor3ub (COLOR_GREEN);
	glBegin (GL_POLYGON);    // amber block
	  glVertex2d (370, 165);
	  glVertex2d (379, 165);
	  glVertex2d (379,  80);
	  glVertex2d (370,  80);
	glEnd ();
	glBegin (GL_LINE_STRIP); // amber block outline
	  glVertex2d (370, 175);
	  glVertex2d (379, 165);
	  glVertex2d (379,  80);
	  glVertex2d (370,  80);
	  glVertex2d (370, 165);
	  glVertex2d (379, 165);
	glEnd ();

//yElecMode = 0;
	// The Yellow Electric Pump
	if (yElecMode == 1) glColor3ub (COLOR_GREEN);
	else if (yElecMode == 0) glColor3ub (COLOR_WHITE);
	else glColor3ub (COLOR_AMBER);
	m_pFontManager->Print (405, 285, "ELEC", m_Font);
	sprintf(buffer, "%i", yElecMode);
	m_pFontManager->Print (405, 265, buffer, m_Font);
	glLineWidth (2);
	if (yElecMode == 1) glColor3ub (COLOR_GREEN);
	else if (yElecMode == 2) glColor3ub (COLOR_AMBER);
	glBegin (GL_LINE_STRIP); // left-pointing arrow
	  glVertex2d (390, 290);
	  glVertex2d (400, 295);
	  glVertex2d (400, 285);
	  glVertex2d (390, 290);
	glEnd ();
	if (yElecMode > 0) {
	  glBegin (GL_LINES);
	    glVertex2d (380, 290); // hor line (yElecConn)
	    glVertex2d (390, 290);
	  glEnd ();
	}

//ratMode = 0;
	// The RAT Pump
	if (ratMode == 1) glColor3ub (COLOR_GREEN);
	else if (ratMode == 0) glColor3ub (COLOR_WHITE);
	else glColor3ub (COLOR_AMBER);
	m_pFontManager->Print (170, 285, "RAT", m_Font);
	sprintf(buffer, "%i", ratMode);
	m_pFontManager->Print (170, 265, buffer, m_Font);
	glLineWidth (2);
	if (ratMode == 1) glColor3ub (COLOR_GREEN);
	else if (ratMode == 2) glColor3ub (COLOR_AMBER);
	glBegin (GL_LINE_STRIP); // right-pointing arrow
	  glVertex2d (220, 290);
	  glVertex2d (210, 295);
	  glVertex2d (210, 285);
	  glVertex2d (220, 290);
	glEnd ();
	if (ratMode > 0) {
	  glBegin (GL_LINES);
	    glVertex2d (220, 290); // hor line (ratConn)
	    glVertex2d (230, 290);
	  glEnd ();
	}

/* end of inlined code */


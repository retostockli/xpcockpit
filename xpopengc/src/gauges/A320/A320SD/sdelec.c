/*=============================================================================

  This is the sdelec.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style System Display Widget ===

  This file contains the code for the SD subpage  #3: SD_ELEC
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

/* inlined code: SD ELEC Page */

	m_pFontManager->SetSize(m_Font, 14, 14);
	m_pFontManager->Print ( 20, 425, "ELEC", m_Font);
	glLineWidth (2);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES); // title underlining
	  glVertex2d ( 20, 420);
	  glVertex2d ( 66, 420);
	glEnd ();

	// The local variables

	int bat1V, bat1A, bat2V, bat2A, gen1P, gen1V, gen1F, gen2P, gen2V, gen2F;

	// The fixed text fields

	glColor3ub (COLOR_GRAY50);
	glBegin(GL_POLYGON);	 // DC1
	  glVertex2d ( 31, 377);
	  glVertex2d ( 83, 377);
	  glVertex2d ( 83, 358);
	  glVertex2d ( 31, 358);
	glEnd ();
	glBegin(GL_POLYGON);	 // DC2
	  glVertex2d (376, 377);
	  glVertex2d (428, 377);
	  glVertex2d (428, 358);
	  glVertex2d (376, 358);
	glEnd ();
	glBegin(GL_POLYGON);	 // AC1
	  glVertex2d ( 31, 248);
	  glVertex2d ( 83, 248);
	  glVertex2d ( 83, 229);
	  glVertex2d ( 31, 229);
	glEnd ();
	glBegin(GL_POLYGON);	 // AC2
	  glVertex2d (376, 248);
	  glVertex2d (428, 248);
	  glVertex2d (428, 229);
	  glVertex2d (376, 229);
	glEnd ();
	glBegin(GL_POLYGON);	 // DC BAT
	  glVertex2d (194, 424);
	  glVertex2d (266, 424);
	  glVertex2d (266, 404);
	  glVertex2d (194, 404);
	glEnd ();
	glBegin(GL_POLYGON);	 // DC ESS
	  glVertex2d (194, 358);
	  glVertex2d (266, 358);
	  glVertex2d (266, 337);
	  glVertex2d (194, 337);
	glEnd ();
	glBegin(GL_POLYGON);	 // AC ESS
	  glVertex2d (194, 248);
	  glVertex2d (266, 248);
	  glVertex2d (266, 229);
	  glVertex2d (194, 229);
	glEnd ();

	glColor3ub (COLOR_GREEN);
	m_pFontManager->SetSize(m_Font, 14, 14);
	m_pFontManager->Print ( 35, 361, "DC 1", m_Font);
	m_pFontManager->Print (380, 361, "DC 2", m_Font);
	m_pFontManager->Print ( 35, 232, "AC 1", m_Font);
	m_pFontManager->Print (380, 232, "AC 2", m_Font);
	m_pFontManager->Print (196, 407, "DC BAT", m_Font);
	m_pFontManager->Print (196, 340, "DC ESS", m_Font);
	m_pFontManager->Print (196, 232, "AC ESS", m_Font);

	// The connections

//sdACCrossConnect = 7;
	glLineWidth (2);
	glColor3ub (COLOR_GREEN);
	switch (sdACCrossConnect) {
	  case 0: // both engines running
	  break;
	  case 1: // APU GEN - AC1
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - XCONN
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 210);
	    glEnd ();
	    glBegin (GL_LINES);      // XCONN (AC1 - APU GEN)
	      glVertex2d ( 57, 210);
	      glVertex2d (172, 210);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // APU GEN Arrow
	      glVertex2d (172, 210);
	      glVertex2d (168, 201);
	      glVertex2d (176, 201);
	      glVertex2d (172, 210);
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (172, 201); // APU GEN Arrow - APU GEN
	      glVertex2d (172, 189);
	    glEnd ();
	  break;
	  case 2: // APU power, engine 1 running
	    glBegin (GL_LINE_STRIP); // AC2 Arrow
	      glVertex2d (402, 229);
	      glVertex2d (398, 220);
	      glVertex2d (406, 220);
	      glVertex2d (402, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC2 Arrow - XCONN
	      glVertex2d (402, 220); 
	      glVertex2d (402, 210);
	    glEnd ();
	    glBegin (GL_LINES);      // XCONN (APU GEN - AC2)
	      glVertex2d (172, 210);
	      glVertex2d (402, 210);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // APU GEN Arrow
	      glVertex2d (172, 210);
	      glVertex2d (168, 201);
	      glVertex2d (176, 201);
	      glVertex2d (172, 210);
	    glEnd ();
	    glBegin (GL_LINES);      // APU GEN Arrow - APU GEN
	      glVertex2d (172, 201);
	      glVertex2d (172, 189);
	    glEnd ();
	  break;
	  case 3: // APU power, no engines running
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - XCONN
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 210);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC2 Arrow
	      glVertex2d (402, 229);
	      glVertex2d (398, 220);
	      glVertex2d (406, 220);
	      glVertex2d (402, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC2 Arrow - XCONN
	      glVertex2d (402, 220); 
	      glVertex2d (402, 210);
	    glEnd ();
	    glBegin (GL_LINES);      // XCONN (AC1 - AC2)
	      glVertex2d ( 57, 210);
	      glVertex2d (402, 210);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // APU GEN Arrow
	      glVertex2d (172, 210);
	      glVertex2d (168, 201);
	      glVertex2d (176, 201);
	      glVertex2d (172, 210);
	    glBegin (GL_LINES);      // APU GEN Arrow - APU GEN
	      glVertex2d (172, 201);
	      glVertex2d (172, 189);
	    glEnd ();
	    glEnd ();
	  break;
	  case 4: // External power, engine 2 running
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - XCONN
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 210);
	    glEnd ();
	    glBegin (GL_LINES);      // XCONN (AC1 - EXT POW)
	      glVertex2d ( 57, 210);
	      glVertex2d (287, 210);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // EXT POW Arrow
	      glVertex2d (287, 210);
	      glVertex2d (283, 201);
	      glVertex2d (291, 201);
	      glVertex2d (287, 210);
	    glEnd ();
	    glBegin (GL_LINES);      // EXT POW Arrow - EXT POW
	      glVertex2d (287, 201);
	      glVertex2d (287, 168);
	    glEnd ();
	  break;
	  case 5: // External power, engine 1 running
	    glBegin (GL_LINE_STRIP); // AC2 Arrow
	      glVertex2d (402, 229);
	      glVertex2d (398, 220);
	      glVertex2d (406, 220);
	      glVertex2d (402, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC2 Arrow - XCONN
	      glVertex2d (402, 220); 
	      glVertex2d (402, 210);
	    glEnd ();
	    glBegin (GL_LINES);      // XCONN (EXT POW - AC2)
	      glVertex2d (287, 210);
	      glVertex2d (402, 210);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // EXT POW Arrow
	      glVertex2d (287, 210);
	      glVertex2d (283, 201);
	      glVertex2d (291, 201);
	      glVertex2d (287, 210);
	    glEnd ();
	    glBegin (GL_LINES);      // EXT POW Arrow - EXT POW
	      glVertex2d (287, 201);
	      glVertex2d (287, 168);
	    glEnd ();
	  break;
	  case 6: // External power, no engines running
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - XCONN
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 210);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC2 Arrow
	      glVertex2d (402, 229);
	      glVertex2d (398, 220);
	      glVertex2d (406, 220);
	      glVertex2d (402, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC2 Arrow - XCONN
	      glVertex2d (402, 220); 
	      glVertex2d (402, 210);
	    glEnd ();
	    glBegin (GL_LINES);      // XCONN (AC1 - AC2)
	      glVertex2d ( 57, 210);
	      glVertex2d (402, 210);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // EXT POW Arrow
	      glVertex2d (287, 210);
	      glVertex2d (283, 201);
	      glVertex2d (291, 201);
	      glVertex2d (287, 210);
	    glEnd ();
	    glBegin (GL_LINES);      // EXT POW Arrow - EXT POW
	      glVertex2d (287, 201);
	      glVertex2d (287, 168);
	    glEnd ();
	  break;
	  default:
	  break;
	}

//sdELConnectLeft = 28;
	glLineWidth (2);
	glColor3ub (COLOR_GREEN);
	switch (sdELConnectLeft) {
	  case 0:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // XCONN - GEN1
	      glVertex2d ( 57, 210);
	      glVertex2d ( 57, 189);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - XCONN
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 210);
	    glEnd ();
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - GEN1
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 189);
	    glEnd ();
	  break;
	  case 4:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - TR1
	      glVertex2d ( 57, 358);
	      glVertex2d ( 57, 337);
	    glEnd ();
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	  break;
	  case 5:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - TR1
	      glVertex2d ( 57, 358);
	      glVertex2d ( 57, 337);
	    glEnd ();
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // XCONN - GEN1
	      glVertex2d ( 57, 210);
	      glVertex2d ( 57, 189);
	    glEnd ();
	  break;
	  case 6:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - TR1
	      glVertex2d ( 57, 358);
	      glVertex2d ( 57, 337);
	    glEnd ();
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - XCONN
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 210);
	    glEnd ();
	  break;
	  case 7:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - TR1
	      glVertex2d ( 57, 358);
	      glVertex2d ( 57, 337);
	    glEnd ();
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - GEN1
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 189);
	    glEnd ();
	  break;
	  case 8:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 9:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // XCONN - GEN1
	      glVertex2d ( 57, 210);
	      glVertex2d ( 57, 189);
	    glEnd ();
	  break;
	  case 10:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - XCONN
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 210);
	    glEnd ();
	  break;
	  case 11:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - GEN1
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 189);
	    glEnd ();
	  break;
	  case 12:
	    glBegin (GL_LINES);      // DC BAT - DC ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 358);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - TR1
	      glVertex2d ( 57, 358);
	      glVertex2d ( 57, 337);
	    glEnd ();
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	  break;
	  case 13:
	    glBegin (GL_LINES);      // DC BAT - DC ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 358);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - TR1
	      glVertex2d ( 57, 358);
	      glVertex2d ( 57, 337);
	    glEnd ();
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // XCONN - GEN1
	      glVertex2d ( 57, 210);
	      glVertex2d ( 57, 189);
	    glEnd ();
	  break;
	  case 14:
	    glBegin (GL_LINES);      // DC BAT - DC ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 358);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - TR1
	      glVertex2d ( 57, 358);
	      glVertex2d ( 57, 337);
	    glEnd ();
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - XCONN
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 210);
	    glEnd ();
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print ( 35, 170, "GEN 1", m_Font);
	    gen1P = 0;
	    gen1V = 0;
	    gen1F = 0;
//	    glColor3ub (COLOR_GREEN);
//	    m_pFontManager->Print ( 35, 147, "  0", m_Font); // %
//	    glColor3ub (COLOR_AMBER);
//	    m_pFontManager->Print ( 35, 130, "  0", m_Font); // V
//	    m_pFontManager->Print ( 35, 110, "  0", m_Font); // Hz
	  break;
	  case 15:
	    glBegin (GL_LINES);      // DC BAT - DC ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 358);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - TR1
	      glVertex2d ( 57, 358);
	      glVertex2d ( 57, 337);
	    glEnd ();
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - GEN1
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 189);
	    glEnd ();
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->Print ( 35, 170, "GEN", m_Font);
	    if (sdELConnectRight != 11) glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print ( 35, 170, "    1", m_Font);
	    gen1P =   7;
	    gen1V = 115;
	    gen1F = 400;
//	    glColor3ub (COLOR_GREEN);
//	    m_pFontManager->Print ( 35, 147, "  7", m_Font); // %
//	    m_pFontManager->Print ( 35, 130, "115", m_Font); // V
//	    m_pFontManager->Print ( 35, 110, "400", m_Font); // Hz
	  break;
	  case 16:
	    glBegin (GL_LINES);      // DC BAT - DC ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 358);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 17:
	    glBegin (GL_LINES);      // DC BAT - DC ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 358);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // XCONN - GEN1
	      glVertex2d ( 57, 210);
	      glVertex2d ( 57, 189);
	    glEnd ();
	  break;
	  case 18:
	    glBegin (GL_LINES);      // DC BAT - DC ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 358);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - XCONN
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 210);
	    glEnd ();
	  break;
	  case 19:
	    glBegin (GL_LINES);      // DC BAT - DC ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 358);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - GEN1
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 189);
	    glEnd ();
	  break;
	  case 20:
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	  break;
	  case 21:
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // XCONN - GEN1
	      glVertex2d ( 57, 210);
	      glVertex2d ( 57, 189);
	    glEnd ();
	  break;
	  case 22:
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - XCONN
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 210);
	    glEnd ();
	  break;
	  case 23:
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - GEN1
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 189);
	    glEnd ();
	  break;
	  case 24:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	  break;
	  case 25:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // XCONN - GEN1
	      glVertex2d ( 57, 210);
	      glVertex2d ( 57, 189);
	    glEnd ();
	  break;
	  case 26:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - XCONN
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 210);
	    glEnd ();
	  break;
	  case 27:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC1 - DC BAT/ESS
	      glVertex2d ( 83, 368);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // TR1 - AC1
	      glVertex2d ( 57, 275);
	      glVertex2d ( 57, 248);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC1 Arrow
	      glVertex2d ( 57, 229);
	      glVertex2d ( 53, 220);
	      glVertex2d ( 61, 220);
	      glVertex2d ( 57, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 Arrow - GEN1
	      glVertex2d ( 57, 220);
	      glVertex2d ( 57, 189);
	    glEnd ();
	  break;
	  default:
	  break;
	}

//sdELConnectRight = 24;
	glLineWidth (2);
	glColor3ub (COLOR_GREEN);
	switch (sdELConnectRight) {
	  case 0:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // XCONN - GEN2
	      glVertex2d (402, 210);
	      glVertex2d (402, 189);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // AC2 Arrow
	      glVertex2d (402, 229);
	      glVertex2d (398, 220);
	      glVertex2d (406, 220);
	      glVertex2d (402, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC2 Arrow - XCONN
	      glVertex2d (402, 220);
	      glVertex2d (402, 210);
	    glEnd ();
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // AC2 Arrow
	      glVertex2d (402, 229);
	      glVertex2d (398, 220);
	      glVertex2d (406, 220);
	      glVertex2d (402, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC2 Arrow - GEN2
	      glVertex2d (402, 220);
	      glVertex2d (402, 189);
	    glEnd ();
	  break;
	  case 4:
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	  break;
	  case 5:
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // XCONN - GEN2
	      glVertex2d (402, 210);
	      glVertex2d (402, 189);
	    glEnd ();
	  break;
	  case 6:
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC2 Arrow
	      glVertex2d (402, 229);
	      glVertex2d (398, 220);
	      glVertex2d (406, 220);
	      glVertex2d (402, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC2 Arrow - XCONN
	      glVertex2d (402, 220);
	      glVertex2d (402, 210);
	    glEnd ();
	  break;
	  case 7:
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC2 Arrow
	      glVertex2d (402, 229);
	      glVertex2d (398, 220);
	      glVertex2d (406, 220);
	      glVertex2d (402, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC2 Arrow - GEN2
	      glVertex2d (402, 220);
	      glVertex2d (402, 189);
	    glEnd ();
	  break;
	  case 8:
	    glBegin (GL_LINES);      // DC2 - TR2
	      glVertex2d (402, 358);
	      glVertex2d (402, 337);
	    glEnd ();
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	  break;
	  case 9:
	    glBegin (GL_LINES);      // DC2 - TR2
	      glVertex2d (402, 358);
	      glVertex2d (402, 337);
	    glEnd ();
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // XCONN - GEN2
	      glVertex2d (402, 210);
	      glVertex2d (402, 189);
	    glEnd ();
	  break;
	  case 10:
	    glBegin (GL_LINES);      // DC2 - TR2
	      glVertex2d (402, 358);
	      glVertex2d (402, 337);
	    glEnd ();
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC2 Arrow
	      glVertex2d (402, 229);
	      glVertex2d (398, 220);
	      glVertex2d (406, 220);
	      glVertex2d (402, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC2 Arrow - XCONN
	      glVertex2d (402, 220);
	      glVertex2d (402, 210);
	    glEnd ();
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (380, 170, "GEN 2", m_Font);
	    gen1P = 0;
	    gen1V = 0;
	    gen1F = 0;
//	    glColor3ub (COLOR_GREEN);
//	    m_pFontManager->SetSize(m_Font, 11, 12);
//	    m_pFontManager->Print (380, 147, "  0", m_Font); // %
//	    glColor3ub (COLOR_AMBER);
//	    m_pFontManager->Print (380, 130, "  0", m_Font); // V
//	    m_pFontManager->Print (380, 110, "  0", m_Font); // Hz
	  break;
	  case 11:
	    glBegin (GL_LINES);      // DC2 - TR2
	      glVertex2d (402, 358);
	      glVertex2d (402, 337);
	    glEnd ();
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC2 Arrow
	      glVertex2d (402, 229);
	      glVertex2d (398, 220);
	      glVertex2d (406, 220);
	      glVertex2d (402, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC2 Arrow - GEN2
	      glVertex2d (402, 220);
	      glVertex2d (402, 189);
	    glEnd ();
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->Print (380, 170, "GEN", m_Font);
	    if (sdELConnectLeft != 15) glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (380, 170, "    2", m_Font);
	    gen1P =   7;
	    gen1V = 115;
	    gen1F = 400;
//	    glColor3ub (COLOR_GREEN);
//	    m_pFontManager->SetSize(m_Font, 11, 12);
//	    m_pFontManager->Print (380, 147, "  2", m_Font); // %
//	    m_pFontManager->Print (380, 130, "115", m_Font); // V
//	    m_pFontManager->Print (380, 110, "400", m_Font); // Hz
	  break;
	  case 12:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT/ESS - DC2
	      glVertex2d (210, 368);
	      glVertex2d (376, 368);
	    glEnd ();
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 13:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT/ESS - DC2
	      glVertex2d (210, 368);
	      glVertex2d (376, 368);
	    glEnd ();
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // XCONN - GEN2
	      glVertex2d (402, 210);
	      glVertex2d (402, 189);
	    glEnd ();
	  break;
	  case 14:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT/ESS - DC2
	      glVertex2d (210, 368);
	      glVertex2d (376, 368);
	    glEnd ();
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // AC2 Arrow
	      glVertex2d (402, 229);
	      glVertex2d (398, 220);
	      glVertex2d (406, 220);
	      glVertex2d (402, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC2 Arrow - XCONN
	      glVertex2d (402, 220);
	      glVertex2d (402, 210);
	    glEnd ();
	  break;
	  case 15:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT/ESS - DC2
	      glVertex2d (210, 368);
	      glVertex2d (376, 368);
	    glEnd ();
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // AC2 Arrow
	      glVertex2d (402, 229);
	      glVertex2d (398, 220);
	      glVertex2d (406, 220);
	      glVertex2d (402, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC2 Arrow - GEN2
	      glVertex2d (402, 220);
	      glVertex2d (402, 189);
	    glEnd ();
	  break;
	  case 16:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT/ESS - DC2
	      glVertex2d (210, 368);
	      glVertex2d (376, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	  break;
	  case 17:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT/ESS - DC2
	      glVertex2d (210, 368);
	      glVertex2d (376, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glBegin (GL_LINES);      // XCONN - GEN2
	      glVertex2d (402, 210);
	      glVertex2d (402, 189);
	    glEnd ();
	    glEnd ();
	  break;
	  case 18:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT/ESS - DC2
	      glVertex2d (210, 368);
	      glVertex2d (376, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC2 Arrow
	      glVertex2d (402, 229);
	      glVertex2d (398, 220);
	      glVertex2d (406, 220);
	      glVertex2d (402, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC2 Arrow - XCONN
	      glVertex2d (402, 220);
	      glVertex2d (402, 210);
	    glEnd ();
	  break;
	  case 19:
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT/ESS - DC2
	      glVertex2d (210, 368);
	      glVertex2d (376, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC2 Arrow
	      glVertex2d (402, 229);
	      glVertex2d (398, 220);
	      glVertex2d (406, 220);
	      glVertex2d (402, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC2 Arrow - GEN2
	      glVertex2d (402, 220);
	      glVertex2d (402, 189);
	    glEnd ();
	  break;
	  case 20:
	    glBegin (GL_LINES);      // DC2 - TR2
	      glVertex2d (402, 358);
	      glVertex2d (402, 337);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT/ESS - DC2
	      glVertex2d (210, 368);
	      glVertex2d (376, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	  break;
	  case 21:
	    glBegin (GL_LINES);      // DC2 - TR2
	      glVertex2d (402, 358);
	      glVertex2d (402, 337);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT/ESS - DC2
	      glVertex2d (210, 368);
	      glVertex2d (376, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // XCONN - GEN2
	      glVertex2d (402, 210);
	      glVertex2d (402, 189);
	    glEnd ();
	  break;
	  case 22:
	    glBegin (GL_LINES);      // DC2 - TR2
	      glVertex2d (402, 358);
	      glVertex2d (402, 337);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT/ESS - DC2
	      glVertex2d (210, 368);
	      glVertex2d (376, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC2 Arrow
	      glVertex2d (402, 229);
	      glVertex2d (398, 220);
	      glVertex2d (406, 220);
	      glVertex2d (402, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC2 Arrow - XCONN
	      glVertex2d (402, 220);
	      glVertex2d (402, 210);
	    glEnd ();
	  break;
	  case 23:
	    glBegin (GL_LINES);      // DC2 - TR2
	      glVertex2d (402, 358);
	      glVertex2d (402, 337);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT - DC BAT/ESS
	      glVertex2d (210, 405);
	      glVertex2d (210, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT/ESS - DC2
	      glVertex2d (210, 368);
	      glVertex2d (376, 368);
	    glEnd ();
	    glBegin (GL_LINES);      // TR2 - AC2
	      glVertex2d (402, 275);
	      glVertex2d (402, 248);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC2 Arrow
	      glVertex2d (402, 229);
	      glVertex2d (398, 220);
	      glVertex2d (406, 220);
	      glVertex2d (402, 229);
	    glEnd ();
	    glBegin (GL_LINES);      // AC2 Arrow - GEN2
	      glVertex2d (402, 220);
	      glVertex2d (402, 189);
	    glEnd ();
	  break;
	  default:
	  break;
	}

//sdELConnectCenter = 16;
	switch (sdELConnectCenter) {
	  case 0:
	  break;
	  case 1:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // AC1 - AC ESS
	      glVertex2d ( 83, 238);
	      glVertex2d (194, 238);
	    glEnd ();
	  break;
	  case 2:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // AC ESS - AC2
	      glVertex2d (266, 238);
	      glVertex2d (376, 238);
	    glEnd ();
	  break;
	  case 3:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // AC1 - AC ESS
	      glVertex2d ( 83, 238);
	      glVertex2d (194, 238);
	    glEnd ();
	    glBegin (GL_LINES);      // AC ESS - AC2
	      glVertex2d (266, 238);
	      glVertex2d (376, 238);
	    glEnd ();
	  break;
	  case 4:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // DC ESS Arrow
	      glVertex2d (210, 335);
	      glVertex2d (206, 326);
	      glVertex2d (214, 326);
	      glVertex2d (210, 335);
	    glEnd ();
	    glBegin (GL_LINES);      // DC ESS Arrow - TR ESS
	      glVertex2d (210, 326);
	      glVertex2d (210, 316);
	    glEnd ();
	    glLineWidth (2);
	    glColor3ub (COLOR_WHITE);
	    glBegin (GL_LINE_STRIP); // TR ESS Box
	      glVertex2d (135, 315);
	      glVertex2d (219, 315);
	      glVertex2d (219, 255);
	      glVertex2d (135, 255);
	      glVertex2d (135, 315);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    m_pFontManager->Print (147, 297, "TR ESS", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (200, 278, "V", m_Font);
	    m_pFontManager->Print (200, 261, "A", m_Font);
	    // get and display the load voltage and amperage somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (160, 278, "  0", m_Font);
	    m_pFontManager->Print (160, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // TR ESS - AC ESS
	      glVertex2d (210, 255);
	      glVertex2d (210, 248);
	    glEnd ();
	  break;
	  case 5:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // DC ESS Arrow
	      glVertex2d (210, 335);
	      glVertex2d (206, 326);
	      glVertex2d (214, 326);
	      glVertex2d (210, 335);
	    glEnd ();
	    glBegin (GL_LINES);      // DC ESS Arrow - TR ESS
	      glVertex2d (210, 326);
	      glVertex2d (210, 316);
	    glEnd ();
	    glLineWidth (2);
	    glColor3ub (COLOR_WHITE);
	    glBegin (GL_LINE_STRIP); // TR ESS Box
	      glVertex2d (135, 315);
	      glVertex2d (219, 315);
	      glVertex2d (219, 255);
	      glVertex2d (135, 255);
	      glVertex2d (135, 315);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    m_pFontManager->Print (147, 297, "TR ESS", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (200, 278, "V", m_Font);
	    m_pFontManager->Print (200, 261, "A", m_Font);
	    // get and display the load voltage and amperage somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (160, 278, "  0", m_Font);
	    m_pFontManager->Print (160, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // TR ESS - AC ESS
	      glVertex2d (210, 255);
	      glVertex2d (210, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 - AC ESS
	      glVertex2d ( 83, 238);
	      glVertex2d (194, 238);
	    glEnd ();
	  break;
	  case 6:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // DC ESS Arrow
	      glVertex2d (210, 335);
	      glVertex2d (206, 326);
	      glVertex2d (214, 326);
	      glVertex2d (210, 335);
	    glEnd ();
	    glBegin (GL_LINES);      // DC ESS Arrow - TR ESS
	      glVertex2d (210, 326);
	      glVertex2d (210, 316);
	    glEnd ();
	    glLineWidth (2);
	    glColor3ub (COLOR_WHITE);
	    glBegin (GL_LINE_STRIP); // TR ESS Box
	      glVertex2d (135, 315);
	      glVertex2d (219, 315);
	      glVertex2d (219, 255);
	      glVertex2d (135, 255);
	      glVertex2d (135, 315);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    m_pFontManager->Print (147, 297, "TR ESS", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (200, 278, "V", m_Font);
	    m_pFontManager->Print (200, 261, "A", m_Font);
	    // get and display the load voltage and amperage somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (160, 278, "  0", m_Font);
	    m_pFontManager->Print (160, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // TR ESS - AC ESS
	      glVertex2d (210, 255);
	      glVertex2d (210, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // AC ESS - AC2
	      glVertex2d (266, 238);
	      glVertex2d (376, 238);
	    glEnd ();
	  break;
	  case 7:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // DC ESS Arrow
	      glVertex2d (210, 335);
	      glVertex2d (206, 326);
	      glVertex2d (214, 326);
	      glVertex2d (210, 335);
	    glEnd ();
	    glBegin (GL_LINES);      // DC ESS Arrow - TR ESS
	      glVertex2d (210, 326);
	      glVertex2d (210, 316);
	    glEnd ();
	    glLineWidth (2);
	    glColor3ub (COLOR_WHITE);
	    glBegin (GL_LINE_STRIP); // TR ESS Box
	      glVertex2d (135, 315);
	      glVertex2d (219, 315);
	      glVertex2d (219, 255);
	      glVertex2d (135, 255);
	      glVertex2d (135, 315);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    m_pFontManager->Print (147, 297, "TR ESS", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (200, 278, "V", m_Font);
	    m_pFontManager->Print (200, 261, "A", m_Font);
	    // get and display the load voltage and amperage somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (160, 278, "  0", m_Font);
	    m_pFontManager->Print (160, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // TR ESS - AC ESS
	      glVertex2d (210, 255);
	      glVertex2d (210, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 - AC ESS
	      glVertex2d ( 83, 238);
	      glVertex2d (194, 238);
	    glEnd ();
	    glBegin (GL_LINES);      // AC ESS - AC2
	      glVertex2d (266, 238);
	      glVertex2d (376, 238);
	    glEnd ();
	  break;
	  case 8:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // DC ESS Arrow
	      glVertex2d (210, 335);
	      glVertex2d (206, 326);
	      glVertex2d (214, 326);
	      glVertex2d (210, 335);
	    glEnd ();
	    glBegin (GL_LINES);      // DC ESS Arrow - TR ESS
	      glVertex2d (210, 326);
	      glVertex2d (210, 316);
	    glEnd ();
	    glLineWidth (2);
	    glColor3ub (COLOR_WHITE);
	    glBegin (GL_LINE_STRIP); // TR ESS Box
	      glVertex2d (135, 315);
	      glVertex2d (219, 315);
	      glVertex2d (219, 255);
	      glVertex2d (135, 255);
	      glVertex2d (135, 315);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    m_pFontManager->Print (147, 297, "TR ESS", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (200, 278, "V", m_Font);
	    m_pFontManager->Print (200, 261, "A", m_Font);
	    // get and display the load voltage and amperage somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (160, 278, "  0", m_Font);
	    m_pFontManager->Print (160, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // TR ESS Arrow
	      glVertex2d (219, 285);
	      glVertex2d (228, 281);
	      glVertex2d (228, 289);
	      glVertex2d (219, 285);
	    glEnd ();
	    glBegin (GL_LINES);      // TR ESS Arrow - EMER GEN
	      glVertex2d (228, 285);
	      glVertex2d (245, 285);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // EMER GEN Box
	      glVertex2d (245, 325);
	      glVertex2d (330, 325);
	      glVertex2d (330, 265);
	      glVertex2d (245, 265);
	      glVertex2d (245, 325);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (250, 308, "EMER GEN", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (310, 288, "V", m_Font);
	    m_pFontManager->Print (305, 270, "Hz", m_Font);
	    // get and display the output voltage and frequency somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (165, 278, "  0", m_Font);
	    m_pFontManager->Print (165, 261, "  0", m_Font);
	  break;
	  case 9:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // DC ESS Arrow
	      glVertex2d (210, 335);
	      glVertex2d (206, 326);
	      glVertex2d (214, 326);
	      glVertex2d (210, 335);
	    glEnd ();
	    glBegin (GL_LINES);      // DC ESS Arrow - TR ESS
	      glVertex2d (210, 326);
	      glVertex2d (210, 316);
	    glEnd ();
	    glLineWidth (2);
	    glColor3ub (COLOR_WHITE);
	    glBegin (GL_LINE_STRIP); // TR ESS Box
	      glVertex2d (135, 315);
	      glVertex2d (219, 315);
	      glVertex2d (219, 255);
	      glVertex2d (135, 255);
	      glVertex2d (135, 315);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    m_pFontManager->Print (147, 297, "TR ESS", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (200, 278, "V", m_Font);
	    m_pFontManager->Print (200, 261, "A", m_Font);
	    // get and display the load voltage and amperage somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (160, 278, "  0", m_Font);
	    m_pFontManager->Print (160, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // TR ESS Arrow
	      glVertex2d (219, 285);
	      glVertex2d (228, 281);
	      glVertex2d (228, 289);
	      glVertex2d (219, 285);
	    glEnd ();
	    glBegin (GL_LINES);      // TR ESS Arrow - EMER GEN
	      glVertex2d (228, 285);
	      glVertex2d (245, 285);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // EMER GEN Box
	      glVertex2d (245, 325);
	      glVertex2d (330, 325);
	      glVertex2d (330, 265);
	      glVertex2d (245, 265);
	      glVertex2d (245, 325);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (250, 308, "EMER GEN", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (310, 288, "V", m_Font);
	    m_pFontManager->Print (305, 270, "Hz", m_Font);
	    // get and display the output voltage and frequency somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (165, 278, "  0", m_Font);
	    m_pFontManager->Print (165, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // AC1 - AC ESS
	      glVertex2d ( 83, 238);
	      glVertex2d (194, 238);
	    glEnd ();
	  break;
	  case 10:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // DC ESS Arrow
	      glVertex2d (210, 335);
	      glVertex2d (206, 326);
	      glVertex2d (214, 326);
	      glVertex2d (210, 335);
	    glEnd ();
	    glBegin (GL_LINES);      // DC ESS Arrow - TR ESS
	      glVertex2d (210, 326);
	      glVertex2d (210, 316);
	    glEnd ();
	    glLineWidth (2);
	    glColor3ub (COLOR_WHITE);
	    glBegin (GL_LINE_STRIP); // TR ESS Box
	      glVertex2d (135, 315);
	      glVertex2d (219, 315);
	      glVertex2d (219, 255);
	      glVertex2d (135, 255);
	      glVertex2d (135, 315);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    m_pFontManager->Print (147, 297, "TR ESS", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (200, 278, "V", m_Font);
	    m_pFontManager->Print (200, 261, "A", m_Font);
	    // get and display the load voltage and amperage somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (160, 278, "  0", m_Font);
	    m_pFontManager->Print (160, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // TR ESS Arrow
	      glVertex2d (219, 285);
	      glVertex2d (228, 281);
	      glVertex2d (228, 289);
	      glVertex2d (219, 285);
	    glEnd ();
	    glBegin (GL_LINES);      // TR ESS Arrow - EMER GEN
	      glVertex2d (228, 285);
	      glVertex2d (245, 285);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // EMER GEN Box
	      glVertex2d (245, 325);
	      glVertex2d (330, 325);
	      glVertex2d (330, 265);
	      glVertex2d (245, 265);
	      glVertex2d (245, 325);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (250, 308, "EMER GEN", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (310, 288, "V", m_Font);
	    m_pFontManager->Print (305, 270, "Hz", m_Font);
	    // get and display the output voltage and frequency somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (165, 278, "  0", m_Font);
	    m_pFontManager->Print (165, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // AC ESS - AC2
	      glVertex2d (266, 238);
	      glVertex2d (376, 238);
	    glEnd ();
	  break;
	  case 11:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // DC ESS Arrow
	      glVertex2d (210, 335);
	      glVertex2d (206, 326);
	      glVertex2d (214, 326);
	      glVertex2d (210, 335);
	    glEnd ();
	    glBegin (GL_LINES);      // DC ESS Arrow - TR ESS
	      glVertex2d (210, 326);
	      glVertex2d (210, 316);
	    glEnd ();
	    glLineWidth (2);
	    glColor3ub (COLOR_WHITE);
	    glBegin (GL_LINE_STRIP); // TR ESS Box
	      glVertex2d (135, 315);
	      glVertex2d (219, 315);
	      glVertex2d (219, 255);
	      glVertex2d (135, 255);
	      glVertex2d (135, 315);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    m_pFontManager->Print (147, 297, "TR ESS", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (200, 278, "V", m_Font);
	    m_pFontManager->Print (200, 261, "A", m_Font);
	    // get and display the load voltage and amperage somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (160, 278, "  0", m_Font);
	    m_pFontManager->Print (160, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // TR ESS Arrow
	      glVertex2d (219, 285);
	      glVertex2d (228, 281);
	      glVertex2d (228, 289);
	      glVertex2d (219, 285);
	    glEnd ();
	    glBegin (GL_LINES);      // TR ESS Arrow - EMER GEN
	      glVertex2d (228, 285);
	      glVertex2d (245, 285);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // EMER GEN Box
	      glVertex2d (245, 325);
	      glVertex2d (330, 325);
	      glVertex2d (330, 265);
	      glVertex2d (245, 265);
	      glVertex2d (245, 325);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (250, 308, "EMER GEN", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (310, 288, "V", m_Font);
	    m_pFontManager->Print (305, 270, "Hz", m_Font);
	    // get and display the output voltage and frequency somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (165, 278, "  0", m_Font);
	    m_pFontManager->Print (165, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // AC1 - AC ESS
	      glVertex2d ( 83, 238);
	      glVertex2d (194, 238);
	    glEnd ();
	    glBegin (GL_LINES);      // AC ESS - AC2
	      glVertex2d (266, 238);
	      glVertex2d (376, 238);
	    glEnd ();
	  break;
	  case 12:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // DC ESS Arrow
	      glVertex2d (210, 335);
	      glVertex2d (206, 326);
	      glVertex2d (214, 326);
	      glVertex2d (210, 335);
	    glEnd ();
	    glBegin (GL_LINES);      // DC ESS Arrow - TR ESS
	      glVertex2d (210, 326);
	      glVertex2d (210, 316);
	    glEnd ();
	    glLineWidth (2);
	    glColor3ub (COLOR_WHITE);
	    glBegin (GL_LINE_STRIP); // TR ESS Box
	      glVertex2d (135, 315);
	      glVertex2d (219, 315);
	      glVertex2d (219, 255);
	      glVertex2d (135, 255);
	      glVertex2d (135, 315);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    m_pFontManager->Print (147, 297, "TR ESS", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (200, 278, "V", m_Font);
	    m_pFontManager->Print (200, 261, "A", m_Font);
	    // get and display the load voltage and amperage somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (160, 278, "  0", m_Font);
	    m_pFontManager->Print (160, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // EMER GEN Box
	      glVertex2d (245, 325);
	      glVertex2d (330, 325);
	      glVertex2d (330, 265);
	      glVertex2d (245, 265);
	      glVertex2d (245, 325);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (250, 308, "EMER GEN", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (310, 288, "V", m_Font);
	    m_pFontManager->Print (305, 270, "Hz", m_Font);
	    // get and display the output voltage and frequency somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (165, 278, "  0", m_Font);
	    m_pFontManager->Print (165, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // EMER GEN - AC ESS Arrow
	      glVertex2d (255, 265);
	      glVertex2d (255, 257);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC ESS Arrow
	      glVertex2d (255, 248);
	      glVertex2d (251, 257);
	      glVertex2d (259, 257);
	      glVertex2d (255, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // TR ESS - AC ESS
	      glVertex2d (210, 255);
	      glVertex2d (210, 248);
	    glEnd ();
	  break;
	  case 13:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // DC ESS Arrow
	      glVertex2d (210, 335);
	      glVertex2d (206, 326);
	      glVertex2d (214, 326);
	      glVertex2d (210, 335);
	    glEnd ();
	    glBegin (GL_LINES);      // DC ESS Arrow - TR ESS
	      glVertex2d (210, 326);
	      glVertex2d (210, 316);
	    glEnd ();
	    glLineWidth (2);
	    glColor3ub (COLOR_WHITE);
	    glBegin (GL_LINE_STRIP); // TR ESS Box
	      glVertex2d (135, 315);
	      glVertex2d (219, 315);
	      glVertex2d (219, 255);
	      glVertex2d (135, 255);
	      glVertex2d (135, 315);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    m_pFontManager->Print (147, 297, "TR ESS", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (200, 278, "V", m_Font);
	    m_pFontManager->Print (200, 261, "A", m_Font);
	    // get and display the load voltage and amperage somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (160, 278, "  0", m_Font);
	    m_pFontManager->Print (160, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // EMER GEN Box
	      glVertex2d (245, 325);
	      glVertex2d (330, 325);
	      glVertex2d (330, 265);
	      glVertex2d (245, 265);
	      glVertex2d (245, 325);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (250, 308, "EMER GEN", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (310, 288, "V", m_Font);
	    m_pFontManager->Print (305, 270, "Hz", m_Font);
	    // get and display the output voltage and frequency somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (165, 278, "  0", m_Font);
	    m_pFontManager->Print (165, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // EMER GEN - AC ESS Arrow
	      glVertex2d (255, 265);
	      glVertex2d (255, 257);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC ESS Arrow
	      glVertex2d (255, 248);
	      glVertex2d (251, 257);
	      glVertex2d (259, 257);
	      glVertex2d (255, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // TR ESS - AC ESS
	      glVertex2d (210, 255);
	      glVertex2d (210, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 - AC ESS
	      glVertex2d ( 83, 238);
	      glVertex2d (194, 238);
	    glEnd ();
	  break;
	  case 14:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // DC ESS Arrow
	      glVertex2d (210, 335);
	      glVertex2d (206, 326);
	      glVertex2d (214, 326);
	      glVertex2d (210, 335);
	    glEnd ();
	    glBegin (GL_LINES);      // DC ESS Arrow - TR ESS
	      glVertex2d (210, 326);
	      glVertex2d (210, 316);
	    glEnd ();
	    glLineWidth (2);
	    glColor3ub (COLOR_WHITE);
	    glBegin (GL_LINE_STRIP); // TR ESS Box
	      glVertex2d (135, 315);
	      glVertex2d (219, 315);
	      glVertex2d (219, 255);
	      glVertex2d (135, 255);
	      glVertex2d (135, 315);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    m_pFontManager->Print (147, 297, "TR ESS", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (200, 278, "V", m_Font);
	    m_pFontManager->Print (200, 261, "A", m_Font);
	    // get and display the load voltage and amperage somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (160, 278, "  0", m_Font);
	    m_pFontManager->Print (160, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // EMER GEN Box
	      glVertex2d (245, 325);
	      glVertex2d (330, 325);
	      glVertex2d (330, 265);
	      glVertex2d (245, 265);
	      glVertex2d (245, 325);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (250, 308, "EMER GEN", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (310, 288, "V", m_Font);
	    m_pFontManager->Print (305, 270, "Hz", m_Font);
	    // get and display the output voltage and frequency somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (165, 278, "  0", m_Font);
	    m_pFontManager->Print (165, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // EMER GEN - AC ESS Arrow
	      glVertex2d (255, 265);
	      glVertex2d (255, 257);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC ESS Arrow
	      glVertex2d (255, 248);
	      glVertex2d (251, 257);
	      glVertex2d (259, 257);
	      glVertex2d (255, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // TR ESS - AC ESS
	      glVertex2d (210, 255);
	      glVertex2d (210, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // AC ESS - AC2
	      glVertex2d (266, 238);
	      glVertex2d (376, 238);
	    glEnd ();
	  break;
	  case 15:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // DC ESS Arrow
	      glVertex2d (210, 335);
	      glVertex2d (206, 326);
	      glVertex2d (214, 326);
	      glVertex2d (210, 335);
	    glEnd ();
	    glBegin (GL_LINES);      // DC ESS Arrow - TR ESS
	      glVertex2d (210, 326);
	      glVertex2d (210, 316);
	    glEnd ();
	    glLineWidth (2);
	    glColor3ub (COLOR_WHITE);
	    glBegin (GL_LINE_STRIP); // TR ESS Box
	      glVertex2d (135, 315);
	      glVertex2d (219, 315);
	      glVertex2d (219, 255);
	      glVertex2d (135, 255);
	      glVertex2d (135, 315);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 12, 12);
	    m_pFontManager->Print (147, 297, "TR ESS", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (200, 278, "V", m_Font);
	    m_pFontManager->Print (200, 261, "A", m_Font);
	    // get and display the load voltage and amperage somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (160, 278, "  0", m_Font);
	    m_pFontManager->Print (160, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // EMER GEN Box
	      glVertex2d (245, 325);
	      glVertex2d (330, 325);
	      glVertex2d (330, 265);
	      glVertex2d (245, 265);
	      glVertex2d (245, 325);
	    glEnd ();
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (250, 308, "EMER GEN", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->SetSize(m_Font, 11, 12);
	    m_pFontManager->Print (310, 288, "V", m_Font);
	    m_pFontManager->Print (305, 270, "Hz", m_Font);
	    // get and display the output voltage and frequency somewhere...
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (165, 278, "  0", m_Font);
	    m_pFontManager->Print (165, 261, "  0", m_Font);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // EMER GEN - AC ESS Arrow
	      glVertex2d (255, 265);
	      glVertex2d (255, 257);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // AC ESS Arrow
	      glVertex2d (255, 248);
	      glVertex2d (251, 257);
	      glVertex2d (259, 257);
	      glVertex2d (255, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // TR ESS - AC ESS
	      glVertex2d (210, 255);
	      glVertex2d (210, 248);
	    glEnd ();
	    glBegin (GL_LINES);      // AC1 - AC ESS
	      glVertex2d ( 83, 238);
	      glVertex2d (194, 238);
	    glEnd ();
	    glBegin (GL_LINES);      // AC ESS - AC2
	      glVertex2d (266, 238);
	      glVertex2d (376, 238);
	    glEnd ();
	  break;
	  default:
	  break;
	}

//sdELBatterySupply = 16;
	glLineWidth (2);
	glColor3ub (COLOR_GREEN);
	switch (sdELBatterySupply) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	    bat1V = 28;
	    bat1A =  0;
	    bat2V = 28;
	    bat2A =  0;
	  break;
	  case 1:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // BATT1 - DC BAT
	      glVertex2d (163, 414);
	      glVertex2d (194, 414);
	    glEnd ();
	  break;
	  case 2:
	    glLineWidth (2);
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // BATT1 - BATT1 Arrow Right
	      glVertex2d (163, 414);
	      glVertex2d (185, 414);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // BATT1 Arrow Right
	      glVertex2d (194, 414);
	      glVertex2d (185, 410);
	      glVertex2d (185, 418);
	      glVertex2d (194, 414);
	    glEnd ();
	    bat1V = 28;
	    bat1A = 0;
	    bat2V = 28;
	    bat2A = 0;
	  break;
	  case 3:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // BATT1 Arrow Left
	      glVertex2d (163, 414);
	      glVertex2d (172, 410);
	      glVertex2d (172, 418);
	      glVertex2d (163, 414);
	    glEnd ();
	    glBegin (GL_LINES);      // BATT1 Arrow Left - DC BAT
	      glVertex2d (172, 414);
	      glVertex2d (194, 414);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    bat1V =  29;
	    bat1A = -29;
	    bat2V =  29;
	    bat2A = -29;
	  break;
	  case 4:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // DC BAT - BATT2
	      glVertex2d (266, 414);
	      glVertex2d (296, 414);
	    glEnd ();
	  break;
	  case 5:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // BATT1 - DC BAT
	      glVertex2d (163, 414);
	      glVertex2d (194, 414);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT - BATT2
	      glVertex2d (266, 414);
	      glVertex2d (296, 414);
	    glEnd ();
	  break;
	  case 6:
	    glLineWidth (2);
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // BATT1 - BATT1 Arrow Right
	      glVertex2d (163, 414);
	      glVertex2d (185, 414);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // BATT1 Arrow Right
	      glVertex2d (194, 414);
	      glVertex2d (185, 410);
	      glVertex2d (185, 418);
	      glVertex2d (194, 414);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // DC BAT - BATT2
	      glVertex2d (266, 414);
	      glVertex2d (296, 414);
	    glEnd ();
	  break;
	  case 7:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // BATT1 Arrow Left
	      glVertex2d (163, 414);
	      glVertex2d (172, 410);
	      glVertex2d (172, 418);
	      glVertex2d (163, 414);
	    glEnd ();
	    glBegin (GL_LINES);      // BATT1 Arrow Left - DC BAT
	      glVertex2d (172, 414);
	      glVertex2d (194, 414);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT - BATT2
	      glVertex2d (266, 414);
	      glVertex2d (296, 414);
	    glEnd ();
	  break;
	  case 8:
	    glLineWidth (2);
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP); // BATT2 Arrow Left
	      glVertex2d (266, 414);
	      glVertex2d (275, 410);
	      glVertex2d (275, 418);
	      glVertex2d (266, 414);
	    glEnd ();
	    glBegin (GL_LINES);      // BATT2 Arrow Left - DC BAT
	      glVertex2d (275, 414);
	      glVertex2d (296, 414);
	    glEnd ();
	    bat1V = 28;
	    bat1A = 0;
	    bat2V = 28;
	    bat2A = 0;
	  break;
	  case 9:
	    glLineWidth (2);
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP); // BATT2 Arrow Left
	      glVertex2d (266, 414);
	      glVertex2d (275, 410);
	      glVertex2d (275, 418);
	      glVertex2d (266, 414);
	    glEnd ();
	    glBegin (GL_LINES);      // BATT2 Arrow Left - DC BAT
	      glVertex2d (275, 414);
	      glVertex2d (296, 414);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // BATT1 - DC BAT
	      glVertex2d (163, 414);
	      glVertex2d (194, 414);
	    glEnd ();
	  break;
	  case 10:
	    glLineWidth (2);
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // BATT1 - BATT1 Arrow Right
	      glVertex2d (163, 414);
	      glVertex2d (185, 414);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // BATT1 Arrow Right
	      glVertex2d (194, 414);
	      glVertex2d (185, 410);
	      glVertex2d (185, 418);
	      glVertex2d (194, 414);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // BATT2 Arrow Left
	      glVertex2d (266, 414);
	      glVertex2d (275, 410);
	      glVertex2d (275, 418);
	      glVertex2d (266, 414);
	    glEnd ();
	    glBegin (GL_LINES);      // BATT2 Arrow Left - DC BAT
	      glVertex2d (275, 414);
	      glVertex2d (296, 414);
	    glEnd ();
	  break;
	  case 11:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // BATT1 Arrow Left
	      glVertex2d (163, 414);
	      glVertex2d (172, 410);
	      glVertex2d (172, 418);
	      glVertex2d (163, 414);
	    glEnd ();
	    glBegin (GL_LINES);      // BATT1 Arrow Left - DC BAT
	      glVertex2d (172, 414);
	      glVertex2d (194, 414);
	    glEnd ();
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP); // BATT2 Arrow Left
	      glVertex2d (266, 414);
	      glVertex2d (275, 410);
	      glVertex2d (275, 418);
	      glVertex2d (266, 414);
	    glEnd ();
	    glBegin (GL_LINES);      // BATT2 Arrow Left - DC BAT
	      glVertex2d (275, 414);
	      glVertex2d (296, 414);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 12:
	    glLineWidth (2);
	    glBegin (GL_LINES);      // DC BAT - BATT2 Arrow Right
	      glVertex2d (266, 414);
	      glVertex2d (287, 414);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // BATT2 Arrow Right
	      glVertex2d (296, 414);
	      glVertex2d (287, 410);
	      glVertex2d (287, 418);
	      glVertex2d (296, 414);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    bat2V =  29;
	    bat2A = -29;
	  break;
	  case 13:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // DC BAT - BATT2 Arrow Right
	      glVertex2d (266, 414);
	      glVertex2d (287, 414);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // BATT2 Arrow Right
	      glVertex2d (296, 414);
	      glVertex2d (287, 410);
	      glVertex2d (287, 418);
	      glVertex2d (296, 414);
	    glEnd ();
	    glBegin (GL_LINES);      // BATT1 - DC BAT
	      glVertex2d (163, 414);
	      glVertex2d (194, 414);
	    glEnd ();
	  break;
	  case 14:
	    glLineWidth (2);
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINES);      // BATT1 - BATT1 Arrow Right
	      glVertex2d (163, 414);
	      glVertex2d (185, 414);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // BATT1 Arrow Right
	      glVertex2d (194, 414);
	      glVertex2d (185, 410);
	      glVertex2d (185, 418);
	      glVertex2d (194, 414);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINES);      // DC BAT - BATT2 Arrow Right
	      glVertex2d (266, 414);
	      glVertex2d (287, 414);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // BATT2 Arrow Right
	      glVertex2d (296, 414);
	      glVertex2d (287, 410);
	      glVertex2d (287, 418);
	      glVertex2d (296, 414);
	    glEnd ();
	  break;
	  case 15:
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP); // BATT1 Arrow Left
	      glVertex2d (163, 414);
	      glVertex2d (172, 410);
	      glVertex2d (172, 418);
	      glVertex2d (163, 414);
	    glEnd ();
	    glBegin (GL_LINES);      // BATT1 Arrow Left - DC BAT
	      glVertex2d (172, 414);
	      glVertex2d (194, 414);
	    glEnd ();
	    glBegin (GL_LINES);      // DC BAT - BATT2 Arrow Right
	      glVertex2d (266, 414);
	      glVertex2d (287, 414);
	    glEnd ();
	    glBegin (GL_LINE_STRIP); // BATT2 Arrow Right
	      glVertex2d (296, 414);
	      glVertex2d (287, 410);
	      glVertex2d (287, 418);
	      glVertex2d (296, 414);
	    glEnd ();
	    glColor3ub (COLOR_GREEN);
	    bat1V =  29;
	    bat1A = -29;
	    bat2V =  29;
	    bat2A = -29;
	  break;
	  default:
	  break;
	}

	// The boxes

	glLineWidth (2);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP); // Batt 1
	  glVertex2d ( 90, 446);
	  glVertex2d (163, 446);
	  glVertex2d (163, 384);
	  glVertex2d ( 90, 384);
	  glVertex2d ( 90, 446);
	glEnd ();
	m_pFontManager->SetSize(m_Font, 12, 12);
	m_pFontManager->Print (104, 428, "BAT 1", m_Font);
	if (ohpElecBat1 == 0) {
	  m_pFontManager->SetSize(m_Font, 18, 18);
	  m_pFontManager->Print (107, 400, "OFF", m_Font);
	} else {
	// get the battery voltage and amperage somewhere...
	  glColor3ub (COLOR_CYAN);
	  m_pFontManager->SetSize(m_Font, 12, 12);
	  m_pFontManager->Print (144, 408, "V", m_Font);
	  m_pFontManager->Print (144, 390, "A", m_Font);
	  glColor3ub (COLOR_GREEN);
	  sprintf (buffer, "%3i", bat1V);
	  m_pFontManager->Print (104, 408, buffer, m_Font);
	  sprintf (buffer, "%3i", bat1A);
	  m_pFontManager->Print (104, 390, buffer, m_Font);
	}

	glLineWidth (2);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP); // Batt 2
	  glVertex2d (296, 446);
	  glVertex2d (370, 446);
	  glVertex2d (370, 384);
	  glVertex2d (296, 384);
	  glVertex2d (296, 446);
	glEnd ();
	m_pFontManager->SetSize(m_Font, 12, 12);
	m_pFontManager->Print (310, 428, "BAT 2", m_Font);
	if (ohpElecBat2 == 0) {
	  m_pFontManager->SetSize(m_Font, 18, 18);
	  m_pFontManager->Print (313, 400, "OFF", m_Font);
	} else {
	// get the battery voltage and amperage somewhere...
	  glColor3ub (COLOR_CYAN);
	  m_pFontManager->SetSize(m_Font, 12, 12);
	  m_pFontManager->Print (350, 408, "V", m_Font);
	  m_pFontManager->Print (350, 390, "A", m_Font);
	  glColor3ub (COLOR_GREEN);
	  sprintf (buffer, "%3i", bat2V);
	  m_pFontManager->Print (314, 408, buffer, m_Font);
	  sprintf (buffer, "%3i", bat2A);
	  m_pFontManager->Print (314, 390, buffer, m_Font);
	}

	glLineWidth (2);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP); // TR 1
	  glVertex2d ( 20, 337);
	  glVertex2d ( 94, 337);
	  glVertex2d ( 94, 275);
	  glVertex2d ( 20, 275);
	  glVertex2d ( 20, 337);
	glEnd ();
	m_pFontManager->SetSize(m_Font, 12, 12);
	m_pFontManager->Print ( 40, 319, "TR 1", m_Font);
	glColor3ub (COLOR_CYAN);
	m_pFontManager->SetSize(m_Font, 12, 12);
	m_pFontManager->Print ( 71, 299, "V", m_Font);
	m_pFontManager->Print ( 71, 281, "A", m_Font);
	// get the load voltage and amperage somewhere...
	m_pFontManager->Print ( 30, 299, " 29", m_Font); // V
	m_pFontManager->Print ( 30, 281, " 26", m_Font); // A

	glLineWidth (2);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP); // TR 2
	  glVertex2d (365, 337);
	  glVertex2d (439, 337);
	  glVertex2d (439, 275);
	  glVertex2d (365, 275);
	  glVertex2d (365, 337);
	glEnd ();
	m_pFontManager->SetSize(m_Font, 12, 12);
	m_pFontManager->Print (385, 319, "TR 2", m_Font);
	glColor3ub (COLOR_CYAN);
	m_pFontManager->SetSize(m_Font, 12, 12);
	m_pFontManager->Print (416, 299, "V", m_Font);
	m_pFontManager->Print (416, 281, "A", m_Font);
	// get the load voltage and amperage somewhere...
	m_pFontManager->Print (375, 299, " 29", m_Font); // V
	m_pFontManager->Print (375, 281, " 22", m_Font); // A

	glLineWidth (2);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP); // Gen 1
	  glVertex2d ( 20, 188);
	  glVertex2d ( 94, 188);
	  glVertex2d ( 94, 104);
	  glVertex2d ( 20, 104);
	  glVertex2d ( 20, 188);
	glEnd ();
	if (ohpElecGen1 == 0) {
	  m_pFontManager->SetSize(m_Font, 12, 12);
	  glColor3ub (COLOR_AMBER);
	  m_pFontManager->Print ( 35, 170, "GEN 1", m_Font);
	  m_pFontManager->SetSize(m_Font, 18, 18);
	  glColor3ub (COLOR_WHITE);
	  m_pFontManager->Print ( 35, 130, "OFF", m_Font);
	} else { // title in individual cases...
	  m_pFontManager->SetSize(m_Font, 12, 12);
	  glColor3ub (COLOR_CYAN);
	  m_pFontManager->Print ( 75, 147, "%", m_Font);
	  m_pFontManager->Print ( 75, 130, "V", m_Font);
	  m_pFontManager->Print ( 70, 110, "Hz", m_Font);
	  m_pFontManager->SetSize(m_Font, 11, 12);
	  // get the load percentage, voltage and frequency somewhere...
	  glColor3ub (COLOR_GREEN);
	  sprintf (buffer, "%3i", gen1P);
	  m_pFontManager->Print ( 35, 147, buffer, m_Font); // %
	  if (gen1V == 0) glColor3ub (COLOR_AMBER);
	  sprintf (buffer, "%3i", gen1V);
	  m_pFontManager->Print ( 35, 130, buffer, m_Font); // V
	  sprintf (buffer, "%3i", gen1F);
	  m_pFontManager->Print ( 35, 110, buffer, m_Font); // Hz
	}

	glLineWidth (2);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP); // Gen 2
	  glVertex2d (365, 188);
	  glVertex2d (440, 188);
	  glVertex2d (440, 104);
	  glVertex2d (365, 104);
	  glVertex2d (365, 188);
	glEnd ();
	if (ohpElecGen2 == 0) {
	  glColor3ub (COLOR_AMBER);
	  m_pFontManager->SetSize(m_Font, 12, 12);
	  m_pFontManager->Print (380, 170, "GEN 2", m_Font);
	  glColor3ub (COLOR_WHITE);
	  m_pFontManager->SetSize(m_Font, 18, 18);
	  m_pFontManager->Print (380, 130, "OFF", m_Font);
	} else { // title in individual cases...
	glColor3ub (COLOR_GRAY50);
	  glColor3ub (COLOR_CYAN);
	  m_pFontManager->SetSize(m_Font, 12, 12);
	  m_pFontManager->Print (420, 147, "%", m_Font);
	  m_pFontManager->Print (420, 130, "V", m_Font);
	  m_pFontManager->Print (415, 110, "Hz", m_Font);
	  m_pFontManager->SetSize(m_Font, 11, 12);
	  // get the load percentage, voltage and frequency somewhere...
	  glColor3ub (COLOR_GREEN);
	  sprintf (buffer, "%3i", gen2P);
	  m_pFontManager->Print (380, 147, buffer, m_Font); // %
	  if (gen2V == 0) glColor3ub (COLOR_AMBER);
	  sprintf (buffer, "%3i", gen2V);
	  m_pFontManager->Print (380, 130, buffer, m_Font); // V
	  sprintf (buffer, "%3i", gen2F);
	  m_pFontManager->Print (380, 110, buffer, m_Font); // Hz
	}

	glLineWidth (2);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP); // APU Gen
	  glVertex2d (135, 188);
	  glVertex2d (209, 188);
	  glVertex2d (209, 104);
	  glVertex2d (135, 104);
	  glVertex2d (135, 188);
	glEnd ();
	m_pFontManager->SetSize(m_Font, 11, 12);
	switch (sdApuBox) {
	  case 0: // master off
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->Print (140, 170, "APU GEN", m_Font);
	  break;
	  case 1: // master on; available
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->Print (140, 170, "APU GEN", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->Print (190, 147, "%", m_Font);
	    m_pFontManager->Print (190, 130, "V", m_Font);
	    m_pFontManager->Print (185, 110, "Hz", m_Font);
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (150, 147, "  0", m_Font);
	    m_pFontManager->Print (150, 130, "115", m_Font);
	    m_pFontManager->Print (150, 110, "400", m_Font);
	  break;
	  case 2: // ?
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (140, 170, "APU GEN", m_Font);
	    glColor3ub (COLOR_WHITE);
	    m_pFontManager->SetSize(m_Font, 18, 18);
	    m_pFontManager->Print (150, 130, "OFF", m_Font);
	  break;
	  case 3: // master on; N < 95
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (140, 170, "APU GEN", m_Font);
	    glColor3ub (COLOR_CYAN);
	    m_pFontManager->Print (190, 147, "%", m_Font);
	    m_pFontManager->Print (190, 130, "V", m_Font);
	    m_pFontManager->Print (185, 110, "Hz", m_Font);
	    glColor3ub (COLOR_GREEN);
	    m_pFontManager->Print (150, 147, buffer, m_Font);
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (150, 130, buffer, m_Font);
	    m_pFontManager->Print (150, 110, buffer, m_Font);
	  break;
	  default:
	  break;
	}

	if ((ohpElecExtPwr != 0) && (sdExtPowBox > 0)) { // Ext Pwr
	  glLineWidth (2);
	  glColor3ub (COLOR_WHITE);
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (250, 167);
	    glVertex2d (324, 167);
	    glVertex2d (324, 104);
	    glVertex2d (250, 104);
	    glVertex2d (250, 167);
	  glEnd ();
	  glColor3ub (COLOR_CYAN);
	  m_pFontManager->SetSize(m_Font, 11, 12);
	  m_pFontManager->Print (305, 130, "V", m_Font);
	  m_pFontManager->Print (300, 110, "Hz", m_Font);
	  switch (sdExtPowBox) {
	    case 1:
	      glColor3ub (COLOR_WHITE);
	      m_pFontManager->Print (255, 150, "EXT POW", m_Font);
	      glColor3ub (COLOR_GREEN);
	      m_pFontManager->Print (265, 130, "115", m_Font);
	      m_pFontManager->Print (265, 110, "400", m_Font);
	    break;
	    case 2:
	      glColor3ub (COLOR_AMBER);
	      m_pFontManager->Print (255, 150, "EXT POW", m_Font);
	      m_pFontManager->Print (265, 130, "  0", m_Font);
	      m_pFontManager->Print (265, 110, "  0", m_Font);
	    break;
	    case 3:
	      glColor3ub (COLOR_WHITE);
	      m_pFontManager->Print (255, 150, "STAT INV", m_Font);
	      glColor3ub (COLOR_GREEN);
	      m_pFontManager->Print (265, 130, "115", m_Font);
	      m_pFontManager->Print (265, 110, "400", m_Font);
	    break;
	    case 4:
	      glColor3ub (COLOR_AMBER);
	      m_pFontManager->Print (255, 150, "STAT INV", m_Font);
	      m_pFontManager->Print (265, 130, "  0", m_Font);
	      m_pFontManager->Print (265, 110, "  0", m_Font);
	    break;
	    default:
	    break;
	  }
	}

	glColor3ub (COLOR_YELLOW);
	m_pFontManager->SetSize(m_Font, 14, 14);
	sprintf(buffer, "%i", sdACCrossConnect);
	m_pFontManager->Print (225, 180, buffer, m_Font); // Testing!

	glColor3ub (COLOR_YELLOW);
	m_pFontManager->SetSize(m_Font, 14, 14);
	sprintf(buffer, "%i", sdELConnectLeft);
	m_pFontManager->Print ( 30, 195, buffer, m_Font); // Testing!

	glColor3ub (COLOR_YELLOW);
	m_pFontManager->SetSize(m_Font, 14, 14);
	sprintf(buffer, "%i", sdELConnectRight);
	m_pFontManager->Print (410, 195, buffer, m_Font); // Testing!

	glColor3ub (COLOR_YELLOW);
	m_pFontManager->SetSize(m_Font, 14, 14);
	sprintf(buffer, "%i", sdELConnectCenter);
	m_pFontManager->Print (290, 340, buffer, m_Font); // Testing!

	glColor3ub (COLOR_YELLOW);
	m_pFontManager->SetSize(m_Font, 14, 14);
	sprintf(buffer, "%i", sdELBatterySupply);
	m_pFontManager->Print (225, 430, buffer, m_Font); // Testing!

/* end of inlined code */ 


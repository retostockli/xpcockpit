/*=============================================================================

  This is the sddoor.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style System Display Widget ===

  This file contains the code for the SD subpage  #8: SD_DOOR
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

/* inlined code: SD DOOR Page */

	m_pFontManager->SetSize(m_Font, 14, 14);
	m_pFontManager->Print (175, 440, "DOOR/OXY", m_Font);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES); // title underlining
	  glVertex2d (175, 435);
	  glVertex2d (267, 435);
	glEnd ();
	m_pFontManager->SetSize(m_Font, 12, 12);
	m_pFontManager->Print (340, 410, "OXY", m_Font);
	glColor3ub (COLOR_CYAN);
	m_pFontManager->Print (425, 410, "PSI", m_Font);
	m_pFontManager->SetSize(m_Font, 14, 14);
	glColor3ub (COLOR_GREEN);
	m_pFontManager->Print (375, 410, "1200", m_Font); // May become variable in later version
	// The outline
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP);
	  glVertex2d (200,  80);
	  glVertex2d (196,  90);
	  glVertex2d (196, 388);
	  glVertex2d (217, 425);
	  glVertex2d (232, 425);
	  glVertex2d (253, 388);
	  glVertex2d (253,  90);
	  glVertex2d (249,  80);
	glEnd ();
	glBegin (GL_LINES);
	  glVertex2d (160, 255);
	  glVertex2d (196, 280);
	  glVertex2d (253, 280);
	  glVertex2d (287, 255);
	glEnd ();
	// The doors (closed)
	glLineWidth (2);
	glColor3ub (COLOR_GREEN);
	glBegin (GL_LINE_LOOP); // center front
	  glVertex2d (217, 398);
	  glVertex2d (232, 398);
	  glVertex2d (235, 395);
	  glVertex2d (235, 390);
	  glVertex2d (232, 387);
	  glVertex2d (217, 387);
	  glVertex2d (214, 390);
	  glVertex2d (214, 395);
	glEnd ();
	glBegin (GL_LINE_LOOP); // left front
	  glVertex2d (206, 380);
	  glVertex2d (211, 380);
	  glVertex2d (214, 377);
	  glVertex2d (214, 366);
	  glVertex2d (211, 363);
	  glVertex2d (206, 363);
	  glVertex2d (203, 366);
	  glVertex2d (203, 377);
	glEnd ();
	glBegin (GL_LINE_LOOP); // right front
	  glVertex2d (235, 380);
	  glVertex2d (240, 380);
	  glVertex2d (243, 377);
	  glVertex2d (243, 366);
	  glVertex2d (240, 363);
	  glVertex2d (235, 363);
	  glVertex2d (232, 366);
	  glVertex2d (232, 377);
	glEnd ();
	glBegin (GL_LINE_LOOP); // left forward passengers
	  glVertex2d (199, 341);
	  glVertex2d (203, 341);
	  glVertex2d (206, 338);
	  glVertex2d (206, 330);
	  glVertex2d (203, 327);
	  glVertex2d (199, 327);
	  glVertex2d (196, 330);
	  glVertex2d (196, 338);
	glEnd ();
	glBegin (GL_LINE_LOOP); // right forward passengers
	  glVertex2d (246, 341);
	  glVertex2d (250, 341);
	  glVertex2d (253, 338);
	  glVertex2d (253, 330);
	  glVertex2d (250, 327);
	  glVertex2d (246, 327);
	  glVertex2d (243, 330);
	  glVertex2d (243, 338);
	glEnd ();
	glBegin (GL_LINE_LOOP); // maintenance
	  glVertex2d (232, 319);
	  glVertex2d (235, 319);
	  glVertex2d (238, 316);
	  glVertex2d (238, 312);
	  glVertex2d (235, 309);
	  glVertex2d (232, 309);
	  glVertex2d (229, 312);
	  glVertex2d (229, 316);
	glEnd ();
	glBegin (GL_LINE_LOOP); // forward cargo
	  glVertex2d (239, 300);
	  glVertex2d (253, 300);
	  glVertex2d (253, 286);
	  glVertex2d (239, 286);
	glEnd ();
	glBegin (GL_LINE_LOOP); // left forward emergency
	  glVertex2d (199, 267);
	  glVertex2d (203, 267);
	  glVertex2d (206, 264);
	  glVertex2d (206, 256);
	  glVertex2d (203, 253);
	  glVertex2d (199, 253);
	  glVertex2d (196, 256);
	  glVertex2d (196, 264);
	glEnd ();
	glBegin (GL_LINE_LOOP); // right forward emergency
	  glVertex2d (246, 267);
	  glVertex2d (250, 267);
	  glVertex2d (253, 264);
	  glVertex2d (253, 256);
	  glVertex2d (250, 253);
	  glVertex2d (246, 253);
	  glVertex2d (243, 256);
	  glVertex2d (243, 264);
	glEnd ();
	glBegin (GL_LINE_LOOP); // left rear emergency
	  glVertex2d (199, 242);
	  glVertex2d (203, 242);
	  glVertex2d (206, 239);
	  glVertex2d (206, 231);
	  glVertex2d (203, 228);
	  glVertex2d (199, 228);
	  glVertex2d (196, 231);
	  glVertex2d (196, 239);
	glEnd ();
	glBegin (GL_LINE_LOOP); // right rear emergency
	  glVertex2d (246, 242);
	  glVertex2d (250, 242);
	  glVertex2d (253, 239);
	  glVertex2d (253, 231);
	  glVertex2d (250, 228);
	  glVertex2d (246, 228);
	  glVertex2d (243, 231);
	  glVertex2d (243, 239);
	glEnd ();
	glBegin (GL_LINE_LOOP); // rear cargo
	  glVertex2d (239, 180);
	  glVertex2d (253, 180);
	  glVertex2d (253, 166);
	  glVertex2d (239, 166);
	glEnd ();
	glBegin (GL_LINE_LOOP); // bulk cargo
	  glVertex2d (239, 144);
	  glVertex2d (244, 144);
	  glVertex2d (247, 141);
	  glVertex2d (247, 132);
	  glVertex2d (244, 129);
	  glVertex2d (239, 129);
	  glVertex2d (236, 132);
	  glVertex2d (236, 141);
	glEnd ();
	glBegin (GL_LINE_LOOP); // left rear passengers
	  glVertex2d (199, 103);
	  glVertex2d (203, 103);
	  glVertex2d (206, 100);
	  glVertex2d (206,  92);
	  glVertex2d (203,  89);
	  glVertex2d (199,  89);
	  glVertex2d (196,  92);
	  glVertex2d (196, 100);
	glEnd ();
	glBegin (GL_LINE_LOOP); // right rear passengers
	  glVertex2d (246, 103);
	  glVertex2d (250, 103);
	  glVertex2d (253, 100);
	  glVertex2d (253,  92);
	  glVertex2d (250,  89);
	  glVertex2d (246,  89);
	  glVertex2d (243,  92);
	  glVertex2d (243, 100);
	glEnd ();

	glColor3ub (COLOR_AMBER);
	// Show open doors
	if (paxDoor1 > 0.2) {
	  glBegin (GL_POLYGON); // left forward passengers
	    glVertex2d (199, 341);
	    glVertex2d (203, 341);
	    glVertex2d (206, 338);
	    glVertex2d (206, 330);
	    glVertex2d (203, 327);
	    glVertex2d (199, 327);
	    glVertex2d (196, 330);
	    glVertex2d (196, 338);
	  glEnd ();
	  m_pFontManager->SetSize(m_Font, 14, 14);
	  m_pFontManager->Print (90, 327, "CABIN----", m_Font);
	}
	if (cargoDoor1 > 0.2) {
	  glBegin (GL_POLYGON); // forward cargo
	    glVertex2d (239, 300);
	    glVertex2d (253, 300);
	    glVertex2d (253, 286);
	    glVertex2d (239, 286);
	  glEnd ();
	  m_pFontManager->SetSize(m_Font, 14, 14);
	  m_pFontManager->Print (260, 286, "----CARGO", m_Font);
	}
	if (cargoDoor2 > 0.2) {
	  glBegin (GL_POLYGON); // rear cargo
	    glVertex2d (239, 180);
	    glVertex2d (253, 180);
	    glVertex2d (253, 166);
	    glVertex2d (239, 166);
	  glEnd ();
	  m_pFontManager->SetSize(m_Font, 14, 14);
	  m_pFontManager->Print (260, 166, "----CARGO", m_Font);
	}
	if (bulkDoor > 0.2) {
	  glBegin (GL_POLYGON); // bulk cargo
	    glVertex2d (239, 144);
	    glVertex2d (244, 144);
	    glVertex2d (247, 141);
	    glVertex2d (247, 132);
	    glVertex2d (244, 129);
	    glVertex2d (239, 129);
	    glVertex2d (236, 132);
	    glVertex2d (236, 141);
	  glEnd ();
	  m_pFontManager->SetSize(m_Font, 14, 14);
	  m_pFontManager->Print (260, 129, "----BULK", m_Font);
	}

/* end of inlined code */


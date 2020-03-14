/*=============================================================================

  This is the sdbleed.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style System Display Widget ===

  This file contains the code for the SD subpage  #1: SD_BLEED
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

/* inlined code: SD BLEED Page */

	m_pFontManager->SetSize (m_Font, 14, 14);
	m_pFontManager->Print (178, 440, "BLEED", m_Font);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES); // title underlining
	  glVertex2d (178, 435);
	  glVertex2d (240, 435);
	glEnd ();

	// The fixed background stuff

	// Gray text fields
	glColor3ub (COLOR_GRAY50);
	glBegin (GL_POLYGON);
	  glVertex2d ( 65, 308); glVertex2d ( 65, 398); glVertex2d (125, 398); glVertex2d (125, 308); glVertex2d ( 65, 308);
	glEnd ();
	glBegin (GL_POLYGON);
	  glVertex2d (335, 308); glVertex2d (335, 398); glVertex2d (395, 398); glVertex2d (395, 308); glVertex2d (335, 308);
	glEnd ();
	glBegin (GL_POLYGON);
	  glVertex2d ( 66, 180); glVertex2d ( 66, 215); glVertex2d (137, 215); glVertex2d (137, 180); glVertex2d ( 66, 180);
	glEnd ();
	glBegin (GL_POLYGON);
	  glVertex2d (323, 180); glVertex2d (323, 215); glVertex2d (394, 215); glVertex2d (394, 180); glVertex2d (323, 180);
	glEnd ();
	// Masking segments
	glColor3ub (COLOR_BLACK);
	aCircle.SetOrigin ( 95, 285);
	aCircle.SetRadius (42);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (295, 65);
	  glBegin (GL_POLYGON);
	    aCircle.Evaluate ();
	  glEnd ();
	aCircle.SetOrigin (365, 285);
	  glBegin (GL_POLYGON);
	    aCircle.Evaluate ();
	  glEnd ();

	glLineWidth (2);
	// Straight Lines
	glColor3ub (COLOR_GREEN);
	// Horizontal lines
	glBegin (GL_LINE_STRIP); // top
	  glVertex2d ( 95, 398); glVertex2d ( 95, 417); glVertex2d (365, 417); glVertex2d (365, 398);
	glEnd ();
	// Vertical lines left
	glBegin (GL_LINES);
	  glVertex2d ( 95, 215); glVertex2d ( 95, 271);
	  glVertex2d ( 95, 160); glVertex2d ( 95, 180);
	  glVertex2d ( 95, 100); glVertex2d ( 95, 132);
	glEnd ();
	// Vertical lines right
	glBegin (GL_LINES);
	  glVertex2d (365, 215); glVertex2d (365, 271);
	  glVertex2d (365, 160); glVertex2d (365, 180);
	  glVertex2d (365, 100); glVertex2d (365, 132);
	glEnd ();
	// Vertical lines center
	glBegin (GL_LINES);
	  glVertex2d (230, 398); glVertex2d (230, 417);
	  glVertex2d (230, 358); glVertex2d (230, 370);
	glEnd ();
	// Corner lines
	glBegin (GL_LINE_STRIP); // Left
	  glVertex2d (137, 114); glVertex2d (154, 114); glVertex2d (154, 100);
	glEnd ();
	glBegin (GL_LINE_STRIP); // Right
	  glVertex2d (323, 114); glVertex2d (306, 114); glVertex2d (306, 100);
	glEnd ();

	// Variable lines
bleedIntercon = 3; // 4 pos (image BleedIntercon) (??)
	glColor3ub (COLOR_GREEN);
	glLineWidth (3);
	switch (bleedIntercon) {
	  case 0:
	    glBegin (GL_LINES);
	      glVertex2d (244, 244); glVertex2d (256, 244);
	      glVertex2d (284, 244); glVertex2d (296, 244);
	    glEnd ();
	  break;
	  case 1:
	    glBegin (GL_LINES);
	      glVertex2d ( 95, 244); glVertex2d (256, 244); glVertex2d (284, 244); glVertex2d (296, 244);
	      glVertex2d (230, 215); glVertex2d (230, 244);
	      glVertex2d (230, 175); glVertex2d (230, 187);
	    glEnd ();
	  break;
	  case 2:
	    glBegin (GL_LINES);
	      glVertex2d ( 95, 244); glVertex2d (256, 244); glVertex2d (284, 244); glVertex2d (365, 244);
	      glVertex2d (230, 215); glVertex2d (230, 227);
	      glVertex2d (230, 175); glVertex2d (230, 187);
	    glEnd ();
	  break;
	  case 3:
	    glBegin (GL_LINES);
	      glVertex2d ( 95, 244); glVertex2d (256, 244); glVertex2d (284, 244); glVertex2d (365, 244);
	      glVertex2d (230, 215); glVertex2d (230, 244);
	      glVertex2d (230, 175); glVertex2d (230, 187);
	    glEnd ();
	  break;
	}

	// Curves
	glColor3ub (COLOR_WHITE);
	aCircle.SetOrigin ( 95, 285);
	aCircle.SetRadius (32);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (295, 65);
	  glLineWidth (3);
	  glBegin (GL_LINE_STRIP);
	    aCircle.Evaluate ();
	  glEnd ();
	aCircle.SetOrigin ( 95, 345);
	  glLineWidth (3);
	  glBegin (GL_LINE_STRIP);
	    aCircle.Evaluate ();
	  glEnd ();
	aCircle.SetOrigin (365, 285);
	  glLineWidth (3);
	  glBegin (GL_LINE_STRIP);
	    aCircle.Evaluate ();
	  glEnd ();
	aCircle.SetOrigin (365, 345);
	  glLineWidth (3);
	  glBegin (GL_LINE_STRIP);
	    aCircle.Evaluate ();
	  glEnd ();

	// Texts
	m_pFontManager->SetSize (m_Font, 12, 12);
	glColor3ub (COLOR_WHITE);
	m_pFontManager->Print ( 45, 350, "C", m_Font);
	m_pFontManager->Print (130, 350, "H", m_Font);
	m_pFontManager->Print (320, 350, "H", m_Font);
	m_pFontManager->Print (400, 350, "C", m_Font);
	m_pFontManager->Print (216, 341, "RAM", m_Font);
	m_pFontManager->Print (216, 325, "AIR", m_Font);
	m_pFontManager->Print ( 40, 290, "LO", m_Font);
	m_pFontManager->Print (130, 290, "HI", m_Font);
	m_pFontManager->Print (310, 290, "HI", m_Font);
	m_pFontManager->Print (400, 290, "LO", m_Font);
	m_pFontManager->Print (216, 155, "APU", m_Font);
	m_pFontManager->Print ( 90,  80, "IP", m_Font);
	m_pFontManager->Print (145,  80, "HP", m_Font);
	m_pFontManager->Print (295,  80, "HP", m_Font);
	m_pFontManager->Print (360,  80, "IP", m_Font);
	glColor3ub (COLOR_CYAN);
	m_pFontManager->Print (108, 383, ";", m_Font);
	m_pFontManager->Print (378, 383, ";", m_Font);
	m_pFontManager->Print (108, 330, ";", m_Font);
	m_pFontManager->Print (378, 330, ";", m_Font);
	m_pFontManager->Print (105, 200, "PSI", m_Font);
	m_pFontManager->Print (325, 200, "PSI", m_Font);
	m_pFontManager->Print (120, 183, ";", m_Font);
	m_pFontManager->Print (325, 183, ";", m_Font);

	// The valves

	// APU Bleed Valve
//apuBldValve = 0;
	switch (apuBldValve) {
	  case 0:
	    aCircle.SetOrigin (230, 200);
	    aCircle.SetRadius (14);
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
	      glVertex2d (216, 200);
	      glVertex2d (244, 200);
	    glEnd ();
	    break;
	  case 1:
	    aCircle.SetOrigin (230, 200);
	    aCircle.SetRadius (14);
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
	      glVertex2d (230, 186);
	      glVertex2d (230, 214);
	    glEnd ();
	    break;
	  case 2:
	    aCircle.SetOrigin (230, 200);
	    aCircle.SetRadius (14);
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
	      glVertex2d (216, 200);
	      glVertex2d (244, 200);
	    glEnd ();
	    break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    aCircle.SetOrigin (230, 200);
	    aCircle.SetRadius (14);
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
	      glVertex2d (230, 186);
	      glVertex2d (230, 214);
	    glEnd ();
	    break;
	  }

	// Engine 1 Bleed Valve
//eng1BldVlv = 3;
	aCircle.SetOrigin ( 95, 146);
	aCircle.SetRadius (14);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (0, 360);
	glLineWidth (3);
	switch (eng1BldVlv) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d ( 83, 146); glVertex2d (107, 146);
	    glEnd ();
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d ( 95, 124); glVertex2d ( 95, 158);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d ( 83, 146); glVertex2d (107, 146);
	    glEnd ();
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d ( 95, 124); glVertex2d ( 95, 158);
	    glEnd ();
	  break;
	}

	// Engine 2 Bleed Valve
//eng2BldVlv = 3;
	aCircle.SetOrigin (365, 146);
	aCircle.SetRadius (14);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (0, 360);
	glLineWidth (3);
	switch (eng2BldVlv) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (353, 146); glVertex2d (377, 146);
	    glEnd ();
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (365, 134); glVertex2d (365, 158);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (353, 146); glVertex2d (377, 146);
	    glEnd ();
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (365, 134); glVertex2d (365, 158);
	    glEnd ();
	  break;
	}

	// Engine 1 HP Bleed Valve
//eng1HPBleed = 2;
	aCircle.SetRadius (14);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (0, 360);
	glLineWidth (3);
	aCircle.SetOrigin (123, 114);
	switch (eng1HPBleed) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (123, 102); glVertex2d (123, 126);
	    glEnd ();
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d ( 95, 114); glVertex2d (110, 114);
	      glVertex2d (111, 114); glVertex2d (135, 114);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d ( 95, 114); glVertex2d (110, 114);
	      glVertex2d (123, 102); glVertex2d (123, 126);
	    glEnd ();
	  break;
	}

	// Engine 2 HP Bleed Valve
//eng2HPBleed = 2;
	aCircle.SetRadius (14);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (0, 360);
	glLineWidth (3);
	aCircle.SetOrigin (337, 114);
	switch (eng2HPBleed) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (337, 102); glVertex2d (337, 126);
	    glEnd ();
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (325, 114); glVertex2d (349, 114);
	      glVertex2d (350, 114); glVertex2d (365, 114);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (337, 102); glVertex2d (337, 126);
	      glVertex2d (350, 114); glVertex2d (365, 114);
	    glEnd ();
	  break;
	}

	// Pack 1 Flow Control Valve
//pack1FCV = 3;
	aCircle.SetRadius (14);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (0, 360);
	glLineWidth (3);
	aCircle.SetOrigin ( 95, 285);
	switch (pack1FCV) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d ( 83, 285); glVertex2d (107, 285);
	    glEnd ();
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d ( 95, 273); glVertex2d ( 95, 297);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d ( 83, 285); glVertex2d (107, 285);
	    glEnd ();
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d ( 95, 273); glVertex2d ( 95, 297);
	    glEnd ();
	  break;
	}

	// Pack 2 Flow Control Valve
//pack2FCV = 3;
	aCircle.SetRadius (14);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (0, 360);
	glLineWidth (3);
	aCircle.SetOrigin (365, 285);
	switch (pack2FCV) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (353, 285); glVertex2d (377, 285);
	    glEnd ();
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (365, 273); glVertex2d (365, 297);
	    glEnd ();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (353, 285); glVertex2d (377, 285);
	    glEnd ();
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin (GL_LINE_STRIP);
	      aCircle.Evaluate ();
	    glEnd ();
	    glBegin (GL_LINES);
	      glVertex2d (365, 273); glVertex2d (365, 297);
	    glEnd ();
	  break;
	}

	// Ram Air Valve
//ramAirValve = 0.5;
	aCircle.SetRadius (14);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (0, 360);
	glLineWidth (3);
	aCircle.SetOrigin (230, 384);
	if (ramAirValve < 0.1) { // closed
	  glColor3ub (COLOR_GREEN);
	  glBegin (GL_LINE_STRIP);
	    aCircle.Evaluate ();
	  glEnd ();
	  glBegin (GL_LINES);
	    glVertex2d (218, 384); glVertex2d (242, 384);
	  glEnd ();
	} else if (ramAirValve > 0.9) { // open
	  glColor3ub (COLOR_AMBER);
	  glBegin (GL_LINE_STRIP);
	    aCircle.Evaluate ();
	  glEnd ();
	  glBegin (GL_LINES);
	    glVertex2d (230, 372); glVertex2d (230, 396);
	  glEnd ();
	} else { // in transit
	  glColor3ub (COLOR_AMBER);
	  glBegin (GL_LINE_STRIP);
	    aCircle.Evaluate ();
	  glEnd ();
	  glBegin (GL_LINES);
	    glVertex2d (221, 375); glVertex2d (239, 393);
	  glEnd ();
	}

	// Cross-Bleed Valve
//xBldVlv = 0;
	if (bleedIntercon >= 1) {
	  aCircle.SetOrigin (270, 244);
	  aCircle.SetRadius (14);
	  aCircle.SetDegreesPerPoint (5);
	  aCircle.SetArcStartEnd (0, 360);
	  glLineWidth (3);
	  glBegin (GL_LINE_STRIP);
	    aCircle.Evaluate ();
	  glEnd ();
	  switch (xBldVlv) {
	    case 0:
	      glColor3ub (COLOR_GREEN);
	      glBegin (GL_LINE_STRIP);
                  aCircle.Evaluate ();
	      glEnd ();
	      glBegin (GL_LINES);
                  glVertex2d (270, 232); glVertex2d (270, 256);
	      glEnd ();
	    break;
	    case 1:
	      glColor3ub (COLOR_GREEN);
	      glBegin (GL_LINE_STRIP);
                  aCircle.Evaluate ();
	      glEnd ();
	      glBegin (GL_LINES);
                  glVertex2d (258, 244); glVertex2d (282, 244);
	      glEnd ();
	    break;
	    case 2:
	      glColor3ub (COLOR_AMBER);
	      glBegin (GL_LINE_STRIP);
                  aCircle.Evaluate ();
	      glEnd ();
	      glBegin (GL_LINES);
                  glVertex2d (270, 232); glVertex2d (270, 256);
	      glEnd ();
	    break;
	    case 3:
	      glColor3ub (COLOR_AMBER);
	      glBegin (GL_LINE_STRIP);
                  aCircle.Evaluate ();
	      glEnd ();
	      glBegin (GL_LINES);
                  glVertex2d (258, 244); glVertex2d (282, 244);
	      glEnd ();
	    break;
	  }
	}

	// The needles

//pack1FlowInd = 0.6;
	glPushMatrix ();
	  glLineWidth (3);
	  glColor3ub (COLOR_GREEN);
	  glTranslated ( 95, 285, 0);
	  glRotated (65 - ((pack1FlowInd / 0.6) * 65), 0, 0, 1); // 1.2 == full-scale
	  glBegin (GL_LINES);
	    glVertex2d (  0, 14);
	    glVertex2d (  0, 30);
	  glEnd ();
	glPopMatrix ();

//pack2FlowInd = 0.0;
	glPushMatrix ();
	  glLineWidth (3);
	  glColor3ub (COLOR_GREEN);
	  glTranslated (365, 285, 0);
	  glRotated (65 - ((pack2FlowInd / 0.6) * 65), 0, 0, 1); // 1.2 == full-scale
	  glBegin (GL_LINES);
	    glVertex2d (  0, 14);
	    glVertex2d (  0, 30);
	  glEnd ();
	glPopMatrix ();

//pack1Temp = 0.5;
	glPushMatrix ();
	  glLineWidth (3);
	  glColor3ub (COLOR_GREEN);
	  glTranslated ( 95, 345, 0);
	  glRotated (65 - (pack1Temp * 2 * 65), 0, 0, 1);
	  glBegin (GL_LINES);
	    glVertex2d (  0, 14);
	    glVertex2d (  0, 30);
	  glEnd ();
	glPopMatrix ();

//pack2Temp = 1.0;
	glPushMatrix ();
	  glLineWidth (3);
	  glColor3ub (COLOR_GREEN);
	  glTranslated (365, 345, 0);
	  glRotated (65 - (pack2Temp * 2 * 65), 0, 0, 1);
	  glBegin (GL_LINES);
	    glVertex2d (  0, 14);
	    glVertex2d (  0, 30);
	  glEnd ();
	glPopMatrix ();

	// The numeric value displays

//printf ("pack 1 temp: %f, pack 2 temp: %f\n", pack1Temp, pack2Temp);
	// Pack outlet temperature (normally green, amber if temp > 90 dC) // Note: does not seem to work as specified...
	if ((pack1Temp * 32.63) < 90.0) glColor3ub (COLOR_GREEN); // 32.63: funny correction value for pack temp ??
	else glColor3ub (COLOR_AMBER);
	sprintf (buffer, "%4.0f", (pack1Temp * 32.63));
	m_pFontManager->Print ( 65, 383, buffer, m_Font);
	if ((pack2Temp * 32.63) < 90.0) glColor3ub (COLOR_GREEN);
	else glColor3ub (COLOR_AMBER);
	sprintf (buffer, "%4.0f", (pack2Temp * 32.63));
	m_pFontManager->Print (335, 383, buffer, m_Font);

// Note: these variables are not yet defined as dataref...
	// Pack compressor outlet temperature (normally green, amber if temp > 230 dC)
//	sprintf (buffer, "%4.0f", pack1CmpTemp);
//	m_pFontManager->Print (xxx, 330, buffer, m_Font);
	m_pFontManager->Print ( 65, 330, " 170", m_Font);
//	sprintf (buffer, "%4.0f", pack2CmpTemp);
//	m_pFontManager->Print (xxx, 330, buffer, m_Font);
	m_pFontManager->Print (335, 330, " 170", m_Font);
	// Precooler inlet pressure (normally green; amber if press < 4 or press > 57 psi)
//	sprintf (buffer, "%3.0f", pack1PrciPress);
//	m_pFontManager->Print (xxx, 200, buffer, m_Font);
	m_pFontManager->Print ( 66, 200, " +1", m_Font);
//	sprintf (buffer, "%3.0f", pack2PrciPress);
//	m_pFontManager->Print (xxx, 200, buffer, m_Font);
	m_pFontManager->Print (363, 200, "+1", m_Font);
	// Precooler outlet temperature (normally green, amber if temp < 150 or temp > 250, 70, 290 dC)
//	sprintf (buffer, "%4.0f", pack1prcoTemp);
//	m_pFontManager->Print (xxx, 183, buffer, m_Font);
	m_pFontManager->Print ( 66, 183, " 147", m_Font);
//	sprintf (buffer, "%4.0f", pack2prcoTemp);
//	m_pFontManager->Print (xxx, 183, buffer, m_Font);
	m_pFontManager->Print (353, 183, "147", m_Font);

	// Engine indicators
	if (eng1N2 > 55.0) glColor3ub (COLOR_WHITE);
	else glColor3ub (COLOR_AMBER);
	m_pFontManager->Print ( 50, 135, "1", m_Font);
	if (eng2N2 > 55.0) glColor3ub (COLOR_WHITE);
	else glColor3ub (COLOR_AMBER);
	m_pFontManager->Print (400, 135, "2", m_Font);


/* end of inlined code */


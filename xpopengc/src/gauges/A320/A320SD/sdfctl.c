/*=============================================================================

  This is the sdfctl.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style System Display Widget ===

  This file contains the code for the SD subpage #10: SD_FCTL
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

/* inlined code: SD FCTL Page */

	m_pFontManager->SetSize(m_Font, 14, 14);
	m_pFontManager->Print (206, 440, "F/CTL", m_Font);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES); // title underlining
	  glVertex2d (206, 435);
	  glVertex2d (264, 435);
	glEnd ();

	// The fixed background stuff

	// Lines and curves

	glLineWidth (2);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP);
	  glVertex2d ( 75, 384); glVertex2d ( 75, 390); glVertex2d (181, 410); glVertex2d (181, 404);
	glEnd ();
	glBegin (GL_LINE_STRIP);
	  glVertex2d (280, 404); glVertex2d (280, 410); glVertex2d (386, 390); glVertex2d (386, 384);
	glEnd ();
	glBegin (GL_LINE_STRIP);
	  glVertex2d (106, 345); glVertex2d (106, 340); glVertex2d (181, 345); glVertex2d (181, 350);
	glEnd ();
	glBegin (GL_LINE_STRIP);
	  glVertex2d (280, 350); glVertex2d (280, 345); glVertex2d (355, 340); glVertex2d (355, 345);
	glEnd ();
	glBegin (GL_LINE_STRIP);
	  glVertex2d (180, 202); glVertex2d (144, 186); glVertex2d (144, 170); glVertex2d (200, 170);
	glEnd ();
	glBegin (GL_LINE_STRIP);
	  glVertex2d (279, 202); glVertex2d (315, 186); glVertex2d (315, 170); glVertex2d (259, 170);
	glEnd ();

	// Texts

	glColor3ub (COLOR_WHITE);
	m_pFontManager->SetSize(m_Font, 12, 12);
	m_pFontManager->Print ( 24, 352, "L", m_Font);
	m_pFontManager->Print ( 13, 331, "AIL", m_Font);
	m_pFontManager->Print (428, 352, "R", m_Font);
	m_pFontManager->Print (419, 331, "AIL", m_Font);
	m_pFontManager->Print (197, 340, "SPD BRK", m_Font);
	m_pFontManager->Print (162, 223, "PITCH TRIM", m_Font);
	m_pFontManager->Print (216, 173, "RUD", m_Font);
	m_pFontManager->Print ( 85, 202, "L", m_Font);
	m_pFontManager->Print ( 71, 180, "ELEV", m_Font);
	m_pFontManager->Print (365, 202, "R", m_Font);
	m_pFontManager->Print (352, 180, "ELEV", m_Font);

	// The spoiler indicators
	// Note: for any changes to this part, also check the WHEEL page!

	m_pFontManager->SetSize(m_Font, 12, 12);
	glLineWidth (2);
	switch (spoiler1L) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (198, 366); glVertex2d (198, 382);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (192, 382); glVertex2d (198, 398); glVertex2d (204, 382); glVertex2d (192, 382);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (194, 368, "1", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (192, 382); glVertex2d (198, 398); glVertex2d (204, 382);
	    glEnd();
	    m_pFontManager->Print (194, 368, "1", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (190, 366); glVertex2d (206, 366);
	glEnd();
	glLineWidth (2);
	switch (spoiler1R) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (262, 366); glVertex2d (262, 382);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (256, 382); glVertex2d (262, 398); glVertex2d (270, 382); glVertex2d (256, 382);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (258, 368, "1", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (256, 382); glVertex2d (262, 398); glVertex2d (270, 382);
	    glEnd();
	    m_pFontManager->Print (258, 368, "1", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (254, 366); glVertex2d (270, 366);
	glEnd();
	glLineWidth (2);
	switch (spoiler2L) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (171, 364); glVertex2d (171, 380);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (165, 380); glVertex2d (171, 396); glVertex2d (177, 380); glVertex2d (165, 380);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (167, 366, "2", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (165, 380); glVertex2d (171, 396); glVertex2d (177, 380);
	    glEnd();
	    m_pFontManager->Print (167, 366, "2", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (163, 364); glVertex2d (179, 364);
	glEnd();
	glLineWidth (2);
	switch (spoiler2R) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (289, 364); glVertex2d (289, 380);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (283, 380); glVertex2d (289, 396); glVertex2d (295, 380); glVertex2d (283, 380);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (285, 366, "2", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (283, 380); glVertex2d (289, 396); glVertex2d (295, 380);
	    glEnd();
	    m_pFontManager->Print (285, 366, "2", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (281, 364); glVertex2d (297, 364);
	glEnd();
	glLineWidth (2);
	switch (spoiler3L) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (144, 362);glVertex2d (144, 376);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (138, 376); glVertex2d (144, 394); glVertex2d (150, 376); glVertex2d (138, 376);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (140, 364, "3", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (138, 376); glVertex2d (144, 394); glVertex2d (150, 376);
	    glEnd();
	    m_pFontManager->Print (140, 364, "3", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (136, 362); glVertex2d (152, 362);
	glEnd();
	glLineWidth (2);
	switch (spoiler3R) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (316, 362); glVertex2d (316, 378);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (310, 378); glVertex2d (316, 394); glVertex2d (322, 378); glVertex2d (310, 378);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (312, 364, "3", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (310, 378); glVertex2d (316, 394); glVertex2d (322, 378);
	    glEnd();
	    m_pFontManager->Print (312, 364, "3", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (308, 362); glVertex2d (324, 362);
	glEnd();
	glLineWidth (2);
	switch (spoiler4L) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (117, 358); glVertex2d (117, 374);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (111, 374); glVertex2d (117, 390); glVertex2d (123, 374); glVertex2d (111, 374);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (113, 360, "4", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (111, 374); glVertex2d (117, 390); glVertex2d (123, 374);
	    glEnd();
	    m_pFontManager->Print (113, 360, "4", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (109, 358); glVertex2d (125, 358);
	glEnd();
	glLineWidth (2);
	switch (spoiler4R) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (343, 358); glVertex2d (343, 374);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (337, 374); glVertex2d (343, 390); glVertex2d (349, 374); glVertex2d (337, 374);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (339, 360, "4", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (337, 374); glVertex2d (343, 390); glVertex2d (349, 374);
	    glEnd();
	    m_pFontManager->Print (339, 360, "4", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (335, 358); glVertex2d (351, 358);
	glEnd();
	glLineWidth (2);
	switch (spoiler5L) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d ( 90, 356); glVertex2d ( 90, 372);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d ( 84, 372); glVertex2d ( 90, 388); glVertex2d ( 96, 372); glVertex2d ( 84, 372);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print ( 86, 358, "5", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d ( 84, 372); glVertex2d ( 90, 388); glVertex2d ( 96, 372);
	    glEnd();
	    m_pFontManager->Print ( 86, 358, "5", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d ( 82, 356); glVertex2d ( 98, 356);
	glEnd();
	glLineWidth (2);
	switch (spoiler5R) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (370, 356); glVertex2d (370, 372);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (364, 372); glVertex2d (370, 388); glVertex2d (376, 372); glVertex2d (364, 372);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (366, 358, "5", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (364, 372); glVertex2d (370, 388); glVertex2d (376, 372);
	    glEnd();
	    m_pFontManager->Print (366, 358, "5", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (362, 356); glVertex2d (378, 356);
	glEnd();

	// Ailerons

	// Left Aileron: Scale
	glLineWidth (2);
	if (lAilAvail != 0) glColor3ub (COLOR_WHITE);
	else glColor3ub (COLOR_AMBER);
	glBegin (GL_POLYGON);
	  glVertex2d ( 50, 355); glVertex2d ( 57, 355); glVertex2d ( 57, 345); glVertex2d ( 50, 345);
	glEnd ();
	glBegin (GL_POLYGON);
	  glVertex2d ( 50, 270); glVertex2d ( 57, 270); glVertex2d ( 57, 260); glVertex2d ( 50, 260);
	glEnd ();
	glBegin (GL_LINES);
	  glVertex2d ( 57, 355); glVertex2d ( 57, 260);
	  glVertex2d ( 50, 318); glVertex2d ( 57, 318);
	glEnd ();
	glLineWidth (4);
	glBegin (GL_LINES);
	  glVertex2d ( 47, 308); glVertex2d ( 57, 308);
	glEnd ();
	// Left Aileron: Pointer
	// Note: vertical position depends on lAilDef
	glPushMatrix ();
	  glTranslated (0, -1.5 * lAilDef, 0);
	  glLineWidth (2);
	  if (lAilAvail != 0) glColor3ub (COLOR_GREEN);
	  else glColor3ub (COLOR_AMBER);
	  glBegin(GL_LINE_STRIP);
	    glVertex2d ( 57, 318); glVertex2d ( 69, 324); glVertex2d ( 69, 312); glVertex2d ( 57, 318); 
	  glEnd();
	glPopMatrix ();

	// Right Aileron: Scale
	glLineWidth (2);
	if (rAilAvail != 0) glColor3ub (COLOR_WHITE);
	else glColor3ub (COLOR_AMBER);
	glBegin (GL_POLYGON);
	  glVertex2d (402, 355); glVertex2d (409, 355); glVertex2d (409, 345); glVertex2d (402, 345);
	glEnd ();
	glBegin (GL_POLYGON);
	  glVertex2d (402, 270); glVertex2d (409, 270); glVertex2d (409, 260); glVertex2d (402, 260);
	glEnd ();
	glBegin (GL_LINES);
	  glVertex2d (402, 355); glVertex2d (402, 260);
	  glVertex2d (402, 318); glVertex2d (409, 318);
	glEnd ();
	glLineWidth (4);
	glBegin (GL_LINES);
	  glVertex2d (402, 308); glVertex2d (412, 308);
	glEnd ();
	// Right Aileron: Pointer
	// Note: vertical position depends on rAilDef
	glPushMatrix ();
	  glTranslated (0, -1.5 * rAilDef, 0);
	  glLineWidth (2);
	  if (rAilAvail != 0) glColor3ub (COLOR_GREEN);
	  else glColor3ub (COLOR_AMBER);
	  glBegin(GL_LINE_STRIP);
	    glVertex2d (402, 318); glVertex2d (390, 312); glVertex2d (390, 324); glVertex2d (402, 318); 
	  glEnd();
	glPopMatrix ();

	// Elevator and Pitch Trim

	// Left Elevator: Scale
	if (lElevAvail != 0) glColor3ub (COLOR_WHITE);
	else glColor3ub (COLOR_AMBER);
	glLineWidth (2);
	glBegin (GL_POLYGON);
	  glVertex2d (119, 204); glVertex2d (126, 204); glVertex2d (126, 194); glVertex2d (119, 194);
	glEnd ();
	glBegin (GL_POLYGON);
	  glVertex2d (119, 118); glVertex2d (126, 118); glVertex2d (126, 108); glVertex2d (119, 108);
	glEnd ();
	glBegin (GL_LINES);
	  glVertex2d (126, 204); glVertex2d (126, 108);
	glEnd ();
	glLineWidth (4);
	glBegin (GL_LINES);
	  glVertex2d (117, 142); glVertex2d (126, 142);
	glEnd ();
	// Left Elevator: Pointer
	glPushMatrix ();
	  glTranslated (0, -2 * lElevDef, 0);
	  glLineWidth (2);
	  if (lElevAvail != 0) glColor3ub (COLOR_GREEN);
	  else glColor3ub (COLOR_AMBER);
	  glBegin(GL_LINE_STRIP);
	    glVertex2d (126, 142); glVertex2d (138, 136); glVertex2d (138, 148); glVertex2d (126, 142); 
	  glEnd();
	glPopMatrix ();

	// Right Elevator: Scale
	if (rElevAvail != 0) glColor3ub (COLOR_WHITE);
	else glColor3ub (COLOR_AMBER);
	glLineWidth (2);
	glBegin (GL_POLYGON);
	  glVertex2d (335, 204); glVertex2d (342, 204); glVertex2d (342, 194); glVertex2d (335, 194);
	glEnd ();
	glBegin (GL_POLYGON);
	  glVertex2d (335, 118); glVertex2d (342, 118); glVertex2d (342, 108); glVertex2d (335, 108);
	glEnd ();
	glBegin (GL_LINES);
	  glVertex2d (335, 204); glVertex2d (335, 108);
	glEnd ();
	glLineWidth (4);
	glBegin (GL_LINES);
	  glVertex2d (335, 142); glVertex2d (345, 142);
	glEnd ();
	// Right Elevator: Pointer
	glPushMatrix ();
	  glTranslated (0, -2 * rElevDef, 0);
	  glLineWidth (2);
	  if (rElevAvail != 0) glColor3ub (COLOR_GREEN);
	  else glColor3ub (COLOR_AMBER);
	  glBegin(GL_LINE_STRIP);
	    glVertex2d (335, 142); glVertex2d (323, 136); glVertex2d (323, 148); glVertex2d (335, 142); 
	  glEnd();
	glPopMatrix ();

	// Elevator Pitch) Trim
	glColor3ub (COLOR_GREEN); // Note: should be amber when both G and Y Hyd sys unavail!
	m_pFontManager->SetSize(m_Font, 14, 14);
	sprintf(buffer, "%4.1f", elevTrim);
	m_pFontManager->Print (200, 200, buffer, m_Font);
	if (elevTrim >= 0) {
	  glColor3ub (COLOR_WHITE);
	  m_pFontManager->SetSize(m_Font, 12, 12);
	  m_pFontManager->Print (250, 200, "UP", m_Font);
	}

	// Rudder and Rudder Trim

	// Fixed background stuff
	glColor3ub (COLOR_GREEN);
	aCircle.SetOrigin (230, 195);
	aCircle.SetRadius (110);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (150, 210);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	glLineWidth (2);
	glBegin (GL_LINES);
	glVertex2d (175,  99); glVertex2d (178, 105);
	glVertex2d (285,  99); glVertex2d (282, 105);
	glEnd ();
	glPushMatrix ();
	  glTranslated (230, 195, 0);
	  // The rudder indicator
	  glPushMatrix ();
	    glRotated (rudDef, 0, 0, 1);
	    aCircle.SetOrigin (0, -55);
	    aCircle.SetRadius (8);
	    aCircle.SetDegreesPerPoint (5);
	    aCircle.SetArcStartEnd (-90,  90);
	    glLineWidth (2);
	    glColor3ub (COLOR_GREEN); //  variable ?
	    glBegin(GL_LINE_STRIP);
	    aCircle.Evaluate ();
	    glEnd();
	    glLineWidth (2);
	    glBegin (GL_LINES);
	    glVertex2d (  -8, -55); glVertex2d (  0, -110);
	    glVertex2d (   8, -55); glVertex2d (  0, -110);
	    glEnd ();
	  glPopMatrix ();
	    glColor3ub (COLOR_CYAN); //  variable ?
	  // The rudder left extreme indicator
	  glPushMatrix ();
	    glRotated (-rtlPos, 0, 0, 1);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d ( 0, -110); glVertex2d (  0, -116); glVertex2d (  5, -116);
	    glEnd ();
	  glPopMatrix ();
	  // The rudder right extreme indicator
	  glPushMatrix ();
	    glRotated (rtlPos, 0, 0, 1);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d ( 0, -110); glVertex2d (  0, -116); glVertex2d (  -5, -116);
	    glEnd ();
	  glPopMatrix ();
	  // The rudder trim indicator
	  glPushMatrix ();
	    glRotated (ytrPos, 0, 0, 1);
	    glBegin (GL_LINE_STRIP);
	      glVertex2d ( -2, -110); glVertex2d (  0, -120); glVertex2d (  2, -110);
	    glEnd ();
	  glPopMatrix ();

	glPopMatrix ();

	// ELAC's and SEC's

	glColor3ub (COLOR_WHITE);
	m_pFontManager->SetSize(m_Font, 12, 12);
	m_pFontManager->Print (120, 300, "ELAC", m_Font);
	m_pFontManager->Print (245, 300, "SEC", m_Font);

	// ELAC 1
	if (elac1 != 0) {
	  glColor3ub (COLOR_GREEN);
	  m_pFontManager->Print (170, 300, "1", m_Font);
	  glColor3ub (COLOR_WHITE);
	  glLineWidth (2);
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (120, 294); glVertex2d (184, 294); glVertex2d (184, 318); glVertex2d (173, 318);
	  glEnd ();
	} else {
	  glColor3ub (COLOR_AMBER);
	  m_pFontManager->Print (170, 300, "1", m_Font);
	  glLineWidth (2);
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (120, 294); glVertex2d (184, 294); glVertex2d (184, 318); glVertex2d (173, 318);
	  glEnd ();
	}
	// ELAC 2
	if (elac2 != 0) {
	  glColor3ub (COLOR_GREEN);
	  m_pFontManager->Print (195, 290, "2", m_Font);
	  glColor3ub (COLOR_WHITE);
	  glLineWidth (2);
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (145, 284); glVertex2d (209, 284); glVertex2d (209, 308); glVertex2d (198, 308);
	  glEnd ();
	} else {
	  glColor3ub (COLOR_AMBER);
	  m_pFontManager->Print (195, 290, "2", m_Font);
	  glLineWidth (2);
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (145, 284); glVertex2d (209, 284); glVertex2d (209, 308); glVertex2d (198, 308);
	  glEnd ();
	}
	// SEC 1
	if (sec1 != 0) {
	  glColor3ub (COLOR_GREEN);
	  m_pFontManager->Print (295, 300, "1", m_Font);
	  glColor3ub (COLOR_WHITE);
	  glLineWidth (2);
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (245, 294); glVertex2d (309, 294); glVertex2d (309, 318); glVertex2d (298, 318);
	  glEnd ();
	} else {
	  glColor3ub (COLOR_AMBER);
	  m_pFontManager->Print (295, 300, "1", m_Font);
	  glLineWidth (2);
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (245, 294); glVertex2d (309, 294); glVertex2d (309, 318); glVertex2d (298, 318);
	  glEnd ();
	}
	// SEC 2
	if (sec2 != 0) {
	  glColor3ub (COLOR_GREEN);
	  m_pFontManager->Print (320, 290, "2", m_Font);
	  glColor3ub (COLOR_WHITE);
	  glLineWidth (2);
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (270, 284); glVertex2d (334, 284); glVertex2d (334, 308); glVertex2d (323, 308);
	  glEnd ();
	} else {
	  glColor3ub (COLOR_AMBER);
	  m_pFontManager->Print (320, 290, "2", m_Font);
	  glLineWidth (2);
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (270, 284); glVertex2d (334, 284); glVertex2d (334, 308); glVertex2d (323, 308);
	  glEnd ();
	}
	// SEC 3
	if (sec3 != 0) {
	  glColor3ub (COLOR_GREEN);
	  m_pFontManager->Print (345, 280, "3", m_Font);
	  glColor3ub (COLOR_WHITE);
	  glLineWidth (2);
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (295, 274); glVertex2d (359, 274); glVertex2d (359, 298); glVertex2d (348, 298);
	  glEnd ();
	} else {
	  glColor3ub (COLOR_AMBER);
	  m_pFontManager->Print (345, 280, "3", m_Font);
	  glLineWidth (2);
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (295, 274); glVertex2d (359, 274); glVertex2d (359, 298); glVertex2d (348, 298);
	  glEnd ();
	}

	// The hydraulics indicators

	// Background rectangles
	glColor3ub (COLOR_GRAY50);
	glBegin (GL_POLYGON); // Spoilers
	  glVertex2d (213, 397); glVertex2d (250, 397); glVertex2d (250, 415); glVertex2d (213, 415);
	glEnd ();
	glBegin (GL_POLYGON); // Left aileron
	  glVertex2d ( 77, 267); glVertex2d (107, 267); glVertex2d (107, 285); glVertex2d ( 77, 285);
	glEnd ();
	glBegin (GL_POLYGON); // Right aileron
	  glVertex2d (362, 267); glVertex2d (393, 267); glVertex2d (393, 285); glVertex2d (362, 285);
	glEnd ();
	glBegin (GL_POLYGON); // Left elevator
	  glVertex2d ( 82, 132); glVertex2d (114, 132); glVertex2d (114, 150); glVertex2d ( 82, 150);
	glEnd ();
	glBegin (GL_POLYGON); // Right elevator
	  glVertex2d (352, 132); glVertex2d (384, 132); glVertex2d (384, 150); glVertex2d (352, 150);
	glEnd ();
	glBegin (GL_POLYGON); // Pitch trim
	  glVertex2d (262, 220); glVertex2d (292, 220); glVertex2d (292, 238); glVertex2d (262, 238);
	glEnd ();
	glBegin (GL_POLYGON); // Rudder
	  glVertex2d (213, 150); glVertex2d (250, 150); glVertex2d (250, 168); glVertex2d (213, 168);
	glEnd ();

	// Boxes
	// Left aileron
	if (elac1 == 0) {
	  glColor3ub (COLOR_AMBER);
	  glBegin (GL_LINE_STRIP);
	    glVertex2d ( 78, 268); glVertex2d ( 91, 268); glVertex2d ( 91, 284); glVertex2d ( 78, 284); glVertex2d ( 78, 268);
	  glEnd ();
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (363, 268); glVertex2d (377, 268); glVertex2d (377, 284); glVertex2d (363, 284); glVertex2d (363, 268);
	  glEnd ();
	}
	// Right aileron
	if (elac2 == 0) {
	  glColor3ub (COLOR_AMBER);
	  glBegin (GL_LINE_STRIP);
	    glVertex2d ( 93, 268); glVertex2d (106, 268); glVertex2d (106, 284); glVertex2d ( 93, 284); glVertex2d ( 93, 268);
	  glEnd ();
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (378, 268); glVertex2d (392, 268); glVertex2d (392, 284); glVertex2d (378, 284); glVertex2d (378, 268);
	  glEnd ();
	}

	// The indicator letters
	if (hydPressGreen < 1500) glColor3ub (COLOR_AMBER);
	else glColor3ub (COLOR_GREEN);
	m_pFontManager->Print (216, 400, "G", m_Font); // Spoilers
	m_pFontManager->Print ( 95, 270, "G", m_Font); // Left aileron
	m_pFontManager->Print (365, 270, "G", m_Font); // Right aileron
	m_pFontManager->Print (100, 135, "G", m_Font); // Left elevator
	m_pFontManager->Print (265, 223, "G", m_Font); // Pitch trim
	m_pFontManager->Print (216, 153, "G", m_Font); // Rudder
	if (hydPressYellow < 1500) glColor3ub (COLOR_AMBER);
	else glColor3ub (COLOR_GREEN);
	m_pFontManager->Print (236, 400, "Y", m_Font); // Spoilers
	m_pFontManager->Print (355, 135, "Y", m_Font); // Right elevator
	m_pFontManager->Print (280, 223, "Y", m_Font); // Pitch trim
	m_pFontManager->Print (236, 153, "Y", m_Font); // Rudder
	if (hydPressBlue < 1500) glColor3ub (COLOR_AMBER);
	else glColor3ub (COLOR_GREEN);
	m_pFontManager->Print (226, 400, "B", m_Font); // Spoilers
	m_pFontManager->Print (380, 270, "B", m_Font); // Right aileron
	m_pFontManager->Print ( 85, 135, "B", m_Font); // Left elevator
	m_pFontManager->Print ( 80, 270, "B", m_Font); // Left aileron
	m_pFontManager->Print (370, 135, "B", m_Font); // Right elevator
	m_pFontManager->Print (226, 153, "B", m_Font); // Rudder

/* end of inlined code */


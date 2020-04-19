/*=============================================================================

  This is the sdcond.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style System Display Widget ===

  This file contains the code for the SD subpage  #7: SD_COND
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

/* inlined code: SD COND Page */

	m_pFontManager->SetSize (m_Font, 14, 14);
	m_pFontManager->Print (30, 430, "COND", m_Font);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES); // title underlining
	  glVertex2d (30, 425);
	  glVertex2d (75, 425);
	glEnd ();

	// The outline
	aCircle.SetOrigin ( 90, 340);
	aCircle.SetRadius (23);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (180, 310);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd ();
	glLineWidth (3);
	glBegin (GL_LINE_STRIP);
	  glVertex2d ( 72, 354);
	  glVertex2d (115, 390);
	  glVertex2d (370, 390);
	  glVertex2d (385, 380);
	glEnd ();
	glBegin (GL_LINES);
	  glVertex2d (170, 390);
	  glVertex2d (170, 317);
	  glVertex2d (270, 370);
	  glVertex2d (270, 317);
	  glVertex2d ( 90, 317);
	  glVertex2d (110, 317);
	  glVertex2d (140, 317);
	  glVertex2d (205, 317);
	  glVertex2d (235, 317);
	  glVertex2d (300, 317);
	glEnd ();
	glBegin (GL_LINE_STRIP);
	  glVertex2d (330, 317);
	  glVertex2d (370, 317);
	  glVertex2d (385, 327);
	glEnd ();
	aCircle.SetOrigin (125, 250);
	aCircle.SetRadius (50);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (330,  30);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd ();
	glBegin (GL_LINES);
	  glVertex2d (125, 300);
	  glVertex2d (125, 295);
	glEnd ();
	aCircle.SetOrigin (220, 250);
	aCircle.SetRadius (50);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (330,  30);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd ();
	glBegin (GL_LINES);
	  glVertex2d (220, 300);
	  glVertex2d (220, 295);
	glEnd ();
	aCircle.SetOrigin (315, 250);
	aCircle.SetRadius (50);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (330,  30);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd ();
	glBegin (GL_LINES);
	  glVertex2d (315, 300);
	  glVertex2d (315, 295);
	glEnd ();
	glColor3ub (COLOR_GREEN);
	glBegin (GL_LINES);
	  glVertex2d (125, 250);
	  glVertex2d (125, 240);
	  glVertex2d (220, 250);
	  glVertex2d (220, 240);
	  glVertex2d (315, 250);
	  glVertex2d (315, 240);
	  glVertex2d (125, 240);
	  glVertex2d (370, 240);
	  glVertex2d (400, 240);
	  glVertex2d (410, 240);
	glEnd ();
	aCircle.SetOrigin (385, 240);
	aCircle.SetRadius (15);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (  0, 360);
	glLineWidth (3);
	glColor3ub (COLOR_GREEN);
	glBegin (GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd ();

	// The fixed texts
	glColor3ub (COLOR_WHITE);
	m_pFontManager->SetSize (m_Font, 14, 14);
	m_pFontManager->Print (350, 410, "TEMP:", m_Font);
	m_pFontManager->Print (110, 366, "CKPT", m_Font);
	m_pFontManager->Print (205, 366, "FWD", m_Font);
	m_pFontManager->Print (300, 366, "AFT", m_Font);
	m_pFontManager->Print (410, 245, "HOT", m_Font);
	m_pFontManager->Print (410, 225, "AIR", m_Font);
	m_pFontManager->Print ( 95, 275, "C", m_Font);
	m_pFontManager->Print (145, 275, "H", m_Font);
	m_pFontManager->Print (190, 275, "C", m_Font);
	m_pFontManager->Print (240, 275, "H", m_Font);
	m_pFontManager->Print (285, 275, "C", m_Font);
	m_pFontManager->Print (335, 275, "H", m_Font);
	glLineWidth (3);
	glColor3ub (COLOR_CYAN);
	m_pFontManager->Print (420, 410, ";", m_Font);
	glLineWidth (3);

	// The needles
	glPushMatrix();
	  glLineWidth (3);
	  glColor3ub (COLOR_GREEN);
	  glTranslated (125, 250, 0);
	  glRotated (30 - 60 * ckptTrimValve, 0, 0, 1);
	  glBegin (GL_LINES);
	    glVertex2d (  0, 0);
	    glVertex2d (  0, 50);
	  glEnd ();
	glPopMatrix();
	glPushMatrix();
	  glLineWidth (3);
	  glColor3ub (COLOR_GREEN);
	  glTranslated (220, 250, 0);
	  glRotated (30 - 60 * forwTrimValve, 0, 0, 1);
	  glBegin (GL_LINES);
	    glVertex2d (  0, 0);
	    glVertex2d (  0, 50);
	  glEnd ();
	glPopMatrix();
	glPushMatrix();
	  glLineWidth (3);
	  glColor3ub (COLOR_GREEN);
	  glTranslated (315, 250, 0);
	  glRotated (30 - 60 * aftTrimValve, 0, 0, 1);
	  glBegin (GL_LINES);
	    glVertex2d (  0, 0);
	    glVertex2d (  0, 50);
	  glEnd ();
	glPopMatrix();

	// The numeric values // NOTE: totally unclear how this works / should work!
	m_pFontManager->SetSize (m_Font, 14, 14);
	glColor3ub (COLOR_GREEN);
	sprintf (buffer, "%2.0f", 50 * ckptTrimValve); // NOTE: Used these instead of the duct temps - not available in V2!
	m_pFontManager->Print (115, 310, buffer, m_Font);
	sprintf (buffer, "%2.0f", 50 * forwTrimValve);
	m_pFontManager->Print (210, 310, buffer, m_Font);
	sprintf (buffer, "%2.0f", 50 * aftTrimValve);
	m_pFontManager->Print (305, 310, buffer, m_Font);
	sprintf (buffer, "%2i", cockpitTemp);
	m_pFontManager->Print (115, 346, buffer, m_Font);
	sprintf (buffer, "%2i", fwdCabinTemp);
	m_pFontManager->Print (210, 346, buffer, m_Font);
	sprintf (buffer, "%2i", aftCabinTemp);
	m_pFontManager->Print (305, 346, buffer, m_Font);

	// The Hot-Air valve
	glLineWidth (3);
	glColor3ub (COLOR_GREEN);
	if (hotAirValve == 0) {
	  glBegin (GL_LINES);
	    glVertex2d (385, 225);
	    glVertex2d (385, 255);
	  glEnd ();
	} else {
	  glBegin (GL_LINES);
	    glVertex2d (370, 240);
	    glVertex2d (400, 240);
	  glEnd ();
	}

/* end of inlined code */


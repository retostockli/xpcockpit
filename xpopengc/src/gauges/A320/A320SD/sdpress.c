/*=============================================================================

  This is the sdpress.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style System Display Widget ===

  This file contains the code for the SD subpage  #2: SD_PRESS
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

/* inlined code: SD PRESS Page */

	m_pFontManager->SetSize(m_Font, 14, 14);
	m_pFontManager->Print (150, 440, "CAB PRESS", m_Font);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES); // title underlining
	  glVertex2d (150, 435);
	  glVertex2d (254, 435);
	glEnd ();

	// The fixed background stuff

	// Air Conditioning
	glLineWidth (2);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP);
	  glVertex2d (410, 205);	
	  glVertex2d (410, 210);	
	  glVertex2d ( 70, 210);	
	  glVertex2d ( 70, 115);	
	  glVertex2d ( 95, 115);	
	  glVertex2d ( 95, 105);	
	glEnd ();
	glBegin (GL_LINE_STRIP);
	  glVertex2d (120, 105);	
	  glVertex2d (120, 115);	
	  glVertex2d (200, 115);	
	  glVertex2d (200, 105);	
	glEnd ();
	glBegin (GL_LINE_STRIP);
	  glVertex2d (225, 105);	
	  glVertex2d (225, 115);	
	  glVertex2d (305, 115);	
	glEnd ();
	glBegin (GL_LINE_STRIP);
	  glVertex2d (350, 115);	
	  glVertex2d (410, 115);	
	  glVertex2d (410, 155);	
	  glVertex2d (420, 155);	
	glEnd ();
	glBegin (GL_LINE_STRIP);
	  glVertex2d (420, 175);	
	  glVertex2d (410, 175);	
	  glVertex2d (410, 185);	
	glEnd ();

	// Pressure Difference
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	aCircle.SetOrigin (110, 285);
	aCircle.SetRadius (45);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (225,  45);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	glColor3ub (COLOR_RED);
	aCircle.SetArcStartEnd (215, 225);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	aCircle.SetArcStartEnd ( 45,  55);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES);
	  glVertex2d ( 78, 253); glVertex2d ( 81, 256);
	  glVertex2d (138, 313); glVertex2d (141, 316);
	glEnd ();

	// Vertical Speed
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	aCircle.SetOrigin (230, 285);
	aCircle.SetRadius (45);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (180,   0);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	glLineWidth (4);
	glColor3ub (COLOR_RED);
	aCircle.SetArcStartEnd (170, 180);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	aCircle.SetArcStartEnd (  0,  10);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES);
	  glVertex2d (230, 330); glVertex2d (230, 320);
	  glVertex2d (230, 240); glVertex2d (230, 250);
	  glVertex2d (185, 285); glVertex2d (190, 285);
	  glVertex2d (198, 316); glVertex2d (201, 313);
	  glVertex2d (198, 253); glVertex2d (201, 256);
	glEnd ();

	// Cabin Altitude
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	aCircle.SetOrigin (365, 285);
	aCircle.SetRadius (45);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (225,  45);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	glColor3ub (COLOR_RED);
	aCircle.SetArcStartEnd (215, 225);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	aCircle.SetArcStartEnd ( 45,  55);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES);
	  glVertex2d (333, 253); glVertex2d (336, 256);
	  glVertex2d (394, 313); glVertex2d (397, 316);
	glEnd ();

	// Outflow Valve
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	aCircle.SetOrigin (351, 115);
	aCircle.SetRadius (45);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (270,   0);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES);
	  glVertex2d (350, 160); glVertex2d (350, 170);
	  glVertex2d (330, 162); glVertex2d (333, 156);
	  glVertex2d (313, 151); glVertex2d (318, 146);
	  glVertex2d (301, 134); glVertex2d (308, 132);
	glEnd ();

	// Packs
	glLineWidth (2);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP);
	  glVertex2d ( 70,  97);	
	  glVertex2d ( 82,  97);	
	  glVertex2d ( 76, 107);	
	  glVertex2d ( 70,  97);	
	glEnd ();
	glBegin (GL_LINE_STRIP);
	  glVertex2d (390,  97);	
	  glVertex2d (402,  97);	
	  glVertex2d (396, 107);	
	  glVertex2d (390,  97);	
	glEnd ();
	m_pFontManager->SetSize(m_Font, 12, 12);
	m_pFontManager->Print ( 50,  80, "PACK 1", m_Font);
	m_pFontManager->Print (373,  80, "PACK 2", m_Font);

	// The texts

	glColor3ub (COLOR_WHITE);
	m_pFontManager->SetSize(m_Font, 12, 12);
	m_pFontManager->Print (208, 400, "LDG ELEV", m_Font);
	m_pFontManager->Print (105, 360, "&P", m_Font); // & == delta-sign
	m_pFontManager->Print (217, 360, "V/S", m_Font);
	m_pFontManager->Print (330, 360, "CAB ALT", m_Font);
	m_pFontManager->Print ( 85, 255, "0", m_Font);
	m_pFontManager->Print (128, 302, "8", m_Font);
	m_pFontManager->Print (193, 279, "0", m_Font);
	m_pFontManager->Print (225, 305, " 2", m_Font);
	m_pFontManager->Print (225, 252, "-2", m_Font);
	m_pFontManager->Print (341, 255, "0", m_Font);
	m_pFontManager->Print (379, 304, "10", m_Font);
	m_pFontManager->Print (140, 138, "VENT", m_Font);
	m_pFontManager->Print ( 88, 120, "INLET", m_Font);
	m_pFontManager->Print (183, 120, "OUTLET", m_Font);
	glColor3ub (COLOR_CYAN);
	m_pFontManager->Print (400, 400, "FT", m_Font);
	m_pFontManager->Print ( 99, 340, "PSI", m_Font);
	m_pFontManager->Print (205, 340, "FT/MIN", m_Font);
	m_pFontManager->Print (355, 340, "FT", m_Font);
	glColor3ub (COLOR_GREEN); // Note: maybe variable?
	m_pFontManager->Print (300, 400, "AUTO", m_Font);
//	m_pFontManager->Print (215, 188, "MAN", m_Font);
	m_pFontManager->Print (140, 175, "SYS1", m_Font);
//	m_pFontManager->Print (280, 175, "SYS2", m_Font);

	// The needles

	// Pressure Difference
//AirbusFBW/CabinDeltaP	float	n
//cabinDeltaP = 2.5;
	glColor3ub (COLOR_GREEN);
	glLineWidth (3);
	glPushMatrix();
	  glTranslated (110, 285, 0);
	  glRotated (135 - (cabinDeltaP * 180 / 8), 0, 0, 1);
	  glBegin (GL_LINES);
	    glVertex2d (  0,  0);
	    glVertex2d (  0, 40);
	  glEnd ();
	glPopMatrix();
	sprintf(buffer, "%4.1f", cabinDeltaP);
	m_pFontManager->Print (125, 255, buffer, m_Font);

	// Vertical Speed
//AirbusFBW/CabinVS	float	n
//cabinVS = -1000;
	glColor3ub (COLOR_GREEN);
	glLineWidth (3);
	glPushMatrix();
	  glTranslated (230, 285, 0);
	  glRotated (90 - ((cabinVS / 1000) * 90 / 2), 0, 0, 1);
	  glBegin (GL_LINES);
	    glVertex2d (  0,  0);
	    glVertex2d (  0, 40);
	  glEnd ();
	glPopMatrix();
	sprintf(buffer, "%5.0f", cabinVS);
	m_pFontManager->Print (250, 279, buffer, m_Font);

	// Cabin Altitude
//AirbusFBW/CabinAlt	float	n
//cabinAlt = 70;
	glColor3ub (COLOR_GREEN);
	glLineWidth (3);
	glPushMatrix();
	  glTranslated (365, 285, 0);
	  glRotated (135 - ((cabinAlt / 1000) * 180 / 10), 0, 0, 1);
	  glBegin (GL_LINES);
	    glVertex2d (  0,  0);
	    glVertex2d (  0, 40);
	  glEnd ();
	glPopMatrix();
	sprintf(buffer, "%3.0f", cabinAlt);
	m_pFontManager->Print (400, 255, buffer, m_Font);

	// Outflow Valve
//AirbusFBW/OutflowValve	float	n
//outflowValve = 1.0;
	glColor3ub (COLOR_GREEN);
	glLineWidth (3);
	glPushMatrix();
	  glTranslated (351, 115, 0);
	  glRotated (90 - (outflowValve * 90), 0, 0, 1);
	  glBegin (GL_LINES);
	    glVertex2d (  0,  0);
	    glVertex2d (  0, 43);
	  glEnd ();
	glPopMatrix();
	aCircle.SetOrigin (351, 115);
	aCircle.SetRadius (2);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (  0, 360);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();

	// Safety Valve
//safetyValve = 1;
	if (safetyValve == 0) {
	  glColor3ub (COLOR_GREEN);
	  glLineWidth (3);
	  glBegin (GL_LINES);
	    glVertex2d (420, 175);
	    glVertex2d (420, 155);
	  glEnd ();
	} else {
	  glColor3ub (COLOR_AMBER);
	  glBegin (GL_LINES);
	    glVertex2d (420, 175);
	    glVertex2d (430, 155);
	  glEnd ();
	}
	aCircle.SetOrigin (420, 175);
	aCircle.SetRadius (2);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (  0, 360);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	if (safetyValve == 0) {
	  glColor3ub (COLOR_WHITE);
	}
	m_pFontManager->Print (380, 188, "SAFETY", m_Font);

	// Inlet Valve
//hotAirValve = 1;
	glLineWidth (3);
	if (hotAirValve == 0) {
	  glColor3ub (COLOR_GREEN);
	  glBegin (GL_LINES);
	    glVertex2d (120, 105);
	    glVertex2d ( 95, 105);
	  glEnd ();
	} else {
	  glColor3ub (COLOR_AMBER);
	  glBegin (GL_LINES);
	    glVertex2d (120, 105);
	    glVertex2d ( 96,  95);
	  glEnd ();
	}
	aCircle.SetOrigin (120, 105);
	aCircle.SetRadius (2);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (  0, 360);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	// Outlet Valve (which variable?)
	glLineWidth (3);
	if (hotAirValve == 0) { // other variable?
	  glColor3ub (COLOR_GREEN);
	  glBegin (GL_LINES);
	    glVertex2d (200, 105);
	    glVertex2d (225, 105);
	  glEnd ();
	} else {
	  glColor3ub (COLOR_AMBER);
	  glBegin (GL_LINES);
	    glVertex2d (200, 105);
	    glVertex2d (224,  95);
	  glEnd ();
	}
	aCircle.SetOrigin (200, 105);
	aCircle.SetRadius (2);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (  0, 360);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();

/* end of inlined code */


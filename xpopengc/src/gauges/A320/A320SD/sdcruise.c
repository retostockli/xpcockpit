/*=============================================================================

  This is the sdcruise.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style System Display Widget ===

  This file contains the code for the SD subpage #11: SD_CRUISE
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

/* inlined code: SD CRUISE Page */

	m_pFontManager->SetSize(m_Font, 14, 14);
	m_pFontManager->Print (194, 440, "CRUISE", m_Font);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES); // title underlining
	  glVertex2d (194, 435);
	  glVertex2d (266, 435);
	glEnd ();

	// The outline drawing

	glLineWidth (2);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINE_STRIP);
	  glVertex2d ( 59, 171);
	  glVertex2d ( 70, 183);
	  glVertex2d (230, 183);
	glEnd ();
	aCircle.SetOrigin ( 37, 130);
	aCircle.SetRadius (10);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (180, 320);
	glLineWidth (3);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	glBegin (GL_LINES);
	  glVertex2d ( 29, 136);
	  glVertex2d ( 38, 147);
	glEnd ();
	glBegin (GL_LINES);
	  glVertex2d ( 37, 120);
	  glVertex2d (252, 120);
	glEnd ();
	glBegin (GL_LINES);
	  glVertex2d ( 98, 120);
	  glVertex2d ( 98, 183);
	glEnd ();
	glBegin (GL_LINES);
	  glVertex2d (177, 140);
	  glVertex2d (177, 120);
	glEnd ();
	glBegin (GL_LINES);
	  glVertex2d (177, 108);
	  glVertex2d (177,  95);
	glEnd ();
	glBegin (GL_LINE_STRIP);
	glBegin (GL_LINES);
	  glVertex2d ( 98, 108);
	  glVertex2d ( 98,  95);
	  glVertex2d (230,  95);
	glEnd ();

	// The subtitles

	glColor3ub (COLOR_WHITE);
	m_pFontManager->SetSize(m_Font, 14, 14);
	m_pFontManager->Print ( 20, 400, "ENG", m_Font);
	glLineWidth (3);
	glBegin (GL_LINES); // subtitle underlining
	  glVertex2d ( 20, 395);
	  glVertex2d ( 55, 395);
	glEnd ();
	m_pFontManager->Print ( 20, 230, "AIR", m_Font);
	glLineWidth (3);
	glBegin (GL_LINES); // subtitle underlining
	  glVertex2d ( 20, 225);
	  glVertex2d ( 55, 225);
	glEnd ();

	// The texts

	m_pFontManager->SetSize(m_Font, 12, 12);
	glColor3ub (COLOR_WHITE);
	m_pFontManager->Print (200, 393, "F.USED", m_Font);
	m_pFontManager->Print (205, 377, "1 + 2", m_Font);
	m_pFontManager->Print (216, 325, "OIL", m_Font);
	m_pFontManager->Print (200, 285, "VIB N1", m_Font);
	m_pFontManager->Print (200, 265, "    N2", m_Font);
	m_pFontManager->Print (190, 230, "LDG ELEV", m_Font);
	m_pFontManager->Print (175, 210, "&P", m_Font); // (delta-P)
	m_pFontManager->Print (355, 210, "CAB V/S", m_Font);
	m_pFontManager->Print (355, 150, "CAB ALT", m_Font);
	m_pFontManager->Print ( 40, 154, "CKPT", m_Font);
	m_pFontManager->Print (125, 154, "FWD", m_Font);
	m_pFontManager->Print (205, 154, "AFT", m_Font);

	glLineWidth (3);
	glBegin (GL_LINES);
	  glVertex2d (160, 385); // F.USED 1
	  glVertex2d (185, 395);
	  glVertex2d (275, 395); // F.USED 2
	  glVertex2d (300, 385);
	  glVertex2d (160, 328); // OIL 1
	  glVertex2d (185, 328);
	  glVertex2d (275, 328); // OIL 2
	  glVertex2d (300, 328);
	  glVertex2d (160, 288); // VIB N1 1
	  glVertex2d (185, 288);
	  glVertex2d (275, 288); // VIB N1 2
	  glVertex2d (300, 288);
	  glVertex2d (160, 268); // VIB N2 1
	  glVertex2d (185, 268);
	  glVertex2d (275, 268); // VIB N2 2
	  glVertex2d (300, 268);
	glEnd ();

	glColor3ub (COLOR_CYAN);
	m_pFontManager->Print (218, 344, "KG", m_Font);
	m_pFontManager->Print (218, 308, "QT", m_Font);
	m_pFontManager->Print (250, 210, "PSI", m_Font);
	m_pFontManager->Print (395, 230, "FT", m_Font);
	m_pFontManager->Print (395, 190, "FT/MIN", m_Font);
	m_pFontManager->Print (395, 130, "FT", m_Font);
	m_pFontManager->Print (255, 125, ";", m_Font); // (degrees-C)

	// The values

	m_pFontManager->SetSize(m_Font, 14, 14);
	glColor3ub (COLOR_GREEN);
	sprintf(buffer, "%5.0f", eng1FuelUsed);
	m_pFontManager->Print ( 90, 370, buffer, m_Font); // f.used 1
	sprintf(buffer, "%5.0f", eng2FuelUsed);
	m_pFontManager->Print (310, 370, buffer, m_Font); // f.used 2
	sprintf(buffer, "%5.0f", eng1FuelUsed + eng2FuelUsed);
	m_pFontManager->Print (202, 360, buffer, m_Font); // f.used 1+2
	sprintf(buffer, "%3.1f", eng1OilQty);
	m_pFontManager->Print ( 90, 325, buffer, m_Font); // oil qty 1
	sprintf(buffer, "%3.1f", eng2OilQty);
	m_pFontManager->Print (310, 325, buffer, m_Font); // oil qty 2
//	sprintf(buffer, "%3.1f", eng1N1Vib); // NOTE: not available in sim or plane!
	sprintf(buffer, "%3.1f", 0.0);
	m_pFontManager->Print ( 90, 285, buffer, m_Font); // vib N1 1
//	sprintf(buffer, "%3.1f", eng2N1Vib); // NOTE: not available in sim or plane!
	sprintf(buffer, "%3.1f", 0.0);
	m_pFontManager->Print (310, 285, buffer, m_Font); // vib N1 2
//	sprintf(buffer, "%3.1f", eng1N2Vib); // NOTE: not available in sim or plane!
	sprintf(buffer, "%3.1f", 0.0);
	m_pFontManager->Print ( 90, 265, buffer, m_Font); // vib N2 1
//	sprintf(buffer, "%3.1f", eng2N2Vib); // NOTE: not available in sim or plane!
	sprintf(buffer, "%3.1f", 0.0);
	m_pFontManager->Print (310, 265, buffer, m_Font); // vib N2 1
//	sprintf(buffer, "%5.0f", xxx);
//	m_pFontManager->Print (380, 230, buffer, m_Font); // ldg elev // NOTE: not available in sim or plane!
	m_pFontManager->Print (330, 230, "AUTO", m_Font); // ldg elev
	sprintf(buffer, "%4.0f", 0.0);
	m_pFontManager->Print (200, 210, buffer, m_Font); // psi
	sprintf(buffer, "%4.0f", 0.0);
	m_pFontManager->Print (340, 190, buffer, m_Font); // cab v/s
	sprintf(buffer, "%5.0f", 0.0);
	m_pFontManager->Print (330, 130, buffer, m_Font); // cab alt
	sprintf(buffer, "%3i", cockpitTemp);
	m_pFontManager->Print ( 45, 130, buffer, m_Font); // ckpt temp
	sprintf(buffer, "%3i", fwdCabinTemp);
	m_pFontManager->Print (120, 130, buffer, m_Font); // fwd temp
	sprintf(buffer, "%3i", aftCabinTemp);
	m_pFontManager->Print (200, 130, buffer, m_Font); // aft temp

/* end of inlined code */


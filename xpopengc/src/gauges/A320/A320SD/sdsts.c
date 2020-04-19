/*=============================================================================

  This is the sdsts.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style System Display Widget ===

  This file contains the code for the SD subpage #12: SD_STATUS
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

/* inlined code: SD STS Page */

	m_pFontManager->SetSize(m_Font, 14, 14);
	m_pFontManager->Print (285, 440, "STATUS", m_Font);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES); // title underlining and vertical separator
	  glVertex2d (285, 435);
	  glVertex2d (355, 435);
	  glVertex2d (320,  80);
	  glVertex2d (320, 425);
	glEnd ();
	glColor3ub (COLOR_AMBER);
	m_pFontManager->SetSize(m_Font, 13, 13);
	m_pFontManager->Print (350, 420, "INOP SYS", m_Font);
	glLineWidth (3);
	glBegin (GL_LINES);
	  glVertex2d (350, 415);
	  glVertex2d (435, 415);
	glEnd ();
	m_pFontManager->SetSize(m_Font, 15, 14);
/*
	glColor3ub (COLOR_GREEN); // test lines only!
//	m_pFontManager->Print (10, 416, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line  1 test
	m_pFontManager->Print (10, 396, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line  2 test
	m_pFontManager->Print (10, 376, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line  3 test
	m_pFontManager->Print (10, 356, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line  4 test
	m_pFontManager->Print (10, 336, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line  5 test
	m_pFontManager->Print (10, 316, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line  6 test
	m_pFontManager->Print (10, 296, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line  7 test
	m_pFontManager->Print (10, 276, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line  8 test
	m_pFontManager->Print (10, 256, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line  9 test
	m_pFontManager->Print (10, 236, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line 10 test
	m_pFontManager->Print (10, 216, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line 11 test
	m_pFontManager->Print (10, 196, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line 12 test
	m_pFontManager->Print (10, 176, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line 13 test
	m_pFontManager->Print (10, 156, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line 14 test
	m_pFontManager->Print (10, 136, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line 15 test
	m_pFontManager->Print (10, 116, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line 16 test
	m_pFontManager->Print (10,  96, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line 17 test
	m_pFontManager->Print (10,  76, "abcdefghijklmnopqrstuvwxyz0123456789", m_Font); // Line 18 test
/**/
	glColor3ub (COLOR_AMBER);
	m_pFontManager->Print (10, 416, (const char*) SD_1a, m_Font); // Line  1 amber
	m_pFontManager->Print (10, 396, (const char*) SD_2a, m_Font); // Line  2 amber
	m_pFontManager->Print (10, 376, (const char*) SD_3a, m_Font); // Line  3 amber
	m_pFontManager->Print (10, 356, (const char*) SD_4a, m_Font); // Line  4 amber
	m_pFontManager->Print (10, 336, (const char*) SD_5a, m_Font); // Line  5 amber
	m_pFontManager->Print (10, 316, (const char*) SD_6a, m_Font); // Line  6 amber
	m_pFontManager->Print (10, 296, (const char*) SD_7a, m_Font); // Line  7 amber
	m_pFontManager->Print (10, 276, (const char*) SD_8a, m_Font); // Line  8 amber
	m_pFontManager->Print (10, 256, (const char*) SD_9a, m_Font); // Line  9 amber
	m_pFontManager->Print (10, 236, (const char*) SD_10a, m_Font); // Line 10 amber
	m_pFontManager->Print (10, 216, (const char*) SD_11a, m_Font); // Line 11 amber
	m_pFontManager->Print (10, 196, (const char*) SD_12a, m_Font); // Line 12 amber
	m_pFontManager->Print (10, 176, (const char*) SD_13a, m_Font); // Line 13 amber
	m_pFontManager->Print (10, 156, (const char*) SD_14a, m_Font); // Line 14 amber
	m_pFontManager->Print (10, 136, (const char*) SD_15a, m_Font); // Line 15 amber
	m_pFontManager->Print (10, 116, (const char*) SD_16a, m_Font); // Line 16 amber
	m_pFontManager->Print (10,  96, (const char*) SD_17a, m_Font); // Line 17 amber
	m_pFontManager->Print (10,  76, (const char*) SD_18a, m_Font); // Line 18 amber
	glColor3ub (COLOR_CYAN);
	m_pFontManager->Print (10, 416, (const char*) SD_1b, m_Font); // Line  1 blue
	m_pFontManager->Print (10, 396, (const char*) SD_2b, m_Font); // Line  2 blue
	m_pFontManager->Print (10, 376, (const char*) SD_3b, m_Font); // Line  3 blue
	m_pFontManager->Print (10, 356, (const char*) SD_4b, m_Font); // Line  4 blue
	m_pFontManager->Print (10, 336, (const char*) SD_5b, m_Font); // Line  5 blue
	m_pFontManager->Print (10, 316, (const char*) SD_6b, m_Font); // Line  6 blue
	m_pFontManager->Print (10, 296, (const char*) SD_7b, m_Font); // Line  7 blue
	m_pFontManager->Print (10, 276, (const char*) SD_8b, m_Font); // Line  8 blue
	m_pFontManager->Print (10, 256, (const char*) SD_9b, m_Font); // Line  9 blue
	m_pFontManager->Print (10, 236, (const char*) SD_10b, m_Font); // Line 10 blue
	m_pFontManager->Print (10, 216, (const char*) SD_11b, m_Font); // Line 11 blue
	m_pFontManager->Print (10, 196, (const char*) SD_12b, m_Font); // Line 12 blue
	m_pFontManager->Print (10, 176, (const char*) SD_13b, m_Font); // Line 13 blue
	m_pFontManager->Print (10, 156, (const char*) SD_14b, m_Font); // Line 14 blue
	m_pFontManager->Print (10, 136, (const char*) SD_15b, m_Font); // Line 15 blue
	m_pFontManager->Print (10, 116, (const char*) SD_16b, m_Font); // Line 16 blue
	m_pFontManager->Print (10,  96, (const char*) SD_17b, m_Font); // Line 17 blue
	m_pFontManager->Print (10,  76, (const char*) SD_18b, m_Font); // Line 18 blue
	glColor3ub (COLOR_GREEN);
	m_pFontManager->Print (10, 416, (const char*) SD_1g, m_Font); // Line  1 green
	m_pFontManager->Print (10, 396, (const char*) SD_2g, m_Font); // Line  2 green
	m_pFontManager->Print (10, 376, (const char*) SD_3g, m_Font); // Line  3 green
	m_pFontManager->Print (10, 356, (const char*) SD_4g, m_Font); // Line  4 green
	m_pFontManager->Print (10, 336, (const char*) SD_5g, m_Font); // Line  5 green
	m_pFontManager->Print (10, 316, (const char*) SD_6g, m_Font); // Line  6 green
	m_pFontManager->Print (10, 296, (const char*) SD_7g, m_Font); // Line  7 green
	m_pFontManager->Print (10, 276, (const char*) SD_8g, m_Font); // Line  8 green
	m_pFontManager->Print (10, 256, (const char*) SD_9g, m_Font); // Line  9 green
	m_pFontManager->Print (10, 236, (const char*) SD_10g, m_Font); // Line 10 green
	m_pFontManager->Print (10, 216, (const char*) SD_11g, m_Font); // Line 11 green
	m_pFontManager->Print (10, 196, (const char*) SD_12g, m_Font); // Line 12 green
	m_pFontManager->Print (10, 176, (const char*) SD_13g, m_Font); // Line 13 green
	m_pFontManager->Print (10, 156, (const char*) SD_14g, m_Font); // Line 14 green
	m_pFontManager->Print (10, 136, (const char*) SD_15g, m_Font); // Line 15 green
	m_pFontManager->Print (10, 116, (const char*) SD_16g, m_Font); // Line 16 green
	m_pFontManager->Print (10,  96, (const char*) SD_17g, m_Font); // Line 17 green
	m_pFontManager->Print (10,  76, (const char*) SD_18g, m_Font); // Line 18 green
	glColor3ub (COLOR_RED);
	m_pFontManager->Print (10, 416, (const char*) SD_1r, m_Font); // Line  1 red
	m_pFontManager->Print (10, 396, (const char*) SD_2r, m_Font); // Line  2 red
	m_pFontManager->Print (10, 376, (const char*) SD_3r, m_Font); // Line  3 red
	m_pFontManager->Print (10, 356, (const char*) SD_4r, m_Font); // Line  4 red
	m_pFontManager->Print (10, 336, (const char*) SD_5r, m_Font); // Line  5 red
	m_pFontManager->Print (10, 316, (const char*) SD_6r, m_Font); // Line  6 red
	m_pFontManager->Print (10, 296, (const char*) SD_7r, m_Font); // Line  7 red
	m_pFontManager->Print (10, 276, (const char*) SD_8r, m_Font); // Line  8 red
	m_pFontManager->Print (10, 256, (const char*) SD_9r, m_Font); // Line  9 red
	m_pFontManager->Print (10, 236, (const char*) SD_10r, m_Font); // Line 10 red
	m_pFontManager->Print (10, 216, (const char*) SD_11r, m_Font); // Line 11 red
	m_pFontManager->Print (10, 196, (const char*) SD_12r, m_Font); // Line 12 red
	m_pFontManager->Print (10, 176, (const char*) SD_13r, m_Font); // Line 13 red
	m_pFontManager->Print (10, 156, (const char*) SD_14r, m_Font); // Line 14 red
	m_pFontManager->Print (10, 136, (const char*) SD_15r, m_Font); // Line 15 red
	m_pFontManager->Print (10, 116, (const char*) SD_16r, m_Font); // Line 16 red
	m_pFontManager->Print (10,  96, (const char*) SD_17r, m_Font); // Line 17 red
	m_pFontManager->Print (10,  76, (const char*) SD_18r, m_Font); // Line 18 red
	glColor3ub (COLOR_WHITE);
	m_pFontManager->Print (10, 416, (const char*) SD_1w, m_Font); // Line  1 white
	m_pFontManager->Print (10, 396, (const char*) SD_2w, m_Font); // Line  2 white
	m_pFontManager->Print (10, 376, (const char*) SD_3w, m_Font); // Line  3 white
	m_pFontManager->Print (10, 356, (const char*) SD_4w, m_Font); // Line  4 white
	m_pFontManager->Print (10, 336, (const char*) SD_5w, m_Font); // Line  5 white
	m_pFontManager->Print (10, 316, (const char*) SD_6w, m_Font); // Line  6 white
	m_pFontManager->Print (10, 296, (const char*) SD_7w, m_Font); // Line  7 white
	m_pFontManager->Print (10, 276, (const char*) SD_8w, m_Font); // Line  8 white
	m_pFontManager->Print (10, 256, (const char*) SD_9w, m_Font); // Line  9 white
	m_pFontManager->Print (10, 236, (const char*) SD_10w, m_Font); // Line 10 white
	m_pFontManager->Print (10, 216, (const char*) SD_11w, m_Font); // Line 11 white
	m_pFontManager->Print (10, 196, (const char*) SD_12w, m_Font); // Line 12 white
	m_pFontManager->Print (10, 176, (const char*) SD_13w, m_Font); // Line 13 white
	m_pFontManager->Print (10, 156, (const char*) SD_14w, m_Font); // Line 14 white
	m_pFontManager->Print (10, 136, (const char*) SD_15w, m_Font); // Line 15 white
	m_pFontManager->Print (10, 116, (const char*) SD_16w, m_Font); // Line 16 white
	m_pFontManager->Print (10,  96, (const char*) SD_17w, m_Font); // Line 17 white
	m_pFontManager->Print (10,  76, (const char*) SD_18w, m_Font); // Line 18 white

/* end of inlined code */


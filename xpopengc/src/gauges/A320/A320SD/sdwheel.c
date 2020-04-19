/*=============================================================================

  This is the sdwheel.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style System Display Widget ===

  This file contains the code for the SD subpage  #9: SD_WHEEL
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

/* inlined code: SD WHEEL Page */

	m_pFontManager->SetSize(m_Font, 14, 14);
	m_pFontManager->Print ( 10, 375, "WHEEL", m_Font);
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES); // title underlining
	  glVertex2d ( 10, 370); glVertex2d ( 67, 370);
	glEnd ();

	// The fixed background stuff

	// The wheel curves
	glLineWidth (3);
	glColor3ub (COLOR_WHITE);

	aCircle.SetOrigin (192, 345); // Nose wheel left
	aCircle.SetRadius (38);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (150, 210);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();

	aCircle.SetOrigin (264, 345); // Nose wheel right
	aCircle.SetArcStartEnd (150, 210);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();

	aCircle.SetOrigin ( 46, 166); // Outer main wheel left
	aCircle.SetArcStartEnd (150, 210);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	aCircle.SetArcStartEnd (-30,  30);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();

	aCircle.SetOrigin (138, 166); // Inner main wheel left
	aCircle.SetArcStartEnd (150, 210);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	aCircle.SetArcStartEnd (-30,  30);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();

	aCircle.SetOrigin (321, 166); // Inner main wheel right
	aCircle.SetArcStartEnd (150, 210);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	aCircle.SetArcStartEnd (-30,  30);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();

	aCircle.SetOrigin (413, 166); // Outer main wheel right
	aCircle.SetArcStartEnd (150, 210);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	aCircle.SetArcStartEnd (-30,  30);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();

	// The wheel numbers

	  glColor3ub (COLOR_WHITE);
	  m_pFontManager->SetSize(m_Font, 14, 14);
	  m_pFontManager->Print ( 40, 160, "1", m_Font);
	  m_pFontManager->Print (132, 160, "2", m_Font);
	  m_pFontManager->Print (315, 160, "3", m_Font);
	  m_pFontManager->Print (407, 160, "4", m_Font);
	  
	// Note: what about the wheel brake temperatures?

	// The door hinges
	glLineWidth (2);
	glColor3ub (COLOR_WHITE);
	glBegin (GL_LINES);
	  glVertex2d (174, 373); glVertex2d (186, 373); // Nose-wheel doors
	  glVertex2d (270, 373); glVertex2d (282, 373);
	  glVertex2d ( 24, 288); glVertex2d ( 44, 288); // Left main-wheel doors
	  glVertex2d (139, 288); glVertex2d (156, 288);
	  glVertex2d (303, 288); glVertex2d (320, 288); // Right main-wheel doors
	  glVertex2d (413, 288); glVertex2d (435, 288);
	glEnd ();

	glLineWidth (3);
	glColor3ub (COLOR_GREEN);

	aCircle.SetOrigin (192, 373); // Nose wheel door left
	aCircle.SetRadius (6);
	aCircle.SetDegreesPerPoint (5);
	aCircle.SetArcStartEnd (  0, 360);
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	aCircle.SetOrigin (264, 373); // Nose wheel door right
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	aCircle.SetOrigin (133, 288); // Main wheel door right
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();
	aCircle.SetOrigin (326, 288); // Main wheel door right
	glBegin(GL_LINE_STRIP);
	  aCircle.Evaluate ();
	glEnd();

	// The texts
	m_pFontManager->SetSize(m_Font, 12, 12);
	glColor3ub (COLOR_WHITE);
	m_pFontManager->Print ( 80, 160, "REL", m_Font);
	m_pFontManager->Print (355, 160, "REL", m_Font);
	glColor3ub (COLOR_CYAN);
	m_pFontManager->Print ( 85, 176, ";", m_Font);
	m_pFontManager->Print (360, 176, ";", m_Font);
	m_pFontManager->Print ( 80, 144, "PSI", m_Font);
	m_pFontManager->Print (355, 144, "PSI", m_Font);

/**//*
AirbusFBW/SDSpoilerArray		int[16]		(image Spoiler_1)		4 pos
AirbusFBW/SDSpoilerArray		int[16]		(image Spoiler_1)		4 pos
AirbusFBW/SDSpoilerArray		int[16]		(image Spoiler_2)		4 pos
AirbusFBW/SDSpoilerArray		int[16]		(image Spoiler_2)		4 pos
AirbusFBW/SDSpoilerArray		int[16]		(image Spoiler_3)		4 pos
AirbusFBW/SDSpoilerArray		int[16]		(image Spoiler_3)		4 pos
AirbusFBW/SDSpoilerArray		int[16]		(image Spoiler_4)		4 pos
AirbusFBW/SDSpoilerArray		int[16]		(image Spoiler_4)		4 pos
AirbusFBW/SDSpoilerArray		int[16]		(image Spoiler_5)		4 pos
AirbusFBW/SDSpoilerArray		int[16]		(image Spoiler_5)		4 pos
AirbusFBW/LeftBrakeRelease		int		(image SD_WHEEL_Release)	1 pos	sh if AirbusFBW/LeftBrakeRelease >= 1
AirbusFBW/RightBrakeRelease		int		(image SD_WHEEL_Release)	1 pos	sh if AirbusFBW/RightBrakeRelease >= 1
AirbusFBW/LeftMLGDoor			float		(image LDG_MainL_Door)		4 pos
AirbusFBW/RightMLGDoor			float		(image LDG_MainR_Door)		4 pos
AirbusFBW/NoseGearDoorsClosed		float		(image LDG_NoseL_Door)		3 pos
AirbusFBW/NoseGearDoorsClosed		float		(image LDG_NoseR_Door)		3 pos
AirbusFBW/AltnBrake			int	
AirbusFBW/NWSAvail			int	
sim/flightmodel2/gear/deploy_ratio[0]	float[10]	(image LDG_Gear_SD)		3 pos
sim/flightmodel2/gear/deploy_ratio[1]	float[10]	(image LDG_Gear_SD)		3 pos
sim/flightmodel2/gear/deploy_ratio[2]	float[10]	(image LDG_Gear_SD)		3 pos
sim/operation/failures/rel_gear_ind	int		(image LGCIU_Inop_SD)		2 pos
sim/operation/failures/rel_gear_act	int		(image SD_HydSys_Backgnd_Square)	1 pos
/**/
	// The spoiler indicators
	// Note: for any changes to this part, also check the F/CTL page!
	m_pFontManager->SetSize(m_Font, 12, 12);
//spoiler1L = 0;
	glLineWidth (2);
	switch (spoiler1L) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (198, 416); glVertex2d (198, 432);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (192, 432); glVertex2d (198, 448); glVertex2d (204, 432); glVertex2d (192, 432);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (194, 418, "1", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (192, 432); glVertex2d (198, 448); glVertex2d (204, 432);
	    glEnd();
	    m_pFontManager->Print (194, 418, "1", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (190, 416); glVertex2d (206, 416);
	glEnd();
//spoiler1R = spoiler1L;
	glLineWidth (2);
	switch (spoiler1R) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (262, 416); glVertex2d (262, 432);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (256, 432); glVertex2d (262, 448); glVertex2d (270, 432); glVertex2d (256, 432);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (258, 418, "1", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (256, 432); glVertex2d (262, 448); glVertex2d (270, 432);
	    glEnd();
	    m_pFontManager->Print (258, 418, "1", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (254, 416); glVertex2d (270, 416);
	glEnd();
//spoiler2L = spoiler1L;
	glLineWidth (2);
	switch (spoiler2L) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (171, 414); glVertex2d (171, 430);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (165, 430); glVertex2d (171, 446); glVertex2d (177, 430); glVertex2d (165, 430);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (167, 416, "2", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (165, 430); glVertex2d (171, 446); glVertex2d (177, 430);
	    glEnd();
	    m_pFontManager->Print (167, 416, "2", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (163, 414); glVertex2d (179, 414);
	glEnd();
//spoiler2R = spoiler1L;
	glLineWidth (2);
	switch (spoiler2R) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (289, 414); glVertex2d (289, 430);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (283, 430); glVertex2d (289, 446); glVertex2d (295, 430); glVertex2d (283, 430);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (285, 416, "2", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (283, 430); glVertex2d (289, 446); glVertex2d (295, 430);
	    glEnd();
	    m_pFontManager->Print (285, 416, "2", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (281, 414); glVertex2d (297, 414);
	glEnd();
//spoiler3L = spoiler1L;
	glLineWidth (2);
	switch (spoiler3L) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (144, 412);glVertex2d (144, 426);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (138, 426); glVertex2d (144, 444); glVertex2d (150, 426); glVertex2d (138, 426);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (140, 414, "3", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (138, 426); glVertex2d (144, 444); glVertex2d (150, 426);
	    glEnd();
	    m_pFontManager->Print (140, 414, "3", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (136, 412); glVertex2d (152, 412);
	glEnd();
//spoiler3R = spoiler1L;
	glLineWidth (2);
	switch (spoiler3R) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (316, 412); glVertex2d (316, 428);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (310, 428); glVertex2d (316, 444); glVertex2d (322, 428); glVertex2d (310, 428);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (312, 414, "3", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (310, 428); glVertex2d (316, 444); glVertex2d (322, 428);
	    glEnd();
	    m_pFontManager->Print (312, 414, "3", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (308, 412); glVertex2d (324, 412);
	glEnd();
//spoiler4L = spoiler1L;
	glLineWidth (2);
	switch (spoiler4L) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (117, 408); glVertex2d (117, 424);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (111, 424); glVertex2d (117, 440); glVertex2d (123, 424); glVertex2d (111, 424);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (113, 410, "4", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (111, 424); glVertex2d (117, 440); glVertex2d (123, 424);
	    glEnd();
	    m_pFontManager->Print (113, 410, "4", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (109, 408); glVertex2d (125, 408);
	glEnd();
//spoiler4R = spoiler1L;
	glLineWidth (2);
	switch (spoiler4R) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (343, 408); glVertex2d (343, 424);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (337, 424); glVertex2d (343, 440); glVertex2d (349, 424); glVertex2d (337, 424);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (339, 410, "4", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (337, 424); glVertex2d (343, 440); glVertex2d (349, 424);
	    glEnd();
	    m_pFontManager->Print (339, 410, "4", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (335, 408); glVertex2d (351, 408);
	glEnd();
//spoiler5L = spoiler1L;
	glLineWidth (2);
	switch (spoiler5L) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d ( 90, 406); glVertex2d ( 90, 422);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d ( 84, 422); glVertex2d ( 90, 438); glVertex2d ( 96, 422); glVertex2d ( 84, 422);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print ( 86, 408, "5", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d ( 84, 422); glVertex2d ( 90, 438); glVertex2d ( 96, 422);
	    glEnd();
	    m_pFontManager->Print ( 86, 408, "5", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d ( 82, 406); glVertex2d ( 98, 406);
	glEnd();
//spoiler5R = spoiler1L;
	glLineWidth (2);
	switch (spoiler5R) {
	  case 0:
	    glColor3ub (COLOR_GREEN);
	  break;
	  case 1:
	    glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (370, 406); glVertex2d (370, 422);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (364, 422); glVertex2d (370, 438); glVertex2d (376, 422); glVertex2d (364, 422);
	    glEnd();
	  break;
	  case 2:
	    glColor3ub (COLOR_AMBER);
	    m_pFontManager->Print (366, 408, "5", m_Font);
	  break;
	  case 3:
	    glColor3ub (COLOR_AMBER);
	    glBegin(GL_LINE_STRIP);
	      glVertex2d (364, 422); glVertex2d (370, 438); glVertex2d (376, 422);
	    glEnd();
	    m_pFontManager->Print (366, 408, "5", m_Font);
	  break;
	}
	glLineWidth (2);
	glBegin(GL_LINES);
	  glVertex2d (362, 406); glVertex2d (378, 406);
	glEnd();

	// The Landing Gear Doors

//lgciuInop = 1;
	if (lgciuInop > 0) {
	  glColor3ub (COLOR_AMBER);
	  glLineWidth (3);
	  glBegin(GL_LINES);
	    glVertex2d (203, 343); glVertex2d (225, 367);
	    glVertex2d (203, 367); glVertex2d (225, 343);
	    glVertex2d (231, 343); glVertex2d (253, 367);
	    glVertex2d (231, 367); glVertex2d (253, 343);
	    glVertex2d ( 65, 256); glVertex2d ( 89, 282);
	    glVertex2d ( 65, 282); glVertex2d ( 89, 256);
	    glVertex2d ( 94, 256); glVertex2d (118, 282);
	    glVertex2d ( 94, 282); glVertex2d (118, 256);
	    glVertex2d (341, 256); glVertex2d (365, 282);
	    glVertex2d (341, 282); glVertex2d (365, 256);
	    glVertex2d (370, 256); glVertex2d (394, 282);
	    glVertex2d (370, 282); glVertex2d (394, 256);
	  glEnd();
	} else {
//noseGearDoor = 1.0;
//sprintf(buffer, "%2.1f", noseGearDoor);
//m_pFontManager->Print (213, 380, buffer, m_Font);
	  if (noseGearDoor < 0.1) {
	  glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (198, 373); glVertex2d (225, 373); // Doors closed
	      glVertex2d (231, 373); glVertex2d (258, 373);
	    glEnd();
	  } else {
	    glColor3ub (COLOR_AMBER);
	    if (noseGearDoor == 1.0) {
	      glBegin(GL_LINES);
	        glVertex2d (192, 367); glVertex2d (192, 340); // Doors open
	        glVertex2d (264, 367); glVertex2d (264, 340);
	      glEnd();
	    } else {
	      glPushMatrix();
	        glTranslated (192, 373, 0);
	        glRotated (45, 0, 0, 1);
	        glBegin(GL_LINES);
	          glVertex2d (0, -6); glVertex2d (0, -33); // Doors in transit
	        glEnd();
	      glPopMatrix();
	      glPushMatrix();
	        glTranslated (264, 373, 0);
	        glRotated (-45, 0, 0, 1);
	        glBegin(GL_LINES);
	          glVertex2d (0, -6); glVertex2d (0, -33); // Doors in transit
	        glEnd();
	      glPopMatrix();
	    }
	  }
//leftMlgDoor = noseGearDoor;
//sprintf(buffer, "%2.1f", leftMlgDoor);
//m_pFontManager->Print ( 75, 295, buffer, m_Font);
	  if (leftMlgDoor < 0.1) {
	  glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d ( 54, 288); glVertex2d (127, 288); // Door closed
	    glEnd();
	  } else {
	    glColor3ub (COLOR_AMBER);
	    if (leftMlgDoor == 1.0) {
	      glBegin(GL_LINES);
	        glVertex2d (133, 282); glVertex2d (133, 222); // Door open
	      glEnd();
	    } else {
	      if (leftMlgDoor < 0.3) {
		glPushMatrix();
		  glTranslated (133, 288, 0);
		  glRotated (-80, 0, 0, 1);
		  glBegin(GL_LINES);
		    glVertex2d (0, -6); glVertex2d (0, -70); // Door in transit
		  glEnd();
		glPopMatrix();
	      } else {
		glPushMatrix();
		  glTranslated (133, 288, 0);
		  glRotated (-45, 0, 0, 1);
	  	  glBegin(GL_LINES);
		    glVertex2d (0, -6); glVertex2d (0, -65); // Door in transit
		  glEnd();
		glPopMatrix();
	      }
	    }
	  }
//rightMlgDoor = noseGearDoor;
//sprintf(buffer, "%2.1f", rightMlgDoor);
//m_pFontManager->Print (352, 295, buffer, m_Font);
	  if (rightMlgDoor < 0.1) {
	  glColor3ub (COLOR_GREEN);
	    glBegin(GL_LINES);
	      glVertex2d (332, 288); glVertex2d (403, 288); // Door closed
	    glEnd();
	  } else {
	    glColor3ub (COLOR_AMBER);
	    if (rightMlgDoor == 1.0) {
	      glBegin(GL_LINES);
	        glVertex2d (326, 282); glVertex2d (326, 222); // Door open
	      glEnd();
	    } else {
	      if (rightMlgDoor < 0.3) {
		glPushMatrix();
		  glTranslated (326, 288, 0);
		  glRotated (80, 0, 0, 1);
		  glBegin(GL_LINES);
		    glVertex2d (0, -6); glVertex2d (0, -70); // Door in transit
		  glEnd();
		glPopMatrix();
	      } else {
		glPushMatrix();
		  glTranslated (326, 288, 0);
		  glRotated (45, 0, 0, 1);
	  	  glBegin(GL_LINES);
		    glVertex2d (0, -6); glVertex2d (0, -65); // Door in transit
		  glEnd();
		glPopMatrix();
	      }
	    }
	  }
	}

	// The Gear Deployment Indicators

	glLineWidth (3);
//noseGearDeploy = 0.0;
	if (noseGearDeploy > 0.0) {
	  if (noseGearDeploy > 0.95) glColor3ub (COLOR_GREEN);
	  else glColor3ub (COLOR_AMBER);
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (203, 367); glVertex2d (225, 367); glVertex2d (225, 343); glVertex2d (203, 367);
	  glEnd ();
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (253, 367); glVertex2d (231, 367); glVertex2d (231, 343); glVertex2d (253, 367);
	  glEnd ();
	}
//leftMainGearDeploy = noseGearDeploy;
	if (leftMainGearDeploy > 0.0) {
	  if (leftMainGearDeploy > 0.95) glColor3ub (COLOR_GREEN);
	  else glColor3ub (COLOR_AMBER);
	  glBegin (GL_LINE_STRIP);
	    glVertex2d ( 65, 282); glVertex2d ( 89, 282); glVertex2d ( 89, 256); glVertex2d ( 65, 282);
	  glEnd ();
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (118, 282); glVertex2d ( 94, 282); glVertex2d ( 94, 256); glVertex2d (118, 282);
	  glEnd ();
	}
//noseGearDoorrightMainGearDeploy = noseGearDeploy;
	if (rightMainGearDeploy > 0.0) {
	  if (rightMainGearDeploy > 0.95) glColor3ub (COLOR_GREEN);
	  else glColor3ub (COLOR_AMBER);
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (341, 282); glVertex2d (365, 282); glVertex2d (365, 256); glVertex2d (341, 282);
	  glEnd ();
	  glBegin (GL_LINE_STRIP);
	    glVertex2d (394, 282); glVertex2d (370, 282); glVertex2d (370, 256); glVertex2d (394, 282);
	  glEnd ();
	}

	// The Brake Indicators

//brkRelL = 1;
	if (brkRelL == 1) {
	  glColor3ub (COLOR_GREEN);
	  glLineWidth (3);
	  // Left Brakes
	  glBegin(GL_LINES);
	    glVertex2d ( 25, 180); glVertex2d ( 25, 155);
	    glVertex2d ( 30, 180); glVertex2d ( 30, 155);
	    glVertex2d ( 35, 180); glVertex2d ( 35, 155);
	    glVertex2d ( 55, 180); glVertex2d ( 55, 155);
	    glVertex2d ( 60, 180); glVertex2d ( 60, 155);
	    glVertex2d ( 65, 180); glVertex2d ( 65, 155);
	    glVertex2d (117, 180); glVertex2d (117, 155);
	    glVertex2d (122, 180); glVertex2d (122, 155);
	    glVertex2d (127, 180); glVertex2d (127, 155);
	    glVertex2d (147, 180); glVertex2d (147, 155);
	    glVertex2d (152, 180); glVertex2d (152, 155);
	    glVertex2d (157, 180); glVertex2d (157, 155);
	  glEnd();
	}
//brkRelR = 1;
	if (brkRelR == 1) {
	  glColor3ub (COLOR_GREEN);
	  glLineWidth (3);
	  // Right Brakes
	  glBegin(GL_LINES);
	    glVertex2d (300, 180); glVertex2d (300, 155);
	    glVertex2d (305, 180); glVertex2d (305, 155);
	    glVertex2d (310, 180); glVertex2d (310, 155);
	    glVertex2d (330, 180); glVertex2d (330, 155);
	    glVertex2d (335, 180); glVertex2d (335, 155);
	    glVertex2d (340, 180); glVertex2d (340, 155);
	    glVertex2d (392, 180); glVertex2d (392, 155);
	    glVertex2d (397, 180); glVertex2d (397, 155);
	    glVertex2d (402, 180); glVertex2d (402, 155);
	    glVertex2d (422, 180); glVertex2d (422, 155);
	    glVertex2d (427, 180); glVertex2d (427, 155);
	    glVertex2d (432, 180); glVertex2d (432, 155);
	  glEnd();
	}

	// The abnormal situations

//sim/operation/failures/rel_gear_act	int		(image SD_HydSys_Backgnd_Square)	1 pos
//AirbusFBW/AltnBrake			int	
//AirbusFBW/NWSAvail			int	

//nwsAvail = 0;
//hydSysBkgnd = 0; <== Does not seem to do anything here...
//altnBrake = 0;
//glColor3ub (COLOR_AMBER);
//sprintf(buffer, "%i", nwsAvail);
//m_pFontManager->Print (166, 259, buffer, m_Font);
//sprintf(buffer, "%i", hydSysBkgnd);
//m_pFontManager->Print (172, 216, buffer, m_Font);
//sprintf(buffer, "%i", altnBrake);
//m_pFontManager->Print (182, 150, buffer, m_Font);

	if (nwsAvail == 0) {
	glColor3ub (COLOR_GRAY50);
	glBegin (GL_POLYGON);
	  glVertex2d (164, 297); glVertex2d (178, 297); glVertex2d (178, 277); glVertex2d (164, 277);
	glEnd ();
	glColor3ub (COLOR_AMBER);
	m_pFontManager->Print (166, 281, "Y", m_Font);
	m_pFontManager->Print (186, 281, "N/W STEERING", m_Font);
	if (altnBrake == 0) {
	  glColor3ub (COLOR_GREEN);
	  m_pFontManager->Print (202, 194, "NORM BRK", m_Font);
	} else {
	  glColor3ub (COLOR_GRAY50);
	  glBegin (GL_POLYGON);
	    glVertex2d (180, 210); glVertex2d (194, 210); glVertex2d (194, 190); glVertex2d (180, 190);
	  glEnd ();
	  glBegin (GL_POLYGON);
	    glVertex2d (180, 188); glVertex2d (194, 188); glVertex2d (194, 168); glVertex2d (180, 168);
	  glEnd ();
	  glColor3ub (COLOR_AMBER);
	  m_pFontManager->Print (166, 281, "Y", m_Font);
	  m_pFontManager->Print (186, 281, "N/W STEERING", m_Font);
	  m_pFontManager->Print (190, 216, "ANTI SKID", m_Font);
	  m_pFontManager->Print (182, 194, "G", m_Font);
	  m_pFontManager->Print (202, 194, "NORM BRK", m_Font);
	  m_pFontManager->Print (182, 172, "Y", m_Font);
	  glColor3ub (COLOR_GREEN);
	  m_pFontManager->Print (202, 172, "ALTN BRK", m_Font);
	  m_pFontManager->Print (214, 150, "ACCU ONLY", m_Font);
 	  }
	} else if (altnBrake == 1) {
	  glColor3ub (COLOR_GRAY50);
	  glBegin (GL_POLYGON);
	    glVertex2d (180, 210); glVertex2d (194, 210); glVertex2d (194, 190); glVertex2d (180, 190);
	  glEnd ();
	  glBegin (GL_POLYGON);
	    glVertex2d (180, 188); glVertex2d (194, 188); glVertex2d (194, 168); glVertex2d (180, 168);
	  glEnd ();
	  glColor3ub (COLOR_AMBER);
	  m_pFontManager->Print (190, 216, "ANTI SKID", m_Font);
	  m_pFontManager->Print (182, 194, "G", m_Font);
	  m_pFontManager->Print (202, 194, "NORM BRK", m_Font);
	  glColor3ub (COLOR_GREEN);
	  m_pFontManager->Print (182, 172, "Y", m_Font);
	  m_pFontManager->Print (202, 172, "ALTN BRK", m_Font);
	  m_pFontManager->Print (214, 150, "ACCU PR", m_Font);
	}

/* end of inlined code */


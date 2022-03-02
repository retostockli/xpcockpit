/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Copyright (C) 2002-2021 by:
    Original author:
      Michael DeFeyter
    Contributors (in alphabetical order):
	  Damion Shelton
	  Reto Stockli
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================*/

#include <math.h>
#include "ogcCircleEvaluator.h"
#include "B737/B737PFDSA/ogcB737PFDSA.h"
#include "B737/B737PFDSA/ogcB737PFDSANav.h"

namespace OpenGC
{

B737PFDSANav::B737PFDSANav()
{
	printf("B737PFDSANav constructed\n");

	m_Font = m_pFontManager->LoadDefaultFont();

	m_PhysicalPosition.x = 0;
	m_PhysicalPosition.y = 0;

	m_PhysicalSize.x = 90;
	m_PhysicalSize.y = 90;

	m_Scale.x = 1.0;
	m_Scale.y = 1.0;

	bottomHide = 0.0F;
}

B737PFDSANav::~B737PFDSANav()
{

}

//////////////////////////////////////////////////////////////////////

void B737PFDSANav::Render()
{
	GaugeComponent::Render();

	/*
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	
	float value = m_pDataSource->GetAirframe()->GetMag_Heading();
	float value2 = m_pDataSource->GetAirframe()->GetNav1_OBS();
	float value3 = m_pDataSource->GetAirframe()->GetNav1_Localizer_Needle();

	int i;
	char buf[10];
	CircleEvaluator aCircle;
	GLUquadric *qobj;

	// First, store the "root" position of the gauge component
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	// gray background
	float R = 44.5;
	glColor3ub(COLOR_GRAY40);

	aCircle.SetOrigin(45, 45);
	aCircle.SetRadius(R);
	aCircle.SetDegreesPerPoint(5);

	aCircle.SetArcStartEnd(270.0,360.0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0,90);
	glVertex2f(0,45);
	aCircle.Evaluate();
	glVertex2f(45,90);
	glEnd();

	aCircle.SetArcStartEnd(0.0,90.0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(90,90);
	glVertex2f(45,90);
	aCircle.Evaluate();
	glVertex2f(90,45);
	glEnd();

	aCircle.SetArcStartEnd(90.0,180.0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(90,0);
	glVertex2f(90,45);
	aCircle.Evaluate();
	glVertex2f(45,0);
	glEnd();

	aCircle.SetArcStartEnd(180.0,270.0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0,0);
	glVertex2f(45,0);
	aCircle.Evaluate();
	glVertex2f(0,45);
	glEnd();


	// cut out corners
	glColor3ub(COLOR_BLACK);
	glBegin(GL_POLYGON);
	glVertex2f(0,84);
	glVertex2f(0,90);
	glVertex2f(6,90);
	glVertex2f(0,84);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(84,90);
	glVertex2f(90,90);
	glVertex2f(90,84);
	glVertex2f(84,90);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(90,6);
	glVertex2f(90,0);
	glVertex2f(84,0);
	glVertex2f(90,6);
	glEnd();
	
	glBegin(GL_POLYGON);
	glVertex2f(6,0);
	glVertex2f(0,0);
	glVertex2f(0,6);
	glVertex2f(6,0);
	glEnd();


	//
	glTranslatef(45, 45, 0);

	// center "plane"
	glLineWidth(2.0);
	glColor3ub(COLOR_WHITE);
	glBegin(GL_LINE_STRIP);
	glVertex2f(-2.6, 9.5);
	glVertex2f(-2.6, -9.5);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2f(2.6, 9.5);
	glVertex2f(2.6, -9.5);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2f(2.6, 0);
	glVertex2f(10, 0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2f(-2.6, 0);
	glVertex2f(-10, 0);
	glEnd();


	glPushMatrix();
	glRotated(value, 0, 0, 1);
	m_pFontManager->SetSize(m_Font, 5, 7);

	// draw marker lines
	float linelength, xcircle, ycircle, radians;
	for (i = 0; i > -360; i -= 5) {
		if (i % 10 == 0) 
			linelength = 7;
		else
			linelength = 3.5;

		radians = i * atan2(0.0, -1.0) / 180;
		glLineWidth(2.0);
		glColor3ub(COLOR_WHITE);
		glBegin(GL_LINE_STRIP);
		xcircle = (R) * sin(radians);
		ycircle = (R) * cos(radians);
		glVertex2f(xcircle, ycircle);
		xcircle = (R - linelength) * sin(radians);
		ycircle = (R - linelength) * cos(radians);
		glVertex2f(xcircle, ycircle);
		glEnd();

		if (i % 30 == 0) {
			glPushMatrix();
			snprintf(buf, sizeof(buf), "%d", -i/10);
			glRotated(i, 0, 0, 1);
			if (i < -90)
				m_pFontManager->Print(-3.6, 29.6, buf, m_Font);
			else
				m_pFontManager->Print(-1.7, 29.6, buf, m_Font);
			glPopMatrix();

		}
	}

	// draw arrow
	glPushMatrix();
	glRotated(-value2, 0, 0, 1);

	glLineWidth( 2.0 );
	glColor3ub(COLOR_WHITE);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 40);
	glVertex2f(1.5, 38.5);
	glVertex2f(1.5, 26.5);
	glVertex2f(5, 26.5);
	glVertex2f(5, 23.5);
	glVertex2f(1.5, 23.5);
	glVertex2f(1.5, 19.5);
	glVertex2f(-1.5, 19.5);
	glVertex2f(-1.5, 23.5);
	glVertex2f(-5, 23.5);
	glVertex2f(-5, 26.5);
	glVertex2f(-1.5, 26.5);
	glVertex2f(-1.5, 38.5);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex2f(-1.5, -19.5);
	glVertex2f(-1.5, -38.5);
	glVertex2f(0, -40);
	glVertex2f(1.5, -38.5);
	glVertex2f(1.5, -19.5);
	glEnd();


	glLineWidth(2.0);
	glColor3ub(COLOR_WHITE);

	glTranslatef(-20, 0, 0);
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, 1, 2, 50, 1, 0, 360);
  gluDeleteQuadric(qobj);

	glTranslatef(10, 0, 0);
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, 1, 2, 50, 1, 0, 360);
  gluDeleteQuadric(qobj);

	glTranslatef(20, 0, 0);
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, 1, 2, 50, 1, 0, 360);
  gluDeleteQuadric(qobj);

	glTranslatef(10, 0, 0);
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, 1, 2, 50, 1, 0, 360);
  gluDeleteQuadric(qobj);

	glTranslatef(-20, 0, 0);

	// localizer needle
	if (m_pDataSource->GetAirframe()->GetNav1_Valid()) {
		float tx = value3 * 30;
		glLineWidth(2.5);
		glColor3ub(COLOR_VIOLET);
		glBegin(GL_LINE_LOOP);
		glVertex2f(-1.5 + tx, -18.2);
		glVertex2f(-1.5 + tx, 18.2);
		glVertex2f(1.5 + tx, 18.2);
		glVertex2f(1.5 + tx, -18.2);
		glEnd();
	}


	glPopMatrix();

	glPopMatrix();


	// cut bottom
	glTranslatef(-45, -45, 0);
	glColor3ub(COLOR_BLACK);
	glBegin(GL_POLYGON);
	glVertex2f(0,0);
	glVertex2f(0,bottomHide);
	glVertex2f(90,bottomHide);
	glVertex2f(90,0);
	glEnd();

	
	glPopMatrix();

	*/

}

}

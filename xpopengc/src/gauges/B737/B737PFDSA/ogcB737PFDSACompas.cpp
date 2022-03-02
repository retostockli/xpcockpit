/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Copyright (C) 2002-2021 by:
    Original author:
      Michael DeFeyter
    Contributors (in alphabetical order):
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

#include "B737/B737PFDSA/ogcB737PFDSA.h"
#include "B737/B737PFDSA/ogcB737PFDSACompas.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace OpenGC
{

B737PFDSACompas::B737PFDSACompas()
{
  printf("B737PFDSACompas constructed\n");

  m_Font = m_pFontManager->LoadDefaultFont();

  m_PhysicalPosition.x = 0;
  m_PhysicalPosition.y = 0;

  m_PhysicalSize.x = 80;
  m_PhysicalSize.y = 80;

  m_Scale.x = 1.0;
  m_Scale.y = 1.0;
}

B737PFDSACompas::~B737PFDSACompas()
{

}

//////////////////////////////////////////////////////////////////////

void B737PFDSACompas::Render()
{
	// Call base class to setup viewport and projection
	GaugeComponent::Render();

	/*
	
	float R = 36;
	float value = m_pDataSource->GetAirframe()->GetMag_Heading();
	
	bool adfValid = m_pDataSource->GetAirframe()->GetAdf1_Valid();
	float adfValue = m_pDataSource->GetAirframe()->GetAdf1_Heading();
	
	bool nav1Valid = (m_pDataSource->GetAirframe()->GetNav1_Valid() != 0 ? true : false);
	float nav1DirValue = m_pDataSource->GetAirframe()->GetNav1_Bearing();
	//	bool nav1HasDME = m_pDataSource->GetAirframe()->GetNav1_Has_DME();
	//	float nav1DMEValue = m_pDataSource->GetAirframe()->GetNav1_DME();

	bool nav2Valid = (m_pDataSource->GetAirframe()->GetNav2_Valid() != 0 ? true : false);
	float nav2DirValue = m_pDataSource->GetAirframe()->GetNav2_Bearing();
	//	bool nav2HasDME = m_pDataSource->GetAirframe()->GetNav2_Has_DME();
	//	float nav2DMEValue = m_pDataSource->GetAirframe()->GetNav2_DME();

	int i;
	char buf[10];
	GLUquadric *qobj;

	// First, store the "root" position of the gauge component
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(40, 40, 0); // move to center
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

	// outer circle markers on -90, -45, 0, 45, 90
	for (i = -90; i <= 90; i += 45) {
		glPushMatrix();
		glRotated(i, 0, 0, 1);
		if (i != 0) {
			glLineWidth(2.0);
			glColor3ub(COLOR_WHITE);
			glBegin(GL_LINE_STRIP);
			glVertex2f(0, R);
			glVertex2f(0, R+3);
			glEnd();
		}
		else {
			glLineWidth(2.0);
			glColor3ub(COLOR_WHITE);
			glBegin(GL_POLYGON);
			glVertex2f(0, R);
			glVertex2f(2, R+3);
			glVertex2f(-2, R+3);
			glEnd();
		}
		glPopMatrix();
	}

	// circle
	glPushMatrix();

	glRotated(value, 0, 0, 1);

	glColor3ub(COLOR_WHITE);
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, R-1, R, 50, 1, 0, 360);
  gluDeleteQuadric(qobj);

	// draw marker lines & text
	m_pFontManager->SetSize(m_Font, 4, 5);

	float linelength;
	for (i = 0; i > -360; i -= 5) {
		if (i % 10 == 0) 
			linelength = 6;
		else
			linelength = 3;

		glPushMatrix();

		glRotated(i, 0, 0, 1);
		glLineWidth(2.0);
		glColor3ub(COLOR_WHITE);
		glBegin(GL_LINE_STRIP);
		glVertex2f(0, R);
		glVertex2f(0, R-linelength);
		glEnd();

		if (i % 30 == 0) {
			snprintf(buf, sizeof(buf), "%d", -i/10);
			if (i < -90)
				m_pFontManager->Print(-3, R-13, buf, m_Font);
			else
				m_pFontManager->Print(-1.5, R-13, buf, m_Font);
		}

		glPopMatrix();
	}



	glPopMatrix();

	// draw VOR/ADF arrows
	if (nav1Valid) {
		glPushMatrix();
		glRotated(-nav1DirValue, 0, 0, 1);
		glLineWidth(2.0);
		glColor3ub(COLOR_GREEN);
		glBegin(GL_POLYGON);
		glVertex2f(0, R-5);
		glVertex2f(-4, R-9);
		glVertex2f(4, R-9);
		glEnd();
		bool draw = true;
		glLineWidth(4.0);
		for (i = R-7; i >= -R+9; i -= 2*(R-9)/14) {
			if (draw) {
				glBegin(GL_LINE_STRIP);
				glVertex2f(0, i);
				glVertex2f(0, i-(2*(R-9)/14));
				glEnd();
			}
			draw = !draw;
		}
		glPopMatrix();
	}
	if (nav2Valid) {
		glPushMatrix();
		glRotated(-nav2DirValue, 0, 0, 1);
		glLineWidth(2.0);
		glColor3ub(COLOR_GREEN);
		glBegin(GL_POLYGON);
		glVertex2f(0, R-5);
		glVertex2f(-4, R-9);
		glVertex2f(4, R-9);
		glEnd();
		//glLineWidth(3.0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(-1.5, R-8);
		glVertex2f(1.5, R-8);
		glVertex2f(1.5, -R+9);
		glVertex2f(-1.5, -R+9);
		glEnd();
		glPopMatrix();
	}
	if (adfValid) {
		glPushMatrix();
		glRotated(-adfValue, 0, 0, 1);
		glLineWidth(2.0);
		glColor3ub(COLOR_CYAN);
		glBegin(GL_POLYGON);
		glVertex2f(0, R-5);
		glVertex2f(-4, R-9);
		glVertex2f(4, R-9);
		glEnd();
		bool draw = true;
		glLineWidth(4.0);
		for (i = R-7; i >= -R+9; i -= 2*(R-9)/14) {
			if (draw) {
				glBegin(GL_LINE_STRIP);
				glVertex2f(0, i);
				glVertex2f(0, i-(2*(R-9)/14));
				glEnd();
			}
			draw = !draw;
		}
		glPopMatrix();
	}

	// small circle in the middle of the gauge
	glPushMatrix();
	glLineWidth(2.0);
	glColor3ub(COLOR_WHITE);
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, 1, 2, 50, 1, 0, 360);
  gluDeleteQuadric(qobj);
	glPopMatrix();

	glPopMatrix();

	*/
}

}

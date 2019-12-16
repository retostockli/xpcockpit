/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcBoeing737AnalogFlapComp.cpp,v $

  Copyright (C) 2002 by:
    Original author:
      Jurgen Roeland (Jurgen.Roeland@AISYstems.be)
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2003/04/18 06:00:27 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
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

#include "ogcBoeing737AnalogFlapComp.h"
#include <math.h>

namespace OpenGC
{

Boeing737AnalogFlapComp::Boeing737AnalogFlapComp()
{
	printf("Boeing737AnalogFlapCompconstructed\n");

	m_Font = m_pFontManager->LoadDefaultFont();

	m_PhysicalPosition.x = 0;
	m_PhysicalPosition.y = 0;

	m_PhysicalSize.x = 200;
	m_PhysicalSize.y = 200;

	m_Scale.x = 1.0;
	m_Scale.y = 1.0;

}

Boeing737AnalogFlapComp::~Boeing737AnalogFlapComp()
{

}

void Boeing737AnalogFlapComp::Render()
{
	GaugeComponent::Render();

	float innerR = 65.0F;
	float outerR = 70.0F;
	float innerGaugeCircle = 17;
	float GaugeWidth=16;
	float xcircle, ycircle, radians;
	char buf[10];
	GLUquadric *qobj;
	//	float v = m_pDataSource->GetAirframe()->GetCommanded_Flaps_Deflection()*100;
	float value = m_pDataSource->GetAirframe()->GetFlaps_Deflection()*40.;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLineWidth(2.0);
	glEnable(GL_LINE_SMOOTH);

	glTranslatef(100, 100, 0);

	// outer gauge
	glColor3ub(31, 33, 58);
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, innerR-10, 92, 50, 1, 0, 360);
  gluDeleteQuadric(qobj);

	glColor3ub(98, 96, 145);
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, 92, 100, 50, 1, 0, 360);
  gluDeleteQuadric(qobj);

	m_pFontManager->SetSize(m_Font, 10, 10);

	float Offset=16;
	//Gauge end deepercircle
	glColor3ub(37, 39, 62);
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, innerR-5, innerR-4, 50, 1, 0, 360);
  gluDeleteQuadric(qobj);
	glColor3ub(255, 255, 255);
	//Setting UP
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, innerR, outerR, 50, 1, 270 - (Offset/2.), Offset);
  gluDeleteQuadric(qobj);
	//Setting 1
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, innerR, outerR, 50, 1, 300 - (Offset/2.), Offset);
  gluDeleteQuadric(qobj);
	//Setting 2
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, innerR, outerR, 50, 1, 340 - (Offset/2.), Offset);
  gluDeleteQuadric(qobj);
	//Setting 5
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, innerR, outerR, 50, 1, 20 - (Offset/2.), Offset);
  gluDeleteQuadric(qobj);
	//Setting 10
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, innerR, outerR, 50, 1, 60 - (Offset/2.), Offset);
  gluDeleteQuadric(qobj);
	//Setting 15
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, innerR, outerR, 50, 1, 90 - (Offset/2.), Offset);
  gluDeleteQuadric(qobj);
	//Setting 25
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, innerR, outerR, 50, 1, 120 - (Offset/2.), Offset);
  gluDeleteQuadric(qobj);
	//Setting 30
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, innerR, outerR, 50, 1, 150 - (Offset/2.), Offset);
  gluDeleteQuadric(qobj);
	//Setting 40
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, innerR, outerR, 50, 1, 180 - (Offset/2.), Offset);
  gluDeleteQuadric(qobj);

	glTranslatef(-8, -4, 0);
	//TXT Flaps
	radians = 235 * atan2(0.0, -1.0) / 180;
	xcircle = (outerR + 10) * sin(radians);
	ycircle = (outerR + 10) * cos(radians);
	sprintf(buf, "FLAPS");
	m_pFontManager->Print(xcircle, ycircle, buf, m_Font);
	//Setting UP
	radians = 270 * atan2(0.0, -1.0) / 180;
	xcircle = (outerR + 10) * sin(radians);
	ycircle = (outerR + 10) * cos(radians);
	sprintf(buf, "UP");
	m_pFontManager->Print(xcircle-2, ycircle, buf, m_Font);
	//Setting 1
	radians = 300 * atan2(0.0, -1.0) / 180;
	xcircle = (outerR + 10) * sin(radians);
	ycircle = (outerR + 10) * cos(radians);
	sprintf(buf, "1");
	m_pFontManager->Print(xcircle+3, ycircle, buf, m_Font);
	//Setting 2
	radians = 340 * atan2(0.0, -1.0) / 180;
	xcircle = (outerR + 10) * sin(radians);
	ycircle = (outerR + 10) * cos(radians);
	sprintf(buf, "2");
	m_pFontManager->Print(xcircle+3, ycircle-2, buf, m_Font);
	//Setting 5
	radians = 20 * atan2(0.0, -1.0) / 180;
	xcircle = (outerR + 10) * sin(radians);
	ycircle = (outerR + 10) * cos(radians);
	sprintf(buf, "5");
	m_pFontManager->Print(xcircle, ycircle, buf, m_Font);
	//Setting 10
	radians = 60 * atan2(0.0, -1.0) / 180;
	xcircle = (outerR + 10) * sin(radians);
	ycircle = (outerR + 10) * cos(radians);
	sprintf(buf, "10");
	m_pFontManager->Print(xcircle, ycircle, buf, m_Font);
	//Setting 15
	radians = 90 * atan2(0.0, -1.0) / 180;
	xcircle = (outerR + 10) * sin(radians);
	ycircle = (outerR + 10) * cos(radians);
	sprintf(buf, "15");
	m_pFontManager->Print(xcircle, ycircle, buf, m_Font);
	//Setting 25
	radians = 120 * atan2(0.0, -1.0) / 180;
	xcircle = (outerR + 10) * sin(radians);
	ycircle = (outerR + 10) * cos(radians);
	sprintf(buf, "25");
	m_pFontManager->Print(xcircle, ycircle, buf, m_Font);
	//Setting 30
	radians = 150 * atan2(0.0, -1.0) / 180;
	xcircle = (outerR + 10) * sin(radians);
	ycircle = (outerR + 10) * cos(radians);
	sprintf(buf, "30");
	m_pFontManager->Print(xcircle, ycircle, buf, m_Font);
	//Setting 40
	radians = 180 * atan2(0.0, -1.0) / 180;
	xcircle = (outerR + 10) * sin(radians);
	ycircle = (outerR + 10) * cos(radians);
	sprintf(buf, "40");
	m_pFontManager->Print(xcircle, ycircle, buf, m_Font);
	glTranslatef(8, 4, 0);

//	glColor3ub(31, 33, 58);
	glColor3ub(0, 0, 0);
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, 0, innerGaugeCircle, 50, 1, 0, 360);
  gluDeleteQuadric(qobj);

	glColor3ub(37, 39, 62);
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, innerGaugeCircle, innerGaugeCircle+1, 50, 1, 0, 360);
  gluDeleteQuadric(qobj);

  //  	int numflapsettingsteps=9;
	int numflapsettings[9];
	numflapsettings[0]=270;
	numflapsettings[1]=300;
	numflapsettings[2]=340;
	numflapsettings[3]=20;
	numflapsettings[4]=60;
	numflapsettings[5]=90;
	numflapsettings[6]=120;
	numflapsettings[7]=150;
	numflapsettings[8]=180;
       	int numflapdegrees[9];
	numflapdegrees[0]=0;
	numflapdegrees[1]=1;
	numflapdegrees[2]=2;
	numflapdegrees[3]=5;
	numflapdegrees[4]=10;
	numflapdegrees[5]=15;
	numflapdegrees[6]=25;
	numflapdegrees[7]=30;
	numflapdegrees[8]=40;
	//	int diffflapdegrees=0;
	int previousflappos=0;//8*value/100.;
	if(value >= 40)
		previousflappos=8;
	else if(value >= 30)
		previousflappos=7;
	else if(value >= 25)
		previousflappos=6;
	else if(value >= 15)
		previousflappos=5;
	else if(value >= 10)
		previousflappos=4;
	else if(value >= 5)
		previousflappos=3;
	else if(value >= 2)
		previousflappos=2;
	else if(value >= 1)
		previousflappos=1;
	else if(value >= 0)
		previousflappos=0;

	float onestepdegrees=numflapdegrees[previousflappos+1]-numflapdegrees[previousflappos];
	float remainingpart = value - numflapdegrees[previousflappos];
	int offset=0;
	float degreediff=0.;
	if(previousflappos < 8 && onestepdegrees > 0)
	{
		if(numflapsettings[previousflappos+1] < numflapsettings[previousflappos])
			offset=360;
		float fullstepbetweentwo = numflapsettings[previousflappos+1]+offset-numflapsettings[previousflappos];
		//fullstepbetweentwo = onestepdegree in fs value
		//how much % is remainingpart of onestepdegree
		float perc = remainingpart *100. / (float)onestepdegrees;
		degreediff = fullstepbetweentwo * perc / 100.;
	}
	glRotated(numflapsettings[previousflappos]+degreediff,0,0,-1);
//Gauge
	glColor3ub(255, 255, 255);
	glBegin(GL_POLYGON);
	glVertex2f(GaugeWidth/2 * -1., innerGaugeCircle);
	glVertex2f(GaugeWidth/2, innerGaugeCircle);
	glVertex2f(GaugeWidth/2, innerR-40);
	glVertex2f(0, innerR-5);
	glVertex2f(GaugeWidth/2*-1., innerR-40);
	glVertex2f(GaugeWidth/2 * -1., innerGaugeCircle);
	glEnd();

	
	glBegin(GL_LINE_STRIP);
	glColor3ub(192, 192, 192);
	glVertex2f(0, innerR-5);
	glVertex2f(0, innerR-30);
	glEnd();

	glRotated(0,0,0,1);
	/*sprintf(buf, "TF%.2f",value);
	m_pFontManager->Print(0, 10, buf, m_Font);
	sprintf(buf, "F%.2f",v);
	m_pFontManager->Print(0, -15, buf, m_Font);*/
	glPopMatrix();
}
}

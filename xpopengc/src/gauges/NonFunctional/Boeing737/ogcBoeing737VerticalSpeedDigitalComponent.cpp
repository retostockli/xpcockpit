/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcBoeing737VerticalSpeedDigitalComponent.cpp,v $

  Copyright (C) 2001-2 by:
    Original author:
      Jurgen Roeland (Jurgen.Roeland@AISYstems.be)
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2003/04/18 06:00:36 $
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

#include "ogcBoeing737VerticalSpeedDigitalComponent.h"
#include <math.h>

namespace OpenGC
{

Boeing737VerticalSpeedDigitalComponent::Boeing737VerticalSpeedDigitalComponent()
{
	printf("Boeing737VerticalSpeedDigitalComponent constructed\n");

	m_Font = m_pFontManager->LoadDefaultFont();

	m_PhysicalPosition.x = 0;
	m_PhysicalPosition.y = 0;

	m_PhysicalSize.x = 200;
	m_PhysicalSize.y = 200;

	m_Scale.x = 1.0;
	m_Scale.y = 1.0;

}

Boeing737VerticalSpeedDigitalComponent::~Boeing737VerticalSpeedDigitalComponent()
{

}

void Boeing737VerticalSpeedDigitalComponent::Render()
{
	GaugeComponent::Render();

	float innerR = 80.0F;
	float outerR = 100.0F;
	//	float innerGaugeCircle = 17;
	float GaugeWidth=12;
	float xcircle, ycircle, radians;
	char buf[10];
	GLUquadric *qobj;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLineWidth(3.0);
	glEnable(GL_LINE_SMOOTH);

	glTranslatef(100, 100, 0);

	int numMarkSettings=13;//per .5
	int numMarkSetting[13];
	//same for postive and negative
	numMarkSetting[0]=270;//0
	numMarkSetting[1]=305;//.5
	numMarkSetting[2]=335;//1
	numMarkSetting[3]=0;//1.5
	numMarkSetting[4]=15;//2
	numMarkSetting[5]=27;//2.5
	numMarkSetting[6]=38;//3
	numMarkSetting[7]=46;//3.5
	numMarkSetting[8]=55;//4
	numMarkSetting[9]=62;//4.5
	numMarkSetting[10]=68;//5
	numMarkSetting[11]=74;//5.5
	numMarkSetting[12]=80;//6
	int numMarkNegativeSettings[13];
	for(int p = 0;p < numMarkSettings;p++)
	{
		numMarkNegativeSettings[p] = numMarkSetting[p]*-1. + 180.;
		if(numMarkNegativeSettings[p] >= 360)
			numMarkNegativeSettings[p]-=360;
	}
	// outer gauge
	glColor3ub(255, 255, 255);
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, 98, 100, 50, 1, 100, 340);
  gluDeleteQuadric(qobj);

	m_pFontManager->SetSize(m_Font, 15, 15);

	//	float Offset=16;
	for(int x=0;x<numMarkSettings;x++)
	{
		//Setting stripe positive
		glColor3ub(255, 255, 255);
		radians = numMarkSetting[x] * atan2(0.0, -1.0) / 180;
		if(x % 2 == 0 )
		{
			glLineWidth(3.0);
			xcircle = (outerR) * sin(radians);
			ycircle = (outerR) * cos(radians);
		}
		else
		{
			glLineWidth(1.0);
			xcircle = (outerR+innerR) /2 * sin(radians);
			ycircle = (outerR+innerR) /2 * cos(radians);
		}
		glBegin(GL_LINE_STRIP);
		glVertex2f(xcircle, ycircle);
		xcircle = (innerR) * sin(radians);
		ycircle = (innerR) * cos(radians);
		glVertex2f(xcircle, ycircle);
		glEnd();
		if(x==0||x==2||x==4||x==8)
		{
			glTranslatef(-6, -6, 0);

			radians = numMarkSetting[x] * atan2(0.0, -1.0) / 180;
			xcircle = (innerR - 12) * sin(radians);
			ycircle = (innerR - 12) * cos(radians);
			sprintf(buf, "%d",x/2);
			m_pFontManager->Print(xcircle, ycircle, buf, m_Font);

			glTranslatef(6, 6, 0);
		}
		//Setting stripe positive
		glLineWidth(3.0);
		glColor3ub(255, 255, 255);
		radians = numMarkNegativeSettings[x] * atan2(0.0, -1.0) / 180;
		if(x % 2 == 0 )
		{
			glLineWidth(3.0);
			xcircle = (outerR) * sin(radians);
			ycircle = (outerR) * cos(radians);
		}
		else
		{
			glLineWidth(1.0);
			xcircle = (outerR+innerR) /2 * sin(radians);
			ycircle = (outerR+innerR) /2 * cos(radians);
		}
		glBegin(GL_LINE_STRIP);
		glVertex2f(xcircle, ycircle);
		xcircle = (innerR) * sin(radians);
		ycircle = (innerR) * cos(radians);
		glVertex2f(xcircle, ycircle);
		glEnd();
		if(x==2||x==4||x==8)
		{
			glTranslatef(-6, -6, 0);

			radians = numMarkNegativeSettings[x] * atan2(0.0, -1.0) / 180;
			xcircle = (innerR - 12) * sin(radians);
			ycircle = (innerR - 12) * cos(radians);
			sprintf(buf, "%d",x/2);
			m_pFontManager->Print(xcircle, ycircle, buf, m_Font);

			glTranslatef(6, 6, 0);
		}
		glTranslatef(-6, -6, 0);
		radians = 90 * atan2(0.0, -1.0) / 180;
		xcircle = (innerR +10) * sin(radians);
		ycircle = (innerR +10) * cos(radians);
		sprintf(buf, "6");
		m_pFontManager->Print(xcircle, ycircle, buf, m_Font);
		glTranslatef(6, 6, 0);
		/*glBegin(GL_LINE_STRIP);
		radians = numMarkSetting[x] *-1. * atan2(0.0, -1.0) / 180;
		xcircle = (outerR) * sin(radians);
		ycircle = (outerR) * cos(radians);
		glVertex2f(xcircle, ycircle);
		xcircle = (innerR) * sin(radians);
		ycircle = (innerR) * cos(radians);
		glVertex2f(xcircle, ycircle);
		glEnd();*/
	}
	//set marks 0.1-0.4
	float step;
	glLineWidth(1.0);
	float m;
	for(int z=0;z<=1;z++)
	{
		step = (numMarkSetting[z+1]-numMarkSetting[z])/5.;
		for(m = numMarkSetting[z];m <  numMarkSetting[z+1];m+=step)
		{
			radians = m * atan2(0.0, -1.0) / 180.;
			glBegin(GL_LINE_STRIP);
			xcircle = (innerR+5)  * sin(radians);
			ycircle = (innerR+5)  * cos(radians);
			glVertex2f(xcircle, ycircle);
			xcircle = (innerR) * sin(radians);
			ycircle = (innerR) * cos(radians);
			glVertex2f(xcircle, ycircle);
			glEnd();		
		}
		step = (numMarkNegativeSettings[z]-numMarkNegativeSettings[z+1])/5.;
		for(m = numMarkNegativeSettings[z+1];m <  numMarkNegativeSettings[z];m+=step)
		{
			radians = m * atan2(0.0, -1.0) / 180.;
			glBegin(GL_LINE_STRIP);
			xcircle = (innerR+5)  * sin(radians);
			ycircle = (innerR+5)  * cos(radians);
			glVertex2f(xcircle, ycircle);
			xcircle = (innerR) * sin(radians);
			ycircle = (innerR) * cos(radians);
			glVertex2f(xcircle, ycircle);
			glEnd();		
		}
	}
	//set .5
	m_pFontManager->SetSize(m_Font, 8, 8);
	glTranslatef(-6, -6, 0);
	radians = numMarkNegativeSettings[1] * atan2(0.0, -1.0) / 180;
	xcircle = (innerR - 12) * sin(radians);
	ycircle = (innerR - 12) * cos(radians);
	sprintf(buf, ".5");
	m_pFontManager->Print(xcircle, ycircle, buf, m_Font);
	radians = numMarkSetting[1] * atan2(0.0, -1.0) / 180;
	xcircle = (innerR - 12) * sin(radians);
	ycircle = (innerR - 12) * cos(radians);
	m_pFontManager->Print(xcircle, ycircle, buf, m_Font);
	glTranslatef(6, 6, 0);
	
	int index;
	float remainingpart,degreediff=0;
	float value = m_pDataSource->GetAirframe()->GetVertical_Speed_FPM();
	if(value > 6000)
		value = 6000;
	else if(value < -6000)
		value=-6000;
	index=0;
	if(value >= 0)
	{
		//Feet per minute : 1  = 1000
		index = value / 500.;//array is per .5
		//	float onestepdegrees=100./8.;
		//	int previousflappos=8*value/100.;
		remainingpart = value - (index * 500.);
		//fullstepbetweentwo = onestepdegree in fs value
		//how much % is remainingpart of onestepdegree
		float onestepdegrees = numMarkSetting[index+1]-numMarkSetting[index];
		if(onestepdegrees< 0)
			onestepdegrees+=360;
		float perc = remainingpart * 100. / (float)500;
		degreediff = onestepdegrees * perc / 100.;
	}
	else
	{
		//Feet per minute : 1  = 1000
		float value2=value*-1.;
		index = value2 / 500.;//array is per .5
		//	float onestepdegrees=100./8.;
		//	int previousflappos=8*value/100.;
		remainingpart = value2 - (index * 500.);
		//fullstepbetweentwo = onestepdegree in fs value
		//how much % is remainingpart of onestepdegree
		float onestepdegrees = numMarkNegativeSettings[index]-numMarkNegativeSettings[index+1];
		if(onestepdegrees< 0)
			onestepdegrees+=360;
		float perc = remainingpart * 100. / (float)500;
		degreediff = onestepdegrees * perc / 100.;
	}
	
	//	int offset=0;
	//if(previousflappos < 8 && onestepdegrees > 0)
	//{
	//	if(numMarkSetting[previousflappos+1] < numMarkSetting[previousflappos])
	//		offset=360;
	//	float fullstepbetweentwo = numMarkSetting[previousflappos+1]+offset-numMarkSetting[previousflappos];
	//}
	if(index < numMarkSettings && index >=0)
	{
		if(value>=0)
			glRotated(numMarkSetting[index]+degreediff,0,0,-1);//+degreediff
		else
			glRotated(numMarkNegativeSettings[index]-degreediff,0,0,-1);//+degreediff
	}
	//Gauge
	glColor3ub(255, 255, 255);
	glBegin(GL_POLYGON);
	glVertex2f(GaugeWidth/2 * -1., 0);
	glVertex2f(GaugeWidth/2, 0);
	glVertex2f(GaugeWidth/2, innerR-30);
	glVertex2f(0, (outerR+innerR)/2.);
	glVertex2f(GaugeWidth/2*-1., innerR-30);
	glVertex2f(GaugeWidth/2 * -1., 0);
	glEnd();

	qobj = gluNewQuadric();
	gluPartialDisk(qobj, 0, GaugeWidth/2, 50, 1, 0, 360);
  gluDeleteQuadric(qobj);

	glColor3ub(0, 0, 0);
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, 0, 4, 50, 1, 0, 360);
  gluDeleteQuadric(qobj);

	glPopMatrix();
}

}

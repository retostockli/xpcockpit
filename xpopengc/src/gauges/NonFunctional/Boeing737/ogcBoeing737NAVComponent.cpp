/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcBoeing737NAVComponent.cpp,v $

  Copyright (C) 2001-2 by:
    Original author:
      Jurgen Roeland
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2003/04/18 06:00:32 $
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

#include "ogcBoeing737NAVComponent.h"
#include "navlist.hxx"
#include "fixlist.hxx"
#include "navAirportList.h"
#include "navVORList.h"
#include "navNDBList.h"
#include "navWaypointList.h"
#include "navGeoPointsIndexMap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace OpenGC
{

Boeing737NAVComponent::Boeing737NAVComponent()
{
	printf("Boeing737NAVComponent constructed\n");

	//convertor = new RijksDriehoeksHayfordConverter( GeoPos(0,0), 0.0);
  convertor = new RijksDriehoeksHayfordConverter();

	m_Font = m_pFontManager->LoadDefaultFont();

	m_PhysicalPosition.x = 0;
	m_PhysicalPosition.y = 0;

	m_PhysicalSize.x = 200;
	m_PhysicalSize.y = 230;

	m_Scale.x = 1.0;
	m_Scale.y = 1.0;

	outerR_Miles = 80.;
	outerR_pixels = 100.0F;
	Range_Step_pixels = outerR_pixels/4.0;
	
	//Center plane point position coordinates in opengl (mostly 100,100)
	//This is equal to 0,0 from the convertor (referenceposition)
	ReferencePositionUser.set(outerR_pixels,outerR_pixels);
	scaleNAVxEast=0.;
	scaleNAVyNorth=0.;
	scaleNAVxWest=0.;
	scaleNAVySouth=0.;
}

Boeing737NAVComponent::~Boeing737NAVComponent()
{
	delete convertor;
	convertor = 0;
}

void Boeing737NAVComponent::Render()
{
	GaugeComponent::Render();

	outerR_Miles = m_pDataSource->GetAirframe()->GetNavRange_MaxDistance;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLineWidth(1.0);
	glEnable(GL_LINE_SMOOTH);

	m_pFontManager->SetSize(m_Font, 4, 4);


	//CENTER POINT 
	glTranslatef(100, outerR_pixels, 0);

	DrawFixedParts();

	DrawNAVPoints(m_pDataSource->GetAirframe()->GetMag_Heading());

	//Rotate this according to heading
	DrawRangeCircles();

	DrawNavigationRadioData();

	glPopMatrix();
}

void Boeing737NAVComponent::DrawRangeCircles()
{
	m_pFontManager->SetSize(m_Font, 4, 4);
	glRotatef(m_pDataSource->GetAirframe()->GetMag_Heading(), 0, 0, 1.0F);
	// outer gauge
	GLUquadric *qobj;
	glColor3ub(255, 255, 255);
	qobj = gluNewQuadric();
	gluPartialDisk(qobj, outerR_pixels-0.5, outerR_pixels, 50, 1, 0, 360);
  gluDeleteQuadric(qobj);

	//Draw ranges
	for(float x = Range_Step_pixels;x < outerR_pixels;x+=Range_Step_pixels)
	{
		qobj = gluNewQuadric();
		gluPartialDisk(qobj, x-0.5, x, 50, 1, 0, 360);
    gluDeleteQuadric(qobj);
	}
	int resdeg10;
	//Draw 5/10 Degree marks
	int length;
	for(int deg=0;deg < 360;deg+=5)
	{
		glBegin(GL_LINE_STRIP);
		radians = deg * atan2(0.0, -1.0) / 180;
		xcircle = (outerR_pixels ) * sin(radians);
		ycircle = (outerR_pixels ) * cos(radians);
		glVertex2f(xcircle, ycircle);
		if(deg % 10==0)
			length=7;
		else
			length=3;
		xcircle = (outerR_pixels - length) * sin(radians);
		ycircle = (outerR_pixels - length) * cos(radians);
		glVertex2f(xcircle, ycircle);
		glEnd();
		if(deg % 30==0)
		{
			//Draw text
			//glTranslatef(-6, -6, 0);
			glRotatef(deg*-1., 0, 0, 1.0F);
			resdeg10=deg/10;
			if(resdeg10> 0 && resdeg10 < 10)
				radians = 359 * atan2(0.0, -1.0) / 180.0;
			else
				radians = 357.5 * atan2(0.0, -1.0) / 180.0;
			xcircle = (outerR_pixels - 12) * sin(radians);
			ycircle = (outerR_pixels - 12) * cos(radians);
			if(resdeg10==0)
				sprintf(buf, "36");
			else
				sprintf(buf, "%d",resdeg10);
			m_pFontManager->Print(xcircle, ycircle, buf, m_Font);
			glRotatef(deg, 0, 0, 1.0F);
//			glTranslatef(6, 6, 0);
		}
	}
	glRotatef(m_pDataSource->Mag_Heading*-1., 0, 0, 1.0F);
}

void Boeing737NAVComponent::DrawFixedParts()
{
	glColor3ub(255, 255, 255);
	//Text CurrentZoomMiles
	radians = 0 * atan2(0.0, -1.0) / 180;
	xcircle = (outerR_pixels / 2.) * sin(radians);
	ycircle = (outerR_pixels / 2.) * cos(radians);
	sprintf(buf, "%d",(int)(outerR_Miles/2.));
	m_pFontManager->Print(xcircle-7, ycircle+1, buf, m_Font);
	//center vertical line
	glBegin(GL_LINE_STRIP);
	glVertex2f(0, outerR_pixels);
	glVertex2f(0, 0);
	glEnd();
	//Draw small triangle under heading
	glBegin(GL_LINE_STRIP);
	glVertex2f(-5, outerR_pixels+5);
	glVertex2f(0, outerR_pixels);
	glVertex2f(5,outerR_pixels+5);
	glEnd();
	//BOX around heading
	glBegin(GL_LINE_STRIP);
	glVertex2f(-15, outerR_pixels+12);
	glVertex2f(-15, outerR_pixels+5);
	glVertex2f(15, outerR_pixels+5);
	glVertex2f(15, outerR_pixels+12);
	glEnd();
	//AIRPLANE
	glBegin(GL_LINE_STRIP);
	glVertex2f(-2.5, -7.5);
	glVertex2f(2.5, -7.5);
	glVertex2f(0, 0);
	glVertex2f(-2.5, -7.5);
	glEnd();
}

void Boeing737NAVComponent::DrawNAVPoints(float rotatefsHeading)
{
	m_pFontManager->SetSize(m_Font, 2, 2);

	glRotatef(rotatefsHeading, 0, 0, 1.0F);
	//Center plane point position coordinates in opengl (mostly 100,100)
	//This is equal to 0,0 from the convertor (referenceposition)
	ReferencePositionAirplaneNow.setLatitude(m_pDataSource->Latitude);
	ReferencePositionAirplaneNow.setLongitude(m_pDataSource->Longitude);
	//ReferencePositionAirplaneNow.setLatitude(50,54.76,GeoPos::north);
	//ReferencePositionAirplaneNow.setLongitude(4,30.16,GeoPos::east);
	convertor->setReferencePos(ReferencePositionAirplaneNow,0);
	//ReferencePositionUser.set(outerR_pixels,outerR_pixels);
	GeoPos ReferencePositionAirplaneNow2;
	ReferencePositionAirplaneNow2.setLatitude(52,0,GeoPos::north);
	ReferencePositionAirplaneNow2.setLongitude(4,0,GeoPos::east);
	float NM2 = convertor->getNauticalMilesBetween(ReferencePositionAirplaneNow,ReferencePositionAirplaneNow2);

	//Left under user and lat lon calculation
	CornerLeftUnder = convertor->MoveNMilesFrom(ReferencePositionAirplaneNow,outerR_Miles*-1,outerR_Miles*-1);
	convertor->convertToPos(CornerLeftUnder,CornerLeftUnderUser);
	CornerRightTop = convertor->MoveNMilesFrom(ReferencePositionAirplaneNow,outerR_Miles,outerR_Miles);
	convertor->convertToPos(CornerRightTop,CornerRightTopUser);
	
	//for scale : just multiply scale by userpos afterwards
	//(outerR_pixels * 2) = diff between CornerLeftUnderUser and CornerRightTopUser
	scaleNAVxEast = (float)outerR_pixels / (float)CornerRightTopUser.getX();
	scaleNAVxWest = (float)outerR_pixels / (float)abs(CornerLeftUnderUser.getX());
	scaleNAVyNorth = (float)outerR_pixels / (float)CornerRightTopUser.getY();
	scaleNAVySouth = (float)outerR_pixels / (float)abs(CornerLeftUnderUser.getY());


	//Take the right position from the index map
	long hashkeyrefpoint = pDataContainer->m_pnavGeoPointsIndexMap->getHaskKeyForLocation(ReferencePositionAirplaneNow);

	GeoPos position,positionloop;
	char buf[255];
	int py;
	int px;
	float NM=0.;
	//FIXES
	int longitudeidx;
	for(int latitudeidx = -1;latitudeidx <= 1; latitudeidx++) 
	{
		for(longitudeidx = -1;longitudeidx<= 1; longitudeidx++)
		{
			positionloop = pDataContainer->m_pnavGeoPointsIndexMap->getHashKeyMovedBy(hashkeyrefpoint,latitudeidx,longitudeidx);
			navGeoPointsIndexMap::GeoLoc_list_type* vector_containing_navpoints = pDataContainer->m_pnavGeoPointsIndexMap->getVectorAtGeoPos(positionloop);
			if(vector_containing_navpoints==0)
				continue;

			navGeoPointsIndexMap::GeoLoc_list_iterator currentwp = vector_containing_navpoints->begin();
			navGeoPointsIndexMap::GeoLoc_list_iterator lastwp    = vector_containing_navpoints->end();
			for ( ; currentwp != lastwp ; ++currentwp ) 
			{
				position = (*currentwp)->getGeoGraphicalLocation();
				NM = convertor->getNauticalMilesBetween(ReferencePositionAirplaneNow,position);
				if(NM < outerR_Miles)
				{
					convertor->convertToPos(position, cycleUser);
					if(cycleUser.getY() < 0)
						py= cycleUser.getY()*scaleNAVySouth;
					else
						py= cycleUser.getY()*scaleNAVyNorth;
					if(cycleUser.getX()>0)
						px = cycleUser.getX()*scaleNAVxEast;
					else
						px = cycleUser.getX()*scaleNAVxWest;
					(*currentwp)->DrawObject(px,py, this, m_Font);
				}
			}
		}
	}
	glRotatef(rotatefsHeading*-1, 0, 0, 1.0F);
/*				glBegin(GL_LINE_STRIP);
				glVertex2f(-100, -100);
				glVertex2f(+100, +100);
				glEnd();
				glColor3ub(0, 255, 0);
				glBegin(GL_LINE_STRIP);
				glVertex2f(-95, -95);
				glVertex2f(+95, +95);
				glEnd();*/
}

void Boeing737NAVComponent::DrawNavigationRadioData()
{
	//Rotate according to heading
	glRotatef(m_pDataSource->Mag_Heading, 0, 0, 1.0F);

	//Draw OBS NAV indictaor autopilot
	glLineWidth(2.0);
	float autopilotvalrotaterelativetoheading = m_pDataSource->Autopilot_Course_Heading;
	if(autopilotvalrotaterelativetoheading < 0) autopilotvalrotaterelativetoheading+=360.;
	glRotatef(autopilotvalrotaterelativetoheading*-1, 0, 0, 1.0F);//omdat CCW
	glColor3ub(152, 68, 121);
	glBegin(GL_LINE_STRIP);
	glVertex2f(-5, outerR_pixels);
	glVertex2f(-5, outerR_pixels+4);
	glVertex2f(-1, outerR_pixels);
	glVertex2f(1, outerR_pixels);
	glVertex2f(5, outerR_pixels+4);
	glVertex2f(5, outerR_pixels);
	glVertex2f(0.0, outerR_pixels);
	glVertex2f(-5, outerR_pixels);
	glEnd();
	glLineStipple(3,61680);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINE_STRIP);
	glVertex2f(0, 0);
	glVertex2f(0, outerR_pixels);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	glRotatef(autopilotvalrotaterelativetoheading , 0, 0, 1.0F);//omdat CCW

	//NAV2 indicator arrow
	//Upper arrow
	glRotatef(m_pDataSource->Nav2_Radial*-1, 0, 0, 1.0F);//omdat CCW
	int param_LongMarksOffset=12;
	int param_ShortMarksOffset=6;
	glColor3ub(0, 232, 32);// set NAV OBS COLOR
	glBegin(GL_LINE_STRIP);
		glVertex2f(-5, outerR_pixels-10);
		glVertex2f(5, outerR_pixels-10);
		glVertex2f(5, outerR_pixels-10);
		glVertex2f(3, outerR_pixels-10);
		glVertex2f(3, outerR_pixels-5);
		glVertex2f(0, outerR_pixels);
		glVertex2f(-3, outerR_pixels-5);
		glVertex2f(-3, outerR_pixels-10);
		glVertex2f(-5, outerR_pixels-10);
		glVertex2f(-5, outerR_pixels-10);
	glEnd();
	//Upper arrow stripe through
	glBegin(GL_LINE_STRIP);
		glVertex2f(0.0, outerR_pixels-10);
		glVertex2f(0.0, outerR_pixels-5);
	glEnd();
	//Lower arrow
	glBegin(GL_LINE_STRIP);
		//Start at top inside gauge
		glVertex2f(0.0, (outerR_pixels*-1.)+param_LongMarksOffset+2);
		glVertex2f(-3, (outerR_pixels*-1.)+param_LongMarksOffset);
		glVertex2f(-3, (outerR_pixels*-1.)+param_LongMarksOffset);
		glVertex2f(-7, (outerR_pixels*-1.)+param_LongMarksOffset-2);
		glVertex2f(-7, (outerR_pixels*-1.)+param_LongMarksOffset-8);
		glVertex2f(-0, (outerR_pixels*-1.)+param_LongMarksOffset-5);
		glVertex2f(7, (outerR_pixels*-1.)+param_LongMarksOffset-8);
		glVertex2f(7, (outerR_pixels*-1.)+param_LongMarksOffset-2);
		glVertex2f(3, (outerR_pixels*-1.)+param_LongMarksOffset);
		glVertex2f(3, (outerR_pixels*-1.)+param_LongMarksOffset);
		glVertex2f(0.0, (outerR_pixels*-1.)+param_LongMarksOffset+2);
	glEnd();
	//Vertical stripe
	glBegin(GL_LINE_STRIP);
		glVertex2f(0.0, (outerR_pixels*-1.)+param_LongMarksOffset+2);
		glVertex2f(0.0, (outerR_pixels*-1.)+8);
	glEnd();
	glBegin(GL_LINE_STRIP);
		glVertex2f(-7, (outerR_pixels*-1.)+5);
		glVertex2f(0.0, (outerR_pixels*-1.)+8);
		glVertex2f(+7, (outerR_pixels*-1.)+5);
	glEnd();
	glRotatef(m_pDataSource->Nav2_Radial, 0, 0, 1.0F);//omdat CCW


	//rotate back to 0
	glRotatef(m_pDataSource->Mag_Heading*-1, 0, 0, 1.0F);
	
	//ADF Needle positioning is absolute, has no interaction with current heading
	//Draw ADF Needle
	//if adf unavailable then draw in 90°
	float adfbear=m_pDataSource->Adf1_Heading;
	if(strcmp(m_pDataSource->Adf1_Name,"")==0)
		adfbear=90.;
	glRotatef(adfbear*-1, 0, 0, 1.0F);//omdat CCW
	glColor3ub(0, 248, 248);// magenta
	glBegin(GL_POLYGON);
		glVertex2f(0.0, outerR_pixels-17);
		glVertex2f(5, outerR_pixels-17);
		glVertex2f(0.0, outerR_pixels-12);
		glVertex2f(-5, outerR_pixels-17);
		glVertex2f(0, outerR_pixels-17);
	glEnd();
	//line
	glBegin(GL_LINE_STRIP);
		glVertex2f(0, (outerR_pixels*-1.)+17);
		glVertex2f(0, (outerR_pixels)-17);
	glEnd();
	//back arrow
	glBegin(GL_LINE_STRIP);
		glVertex2f(-5, (outerR_pixels*-1.)+12);
		glVertex2f(0., (outerR_pixels*-1.)+17);
		glVertex2f(5, (outerR_pixels*-1.)+12);
	glEnd();
	glRotatef(adfbear, 0, 0, 1.0F);//omdat CCW


}

}

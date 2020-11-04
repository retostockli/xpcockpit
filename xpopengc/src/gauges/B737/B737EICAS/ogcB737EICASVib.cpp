/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737EICASVib.cpp,v $

  Copyright (C) 2001-2015 by:
  Original author:
  Michael DeFeyter
  Contributors (in alphabetical order):
  Reto Stockli

  Last modification:
  Date:      $Date: 2015/10/12 $
  Version:   $Revision: $
  Author:    $Author: stockli $
  
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
#include "B737/B737EICAS/ogcB737EICAS.h"
#include "B737/B737EICAS/ogcB737EICASVib.h"

namespace OpenGC
{

  B737EICASVib::B737EICASVib()
  {
    printf("B737EICAVib constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 42;
    m_PhysicalSize.y = 34;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    currentEngine = 1;
  }

  B737EICASVib::~B737EICASVib()
  {

  }

  void B737EICASVib::Render()
  {
    GaugeComponent::Render();

    float value;
    char buf[10];
    int acf_type = m_pDataSource->GetAcfType();

    float *engn_vibration1;
    float *engn_vibration2;
    if (acf_type == 1) {
      engn_vibration1 = link_dataref_flt("x737/engine/viblevel_1",-2);
      engn_vibration2 = link_dataref_flt("x737/engine/viblevel_2",-2);
    } else {
      engn_vibration1 = link_dataref_flt("xpserver/viblevel_1",-2);
      engn_vibration2 = link_dataref_flt("xpserver/viblevel_2",-2);
    }
      
    switch (currentEngine)
      {
      case 1: value = *engn_vibration1; break;
      case 2: value = *engn_vibration2; break;
      case 3: value = FLT_MISS; break;
      case 4: value = FLT_MISS; break;
      }
	
    if (value != FLT_MISS) {

      float R = 14.0F;
      float min = 0.F;
      float max = 5.0F;
      float minDegrees = 220.0F;
      float maxDegrees = 110.0F;
      float maxDegreesUse360 = maxDegrees;
      if(maxDegrees < minDegrees)
	maxDegreesUse360 +=360;
      float negativeoffset=0;
      if(min < 0)
	negativeoffset= min*-1.0F;

      float xcircle, ycircle, radians;
      GLUquadric *qobj;

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      glLineWidth(2.0);
      glEnable(GL_LINE_SMOOTH);

      glTranslatef(18, 18, 0);

      // gauge
      glColor3ub(COLOR_GRAYBLUE);
      qobj = gluNewQuadric();
      gluPartialDisk(qobj, 0, R+1, 50, 1, minDegrees, value / max * (maxDegreesUse360-minDegrees));
      gluDeleteQuadric(qobj);

      float percentage = value / (max-min) ;
      float degree = minDegrees + ((maxDegreesUse360 - minDegrees) * percentage);
      glBegin(GL_LINE_STRIP);
      glColor3ub(COLOR_WHITE);
      glVertex2f(0, 0);
      radians = degree * atan2(0.0, -1.0) / 180;
      xcircle = (R) * sin(radians);
      ycircle = (R) * cos(radians);
      glVertex2f(xcircle, ycircle);
      glEnd();
	
      //circle outside
      glColor3ub(COLOR_WHITE);
      qobj = gluNewQuadric();
      gluPartialDisk(qobj, R, R+1, 50, 1, minDegrees, maxDegreesUse360-minDegrees);
      gluDeleteQuadric(qobj);

      // unit markers
      m_pFontManager->SetSize(m_Font, 4, 4);
      float percentagev, degreev;
      for(float xs = min; xs <= max; xs += 1)
	{
	  percentagev = (xs+negativeoffset) / (max-min) ;
	  degreev =  minDegrees+ ((maxDegreesUse360- minDegrees)*percentagev);
	  glBegin(GL_LINE_STRIP);
	  glColor3ub(COLOR_WHITE);
	  radians=degreev * atan2(0.0, -1.0) / 180;
	  xcircle = (R) * sin(radians);
	  ycircle = (R) * cos(radians);
	  glVertex2f(xcircle, ycircle);
	  xcircle = (R - 2) * sin(radians);
	  ycircle = (R - 2) * cos(radians);
	  glVertex2f(xcircle, ycircle);
	  glEnd();
	  xcircle = (R-5) * sin(radians);
	  ycircle = (R-5) * cos(radians);
	  snprintf(buf, sizeof(buf), "%.0f",xs);
	  glTranslatef(-2, -2, 0);
	  m_pFontManager->Print(xcircle , ycircle, buf, m_Font);			
	  glTranslatef(2, 2, 0);
	}

	
      glPopMatrix();

    } else {
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      snprintf(buf, sizeof(buf), "INOP");
      glTranslatef(10, 18, 0);
      glColor3ub(COLOR_ORANGE);
      m_pFontManager->SetSize(m_Font, 4, 4);
      m_pFontManager->Print(0 , 0, buf, m_Font);			
      glTranslatef(-10, -18, 0);
      glPopMatrix();
    }

  }

  void B737EICASVib::setCurrentEngine(int engine)
  {
    currentEngine = engine;
  }

}

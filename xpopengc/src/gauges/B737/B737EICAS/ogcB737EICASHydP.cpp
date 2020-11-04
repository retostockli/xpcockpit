/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737EICASHydP.cpp,v $

  Copyright (C) 2002-2015 by:
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
#include "B737/B737EICAS/ogcB737EICASHydP.h"

namespace OpenGC
{

  B737EICASHydP::B737EICASHydP()
  {
    printf("B737EICASHydP constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 36;
    m_PhysicalSize.y = 36;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    currentEngine = 1;
  }

  B737EICASHydP::~B737EICASHydP()
  {

  }

  void B737EICASHydP::Render()
  {
    GaugeComponent::Render();

    float value;

    int acf_type = m_pDataSource->GetAcfType();

    float *engn_hydp1;
    float *engn_hydp2;
    if (acf_type == 1) {
      engn_hydp1 = link_dataref_flt("x737/systems/hydraulics/systemAHydPress",2);
      engn_hydp2 = link_dataref_flt("x737/systems/hydraulics/systemBHydPress",2);
    } else {
      engn_hydp1 = link_dataref_flt("sim/cockpit2/hydraulics/indicators/hydraulic_pressure_1",2);
      engn_hydp2 = link_dataref_flt("sim/cockpit2/hydraulics/indicators/hydraulic_pressure_2",2);
    }
    
    switch (currentEngine)
      {
      case 1: value = *engn_hydp1; break;
      case 2: value = *engn_hydp2; break;
      case 3: value = FLT_MISS; break;
      case 4: value = FLT_MISS; break;
      }

    if (value != FLT_MISS) {

      value /= 1000.0;

      float R = 14.0F;
      float minDegrees = 140.0F;
      float maxDegrees = 40.0F;

      char buf[10];

      int degreeV[5];
      degreeV[0] = 140; // 0
      degreeV[1] = 184; // 1
      degreeV[2] = 228; // 2
      degreeV[3] = 314; // 3
      degreeV[4] = 400; // 4

      float maxDegreesUse360 = maxDegrees;
      if(maxDegrees < minDegrees)
	maxDegreesUse360 +=360;

      float xcircle, ycircle, radians;
      GLUquadric *qobj;

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      glLineWidth(2.0);
      glEnable(GL_LINE_SMOOTH);

      glTranslatef(18, 18, 0);

      // gauge
      //float percentage = (value + negativeoffset) / (max - min);
      //float degree = minDegrees + ((maxDegreesUse360 - minDegrees) * percentage);
      int x = (int) min(max(value,0.f),4.f);
      float degreeT = (degreeV[x+1] - degreeV[x]) * (value - x);
      float degree = degreeV[x] + degreeT;
      glBegin(GL_LINE_STRIP);
      glColor3ub(COLOR_WHITE);
      glVertex2f(0, 0);
      radians = degree * atan2(0.0, -1.0) / 180;
      xcircle = (R) * sin(radians);
      ycircle = (R) * cos(radians);
      glVertex2f(xcircle, ycircle);
      glEnd();

      // white partial circle 
      glColor3ub(COLOR_WHITE);

      qobj = gluNewQuadric();
      gluPartialDisk(qobj, R, R+1, 50, 1, minDegrees, maxDegreesUse360-minDegrees);
      gluDeleteQuadric(qobj);

      // red partial circles + lines
      int redB1 = 140;
      int redE1 = 228;
      int redB2 = 0;
      int redE2 = 40;
      glColor3ub(COLOR_RED);
	
      qobj = gluNewQuadric();
      gluPartialDisk(qobj, R, R+1, 50, 1, redB1, redE1-redB1);
      gluDeleteQuadric(qobj);
	
      glBegin(GL_LINE_STRIP);
      radians = redE1 * atan2(0.0, -1.0) / 180;
      xcircle = (R) * sin(radians);
      ycircle = (R) * cos(radians);
      glVertex2f(xcircle, ycircle);
      xcircle = (R + 3) * sin(radians);
      ycircle = (R + 3) * cos(radians);
      glVertex2f(xcircle, ycircle);
      glEnd();
	
      qobj = gluNewQuadric();
      gluPartialDisk(qobj, R, R+1, 50, 1, redB2, redE2-redB2);
      gluDeleteQuadric(qobj);
  
      glBegin(GL_LINE_STRIP);
      radians = redB2 * atan2(0.0, -1.0) / 180;
      xcircle = (R) * sin(radians);
      ycircle = (R) * cos(radians);
      glVertex2f(xcircle, ycircle);
      xcircle = (R + 3) * sin(radians);
      ycircle = (R + 3) * cos(radians);
      glVertex2f(xcircle, ycircle);
      glEnd();

      // yellow partial circles + lines
      int yellowB1 = 228;
      int yellowE1 = 260;
      int yellowB2 = 330;
      int yellowE2 = 360;
      glColor3ub(COLOR_YELLOW);
	
      qobj = gluNewQuadric();
      gluPartialDisk(qobj, R, R+1, 50, 1, yellowB1, yellowE1-yellowB1);
      gluDeleteQuadric(qobj);

      glBegin(GL_LINE_STRIP);
      radians = yellowE1 * atan2(0.0, -1.0) / 180;
      xcircle = (R) * sin(radians);
      ycircle = (R) * cos(radians);
      glVertex2f(xcircle, ycircle);
      xcircle = (R + 3) * sin(radians);
      ycircle = (R + 3) * cos(radians);
      glVertex2f(xcircle, ycircle);
      glEnd();
	
      qobj = gluNewQuadric();
      gluPartialDisk(qobj, R, R+1, 50, 1, yellowB2, yellowE2-yellowB2);
      gluDeleteQuadric(qobj);
  
      glBegin(GL_LINE_STRIP);
      radians = yellowB2 * atan2(0.0, -1.0) / 180;
      xcircle = (R) * sin(radians);
      ycircle = (R) * cos(radians);
      glVertex2f(xcircle, ycircle);
      xcircle = (R + 3) * sin(radians);
      ycircle = (R + 3) * cos(radians);
      glVertex2f(xcircle, ycircle);
      glEnd();

      m_pFontManager->SetSize(m_Font, 4, 4);
      // unit markers
      for(int i = 0; i < 5; i++)
	{
	  if (i <= 2 || i == 4)
	    glColor3ub(COLOR_RED);
	  else
	    glColor3ub(COLOR_WHITE);
	  glBegin(GL_LINE_STRIP);
	  radians = degreeV[i] * atan2(0.0, -1.0) / 180;
	  xcircle = (R) * sin(radians);
	  ycircle = (R) * cos(radians);
	  glVertex2f(xcircle, ycircle);
	  xcircle = (R - 2) * sin(radians);
	  ycircle = (R - 2) * cos(radians);
	  glVertex2f(xcircle, ycircle);
	  glEnd();

	  glColor3ub(COLOR_WHITE);
	  xcircle = (R-5) * sin(radians);
	  ycircle = (R-5) * cos(radians);
	  snprintf(buf, sizeof(buf), "%d", i);
	  glTranslatef(-2, -2, 0);
	  m_pFontManager->Print(xcircle , ycircle, buf, m_Font);			
	  glTranslatef(2, 2, 0);
	}


      glPopMatrix();

    }
  }

  void B737EICASHydP::setCurrentEngine(int engine)
  {
    currentEngine = engine;
  }

}

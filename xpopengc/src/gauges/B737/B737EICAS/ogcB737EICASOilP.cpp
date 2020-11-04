/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737EICASOilP.cpp,v $

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
#include "B737/B737EICAS/ogcB737EICASOilP.h"

namespace OpenGC
{

  B737EICASOilP::B737EICASOilP()
  {
    printf("B737EICASOilP constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 36;
    m_PhysicalSize.y = 36;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    currentEngine = 1;
  }

  B737EICASOilP::~B737EICASOilP()
  {

  }

  void B737EICASOilP::Render()
  {
    GaugeComponent::Render();

    float value;

    /* oil pressure comes in psi/100 */
    float *engn_oilpressure = link_dataref_flt_arr("sim/flightmodel/engine/ENGN_oil_press",8,-1,-2);

    switch (currentEngine)
      {
      case 1: value = *(engn_oilpressure+0); break;
      case 2: value = *(engn_oilpressure+1); break;
      case 3: value = *(engn_oilpressure+2); break;
      case 4: value = *(engn_oilpressure+3); break;
      }

    if (value != FLT_MISS) {

      float R = 14.0F;
      float min = 0.0F;
      float max = 100.0F;
      float minDegrees = 120.0F;
      float maxDegrees = 60.0F; //5.0F;
      float yellowstripevalue = 30;
      float redstripevalue = 15;

      float negativeoffset=0;
      if(min < 0)
	negativeoffset= min*-1.0F;

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
      float percentage = (value * 100.0 + negativeoffset) / (max - min) ;
      float degree = minDegrees + ((maxDegreesUse360 - minDegrees) * percentage);
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

      float percentagey = (yellowstripevalue + negativeoffset) / (max - min) ;
      float degreeyellow = minDegrees+ ((maxDegreesUse360 - minDegrees) * percentagey);
      float percentager = (redstripevalue + negativeoffset)  /(max - min) ;
      float degreered =  minDegrees+ ((maxDegreesUse360 - minDegrees) * percentager);

      // yellow partial circle
      glColor3ub(COLOR_YELLOW);

      qobj = gluNewQuadric();
      gluPartialDisk(qobj, R, R+1, 50, 1, degreeyellow, degreered-degreeyellow);
      gluDeleteQuadric(qobj);

      // red partial circle
      glColor3ub(COLOR_RED);
      qobj = gluNewQuadric();
      if(redstripevalue > yellowstripevalue)
	gluPartialDisk(qobj, R, R+1, 50, 1, degreered, maxDegreesUse360-degreered);
      else
	gluPartialDisk(qobj, R, R+1, 50, 1, degreered, minDegrees-degreered);
      gluDeleteQuadric(qobj);

      //yellow stripe
      glBegin(GL_LINE_STRIP);
      glColor3ub(COLOR_YELLOW);
      radians=degreeyellow * atan2(0.0, -1.0) / 180;
      xcircle = (R) * sin(radians);
      ycircle = (R) * cos(radians);
      glVertex2f(xcircle, ycircle);
      xcircle = (R + 4) * sin(radians);
      ycircle = (R + 4) * cos(radians);
      glVertex2f(xcircle, ycircle);
      glEnd();

      //red stripe
      glBegin(GL_LINE_STRIP);
      glColor3ub(COLOR_RED);
      radians=degreered * atan2(0.0, -1.0) / 180;
      xcircle = (R) * sin(radians);
      ycircle = (R) * cos(radians);
      glVertex2f(xcircle, ycircle);
      xcircle = (R + 4) * sin(radians);
      ycircle = (R + 4) * cos(radians);
      glVertex2f(xcircle, ycircle);
      glEnd();

      // unit markers
      float percentagev, degreev;
      for(float xs = min; xs <= max; xs += 50)
	{
	  percentagev = (xs+negativeoffset) / (max-min) ;
	  degreev =  minDegrees+ ((maxDegreesUse360- minDegrees)*percentagev);
	  glBegin(GL_LINE_STRIP);
	  if(redstripevalue > yellowstripevalue)
	    {
	      if(xs >= redstripevalue)
		glColor3ub(COLOR_RED);
	      else if(xs >= yellowstripevalue)
		glColor3ub(COLOR_YELLOW);
	      else
		glColor3ub(COLOR_WHITE);
	    }
	  else
	    {
	      if(xs <= redstripevalue)
		glColor3ub(COLOR_RED);
	      else if(xs <= yellowstripevalue)
		glColor3ub(COLOR_YELLOW);
	      else
		glColor3ub(COLOR_WHITE);
	    }
	  radians=degreev * atan2(0.0, -1.0) / 180;
	  xcircle = (R) * sin(radians);
	  ycircle = (R) * cos(radians);
	  glVertex2f(xcircle, ycircle);
	  xcircle = (R - 2) * sin(radians);
	  ycircle = (R - 2) * cos(radians);
	  glVertex2f(xcircle, ycircle);
	  glEnd();
	}

      // text in unitmarkers
      m_pFontManager->SetSize(m_Font, 4, 4);
      glColor3ub(COLOR_WHITE);

      m_pFontManager->Print(  7, -6, "0", m_Font);
      m_pFontManager->Print(-11, -2, "50", m_Font);
      m_pFontManager->Print(  1,  2, "100", m_Font);
      //

      glPopMatrix();

    }
  }

  void B737EICASOilP::setCurrentEngine(int engine)
  {
    currentEngine = engine;
  }

}

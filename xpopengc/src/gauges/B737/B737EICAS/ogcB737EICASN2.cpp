/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737EICASN2.cpp,v $

  Copyright (C) 2002 by:
  Original author:
  Michael DeFeyter
  Contributors (in alphabetical order):
    Reto Stockli

  Last modification:
  Date:      $Date: 2015/10/06 $
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
#include "B737/B737EICAS/ogcB737EICASN2.h"

namespace OpenGC
{

  B737EICASN2::B737EICASN2()
  {
    printf("B737EICASN2 constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 42;
    m_PhysicalSize.y = 34;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    currentEngine = 1;
  }

  B737EICASN2::~B737EICASN2()
  {

  }

  void B737EICASN2::Render()
  {
    GaugeComponent::Render();
  
    float value;
  
    float *engn_n2 = link_dataref_flt_arr("sim/flightmodel/engine/ENGN_N2_",16,-1,-1);
    switch (currentEngine)
      {
      case 1: value = *(engn_n2+0); break;
      case 2: value = *(engn_n2+1); break;
      case 3: value = *(engn_n2+2); break;
      case 4: value = *(engn_n2+3); break;
      }

    if (value != FLT_MISS) {

      float *n2_red_limit = link_dataref_flt("sim/aircraft/limits/red_hi_N2",-2);

      float R = 16.0F;
      float min = 0.0F;
      float max = 105.0F;
      float minDegrees = 90.0F;
      float maxDegrees = 300.0F;
      float xcircle, ycircle, radians;
      char buf[10];
      GLUquadric *qobj;

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      glLineWidth(2.0);
      glEnable(GL_LINE_SMOOTH);

      glTranslatef(20, 20, 0);

      // gauge
      if (value > *n2_red_limit)
	glColor3ub(COLOR_RED);
      else
	glColor3ub(COLOR_GRAYBLUE);

      qobj = gluNewQuadric();
      gluPartialDisk(qobj, 0, R+1, 50, 1, minDegrees, value / max * (maxDegrees-minDegrees));
      gluDeleteQuadric(qobj);

      float percentage = value / (max-min) ;
      float degree = minDegrees + ((maxDegrees - minDegrees) * percentage);
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
      gluPartialDisk(qobj, R, R+1, 50, 1, minDegrees, maxDegrees-minDegrees);
      gluDeleteQuadric(qobj);

      // yellow stripe
      float percentagey = 100 / (max - min) ;
      float degreeyellow = minDegrees + ((maxDegrees - minDegrees) * percentagey);
      glBegin(GL_LINE_STRIP);
      glColor3ub(COLOR_YELLOW);
      radians = degreeyellow * atan2(0.0, -1.0) / 180.;
      xcircle = (R) * sin(radians);
      ycircle = (R) * cos(radians);
      glVertex2f(xcircle, ycircle);
      xcircle = (R + 4) * sin(radians);
      ycircle = (R + 4) * cos(radians);
      glVertex2f(xcircle, ycircle);
      glEnd();

      // red stripe
      float percentager = *n2_red_limit / (max - min);
      float degreered =  minDegrees + ((maxDegrees - minDegrees) * percentager);
      glBegin(GL_LINE_STRIP);
      glColor3ub(COLOR_RED);
      radians = degreered * atan2(0.0, -1.0) / 180.;
      xcircle = (R) * sin(radians);
      ycircle = (R) * cos(radians);
      glVertex2f(xcircle, ycircle);
      xcircle = (R + 5) * sin(radians);
      ycircle = (R + 5) * cos(radians);
      glVertex2f(xcircle, ycircle);
      glEnd();

      glTranslatef(-20, -20, 0);

      // white rectangle containing the text
      glBegin(GL_LINE_LOOP);
      glColor3ub(COLOR_WHITE);
      glVertex2f( 20, 30 );
      glVertex2f( 42, 30 );
      glVertex2f( 42, 20 );
      glVertex2f( 20, 20 );
      glEnd();

      // text
      m_pFontManager->SetSize(m_Font, 5, 5);
      glColor3ub(COLOR_WHITE);
      snprintf(buf, sizeof(buf), "%.01f", value);
      m_pFontManager->Print( 21.9, 22.7, buf, m_Font);

      glPopMatrix();

    }
  }

  void B737EICASN2::setCurrentEngine(int engine)
  {
    currentEngine = engine;
  }

}

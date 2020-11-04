/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737EICASFF.cpp,v $

  Copyright (C) 2002-2015 by:
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
#include "B737/B737EICAS/ogcB737EICASFF.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace OpenGC
{

  B737EICASFF::B737EICASFF()
  {
    printf("B737EICASFF constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 42;
    m_PhysicalSize.y = 34;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    currentEngine = 1;
  }

  B737EICASFF::~B737EICASFF()
  {

  }

  void B737EICASFF::Render()
  {
    GaugeComponent::Render();

    float value;

    float *engn_ff = link_dataref_flt_arr("sim/flightmodel/engine/ENGN_FF_",8,-1,-2);

    switch (currentEngine)
      {
      case 1: value = *(engn_ff+0); break;
      case 2: value = *(engn_ff+1); break;
      case 3: value = *(engn_ff+2); break;
      case 4: value = *(engn_ff+3); break;
      }

    if (value != FLT_MISS) {

      // unit conversion: kg/s -> 1000 kg/h
      value *= 3.6;

      if (value > 12)
	value = 12;

      float R = 14.0F;
      float min = 0.0F;
      float max = 12.0F;
      float minDegrees = 90.0F;
      float maxDegrees = 320.0F;
      float xcircle, ycircle, radians;
      char buf[10];
      GLUquadric *qobj;

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      glLineWidth(2.0);
      glEnable(GL_LINE_SMOOTH);

      glTranslatef(20, 20, 0);

      // gauge
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

      // unit markers
      m_pFontManager->SetSize(m_Font, 4, 4);
      float percentagev, degreev;
      for(float xs = 0; xs <= max; xs += 2)
	{
	  percentagev = xs / (max - min) ;
	  degreev = minDegrees + ((maxDegrees - minDegrees) * percentagev);
	  glLineWidth(2.0);
	  glColor3ub(COLOR_WHITE);
	  glBegin(GL_LINE_STRIP);
	  radians = degreev * atan2(0.0, -1.0) / 180.;
	  xcircle = (R) * sin(radians);
	  ycircle = (R) * cos(radians);
	  glVertex2f(xcircle, ycircle);
	  xcircle = (R - 2) * sin(radians);
	  ycircle = (R - 2) * cos(radians);
	  glVertex2f(xcircle, ycircle);
	  glEnd();

	  // text in unitmarkers
	  if ((xs > 0) && ((int)(xs) % 4 == 0)) {
	    glTranslatef(-2, -2, 0);
	    snprintf(buf, sizeof(buf), "%.0f", xs);
	    glColor3ub(COLOR_WHITE);
	    xcircle = (R - 5) * sin(radians);
	    ycircle = (R - 5) * cos(radians);
	    m_pFontManager->Print(  xcircle, ycircle, buf, m_Font);
	    glTranslatef(2, 2, 0);
	  }
	}
      m_pFontManager->SetSize(m_Font, 5, 5);

      glTranslatef(-20, -20, 0);

      // white rectangle containing the text
      glLineWidth(2.0);
      glColor3ub(COLOR_WHITE);
      glBegin(GL_LINE_LOOP);
      glVertex2f( 20, 30 );
      glVertex2f( 40, 30 );
      glVertex2f( 40, 20 );
      glVertex2f( 20, 20 );
      glEnd();

      // text
      glColor3ub(COLOR_WHITE);
      snprintf(buf, sizeof(buf), "%.02f", value);
      m_pFontManager->Print( 21.9, 22.7, buf, m_Font);

      glPopMatrix();

    }
  }

  void B737EICASFF::setCurrentEngine(int engine)
  {
    currentEngine = engine;
  }

}

/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737EICASN1.cpp,v $

  Copyright (C) 2002-2015 by:
  Original author:
    Michael DeFeyter
  Contributors (in alphabetical order):
    Reto Stockli

  Last modification:
  Date:      $Date: 2015/09/12 $
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
#include "B737/B737EICAS/ogcB737EICASN1.h"

namespace OpenGC
{


  B737EICASN1::B737EICASN1()
  {
    printf("B737EICASN1 constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 42;
    m_PhysicalSize.y = 40;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    currentEngine = 1;
  }

  B737EICASN1::~B737EICASN1()
  {

  }

  void B737EICASN1::Render()
  {
    GaugeComponent::Render();

    float value;
    int rev;

    float *engn_n1 = link_dataref_flt_arr("sim/flightmodel/engine/ENGN_N1_",8,-1,-1);
    int *reverser = link_dataref_int_arr("sim/cockpit/warnings/annunciators/reverser_on",8,-1);
    switch (currentEngine)
      {
      case 1: 
	value = *(engn_n1+0); 
	rev = *(reverser+0); 
	break;
      case 2: 
	value = *(engn_n1+1); 
	rev = *(reverser+1); 
	break;
      case 3: 
	value = *(engn_n1+2); 
	rev = *(reverser+2); 
	break;
      case 4: 
	value = *(engn_n1+3); 
	rev = *(reverser+3); 
	break;
      }

    if (value != FLT_MISS) {

      int acf_type = m_pDataSource->GetAcfType();

      float *n1_red_limit = link_dataref_flt("sim/aircraft/limits/red_hi_N1",-2);

      float *n1_green_limit;
      if (acf_type == 1) {
	n1_green_limit = link_dataref_flt("x737/systems/eec/N1_limit",-2);
      } else if (acf_type == 3) {
	if (currentEngine == 1) {
	  n1_green_limit = link_dataref_flt("laminar/B738/engine/eng1_N1_bug",-3);
	} else {
	  n1_green_limit = link_dataref_flt("laminar/B738/engine/eng2_N1_bug",-3);
	}
      } else {
	n1_green_limit = link_dataref_flt("sim/aircraft/limits/green_hi_N1",-2);
      }

      float R = 16.0F;
      float min = 0.0F;

      float max_red;
      if (*n1_red_limit != FLT_MISS) {
	max_red = *n1_red_limit;
      } else {
	max_red = 105.0F;
      }

      float max_green;
      if (*n1_green_limit != FLT_MISS) {
	if (acf_type == 3) {
	  max_green = 100.0 * *n1_green_limit;
	} else {
	  max_green = *n1_green_limit;
	}
      } else {
	max_green = 98.0F;
      }

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
      if(value > max_green)
	glColor3ub(COLOR_RED);
      else
	glColor3ub(COLOR_GRAYBLUE);
      qobj = gluNewQuadric();
      gluPartialDisk(qobj, 0, R+1, 50, 1, minDegrees, value / max_red * (maxDegrees-minDegrees));
      gluDeleteQuadric(qobj);

      float percentage = value / (max_red-min) ;
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

      // green stripe
      float percentageg = max_green / (max_red - min) ;
      float degreegreen = minDegrees + ((maxDegrees - minDegrees) * percentageg);
      glColor3ub(COLOR_GREEN);
      radians = degreegreen * atan2(0.0, -1.0) / 180.;
      glBegin(GL_LINE_STRIP);
      xcircle = (R) * sin(radians);
      ycircle = (R) * cos(radians);
      glVertex2f(xcircle, ycircle);
      xcircle = (R + 2) * sin(radians);
      ycircle = (R + 2) * cos(radians);
      glVertex2f(xcircle, ycircle);
      glEnd();
      glBegin(GL_LINE_STRIP);
      xcircle = (R + 2) * sin(radians);
      ycircle = (R + 2) * cos(radians);
      glVertex2f(xcircle, ycircle);
      xcircle = (R + 4) * sin(radians-0.05);
      ycircle = (R + 5) * cos(radians-0.05);
      glVertex2f(xcircle, ycircle);
      glEnd();
      glBegin(GL_LINE_STRIP);
      xcircle = (R + 2) * sin(radians);
      ycircle = (R + 2) * cos(radians);
      glVertex2f(xcircle, ycircle);
      xcircle = (R + 4) * sin(radians+0.05);
      ycircle = (R + 4) * cos(radians+0.05);
      glVertex2f(xcircle, ycircle);
      glEnd();

      // red stripe
      float percentager = max_red / (max_red - min) ;
      float degreered =  minDegrees + ((maxDegrees - minDegrees) * percentager);
      glColor3ub(COLOR_RED);
      glBegin(GL_LINE_STRIP);
      radians = degreered * atan2(0.0, -1.0) / 180.;
      xcircle = (R) * sin(radians);
      ycircle = (R) * cos(radians);
      glVertex2f(xcircle, ycircle);
      xcircle = (R + 5) * sin(radians);
      ycircle = (R + 5) * cos(radians);
      glVertex2f(xcircle, ycircle);
      glEnd();

      // unit markers
      m_pFontManager->SetSize(m_Font, 4, 4);
      float percentagev, degreev;
      for(float xs = 0; xs <= max_red; xs += 10)
	{
	  percentagev = xs / (max_red - min) ;
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
	  if ((xs > 0) && ((int)(xs/10) % 2 == 0)) {
	    glTranslatef(-2, -1, 0);
	    snprintf(buf, sizeof(buf), "%.0f", xs/10);
	    glColor3ub(COLOR_WHITE);
	    xcircle = (R - 4.5) * sin(radians);
	    ycircle = (R - 4.5) * cos(radians);
	    m_pFontManager->Print(xcircle, ycircle, buf, m_Font);
	    glTranslatef(2, 1, 0);
	  }
	}

      glTranslatef(-20, -20, 0);

      // white rectangle containing the text
      glLineWidth(2.0);
      glColor3ub(COLOR_WHITE);
      glBegin(GL_LINE_LOOP);
      glVertex2f( 20, 30 );
      glVertex2f( 42, 30 );
      glVertex2f( 42, 20 );
      glVertex2f( 20, 20 );
      glEnd();

      // n1: text
      m_pFontManager->SetSize(m_Font, 5, 5);
      glColor3ub(COLOR_WHITE);
      snprintf(buf, sizeof(buf), "%.01f", value);
      m_pFontManager->Print( 21.9, 22.7, buf, m_Font);

      // n1-limit or reverser: text
      m_pFontManager->SetSize(m_Font, 5, 5);
      glColor3ub(COLOR_GREEN);
      if (rev == 1) {
	snprintf(buf, sizeof(buf), "REV");
      } else {
	snprintf(buf, sizeof(buf), "%.01f", max_green);
      }
      m_pFontManager->Print( 21.9, 32.0, buf, m_Font);

      glPopMatrix();

    }
  }

  void B737EICASN1::setCurrentEngine(int engine)
  {
    currentEngine = engine;
  }

}

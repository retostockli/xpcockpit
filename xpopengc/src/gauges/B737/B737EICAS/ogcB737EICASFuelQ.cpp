/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737EICASFuelQ.cpp,v $

  Copyright (C) 2001-2015 by:
  Original author:
  Michael De Feyter
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
#include "B737/B737EICAS/ogcB737EICASFuelQ.h"

namespace OpenGC
{

  B737EICASFuelQ::B737EICASFuelQ()
  {
    printf("B737EICAFuelQ constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 42;
    m_PhysicalSize.y = 34;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    currentTank = 1;
  }

  B737EICASFuelQ::~B737EICASFuelQ()
  {

  }

  void B737EICASFuelQ::Render()
  {
    GaugeComponent::Render();

    float value;
    float capacity;

    float *fuel = link_dataref_flt_arr("sim/flightmodel/weight/m_fuel",9,-1,-1);
    float *fuel_ratio = link_dataref_flt_arr("sim/aircraft/overflow/acf_tank_rat",9,-1,-3);

    float *fuel_capacity = link_dataref_flt("sim/aircraft/weight/acf_m_fuel_tot",0);
    //	float *fuel_total = link_dataref_flt("sim/flightmodel/weight/m_fuel_total",0);

    //    printf("%f %f %f \n",*(fuel+2),*(fuel_ratio+2),*fuel_capacity);

    switch (currentTank)
      {
      case 1: /* left tank */
	value = *(fuel+0); 
	capacity = *(fuel_ratio+0) * *fuel_capacity;
	break;
      case 0: /* center tank */
	value = *(fuel+1); 
	capacity = *(fuel_ratio+1) * *fuel_capacity;
	break;
      case 2: /* right tank */
	value = *(fuel+2); 
	capacity = *(fuel_ratio+2) * *fuel_capacity;
	break;
      }
	
    if ((value != FLT_MISS) && (capacity != FLT_MISS)) {
	
      if (capacity <= 0)
	capacity = 1;

      if (value < 0 )
	value = 0;

      // value and capacity are in [kg]

      float R = 14.0F;
      float min = 0.F;
      float max = capacity;
      float minDegrees = 240.0F;
      float maxDegrees = 120.0F;
      float maxDegreesUse360 = maxDegrees;
      if(maxDegrees < minDegrees)
	maxDegreesUse360 +=360;
      float negativeoffset=0;
      if(min < 0)
	negativeoffset= min*-1.0F;

      float xcircle, ycircle, radians;
      char buf[12];
      GLUquadric *qobj;

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      glLineWidth(2.0);
      glEnable(GL_LINE_SMOOTH);

      glTranslatef(18, 14, 0);

      // gauge
      glColor3ub(COLOR_WHITE);
      qobj = gluNewQuadric();
      gluPartialDisk(qobj, R-2, R, 50, 1, minDegrees, value / max * (maxDegreesUse360-minDegrees));
      gluDeleteQuadric(qobj);

      // unit markers
      glColor3ub(COLOR_WHITE);
      float percentagev, degreev;
      float xs = 0;
      for(int i = 0; i < 11; i++)
	{
	  percentagev = (xs+negativeoffset) / (max-min) ;
	  degreev =  minDegrees+ ((maxDegreesUse360- minDegrees)*percentagev);
	  glBegin(GL_LINE_STRIP);
	  radians=degreev * atan2(0.0, -1.0) / 180;
	  xcircle = (R) * sin(radians);
	  ycircle = (R) * cos(radians);
	  glVertex2f(xcircle, ycircle);
	  xcircle = (R - 3) * sin(radians);
	  ycircle = (R - 3) * cos(radians);
	  glVertex2f(xcircle, ycircle);
	  glEnd();
	  xs += max/10;
	}

      glTranslatef(-18, -14, 0);

      // text gauge
      m_pFontManager->SetSize(m_Font, 5, 5);
      glColor3ub(COLOR_WHITE);
      snprintf(buf, sizeof(buf), "%.0f", value);
      if (value  < 100)
	m_pFontManager->Print(14, 11, buf, m_Font);
      else if (value  < 1000)
	m_pFontManager->Print(12, 11, buf, m_Font);
      else if (value < 10000)
	m_pFontManager->Print(10, 11, buf, m_Font);
      else
	m_pFontManager->Print(8, 11, buf, m_Font);
	

      // text tank
      m_pFontManager->SetSize(m_Font, 4, 4);
      glColor3ub(COLOR_LIGHTBLUE);
      if (currentTank > 0) {
	snprintf(buf, sizeof(buf), "%d", currentTank);
	m_pFontManager->Print(16, 20, buf, m_Font);
      }

      else {
	snprintf(buf, sizeof(buf), "CTR");
	m_pFontManager->Print(12, 19, buf, m_Font);
      }

      glPopMatrix();

    }

  }

  void B737EICASFuelQ::setCurrentTank(int tank)
  {
    currentTank = tank;
  }

}

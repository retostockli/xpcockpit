/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737VSI.cpp,v $

  Copyright (C) 2001-2015 by:
  Original author:
  Damion Shelton
  Contributors (in alphabetical order):
  Reto Stockli

  Last modification:
  Date:      $Date: 2015/11/26 $
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

// Microsoft's implementation of OpenGL needs to have certain
// symbols (found in windows.h) defined
#include "ogcGLHeaders.h"
#include <stdio.h>
#include <math.h>

#include "ogcGaugeComponent.h"
#include "ogcB737PFD.h"
#include "ogcB737VSI.h"

namespace OpenGC
{

  B737VSI::B737VSI()
  {
    printf("B737VSI constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 16;
    m_PhysicalSize.y = 108;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    m_NeedleCenter = 54.0;
    m_MaxNeedleDeflection = 42.0;
  }

  B737VSI::~B737VSI()
  {

  }

  void B737VSI::Render()
  {
    // Call base class to setup viewport and projection
    GaugeComponent::Render();
 
    // Save matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
  
    m_pFontManager->SetSize(m_Font,4.0, 4.0);

    // Draw in gray-blue
    glColor3ub(COLOR_GRAYBLUE);

    // Bottom angular part
    glBegin(GL_POLYGON);
    glVertex2f(0.0, 10.0);
    glVertex2f(0.0, 40.0);
    glVertex2f(4.5, 44.0);
    glVertex2f(16.0, 44.0);
    glVertex2f(16.0, 27.0);
    glVertex2f(7.0, 10.0);
    glEnd();

    // Center rectangle
    glBegin(GL_POLYGON);
    glVertex2f(4.5, 44.0);
    glVertex2f(4.5, 66.0);
    glVertex2f(16.0, 66.0);
    glVertex2f(16.0, 44.0);
    glEnd();

    // Top angular part
    glBegin(GL_POLYGON);
    glVertex2f(4.5, 66.0);
    glVertex2f(0.0, 70.0);
    glVertex2f(0.0, 100.0);
    glVertex2f(7.0, 100.0);
    glVertex2f(16.0, 83.0);
    glVertex2f(16.0, 66.0);
    glEnd();
  
    glColor3ub(COLOR_WHITE);
 
    // float *vertical_speed = link_dataref_flt("sim/flightmodel/position/vh_ind_fpm",1);
    float *vertical_speed = link_dataref_flt("sim/cockpit2/gauges/indicators/vvi_fpm_pilot",1);

    float temp = *vertical_speed;
    
    if (*vertical_speed != FLT_MISS) {

      
      char buffer[6];

      // if vertical +- 200 fpm display digital readout
      if ( (*vertical_speed < -199.0) && (*vertical_speed > -9999.0) )
	{
	  snprintf(buffer, sizeof(buffer), "%05d", (int) *vertical_speed);
	  m_pFontManager->Print( -0.5, 3.5 , &buffer[0], m_Font);
	}
      else
	{
	  if ( (*vertical_speed > 199.0) && (*vertical_speed < 9999.0) )
	    {
	      snprintf(buffer, sizeof(buffer), "%04d", (int) *vertical_speed);
	      m_pFontManager->Print( 1.0, 103.5 , &buffer[0], m_Font);
	    }
	}
      
      m_pFontManager->Print( 2.0, VSpeedToNeedle(6000.0),"6-",m_Font);
      m_pFontManager->Print( 2.0, VSpeedToNeedle(2000.0),"2-",m_Font);
      m_pFontManager->Print( 2.0, VSpeedToNeedle(1000.0),"1-",m_Font);
      m_pFontManager->Print( 2.0, VSpeedToNeedle(-1000.0),"1-",m_Font);
      m_pFontManager->Print( 2.0, VSpeedToNeedle(-2000.0),"2-",m_Font);
      m_pFontManager->Print( 2.0, VSpeedToNeedle(-6000.0),"6-",m_Font);

      float pos;
      glLineWidth(1.5);
      pos = 0.5*(VSpeedToNeedle(1000.0)) + 1.75;
      glBegin(GL_LINES);
      glVertex2f(6.0,pos);
      glVertex2f(7.5,pos);
      glEnd();
      pos = 0.5*(VSpeedToNeedle(1000.0) + VSpeedToNeedle(2000.0)) + 1.75;
      glBegin(GL_LINES);
      glVertex2f(6.0,pos);
      glVertex2f(7.5,pos);
      glEnd();
      pos = 0.5*(VSpeedToNeedle(2000.0) + VSpeedToNeedle(6000.0)) + 1.75;
      glBegin(GL_LINES);
      glVertex2f(6.0,pos);
      glVertex2f(7.5,pos);
      glEnd();
      pos = 0.5*(VSpeedToNeedle(-1000.0)) + 1.75;
      glBegin(GL_LINES);
      glVertex2f(6.0,pos);
      glVertex2f(7.5,pos);
      glEnd();
      pos = 0.5*(VSpeedToNeedle(-1000.0) + VSpeedToNeedle(-2000.0)) + 1.75;
      glBegin(GL_LINES);
      glVertex2f(6.0,pos);
      glVertex2f(7.5,pos);
      glEnd();
       pos = 0.5*(VSpeedToNeedle(-2000.0) + VSpeedToNeedle(-6000.0)) + 1.75;
      glBegin(GL_LINES);
      glVertex2f(6.0,pos);
      glVertex2f(7.5,pos);
      glEnd();
       
      glBegin(GL_LINES);
  
      // Horizontal center detent
      glVertex2f(5.0, VSpeedToNeedle(0.0) + 1.75);
      glVertex2f(10.0, VSpeedToNeedle(0.0) + 1.75);

      //      printf("%f \n",temp);

      // Next draw the angled line that indicates climb rate
      // We need to add 1.75 to the calculated needle position because of how
      // GL interprets the line width
      // glVertex2f( 5.0, VSpeedToNeedle(*vertical_speed) + 1.75);
      // FIX for strange behavior below -1000 fpm
      glVertex2f( 5.0, VSpeedToNeedle(temp) + 1.75);
      glVertex2f( 30.0, m_NeedleCenter + 1.75);
      glEnd();

    }

    glPopMatrix();
  }

  float B737VSI::VSpeedToNeedle(float vspd)
  {
    float needle;
  
    if(vspd>=0.0)
      {
	if(vspd>6000.0)
	  vspd = 6000.0;
    
	needle = (1.0-exp(-3.0*vspd/6000.0)) * m_MaxNeedleDeflection + m_NeedleCenter;
      }
    else
      {
	vspd = fabs(vspd);

	if(vspd>6000.0)
	  vspd = 6000.0;
    
	needle = m_NeedleCenter - (1.0-exp(-3.0*vspd/6000.0)) * m_MaxNeedleDeflection;
       }

    //    printf("%f %f\n",vspd,needle);

    return needle;
  }

} // end namespace OpenGC


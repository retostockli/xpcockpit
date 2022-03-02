/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Copyright (C) 2001-2021 by:
  Original author:
  Damion Shelton
  Contributors (in alphabetical order):
  Reto Stockli
  
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
#include "ogcB737PFDVSI.h"

namespace OpenGC
{

  B737PFDVSI::B737PFDVSI()
  {
    printf("B737PFDVSI constructed\n");

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

  B737PFDVSI::~B737PFDVSI()
  {

  }

  void B737PFDVSI::Render()
  {
    // Call base class to setup viewport and projection
    GaugeComponent::Render();

    int acf_type = m_pDataSource->GetAcfType();

    // Save matrix
    glMatrixMode(GL_MODELVIEW);
      
    char buffer[6];
    float bigFontSize = 5.0;
    float fontSize = 4.0;
    float lineWidth = 3.0;

    // float *vertical_speed = link_dataref_flt("sim/flightmodel/position/vh_ind_fpm",1);
    float *vertical_speed = link_dataref_flt("sim/cockpit2/gauges/indicators/vvi_fpm_pilot",1);
    float temp = *vertical_speed;

    int *irs_mode;
    if ((acf_type == 2) || (acf_type == 3)) {
      irs_mode = link_dataref_int("laminar/B738/irs/irs_mode");
    } else {
      irs_mode = link_dataref_int("xpserver/irs_mode");
      *irs_mode = 2;
    }
    
    if ((*vertical_speed != FLT_MISS) && (*irs_mode == 2)) {

      glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

      glPushMatrix();
      
      m_pFontManager->SetSize(m_Font,fontSize, fontSize);
      
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
      
      m_pFontManager->Print( 2.0, VSpeedToNeedle(6000.0)-1.5,"6-",m_Font);
      m_pFontManager->Print( 2.0, VSpeedToNeedle(2000.0)-1.5,"2-",m_Font);
      m_pFontManager->Print( 2.0, VSpeedToNeedle(1000.0)-1.5,"1-",m_Font);
      m_pFontManager->Print( 2.0, VSpeedToNeedle(-1000.0)-1.5,"1-",m_Font);
      m_pFontManager->Print( 2.0, VSpeedToNeedle(-2000.0)-1.5,"2-",m_Font);
      m_pFontManager->Print( 2.0, VSpeedToNeedle(-6000.0)-1.5,"6-",m_Font);

      float pos;
      glLineWidth(lineWidth);
      pos = 0.5*(VSpeedToNeedle(0.0) + VSpeedToNeedle(1000.0));
      glBegin(GL_LINES);
      glVertex2f(6.0,pos);
      glVertex2f(7.5,pos);
      glEnd();
      pos = 0.5*(VSpeedToNeedle(1000.0) + VSpeedToNeedle(2000.0));
      glBegin(GL_LINES);
      glVertex2f(6.0,pos);
      glVertex2f(7.5,pos);
      glEnd();
      pos = 0.5*(VSpeedToNeedle(2000.0) + VSpeedToNeedle(6000.0));
      glBegin(GL_LINES);
      glVertex2f(6.0,pos);
      glVertex2f(7.5,pos);
      glEnd();

      pos = 0.5*(VSpeedToNeedle(0.0) + VSpeedToNeedle(-1000.0));
      glBegin(GL_LINES);
      glVertex2f(6.0,pos);
      glVertex2f(7.5,pos);
      glEnd();
      pos = 0.5*(VSpeedToNeedle(-1000.0) + VSpeedToNeedle(-2000.0));
      glBegin(GL_LINES);
      glVertex2f(6.0,pos);
      glVertex2f(7.5,pos);
      glEnd();
      pos = 0.5*(VSpeedToNeedle(-2000.0) + VSpeedToNeedle(-6000.0));
      glBegin(GL_LINES);
      glVertex2f(6.0,pos);
      glVertex2f(7.5,pos);
      glEnd();
  
      // Horizontal center detent
      glBegin(GL_LINES);
      glVertex2f(5.0, VSpeedToNeedle(0.0));
      glVertex2f(10.0, VSpeedToNeedle(0.0));

      //      printf("%f \n",temp);

      // Next draw the angled line that indicates climb rate
      // FIX for strange behavior below -1000 fpm
      glVertex2f( 5.0, VSpeedToNeedle(*vertical_speed));
      //      glVertex2f( 5.0, VSpeedToNeedle(temp));
      glVertex2f( 30.0, m_NeedleCenter);
      glEnd();

      glPopMatrix();
      
    } else {
      
      /* no data available */
      
      glPushMatrix();
      
      glColor3ub(COLOR_ORANGE);
      
      glTranslatef(m_PhysicalSize.x*0.5, m_PhysicalSize.y/2.0+1.0*bigFontSize, 0);
      
      m_pFontManager->SetSize(m_Font, bigFontSize, bigFontSize );
      m_pFontManager->Print(-0.5*bigFontSize, bigFontSize ,"V", m_Font ); 
      m_pFontManager->Print(-0.5*bigFontSize, -0.5*bigFontSize ,"E", m_Font ); 
      m_pFontManager->Print(-0.5*bigFontSize, -2.0*bigFontSize ,"R", m_Font ); 
      m_pFontManager->Print(-0.5*bigFontSize, -3.5*bigFontSize ,"T", m_Font ); 
      
      float ss = bigFontSize;
      glBegin(GL_LINE_LOOP);
      glVertex2f(-0.8*ss,+2.5*ss);
      glVertex2f(-0.8*ss,-4.0*ss);
      glVertex2f(0.8*ss,-4.0*ss);
      glVertex2f(0.8*ss,2.5*ss);
      glEnd();
      
      glPopMatrix();
	
    }
      
  }

  float B737PFDVSI::VSpeedToNeedle(float vspd)
  {
    float needle;
    float avspd = fabs(vspd);

    if (avspd < 1000.0) {
      needle = avspd/1000.0 * 10./22. * m_MaxNeedleDeflection;
    } else if (avspd < 2000.0) {
      needle = ((avspd-1000.)/1000.0 * 7./22. + 10./22.) * m_MaxNeedleDeflection;
    } else if (avspd < 6000.0) {
      needle = ((avspd-2000.)/4000.0 * 5./22. + 17./22.) * m_MaxNeedleDeflection;
    } else {
      needle = m_MaxNeedleDeflection;
    }
    
    if(vspd>=0.0) {
      needle = m_NeedleCenter + needle;
    } else {
      needle = m_NeedleCenter - needle;
    }
    
    //    printf("%f %f\n",vspd,needle);

    return needle;
  }

} // end namespace OpenGC


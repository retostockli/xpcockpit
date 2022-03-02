/*=============================================================================

  This is the ogcBasicClockComponent.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  Created:
    Date:   2015-05-05
    Author: Hans Jansen

  Copyright (C) 2011-2015 Hans Jansen (hansjansen@users.sourceforge.net)
  and/or                  Reto Stöckli (stockli@users.sourceforge.net)

  This program is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>. 

===============================================================================

  The OpenGC subproject has been derived from:
    OpenGC - The Open Source Glass Cockpit Project
    Copyright (c) 2001-2003 Damion Shelton

===============================================================================

  The following datarefs are used by this instrument:
    sim/cockpit2/clock_timer/zulu_time_hours
    sim/cockpit2/clock_timer/zulu_time_minutes
    sim/cockpit2/clock_timer/zulu_time_seconds

=============================================================================*/

#include <stdio.h>
#include <math.h>

#include "ogcGaugeComponent.h"
#include "ogcCircleEvaluator.h"
#include "BasicClock/ogcBasicClockComponent.h"

namespace OpenGC
{

  BasicClockComponent::BasicClockComponent()
  {
    if (verbosity > 0) printf("BasicClockComponent - constructing\n");

    m_Font = m_pFontManager->LoadDefaultFont();
  
    //    m_PhysicalPosition.x = 0;
    //    m_PhysicalPosition.y = 0;

    //    m_PhysicalSize.x = 0;
    //    m_PhysicalSize.y = 0;

    //    m_Scale.x = 1.0;
    //    m_Scale.y = 1.0;

    if (verbosity > 1) printf("BasicClockComponent - constructed\n");
  }

  BasicClockComponent::~BasicClockComponent()
  {
  }

  void BasicClockComponent::Render()
  {

    // Call base class to setup for size and position
    GaugeComponent::Render();

    // For drawing circles
    CircleEvaluator aCircle;

    float fontSize = 12;

    // (Re-)Define the datarefs we want to use
    int *zuluHours   = link_dataref_int("sim/cockpit2/clock_timer/zulu_time_hours");
    int *zuluMinutes = link_dataref_int("sim/cockpit2/clock_timer/zulu_time_minutes");
    int *zuluSeconds = link_dataref_int("sim/cockpit2/clock_timer/zulu_time_seconds");

    if (verbosity > 1)
    {
      printf ("BasicClockComponent - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("BasicClockComponent -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("BasicClockComponent -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("BasicClockComponent -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    double partSize = m_PhysicalSize.x;	// defines total component size (just for name simplification) (always square!)
    double partCenter = partSize / 2;		// defines component center

    m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	
    glPushMatrix();
    glTranslated(partCenter, partCenter, 0);

    // The rim of the dial
    glColor3ub(100,100,120); // lighter Gray-blue
    aCircle.SetOrigin(0, 0);
    aCircle.SetRadius(partSize/2);
    aCircle.SetDegreesPerPoint(5);
    aCircle.SetArcStartEnd(0.0,360.0);
    glBegin(GL_TRIANGLE_FAN);
      glVertex2f(0,partSize/2);
      glVertex2f(partSize/2,0);
      aCircle.Evaluate();
      glVertex2f(0,0);
    glEnd();

    // The background of the dial
    glColor3ub(10,10,20); // darker Gray-blue
    aCircle.SetOrigin(0, 0);
    aCircle.SetRadius(partSize*0.93/2);
    aCircle.SetDegreesPerPoint(5);
    aCircle.SetArcStartEnd(0.0,360.0);
    glBegin(GL_TRIANGLE_FAN);
      glVertex2f(0,partSize*0.93/2);
      glVertex2f(partSize*0.93/2,0);
      aCircle.Evaluate();
      glVertex2f(0,0);
    glEnd();
    glPopMatrix();

    // The tick marks
    glColor3ub( 255, 255, 255 ); // White
    glPushMatrix();
    glTranslated(partCenter, partCenter, 0);
    for ( int i=1; i<=60; i++ ) {
      glRotated(-6.0,0,0,1);
      if ( i % 5 == 0 ) {
        glBegin(GL_POLYGON);
          glVertex2f(-1,partSize*0.75/2);
          glVertex2f(-1,partSize*0.85/2);
          glVertex2f(1,partSize*0.85/2);
          glVertex2f(1,partSize*0.75/2);
        glEnd();
      } else {
        glBegin(GL_POLYGON);
          glVertex2f(-1,partSize*0.80/2);
          glVertex2f(-1,partSize*0.85/2);
          glVertex2f(1,partSize*0.85/2);
          glVertex2f(1,partSize*0.80/2);
        glEnd();
      }
    }
    glPopMatrix();

    // The minutes digits
        int cl_i = (int) partSize * 0.33;
        int cl_m = (int) partSize * 0.36;
        int cl_o = (int) partSize * 0.39;
        int d_r  = (int) partSize * 0.35;
        int d_rx = (int) d_r * 0.82;
        int d_ry = (int) d_r * 0.5;
        int d_h  = (int) fontSize * 0.5;
        int d_f  = (int) fontSize * 0.125;
        int d_w  = (int) fontSize;        // width of a single character (non-proportional font!)
    m_pFontManager->Print( partSize/2 - d_w,        partSize/2 - d_h - d_r  - d_f, "30", m_Font );
    m_pFontManager->Print( partSize/2 - d_w + d_rx, partSize/2 - d_h - d_ry - d_f, "20", m_Font );
    m_pFontManager->Print( partSize/2 - d_w + d_rx, partSize/2 - d_h + d_ry - d_f, "10", m_Font );
    m_pFontManager->Print( partSize/2 - d_w,        partSize/2 - d_h + d_r  - d_f, "60", m_Font );
    m_pFontManager->Print( partSize/2 - d_w - d_rx, partSize/2 - d_h + d_ry - d_f, "50", m_Font );
    m_pFontManager->Print( partSize/2 - d_w - d_rx, partSize/2 - d_h - d_ry - d_f, "40", m_Font );

    // The hands
    glTranslated(partCenter, partCenter, 0);

    glPushMatrix(); // Hours
    glRotated(*zuluHours * -6.0, 0, 0, 1);
// ==> my latest version -- better resolution? <== //    glRotated((*zuluHours + *zuluMinutes / 60.0) * -30.0, 0, 0, 1);
    glColor3ub( 255, 255, 255 ); // White
    glBegin(GL_POLYGON);
      glVertex2f(-5,0);
      glVertex2f(-5,partSize*0.50/2);
      glVertex2f(0,partSize*0.60/2);
      glVertex2f(5,partSize*0.50/2);
      glVertex2f(5,0);
    glEnd();
    glColor3ub( 192, 192, 192 ); // Light Grey
    glBegin(GL_POLYGON);
      glVertex2f(-3,0);
      glVertex2f(-3,partSize*0.45/2);
      glVertex2f(0,partSize*0.55/2);
      glVertex2f(3,partSize*0.45/2);
      glVertex2f(3,0);
    glEnd();
    glPopMatrix();

    glPushMatrix(); // Minutes
    glRotated(*zuluMinutes * -6.0, 0, 0, 1);
    glColor3ub( 255, 255, 255 ); // White
    glBegin(GL_POLYGON);
      glVertex2f(-4,0);
      glVertex2f(-4,partSize*0.60/2);
      glVertex2f(0,partSize*0.70/2);
      glVertex2f(4,partSize*0.60/2);
      glVertex2f(4,0);
    glEnd();
    glColor3ub( 192, 192, 192 ); // Light Grey
    glBegin(GL_POLYGON);
      glVertex2f(-2,0);
      glVertex2f(-2,partSize*0.55/2);
      glVertex2f(0,partSize*0.65/2);
      glVertex2f(2,partSize*0.55/2);
      glVertex2f(2,0);
    glEnd();
    glPopMatrix();

    glPushMatrix(); // Seconds
    glRotated(*zuluSeconds * -6.0, 0, 0, 1);
    glColor3ub( 255, 255, 0 ); // Yellow
    glBegin(GL_POLYGON);
      glVertex2f(-2,0);
      glVertex2f(-2,partSize*0.68/2);
      glVertex2f(0,partSize*0.78/2);
      glVertex2f(2,partSize*0.68/2);
      glVertex2f(2,0);
    glEnd();
    glPopMatrix();

    glPushMatrix(); // Central disc
    glColor3ub(192, 192, 0); // somewhat darker Yellow
    aCircle.SetOrigin(0, 0);
    aCircle.SetRadius(partSize*0.10/2);
    aCircle.SetDegreesPerPoint(15);
    aCircle.SetArcStartEnd(0.0,360.0);
    glBegin(GL_TRIANGLE_FAN);
      glVertex2f(0,partSize*0.10/2);
      glVertex2f(partSize*0.10/2,0);
      aCircle.Evaluate();
      glVertex2f(0,0);
    glEnd();
    glPopMatrix();

    glTranslated(-partCenter, -partCenter, 0);
}

} // end namespace OpenGC

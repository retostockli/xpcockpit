/*=============================================================================

  This is the ogcB737ClockComponent.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === B737 style Main-Panel Clock ===

  Created:
  Date:        2015-06-14
  Author:      Hans Jansen
  Last change: 2020-01-22

  Copyright (C) 2011-2020 Hans Jansen (hansjansen@users.sourceforge.net)
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

  =============================================================================*/

#include <stdio.h>
#include <math.h>

#include "ogcGaugeComponent.h"
#include "ogcB737Clock.h"
#include "ogcB737ClockComponent.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  B737ClockComponent::B737ClockComponent () {
    if (verbosity > 0) printf("ogcB737ClockComponent - constructing\n");

    m_Font = m_pFontManager->LoadFont((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 80;
    m_PhysicalSize.y = 80;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 2) printf ("ogcB737ClockComponent - constructed\n");
  }

  B737ClockComponent::~B737ClockComponent () {
  }
 
  void B737ClockComponent::Render () {

    float fontHeight = 4;
    float fontWidth = 3.5;
    char buff[36];

    // Call base class to setup for size and position
    GaugeComponent::Render ();

    // Request the datarefs we want to use

    //    int *cy = link_dataref_int ("sim/cockpit2/clock_timer/current_year");        // not defined in sim & not used here
    int *cm = link_dataref_int ("sim/cockpit2/clock_timer/current_month");
    int *cd = link_dataref_int ("sim/cockpit2/clock_timer/current_day");
    int *zth = link_dataref_int ("sim/cockpit2/clock_timer/zulu_time_hours");
    int *ztm = link_dataref_int ("sim/cockpit2/clock_timer/zulu_time_minutes");
    int *zts = link_dataref_int ("sim/cockpit2/clock_timer/zulu_time_seconds");
    int *ds = link_dataref_int ("sim/cockpit2/clock_timer/date_is_showing");
    int *tr = link_dataref_int ("sim/cockpit2/clock_timer/timer_running");      
    int *eth = link_dataref_int ("sim/cockpit2/clock_timer/elapsed_time_hours"); 
    int *etm = link_dataref_int ("sim/cockpit2/clock_timer/elapsed_time_minutes");
    int *ets = link_dataref_int ("sim/cockpit2/clock_timer/elapsed_time_seconds");
    float *trt = link_dataref_flt ("sim/time/total_running_time_sec", 0);

    /* Draw the Dial */
    
    // For drawing circles
    CircleEvaluator aCircle;

    double partSize = m_PhysicalSize.x;	// defines total component size (just for name simplification) (always square!)
    double partCenter = partSize / 2;		// defines component center

    m_pFontManager->SetSize(m_Font, fontHeight, fontWidth);

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
    glLineWidth(2.0);
    for ( int i=1; i<=60; i++ ) {
      glRotated(-6.0,0,0,1);
      if ( i % 5 == 0 ) {
        glBegin(GL_LINES);
	glVertex2f(0,partSize*0.75/2);
	glVertex2f(0,partSize*0.90/2);
        glEnd();
      } else {
        glBegin(GL_LINES);
	glVertex2f(0,partSize*0.75/2);
	glVertex2f(0,partSize*0.825/2);
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
    int d_h  = (int) fontHeight * 0.5;
    int d_f  = (int) fontHeight * 0.125;
    int d_w  = (int) fontWidth;        // width of a single character (non-proportional font!)
    m_pFontManager->Print( partSize/2 - d_w,        partSize/2 - d_h - d_r  - d_f, "30", m_Font );
    m_pFontManager->Print( partSize/2 - d_w + d_rx, partSize/2 - d_h - d_ry - d_f, "20", m_Font );
    m_pFontManager->Print( partSize/2 - d_w + d_rx, partSize/2 - d_h + d_ry - d_f, "10", m_Font );
    m_pFontManager->Print( partSize/2 - d_w,        partSize/2 - d_h + d_r  - d_f, "60", m_Font );
    m_pFontManager->Print( partSize/2 - d_w - d_rx, partSize/2 - d_h + d_ry - d_f, "50", m_Font );
    m_pFontManager->Print( partSize/2 - d_w - d_rx, partSize/2 - d_h - d_ry - d_f, "40", m_Font );

    // The hands
    glTranslated(partCenter, partCenter, 0);

    glPushMatrix(); // Seconds
    glRotated(*zts * -6.0, 0, 0, 1);
    glColor3ub( 255, 255, 0 ); // Yellow
    glBegin(GL_POLYGON);
    glVertex2f(-1,0);
    glVertex2f(-1,partSize*0.68/2);
    glVertex2f(0,partSize*0.78/2);
    glVertex2f(1,partSize*0.68/2);
    glVertex2f(1,0);
    glEnd();
    glPopMatrix();

    glPushMatrix(); // Central disc
    glColor3ub(100, 100, 100); // somewhat darker Yellow
    aCircle.SetOrigin(0, 0);
    aCircle.SetRadius(partSize*0.07/2);
    aCircle.SetDegreesPerPoint(15);
    aCircle.SetArcStartEnd(0.0,360.0);
    glBegin(GL_POLYGON);
    aCircle.Evaluate();
    glEnd();
    glPopMatrix();

    glTranslated(-partCenter, -partCenter, 0);

    // The borders of the displays
    /*
    glColor3ub (255,255,255);
    glBegin (GL_LINE_STRIP);
    glVertex2f ( 20, 120); glVertex2f (150, 120); glVertex2f (150,  85); glVertex2f ( 20,  85); glVertex2f ( 20, 120);
    glEnd ();
    glBegin (GL_LINE_STRIP);
    glVertex2f ( 35,  50); glVertex2f (130,  50); glVertex2f (130,  15); glVertex2f ( 35,  15); glVertex2f ( 35,  50);
    glEnd ();
    */
    
    // The texts
    glColor3ub (255,255,255);
    m_pFontManager->SetSize (m_Font, fontHeight, fontWidth);
    m_pFontManager->Print ( 35, 45, "GMT", m_Font);
    m_pFontManager->Print ( 30, 32, "ET/CHR", m_Font);

    // The displays
    glColor3ub (255,255,255);
    if (*ds == 0) {
      // normal time display
      m_pFontManager->SetSize (m_Font, 2.5*fontHeight, 2.5*fontWidth);
      sprintf (buff, "%02d:%02d", *zth, *ztm); m_pFontManager->Print ( 20,  50, buff, m_Font);
    } else {
      // date display
      m_pFontManager->SetSize (m_Font, 2.5*fontHeight, 2.5*fontWidth);
      m_pFontManager->Print (113,  95, "  ", m_Font);
      sprintf (buff, "%02d:%02d", *cm, *cd);
      m_pFontManager->Print ( 20,  50, buff, m_Font);
    }

  } // end Render()

} // end namespace OpenGC

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

    m_Font = m_pFontManager->LoadDefaultFont();
    m_DigiFont = m_pFontManager->LoadFont((char*) "digital.ttf");

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

    int acf_type = m_pDataSource->GetAcfType();
    if ((acf_type == 2) || (acf_type == 3)) {

      float fontHeight = 4;
      float fontWidth = 4;
      char buff[36];

      // Call base class to setup for size and position
      GaugeComponent::Render ();

      // Request the datarefs we want to use

      //    int *cy = link_dataref_int ("sim/cockpit2/clock_timer/current_year");        // not defined in sim & not used here
      int *mon = link_dataref_int ("sim/cockpit2/clock_timer/current_month");
      int *day = link_dataref_int ("sim/cockpit2/clock_timer/current_day");
      int *zth = link_dataref_int ("sim/cockpit2/clock_timer/zulu_time_hours");
      int *ztm = link_dataref_int ("sim/cockpit2/clock_timer/zulu_time_minutes");
      int *lth = link_dataref_int ("sim/cockpit2/clock_timer/local_time_hours");
      int *ltm = link_dataref_int ("sim/cockpit2/clock_timer/local_time_minutes");
      float *et_mode = link_dataref_flt("laminar/B738/clock/captain/et_mode",0);
      float *c_mode = link_dataref_flt ("laminar/B738/clock/captain/chrono_mode",0);
      float *c_disp = link_dataref_flt ("laminar/B738/clock/chrono_display_mode_capt",0);
      float *t_disp = link_dataref_flt ("laminar/B738/clock/clock_display_mode_capt",0);
      float *eth = link_dataref_flt ("laminar/B738/clock/captain/et_hours",0);
      float *etm = link_dataref_flt ("laminar/B738/clock/captain/et_minutes",0);
      float *ets = link_dataref_flt ("laminar/B738/clock/captain/et_seconds",0);
      float *cm = link_dataref_flt ("laminar/B738/clock/captain/chrono_minutes",0);
      float *cs = link_dataref_flt ("laminar/B738/clock/captain/chrono_seconds",0);
   
      /* Draw the Dial */

      if ((*zth != INT_MISS) && (*ztm != INT_MISS)) {
    
	// For drawing circles
	CircleEvaluator aCircle;

	float partSize = m_PhysicalSize.x; // defines total component size (square part!)
	float partCenter = partSize / 2;   // defines component center
      
      
	/* Draw from Center */
	glPushMatrix();
	glTranslated(partCenter, partCenter, 0);
      
      
	// The background of the dial
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
      
	glColor3ub(COLOR_GRAY25);
	glBegin(GL_POLYGON);
	glVertex2f(-partSize/2,-partSize/2*0.65);
	glVertex2f(-partSize/2*0.65,-partSize/2);
	glVertex2f(+partSize/2*0.65,-partSize/2);
	glVertex2f(+partSize/2,-partSize/2*0.65);
	glVertex2f(+partSize/2,+partSize/2*0.65);
	glVertex2f(+partSize/2*0.65,+partSize/2);
	glVertex2f(-partSize/2*0.65,+partSize/2);
	glVertex2f(-partSize/2,+partSize/2*0.65);
	glEnd();
	glColor3ub(COLOR_BLACK);
	glBegin(GL_POLYGON);
	glVertex2f(-partSize/2*0.82,-partSize/2*0.45);
	glVertex2f(-partSize/2*0.45,-partSize/2*0.82);
	glVertex2f(+partSize/2*0.45,-partSize/2*0.82);
	glVertex2f(+partSize/2*0.82,-partSize/2*0.45);
	glVertex2f(+partSize/2*0.82,+partSize/2*0.45);
	glVertex2f(+partSize/2*0.45,+partSize/2*0.82);
	glVertex2f(-partSize/2*0.45,+partSize/2*0.82);
	glVertex2f(-partSize/2*0.82,+partSize/2*0.45);
	glEnd();
      
	glColor3ub(COLOR_DARKVIOLETT);
	aCircle.SetOrigin(0, 0);
	aCircle.SetRadius(partSize*0.81/2);
	aCircle.SetDegreesPerPoint(5);
	aCircle.SetArcStartEnd(0.0,360.0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0,partSize*0.81/2);
	glVertex2f(partSize*0.81/2,0);
	aCircle.Evaluate();
	glVertex2f(0,0);
	glEnd();
      
	glPopMatrix();

	// Buttons
	glColor3ub(COLOR_WHITE); // White
	m_pFontManager->SetSize(m_Font, 0.7*fontWidth, 0.8*fontHeight);
	m_pFontManager->Print(partSize*0.015, partSize*0.3, "ET", m_Font );
	m_pFontManager->Print(partSize*0.3, partSize*0.025, "RESET", m_Font );
	m_pFontManager->Print(partSize*0.225, partSize*0.925, "CHR", m_Font );
	m_pFontManager->Print(partSize*0.45, partSize*0.925, "TIME/DATE", m_Font );
	m_pFontManager->Print(partSize*0.91, partSize*0.675, "SET", m_Font );
	m_pFontManager->Print(partSize*0.7, partSize*0.025, "-", m_Font );
	m_pFontManager->Print(partSize*0.94, partSize*0.3, "+", m_Font );

	float ox;
	float oy;
	float dx;
	float dy;
	glColor3ub(COLOR_BLACK);
	// CHR Button
	glBegin(GL_POLYGON);
	ox = partSize*0.05;
	oy = partSize*0.85;
	dx = partSize*0.075;
	dy = partSize*0.075;
	glVertex2f(ox,oy);
	glVertex2f(ox+dx,oy-dy);
	glVertex2f(ox+2.5*dx,oy+0.5*dy);
	glVertex2f(ox+1.5*dx,oy+1.5*dy);
	glEnd();
	// ET Button
	glBegin(GL_POLYGON);
	ox = partSize*0.015;
	oy = partSize*0.21;
	dx = partSize*0.06;
	dy = partSize*0.06;
	glVertex2f(ox,oy);
	glVertex2f(ox+1.5*dx,oy-1.5*dy);
	glVertex2f(ox+2.5*dx,oy-0.5*dy);
	glVertex2f(ox+dx,oy+dy);
	glEnd();
	// RESET Button
	glBegin(GL_POLYGON);
	ox = partSize*0.015+1.6*dx;
	oy = partSize*0.21-1.6*dy;
	glVertex2f(ox,oy);
	glVertex2f(ox+1.5*dx,oy-1.5*dy);
	glVertex2f(ox+2.5*dx,oy-0.5*dy);
	glVertex2f(ox+dx,oy+dy);
	glEnd();
	// TIME/DATE Button
	glBegin(GL_POLYGON);
	ox = partSize*0.735;
	oy = partSize*0.925;
	dx = partSize*0.06;
	dy = partSize*0.06;
	glVertex2f(ox,oy);
	glVertex2f(ox+1.5*dx,oy-1.5*dy);
	glVertex2f(ox+2.5*dx,oy-0.5*dy);
	glVertex2f(ox+dx,oy+dy);
	glEnd();
	// SET Button
	glBegin(GL_POLYGON);
	ox = partSize*0.735+1.6*dx;
	oy = partSize*0.925-1.6*dy;
	dx = partSize*0.06;
	dy = partSize*0.06;
	glVertex2f(ox,oy);
	glVertex2f(ox+1.5*dx,oy-1.5*dy);
	glVertex2f(ox+2.5*dx,oy-0.5*dy);
	glVertex2f(ox+dx,oy+dy);
	glEnd();
	// - Button
	glBegin(GL_POLYGON);
	ox = partSize*0.735;
	oy = partSize*0.084;
	dx = partSize*0.06;
	dy = partSize*0.06;
	glVertex2f(ox,oy);
	glVertex2f(ox+dx,oy-dy);
	glVertex2f(ox+2.5*dx,oy+0.5*dy);
	glVertex2f(ox+1.5*dx,oy+1.5*dy);
	glEnd();
	// + Button
	glBegin(GL_POLYGON);
	ox = partSize*0.735+1.6*dx;
	oy = partSize*0.084+1.6*dy;
	dx = partSize*0.06;
	dy = partSize*0.06;
	glVertex2f(ox,oy);
	glVertex2f(ox+dx,oy-dy);
	glVertex2f(ox+2.5*dx,oy+0.5*dy);
	glVertex2f(ox+1.5*dx,oy+1.5*dy);
	glEnd();
        
  
	// The tick marks
	glColor3ub( COLOR_WHITE ); // White
	glPushMatrix();
	glTranslated(partCenter, partCenter, 0);
	for ( int i=1; i<=60; i++ ) {
	  glRotated(-6.0,0,0,1);
	  if ( i % 5 == 0 ) {
	    glLineWidth(4.0);
	    glBegin(GL_LINES);
	    glVertex2f(0,partSize*0.675/2);
	    glVertex2f(0,partSize*0.80/2);
	    glEnd();
	  } else {
	    glLineWidth(2.0);
	    glBegin(GL_LINES);
	    glVertex2f(0,partSize*0.675/2);
	    glVertex2f(0,partSize*0.75/2);
	    glEnd();
	  }
	}
	glPopMatrix();

	// The Seconds Digits of the Dial
	float d_r  = partSize * 0.315;
	float d_rx = d_r * 0.82;
	float d_ry = d_r * 0.5;
	float d_h  = fontHeight * 0.5;
	float d_f  = fontHeight * 0.125;
	float d_w  = fontWidth; // width of a single character (non-proportional font!)
	m_pFontManager->SetSize(m_Font, fontWidth, fontHeight);
	m_pFontManager->Print( partCenter - d_w + d_f,        partCenter - d_h - d_r  + d_f, "30", m_Font );
	m_pFontManager->Print( partCenter - d_w + d_rx,       partCenter - d_h - d_ry + d_f, "20", m_Font );
	m_pFontManager->Print( partCenter - d_w + d_rx,       partCenter - d_h + d_ry - d_f, "10", m_Font );
	m_pFontManager->Print( partCenter - d_w + d_f,        partCenter - d_h + d_r  - d_f, "60", m_Font );
	m_pFontManager->Print( partCenter - d_w - d_rx + 2*d_f, partCenter - d_h + d_ry - d_f, "50", m_Font );
	m_pFontManager->Print( partCenter - d_w - d_rx + 2*d_f, partCenter - d_h - d_ry + d_f, "40", m_Font );


	// The hand
	glPushMatrix(); 
	glTranslated(partCenter, partCenter, 0);
	glRotated(*cs * -6.0, 0, 0, 1);
	glColor3ub( COLOR_WHITE ); 
	glBegin(GL_POLYGON);
	glVertex2f(-0.75,partSize*0.775/2);
	glVertex2f(0.75,partSize*0.775/2);
	glVertex2f(0.25,partSize*0.45/2);
	glVertex2f(-0.25,partSize*0.45/2);
	glEnd();
	glPopMatrix();

	// The states
	glColor3ub (COLOR_WHITE);
	m_pFontManager->SetSize (m_Font, 0.5*fontWidth, 0.5*fontHeight);
	if (*et_mode == 1.0) {
	  m_pFontManager->Print(partSize*0.22, partSize*0.15, "RUN", m_Font );
	} else {
	  m_pFontManager->Print(partSize*0.22, partSize*0.15, "HLD", m_Font );
	}
	if ((*t_disp == 1.0) || (*t_disp == 2.0)) {
	  m_pFontManager->Print(partSize*0.72, partSize*0.8225, "UTC", m_Font );
	} else {
	  m_pFontManager->Print(partSize*0.72, partSize*0.8225, "MAN", m_Font );
	}
      
	// The displays
	glColor3ub (COLOR_WHITE);
	if ((*t_disp == 1.0) || (*t_disp == 3.0)) {
	  // time display
	  m_pFontManager->SetSize (m_Font, 0.8*fontWidth, 0.8*fontHeight);
	  m_pFontManager->Print ( partCenter-1.5*fontWidth, partCenter+1.4*2.5*fontHeight, "TIME", m_Font);
	  m_pFontManager->SetSize (m_DigiFont, 2.5*fontWidth, 2.5*fontHeight);
	  if (*t_disp == 1.0) {
	    sprintf (buff, "%02d", *zth);
	  } else {
	    sprintf (buff, "%02d", *lth);
	  }
	  m_pFontManager->Print ( partCenter-1.4*2.5*fontWidth, partCenter+0.3*2.5*fontHeight, buff, m_DigiFont);
	  sprintf (buff, ":");
	  m_pFontManager->Print ( partCenter-0.3*2.5*fontWidth, partCenter+0.3*2.5*fontHeight, buff, m_DigiFont);
	  if (*t_disp == 1.0) {	
	    sprintf (buff, "%02d", *ztm);
	  } else {
	    sprintf (buff, "%02d", *ltm);
	  }
	  m_pFontManager->Print ( partCenter+0.2*2.5*fontWidth, partCenter+0.3*2.5*fontHeight, buff, m_DigiFont);
	} else {
	  // date display
	  m_pFontManager->SetSize (m_Font, 0.8*fontWidth, 0.8*fontHeight);
	  m_pFontManager->Print ( partCenter-1.5*fontWidth, partCenter+1.4*2.5*fontHeight, "DATE", m_Font);
	  m_pFontManager->SetSize (m_DigiFont, 2.5*fontWidth, 2.5*fontHeight);
	  sprintf (buff, "%02d", *mon);
	  m_pFontManager->Print ( partCenter-1.4*2.5*fontWidth, partCenter+0.3*2.5*fontHeight, buff, m_DigiFont);
	  sprintf (buff, "%02d", *day);
	  m_pFontManager->Print ( partCenter+0.2*2.5*fontWidth, partCenter+0.3*2.5*fontHeight, buff, m_DigiFont);
	}
	if (*c_disp == 1.0) {
	  // ET display
	  m_pFontManager->SetSize (m_Font, 0.8*fontWidth, 0.8*fontHeight);
	  m_pFontManager->Print ( partCenter-2.0*fontWidth, partCenter-1.8*2.5*fontHeight, "ET", m_Font);
	  m_pFontManager->SetSize (m_DigiFont, 2.5*fontWidth, 2.5*fontHeight);
	  sprintf (buff, "%02.0f", *eth);
	  m_pFontManager->Print ( partCenter-1.4*2.5*fontWidth, partCenter-1.2*2.5*fontHeight, buff, m_DigiFont);
	  sprintf (buff, ":");
	  m_pFontManager->Print ( partCenter-0.3*2.5*fontWidth, partCenter-1.2*2.5*fontHeight, buff, m_DigiFont);
	  sprintf (buff, "%02.0f", *etm);
	  m_pFontManager->Print ( partCenter+0.2*2.5*fontWidth, partCenter-1.2*2.5*fontHeight, buff, m_DigiFont);
	} else {
	  // CHR display
	  m_pFontManager->SetSize (m_Font, 0.8*fontWidth, 0.8*fontHeight);
	  m_pFontManager->Print ( partCenter+0.5*fontWidth, partCenter-1.8*2.5*fontHeight, "CHR", m_Font);
	  m_pFontManager->SetSize (m_DigiFont, 2.5*fontWidth, 2.5*fontHeight);
	  sprintf (buff, "%02.0f", *cm);
	  m_pFontManager->Print ( partCenter-1.4*2.5*fontWidth, partCenter-1.2*2.5*fontHeight, buff, m_DigiFont);
	  sprintf (buff, ":");
	  m_pFontManager->Print ( partCenter-0.3*2.5*fontWidth, partCenter-1.2*2.5*fontHeight, buff, m_DigiFont);
	  sprintf (buff, "%02.0f", *cs);
	  m_pFontManager->Print ( partCenter+0.2*2.5*fontWidth, partCenter-1.2*2.5*fontHeight, buff, m_DigiFont);
	}

      }

    } // if ZIBO737
				    
  } // end Render()

} // end namespace OpenGC

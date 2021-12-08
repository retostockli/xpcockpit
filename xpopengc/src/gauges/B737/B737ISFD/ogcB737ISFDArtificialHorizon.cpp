/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Copyright (C) 2021 by:
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

#include <stdio.h>

#include "ogcGaugeComponent.h"
#include "ogcCircleEvaluator.h"
#include "ogcB737ISFD.h"
#include "ogcB737ISFDArtificialHorizon.h"

namespace OpenGC
{

  B737ISFDArtificialHorizon::B737ISFDArtificialHorizon()
  {
    printf("B737ISFDArtificialHorizon constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 62;
    m_PhysicalSize.y = 70;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  B737ISFDArtificialHorizon::~B737ISFDArtificialHorizon()
  {

  }

  void B737ISFDArtificialHorizon::Render()
  {
    // Call base class to setup viewport and projection
    GaugeComponent::Render();

    // For drawing circles
    CircleEvaluator aCircle;
    char buffer[8];

    float bigFontSize = 5.0;
    float lineWidth = 3.0;
    float scaley = 0.8;
    float scalex = 0.75;
    float shifty = m_PhysicalSize.y * 0.07;
    float radius = m_PhysicalSize.x / 2.0;

    // First, store the "root" position of the gauge component
    glMatrixMode(GL_MODELVIEW);

    bool is_captain = (this->GetArg() == 0);
    
    int acf_type = m_pDataSource->GetAcfType();
    
    float *pitch = link_dataref_flt("sim/flightmodel/position/theta",-1);
    float *roll = link_dataref_flt("sim/flightmodel/position/phi",-1);

    int *nav1_has_glideslope = link_dataref_int("sim/cockpit2/radios/indicators/nav1_flag_glideslope");
    float *nav1_hdef = link_dataref_flt("sim/cockpit/radios/nav1_hdef_dot",-2);
    float *nav1_vdef = link_dataref_flt("sim/cockpit/radios/nav1_vdef_dot",-2);
    //    int *nav2_has_glideslope = link_dataref_int("sim/cockpit2/radios/indicators/nav2_flag_glideslope");
    //    float *nav2_hdef = link_dataref_flt("sim/cockpit/radios/nav2_hdef_dot",-2);
    //    float *nav2_vdef = link_dataref_flt("sim/cockpit/radios/nav2_vdef_dot",-2);
   
    int *irs_mode;
    if ((acf_type == 2) || (acf_type == 3)) {
      irs_mode = link_dataref_int("laminar/B738/irs/irs_mode");
    } else {
      irs_mode = link_dataref_int("xpserver/irs_mode");
      *irs_mode = 2;
    }

    if ((*roll != FLT_MISS) && (*pitch != FLT_MISS)) {

      if (*irs_mode == 2) {

	glPushMatrix();
      
	// Move to the center of the window
	glTranslatef(m_PhysicalSize.x/2,m_PhysicalSize.y/2-shifty,0);

	// Rotate based on the bank
	glRotatef(*roll, 0, 0, 1);

	// Translate in the direction of the rotation based
	// on the pitch. On the 737, a pitch of 1 degree = 2 mm
	//glTranslatef(0, *pitch * -2.0, 0);
	glTranslatef(0, 0, 0);

	//-------------------Gauge Background------------------
	// It's drawn oversize to allow for pitch and bank

	// The "ground" rectangle
	// Remember, the coordinate system is now centered in the gauge component
	glColor3ub(COLOR_GROUND);

	glBegin(GL_POLYGON);
	glVertex2f(-300,-300);
	glVertex2f(-300,0);
	glVertex2f(300,0);
	glVertex2f(300,-300);
	glVertex2f(-300,-300);
	glEnd();

	// The "sky" rectangle
	// Remember, the coordinate system is now centered in the gauge component
	glColor3ub(COLOR_SKY);
  
	glBegin(GL_POLYGON);
	glVertex2f(-300,0);
	glVertex2f(-300,300);
	glVertex2f(300,300);
	glVertex2f(300,0);
	glVertex2f(-300,0);
	glEnd();


	//------------Draw the pitch markings--------------

	// Draw in white
	glColor3ub(COLOR_WHITE);
	// Specify line width
	glLineWidth(lineWidth);
	// The size for all pitch text
	m_pFontManager->SetSize(m_Font,4.0, 4.0);

	glBegin(GL_LINES);

	// The dividing line between sky and ground
	glVertex2f(-100,0);
	glVertex2f(100,0);

	// +2.5 degrees
	glVertex2f(-5.0*scalex,5.0*scaley);
	glVertex2f(5.0*scalex,5.0*scaley);

	// +5.0 degrees
	glVertex2f(-10.0*scalex,10.0*scaley);
	glVertex2f(10.0*scalex,10.0*scaley);

	// +7.5 degrees
	glVertex2f(-5.0*scalex,15.0*scaley);
	glVertex2f(5.0*scalex,15.0*scaley);

	// +10.0 degrees
	glVertex2f(-20.0*scalex,20.0*scaley);
	glVertex2f(20.0*scalex,20.0*scaley);

	// +12.5 degrees
	glVertex2f(-5.0*scalex,25.0*scaley);
	glVertex2f(5.0*scalex,25.0*scaley);

	// +15.0 degrees
	glVertex2f(-10.0*scalex,30.0*scaley);
	glVertex2f(10.0*scalex,30.0*scaley);

	// +17.5 degrees
	glVertex2f(-5.0*scalex,35.0*scaley);
	glVertex2f(5.0*scalex,35.0*scaley);

	// +20.0 degrees
	glVertex2f(-20.0*scalex,40.0*scaley);
	glVertex2f(20.0*scalex,40.0*scaley);

	// -2.5 degrees
	glVertex2f(-5.0*scalex,-5.0*scaley);
	glVertex2f(5.0*scalex,-5.0*scaley);

	// -5.0 degrees
	glVertex2f(-10.0*scalex,-10.0*scaley);
	glVertex2f(10.0*scalex,-10.0*scaley);

	// -7.5 degrees
	glVertex2f(-5.0*scalex,-15.0*scaley);
	glVertex2f(5.0*scalex,-15.0*scaley);

	// -10.0 degrees
	glVertex2f(-20.0*scalex,-20.0*scaley);
	glVertex2f(20.0*scalex,-20.0*scaley);

	// -12.5 degrees
	glVertex2f(-5.0*scalex,-25.0*scaley);
	glVertex2f(5.0*scalex,-25.0*scaley);

	// -15.0 degrees
	glVertex2f(-10.0*scalex,-30.0*scaley);
	glVertex2f(10.0*scalex,-30.0*scaley);

	// -17.5 degrees
	glVertex2f(-5.0*scalex,-35.0*scaley);
	glVertex2f(5.0*scalex,-35.0*scaley);

	// -20.0 degrees
	glVertex2f(-20.0*scalex,-40.0*scaley);
	glVertex2f(20.0*scalex,-40.0*scaley);

	glEnd();

	// +10
	m_pFontManager->Print(-29.5*scalex,18.0*scaley,"10",m_Font);

	// -10
	m_pFontManager->Print(-29.5*scalex,-22.0*scaley,"10",m_Font);

	// +20
	m_pFontManager->Print(-29.5*scalex,38.0*scaley,"20",m_Font);

	// -20
	m_pFontManager->Print(-29.5*scalex,-42.0*scaley,"20",m_Font);

	glPopMatrix();

	//-----The background behind the bank angle markings-------
	// Reset the modelview matrix
	glPushMatrix();
	
	// Draw in the sky color
	glColor3ub(COLOR_SKY);
	
	aCircle.SetOrigin(m_PhysicalSize.x/2.0,m_PhysicalSize.y/3.0);
	aCircle.SetRadius(m_PhysicalSize.x/1.5);
	aCircle.SetDegreesPerPoint(5);
	aCircle.SetArcStartEnd(300.0,360.0);
	
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0,m_PhysicalSize.y);
	glVertex2f(0,m_PhysicalSize.y/3.0);
	aCircle.Evaluate();
	glVertex2f(m_PhysicalSize.x/2.0,m_PhysicalSize.y);
	glEnd();
	
	aCircle.SetArcStartEnd(0.0,60.0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(m_PhysicalSize.x,m_PhysicalSize.y);
	glVertex2f(m_PhysicalSize.x/2.0,m_PhysicalSize.y);
	aCircle.Evaluate();
	glVertex2f(m_PhysicalSize.x,m_PhysicalSize.y/3.0);
	glEnd();
	
	glPopMatrix();
	
      } else {
	
	/* no data available */
	
	glPushMatrix();
	
	glColor3ub(COLOR_ORANGE);
	
	glTranslatef(m_PhysicalSize.x/2.0, m_PhysicalSize.y/2.0+2.0*bigFontSize, 0);
	
	m_pFontManager->SetSize(m_Font, bigFontSize, bigFontSize );
	m_pFontManager->Print(-1.5*bigFontSize, -0.5*bigFontSize ,"ATT", m_Font ); 
	
	float ss = bigFontSize;
	glBegin(GL_LINE_LOOP);
	glVertex2f(-2.0*ss,-ss);
	glVertex2f(2.0*ss,-ss);
	glVertex2f(2.0*ss,ss);
	glVertex2f(-2.0*ss,ss);
	glEnd();
	
	glPopMatrix();
	
      }
	
      //----------------The bank angle markings----------------

      // Left side bank markings
      // Reset the modelview matrix
      glPushMatrix();

      // Draw in white
      glColor3ub(COLOR_WHITE);

      // Draw the center detent
      glBegin(GL_POLYGON);
      glVertex2f(m_PhysicalSize.x/2,m_PhysicalSize.y-4.5);
      glVertex2f(m_PhysicalSize.x/2-3.5f,m_PhysicalSize.y);
      glVertex2f(m_PhysicalSize.x/2+3.5f,m_PhysicalSize.y);
      glVertex2f(m_PhysicalSize.x/2,m_PhysicalSize.y-4.5);
      glEnd();

      // Move to the center of the window
      glTranslatef(m_PhysicalSize.x/2,m_PhysicalSize.y/2-shifty,0);

      glRotatef(10.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,49);
      glEnd();
  
      glRotatef(10.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,49);
      glEnd();

      glRotatef(10.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,53);
      glEnd();

      glRotatef(15.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,49);
      glEnd();

      glRotatef(15.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,51);
      glEnd();

      glPopMatrix();

      // Right side bank markings
      // Reset the modelview matrix
      glPushMatrix();
      // Move to the center of the window
      glTranslatef(47,49,0);

      glRotatef(-10.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,49);
      glEnd();
  
      glRotatef(-10.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,49);
      glEnd();

      glRotatef(-10.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,53);
      glEnd();

      glRotatef(-15.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,49);
      glEnd();

      glRotatef(-15.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,51);
      glEnd();

      glPopMatrix();

      //----------------End Draw Bank Markings----------------


      //----------------Bank Indicator----------------
      // Reset the modelview matrix
      glPushMatrix();
      // Move to the center of the window
      glTranslatef(47,49,0);
      // Rotate based on the bank
      glRotatef(*roll, 0, 0, 1);

      // Draw in white
      glColor3ub(COLOR_WHITE);
      // Specify line width
      glLineWidth(lineWidth);

      glBegin(GL_LINE_LOOP); // the bottom rectangle
      glVertex2f(-4.5, 39.5);
      glVertex2f(4.5, 39.5);
      glVertex2f(4.5, 41.5);
      glVertex2f(-4.5, 41.5);
      glEnd();

      glBegin(GL_LINE_STRIP); // the top triangle
      glVertex2f(-4.5, 41.5);
      glVertex2f(0, 46);
      glVertex2f(4.5, 41.5);
      glEnd();

      glPopMatrix();
      //--------------End draw bank indicator------------

      //----------------Attitude Indicator----------------
      // Reset the modelview matrix
      glPushMatrix();
      // Move to the center of the window
      glTranslatef(47,49,0);

      // The center axis indicator
      // Black background
      glColor3ub(COLOR_BLACK);
      glBegin(GL_POLYGON);
      glVertex2f(1.25,1.25);
      glVertex2f(1.25,-1.25);
      glVertex2f(-1.25,-1.25);
      glVertex2f(-1.25,1.25);
      glVertex2f(1.25,1.25);
      glEnd();
      // White lines
      glColor3ub(COLOR_WHITE);
      glLineWidth(lineWidth);
      glBegin(GL_LINE_LOOP);
      glVertex2f(1.25,1.25);
      glVertex2f(1.25,-1.25);
      glVertex2f(-1.25,-1.25);
      glVertex2f(-1.25,1.25);
      glEnd();

      // The left part
      // Black background
      glColor3ub(COLOR_BLACK);
      glBegin(GL_POLYGON);
      glVertex2f(-39,1.25);
      glVertex2f(-19,1.25);
      glVertex2f(-19,-1.25);
      glVertex2f(-39,-1.25);
      glVertex2f(-39,1.25);
      glEnd();
      glBegin(GL_POLYGON);
      glVertex2f(-19,1.25);
      glVertex2f(-19,-5.75);
      glVertex2f(-22,-5.75);
      glVertex2f(-22,1.25);
      glVertex2f(-19,1.25);
      glEnd();
  
      // White lines
      glColor3ub(COLOR_WHITE);
      glLineWidth(lineWidth);
      glBegin(GL_LINE_LOOP);
      glVertex2f(-39,1.25);
      glVertex2f(-19,1.25);
      glVertex2f(-19,-5.75);
      glVertex2f(-22,-5.75);
      glVertex2f(-22,-1.25);
      glVertex2f(-39,-1.25);
      glEnd();

      // The right part
      // Black background
      glColor3ub(COLOR_BLACK);
      glBegin(GL_POLYGON);
      glVertex2f(39,1.25);
      glVertex2f(19,1.25);
      glVertex2f(19,-1.25);
      glVertex2f(39,-1.25);
      glVertex2f(39,1.25);
      glEnd();
      glBegin(GL_POLYGON);
      glVertex2f(19,1.25);
      glVertex2f(19,-5.75);
      glVertex2f(22,-5.75);
      glVertex2f(22,1.25);
      glVertex2f(19,1.25);
      glEnd();
  
      // White lines
      glColor3ub(COLOR_WHITE);
      glLineWidth(lineWidth);
      glBegin(GL_LINE_LOOP);
      glVertex2f(39,1.25);
      glVertex2f(19,1.25);
      glVertex2f(19,-5.75);
      glVertex2f(22,-5.75);
      glVertex2f(22,-1.25);
      glVertex2f(39,-1.25);
      glEnd();

      glPopMatrix();
      //--------------End draw attitude indicator------------


    }
    
  }
  
} // end namespace OpenGC

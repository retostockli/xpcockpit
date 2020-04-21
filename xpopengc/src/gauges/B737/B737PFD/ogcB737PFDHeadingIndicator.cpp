/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737PFDHeadingIndicator.cpp,v $

  Copyright (C) 2001-2015 by:
  Original author:
  Damion Shelton
  Contributors (in alphabetical order):
  Reto Stockli

  Last modification:
  Date:      $Date: 2015/11/24 $
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

#include "ogcGLHeaders.h"
#include <stdio.h>

#include "ogcGaugeComponent.h"
#include "ogcCircleEvaluator.h"
#include "ogcB737PFDHeadingIndicator.h"

namespace OpenGC
{

  B737PFDHeadingIndicator::B737PFDHeadingIndicator()
  {
    printf("B737PFDHeadingIndicator constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 130;
    m_PhysicalSize.y = 45;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  B737PFDHeadingIndicator::~B737PFDHeadingIndicator()
  {

  }

  void B737PFDHeadingIndicator::Render()
  {
    // Call base class to setup viewport and projection
    GaugeComponent::Render();

    int acf_type = m_pDataSource->GetAcfType();

    /*
      A few general notes:
  
      This is a hard gauge to get sized correctly, so we set up a bunch
      of symbolic constants for reference rather than hard coding everything.

      Also, the implementation of the 737 heading indicator is a little curious
      because although the gauge is circular in the real world, a given number
      of degrees on the displayed circle do not represent an equivalent
      difference in heading. indicatorDegreesPerTrueDegrees is the conversion
      factor between "gauge" degrees and "display" degrees.
    */

    // Save matrix
    glMatrixMode(GL_MODELVIEW);

    float centerX = 60;
    float centerY = -40;
    float radius = 65.0;
    float indicatorDegreesPerTrueDegrees = 1.5;

    float bigFontSize = 5.0;
    float littleFontSize = 3.5;

    char buffer[32];

    // The x-position of the font (depends on the number of characters in the heading)
    float fontx;

    // What's the heading?
    float *heading_mag = link_dataref_flt("sim/flightmodel/position/magpsi",-1);
    //    float *heading_true = link_dataref_flt("sim/flightmodel/position/psi",0);

    int *has_heading_bug;
    float *heading_mag_ap;
    if ((acf_type == 2) || (acf_type == 3)) {
      has_heading_bug = link_dataref_int("laminar/B738/nd/hdg_bug_line");
      heading_mag_ap = link_dataref_flt("laminar/B738/autopilot/mcp_hdg_dial",0);
    } else {
      has_heading_bug = link_dataref_int("xpserver/has_hdg_bug");
      heading_mag_ap = link_dataref_flt("sim/cockpit2/autopilot/heading_dial_deg_mag_pilot",0);
      *has_heading_bug = 1;
    }

    float *ap_course1 = link_dataref_flt("sim/cockpit/radios/nav1_obs_degm",0);    // NAV autopilot course1     
    //    float *ap_course2 = link_dataref_flt("sim/cockpit/radios/nav2_obs_degm",0);    // NAV autopilot course2    
    //    float *ap_course1_copilot = link_dataref_flt("sim/cockpit/radios/nav1_obs_degm2",0);    // NAV autopilot course1     
    //    float *ap_course2_copilot = link_dataref_flt("sim/cockpit/radios/nav2_obs_degm2",0);    // NAV autopilot course2    

    /* Draw integerized heading above center detent */
    // Convert the display heading to a string
    if (*heading_mag != FLT_MISS) {

      glPushMatrix();
      
      glTranslatef(centerX, centerY, 0);

      // Draw in gray
      glColor3ub(51,51,76);
      glLineWidth( 1.75 );

      // Set up the circle
      CircleEvaluator aCircle;
      aCircle.SetRadius(radius);
      aCircle.SetArcStartEnd(300,60);
      aCircle.SetDegreesPerPoint(5);
      aCircle.SetOrigin(0.0, 0.0);

      // Draw the circle
      glBegin(GL_TRIANGLE_FAN);
      glVertex2f(0,0);
      aCircle.Evaluate();
      glEnd();

      // Draw the center detent
      glColor3ub(255,255,255);
      glBegin(GL_LINE_LOOP);
      glVertex2f(0.0f,radius);
      glVertex2f(-3.0f,radius+5.0);
      glVertex2f(3.0f,radius+5.0);
      glEnd();

      snprintf( buffer, sizeof(buffer), "%d", (int) *heading_mag );

      if((int) *heading_mag>=100)
	fontx = -bigFontSize*1.5;
      else
	fontx = -bigFontSize/2;
  
      m_pFontManager->SetSize(m_Font, bigFontSize, bigFontSize );
      m_pFontManager->Print( fontx, radius+6.0,&buffer[0], m_Font ); 

      // Figure out the nearest heading that's a multiple of 10
      float nearestTen = (float)( (int) *heading_mag - (int) *heading_mag % 10);

      // Derotate by this offset
      glRotatef( -1.0*(*heading_mag - nearestTen) * indicatorDegreesPerTrueDegrees,0,0,-1);

      // Now derotate by 40 "virtual" degrees
      glRotatef(-40*indicatorDegreesPerTrueDegrees,0,0,-1);

      // Now draw lines 5 virtual degrees apart around the perimeter of the circle
      for(int i = (int) nearestTen - 40; i<= (int) nearestTen+40; i+=5)
	{
	  // The length of the tickmarks on the compass rose
	  float tickLength;

	  // Make sure the display heading is between 0 and 360
	  int displayHeading = (i+720)%360;

	  //printf("%i \n",displayHeading);

	  // If the heading is a multiple of ten, it gets a long tick
	  if(displayHeading%10==0)
	    {
	      tickLength = 3;

	      // Convert the display heading to a string
	      snprintf( buffer, sizeof(buffer), "%d", displayHeading/10 );
	      if(displayHeading%30==0)
		{
		  if(displayHeading>=100)
		    fontx = -bigFontSize;
		  else
		    fontx = -bigFontSize/2;

		  m_pFontManager->SetSize(m_Font, bigFontSize, bigFontSize);
		  m_pFontManager->Print(fontx, radius-tickLength-bigFontSize-1, &buffer[0], m_Font );
		}
	      else
		{
		  if(displayHeading>=100)
		    fontx = -littleFontSize;
		  else
		    fontx = -littleFontSize/2;

		  m_pFontManager->SetSize(m_Font, littleFontSize, littleFontSize);
		  m_pFontManager->Print(fontx, radius-tickLength-littleFontSize-1, &buffer[0], m_Font );
		}

	    }
	  else // Otherwise it gets a short tick
	    tickLength = 2;

	  glBegin(GL_LINES);
	  glVertex2f(0,radius);
	  glVertex2f(0,radius-tickLength);
	  glEnd();

	  glRotatef(5.0 * indicatorDegreesPerTrueDegrees,0,0,-1);
	}

      glPopMatrix();

      // draw magenta AP Heading

      if ((*heading_mag_ap != FLT_MISS) && (*has_heading_bug)) {
	
	glPushMatrix();
	
	// translate to center of drawing area
	glTranslatef(centerX, centerY, 0);
	
	// rotate to dialed AP Heading
	glRotatef((*heading_mag_ap - *heading_mag) * indicatorDegreesPerTrueDegrees,0,0,-1);
	
	glColor3ub(255, 0, 200);
	glLineWidth(2.0);
	
	glBegin(GL_LINE_LOOP);
	glVertex2f(-4.0f,radius);
	glVertex2f(4.0f,radius);
	glVertex2f(4.0f,radius+3.25);
	glVertex2f(2.8f,radius+3.25);
	glVertex2f(0.0f,radius);
	glVertex2f(-2.8f,radius+3.25);
	glVertex2f(-4.0f,radius+3.25);
	glEnd();  
	
	glPopMatrix();

	glPushMatrix();

	snprintf( buffer, sizeof(buffer), "%d", (int) *heading_mag_ap );
  
	m_pFontManager->SetSize(m_Font, bigFontSize, bigFontSize );
	m_pFontManager->Print(30, 1 ,&buffer[0], m_Font ); 


	m_pFontManager->SetSize(m_Font, littleFontSize, littleFontSize );
	m_pFontManager->Print(45, 1 ,"H", m_Font ); 

	glColor3ub(0, 255, 82);
  
	m_pFontManager->SetSize(m_Font, littleFontSize, littleFontSize );
	m_pFontManager->Print(70, 1 ,"MAG", m_Font ); 

	glPopMatrix();

      }


      if (*ap_course1 != FLT_MISS) {

	glPushMatrix();     

	// translate to center of drawing area
	glTranslatef(centerX, centerY, 0);

	// rotate to dialed NAV1/ADF1 Heading
	glRotatef((*ap_course1 - *heading_mag) * indicatorDegreesPerTrueDegrees, 0, 0, -1);
	
	glColor3ub( 255, 255,  255 );
	glLineWidth(3.0);
	
	// draw Heading arrow
	glBegin(GL_LINES);
	glVertex2f(0,0);
	glVertex2f(0,radius-4);
	glEnd();
	
	glBegin(GL_POLYGON);
	glVertex2f(0,0.8*radius);
	glVertex2f(-2,0.75*radius);
	glVertex2f(2,0.75*radius);
	glEnd();
	
	glBegin(GL_LINE_LOOP);
	glVertex2f(0,0.8*radius);
	glVertex2f(-2,0.75*radius);
	glVertex2f(2,0.75*radius);
	glVertex2f(0,0.8*radius);
	glEnd();

	glPopMatrix();
	
      }

    }

  }

} // end namespace OpenGC

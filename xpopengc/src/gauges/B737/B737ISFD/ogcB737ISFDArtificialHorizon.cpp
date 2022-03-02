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
#include <math.h>

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

    /* vertical center of gauge */
    float center = m_PhysicalSize.y/2.0 - m_PhysicalSize.y * 0.07;

    /* vertical center of bank marking circle and radius */
    float bankcenter = m_PhysicalSize.y / 3.0;
    float bankradius = m_PhysicalSize.x / 1.475;
    
    // First, store the "root" position of the gauge component
    glMatrixMode(GL_MODELVIEW);
   
    int acf_type = m_pDataSource->GetAcfType();
    
    float *pitch = link_dataref_flt("sim/flightmodel/position/theta",-1);
    float *roll = link_dataref_flt("sim/flightmodel/position/phi",-1);

    int *nav1_CDI = link_dataref_int("sim/cockpit/radios/nav1_CDI");   
    int *nav1_horizontal = link_dataref_int("sim/cockpit2/radios/indicators/nav1_display_horizontal");
    int *nav1_vertical = link_dataref_int("sim/cockpit2/radios/indicators/nav1_display_vertical");
    float *nav1_hdef = link_dataref_flt("sim/cockpit/radios/nav1_hdef_dot",-2);   
    float *nav1_vdef = link_dataref_flt("sim/cockpit/radios/nav1_vdef_dot",-2);   

    float *isfd_horizontal;
    if ((acf_type == 2) || (acf_type == 3)) {
      isfd_horizontal = link_dataref_flt("laminar/B738/gauges/standby_app_horz",0);
    } else {
      isfd_horizontal = link_dataref_flt("xpserver/isfd/app_horizontal",0);
    }
    float *isfd_vertical;
    if ((acf_type == 2) || (acf_type == 3)) {
      isfd_vertical = link_dataref_flt("laminar/B738/gauges/standby_app_vert",0);
    } else {
      isfd_vertical = link_dataref_flt("xpserver/isfd/app_vertical",0);
    }

    if ((*roll != FLT_MISS) && (*pitch != FLT_MISS)) {

      glPushMatrix();
      glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
      
      // Move to the center of the window
      glTranslatef(m_PhysicalSize.x/2,center,0);

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


      // The dividing line between sky and ground
      glBegin(GL_LINES);
      glVertex2f(-300,0);
      glVertex2f(300,0);
      glEnd();

      for (float s=-1.0;s<=1.0;s=s+2.0) {
	for (float d=2.5;d<=50.0;d=d+2.5) {
	  if (remainder(d,10.0) == 0.0) {
	    glLineWidth(lineWidth);
	    glBegin(GL_LINES);
	    glVertex2f(-20.0*scalex,s*d*2.0*scaley);
	    glVertex2f(20.0*scalex,s*d*2.0*scaley);
	    glEnd();
	    snprintf(buffer, sizeof(buffer), "%i", (int) d);
	    m_pFontManager->Print(-29.5*scalex,s*d*2.0*scaley-2.0,&buffer[0],m_Font);
	  } else if (remainder(d,5.0) == 0.0) {
	    glLineWidth(lineWidth);
	    glBegin(GL_LINES);
	    glVertex2f(-10.0*scalex,s*d*2.0*scaley);
	    glVertex2f(10.0*scalex,s*d*2.0*scaley);
	    glEnd();
	  } else {
	    glLineWidth(lineWidth);
	    glBegin(GL_LINES);
	    glVertex2f(-5.0*scalex,s*d*2.0*scaley);
	    glVertex2f(5.0*scalex,s*d*2.0*scaley);
	    glEnd();
	  }
	}
      }


      glPopMatrix();

      //-----The background behind the bank angle markings-------
      // Reset the modelview matrix
      glPushMatrix();
	
      // Draw in the sky color
      glColor3ub(COLOR_SKY);
	
      aCircle.SetOrigin(m_PhysicalSize.x/2.0,bankcenter);
      aCircle.SetRadius(bankradius);
      aCircle.SetDegreesPerPoint(5);
      aCircle.SetArcStartEnd(300.0,360.0);
	
      glBegin(GL_TRIANGLE_FAN);
      glVertex2f(0,m_PhysicalSize.y);
      glVertex2f(0,bankcenter);
      aCircle.Evaluate();
      glVertex2f(m_PhysicalSize.x/2.0,m_PhysicalSize.y);
      glEnd();
	
      aCircle.SetArcStartEnd(0.0,60.0);
      glBegin(GL_TRIANGLE_FAN);
      glVertex2f(m_PhysicalSize.x,m_PhysicalSize.y);
      glVertex2f(m_PhysicalSize.x/2.0,m_PhysicalSize.y);
      aCircle.Evaluate();
      glVertex2f(m_PhysicalSize.x,bankcenter);
      glEnd();
	
      glPopMatrix();
	
    } else {
	
      /* no data available */
	
      glPushMatrix();
	
      glColor3ub(COLOR_ORANGE);
      glLineWidth(lineWidth);
	
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

    glLineWidth(lineWidth);

    // Draw the center detent
    glBegin(GL_POLYGON);
    glVertex2f(m_PhysicalSize.x/2,m_PhysicalSize.y-4.5);
    glVertex2f(m_PhysicalSize.x/2-3.5f,m_PhysicalSize.y);
    glVertex2f(m_PhysicalSize.x/2+3.5f,m_PhysicalSize.y);
    glVertex2f(m_PhysicalSize.x/2,m_PhysicalSize.y-4.5);
    glEnd();

    // Move to the center of the bank circle
    glTranslatef(m_PhysicalSize.x/2,bankcenter,0);

    glRotatef(10.0,0,0,1);
    glBegin(GL_LINES);
    glVertex2f(0,bankradius);
    glVertex2f(0,bankradius+3);
    glEnd();
  
    glRotatef(10.0,0,0,1);
    glBegin(GL_LINES);
    glVertex2f(0,bankradius);
    glVertex2f(0,bankradius+3);
    glEnd();

    glRotatef(10.0,0,0,1);
    glBegin(GL_LINES);
    glVertex2f(0,bankradius);
    glVertex2f(0,bankradius+6);
    glEnd();

    glRotatef(15.0,0,0,1);
    glBegin(GL_LINES);
    glVertex2f(0,bankradius);
    glVertex2f(0,bankradius+3);
    glEnd();

    glRotatef(15.0,0,0,1);
    glBegin(GL_LINES);
    glVertex2f(0,bankradius);
    glVertex2f(0,bankradius+6);
    glEnd();

    glPopMatrix();

    // Right side bank markings
    // Reset the modelview matrix
    glPushMatrix();
      
    // Move to the center of the bank circle
    glTranslatef(m_PhysicalSize.x/2,bankcenter,0);

    glRotatef(-10.0,0,0,1);
    glBegin(GL_LINES);
    glVertex2f(0,bankradius);
    glVertex2f(0,bankradius+3);
    glEnd();
  
    glRotatef(-10.0,0,0,1);
    glBegin(GL_LINES);
    glVertex2f(0,bankradius);
    glVertex2f(0,bankradius+3);
    glEnd();

    glRotatef(-10.0,0,0,1);
    glBegin(GL_LINES);
    glVertex2f(0,bankradius);
    glVertex2f(0,bankradius+6);
    glEnd();

    glRotatef(-15.0,0,0,1);
    glBegin(GL_LINES);
    glVertex2f(0,bankradius);
    glVertex2f(0,bankradius+3);
    glEnd();

    glRotatef(-15.0,0,0,1);
    glBegin(GL_LINES);
    glVertex2f(0,bankradius);
    glVertex2f(0,bankradius+6);
    glEnd();

    glPopMatrix();

    //----------------End Draw Bank Markings----------------


    //----------------Bank Indicator----------------
    // Reset the modelview matrix
    glPushMatrix();
    // Move to the center of the bank circle
    glTranslatef(m_PhysicalSize.x/2,bankcenter,0);
    // Rotate based on the bank
    glRotatef(*roll, 0, 0, 1);

    // Draw in white
    // Specify line width
    glLineWidth(lineWidth);

    glColor3ub(COLOR_BLACK);
    glBegin(GL_POLYGON); // the top triangle
    glVertex2f(-3.5, bankradius-4.5);
    glVertex2f(0, bankradius);
    glVertex2f(3.5, bankradius-4.5);
    glEnd();
    glColor3ub(COLOR_WHITE);
    glBegin(GL_LINE_LOOP); // the top triangle
    glVertex2f(-3.5, bankradius-4.5);
    glVertex2f(0, bankradius);
    glVertex2f(3.5, bankradius-4.5);
    glEnd();

    glPopMatrix();
    //--------------End draw bank indicator------------

    //----------------Attitude Indicator----------------
    // Reset the modelview matrix
    glPushMatrix();
    // Move to the center of the window
    glTranslatef(m_PhysicalSize.x/2,center,0);

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
    glVertex2f(-18,1.25);
    glVertex2f(-5,1.25);
    glVertex2f(-5,-1.25);
    glVertex2f(-18,-1.25);
    glEnd();
    glColor3ub(COLOR_BLACK);
    glBegin(GL_POLYGON);
    glVertex2f(-5,1.25);
    glVertex2f(-5,-5.75);
    glVertex2f(-8,-5.75);
    glVertex2f(-8,1.25);
    glEnd();
 
    // White lines
    glColor3ub(COLOR_WHITE);
    glLineWidth(lineWidth);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-18,1.25);
    glVertex2f(-5,1.25);
    glVertex2f(-5,-5.75);
    glVertex2f(-8,-5.75);
    glVertex2f(-8,-1.25);
    glVertex2f(-18,-1.25);
    glVertex2f(-18,1.25);
    glEnd();

    // The right part
    // Black background
    glColor3ub(COLOR_BLACK);
    glBegin(GL_POLYGON);
    glVertex2f(18,1.25);
    glVertex2f(5,1.25);
    glVertex2f(5,-1.25);
    glVertex2f(18,-1.25);
    glEnd();
    glColor3ub(COLOR_BLACK);
    glBegin(GL_POLYGON);
    glVertex2f(5,1.25);
    glVertex2f(5,-5.75);
    glVertex2f(8,-5.75);
    glVertex2f(8,1.25);
    glEnd();
 
    // White lines
    glColor3ub(COLOR_WHITE);
    glLineWidth(lineWidth);
    glBegin(GL_LINE_LOOP);
    glVertex2f(18,1.25);
    glVertex2f(5,1.25);
    glVertex2f(5,-5.75);
    glVertex2f(8,-5.75);
    glVertex2f(8,-1.25);
    glVertex2f(18,-1.25);
    glVertex2f(18,1.25);
    glEnd();

    glPopMatrix();
    //--------------End draw attitude indicator------------

    //--------------Start draw Horizontal Localizer--------
    if (*isfd_horizontal == 1) {
      // Draw the localizer
    
      // Position of Localizer in ISFD
      float localizer_x = m_PhysicalSize.x/2.0;
      float localizer_y = 3.5;
    
      // Overall localizer size
      float localizerWidth = 9.0; // per Dot
      float localizerHeight = 3.5;

      // Black Background
      glColor3ub(COLOR_BLACK);
      glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
      glBegin(GL_POLYGON);
      glVertex2f(localizer_x + 2.5*localizerWidth, localizer_y + localizerHeight/2.0 );
      glVertex2f(localizer_x - 2.5*localizerWidth, localizer_y + localizerHeight/2.0 );
      glVertex2f(localizer_x - 2.5*localizerWidth, localizer_y - localizerHeight/2.0 );
      glVertex2f(localizer_x + 2.5*localizerWidth, localizer_y - localizerHeight/2.0 );
      glEnd();	  
	
      // Verticalal center line
      glColor3ub(COLOR_WHITE);
      glBegin(GL_LINES);
      glVertex2f( localizer_x, localizer_y + localizerHeight/2.0 );
      glVertex2f( localizer_x, localizer_y - localizerHeight/2.0 );
      glEnd();
    
      // Draw the localizer dots
	
      // Set up the circle
      aCircle.SetRadius(1.0);
      aCircle.SetArcStartEnd(0,360);
      aCircle.SetDegreesPerPoint(30);
    
      glLineWidth(lineWidth);
    
      aCircle.SetOrigin(localizer_x + localizerWidth * 2, localizer_y);
      glBegin(GL_LINE_LOOP);
      aCircle.Evaluate();
      glEnd();
	
      aCircle.SetOrigin(localizer_x + localizerWidth, localizer_y);
      glBegin(GL_LINE_LOOP);
      aCircle.Evaluate();
      glEnd();
	
      aCircle.SetOrigin(localizer_x + -1 * localizerWidth, localizer_y);
      glBegin(GL_LINE_LOOP);
      aCircle.Evaluate();
      glEnd();
	
      aCircle.SetOrigin(localizer_x + -1 * localizerWidth * 2, localizer_y);
      glBegin(GL_LINE_LOOP);
      aCircle.Evaluate();
      glEnd();
	
      if (*nav1_horizontal == 1) {

	// The horizontal offset of the localizer bug
	float localizerPosition = localizer_x + *nav1_hdef * localizerWidth;
	// This is the localizer bug
	glColor3ub(COLOR_VIOLET);
	if (fabs(*nav1_hdef) < 2.49) {
	  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	  glBegin(GL_POLYGON);
	} else {
	  glBegin(GL_LINE_LOOP);
	}
	glVertex2f( localizerPosition - 4, localizer_y ); 	
	glVertex2f( localizerPosition, localizer_y + 2 );
	glVertex2f( localizerPosition + 4, localizer_y );
	glVertex2f( localizerPosition, localizer_y - 2 );
	glEnd();
      }

    }
    //--------------End draw Horizontal Localizer--------

    //--------------Start draw Vertical Glideslope-------
    if (*isfd_vertical == 1) {
      // Draw the glideslope dots
    
      // The height of the glideslope markers above and below center (+/- 1 and 2 degrees deflection)
      float glideslopeWidth = 3.5;
      float glideslopeHeight = 9;
	
      // Horizontal position of the dots relative to the ADI center
      float glideslope_x = m_PhysicalSize.x - 8;
      float glideslope_y = center;

      // Black Background
      glColor3ub(COLOR_BLACK);
      glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
      glBegin(GL_POLYGON);
      glVertex2f(glideslope_x + glideslopeWidth/2.0, glideslope_y + 2.5*glideslopeHeight );
      glVertex2f(glideslope_x - glideslopeWidth/2.0, glideslope_y + 2.5*glideslopeHeight );
      glVertex2f(glideslope_x - glideslopeWidth/2.0, glideslope_y - 2.5*glideslopeHeight );
      glVertex2f(glideslope_x + glideslopeWidth/2.0, glideslope_y - 2.5*glideslopeHeight );
      glEnd();	  
	
      // Horizontal center line
      glColor3ub(COLOR_WHITE);
      glBegin(GL_LINES);
      glVertex2f( glideslope_x - glideslopeWidth/2.0, center );
      glVertex2f( glideslope_x + glideslopeWidth/2.0, center );
      glEnd();
    	
      // Set up the circle
      CircleEvaluator aCircle;
      aCircle.SetRadius(1.0);
      aCircle.SetArcStartEnd(0,360);
      aCircle.SetDegreesPerPoint(30);
    
      glLineWidth(lineWidth);
    
      aCircle.SetOrigin(glideslope_x, glideslope_y + glideslopeHeight * 2);
      glBegin(GL_LINE_LOOP);
      aCircle.Evaluate();
      glEnd();
	
      aCircle.SetOrigin(glideslope_x, glideslope_y + glideslopeHeight);
      glBegin(GL_LINE_LOOP);
      aCircle.Evaluate();
      glEnd();
	
      aCircle.SetOrigin(glideslope_x, glideslope_y + -1 * glideslopeHeight);
      glBegin(GL_LINE_LOOP);
      aCircle.Evaluate();
      glEnd();
	
      aCircle.SetOrigin(glideslope_x, glideslope_y + -1 * glideslopeHeight * 2);
      glBegin(GL_LINE_LOOP);
      aCircle.Evaluate();
      glEnd();
    
      // This is the glideslope bug
      if ((*nav1_vertical == 1) && (*nav1_CDI == 1)) {
	float rawGlideslope = *nav1_vdef;

	if (rawGlideslope < - 2.5) rawGlideslope = -2.5;
	if (rawGlideslope >   2.5) rawGlideslope = 2.5;
     
	// The vertical offset of the glideslope bug
	float glideslopePosition = glideslope_y - rawGlideslope * glideslopeHeight;

	// fill the glideslope bug when we are in glide slope color is magenta
	glColor3ub(COLOR_VIOLET);
	if (fabs(rawGlideslope) < 2.49) {
	  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	  glBegin(GL_POLYGON);
	} else {
	  glBegin(GL_LINE_LOOP);
	}
	glVertex2f( glideslope_x - 2, glideslopePosition ); 	
	glVertex2f( glideslope_x, glideslopePosition + 4 );
	glVertex2f( glideslope_x + 2, glideslopePosition );
	glVertex2f( glideslope_x, glideslopePosition - 4 );
	glEnd();	
      }

    }
    //--------------End draw Vertical Glideslope-------
    
  }
  
} // end namespace OpenGC

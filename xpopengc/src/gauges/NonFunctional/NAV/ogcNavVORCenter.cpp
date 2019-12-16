/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcNavVORCenter.cpp,v $

  Copyright (C) 2001-2 by:
    Original author:
      John Wojnaroski
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2003/06/27 04:53:27 $
    Version:   $Revision: 1.2 $
    Author:    $Author: damion $
  
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

// Provides a basic HSI and VOR compass centered display

#include <Source/Base/ogcGLHeaders.h>
#include <stdio.h>

#include <Source/Base/ogcGaugeComponent.h>
#include "ogcNavVORCenter.h"
#include <Source/DataSources/ogcDataSource.h>

#define width 1.75
int timer;

ogcNavVORCenter::ogcNavVORCenter()
{
  printf("ogcNavVORCenter constructed\n");


  m_Font = m_pFontManager->LoadDefaultFont();

  m_PhysicalPosition.x = 0;
  m_PhysicalPosition.y = 0;
  m_PhysicalSize.x = 180;
  m_PhysicalSize.y = 180;
  m_Scale.x = 1.0;
  m_Scale.y = 1.0;

  m_PixelPosition.x = 0;
  m_PixelPosition.y = 0;

  m_PixelSize.x = 0;
  m_PixelSize.y = 0;

  m_pDataSource->New_Ref_Heading = false;
  timer = 0;
}

ogcNavVORCenter::~ogcNavVORCenter()
{
}

void ogcNavVORCenter::Render()
{
  // Call base class to setup viewport and projection
  ogcGaugeComponent::Render();

  double centerX = 90;
  double centerY = 96;
  double radius = 67.0;
  double indicatorDegreesPerTrueDegrees = 1.0;

  //double littleFontSize = 4.5;
  double bigFontSize = 4.5;

  char buffer[10];

  double heading = m_pDataSource->Mag_Heading;
  double angle_off = m_pDataSource->Nav1_Radial - heading;
  double left_bearing = m_pDataSource->Nav1_Bearing - heading;
  double right_bearing = m_pDataSource->Nav2_Bearing - heading;
  double cmd_offset = m_pDataSource->Ref_Heading - heading;

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
 
  glTranslatef(centerX, centerY, 0.0);
  
  // draw in white
  glColor3ub( 255,255,255 );

  // Draw outer edge indecies
  glBegin(GL_LINES);
  glVertex2f( -radius, 0.0 ); glVertex2f( -radius - 6.0, 0.0 );
  glVertex2f( radius, 0.0 ); glVertex2f( radius + 6.0, 0.0 );

  glVertex2f( (-radius-6.0) * 0.707106, (-radius-6.0) * 0.707106 );
  glVertex2f( -radius * 0.707106, -radius * 0.707106 );

  glVertex2f( radius * 0.707106, radius * 0.707106 );
  glVertex2f( (radius+6) * 0.707106, (radius+6) * 0.707106 );

  glVertex2f( radius * 0.707106, -radius * 0.707106 );
  glVertex2f( (radius+6) * 0.707106, (-radius-6) * 0.707106 );

  glVertex2f( -radius * 0.707106, radius * 0.707106 );
  glVertex2f( (-radius-6) * 0.707106, (radius+6) * 0.707106 );

  glEnd();

  glPushMatrix();

  // Figure out the nearest heading that's a multiple of 10
  double nearestTen = (double)( (int)heading - (int)heading % 10);

  // Derotate by this offset
  glRotated( -1.0*(heading - nearestTen) * indicatorDegreesPerTrueDegrees,0,0,-1);
  
  // Now derotate by 180 "virtual" degrees
  glRotated(-180*indicatorDegreesPerTrueDegrees,0,0,-1);

  // Now draw lines 5 virtual degrees apart around the perimeter of the circle  
  for(int i = (int)nearestTen - 180; i <= (int)nearestTen+180; i+=5)
  {
    // The length of the tickmarks on the compass rose
    double tickLength;

    // Make sure the display heading is between 0 and 360
    int displayHeading = (i+720)%360;

    // If the heading is a multiple of ten, it gets a long tick
    if(displayHeading%10==0)
    {
      tickLength = 5.5;
	    double fontx;
	    if( displayHeading%30==0)
	    {
	      sprintf( buffer, "%2.0f", displayHeading * 0.1);
	      if(displayHeading>=100)
          fontx = -bigFontSize;
        else
          fontx = -bigFontSize/2;

        m_pFontManager->SetSize(m_Font, bigFontSize, bigFontSize);
        m_pFontManager->Print(fontx, radius-tickLength-6.7, &buffer[0], m_Font );
	    }
    }
    else
      tickLength = 2.0;

    glLineWidth( 2.5 );
    glBegin(GL_LINES);
    glVertex2f(0,radius);
    glVertex2f(0,radius-tickLength);
    glEnd();

    glRotated(5.0 * indicatorDegreesPerTrueDegrees,0,0,-1);
  }

  glPopMatrix();

  // Draw the nose and tail of the left bearing pointer
  // Need to add a check if EFIS switch is in VOR or ADF position green=vor blue=adf
 
  glLineWidth( 3.0 );

  glRotated( left_bearing, 0, 0, -1 );

  if ( m_pDataSource->Adf1_Valid )
    glColor3ub( 10, 100, 255 );
  else
    glColor3ub( 0, 255, 100);

  glBegin(GL_LINES);
  glVertex2f( 0, 60 );
  glVertex2f( 0, 72 );
  glVertex2f( 0, -60 );
  glVertex2f( 0, -radius );
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f( -3.5, 67 );
  glVertex2f( 0, 72 );
  glVertex2f( 3.5, 67 );
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f( -3.5, -72 );
  glVertex2f( 0, -radius );
  glVertex2f( 3.5, -72 );
  glEnd();

  glRotated( -left_bearing, 0, 0, -1 );

  glLineWidth( 2.0 );
  glRotated( right_bearing, 0, 0, -1 );

  if ( m_pDataSource->Adf2_Valid )
    glColor3ub( 10, 100, 255);
  else
    glColor3ub( 0, 255, 100);
  
  glBegin(GL_LINE_LOOP);
  glVertex2f( -2, 60 );
  glVertex2f( 2, 60 );
  glVertex2f( 2, 67 );
  glVertex2f( 0, 72 );
  glVertex2f( -2, 67 );
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex2f( -2, -60 );
  glVertex2f( -2, -72 );
  glVertex2f( 0, -67 );
  glVertex2f( 2, -72 );
  glVertex2f( 2, -60 );
  glEnd();

  glRotated( -right_bearing , 0, 0, -1 );
  glColor3ub( 255,255,255 );
  
  // Draw the CDI /////////////////////////////////

  double cdi_dev = (m_pDataSource->Nav1_Course_Indicator * 3) - 2.0;
  glRotated( angle_off, 0, 0, -1 );

  // Draw the course arrow 
  glBegin(GL_LINE_LOOP);
  glVertex2f( -2.0, 30.0 );
  glVertex2f( -2.0, 61.0 );
  glVertex2f( 0.0, 65.0 );
  glVertex2f( 2.0, 61.0 );
  glVertex2f( 2.0, 30.0 );
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex2f( -2.0, -30.0 );
  glVertex2f( -2.0, -61.0 );
  glVertex2f( 2.0, -61.0 );
  glVertex2f( 2.0, -30.0 );
  glEnd();
 
  // Now draw the CDI and TO/FROM
  glBegin(GL_LINE_LOOP);
  glVertex2f( cdi_dev, 30.0 );
  glVertex2f( cdi_dev, -30.0 );
  glVertex2f( cdi_dev+4.0, -30.0 );
  glVertex2f( cdi_dev+4.0, 30.0 );
  glEnd();

  if ( m_pDataSource->To_From_Flag > 0 )
  {
	  glBegin(GL_LINE_LOOP);
    glVertex2f( 0,29 );
	  glVertex2f( -2.0, 25 );
    glVertex2f( 2.0, 25 );
	  glEnd();
	}

  if ( m_pDataSource->To_From_Flag < 0 )
  {
	  glBegin(GL_LINE_LOOP);
    glVertex2f( 0,-29 );
	  glVertex2f( -2.0, -25 );
    glVertex2f( 2.0, -25 );
	  glEnd();
  }

  m_pFontManager->Print( -30.73, -1.43, "o       o       .       o       o", m_Font );
  glRotated( -angle_off, 0, 0, -1 );
  
  glColor3ub( 230,40,230);
  glLineWidth( 2.3 );
  glRotated( cmd_offset, 0, 0, -1);
  
  glBegin(GL_LINE_LOOP);
  glVertex2f( -3.5, 67 );
  glVertex2f( 3.5, 67 );
  glVertex2f( 3.5, 63.0 );
  glVertex2f( -3.5, 63.0 );
  glEnd();

  if ( m_pDataSource->New_Ref_Heading )
  {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple( 2, 0x0F0F );
    
    glBegin(GL_LINES);
    glVertex2f( 0, 0 );
    glVertex2f( 0, 63.0 );
    glEnd();
    glRotated( -cmd_offset, 0 ,0 , 1);
    
    if (timer++ > 200)
    {
	    m_pDataSource->New_Ref_Heading = false;
	    timer = 0;
    }
    glDisable(GL_LINE_STIPPLE);
  }
  
  glPopMatrix();

  // Now draw the aircraft symbol which does not rotate

  glColor3ub( 255,255,255 );
  glBegin(GL_LINES);
  glVertex2f( centerX-width, centerY-7);
  glVertex2f( centerX-width, centerY+7);
  glVertex2f( centerX+width, centerY-7);
  glVertex2f( centerX+width, centerY+7);
  glVertex2f( centerX-5, centerY-7);
  glVertex2f( centerX-width, centerY-7);
  glVertex2f( centerX-10, centerY);
  glVertex2f( centerX-width, centerY);
  glVertex2f( centerX+5, centerY-7);
  glVertex2f( centerX+width, centerY-7);
  glVertex2f( centerX+10, centerY);
  glVertex2f( centerX+width, centerY);
  glEnd();
}
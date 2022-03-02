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
#include "ogcB737ISFD.h"
#include "ogcB737ISFDSpeedTape.h"

namespace OpenGC
{

  B737ISFDSpeedTape::B737ISFDSpeedTape()
  {
    printf("B737SpeedTape constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 16; // make the clip region larger to handle speed bug
    m_PhysicalSize.y = 100;
    indent_x = m_PhysicalSize.x;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  B737ISFDSpeedTape::~B737ISFDSpeedTape()
  {

  }

  void B737ISFDSpeedTape::Render()
  {
    // Call base class to setup viewport and projection
    GaugeComponent::Render();
  
    // indicated air speed (knots)
    float *speed_knots = link_dataref_flt("sim/flightmodel/position/indicated_airspeed",-1);
          
    if (*speed_knots != FLT_MISS) {

      // Speed for integer calculations
      int ias_int = (int) *speed_knots;
      float ias_flt = *speed_knots;
      
      // The speed tape doesn't display speeds < 30 or > 999
      /* if(ias_flt < 10.0)
	 {
	 ias_flt = 10.0;
	 ias_int = 10;
	 } */
  
      if(ias_flt > 999.0)
	{
	  ias_flt = 999.0;
	  ias_int = 999;
	}

      // Save matrix
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      // Draw in gray-blue
      glColor3ub(COLOR_GRAY35);

      // Draw the background rectangle
      glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
      glBegin(GL_POLYGON);
      glVertex2f(0.0, 0.0);
      glVertex2f(0.0, m_PhysicalSize. y);
      glVertex2f(indent_x, m_PhysicalSize.y);
      glVertex2f(indent_x, 0.0);
      glVertex2f(0.0, 0.0);
      glEnd();
  
      // Tick marks are space every 10 kph vertically
      // The tick spacing represents how far apart they are in physical
      // units
      float tickSpacing = 9.0;
      float tickWidth = 1.5;
      float fontHeight = 4;
      float fontWidth = 4;
      float fontIndent = 1.5;
      float lineWidth = 3.0;

      // Vertical Center is shifted by 5%
      float shifty = m_PhysicalSize.y * 0.05;

      m_pFontManager->SetSize(m_Font, fontHeight, fontWidth);

      char buffer[10];
      char buffer2[10];
      memset(buffer,0,sizeof(buffer));
      memset(buffer2,0,sizeof(buffer));

      int nextHighestIAS = (ias_int/10)*10;

      if (nextHighestIAS < ias_int)
	nextHighestIAS += 10;

      // The vertical offset is how high in physical units the next highest 10's
      // airspeed is above the arrow. For airspeeds divisible by 10, this is 0. I.e. 120, 130
      // etc. are all aligned with the arrow
      float vertOffset = (float) nextHighestIAS - ias_flt;
  
      // Vertical location of the tick mark
      float tickLocation = 0;

      glColor3ub(COLOR_WHITE);
      glLineWidth(lineWidth);

      float i = 0; // counter
      int tickSpeed; // speed represented by tick mark
      int charSpd; // speed for computing text digits

      // Draw ticks up from the center
      for (i = 0; i <= ((m_PhysicalSize.y/2) / tickSpacing) + 1; i += 1.0)
	{
	  tickSpeed = nextHighestIAS + (int)(i*10.0);
	  tickLocation = (m_PhysicalSize.y/2) + i*tickSpacing+vertOffset - shifty;
    
	  // do not draw negative tick speeds
	  if (tickSpeed >= 0) {
    
	    if( (tickSpeed % 20)==0 ) {
	      glLineWidth(lineWidth);
	      glBegin(GL_LINES);
	      glVertex2f(indent_x - tickWidth, tickLocation);
	      glVertex2f(indent_x, tickLocation);
	      glEnd();
	    } else {
	      glLineWidth(lineWidth);
	      glBegin(GL_LINES);
	      glVertex2f(indent_x - tickWidth*2.0, tickLocation);
	      glVertex2f(indent_x, tickLocation);
	      glEnd();
	    }

	    if( (tickSpeed % 20)==0 )
	      {
		float texty = tickLocation - fontHeight / 2;

		charSpd = tickSpeed;

		if(charSpd >= 100)
		  {
		    // 100's
		    //_itoa( charSpd / 100, buffer, 10);
		    snprintf( buffer, sizeof(buffer), "%i", charSpd/100);
		    m_pFontManager->Print(fontIndent, texty, &buffer[0], m_Font);
		    charSpd = charSpd-100*(int)(charSpd/100);
	    
		    // draw second digit if tickSpeed is multiplier of 100
		    if (charSpd == 0) {
		      snprintf( buffer, sizeof(buffer), "%i", charSpd/10);
		      m_pFontManager->Print(fontIndent + fontWidth, texty, &buffer[0], m_Font);
		    }
		  }

		if(charSpd >= 10)
		  {
		    // 10's
		    //_itoa( charSpd / 10, buffer, 10);
		    snprintf(buffer, sizeof(buffer), "%i", charSpd/10);
		    m_pFontManager->Print(fontIndent + fontWidth, texty, &buffer[0], m_Font);
		    charSpd = charSpd-10*(int)(charSpd/10);
		  }
	
		snprintf(buffer, sizeof(buffer), "%i", 0);
		m_pFontManager->Print(fontIndent + fontWidth*2, texty, &buffer[0], m_Font);
	      }
    
	  }

	}

      // Draw ticks down from the center
      for (i = 1; i <= ((m_PhysicalSize.y/2) / tickSpacing) + 1; i += 1.0)
	{
	  tickSpeed = (int) nextHighestIAS - i*10;
	  
	  // Only draw ticks if they are positive
	  if( tickSpeed >= 0)
	    {
	      tickLocation = (m_PhysicalSize.y/2) - ( (i-1) * tickSpacing) - (10 - vertOffset) - shifty;

	      glLineWidth(lineWidth);
	      glBegin(GL_LINES);
	      glVertex2f(indent_x - tickWidth, tickLocation);
	      glVertex2f(indent_x, tickLocation);
	      glEnd();

	      if( (tickSpeed % 20)==0 )
		{
		  float texty = tickLocation - fontHeight / 2;

		  charSpd = tickSpeed;

		  if(charSpd >= 100)
		    {
		      // 100's
		      //_itoa( charSpd / 100, buffer, 10);
		      snprintf( buffer, sizeof(buffer), "%i", charSpd/100);
		      m_pFontManager->Print(fontIndent, texty, &buffer[0], m_Font);
		      charSpd = charSpd-100*(int)(charSpd/100);

		      // draw second digit if tickSpeed is multiplier of 100
		      if (charSpd == 0) {
			snprintf( buffer, sizeof(buffer), "%i", charSpd/10);
			m_pFontManager->Print(fontIndent + fontWidth, texty, &buffer[0], m_Font);
		      }
		    }

		  if (charSpd >=10)
		    {
		      // 10's
		      //_itoa( charSpd / 10, buffer, 10);
		      snprintf( buffer, sizeof(buffer), "%i", charSpd/10);
		      m_pFontManager->Print(fontIndent + fontWidth, texty, &buffer[0], m_Font);
		      charSpd = charSpd-10*(int)(charSpd/10);
		    }

		  snprintf( buffer, sizeof(buffer), "%i", 0);
		  m_pFontManager->Print(fontIndent + fontWidth*2, texty, &buffer[0], m_Font);
		}
	    }
	}
      
    }
      
    glPopMatrix();
    
  }
  
} // end namespace OpenGC

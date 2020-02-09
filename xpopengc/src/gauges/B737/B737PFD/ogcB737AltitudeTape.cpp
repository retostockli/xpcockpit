/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737AltitudeTape.cpp,v $

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

#include <stdio.h>
#include <math.h>

#include "ogcGaugeComponent.h"
#include "ogcB737AltitudeTape.h"

namespace OpenGC
{

  B737AltitudeTape::B737AltitudeTape()
  {
    printf("B737AltitudeTape constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 24;
    m_PhysicalSize.y = 136;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  B737AltitudeTape::~B737AltitudeTape()
  {

  }

  void B737AltitudeTape::Render()
  {
    // Call base class to setup viewport and projection
    GaugeComponent::Render();

    int acf_type = m_pDataSource->GetAcfType();

    // Get the barometric altitude (feet)
    float *pressure_altitude = link_dataref_flt("sim/flightmodel/misc/h_ind",0);

    // Altitude above ground level (meters)
    float *altitude_agl = link_dataref_flt("sim/flightmodel/position/y_agl",0);
    
    // Autopilot altitude (feet)
    float *ap_altitude;
    if (acf_type == 1) {
      ap_altitude = link_dataref_flt("x737/systems/afds/ALTHLD_baroalt",0);
    } else {
      ap_altitude = link_dataref_flt("sim/cockpit/autopilot/altitude",0); 
    }
    
    if (*pressure_altitude != FLT_MISS) {

      int alt = (int) *pressure_altitude;

      // Save matrix
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      // Draw in gray-blue
      glColor3ub(51,51,76);

      // Draw the background rectangle
      glBegin(GL_POLYGON);
      glVertex2f(0.0, 0.0);
      glVertex2f(0.0, m_PhysicalSize. y);
      glVertex2f(m_PhysicalSize.x, m_PhysicalSize.y);
      glVertex2f(m_PhysicalSize.x, 0.0);
      glVertex2f(0.0, 0.0);
      glEnd();
  
      // Tick marks are spaced every 100 ft. vertically
      // The tick spacing represents how far apart they are in physical
      // units
      float tickSpacing = 17.0;
      float tickWidth = 3.7;
      float fontHeight = 4;
      float fontWidth = 3.5;
      float fontIndent = 4.5;

      m_pFontManager->SetSize(m_Font, fontHeight, fontWidth);

      char buffer[2];
      memset(buffer,0,sizeof(buffer));

      int nextHighestAlt = (alt/100)*100;
      
      if (nextHighestAlt < alt)
	nextHighestAlt += 100;

      // The vertical offset is how high in physical units the next highest 100's
      // altitude is above the arrow
      float vertOffset = ( (float)nextHighestAlt - (float)alt)/100.0*tickSpacing;

      
      // Vertical location of the tick mark
      float tickLocation = 0;

      glColor3ub(255,255,255);
      glLineWidth(2.0);

      float i = 0; // counter
      int tickAlt; // speed represented by tick mark
      int charAlt; // speed for computing text digits

      // Draw ticks up from the center
      for (i = 0; i <= ((m_PhysicalSize.y/2) / tickSpacing) + 1; i += 1.0)
	{
	  tickAlt = nextHighestAlt + (int)(i*100.0);
	  tickLocation = (m_PhysicalSize.y/2) + i*tickSpacing + vertOffset;
	  float texty = tickLocation - fontHeight / 2;
	  
	  glLineWidth(2.0);
	  glBegin(GL_LINES);
	  glVertex2f(0, tickLocation);
	  glVertex2f(tickWidth, tickLocation);
	  glEnd();

	  if( (tickAlt % 200)==0 )
	    {
	      charAlt = tickAlt;
	      bool hunk = false;
	      bool tenk = false;
	      bool onek = false;
	      bool zero = true;
      
	      // 100000's
	      if(abs(charAlt) >= 100000)
		{
		  snprintf( buffer, sizeof(buffer), "%i", abs(charAlt)/100000);
		  m_pFontManager->Print(fontIndent - fontWidth, texty, buffer, m_Font);
		  charAlt = charAlt-100000*(int)(charAlt/100000);
		  
		  hunk = true;
		  zero = false;
		}
	      
	      // 10000's
	      if(abs(charAlt) >= 10000)
		{
		  snprintf( buffer, sizeof(buffer), "%i", abs(charAlt)/10000);
		  m_pFontManager->Print(fontIndent, texty, buffer, m_Font);
		  charAlt = charAlt-10000*(int)(charAlt/10000);
		  
		  tenk = true;
		  zero = false;
		}
	      else if(hunk)
		{
		  buffer[0] = '0';
		  m_pFontManager->Print(fontIndent, texty, &buffer[0], m_Font);
		}

	      // 1000's
	      if(abs(charAlt) >= 1000)
		{
		  snprintf( buffer, sizeof(buffer), "%i", abs(charAlt)/1000);
		  m_pFontManager->Print(fontIndent + fontWidth, texty, &buffer[0], m_Font);
		  if (charAlt < 0) {
		    buffer[0] = '-';
		    m_pFontManager->Print(fontIndent, texty, &buffer[0], m_Font);
		  }
		  charAlt = charAlt-1000*(int)(charAlt/1000);

		  onek = true;
		  zero = false;
		}
	      else if(hunk || tenk)
		{
		  buffer[0] = '0';
		  m_pFontManager->Print(fontIndent + fontWidth, texty, &buffer[0], m_Font);
		}

	      // 100's
	      if(abs(charAlt) >= 100)
		{
		  snprintf( buffer, sizeof(buffer), "%i", abs(charAlt)/100);
		  m_pFontManager->Print(fontIndent + fontWidth*2, texty, &buffer[0], m_Font);
		  if ((charAlt < 0) && (!onek)) {
		    buffer[0] = '-';
		    m_pFontManager->Print(fontIndent + fontWidth, texty, &buffer[0], m_Font);
		  }
		  charAlt = charAlt-100*(int)(charAlt/100);
		  zero = false;
		}
	      else if(hunk || tenk || onek)
		{
		  buffer[0] = '0';
		  m_pFontManager->Print(fontIndent + fontWidth*2, texty, &buffer[0], m_Font);
		}

	      if (!zero) {
		buffer[0] = '0';
		m_pFontManager->Print(fontIndent + fontWidth*3, texty, &buffer[0], m_Font);
	      }

	      buffer[0] = '0';
	      m_pFontManager->Print(fontIndent + fontWidth*4, texty, &buffer[0], m_Font);

	    }
	}

      // Draw ticks down from the center
      for (i = 1; i <= ((m_PhysicalSize.y/2) / tickSpacing) + 1; i += 1.0)
	{
	  tickAlt = nextHighestAlt - (int)(i*100);
	  tickLocation = (m_PhysicalSize.y/2) - ( (i-1) * tickSpacing) - (tickSpacing - vertOffset);
	  float texty = tickLocation - fontHeight / 2;

	  glLineWidth(2.0);
	  glBegin(GL_LINES);
	  glVertex2f(0, tickLocation);
	  glVertex2f(tickWidth, tickLocation);
	  glEnd();

	  if( (tickAlt % 200)==0 )
	    {
	      charAlt = tickAlt;
	      bool hunk = false;
	      bool tenk = false;
	      bool onek = false;
	      bool zero = true;
      
	      // 100000's
	      if(abs(charAlt) >= 10000)
		{
		  //_itoa( charAlt / 10000, buffer, 10);
		  snprintf(buffer, sizeof(buffer), "%i", abs(charAlt)/100000);
		  m_pFontManager->Print(fontIndent - fontWidth, texty, buffer, m_Font);
		  charAlt = charAlt-100000*(int)(charAlt/100000);

		  hunk = true;
		  zero = false;
		}
	      
	      // 10000's
	      if(abs(charAlt) >= 10000)
		{
		  //_itoa( charAlt / 10000, buffer, 10);
		  snprintf(buffer, sizeof(buffer), "%i", abs(charAlt)/10000);
		  m_pFontManager->Print(fontIndent, texty, buffer, m_Font);
		  charAlt = charAlt-10000*(int)(charAlt/10000);

		  tenk = true;
		  zero = false;
		}
	      else if(hunk)
		{
		  buffer[0] = '0';
		  m_pFontManager->Print(fontIndent, texty, &buffer[0], m_Font);
		}

	      // 1000's
	      if(abs(charAlt) >= 1000)
		{
		  snprintf(buffer, sizeof(buffer), "%i", abs(charAlt)/1000);
		  m_pFontManager->Print(fontIndent + fontWidth, texty, &buffer[0], m_Font);
		  if (charAlt < 0) {
		    buffer[0] = '-';
		    m_pFontManager->Print(fontIndent, texty, &buffer[0], m_Font);
		  }
		  charAlt = charAlt-1000*(int)(charAlt/1000);

		  onek = true;
		  zero = false;
		}
	      else if(hunk || tenk)
		{
		  buffer[0] = '0';
		  m_pFontManager->Print(fontIndent + fontWidth, texty, &buffer[0], m_Font);
		}

	      // 100's
	      if(abs(charAlt) >= 100)
		{
		  snprintf(buffer, sizeof(buffer), "%i", abs(charAlt)/100);
		  m_pFontManager->Print(fontIndent + fontWidth*2, texty, &buffer[0], m_Font);
		  if ((charAlt < 0) && (!onek)) {
		    buffer[0] = '-';
		    m_pFontManager->Print(fontIndent + fontWidth, texty, &buffer[0], m_Font);
		  }
		  charAlt = charAlt-100*(int)(charAlt/100);
		  zero = false;
		}
	      else if(hunk || tenk || onek)
		{
		  buffer[0] = '0';
		  m_pFontManager->Print(fontIndent + fontWidth*2, texty, &buffer[0], m_Font);
		}

	      if (!zero) {
		buffer[0] = '0';
		m_pFontManager->Print(fontIndent + fontWidth*3, texty, &buffer[0], m_Font);
	      }

	      buffer[0] = '0';
	      m_pFontManager->Print(fontIndent + fontWidth*4, texty, &buffer[0], m_Font);
	    }
	}

      if ((*altitude_agl != FLT_MISS) && (*altitude_agl > 5.0)) {
	float aglaltLocation = (-1.0 * *altitude_agl * 3.28084) *
	  tickSpacing / 100.0 + m_PhysicalSize.y/2;
	glColor3ub( 200, 200,  0 );
	glLineWidth(3.0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0.0, aglaltLocation);
	glVertex2f(m_PhysicalSize.x, aglaltLocation);
	glEnd();
	for (int i=-2;i<10;i++) {
	  glBegin(GL_LINE_LOOP);
	  glVertex2f(i*m_PhysicalSize.x/10.0, aglaltLocation);
	  glVertex2f((i+1)*m_PhysicalSize.x/10.0, aglaltLocation-tickSpacing*0.4);
	  glEnd();
	}
      }
      
      // draw MCP dialed altitude if within the altitude tape range
      if (*ap_altitude != FLT_MISS) {
	float mcpaltLocation = (*ap_altitude - *pressure_altitude) *
	  tickSpacing / 100.0 + m_PhysicalSize.y/2;
	
	// draw a magenta MCP altitude indicator
	glColor3ub( 210, 5,  210 );
	glLineWidth(3.0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0.5, mcpaltLocation+tickSpacing*0.2);
	glVertex2f(0.5, mcpaltLocation+tickSpacing*0.6);
	glVertex2f(m_PhysicalSize.x/2, mcpaltLocation+tickSpacing*0.6);
	glVertex2f(m_PhysicalSize.x/2, mcpaltLocation-tickSpacing*0.6);
	glVertex2f(0.5, mcpaltLocation-tickSpacing*0.6);
	glVertex2f(0.5, mcpaltLocation-tickSpacing*0.2);
	glVertex2f(m_PhysicalSize.x/6, mcpaltLocation-tickSpacing*0.05);
	glVertex2f(m_PhysicalSize.x/6, mcpaltLocation+tickSpacing*0.05);
	glEnd();
      }

      /* draw expected altitude change during next 10 seconds */
      /*
      if ((*vertical_speed != FLT_MISS) && (fabs(*vertical_speed) > 100)) {
	float vLocation = *vertical_speed/6.0 * tickSpacing / 100.0 + m_PhysicalSize.y/2;
	
	glColor3ub( 0, 179, 0);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glVertex2f(tickWidth, m_PhysicalSize.y/2);
	glVertex2f(tickWidth/2, m_PhysicalSize.y/2);
	glEnd();	
	glBegin(GL_LINES);
	glVertex2f(tickWidth/2, m_PhysicalSize.y/2);
	glVertex2f(tickWidth/2, vLocation);
	glEnd();
	if (*vertical_speed > 0.0) {
	  glBegin(GL_LINES);
	  glVertex2f(tickWidth/2, vLocation);
	  glVertex2f(tickWidth/2 - tickWidth/4, vLocation - tickWidth/3);
	  glEnd();	
	  glBegin(GL_LINES);
	  glVertex2f(tickWidth/2, vLocation);
	  glVertex2f(tickWidth/2 + tickWidth/4, vLocation - tickWidth/3);
	  glEnd();
	} else {
	  glBegin(GL_LINES);
	  glVertex2f(tickWidth/2, vLocation);
	  glVertex2f(tickWidth/2 - tickWidth/4, vLocation + tickWidth/3);
	  glEnd();	
	  glBegin(GL_LINES);
	  glVertex2f(tickWidth/2, vLocation);
	  glVertex2f(tickWidth/2 + tickWidth/4, vLocation + tickWidth/3);
	  glEnd();
	}
      } 
      */
      
      glPopMatrix();

    }
  }

} // end namespace OpenGC

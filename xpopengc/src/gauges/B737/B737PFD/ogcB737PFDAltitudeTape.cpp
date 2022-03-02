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

#include <stdio.h>
#include <math.h>

#include "ogcGaugeComponent.h"
#include "ogcB737PFD.h"
#include "ogcB737PFDAltitudeTape.h"

namespace OpenGC
{

  B737PFDAltitudeTape::B737PFDAltitudeTape()
  {
    printf("B737PFDAltitudeTape constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 24;
    m_PhysicalSize.y = 136;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  B737PFDAltitudeTape::~B737PFDAltitudeTape()
  {

  }

  void B737PFDAltitudeTape::Render()
  {
    // Call base class to setup viewport and projection
    GaugeComponent::Render();

    int acf_type = m_pDataSource->GetAcfType();
    bool is_captain = (this->GetArg() == 0);

    // Get the barometric altitude (feet)
    float *pressure_altitude;
    if (is_captain) {
      pressure_altitude = link_dataref_flt("sim/flightmodel/misc/h_ind",0);
    } else {
      pressure_altitude = link_dataref_flt("sim/flightmodel/misc/h_ind_copilot",0);
    }

    // Runway Altitude
    float *rwy_altitude;
    if ((acf_type == 2) || (acf_type == 3)) {
      rwy_altitude = link_dataref_flt("laminar/B738/pfd/rwy_altitude",0);
    } else {
      rwy_altitude = link_dataref_flt("xpserver/rwy_altitude",0);
    }
    
    // Autopilot altitude (feet)
    float *ap_altitude;
    if ((acf_type == 2) || (acf_type == 3)) {
      ap_altitude = link_dataref_flt("laminar/B738/autopilot/mcp_alt_dial",0); 
    } else if (acf_type == 1) {
      ap_altitude = link_dataref_flt("x737/systems/afds/ALTHLD_baroalt",0);
    } else {
      ap_altitude = link_dataref_flt("sim/cockpit/autopilot/altitude",0); 
    }
    
    int *alt_disagree;
    if ((acf_type == 2) || (acf_type == 3)) {
      alt_disagree = link_dataref_int("laminar/B738/autopilot/alt_disagree");
    }

    if (*pressure_altitude != FLT_MISS) {

      int alt = (int) *pressure_altitude;
      
      // Save matrix
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      // Draw in gray-blue
      glColor3ub(COLOR_GRAYBLUE);

      // Draw the background rectangle
      glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
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
      float lineWidth = 3.0;

      m_pFontManager->SetSize(m_Font, fontWidth, fontHeight);

      char buffer[10];
      memset(buffer,0,sizeof(buffer));

      int nextHighestAlt = (alt/100)*100;
      
      if (nextHighestAlt < alt)
	nextHighestAlt += 100;

      // The vertical offset is how high in physical units the next highest 100's
      // altitude is above the arrow
      float vertOffset = ( (float)nextHighestAlt - (float)alt)/100.0*tickSpacing;

      
      // Vertical location of the tick mark
      float tickLocation = 0;

      glColor3ub(COLOR_WHITE);
      glLineWidth(lineWidth);

      float i = 0; // counter
      int tickAlt; // speed represented by tick mark
      int charAlt; // speed for computing text digits

      // Draw ticks up from the center
      for (i = 0; i <= ((m_PhysicalSize.y/2) / tickSpacing) + 1; i += 1.0)
	{
	  tickAlt = nextHighestAlt + (int)(i*100.0);
	  tickLocation = (m_PhysicalSize.y/2) + i*tickSpacing + vertOffset;
	  float texty = tickLocation - fontHeight / 2;
	  
	  glLineWidth(lineWidth);
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

	  glLineWidth(lineWidth);
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
		  snprintf(buffer, sizeof(buffer), "%i", abs(charAlt)/100000);
		  m_pFontManager->Print(fontIndent - fontWidth, texty, buffer, m_Font);
		  charAlt = charAlt-100000*(int)(charAlt/100000);

		  hunk = true;
		  zero = false;
		}
	      
	      // 10000's
	      if(abs(charAlt) >= 10000)
		{
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

      if (*rwy_altitude != FLT_MISS) {
	float rwyaltLocation = (*rwy_altitude - *pressure_altitude) *
	  tickSpacing / 100.0 + m_PhysicalSize.y/2;
	glColor3ub(COLOR_YELLOW);
	glLineWidth(3.0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0.0, rwyaltLocation);
	glVertex2f(m_PhysicalSize.x, rwyaltLocation);
	glEnd();
	for (int i=-2;i<10;i++) {
	  glBegin(GL_LINE_LOOP);
	  glVertex2f(i*m_PhysicalSize.x/10.0, rwyaltLocation);
	  glVertex2f((i+1)*m_PhysicalSize.x/10.0, rwyaltLocation-tickSpacing*0.4);
	  glEnd();
	}
      }
      
      // draw MCP dialed altitude if within the altitude tape range
      if (*ap_altitude != FLT_MISS) {
	float mcpaltLocation = (*ap_altitude - *pressure_altitude) *
	  tickSpacing / 100.0 + m_PhysicalSize.y/2;

	// keep MCP altitude within bounds of tape
	mcpaltLocation = fmin(fmax(0.0,mcpaltLocation),m_PhysicalSize.y);
	
	// draw a magenta MCP altitude indicator
	glColor3ub(COLOR_MAGENTA);
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
	
	glColor3ub(COLOR_GREEN);
	glLineWidth(lineWidth);
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

      if ((acf_type == 2) || (acf_type == 3)) {
	if (*alt_disagree == 1) {
	  glColor3ub(COLOR_ORANGE);
	  m_pFontManager->SetSize(m_Font, 0.95*fontWidth, fontHeight);
	  snprintf(buffer, sizeof(buffer), "ALT");
	  m_pFontManager->Print(0.0,2.5*fontHeight, &buffer[0], m_Font);
	  snprintf(buffer, sizeof(buffer), "DISAGREE");
	  m_pFontManager->Print(0.0,1.0*fontHeight, &buffer[0], m_Font);
	}
      }
      
      glPopMatrix();

    }
  }

} // end namespace OpenGC

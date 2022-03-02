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
#include "ogcB737ISFDAltitudeTape.h"

namespace OpenGC
{

  B737ISFDAltitudeTape::B737ISFDAltitudeTape()
  {
    printf("B737ISFDAltitudeTape constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 20;
    m_PhysicalSize.y = 100;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  B737ISFDAltitudeTape::~B737ISFDAltitudeTape()
  {

  }

  void B737ISFDAltitudeTape::Render()
  {
    // Call base class to setup viewport and projection
    GaugeComponent::Render();
   
    int acf_type = m_pDataSource->GetAcfType();
 
    // Get the barometric altitude (feet)
    float *pressure_altitude;
    if ((acf_type == 3) || (acf_type == 2)) {
      pressure_altitude = link_dataref_flt("laminar/B738/gauges/standby_altitude_ft",1);
    } else {
      pressure_altitude = link_dataref_flt("sim/flightmodel/misc/h_ind",0);
    }

    if (*pressure_altitude != FLT_MISS) {

      int alt = (int) *pressure_altitude;
      
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
      glVertex2f(m_PhysicalSize.x, m_PhysicalSize.y);
      glVertex2f(m_PhysicalSize.x, 0.0);
      glVertex2f(0.0, 0.0);
      glEnd();
  
      // Tick marks are spaced every 100 ft. vertically
      // The tick spacing represents how far apart they are in physical
      // units
      float tickSpacing = 10.0;
      float tickWidth = 3.7;
      float fontHeight = 4;
      float fontWidth = 3.5;
      float fontIndent = 1.0;
      float lineWidth = 3.0;

      // Vertical Center is shifted by 5%
      float shifty = m_PhysicalSize.y * 0.05;

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
	  tickLocation = (m_PhysicalSize.y/2) + i*tickSpacing + vertOffset - shifty;
	  float texty = tickLocation - fontHeight / 2;

	  if ((tickAlt % 200)!=0 ) {
	    glLineWidth(lineWidth);
	    glBegin(GL_LINES);
	    glVertex2f(0, tickLocation);
	    glVertex2f(tickWidth, tickLocation);
	    glEnd();
	  }

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
	  tickLocation = (m_PhysicalSize.y/2) - ( (i-1) * tickSpacing) - (tickSpacing - vertOffset) - shifty;
	  float texty = tickLocation - fontHeight / 2;

	  if ((tickAlt % 200)!=0 ) {
	    glLineWidth(lineWidth);
	    glBegin(GL_LINES);
	    glVertex2f(0, tickLocation);
	    glVertex2f(tickWidth, tickLocation);
	    glEnd();
	  }
	    
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
      
      glPopMatrix();

    }
  }

} // end namespace OpenGC

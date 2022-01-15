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
#include "ogcB737PFDSpeedTicker.h"

namespace OpenGC
{

  B737PFDSpeedTicker::B737PFDSpeedTicker()
  {
    printf("B737PFDSpeedTicker constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 21;
    m_PhysicalSize.y = 18;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  B737PFDSpeedTicker::~B737PFDSpeedTicker()
  {

  }

  void B737PFDSpeedTicker::Render()
  {
    // Call base class to setup viewport and projection
    GaugeComponent::Render();

    bool is_captain = (this->GetArg() == 0);

    float lineWidth = 3.0;
    
    // Draw black background
    glColor3ub(COLOR_BLACK);
    // Rectangular part
    glRectd(0.0,0.0,18.0,18.0);
    // Triangular part
    glBegin(GL_TRIANGLES);
    glVertex2f(18.0,7.0);
    glVertex2f(21.0,9.0);
    glVertex2f(18.0,11.0);
    glEnd();

    // White border around background
    glColor3ub(COLOR_WHITE);
    glLineWidth(lineWidth);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.0,0.0);
    glVertex2f(0.0,18.0);
    glVertex2f(18.0,18.0);
    glVertex2f(18.0,11.0);
    glVertex2f(21.0,9.0);
    glVertex2f(18.0,7.0);
    glVertex2f(18.0,0.0);
    glEnd();

    // indicated air speed (knots)
    float *speed_knots;
    if (is_captain) {
      speed_knots = link_dataref_flt("sim/flightmodel/position/indicated_airspeed",-1);
    } else {
      speed_knots = link_dataref_flt("sim/flightmodel/position/indicated_airspeed2",-1);
    }
    
    if (*speed_knots != FLT_MISS) {

      float vtrans;
      char buffer[12];
      memset(buffer,0,sizeof(buffer));

      // Speed for integer calculations
      int ias_int = (int) *speed_knots;
      float ias_flt = *speed_knots;

      float ias1 = ias_flt;  // IAS last digit and floating point
      if (fabs(ias1)>=100.0) {
	ias1 = ias1 - (float) (100*(int)(ias1/100.0));
      }
      if (fabs(ias1)>=10.0) {
	ias1 = ias1 - (float) (10*(int)(ias1/10.0)); 
      }

      // The speed ticker doesn't display speeds < 30
      /*  if(ias_flt<30.0)
	  {
	  ias_flt = 30.0;
	  ias_int = 30;
	  } */

      // y position of the text (for easy changes)
      double fontHeight = 7.0;
      double texty = m_PhysicalSize.y / 2 - fontHeight / 2;

      m_pFontManager->SetSize(m_Font, 6.0, fontHeight);

      // Draw text in white
      glColor3ub(COLOR_WHITE);
  
      if(fabs(ias_flt)>=100.0)
	{
	  // 100's
	  int hundred = abs(ias_int)/100;
	  int hundredup;
	  int hundreddown;
	  hundredup = (hundred+1)%10;
	  hundreddown = (hundred-1+10)%10;
	  // Figure vertical translation factor for the hundreds place based on 1s position
	  if ((ias_int-100*(int)(ias_int/100)) > 90) {
	    vtrans = -max(ias1 - 9.0,0.0)/1.0 * 2.0 * fontHeight;
	  } else {
	    vtrans = 0.0;
	  }
	    
	  snprintf(buffer, sizeof(buffer), "%i", hundred);
	  m_pFontManager->Print(2.0, texty + vtrans, &buffer[0], m_Font);

	  snprintf(buffer, sizeof(buffer), "%i", hundredup);
	  m_pFontManager->Print(2.0, texty + vtrans + fontHeight + fontHeight, &buffer[0], m_Font);

	  snprintf(buffer, sizeof(buffer), "%i", hundreddown);
	  m_pFontManager->Print(2.0, texty + vtrans - fontHeight - fontHeight, &buffer[0], m_Font);
	  
	  ias_int = ias_int-100*(int)(ias_int/100);
	}

      if(fabs(ias_flt)>10.0)
	{
	  // 10's
	  int ten = abs(ias_int)/10;
	  int tenup;
	  int tendown;
	  tenup = (ten+1)%10;
	  tendown = (ten-1+10)%10;
	  // Figure vertical translation factor for the tens place based on 1s position
	  vtrans = -max(ias1 - 9.0,0.0)/1.0 * 2.0 * fontHeight;

	  //printf("%i %i %i %i %f \n",ias_int,ten,tenup,tendown,vtrans);
	  
	  snprintf(buffer, sizeof(buffer), "%i", ten);
	  m_pFontManager->Print(6.5, texty + vtrans, &buffer[0], m_Font);

	  snprintf(buffer, sizeof(buffer), "%i", tenup);
	  m_pFontManager->Print(6.5, texty + vtrans + fontHeight + fontHeight, &buffer[0], m_Font);

	  snprintf(buffer, sizeof(buffer), "%i", tendown);
	  m_pFontManager->Print(6.5, texty + vtrans - fontHeight - fontHeight, &buffer[0], m_Font);

	  ias_int = ias_int-10*(int)(ias_int/10);
	}
 
      // 1's - the most complicated, since they scroll
      // Middle digit
      int three_one = abs(ias_int);
      // Now figure out the digits above and below
      int five_one = (three_one+2)%10;
      int four_one = (three_one+1)%10;
      int two_one = (three_one - 1 + 10)%10;
      int one_one = (three_one - 2 + 10)%10;

      // Figure out the Speed translation factor for the one's place
      glTranslated(0, -1*(fabs(ias_flt) - (int)fabs(ias_flt))*fontHeight, 0);
  
      // Display all of the digits
      snprintf(buffer, sizeof(buffer), "%i", five_one);
      m_pFontManager->Print(11.0, texty+fontHeight*2+fontHeight/5, &buffer[0], m_Font);
      
      snprintf(buffer, sizeof(buffer), "%i", four_one);
      m_pFontManager->Print(11.0, texty+fontHeight+fontHeight/10, &buffer[0], m_Font);
      
      snprintf(buffer, sizeof(buffer), "%i", three_one);
      m_pFontManager->Print(11.0, texty, &buffer[0], m_Font);
      
      if (ias_flt >= 1.0) {
	snprintf(buffer, sizeof(buffer), "%i", two_one);
	m_pFontManager->Print(11.0, texty-fontHeight-fontHeight/10, &buffer[0], m_Font);
	
	snprintf(buffer, sizeof(buffer), "%i", one_one);
	m_pFontManager->Print(11.0, texty-fontHeight*2-fontHeight/5, &buffer[0], m_Font);
      }
      
    }
  }

} // end namespace OpenGC

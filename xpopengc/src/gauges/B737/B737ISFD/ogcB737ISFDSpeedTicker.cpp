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
#include "ogcB737ISFDSpeedTicker.h"

namespace OpenGC
{

  B737ISFDSpeedTicker::B737ISFDSpeedTicker()
  {
    printf("B737ISFDSpeedTicker constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 19;
    m_PhysicalSize.y = 13;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  B737ISFDSpeedTicker::~B737ISFDSpeedTicker()
  {

  }

  void B737ISFDSpeedTicker::Render()
  {
    // Call base class to setup viewport and projection
    GaugeComponent::Render();

    // y position of the text (for easy changes)
    double fontHeight = 7.0;
    double texty = m_PhysicalSize.y / 2 - fontHeight / 2;
    float lineWidth = 3.0;

    // Draw black background
    glColor3ub(COLOR_BLACK);
    glRectd(0.0,0.0,m_PhysicalSize.x,m_PhysicalSize.y);
    glEnd();

    // White border around background
    glLineWidth(lineWidth);
    glColor3ub(COLOR_WHITE);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.3,0.3);
    glVertex2f(m_PhysicalSize.x-0.3,0.3);
    glVertex2f(m_PhysicalSize.x-0.3,m_PhysicalSize.y-0.3);
    glVertex2f(0.3,m_PhysicalSize.y-0.3);
    glEnd();

    // indicated air speed (knots)
    float *speed_knots = link_dataref_flt("sim/flightmodel/position/indicated_airspeed",-1);
    
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
	  m_pFontManager->Print(7.5, texty + vtrans, &buffer[0], m_Font);

	  snprintf(buffer, sizeof(buffer), "%i", tenup);
	  m_pFontManager->Print(7.5, texty + vtrans + fontHeight + fontHeight, &buffer[0], m_Font);

	  snprintf(buffer, sizeof(buffer), "%i", tendown);
	  m_pFontManager->Print(7.5, texty + vtrans - fontHeight - fontHeight, &buffer[0], m_Font);

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
      m_pFontManager->Print(12.0, texty+fontHeight*2+fontHeight/5, &buffer[0], m_Font);
      
      snprintf(buffer, sizeof(buffer), "%i", four_one);
      m_pFontManager->Print(12.0, texty+fontHeight+fontHeight/10, &buffer[0], m_Font);
      
      snprintf(buffer, sizeof(buffer), "%i", three_one);
      m_pFontManager->Print(12.0, texty, &buffer[0], m_Font);
      
      if (ias_flt >= 1.0) {
	snprintf(buffer, sizeof(buffer), "%i", two_one);
	m_pFontManager->Print(12.0, texty-fontHeight-fontHeight/10, &buffer[0], m_Font);
	
	snprintf(buffer, sizeof(buffer), "%i", one_one);
	m_pFontManager->Print(12.0, texty-fontHeight*2-fontHeight/5, &buffer[0], m_Font);
      }
      
    }
  }

} // end namespace OpenGC

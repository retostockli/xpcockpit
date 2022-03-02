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

#include "ogcGaugeComponent.h"
#include "ogcB737PFD.h"
#include "ogcB737PFDAltitudeTicker.h"

namespace OpenGC
{

  B737PFDAltitudeTicker::B737PFDAltitudeTicker()
  {
    printf("B737PFDAltitudeTicker constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 28;
    m_PhysicalSize.y = 18;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  B737PFDAltitudeTicker::~B737PFDAltitudeTicker()
  {

  }

  void B737PFDAltitudeTicker::Render()
  {
    // Call base class to setup viewport and projection
    GaugeComponent::Render();

    int acf_type = m_pDataSource->GetAcfType();
    bool is_captain = (this->GetArg() == 0);

    float lineWidth = 3.0;
 
    // Draw black background
    glLineWidth(lineWidth);
    glColor3ub(COLOR_BLACK);
    // Rectangular part
    glRectd(3.5,0.0,28.0,18.0);
    // Triangular part
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0,9.0);
    glVertex2f(3.5,6.0);
    glVertex2f(3.5,12.0);
    glEnd();

    // White border around background
    glLineWidth(lineWidth);
    glColor3ub(COLOR_WHITE);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.0,9.0);
    glVertex2f(3.5,12.0);
    glVertex2f(3.5,18.0);
    glVertex2f(28.0,18.0);
    glVertex2f(28.0,0.0);
    glVertex2f(3.5,0.0);
    glVertex2f(3.5,6.0);
    glEnd();

    // Get the barometric altitude (feet)
    float *pressure_altitude;
    if (is_captain) {
      pressure_altitude = link_dataref_flt("sim/flightmodel/misc/h_ind",0);
    } else {
      pressure_altitude = link_dataref_flt("sim/flightmodel/misc/h_ind_copilot",0);
    }

    if (*pressure_altitude != FLT_MISS) {

      float vtrans;
      
      
      char buffer[10];
      memset(buffer,0,sizeof(buffer));

      int alt = (int) *pressure_altitude;
      bool negative = (alt < 0);

      // last two digitis (10s and 1s) of altitude
      int alt100 = abs(alt)%100;
      //int alt100 = alt%100;
     
      // y position of the text (for easy changes)
      float bigFontHeight = 8.0;
      float littleFontHeight = 6.5;
      float texty = m_PhysicalSize.y / 2 - bigFontHeight / 2;

      m_pFontManager->SetSize(m_Font, 6.0, bigFontHeight);

      // Draw text in white
      glColor3ub(COLOR_WHITE);

      // 100000's: Space Shuttle: don't draw for now
      if(abs(alt) >= 100000)
	{
	  /*
	  snprintf(buffer, sizeof(buffer), "%i", abs(alt)/10000);
	  m_pFontManager->Print(5.0, texty, buffer, m_Font);
	  */
	  alt = alt-100000*(int)(alt/100000);
	}
      // 10000's
      if(abs(alt) >= 10000)
	{
	  int tenthousand = abs(alt)/10000;
	  int tenthousandup = (tenthousand+1)%10;
	  int tenthousanddown = (tenthousand-1+10)%10;
	  
	  // Figure out the Altitude vertical translation factor for the tenthousands place
	  // based on 10s and 1s position in the 80-100 range if we are below a 1000s switch
	  if ((abs(alt)%10000) > 9900) {
	    vtrans = -max((float)(alt100) - 80.0,0.0)/20.0 * 2.0 * bigFontHeight;
	  } else {
	    vtrans = 0.0;
	  }
	  
	  snprintf(buffer, sizeof(buffer), "%i", tenthousand);
	  m_pFontManager->Print(5.0, texty + vtrans, &buffer[0], m_Font);
	  
	  snprintf(buffer, sizeof(buffer), "%i", tenthousandup);
	  m_pFontManager->Print(5.0, texty + vtrans + bigFontHeight + bigFontHeight, &buffer[0], m_Font);
	  
	  snprintf(buffer, sizeof(buffer), "%i", tenthousanddown);
	  m_pFontManager->Print(5.0, texty + vtrans - bigFontHeight - bigFontHeight, &buffer[0], m_Font);

	  
	}
      else // draw a green square if positive or - if negative
	{
	  if (negative) {
	    snprintf(buffer, sizeof(buffer), "-");
	    m_pFontManager->Print(5.0, texty + bigFontHeight/10.0, &buffer[0], m_Font);
	  } else {
	    glColor3ub(COLOR_GREEN);
	    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	    glBegin(GL_POLYGON);
	    glVertex2f(4.0,texty);
	    glVertex2f(4.0,texty + bigFontHeight);
	    glVertex2f(8.0,texty + bigFontHeight);
	    glVertex2f(8.0,texty);
	    glVertex2f(4.0,texty);
	    glEnd();
	  }
	}
  
      glColor3ub(COLOR_WHITE);
      // 1000's
      alt = alt-10000*(int)(alt/10000);
      int thousand = abs(alt)/1000;
      int thousandup;
      int thousanddown;
      if (negative) {
	thousandup = (thousand-1+10)%10;
	thousanddown = (thousand+1)%10;
      } else {
	thousandup = (thousand+1)%10;
	thousanddown = (thousand-1+10)%10;
      }
      
      // Figure out the Altitude vertical translation factor for the thousands place
      // based on 10s and 1s position in the 80-100 range if we are below a 1000s switch
      if ((abs(alt)-1000*(int)(abs(alt)/1000)) > 900) {
	vtrans = -max((float)(alt100) - 80.0,0.0)/20.0 * 2.0 * bigFontHeight;
      } else {
	vtrans = 0.0;
      }

      if (negative) vtrans = -vtrans;
      
      snprintf(buffer, sizeof(buffer), "%i", thousand);
      m_pFontManager->Print(9.5, texty + vtrans, &buffer[0], m_Font);

      snprintf(buffer, sizeof(buffer), "%i", thousandup);
      m_pFontManager->Print(9.5, texty + vtrans + bigFontHeight + bigFontHeight, &buffer[0], m_Font);

      snprintf(buffer, sizeof(buffer), "%i", thousanddown);
      m_pFontManager->Print(9.5, texty + vtrans - bigFontHeight - bigFontHeight, &buffer[0], m_Font);

      // The 100's, 10's, and 1's are drawn in a smaller size
      m_pFontManager->SetSize(m_Font, 5.0, littleFontHeight);
      texty = m_PhysicalSize.y / 2 - littleFontHeight / 2;
  
      // 100's      
      alt = alt-1000*(int)(alt/1000);
      int hundred = abs(alt)/100;

      int hundredup;
      int hundreddown;
      if (negative) {
	hundredup = (hundred-1+10)%10;\
	hundreddown = (hundred+1)%10;
      } else {
	hundredup = (hundred+1)%10;
	hundreddown = (hundred-1+10)%10;
      }
      // Figure out the Altitude vertical translation factor for the hundreds place
      // based on 10s and 1s position in the 80-100 range
      vtrans = -max((float) (alt100)  - 80.0,0.0)/20.0 * 2.0 * littleFontHeight;

      if (negative) vtrans = -vtrans;
      
      snprintf(buffer, sizeof(buffer), "%i", hundred);
      m_pFontManager->Print(15.0, texty + vtrans, &buffer[0], m_Font);

      snprintf(buffer, sizeof(buffer), "%i", hundredup);
      m_pFontManager->Print(15.0, texty + vtrans + littleFontHeight + littleFontHeight, &buffer[0], m_Font);

      snprintf(buffer, sizeof(buffer), "%i", hundreddown);
      m_pFontManager->Print(15.0, texty + vtrans - littleFontHeight - littleFontHeight, &buffer[0], m_Font);

      // The 10's and 1's position (which is always 0) scroll based on altitude
      // We use three digits for the tens position, high, middle, and low
      // Note that the tens digit is always a multiple of 2      
      alt = alt100;
      int middle_ten = abs(alt) / 10;

      bool roundupnine = false;
  
      switch(middle_ten)
	{
	case 1:
	  middle_ten = 2;
	  break;
	case 3:
	  middle_ten = 4;
	  break;
	case 5:
	  middle_ten = 6;
	  break;
	case 7:
	  middle_ten = 8;
	  break;
	case 9:
	  middle_ten = 0;
	  roundupnine = true;
	  break;
	}

      // Figure out the translation for the tens and ones position
      float vertTranslation;

      if(middle_ten!=0)
	vertTranslation = (middle_ten*10 - (float)abs(alt))/20*littleFontHeight;
      else
	{
	  if(roundupnine==true)
	    vertTranslation = (100 - (float)abs(alt))/20*littleFontHeight;
	  else
	    vertTranslation = (0 - (float)abs(alt))/20*littleFontHeight;
	}

      if (negative) vertTranslation = -vertTranslation;
      
      glTranslated(0, vertTranslation, 0);

      // Now figure out the digits above and below
      int top_ten;
      int bottom_ten;

      if ((abs(*pressure_altitude) < 20.) && (middle_ten == 0)) {
	  top_ten = 2;
	  bottom_ten = 2;
      } else {
	if (negative) {
	  top_ten = (middle_ten - 2 + 10)%10;
	  bottom_ten = (middle_ten+2)%10;
	} else {
	  top_ten = (middle_ten+2)%10;
	  bottom_ten = (middle_ten - 2 + 10)%10;
	}
      }
     
      // Display all of the digits
      snprintf(buffer, sizeof(buffer), "%i", top_ten);
      m_pFontManager->Print(19.0, texty + littleFontHeight + littleFontHeight/10, &buffer[0], m_Font);
      m_pFontManager->Print(23.0, texty + littleFontHeight + littleFontHeight/10, "0", m_Font);

      snprintf(buffer, sizeof(buffer), "%i", middle_ten);
      m_pFontManager->Print(19.0, texty, &buffer[0], m_Font);
      m_pFontManager->Print(23.0, texty, "0", m_Font);

      snprintf(buffer, sizeof(buffer), "%i", bottom_ten);
      m_pFontManager->Print(19.0, texty + -1*littleFontHeight - littleFontHeight/10, &buffer[0], m_Font);
      m_pFontManager->Print(23.0, texty + -1*littleFontHeight - littleFontHeight/10, "0", m_Font);

    }
  }

} // end namespace OpenGC

/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737AltitudeTicker.cpp,v $

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

#include "ogcGaugeComponent.h"
#include "ogcB737AltitudeTicker.h"

namespace OpenGC
{

  B737AltitudeTicker::B737AltitudeTicker()
  {
    printf("B737AltitudeTicker constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 28;
    m_PhysicalSize.y = 18;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  B737AltitudeTicker::~B737AltitudeTicker()
  {

  }

  void B737AltitudeTicker::Render()
  {
    // Call base class to setup viewport and projection
    GaugeComponent::Render();

    glLineWidth(2.0);

    // Draw black background
    glColor3ub(0,0,0);
    // Rectangular part
    glRectd(3.5,0.0,28.0,18.0);
    // Triangular part
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0,9.0);
    glVertex2f(3.5,6.0);
    glVertex2f(3.5,12.0);
    glEnd();

    // White border around background
    glColor3ub(255,255,255);
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
    float *pressure_altitude = link_dataref_flt("sim/flightmodel/misc/h_ind",0);

    if (*pressure_altitude != FLT_MISS) {
  
      char buffer[4];

      int alt = (int) *pressure_altitude;

      // y position of the text (for easy changes)
      float bigFontHeight = 8.0;
      float littleFontHeight = 6.5;
      float texty = m_PhysicalSize.y / 2 - bigFontHeight / 2;

      m_pFontManager->SetSize(m_Font, 6.0, bigFontHeight);

      // Draw text in white
      glColor3ub(255,255,255);

      // 100000's: don't draw for now
      if(abs(alt) >= 100000)
	{
	  //_itoa( alt / 10000, buffer, 10);
	  /*
	  snprintf(buffer, sizeof(buffer), "%i", abs(alt)/10000);
	  m_pFontManager->Print(5.0, texty, buffer, m_Font);
	  */
	  alt = alt-100000*(int)(alt/100000);
	}
      // 10000's
      if(abs(alt) >= 10000)
	{
	  //_itoa( alt / 10000, buffer, 10);
	  snprintf(buffer, sizeof(buffer), "%i", abs(alt)/10000);
	  m_pFontManager->Print(5.0, texty, buffer, m_Font);
	  alt = alt-10000*(int)(alt/10000);
	}
      else // draw a green square
	{
	  glColor3ub(0,179,0);
	  glBegin(GL_POLYGON);
	  glVertex2f(4.0,texty);
	  glVertex2f(4.0,texty + bigFontHeight);
	  glVertex2f(8.0,texty + bigFontHeight);
	  glVertex2f(8.0,texty);
	  glVertex2f(4.0,texty);
	  glEnd();
	  glColor3ub(255,255,255);
	}
  
      // 1000's
      //_itoa( alt / 1000, buffer, 10);
      snprintf(buffer, sizeof(buffer), "%i", abs(alt)/1000);
      m_pFontManager->Print(9.5, texty, &buffer[0], m_Font);
      alt = alt-1000*(int)(alt/1000);

      // The 100's, 10's, and 1's are drawn in a smaller size
      m_pFontManager->SetSize(m_Font, 5.0, littleFontHeight);
      texty = m_PhysicalSize.y / 2 - littleFontHeight / 2;
  
      // 100's
      //_itoa( alt / 100, buffer, 10);
      snprintf(buffer, sizeof(buffer), "%i", abs(alt)/100);
      m_pFontManager->Print(15.0, texty, &buffer[0], m_Font);
      alt = alt-100*(int)(alt/100);

      // The 10's and 1's position (which is always 0) scroll based on altitude
      // We use three digits for the tens position, high, middle, and low
      // Note that the tens digit is always a multiple of 2
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

      glTranslated(0, vertTranslation, 0);

      // Now figure out the digits above and below
      int top_ten = (middle_ten+2)%10;
      int bottom_ten = (middle_ten - 2 + 10)%10;
  
      // Display all of the digits
      //_itoa( top_ten, buffer, 10);
      snprintf(buffer, sizeof(buffer), "%i", top_ten);
      m_pFontManager->Print(19.0, texty + littleFontHeight + littleFontHeight/10, &buffer[0], m_Font);
      m_pFontManager->Print(23.0, texty + littleFontHeight + littleFontHeight/10, "0", m_Font);

      //_itoa( middle_ten, buffer, 10);
      snprintf(buffer, sizeof(buffer), "%i", middle_ten);
      m_pFontManager->Print(19.0, texty, &buffer[0], m_Font);
      m_pFontManager->Print(23.0, texty, "0", m_Font);

      //_itoa( bottom_ten, buffer, 10);
      snprintf(buffer, sizeof(buffer), "%i", bottom_ten);
      m_pFontManager->Print(19.0, texty + -1*littleFontHeight - littleFontHeight/10, &buffer[0], m_Font);
      m_pFontManager->Print(23.0, texty + -1*littleFontHeight - littleFontHeight/10, "0", m_Font);

    }
  }

} // end namespace OpenGC

/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737MapModePlan.cpp,v $

  Last modification:
  Date:      $Date: 2015/09/11 $
  Version:   $Revision: $
  Author:    $Author: stockli $
  
  Copyright (c) 2001-2015 Damion Shelton and Reto Stockli
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

  =========================================================================*/

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string>

#include "ogcGaugeComponent.h"
#include "ogcCircleEvaluator.h"
#include "B737/B737NAV/ogcB737NAV.h"
#include "B737/B737NAV/ogcB737NAVMapModePlan.h"

namespace OpenGC
{

  B737NAVMapModePlan::B737NAVMapModePlan()
  {
    printf("B737NAVMapModePlan constructed\n");
  
    m_Font = m_pFontManager->LoadDefaultFont();

  }

  B737NAVMapModePlan::~B737NAVMapModePlan()
  {
    // Destruction handled by base class
  }

  void B737NAVMapModePlan::Render()
  {
    GaugeComponent::Render();

    int mapMode = m_NAVGauge->GetMapMode();
 
    if (mapMode == 3) {
      // Draw the Plan Map Mode only
      //int acf_type = m_pDataSource->GetAcfType();    
      float mapRange = m_NAVGauge->GetMapRange();
      bool mapCenter = m_NAVGauge->GetMapCenter();
    
      char buffer[5];

      // define geometric stuff
      float fontSize = 4.0 * m_PhysicalSize.x / 150.0;
      float lineWidth = 3.0;

      // double dtor = 0.0174533; /* radians per degree */
      // double radeg = 57.2958;  /* degree per radians */

      // define ACF center position in relative coordinates
      float acf_x;
      float acf_y;
      if (mapCenter) {
	acf_x = 0.500;
	acf_y = 0.500;
      } else {
	acf_x = 0.500;
	acf_y = 0.200;
      }
      //    float map_x_min = 0.000;
      //    float map_x_max = 1.000;
      //    float map_y_min = 0.040;
      float map_y_max = 0.888;
      float map_size = m_PhysicalSize.y*(map_y_max-acf_y);

      // Where is the aircraft?
      //double *aircraftLat = link_dataref_dbl("sim/flightmodel/position/latitude",-4);
      //double *aircraftLon = link_dataref_dbl("sim/flightmodel/position/longitude",-4);
    
      // What's the heading?
      //float *heading_mag = link_dataref_flt("sim/flightmodel/position/magpsi",-1);
      //float heading_map =  m_NAVGauge->GetMapHeading();
      //float *magnetic_variation = link_dataref_flt("sim/flightmodel/position/magnetic_variation",-1);
     
      //if (heading_map != FLT_MISS) {
      
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
    
	glColor3ub(COLOR_WHITE);
	glLineWidth(lineWidth);

	// Shift center and rotate about heading
	//glTranslatef(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y, 0.0);
	//glRotatef(heading_map, 0, 0, 1);

	//------ ACF symbol and distance circles -------
	/*
	glPushMatrix();
      
	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(0, 0, 0, 1);
      
	glBegin(GL_LINE_LOOP);
	glVertex2f(m_PhysicalSize.x*(acf_x-0.030), m_PhysicalSize.y*(acf_y-0.070));
	glVertex2f(m_PhysicalSize.x*(acf_x+0.030), m_PhysicalSize.y*(acf_y-0.070));
	glVertex2f(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2f(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*(acf_y+0.080));
	glVertex2f(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*map_y_max);
	glEnd();
      
	glPopMatrix();
	*/

	// plot range scale and geographical directions
	glPushMatrix();
	glTranslatef(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y, 0);

	m_pFontManager->SetSize( m_Font, fontSize, fontSize );
	glColor3ub(COLOR_GREEN);
	m_pFontManager->Print( -0.5*fontSize, map_size*1.1, "N", m_Font);
	m_pFontManager->Print( -0.5*fontSize, -map_size*1.15, "S", m_Font);
	m_pFontManager->Print( map_size*1.1 , -0.5*fontSize , "E", m_Font);
	m_pFontManager->Print( -map_size*1.2 , -0.5*fontSize , "W", m_Font);

	glColor3ub(COLOR_WHITE);
	m_pFontManager->SetSize( m_Font, 0.75*fontSize, 0.75*fontSize );
	if (mapRange > 5.0) {
	  snprintf(buffer, sizeof(buffer), "%i", (int) (mapRange / 2.0));
	} else {
	  snprintf(buffer, sizeof(buffer), "%2.1f", mapRange / 2.0);
	}
	if ((mapRange / 2.0) < 100) {
	  if ((mapRange / 2.0) < 10) {
	    if ((mapRange / 2.0) < 5) {
	      m_pFontManager->Print( -0.9*fontSize, map_size*0.48, &buffer[0], m_Font);
	      m_pFontManager->Print( -0.9*fontSize, -map_size*0.52, &buffer[0], m_Font);
	    } else {
	      m_pFontManager->Print( -0.25*fontSize, map_size*0.48, &buffer[0], m_Font);
	      m_pFontManager->Print( -0.25*fontSize, -map_size*0.52, &buffer[0], m_Font);
	    }
	  } else {
	    m_pFontManager->Print( -0.65*fontSize, map_size*0.48, &buffer[0], m_Font);
	    m_pFontManager->Print( -0.65*fontSize, -map_size*0.52, &buffer[0], m_Font);
	  }
	} else {
	  m_pFontManager->Print( -1.0*fontSize, map_size*0.48, &buffer[0], m_Font);
	  m_pFontManager->Print( -1.0*fontSize, -map_size*0.52, &buffer[0], m_Font);
	}

	snprintf(buffer, sizeof(buffer), "%i", (int) mapRange);
	if (mapRange < 100) {
	  if (mapRange < 10) {
	    m_pFontManager->Print( -0.25*fontSize, map_size*0.98, &buffer[0], m_Font);
	    m_pFontManager->Print( -0.25*fontSize, -map_size*1.02, &buffer[0], m_Font);
	  } else {
	    m_pFontManager->Print( -0.65*fontSize, map_size*0.98, &buffer[0], m_Font);
	    m_pFontManager->Print( -0.65*fontSize, -map_size*1.02, &buffer[0], m_Font);
	  }
	} else {
	  m_pFontManager->Print( -1.0*fontSize, map_size*0.98, &buffer[0], m_Font);
	  m_pFontManager->Print( -1.0*fontSize, -map_size*1.02, &buffer[0], m_Font);
	}
	glPopMatrix();
    
	// Set up big circle for nav range
	glPushMatrix();
	
	glLineWidth(lineWidth);
	glTranslatef(0.0, 0.0, 0.0);
	CircleEvaluator bCircle;
	bCircle.SetDegreesPerPoint(2);
	
	bCircle.SetArcStartEnd(4,176);
	bCircle.SetRadius(map_size*1.0);
	bCircle.SetOrigin(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y);
	glBegin(GL_LINE_STRIP);
	bCircle.Evaluate();
	glEnd();
	bCircle.SetArcStartEnd(184,356);
	bCircle.SetRadius(map_size*1.0);
	bCircle.SetOrigin(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y);
	glBegin(GL_LINE_STRIP);
	bCircle.Evaluate();
	glEnd();
	bCircle.SetArcStartEnd(8,172);
	bCircle.SetRadius(map_size*0.5);
	bCircle.SetOrigin(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y);
	glBegin(GL_LINE_STRIP);
	bCircle.Evaluate();
	glEnd();
	bCircle.SetArcStartEnd(188,352);
	bCircle.SetRadius(map_size*0.5);
	bCircle.SetOrigin(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y);
	glBegin(GL_LINE_STRIP);
	bCircle.Evaluate();
	glEnd();

	glPopMatrix();

	glPopMatrix();

	//}
      
    }
    
  }

} // end namespace OpenGC

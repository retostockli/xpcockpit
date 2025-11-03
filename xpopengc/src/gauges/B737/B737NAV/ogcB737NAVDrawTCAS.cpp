/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Copyright (c) 2015-2024 Reto Stockli
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
#include "B737/B737NAV/ogcB737NAVDrawTCAS.h"

namespace OpenGC
{

  B737NAVDrawTCAS::B737NAVDrawTCAS()
  {
    printf("B737NAVDrawTCAS constructed\n");
  
    m_Font = m_pFontManager->LoadDefaultFont();

  }

  B737NAVDrawTCAS::~B737NAVDrawTCAS()
  {
    // Destruction handled by base class
  }

  void B737NAVDrawTCAS::Render()
  {
    GaugeComponent::Render();

    int acf_type = m_pDataSource->GetAcfType();
 
    bool mapCenter = m_NAVGauge->GetMapCenter();
    int mapMode = m_NAVGauge->GetMapMode();
    float mapRange = m_NAVGauge->GetMapRange();

    if (mapMode != 3) {
      /* Do not draw TCAS in plan mode */
      /* ADD: and maybe not in other modes as well */

      /* Examine whether TCAS is shown */
      bool show_tcas;
      if ((acf_type == 2) || (acf_type == 3)) {
	float *transponder_mode = link_dataref_flt("laminar/B738/knob/transponder_pos",0);
	int *xpilot_status = link_dataref_int("xpilot/login/status");
	if (((*transponder_mode == 4) || (*transponder_mode == 5)) && (*xpilot_status == 1)) {
	  show_tcas = true;
	} else {
	  show_tcas = false;
	} 
      } else {
	show_tcas = true;
      }
      
      char buffer[15];
    
      // define geometric stuff
      float fontSize = 4.0 * m_PhysicalSize.x / 150.0;
      // float lineWidth = 1.5 * m_PhysicalSize.x / 100.0;

      //double dtor = 0.0174533; /* radians per degree */
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

      // Where is our aircraft?
      double aircraftLon = m_NAVGauge->GetMapCtrLon();
      double aircraftLat = m_NAVGauge->GetMapCtrLat();
      float *aircraftAlt = link_dataref_flt("sim/flightmodel/position/elevation",0); // meters
      float *altitude_agl = link_dataref_flt("sim/flightmodel/position/y_agl",0);
    
      // What's the heading?
      float heading_map =  m_NAVGauge->GetMapHeading();
      float *magnetic_variation = link_dataref_flt("sim/flightmodel/position/magnetic_variation",-1);
        
      /* TCAS Datarefs (AI Planes + plugins like xPilot) */   
      float *plon = link_dataref_flt_arr("sim/cockpit2/tcas/targets/position/lon",MAXMP,-1,-2); 
      float *plat = link_dataref_flt_arr("sim/cockpit2/tcas/targets/position/lat",MAXMP,-1,-2); 
      float *pele = link_dataref_flt_arr("sim/cockpit2/tcas/targets/position/ele",MAXMP,-1,2);
      float *pspd = link_dataref_flt_arr("sim/cockpit2/tcas/targets/position/V_msc",MAXMP,-1,1); // not populated by xPilot
    
      // The input coordinates are in lon/lat, so we have to rotate against true heading
      // despite the NAV display is showing mag heading
      if ((heading_map != FLT_MISS) && (*magnetic_variation != FLT_MISS) && (show_tcas)) {

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
    
	// Shift center and rotate about heading
	glTranslatef(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y, 0.0);
	glRotatef(heading_map - *magnetic_variation, 0, 0, 1);


	/* valid own aircraft coordinates ? */
	if ((aircraftLon != FLT_MISS) && (aircraftLat != FLT_MISS) && (*aircraftAlt != FLT_MISS)) {
        
	  // Set up circle for small symbols
	  CircleEvaluator aCircle;
	  aCircle.SetArcStartEnd(0,360);
	  aCircle.SetDegreesPerPoint(10);

	  float xPos;
	  float yPos;
	  float zPos;

	  // define overall symbol size (for DME, FIX, VOR, APT etc.)
	  // this scale gives the radius of the symbol in physical units
	  float ss = m_PhysicalSize.y*0.02;

	  // plot MP or TCAS planes
	  // First index is our own plane
	  // for (int i=0;i<MAXTCAS;i++) {
	  for (int i=1;i<MAXMP;i++) {
	  
	    xPos = FLT_MISS;
	    yPos = FLT_MISS;
	    zPos = FLT_MISS;

	    /* Only Draw TCAS Blobs if they have valid elevation and speed above 50 kts */
	    if ((pele[i] != 0.0) && ((pspd[i]*1.94) > 50.0)) {
	      double lon = (double) plon[i];
	      double lat = (double) plat[i];
	      double easting;
	      double northing;
	      lonlat2gnomonic(&lon, &lat, &easting, &northing, &aircraftLon, &aircraftLat);
	    
	      // Compute physical position relative to acf center on screen
	      yPos = -northing / 1852.0 / mapRange * map_size; 
	      xPos = easting / 1852.0  / mapRange * map_size;
	      zPos = (pele[i] - *aircraftAlt)*3.28084;
	      // printf("%i %f %f %f %f %f \n",j,xPos,yPos,zPos,mp_alt[i]*3.28084,*aircraftAlt*3.28084);
	    }

	    /* plot all traffic in the air (not the one on the ground) */
	    if ((xPos != FLT_MISS) && (yPos != FLT_MISS) && (zPos != FLT_MISS) &&
		(((fabs(zPos) > 50.) && (*altitude_agl < 10.)) || (*altitude_agl >= 10.))) {	    

	      // printf("%i %f %f %f %f %f \n",i,pele[i],*aircraftAlt,zPos,pspd[i]);

	      glPushMatrix();

	      glTranslatef(xPos, yPos, 0.0);
	      glRotatef(-1.0 * (heading_map - *magnetic_variation), 0, 0, 1);

	      glLineWidth(2.0);
	      glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

	      float ss2 = 0.65*ss;
	      if (fabs(zPos) < 300.) {
		// Resolution Advisory (red filled square)
		glColor3ub(COLOR_RED);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glBegin(GL_POLYGON);
		glVertex2f(-ss2, -ss2);
		glVertex2f(ss2, -ss2);
		glVertex2f(ss2, ss2);
		glVertex2f(-ss2, ss2);
		glEnd();
	      } else if (fabs(zPos) < 900.) {
		// traffic advisory (yellow filled circle)
		glColor3ub(COLOR_YELLOW);
		CircleEvaluator tCircle;
		tCircle.SetDegreesPerPoint(20);	    
		tCircle.SetArcStartEnd(0,360);
		tCircle.SetRadius(ss2);
		tCircle.SetOrigin(0,0);
		glBegin(GL_POLYGON);
		tCircle.Evaluate();
		glEnd();
	      } else {
		// other: white diamond
		glColor3ub(COLOR_WHITE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glBegin(GL_POLYGON);
		glVertex2f(0, -ss2);
		glVertex2f(ss2, 0);
		glVertex2f(0, ss2);
		glVertex2f(-ss2, 0);
		glEnd();
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	      }

	      if (fabs(zPos/100) < 100) {
		if (zPos > 0) {
		  m_pFontManager->SetSize( m_Font, 0.6*fontSize, 0.6*fontSize );
		  snprintf(buffer, sizeof(buffer), "+%i", (int) (fabs(zPos)/100));
		  m_pFontManager->Print(-0.9*fontSize,ss2+0.05*fontSize, &buffer[0], m_Font);	      
		} else {
		  m_pFontManager->SetSize( m_Font, 0.6*fontSize, 0.6*fontSize );
		  snprintf(buffer, sizeof(buffer), "-%i", (int) (fabs(zPos)/100));
		  m_pFontManager->Print(-0.9*fontSize,-ss2-0.65*fontSize, &buffer[0], m_Font);
		}
	      }
	      glPopMatrix();
	    }

	  }
	
	} // valid acf coordinates
    
	glPopMatrix();

      }


      if ((acf_type == 2) || (acf_type == 3)) {
	float *transponder_mode = link_dataref_flt("laminar/B738/knob/transponder_pos",0);

	// plot map options
	glPushMatrix();
	glColor3ub(COLOR_BLACK);
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glVertex2f(0,m_PhysicalSize.y*0.140);
	glVertex2f(0,m_PhysicalSize.y*0.210);
	glVertex2f(m_PhysicalSize.x*0.100,m_PhysicalSize.y*0.210);
	glVertex2f(m_PhysicalSize.x*0.100,m_PhysicalSize.y*0.140);
	glEnd();
	if ((*transponder_mode == 4) || (*transponder_mode == 5)) {
	  m_pFontManager->SetSize( m_Font, 0.75*fontSize, 0.75*fontSize );
	  glColor3ub(COLOR_LIGHTBLUE);
	  m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.176 ,"TFC",m_Font);  
	  if (*transponder_mode == 4) {
	    m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.146 ,"TA ONLY",m_Font);
	  }
	} else {
	  m_pFontManager->SetSize( m_Font, 0.85*fontSize, 0.85*fontSize );
	  glColor3ub(COLOR_ORANGE);
	  m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.176 ,"TCAS",m_Font);
	  m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.146 ,"OFF",m_Font);
	}
	glPopMatrix();

      }
      
    }
    
  }

} // end namespace OpenGC

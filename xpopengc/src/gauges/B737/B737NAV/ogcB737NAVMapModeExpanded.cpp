/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737MapModeExpanded.cpp,v $

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
#include "B737/B737NAV/ogcB737NAVMapModeExpanded.h"

namespace OpenGC
{

  B737NAVMapModeExpanded::B737NAVMapModeExpanded()
  {
    printf("B737NAVMapModeExpanded constructed\n");
  
    m_Font = m_pFontManager->LoadDefaultFont();

  }

  B737NAVMapModeExpanded::~B737NAVMapModeExpanded()
  {
    // Destruction handled by base class
  }

  void B737NAVMapModeExpanded::Render()
  {
    GaugeComponent::Render();

    bool is_captain = (this->GetArg() == 0);
    bool is_copilot = (this->GetArg() == 1);

    int mapMode = m_NAVGauge->GetMapMode();
 
    if (mapMode != 3) {
      // Draw the Expanded Map Mode only
      int acf_type = m_pDataSource->GetAcfType();    
      float mapRange = m_NAVGauge->GetMapRange();
      bool mapCenter = m_NAVGauge->GetMapCenter();
      float heading_map =  m_NAVGauge->GetMapHeading();
    
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
      //double aircraftLon = m_NAVGauge->GetMapCtrLon();
      //double aircraftLat = m_NAVGauge->GetMapCtrLat();
    
      // What's the heading?
      float *heading_mag = link_dataref_flt("sim/flightmodel/position/magpsi",-1);
      //float *heading_true = link_dataref_flt("sim/flightmodel/position/psi",-1);
      float *magnetic_variation = link_dataref_flt("sim/flightmodel/position/magnetic_variation",-1);

      /*
      float *track_mag;
      if (is_captain) {
	track_mag = link_dataref_flt("sim/cockpit2/gauges/indicators/ground_track_mag_pilot",-1);
      } else {
	track_mag = link_dataref_flt("sim/cockpit2/gauges/indicators/ground_track_mag_copilot",-1);
	}*/
      int *efis1_selector;      
      if (is_captain) {
	efis1_selector = link_dataref_int("sim/cockpit2/EFIS/EFIS_1_selection_pilot");
      } else {
	efis1_selector = link_dataref_int("sim/cockpit2/EFIS/EFIS_1_selection_copilot");
      }
      float *adf1_bearing = link_dataref_flt("sim/cockpit2/radios/indicators/adf1_bearing_deg_mag",0);
      float *nav1_bearing = link_dataref_flt("sim/cockpit2/radios/indicators/nav1_bearing_deg_mag",0);
      //    unsigned char *nav1_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/nav1_nav_id",500,-1);
      //   unsigned char *adf1_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/adf1_nav_id",500,-1);
      unsigned char *nav1_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/nav1_nav_id",150,-1);
      unsigned char *adf1_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/adf1_nav_id",150,-1);

      int *efis2_selector;
      if (is_captain) {
	efis2_selector = link_dataref_int("sim/cockpit2/EFIS/EFIS_2_selection_pilot");
      } else {
	efis2_selector = link_dataref_int("sim/cockpit2/EFIS/EFIS_2_selection_copilot");
      }
      float *adf2_bearing = link_dataref_flt("sim/cockpit2/radios/indicators/adf2_bearing_deg_mag",0);
      float *nav2_bearing = link_dataref_flt("sim/cockpit2/radios/indicators/nav2_bearing_deg_mag",0);
      //    unsigned char *nav2_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/nav2_nav_id",500,-1);
      //    unsigned char *adf2_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/adf2_nav_id",500,-1);
      unsigned char *nav2_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/nav2_nav_id",150,-1);
      unsigned char *adf2_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/adf2_nav_id",150,-1);

      float *ap_vspeed = link_dataref_flt("sim/cockpit/autopilot/vertical_velocity",0);
      float *ap_altitude = link_dataref_flt("sim/cockpit/autopilot/altitude",0);
      float *speed_knots = link_dataref_flt("sim/flightmodel/position/indicated_airspeed",-1);
      float *pressure_altitude = link_dataref_flt("sim/flightmodel/misc/h_ind",0);

      int *has_heading_ap_line;
      float *heading_mag_ap;
      if ((acf_type == 2) || (acf_type == 3)) {
	has_heading_ap_line = link_dataref_int("laminar/B738/nd/hdg_bug_line");
	heading_mag_ap = link_dataref_flt("laminar/B738/autopilot/mcp_hdg_dial",0);
      } else {
	has_heading_ap_line = link_dataref_int("xpserver/has_hdg_bug");
	heading_mag_ap = link_dataref_flt("sim/cockpit2/autopilot/heading_dial_deg_mag_pilot",0);
	*has_heading_ap_line = 1;
      }

      if ((heading_map != FLT_MISS) && (*magnetic_variation != FLT_MISS)) {

	float dist_to_altitude = FLT_MISS; // Miles
	if ((*ap_altitude != FLT_MISS) && (*ap_vspeed != FLT_MISS) &&
	    (*speed_knots != FLT_MISS) && (*pressure_altitude != FLT_MISS) &&
	    (*ap_vspeed != 0.0)) {
	  // 1 mile has 5280 feet
	  // one knot has 101 feet per minute
	  dist_to_altitude = *speed_knots * 101.3 * fabs((*ap_altitude - *pressure_altitude) / *ap_vspeed) / 5280.0;
	  //printf("%f %f %f %f %f \n",*ap_altitude,*pressure_altitude,*ap_vspeed,*speed_knots,dist_to_altitude);
	} 
      
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
    
	// Shift center and rotate about heading
	glTranslatef(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y, 0.0);
	//glRotatef(*track_mag, 0, 0, 1);
	glRotatef(heading_map + *magnetic_variation, 0, 0, 1);
	//glRotatef(*heading_mag, 0, 0, 1);

	// plot ADF / VOR 1/2 heading arrows
	// TODO: extend to co-pilot EFIS selector

	if ((((*efis1_selector == 0) && (*adf1_bearing != FLT_MISS) &&
	     (strcmp((const char*) adf1_name,"") != 0)) || 
	    ((*efis1_selector == 2) && (*nav1_bearing != FLT_MISS) &&
	     (strcmp((const char*) nav1_name,"") != 0))) && (heading_map != FLT_MISS)) {

	  glPushMatrix();     

	  if (*efis1_selector == 0) {
	    glRotatef(-*adf1_bearing, 0, 0, 1);
	    glColor3ub(COLOR_CYAN);
	  } else {
	    glRotatef(-*nav1_bearing, 0, 0, 1);
	    glColor3ub(COLOR_GREEN);
	  }

	  // arrow-head giving heading to NAVAID
	  glLineWidth(lineWidth);
	  glTranslatef(0.0, map_size, 0.0);
	  glBegin(GL_LINE_STRIP);
	  glVertex2f(0.0, m_PhysicalSize.y * -0.085);
	  glVertex2f(0.0, m_PhysicalSize.y * 0.015);
	  glEnd();
	  glBegin(GL_LINE_STRIP);
	  glVertex2f(m_PhysicalSize.x * -0.012, m_PhysicalSize.y * 0.005);
	  glVertex2f(0.0, m_PhysicalSize.x * 0.015);
	  glVertex2f(m_PhysicalSize.x * 0.012, m_PhysicalSize.y * 0.005);
	  glEnd();
	  glBegin(GL_LINE_STRIP);
	  glVertex2f(m_PhysicalSize.x * -0.008, m_PhysicalSize.y * -0.075);
	  glVertex2f(m_PhysicalSize.x * 0.008, m_PhysicalSize.y * -0.075);
	  glEnd();

	  // arrow-tail giving heading from NAVAID
	  glTranslatef(0.0, -2.0*map_size, 0.0);
	  glBegin(GL_LINE_STRIP);
	  glVertex2f(0.0, m_PhysicalSize.y * 0.085);
	  glVertex2f(0.0, m_PhysicalSize.y * -0.015);
	  glEnd();
	  glBegin(GL_LINE_STRIP);
	  glVertex2f(m_PhysicalSize.x * -0.012, m_PhysicalSize.y * -0.015);
	  glVertex2f(0.0, m_PhysicalSize.y * -0.005);
	  glVertex2f(m_PhysicalSize.x * 0.012, m_PhysicalSize.y * -0.015);
	  glEnd();

	  glPopMatrix();
	}
 
	if ((((*efis2_selector == 0) && (*adf2_bearing != FLT_MISS) &&
	     (strcmp((const char*) adf2_name,"") != 0)) || 
	    ((*efis2_selector == 2) && (*nav2_bearing != FLT_MISS) &&
	     (strcmp((const char*) nav2_name,"") != 0))) && (heading_map != FLT_MISS)) {

	  glPushMatrix();     

	  if (*efis2_selector == 0) {
	    glRotatef(-*adf2_bearing, 0, 0, 1);
	    glColor3ub(COLOR_CYAN);
	  } else {
	    glRotatef(-*nav2_bearing, 0, 0, 1);
	    glColor3ub(COLOR_GREEN);
	  }

	  // outline arrow-head giving heading to NAVAID
	  glLineWidth(lineWidth);
	  glTranslatef(0.0, map_size, 0.0);
	  glBegin(GL_LINE_LOOP);
	  glVertex2f(m_PhysicalSize.x * -0.012, m_PhysicalSize.y * -0.075);
	  glVertex2f(m_PhysicalSize.x * -0.012, m_PhysicalSize.y *  0.005);
	  glVertex2f(m_PhysicalSize.x *  0.000, m_PhysicalSize.y *  0.015);
	  glVertex2f(m_PhysicalSize.x *  0.012, m_PhysicalSize.y *  0.005);
	  glVertex2f(m_PhysicalSize.x *  0.012, m_PhysicalSize.y * -0.075);
	  glVertex2f(m_PhysicalSize.x *  0.020, m_PhysicalSize.y * -0.075);
	  glVertex2f(m_PhysicalSize.x *  0.020, m_PhysicalSize.y * -0.085);
	  glVertex2f(m_PhysicalSize.x * -0.020, m_PhysicalSize.y * -0.085);
	  glVertex2f(m_PhysicalSize.x * -0.020, m_PhysicalSize.y * -0.075);
	  glEnd();

	  // outline arrow-tail giving heading from NAVAID
	  glTranslatef(0.0, -2.0*map_size, 0.0);
	  glBegin(GL_LINE_LOOP);
	  glVertex2f(m_PhysicalSize.x * -0.012, m_PhysicalSize.y *  0.000);
	  glVertex2f(m_PhysicalSize.x * -0.012, m_PhysicalSize.y *  0.075);
	  glVertex2f(m_PhysicalSize.x *  0.000, m_PhysicalSize.y *  0.085);
	  glVertex2f(m_PhysicalSize.x *  0.012, m_PhysicalSize.y *  0.075);
	  glVertex2f(m_PhysicalSize.x *  0.012, m_PhysicalSize.y * -0.000);
	  glVertex2f(m_PhysicalSize.x *  0.020, m_PhysicalSize.y * -0.005);
	  glVertex2f(m_PhysicalSize.x *  0.020, m_PhysicalSize.y * -0.015);
	  glVertex2f(m_PhysicalSize.x *  0.000, m_PhysicalSize.y * -0.005);
	  glVertex2f(m_PhysicalSize.x * -0.020, m_PhysicalSize.y * -0.015);
	  glVertex2f(m_PhysicalSize.x * -0.020, m_PhysicalSize.y * -0.005);
	  glEnd();
 
	  glPopMatrix();
	}
    
	/* MCP selected Heading bug */
	if ((*heading_mag_ap != FLT_MISS) && (*magnetic_variation != FLT_MISS) &&
	    (heading_map != FLT_MISS)) {

	  glPushMatrix();     

	  glRotatef(-*heading_mag_ap, 0, 0, 1);
	  glColor3ub(COLOR_MAGENTA);
	  glLineWidth(lineWidth);
	  if (*has_heading_ap_line == 1) {
	    glEnable(GL_LINE_STIPPLE);
	    glLineStipple( 4, 0x0F0F );
	    glBegin(GL_LINES);
	    glVertex2f(0.0,0.0);
	    glVertex2f(0.0,map_size);
	    glEnd();
	    glDisable(GL_LINE_STIPPLE);
	  }
	  glBegin(GL_LINE_STRIP);
	  glVertex2f(m_PhysicalSize.x * -0.020,map_size);
	  glVertex2f(m_PhysicalSize.x * -0.020,map_size+m_PhysicalSize.y * 0.020);
	  glVertex2f(m_PhysicalSize.x * -0.010,map_size+m_PhysicalSize.y * 0.020);
	  glVertex2f(0.0,map_size);
	  glVertex2f(m_PhysicalSize.x * 0.010,map_size+m_PhysicalSize.y * 0.020);
	  glVertex2f(m_PhysicalSize.x * 0.020,map_size+m_PhysicalSize.y * 0.020);
	  glVertex2f(m_PhysicalSize.x * 0.020,map_size);
	  glEnd();

	  glPopMatrix();
	}

	/* end of down-shifted coordinate system */
	glPopMatrix();

	//------ ACF symbol and distance circles -------
	glPushMatrix();
      
	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(0, 0, 0, 1);
      
	glColor3ub(COLOR_WHITE);
	glLineWidth(lineWidth);
	glBegin(GL_LINE_LOOP);
	glVertex2f(m_PhysicalSize.x*(acf_x-0.030), m_PhysicalSize.y*(acf_y-0.070));
	glVertex2f(m_PhysicalSize.x*(acf_x+0.030), m_PhysicalSize.y*(acf_y-0.070));
	glVertex2f(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y);
	glEnd();

	if (heading_map != FLT_MISS) {
	  glColor3ub(COLOR_WHITE);
	  glBegin(GL_LINE_STRIP);
	  glVertex2f(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*(acf_y+0.080));
	  glVertex2f(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*map_y_max);
	  glEnd();
	} else {
	  glColor3ub(COLOR_ORANGE);
	  glTranslatef(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y+map_size*0.35, 0);
	  m_pFontManager->SetSize( m_Font, 1.30*fontSize, 1.30*fontSize );
	  m_pFontManager->Print( -2.0*fontSize, 0.0,"MAP", m_Font);
	  glBegin(GL_LINE_LOOP);
	  glVertex2f(-2.2*fontSize, 1.6*fontSize);
	  glVertex2f(-2.2*fontSize, -0.3*fontSize);
	  glVertex2f(2.2*fontSize, -0.3*fontSize);
	  glVertex2f(2.2*fontSize, 1.6*fontSize);
	  glEnd();	  
	}
      
	glPopMatrix();

	// plot range scale
	glPushMatrix();
	glColor3ub(COLOR_WHITE);
	glTranslatef(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y, 0);
	m_pFontManager->SetSize( m_Font, 0.75*fontSize, 0.75*fontSize );
	if (mapRange >= 5.0) {
	  snprintf(buffer, sizeof(buffer), "%i", (int) (mapRange / 2.0));
	} else {
	  snprintf(buffer, sizeof(buffer), "%2.1f", mapRange / 2.0);
	}
	if ((mapRange / 2.0) < 100) {
	  if (mapRange >= 5.0) {
	    m_pFontManager->Print( -1.5*fontSize, map_size*0.505, &buffer[0], m_Font);
	  } else {
	    m_pFontManager->Print( -2.0*fontSize, map_size*0.505, &buffer[0], m_Font);
	  }
	} else {
	  m_pFontManager->Print( -2.25*fontSize, map_size*0.505, &buffer[0], m_Font);
	}
	glPopMatrix();
    
	// Set up big circle for nav range
	if (!mapCenter) {
	  glPushMatrix();
	
	  glLineWidth(lineWidth);
	  glTranslatef(0.0, 0.0, 0.0);
	  CircleEvaluator bCircle;
	  bCircle.SetDegreesPerPoint(2);

	  if (heading_map != FLT_MISS) {
	    bCircle.SetArcStartEnd(270,90);
	    bCircle.SetRadius(map_size*1.0);
	    bCircle.SetOrigin(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y);
	    glBegin(GL_LINE_STRIP);
	    bCircle.Evaluate();
	    glEnd();
	  }

	  bCircle.SetArcStartEnd(270,90);
	  bCircle.SetRadius(map_size*0.75);
	  bCircle.SetOrigin(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y);
	  glBegin(GL_LINE_STRIP);
	  bCircle.Evaluate();
	  glEnd();

	  bCircle.SetArcStartEnd(270,90);
	  bCircle.SetRadius(map_size*0.5);
	  bCircle.SetOrigin(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y);
	  glBegin(GL_LINE_STRIP);
	  bCircle.Evaluate();
	  glEnd();

	  bCircle.SetArcStartEnd(270,90);
	  bCircle.SetRadius(map_size*0.25);
	  bCircle.SetOrigin(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y);
	  glBegin(GL_LINE_STRIP);
	  bCircle.Evaluate();
	  glEnd();

	  if (dist_to_altitude != FLT_MISS) {
	    /* set up distance to AP-dialed altitude in V/S mode */
	    glColor3ub(COLOR_GREEN);
	    bCircle.SetArcStartEnd(345,15);
	    bCircle.SetRadius(map_size/mapRange*dist_to_altitude);
	    bCircle.SetOrigin(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y);
	    glBegin(GL_LINE_STRIP);
	    bCircle.Evaluate();
	    glEnd();
	  }

	  glPopMatrix();
	}
	
	// set up tick marks
	if ((heading_map != FLT_MISS) && (*magnetic_variation != FLT_MISS)) {
	  glColor3ub(COLOR_WHITE);
	  glPushMatrix();
	  glTranslatef(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y, 0);
	  //	  glRotatef(*track_mag,0,0,1);
	  glRotatef(heading_map + *magnetic_variation,0,0,1);
	  //glRotatef(*heading_mag,0,0,1);
	  for ( int i=0; i<=71; i++ ) {
	    float ticklen;
	    if ( i % 2 == 0 ) {
	      ticklen = 0.040;
	    } else {
	      ticklen = 0.020;
	    }
	    glLineWidth(lineWidth);
	    glBegin(GL_LINE_STRIP);
	    glVertex2f(0,map_size);
	    glVertex2f(0,m_PhysicalSize.y*(map_y_max-acf_y-ticklen));
	    glEnd();
	    
	    if ( i % 6 == 0 ) {
	      m_pFontManager->SetSize( m_Font, 0.75*fontSize, 0.75*fontSize );
	      
	      int deg10 = i*5/10;
	      snprintf(buffer, sizeof(buffer), "%i", deg10);
	      if (deg10 < 10) {
		m_pFontManager->Print( -0.4*0.75*fontSize, m_PhysicalSize.y*(map_y_max-acf_y-ticklen-0.005)-0.75*fontSize, &buffer[0], m_Font);
	      } else {
		m_pFontManager->Print( -0.8*0.75*fontSize, m_PhysicalSize.y*(map_y_max-acf_y-ticklen-0.005)-0.75*fontSize, &buffer[0], m_Font);
	      }
	    }
	    glRotatef(-5.0,0,0,1);
	  }
	  
	  glPopMatrix();

	  
	  glPushMatrix();
	  glTranslatef(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y, 0);
	  glRotatef(heading_map - *heading_mag + *magnetic_variation,0,0,1); // put wind correction rotation here
	  
	  // TRIANGLE ON TOP OF THE MAP MARKS THE WIND CORRECTION!
	  glColor3ub(COLOR_WHITE);
	  glBegin(GL_LINE_LOOP);
	  glVertex2f(0.0,map_size);
	  glVertex2f(0.02*m_PhysicalSize.x,map_size*1.045);
	  glVertex2f(-0.02*m_PhysicalSize.x,map_size*1.045);
	  glEnd();
	  glPopMatrix();

	}
	
      }
      
    }
    
  }

} // end namespace OpenGC

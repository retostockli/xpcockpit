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
#include "B737/B737NAV/ogcB737NAVDrawStatic.h"

namespace OpenGC
{
  
  B737NAVDrawStatic::B737NAVDrawStatic()
  {
    printf("B737NAVDrawStatic constructed\n");
    
    m_Font = m_pFontManager->LoadDefaultFont();
    
    m_NAVGauge = NULL;

  }

  B737NAVDrawStatic::~B737NAVDrawStatic()
  {
    // Destruction handled by base class
  }

  void B737NAVDrawStatic::Render()
  {
    GaugeComponent::Render();

    int acf_type = m_pDataSource->GetAcfType();
  
    bool is_captain = (this->GetArg() == 0);
    bool is_copilot = (this->GetArg() == 1);

    // bool mapCenter = m_NAVGauge->GetMapCenter();
    int mapMode = m_NAVGauge->GetMapMode();
    // float mapRange = m_NAVGauge->GetMapRange();
    float heading_map =  m_NAVGauge->GetMapHeading();
 
    // define geometric stuff
    float fontSize = 4.0 * m_PhysicalSize.x / 150.0;
    float lineWidth = 1.0 * m_PhysicalSize.x / 100.0;
   
    char buffer[36];
    
    // double dtor = 0.0174533; /* radians per degree */
    // double radeg = 57.2958;  /* degree per radians */

    // define ACF center position in relative coordinates
    /*
      float acf_x;
      float acf_y;
      if (mapCenter) {
      acf_x = 0.500;
      acf_y = 0.500;
      } else {
      acf_x = 0.500;
      acf_y = 0.200;
      }
    */
    //    float map_x_min = 0.000;
    //    float map_x_max = 1.000;
    //    float map_y_min = 0.040;
    // float map_y_max = 0.888;
    // float map_size = m_PhysicalSize.y*(map_y_max-acf_y);
     
    // What's the heading?
    //float *heading_true = link_dataref_flt("sim/flightmodel/position/psi",-1);
    float *heading_mag = link_dataref_flt("sim/flightmodel/position/magpsi",-1);
    float *track_mag;
    if (is_captain) {
      track_mag = link_dataref_flt("sim/cockpit2/gauges/indicators/ground_track_mag_pilot",-1);
    } else {
      track_mag = link_dataref_flt("sim/cockpit2/gauges/indicators/ground_track_mag_copilot",-1);
    }
    
    /* link x-plane datarefs needed for NAV display */
    float *ground_speed = link_dataref_flt("sim/flightmodel/position/groundspeed",0);
    float *air_speed = link_dataref_flt("sim/flightmodel/position/true_airspeed",0);
    float *wind_direction_mag = link_dataref_flt("sim/cockpit2/gauges/indicators/wind_heading_deg_mag",0);
    // float *wind_direction_mag = link_dataref_flt("sim/weather/wind_direction_degt",0);
    float *wind_speed = link_dataref_flt("sim/cockpit2/gauges/indicators/wind_speed_kts",0);

    int *efis1_selector;
    int *efis2_selector;

    if ((acf_type == 2) || (acf_type == 3)) {
      if (is_captain) {
	efis1_selector = link_dataref_int("laminar/B738/EFIS_control/capt/vor1_off_pos");
	efis2_selector = link_dataref_int("laminar/B738/EFIS_control/capt/vor2_off_pos");
      } else {
	efis1_selector = link_dataref_int("laminar/B738/EFIS_control/fo/vor1_off_pos");
	efis2_selector = link_dataref_int("laminar/B738/EFIS_control/fo/vor2_off_pos");
      }
    } else {
      if (is_captain) {
	efis1_selector = link_dataref_int("sim/cockpit2/EFIS/EFIS_1_selection_pilot");
	efis2_selector = link_dataref_int("sim/cockpit2/EFIS/EFIS_2_selection_pilot");
      } else {
	efis1_selector = link_dataref_int("sim/cockpit2/EFIS/EFIS_1_selection_copilot");
	efis2_selector = link_dataref_int("sim/cockpit2/EFIS/EFIS_2_selection_copilot");
      }
    }
    //    unsigned char *nav1_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/nav1_nav_id",500,-1);
    //    unsigned char *nav2_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/nav2_nav_id",500,-1);
    //    unsigned char *adf1_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/adf1_nav_id",500,-1);
    //    unsigned char *adf2_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/adf2_nav_id",500,-1);
    unsigned char *nav1_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/nav1_nav_id",150,-1);
    unsigned char *nav2_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/nav2_nav_id",150,-1);
    unsigned char *adf1_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/adf1_nav_id",150,-1);
    unsigned char *adf2_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/adf2_nav_id",150,-1);
    int *nav1_has_dme = link_dataref_int("sim/cockpit2/radios/indicators/nav1_has_dme");
    int *nav2_has_dme = link_dataref_int("sim/cockpit2/radios/indicators/nav2_has_dme");
    int *adf1_has_dme = link_dataref_int("sim/cockpit2/radios/indicators/adf1_has_dme");
    int *adf2_has_dme = link_dataref_int("sim/cockpit2/radios/indicators/adf2_has_dme");
    float *nav1_dme_distance_nm = link_dataref_flt("sim/cockpit2/radios/indicators/nav1_dme_distance_nm",-1);
    float *nav2_dme_distance_nm = link_dataref_flt("sim/cockpit2/radios/indicators/nav2_dme_distance_nm",-1);
    float *adf1_dme_distance_nm = link_dataref_flt("sim/cockpit2/radios/indicators/adf1_dme_distance_nm",-1);
    float *adf2_dme_distance_nm = link_dataref_flt("sim/cockpit2/radios/indicators/adf2_dme_distance_nm",-1);

    if ((*track_mag != FLT_MISS) && (*heading_mag != FLT_MISS)) {
      
      // Shift center and rotate about heading
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      /* print small font stuff */
      if (mapMode != 3) {
	if (heading_map != FLT_MISS) {
	  m_pFontManager->SetSize( m_Font, fontSize, fontSize );
	  glColor3ub(COLOR_GREEN);
	  m_pFontManager->Print( m_PhysicalSize.x*0.355, m_PhysicalSize.y*0.932, "TRK", m_Font);
	  m_pFontManager->Print( m_PhysicalSize.x*0.571, m_PhysicalSize.y*0.932, "MAG", m_Font);
	}
      }
	
      m_pFontManager->SetSize( m_Font, 0.75*fontSize, 0.75*fontSize );
      glColor3ub( COLOR_WHITE );
      m_pFontManager->Print( m_PhysicalSize.x*0.014,m_PhysicalSize.y*0.957,"GS", m_Font );
      m_pFontManager->Print( m_PhysicalSize.x*0.150,m_PhysicalSize.y*0.957,"TAS", m_Font );

      if (mapMode != 3) {
	/* Draws the box around the heading number displayed at top-center */
	glLineWidth( lineWidth );
	glBegin(GL_LINE_STRIP);
	glVertex2f( m_PhysicalSize.x*0.443, m_PhysicalSize.y*0.980);
	glVertex2f( m_PhysicalSize.x*0.443, m_PhysicalSize.y*0.922);
	glVertex2f( m_PhysicalSize.x*0.557, m_PhysicalSize.y*0.922);
	glVertex2f( m_PhysicalSize.x*0.557, m_PhysicalSize.y*0.980);
	glEnd();
      }

      /* put all the dynamic data fields that appear on all NAV display modes */

      if (mapMode != 3) {
	/* big heading number on top of NAV display */
	if (*track_mag != FLT_MISS) {
	  m_pFontManager->SetSize( m_Font, 1.30*fontSize, 1.30*fontSize );
	  if (heading_map != FLT_MISS) {
	    glColor3ub( COLOR_WHITE );
	    snprintf(buffer, sizeof(buffer), "%03d", (int) lroundf(*track_mag));
	  } else {
	    glColor3ub( COLOR_ORANGE );
	    snprintf(buffer, sizeof(buffer), "TRK");	    
	  }
	  m_pFontManager->Print( m_PhysicalSize.x*0.452, m_PhysicalSize.y*0.93, &buffer[0], m_Font);
	}
      }
      
      glColor3ub( COLOR_WHITE );
      m_pFontManager->SetSize( m_Font, fontSize, fontSize );

      /* plot ground speed and air speed in knots (convert from m/s) */
      if (*ground_speed != FLT_MISS) {
	snprintf(buffer, sizeof(buffer), "%d", (int) lroundf(*ground_speed * 1.943844));
      } else {
	snprintf(buffer, sizeof(buffer), "---");
      }	
      m_pFontManager->Print( m_PhysicalSize.x*0.060, m_PhysicalSize.y*0.957 , buffer, m_Font );

      if (*air_speed != FLT_MISS) {
	snprintf(buffer, sizeof(buffer), "%d", (int) lroundf(*air_speed * 1.943844));
      } else {
	snprintf(buffer, sizeof(buffer), "---");
      }
      m_pFontManager->Print( m_PhysicalSize.x*0.210, m_PhysicalSize.y*0.957 , buffer, m_Font );

      /* plot wind speed and direction and draw wind arrow */
      if ((*wind_direction_mag != FLT_MISS) && (*wind_speed != FLT_MISS) && (*wind_speed > 0.0)) {
	snprintf(buffer, sizeof(buffer), "%03d°", (int) lroundf(*wind_direction_mag));
      } else {
	snprintf(buffer, sizeof(buffer), "---°");
      }
      m_pFontManager->Print( m_PhysicalSize.x*0.014, m_PhysicalSize.y*0.919 , buffer, m_Font );
  
      if ((*wind_direction_mag != FLT_MISS) && (heading_map != FLT_MISS) &&
	  (*wind_speed != FLT_MISS) && (*wind_speed > 0.0)) {
	glPushMatrix();
	glTranslatef(m_PhysicalSize.x*0.055, m_PhysicalSize.y*0.878, 0);
	glRotatef(180.0 - (*wind_direction_mag - *heading_mag),0,0,1);
	glLineWidth( lineWidth );
	glBegin(GL_LINE_STRIP);
	glVertex2f( m_PhysicalSize.x*0.0, -m_PhysicalSize.y*0.035);
	glVertex2f( m_PhysicalSize.x*0.0, m_PhysicalSize.y*0.035);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2f( m_PhysicalSize.x*0.01, m_PhysicalSize.y*0.025);
	glVertex2f( m_PhysicalSize.x*0.0, m_PhysicalSize.y*0.035);
	glVertex2f( -m_PhysicalSize.x*0.01, m_PhysicalSize.y*0.025);
	glEnd();
	glPopMatrix();    
      }

      m_pFontManager->Print( m_PhysicalSize.x*0.090, m_PhysicalSize.y*0.919 , " /", m_Font );
      if ((*wind_speed != FLT_MISS) && (*wind_speed > 0.0)) {
	snprintf(buffer, sizeof(buffer), "%d", (int) *wind_speed);
      } else {
	snprintf(buffer, sizeof(buffer), "---");
      }
      m_pFontManager->Print( m_PhysicalSize.x*0.128, m_PhysicalSize.y*0.919 , buffer, m_Font );

      if (mapMode != 3) {
      
	/* plot NAVAID name and DME if available on the lower part of the NAV display */
	if ((*efis1_selector == -1) || (*efis1_selector == 1) || (*efis1_selector == 2)) {

	  /* Draw black background to obscure station / tcas symbols */
	  glColor3ub(COLOR_BLACK);
	  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	  glBegin(GL_POLYGON);
	  glVertex2f(0,0);
	  glVertex2f(0,m_PhysicalSize.y*0.15);
	  glVertex2f(m_PhysicalSize.x*0.2,m_PhysicalSize.y*0.15);
	  glVertex2f(m_PhysicalSize.x*0.2,0);
	  glEnd();
	  
	  m_pFontManager->SetSize( m_Font, fontSize, fontSize );
	  
	  if (((*efis1_selector == -1) && ((acf_type == 2) || (acf_type == 3))) ||
	      ((*efis1_selector == 1) && ((acf_type != 2) && (acf_type != 3)))) {

	    glColor3ub(COLOR_CYAN);
	    m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.092 , "ADF 1", m_Font );
	    snprintf( buffer, sizeof(buffer), "%s", adf1_name );
	    if (strcmp(buffer,"") == 0) {
	      snprintf( buffer, sizeof(buffer), "%s", "------" );
	    } 
	    m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.052 , buffer, m_Font );
	    if (*adf1_has_dme == 1) {
	      snprintf( buffer, sizeof(buffer), "%0.1f", *adf1_dme_distance_nm );
	    } else {
	      snprintf( buffer, sizeof(buffer), "%s", "---" );
	    }
	    m_pFontManager->Print( m_PhysicalSize.x*0.080, m_PhysicalSize.y*0.012 , buffer, m_Font );
	    m_pFontManager->SetSize( m_Font, 0.75*fontSize, 0.75*fontSize );
	    m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.012 , "DME", m_Font );
	  } else {
	    glColor3ub(COLOR_GREEN);
	    m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.092 , "VOR 1", m_Font );
	    snprintf( buffer, sizeof(buffer), "%s", nav1_name );
	    if (strcmp(buffer,"") == 0) {
	      snprintf( buffer, sizeof(buffer), "%s", "------" );
	    } 
	    m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.052 , buffer, m_Font );
	    if (*nav1_has_dme == 1) {
	      snprintf( buffer, sizeof(buffer), "%0.1f", *nav1_dme_distance_nm );
	    } else {
	      snprintf( buffer, sizeof(buffer), "%s", "---" );
	    }
	    m_pFontManager->Print( m_PhysicalSize.x*0.080, m_PhysicalSize.y*0.012 , buffer, m_Font );
	    m_pFontManager->SetSize( m_Font, 0.75*fontSize, 0.75*fontSize );
	    m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.012 , "DME", m_Font );
	  }

	}

	if ((*efis2_selector == -1) || (*efis2_selector == 1) || (*efis2_selector == 2)) {

	  /* Draw black background to obscure station / tcas symbols */
	  glColor3ub(COLOR_BLACK);
	  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	  glBegin(GL_POLYGON);
	  glVertex2f(m_PhysicalSize.x,0);
	  glVertex2f(m_PhysicalSize.x,m_PhysicalSize.y*0.15);
	  glVertex2f(m_PhysicalSize.x*0.8,m_PhysicalSize.y*0.15);
	  glVertex2f(m_PhysicalSize.x*0.8,0);
	  glEnd();
	  
	  m_pFontManager->SetSize( m_Font, fontSize, fontSize );
    
	  if (((*efis2_selector == -1) && ((acf_type == 2) || (acf_type == 3))) ||
	      ((*efis2_selector == 1) && ((acf_type != 2) && (acf_type != 3)))) {

	    glColor3ub(COLOR_CYAN);
	    m_pFontManager->Print( m_PhysicalSize.x*0.839, m_PhysicalSize.y*0.092 , "ADF 2", m_Font );
	    snprintf( buffer, sizeof(buffer), "%s", adf2_name );
	    if (strcmp(buffer,"") == 0) {
	      snprintf( buffer, sizeof(buffer), "%s", "------" );
	    } 
	    m_pFontManager->Print( m_PhysicalSize.x*0.839, m_PhysicalSize.y*0.052 , buffer, m_Font );
	    if (*adf2_has_dme == 1) {
	      snprintf( buffer, sizeof(buffer), "%0.1f", *adf2_dme_distance_nm );
	    } else {
	      snprintf( buffer, sizeof(buffer), "%s", "---" );
	    }
	    m_pFontManager->Print( m_PhysicalSize.x*(0.839+0.067), m_PhysicalSize.y*0.012 , buffer, m_Font );      
	    m_pFontManager->SetSize( m_Font, 0.75*fontSize, 0.75*fontSize );
	    m_pFontManager->Print( m_PhysicalSize.x*0.839, m_PhysicalSize.y*0.012 , "DME", m_Font );
	  } else {
	    glColor3ub(COLOR_GREEN);
	    m_pFontManager->Print( m_PhysicalSize.x*0.839, m_PhysicalSize.y*0.092 , "VOR 2", m_Font );
	    snprintf( buffer, sizeof(buffer), "%s", nav2_name );
	    if (strcmp(buffer,"") == 0) {
	      snprintf( buffer, sizeof(buffer), "%s", "------" );
	    } 
	    m_pFontManager->Print( m_PhysicalSize.x*0.839, m_PhysicalSize.y*0.052 , buffer, m_Font );
	    if (*nav2_has_dme == 1) {
	      snprintf( buffer, sizeof(buffer), "%0.1f", *nav2_dme_distance_nm );
	    } else {
	      snprintf( buffer, sizeof(buffer), "%s", "---" );
	    }
	    m_pFontManager->Print( m_PhysicalSize.x*(0.839+0.067), m_PhysicalSize.y*0.012 ,buffer,m_Font);
	    m_pFontManager->SetSize( m_Font, 0.75*fontSize, 0.75*fontSize );
	    m_pFontManager->Print( m_PhysicalSize.x*0.839, m_PhysicalSize.y*0.012 , "DME", m_Font );
	  }

	}
	
      }
  
      glPopMatrix();

    } // valid heading

  }    

} // end namespace OpenGC

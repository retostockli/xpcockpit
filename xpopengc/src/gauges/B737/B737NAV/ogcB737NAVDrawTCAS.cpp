/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737DrawTCAS.cpp,v $

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

    bool mapCenter = m_NAVGauge->GetMapCenter();
    int mapMode = m_NAVGauge->GetMapMode();
    float mapRange = m_NAVGauge->GetMapRange();

    char buffer[15];
    char buffer2[50];
    int maxmp = 19;
    int maxtcas = 20;
    
    // define geometric stuff
    float fontSize = 4.0 * m_PhysicalSize.x / 150.0;
    // float lineWidth = 1.5 * m_PhysicalSize.x / 100.0;

    double dtor = 0.0174533; /* radians per degree */
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
    double *aircraftLat = link_dataref_dbl("sim/flightmodel/position/latitude",-5);
    double *aircraftLon = link_dataref_dbl("sim/flightmodel/position/longitude",-5);
    float *aircraftAlt = link_dataref_flt("sim/flightmodel/position/elevation",0); // meters
    
    // What's the heading?
    float *heading_true = link_dataref_flt("sim/flightmodel/position/psi",-1);
        
    /* TCAS Datarefs (AI Planes) */
    float *tcas_heading = link_dataref_flt_arr("sim/cockpit2/tcas/indicators/relative_bearing_degs",maxtcas,-1,0);
    float *tcas_distance = link_dataref_flt_arr("sim/cockpit2/tcas/indicators/relative_distance_mtrs",maxtcas,-1,1);
    float *tcas_altitude = link_dataref_flt_arr("sim/cockpit2/tcas/indicators/relative_altitude_mtrs",maxtcas,-1,1);

    /* Multiplayer Datarefs (XSquawkbox) */
    float mp_lon[maxmp];
    float mp_lat[maxmp];
    float mp_alt[maxmp];
    for (int i=0;i<maxmp;i++) {
      snprintf(buffer2,sizeof(buffer2),"sim/multiplayer/position/plane%i_x",i+1);
      float *px = link_dataref_flt(buffer2,0); // 0 if no plane exists
      if ((*px != 0.0) && (*px != FLT_MISS)) {
	snprintf(buffer2,sizeof(buffer2),"sim/multiplayer/position/plane%i_lon",i+1);
	float *plon = link_dataref_flt(buffer2,-3);
	if ((*plon != 0.0) && (*plon != FLT_MISS)) {
	  mp_lon[i] = *plon;
	} else {
	  mp_lon[i] = FLT_MISS;
	}
	snprintf(buffer2,sizeof(buffer2),"sim/multiplayer/position/plane%i_lat",i+1);
	float *plat = link_dataref_flt(buffer2,-3);
	if ((*plat != 0.0) && (*plat != FLT_MISS)) {
	  mp_lat[i] = *plat;
	} else {
	  mp_lat[i] = FLT_MISS;
	}
	snprintf(buffer2,sizeof(buffer2),"sim/multiplayer/position/plane%i_el",i+1);
	float *palt = link_dataref_flt(buffer2,1); // elevation in meters
	if ((*palt != 0.0) && (*palt != FLT_MISS)) {
	  mp_alt[i] = *palt;
	} else {
	  mp_alt[i] = FLT_MISS;
	}
      } else {
	mp_lon[i] = FLT_MISS;
	mp_lat[i] = FLT_MISS;
	mp_alt[i] = FLT_MISS;
      }
      
    }
    
    // The input coordinates are in lon/lat, so we have to rotate against true heading
    // despite the NAV display is showing mag heading
    if (*heading_true != FLT_MISS) {

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
    
      // Shift center and rotate about heading
      glTranslatef(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y, 0.0);
      glRotatef(*heading_true, 0, 0, 1);


      /* valid coordinates ? */
      if ((*aircraftLon >= -180.0) && (*aircraftLon <= 180.0) && (*aircraftLat >= -90.0) && (*aircraftLat <= 90.0)) {
        
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

	// plot TCAS of AI aircraft first and then of multiplayer planes
	for (int i=0;i<(maxtcas+maxmp);i++) {

	  xPos = FLT_MISS;
	  yPos = FLT_MISS;
	  zPos = FLT_MISS;
	  if (i<maxtcas) {
	    if ((*(tcas_distance+i) > 0.0) && (*(tcas_heading+i) != FLT_MISS) &&
		(*(tcas_altitude+i) != FLT_MISS)) {

	      float rotateRad = (*(tcas_heading+i) + *heading_true) * dtor;

	      xPos = *(tcas_distance+i) * sin(rotateRad) / 1852.0 / mapRange * map_size; 
	      yPos = *(tcas_distance+i) * cos(rotateRad) / 1852.0 / mapRange * map_size;
	      zPos = *(tcas_altitude+i) *3.28084; // altitude difference to our ACF
	      //printf("%i %f %f %f \n",i,*(tcas_distance+i),*(tcas_heading+i),*(tcas_altitude+i));
	    }
	  } else {
	    int j = i - maxtcas;
	    if ((mp_lon[j] != FLT_MISS) && (mp_lat[j] != FLT_MISS) && (mp_alt[j] != FLT_MISS)) {
	      double lon = (double) mp_lon[j];
	      double lat = (double) mp_lat[j];
	      double easting;
	      double northing;
	      lonlat2gnomonic(&lon, &lat, &easting, &northing, aircraftLon, aircraftLat);
	      
	      // Compute physical position relative to acf center on screen
	      yPos = -northing / 1852.0 / mapRange * map_size; 
	      xPos = easting / 1852.0  / mapRange * map_size;
	      zPos = (mp_alt[j] - *aircraftAlt)*3.28084;
	      // printf("%i %f %f %f %f %f \n",j,xPos,yPos,zPos,mp_alt[j]*3.28084,*aircraftAlt*3.28084);
	    }
	  }

	  if ((xPos != FLT_MISS) && (yPos != FLT_MISS) && (zPos != FLT_MISS)) {	    

	    glPushMatrix();

	    glTranslatef(xPos, yPos, 0.0);
	    glRotatef(-1.0* *heading_true, 0, 0, 1);

	    glLineWidth(2.0);

	    float ss2 = 0.65*ss;
	    if (fabs(zPos) < 300.) {
	      // Resolution Advisory (red filled square)
	      glColor3ub(255, 0, 0);
	      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	      glBegin(GL_POLYGON);
	      glVertex2f(-ss2, -ss2);
	      glVertex2f(ss2, -ss2);
	      glVertex2f(ss2, ss2);
	      glVertex2f(-ss2, ss2);
	      glEnd();
	    } else if (fabs(zPos) < 900.) {
	      // traffic advisory (yellow filled circle)
	      glColor3ub(255, 255, 0);
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
	      glColor3ub(255, 255, 255);
	      glPolygonMode(GL_FRONT,GL_LINE);
	      glBegin(GL_POLYGON);
	      glVertex2f(0, -ss2);
	      glVertex2f(ss2, 0);
	      glVertex2f(0, ss2);
	      glVertex2f(-ss2, 0);
	      glEnd();
	      glPolygonMode(GL_FRONT,GL_FILL);
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

	// TODO: plot TCAS of Multiplayer aircraft


	
      } // valid acf coordinates
    
      glPopMatrix();

    }
    
  }

} // end namespace OpenGC

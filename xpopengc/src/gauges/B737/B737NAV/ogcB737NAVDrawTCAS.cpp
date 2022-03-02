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

    for (int i=0;i<MAXMP;i++) {
      mp_x[i] = FLT_MISS;
      mp_x_save[i] = FLT_MISS;
      mp_lon[i] = FLT_MISS;
      mp_lat[i] = FLT_MISS;
      mp_alt[i] = FLT_MISS;
      mp_alive[i] = 0;
    }
    count = 0;
    maxcount = 50; // the OpenGC updates 50x per second, so check for plane movement every X seconds

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

    // Where is our aircraft?
    double aircraftLon = m_NAVGauge->GetMapCtrLon();
    double aircraftLat = m_NAVGauge->GetMapCtrLat();
    float *aircraftAlt = link_dataref_flt("sim/flightmodel/position/elevation",0); // meters
    float *altitude_agl = link_dataref_flt("sim/flightmodel/position/y_agl",0);
    
    // What's the heading?
    float heading_map =  m_NAVGauge->GetMapHeading();
        
    /* TCAS Datarefs (AI Planes) */
    /*
    float *tcas_heading = link_dataref_flt_arr("sim/cockpit2/tcas/indicators/relative_bearing_degs",MAXTCAS,-1,0);
    float *tcas_distance = link_dataref_flt_arr("sim/cockpit2/tcas/indicators/relative_distance_mtrs",MAXTCAS,-1,1);
    float *tcas_altitude = link_dataref_flt_arr("sim/cockpit2/tcas/indicators/relative_altitude_mtrs",MAXTCAS,-1,1);*/
    
    /* Multiplayer Datarefs (XSquawkbox + AI Planes): max of 19 planes.
       But e.g. XSquawkbox recycles the datarefs for different planes, so generate
       our own database of MAXMP planes. */
    int j;
    for (int i=0;i<19;i++) {
      snprintf(buffer2,sizeof(buffer2),"sim/multiplayer/position/plane%i_x",i+1);
      float *px = link_dataref_flt(buffer2,0); // 0 if no plane exists
      snprintf(buffer2,sizeof(buffer2),"sim/multiplayer/position/plane%i_lon",i+1);
      float *plon = link_dataref_flt(buffer2,-3);
      snprintf(buffer2,sizeof(buffer2),"sim/multiplayer/position/plane%i_lat",i+1);
      float *plat = link_dataref_flt(buffer2,-3);
      snprintf(buffer2,sizeof(buffer2),"sim/multiplayer/position/plane%i_el",i+1);
      float *palt = link_dataref_flt(buffer2,1); // elevation in meters
      if ((*plon != 0.0) && (*plon != FLT_MISS) &&
	  (*plat != 0.0) && (*plat != FLT_MISS) &&
	  (*palt != 0.0) && (*palt != FLT_MISS) &&
	  (*palt < 1e6) &&   // we had some infinite altitudes around
	  (*px != 0.0) && (*px != FLT_MISS)) {

	int jmin = -1;
	int javail = -1;
	float minval = 100.0;
	  
	for (j=0;j<MAXMP;j++) {
	  if ((mp_lon[j] != FLT_MISS) && (mp_lat[j] != FLT_MISS)) {
	    float val = fabs(*plon - mp_lon[j]) + fabs(*plat - mp_lat[j]);
	    if (val < minval) {
	      minval = val;
	      jmin = j;
	    }
	  } else {
	    if (javail < 0) javail = j;
	  }
	}
	if ((jmin >= 0) && (minval < 0.005)) {
	  // FOUND EXISTING: update TCAS position
	  j=jmin;
	  //printf("Found: %i %f %f %f %f \n",j,*plon,mp_lon[j],*plat,mp_lat[j]);
	  mp_x[j] = *px;
	  mp_lon[j] = *plon;
	  mp_lat[j] = *plat;
	  mp_alt[j] = *palt;
	} else if (count == 0) {
	  /* Start of check for alive period (count 0) */
	  j = javail; // first available (empty) TCAS index
	  /* save new TCAS blib */
	  //	  printf("New: %i %f %f %f %f \n",j,*plon,*plat,*palt,minval);
	  mp_x[j] = *px;
	  mp_x_save[j] = *px;
	  mp_lon[j] = *plon;
	  mp_lat[j] = *plat;
	  mp_alt[j] = *palt;
	}
      }      
    }
    if (count == maxcount) {	      
      /* Check if planes did move, if they did, they are alive:
	 If multiplayer planes leave their position datarefs still 
	 show values. But position stays constant */

      int number = 0;
      for (j=0;j<MAXMP;j++) {
	if (mp_x[j] != FLT_MISS) {
	  if (mp_x_save[j] == mp_x[j]) {
	    //	    printf("Remove: %i %f %f %f \n",j,mp_lon[j],mp_lat[j],mp_alt[j]);
	    mp_alive[j] = 0;
	    mp_x[j] = FLT_MISS;
	    mp_x_save[j] = FLT_MISS;
	    mp_lon[j] = FLT_MISS;
	    mp_lat[j] = FLT_MISS;
	    mp_alt[j] = FLT_MISS;
	  } else {
	    mp_alive[j] = 1;
	    number++;
	  }
	  mp_x_save[j] = mp_x[j];
	}
      }
      //      printf("TCAS ALIVE : %i \n",number);
      count = 0;
    } else {
      count ++;
    }
    
    // The input coordinates are in lon/lat, so we have to rotate against true heading
    // despite the NAV display is showing mag heading
    if (heading_map != FLT_MISS) {

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
    
      // Shift center and rotate about heading
      glTranslatef(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y, 0.0);
      glRotatef(heading_map, 0, 0, 1);


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
	// for (int i=0;i<MAXTCAS;i++) {
	for (int i=0;i<MAXMP;i++) {
	  
	  xPos = FLT_MISS;
	  yPos = FLT_MISS;
	  zPos = FLT_MISS;
	  /*
	  if ((*(tcas_distance+i) > 0.0) && (*(tcas_heading+i) != FLT_MISS) &&
	      (*(tcas_altitude+i) != FLT_MISS)) {
	    
	    float rotateRad = (*(tcas_heading+i) + heading_map) * dtor;
	    
	    xPos = *(tcas_distance+i) * sin(rotateRad) / 1852.0 / mapRange * map_size; 
	    yPos = *(tcas_distance+i) * cos(rotateRad) / 1852.0 / mapRange * map_size;
	    zPos = *(tcas_altitude+i) *3.28084; // altitude difference to our ACF
	    //printf("%i %f %f %f \n",i,*(tcas_distance+i),*(tcas_heading+i),*(tcas_altitude+i));
	  }
	  */
	  //printf("%i %i %f %f %f \n",i,mp_alive[i],mp_lon[i],mp_lat[i],mp_alt[i]);
	  
	  if ((mp_lon[i] != FLT_MISS) && (mp_lat[i] != FLT_MISS) &&
	      (mp_alt[i] != FLT_MISS) && (mp_alive[i] == 1)) {
	    double lon = (double) mp_lon[i];
	    double lat = (double) mp_lat[i];
	    double easting;
	    double northing;
	    lonlat2gnomonic(&lon, &lat, &easting, &northing, &aircraftLon, &aircraftLat);
	    
	    // Compute physical position relative to acf center on screen
	    yPos = -northing / 1852.0 / mapRange * map_size; 
	    xPos = easting / 1852.0  / mapRange * map_size;
	    zPos = (mp_alt[i] - *aircraftAlt)*3.28084;
	    // printf("%i %f %f %f %f %f \n",j,xPos,yPos,zPos,mp_alt[i]*3.28084,*aircraftAlt*3.28084);
	  }

	  /* plot all traffic in the air (not the one on the ground) */
	  if ((xPos != FLT_MISS) && (yPos != FLT_MISS) && (zPos != FLT_MISS) &&
	      (((fabs(zPos) > 10.) && (*altitude_agl < 10.)) || (*altitude_agl >= 10.))) {	    

	    //printf("%i %f \n",i,mp_alt[i]-*aircraftAlt);

	    glPushMatrix();

	    glTranslatef(xPos, yPos, 0.0);
	    glRotatef(-1.0* heading_map, 0, 0, 1);

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

	// TODO: plot TCAS of Multiplayer aircraft


	
      } // valid acf coordinates
    
      glPopMatrix();

    }
    
  }

} // end namespace OpenGC

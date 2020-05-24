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
#include "B737/B737NAV/ogcB737NAVDrawFMC.h"

namespace OpenGC
{

  B737NAVDrawFMC::B737NAVDrawFMC()
  {
    printf("B737NAVDrawFMC constructed\n");
  
    m_Font = m_pFontManager->LoadDefaultFont();

    wpt = NULL;
    nwpt = 0;
  }

  B737NAVDrawFMC::~B737NAVDrawFMC()
  {
    // Destruction handled by base class
  }

  void B737NAVDrawFMC::Render()
  {
    GaugeComponent::Render();
     
    int acf_type = m_pDataSource->GetAcfType();
    
    bool mapCenter = m_NAVGauge->GetMapCenter();
    int mapMode = m_NAVGauge->GetMapMode();
    float mapRange = m_NAVGauge->GetMapRange();

    char buffer[10];

    CircleEvaluator aCircle;

    // define geometric stuff
    float fontSize = 4.0 * m_PhysicalSize.x / 150.0;
    float lineWidth = 1.1 * m_PhysicalSize.x / 100.0;

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

    // Get information on what dynamic information we display on NAV MAP
 
    // Where is the aircraft?
    double aircraftLon = m_NAVGauge->GetMapCtrLon();
    double aircraftLat = m_NAVGauge->GetMapCtrLat();
    
    // What's the heading?
    float heading_map =  m_NAVGauge->GetMapHeading();
    float *magnetic_variation = link_dataref_flt("sim/flightmodel/position/magnetic_variation",-1);
     
    float *fmc_ok;
    float *fmc_lon;
    float *fmc_lat;
    int *fmc_turn;
    int *fmc_alt;
    int *fmc_type;
    int *fmc_hold_time;
    float *fmc_rad_ctr_lon;
    float *fmc_rad_ctr_lat;
    float *fmc_rad_lon2;
    float *fmc_rad_lat2;
    int *fmc_rad_turn;
    float *fmc_rad_radius;
    float *fmc_brg;
    float *fmc_crs;
    int *fmc_miss1;
    int *fmc_miss2;
    int *fmc_cur;
    int *fmc_ctr;
    float *fmc_rnp;
    float *fmc_anp;
    unsigned char *fmc_name;
    unsigned char *fmc_pth;
    int *fmc_idx;
    int *fmc_nidx;
    if (acf_type == 3) {
      /* ZIBO 737 FMC */
      fmc_name = link_dataref_byte_arr("laminar/B738/fms/legs",255,-1);
      fmc_pth = link_dataref_byte_arr("laminar/B738/fms/legs_pth",255,-1);
      fmc_lon = link_dataref_flt_arr("laminar/B738/fms/legs_lon",128,-1,-5);
      fmc_lat = link_dataref_flt_arr("laminar/B738/fms/legs_lat",128,-1,-5);
      fmc_alt = link_dataref_int_arr("laminar/B738/fms/legs_alt_rest1",128,-1);
      fmc_ctr = link_dataref_int("laminar/B738/fms/legs_step_ctr"); 
      fmc_cur = link_dataref_int("laminar/B738/fms/vnav_idx"); 
      fmc_turn = link_dataref_int_arr("laminar/B738/fms/legs_turn",128,-1);
      fmc_type = link_dataref_int_arr("laminar/B738/fms/legs_type",128,-1);
      fmc_hold_time = link_dataref_int_arr("laminar/B738/fms/legs_hold_time",128,-1);
      fmc_rad_turn = link_dataref_int_arr("laminar/B738/fms/legs_rad_turn",128,-1);
      fmc_rad_ctr_lon = link_dataref_flt_arr("laminar/B738/fms/legs_radii_ctr_lon",128,-1,-4);
      fmc_rad_ctr_lat = link_dataref_flt_arr("laminar/B738/fms/legs_radii_ctr_lat",128,-1,-4);
      fmc_rad_lon2 = link_dataref_flt_arr("laminar/B738/fms/legs_radii_lon2",128,-1,-4);
      fmc_rad_lat2 = link_dataref_flt_arr("laminar/B738/fms/legs_radii_lat2",128,-1,-4);
      fmc_rad_radius = link_dataref_flt_arr("laminar/B738/fms/legs_radii_radius",128,-1,-1);
      fmc_miss1 = link_dataref_int("laminar/B738/fms/missed_app_wpt_idx");
      fmc_miss2 = link_dataref_int("laminar/B738/fms/missed_app_wpt_idx2");
      //      fmc_brg = link_dataref_flt_arr("laminar/B738/fms/legs_brg_true",128,-1,-5);
      fmc_brg = link_dataref_flt_arr("laminar/B738/fms/legs_brg_mag",128,-1,-5);
      fmc_crs = link_dataref_flt_arr("laminar/B738/fms/legs_crs_mag",128,-1,-5);
      fmc_rnp = link_dataref_flt("laminar/B738/fms/rnp",-2);
      fmc_anp = link_dataref_flt("laminar/B738/fms/anp",-2);
      fmc_nidx = link_dataref_int("laminar/B738/fms/num_of_wpts");

      /* set center lat/lon of map to currently selected waypoint
	 in Plan Mode Map */
      if ((*fmc_ctr != INT_MISS) && (mapMode == 3)) {
	if (*fmc_nidx >= *fmc_ctr) {
	  if ((fmc_lon[*fmc_ctr - 1] != FLT_MISS) && (fmc_lat[*fmc_ctr - 1] != FLT_MISS)) {
	    m_NAVGauge->SetMapCtrLon(fmc_lon[*fmc_ctr - 1]);
	    m_NAVGauge->SetMapCtrLat(fmc_lat[*fmc_ctr - 1]);
	  }
	}
      }
    } else if (acf_type == 1) {
      /* Javier Cortes Flight Management Computer plugin */
      fmc_ok = link_dataref_flt("FJCC/UFMC/PRESENT",-1);
      fmc_lon = link_dataref_flt("FJCC/UFMC/Waypoint/Lon",-3);
      fmc_lat = link_dataref_flt("FJCC/UFMC/Waypoint/Lat",-3);
      fmc_name = link_dataref_byte_arr("FJCC/UFMC/Waypoint/Name",40,-1);
      fmc_idx = link_dataref_int("FJCC/UFMC/Waypoint/Index");
      fmc_nidx = link_dataref_int("FJCC/UFMC/Waypoint/Number_of_Waypoints");
    } else {
      /* all other ACF -- TBD */
    }
      
    // The input coordinates are in lon/lat, so we have to rotate against true heading
    // despite the NAV display is showing mag heading
    if (heading_map != FLT_MISS) {

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      if (mapMode != 3) {
	// plot RNP/ANP
	if ((acf_type == 2) || (acf_type == 3)) {
	  if (*fmc_rnp != FLT_MISS) {
	    glColor3ub( 0, 255, 0 );
	    m_pFontManager->SetSize(m_Font, 0.9*fontSize, 0.9*fontSize);
	    m_pFontManager->Print(0.4*m_PhysicalSize.x,0.05*m_PhysicalSize.y, "RNP", m_Font);
	    snprintf( buffer, sizeof(buffer), "%0.2f", *fmc_rnp );
	    m_pFontManager->Print(0.4*m_PhysicalSize.x,0.01*m_PhysicalSize.y, buffer, m_Font);	  
	  }
	  if (*fmc_anp != FLT_MISS) {
	    glColor3ub( 0, 255, 0 );
	    m_pFontManager->SetSize(m_Font, 0.9*fontSize, 0.9*fontSize);
	    m_pFontManager->Print(0.52*m_PhysicalSize.x,0.05*m_PhysicalSize.y, "ANP", m_Font);
	    snprintf( buffer, sizeof(buffer), "%0.2f", *fmc_anp );
	    m_pFontManager->Print(0.52*m_PhysicalSize.x,0.01*m_PhysicalSize.y, buffer, m_Font);	  
	  }
	}
      }
      
      // Shift center and rotate about heading
      glTranslatef(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y, 0.0);
      glRotatef(heading_map, 0, 0, 1);
 
      /* valid coordinates ? */
      if ((aircraftLon >= -180.0) && (aircraftLon <= 180.0) && (aircraftLat >= -90.0) && (aircraftLat <= 90.0)) {
        
	// Set up circle for small symbols
	CircleEvaluator aCircle;
	aCircle.SetArcStartEnd(0,360);
	aCircle.SetDegreesPerPoint(10);

	double northing;
	double easting;
	double northing2;
	double easting2;
	double lon;
	double lat;
	float xPos;
	float yPos;
	float xPos2;
	float yPos2;
	float xPosC;
	float yPosC;

	// define overall symbol size (for DME, FIX, VOR, APT etc.)
	// this scale gives the radius of the symbol in physical units
	float ss = m_PhysicalSize.y*0.02;
	
	//      printf("%i %i \n",*fmc_nidx,nwpt);
      
	/* Plot FMC waypoints */
	int wpt_current = 0;
	int wpt_miss1 = INT_MISS;
	int wpt_miss2 = INT_MISS;
	if ((acf_type == 1) || (acf_type == 2) || (acf_type == 3)) {
	  if (acf_type == 1) {
	    /* UFMC of Javier Cortez */
	    if ((*fmc_ok > 0.5) && (*fmc_nidx > 0)) {
	    
	      if (*fmc_nidx != nwpt) {
	      
		nwpt = *fmc_nidx;	  
	      
		if (wpt != NULL) {
		  delete [] wpt;
		}
	      
		wpt = new wptstruct[nwpt];
		for (int i=0;i<nwpt;i++) {
		  memset(wpt[i].name,0,sizeof(wpt[i].name));
		  wpt[i].lon = FLT_MISS;
		  wpt[i].lat = FLT_MISS;
		}
		*fmc_idx = 0;
		*fmc_lon = -1000.0;
		*fmc_lat = -1000.0;
	      }
	    
	      if ((*fmc_idx >= 0) && (*fmc_idx < nwpt)) {
	      
		if ((*fmc_lon != -1000.0) && (*fmc_lat != -1000.0)) {
		
		  printf("Fetching Waypoint %i of %i from UFMC: %f %f %s \n",(*fmc_idx)+1,*fmc_nidx,*fmc_lat,*fmc_lon,fmc_name);
		
		  if ((*fmc_lon != 0.0) && (*fmc_lat != 0.0)) {
		    memcpy(&wpt[*fmc_idx].name,fmc_name,sizeof(wpt[*fmc_idx].name));
		    wpt[*fmc_idx].lon = *fmc_lon;
		    wpt[*fmc_idx].lat = *fmc_lat;
		    *fmc_lon = -1000.0;
		    *fmc_lat = -1000.0;
		  }
		  (*fmc_idx)++;
		}
	      
	      }
	    }

	  } else {
	    /* ZIBO MOD FMC */
	    nwpt=0;
	    if (*fmc_nidx != INT_MISS) {
	      wpt_current = *fmc_cur-1; 
	      wpt_miss1 = *fmc_miss1;
	      wpt_miss2 = *fmc_miss2;
	      nwpt = *fmc_nidx;

	      
	      if (nwpt > 0) {
		if (wpt != NULL) {
		  delete [] wpt;
		}
		wpt = new wptstruct[nwpt];
		char str[255];
		memcpy(str,fmc_name,sizeof(str));
		char *pch;
		pch = strtok(str," ");
		if (pch == NULL) {
		  /* no names for any wpts: reset */
		  nwpt = 0;
		}
		for (int i=0;i<nwpt;i++) {
		  memset(wpt[i].name,0,sizeof(wpt[i].name));
		  memset(wpt[i].pth,0,sizeof(wpt[i].pth));
		  if (pch != NULL) {
		    //		printf("%s \n",pch);
		    //		  if ((int) pch[0] != 40) {
		    /* do only name waypoints which are not in () brackets */
		    memcpy(&wpt[i].name,pch,sizeof(wpt[i].name));
		    //		  }
		    
		    pch = strtok(NULL," ");
		  }
		  memcpy(&wpt[i].pth,fmc_pth+2*i,sizeof(wpt[i].pth)-1);
		  wpt[i].lon = fmc_lon[i];
		  wpt[i].lat = fmc_lat[i];
		  wpt[i].alt = fmc_alt[i];
		  wpt[i].rad_ctr_lon = fmc_rad_ctr_lon[i];
		  wpt[i].rad_ctr_lat = fmc_rad_ctr_lat[i];
		  wpt[i].rad_lon2 = fmc_rad_lon2[i];
		  wpt[i].rad_lat2 = fmc_rad_lat2[i];
		  wpt[i].brg = fmc_brg[i]; // radians mag
		  wpt[i].crs = fmc_crs[i]; // degrees mag
		  wpt[i].rad_radius = fmc_rad_radius[i];
		  wpt[i].turn = fmc_turn[i]; // 0,2: left. 3: right
		  wpt[i].hold_time = fmc_hold_time[i]; // holding time in seconds
		  wpt[i].rad_turn = fmc_rad_turn[i]; // HOLD: 0: left, 1: RIGHT

		  /*
		    printf("%s %s %i %f %f %f %f %f\n",wpt[i].name,wpt[i].pth,fmc_turn[i],
		    fmc_rad_lon2[i],fmc_rad_ctr_lon[i],fmc_radius[i],
		    fmc_brg[i]*180./3.14,fmc_brg[max(i-1,0)]*180./3.14); 
		  */
		}
	      } 
	    }
	  }

	  if (nwpt == 0) {
	    if (wpt != NULL) {
	      delete [] wpt;
	      wpt = NULL;
	      //	    printf("FMC waypoints cleared\n");
	    }
	  }
	
	  /*
	    for (int i=0;i<(nwpt);i++) {
	    printf("%i %s %f %f %i \n",i,wpt[i].name,wpt[i].lon,wpt[i].lat,wpt_current);
	    }
	  */
       
	  

	  if (nwpt > 0) {
	    for (int i=max(wpt_current-1,0);i<nwpt;i++) {

	      /*
	      printf("%i %s %f / %f %f / %f %f / %f %f \n",i,wpt[i].name,wpt[i].lon,
		     wpt[i].rad_lon2,wpt[max(i-1,0)].rad_lon2,
		     wpt[i].rad_radius,wpt[max(i-1,0)].rad_radius,
		     wpt[i].crs,wpt[max(i-1,0)].crs);
	      */

	      // convert to azimuthal equidistant coordinates with acf in center
	      if ((wpt[max(i-1,0)].lon != FLT_MISS) && (wpt[max(i-1,0)].lat != FLT_MISS) &&
		  (wpt[i].lon != FLT_MISS) && (wpt[i].lat != FLT_MISS)) {

		
		// convert to azimuthal equidistant coordinates with acf in center
		lon = (double) wpt[max(i-1,0)].lon;
		lat = (double) wpt[max(i-1,0)].lat;
		lonlat2gnomonic(&lon, &lat, &easting, &northing, &aircraftLon, &aircraftLat);
		
		lon = (double) wpt[i].lon;
		lat = (double) wpt[i].lat;
		lonlat2gnomonic(&lon, &lat, &easting2, &northing2, &aircraftLon, &aircraftLat);
	      
		// Compute physical position relative to acf center on screen
		yPos = -northing / 1852.0 / mapRange * map_size; 
		xPos = easting / 1852.0  / mapRange * map_size;
		yPos2 = -northing2 / 1852.0 / mapRange * map_size; 
		xPos2 = easting2 / 1852.0  / mapRange * map_size;

		/* Label Position */
		float xPosL = xPos2;
		float yPosL = yPos2;

		/* Special cases before or after curved routes */
		if ((wpt[max(i-1,0)].rad_lon2 != 0.0) && (wpt[max(i-1,0)].rad_lat2 != 0.0) &&
		    (wpt[max(i-1,0)].rad_radius != 0.0) && // (wpt[max(i-1,0)].crs != 0.0) &&
		    (wpt[max(i-1,0)].rad_lon2 != FLT_MISS) && (wpt[max(i-1,0)].rad_lat2 != FLT_MISS) &&
		    (wpt[max(i-1,0)].rad_radius != FLT_MISS)) { // && (wpt[max(i-1,0)].crs != FLT_MISS)) {
		  // leg does not start at last waypoint
		  lon = (double) wpt[max(i-1,0)].rad_lon2;
		  lat = (double) wpt[max(i-1,0)].rad_lat2;
		  lonlat2gnomonic(&lon, &lat, &easting, &northing, &aircraftLon, &aircraftLat);
		  yPos = -northing / 1852.0 / mapRange * map_size; 
		  xPos = easting / 1852.0  / mapRange * map_size;
		} 
		if ((wpt[i].rad_lon2 != 0.0) && (wpt[i].rad_lat2 != 0.0) &&
		    (wpt[i].rad_radius != 0.0) && // (wpt[i].crs != 0.0) &&
		    (wpt[i].rad_lon2 != FLT_MISS) && (wpt[i].rad_lat2 != FLT_MISS) &&
		    (wpt[i].rad_radius != FLT_MISS)) { // && (wpt[i].crs != FLT_MISS)) {
		  // leg does not end at next waypoint
		  lon = (double) wpt[i].rad_lon2;
		  lat = (double) wpt[i].rad_lat2;
		  lonlat2gnomonic(&lon, &lat, &easting, &northing, &aircraftLon, &aircraftLat);
		  yPos2 = -northing / 1852.0 / mapRange * map_size; 
		  xPos2 = easting / 1852.0  / mapRange * map_size;		    
		}
		
		// Only draw the waypoint if it's visible within the rendering area
		//if (( sqrt(xPos*xPos + yPos*yPos) < map_size) ||
		//    ( sqrt(xPos2*xPos2 + yPos2*yPos2) < map_size)) {
	    
		glPushMatrix();

		/* only draw leg if it is not a Discontinuity in flight plan */
		if (strcmp(wpt[max(i-1,0)].name,"DISCONTINUITY") != 0) {
		  
		glLineWidth(lineWidth);
		if ((i >= wpt_miss1) && (i <= wpt_miss2)) {
		  // missed approach route
		  glColor3ub(0, 189, 231);
		  glEnable(GL_LINE_STIPPLE);
		  glLineStipple( 4, 0x0F0F );
		} else {
		  // regular route
		  glColor3ub(255, 0, 200);
		}		// check if wpt is a holding
		if ((strcmp(wpt[i].pth,"HM") == 0) ||
		    (strcmp(wpt[i].pth,"HF") == 0) ||
		    (strcmp(wpt[i].pth,"HA") == 0)) {
		  // holds are legs_pth
		  // "HA" (hold to altitude)
		  // "HF" (hold to a fix)
		  // "HM" (hold with manual termination)
		  // hold_radius = 1.5	-- 1.5 NM -> about 3 deg/sec at 250kts
		  // hold length is 3.0 NM for a 60 second hold
		  int holdtype = wpt[i].rad_turn * 2 - 1; // 1: Right -1: Left
		  float holdrad = 1.5 / mapRange * map_size; // nm --> pixels
		  float holdlen = max(wpt[i].hold_time,60) / 60.0 * 3.0 / mapRange * map_size; // nm --> Pixels
		  float gamma = (wpt[i].crs - *magnetic_variation)*3.14/180.; // inbound direction (radians)
		  float xPos1 = sin(gamma-3.14)*holdlen + xPos2; // start of inbound course
		  float yPos1 = cos(gamma-3.14)*holdlen + yPos2; // Pos2 is end of inbound course
		  float xPos3 = sin(gamma+0.5*3.14*holdtype)*holdrad*2.0 + xPos2; // start of outbound course 
		  float yPos3 = cos(gamma+0.5*3.14*holdtype)*holdrad*2.0 + yPos2;
		  float xPos4 = sin(gamma+0.5*3.14*holdtype)*holdrad*2.0 + xPos1; // end of outbound course 
		  float yPos4 = cos(gamma+0.5*3.14*holdtype)*holdrad*2.0 + yPos1;

		  // printf("%i %i %i %f \n",i,holdtype, wpt[i].hold_time,wpt[i].crs);
		  
		  glBegin(GL_LINES);
		  glVertex2f(xPos1,yPos1);
		  glVertex2f(xPos2,yPos2);
		  glEnd();

		  xPosC = 0.5*(xPos3-xPos2) + xPos2;
		  yPosC = 0.5*(yPos3-yPos2) + yPos2;
		  aCircle.SetDegreesPerPoint(5);
		  aCircle.SetArcStartEnd(180./3.14*(gamma-0.5*3.14),180./3.14*(gamma+0.5*3.14));
		  aCircle.SetRadius(holdrad);
		  aCircle.SetOrigin(xPosC,yPosC);
		  glBegin(GL_LINE_STRIP);
		  aCircle.Evaluate();
		  glEnd();
		  
		  glBegin(GL_LINES);
		  glVertex2f(xPos3,yPos3);
		  glVertex2f(xPos4,yPos4);
		  glEnd();
		  
		  xPosC = 0.5*(xPos4-xPos1) + xPos1;
		  yPosC = 0.5*(yPos4-yPos1) + yPos1;
		  aCircle.SetDegreesPerPoint(5);
		  aCircle.SetArcStartEnd(180./3.14*(gamma+0.5*3.14),180./3.14*(gamma-0.5*3.14));
		  aCircle.SetRadius(holdrad);
		  aCircle.SetOrigin(xPosC,yPosC);
		  glBegin(GL_LINE_STRIP);
		  aCircle.Evaluate();
		  glEnd();


		} else if ((wpt[max(i-1,0)].rad_ctr_lon != 0.0) &&
			   (wpt[max(i-1,0)].rad_ctr_lat != 0.0) &&
			   (wpt[max(i-1,0)].rad_radius != 0.0) &&
			   ((wpt[max(i-1,0)].brg != 0.0) || (wpt[i].brg != 0.0))) {
		  // draw curved track from waypoint i-1 to waypoint i

		  lon = (double) wpt[max(i-1,0)].rad_ctr_lon;
		  lat = (double) wpt[max(i-1,0)].rad_ctr_lat;
		  lonlat2gnomonic(&lon, &lat, &easting, &northing, &aircraftLon, &aircraftLat);
		  yPosC = -northing / 1852.0 / mapRange * map_size; 
		  xPosC = easting / 1852.0  / mapRange * map_size;		  

		  /*
		  printf("%i %s %i %f %f / %f %f \n",i,wpt[i].name,wpt[i].turn,
			 wpt[max(i-1,0)].brg*180./3.14,wpt[i].brg*180./3.14,
			 wpt[max(i-1,0)].crs,wpt[i].crs);
		  */

		  /*
		  glPushMatrix();
		  glColor3ub(255, 0, 0);
		  glPointSize(8.0);
		  glBegin(GL_POINTS);
		  glVertex2f(xPosC, yPosC);
		  glEnd();		  
		  glPopMatrix();
		  */

		  /*
		  glPushMatrix();		    
		  glColor3ub(0, 255, 0);
		  glPointSize(10.0);
		  glBegin(GL_POINTS);
		  glVertex2f(xPos, yPos);
		  glEnd();		    
		  glPopMatrix();
		  */

		  /*
		  glPushMatrix();		    
		  glColor3ub(0, 0, 255);
		  glPointSize(10.0);
		  glBegin(GL_POINTS);
		  glVertex2f(xPos2, yPos2);
		  glEnd();		    
		  glPopMatrix();
		  */

		  /* start and end angle of circle with center ctr calculated
		     from legs magnetic bearing and waypoint turn direction */
		  float ang = wpt[max(i-1,0)].brg*180./3.14 - *magnetic_variation;
		  float ang2 = wpt[i].brg*180./3.14 - *magnetic_variation;
		  float relbrg = fmod(180./3.14*(wpt[i].brg-wpt[max(i-1,0)].brg) + 360.0, 360.0);
		  float radius = pow(pow(xPos-xPosC,2) + pow(yPos-yPosC,2),0.5);
		  if (relbrg > 180.0) relbrg -= 360.0;
		  if ((wpt[max(i-1,0)].turn == 0) || (wpt[max(i-1,0)].turn == 2)) {
		    /* left turn */
		    ang += 90.0;
		    ang2 += 90.0;
		    if (relbrg > 0.0) relbrg -= 360.0;
		  } else {
		    /* right turn */
		    ang += 270.0;
		    ang2 += 270.0;
		    if (relbrg < 0.0) relbrg += 360.0;
		  }
		  ang = fmodf(ang + 360.0,360.0);
		  ang2 = fmodf(ang2 + 360.0,360.0);
		  //printf("%i %f %f \n",i,ang,ang2);
				  
		  // draw curved leg (partial circle)
		  aCircle.SetDegreesPerPoint(2);
		  if ((wpt[max(i-1,0)].turn == 0) || (wpt[max(i-1,0)].turn == 2)) {
		    aCircle.SetArcStartEnd(ang2,ang);
		  } else {
		    aCircle.SetArcStartEnd(ang,ang2);
		  }
		  aCircle.SetRadius(radius);
		  aCircle.SetOrigin(xPosC,yPosC);
		  glBegin(GL_LINE_STRIP);
		  aCircle.Evaluate();
		  glEnd();
		
		  /* Circle always starts from xPos/yPos, but may need to be extended
		     by a line to xPos2/yPos2 */		
		  float D = pow(pow(xPos2-xPosC,2) + pow(yPos2-yPosC,2),0.5);
		  if (D > (radius+0.1)) {
		    float xPosT = sin(ang2*3.14/180.)*radius + xPosC;
		    float yPosT = cos(ang2*3.14/180.)*radius + yPosC;

		    /*
		    glPushMatrix();		    
		    glColor3ub(255, 0, 255);
		    glPointSize(10.0);
		    glBegin(GL_POINTS);
		    glVertex2f(xPosT, yPosT);
		    glEnd();		    
		    glPopMatrix();
		    */
		    
		    // draw tangent
		    glBegin(GL_LINES);
		    glVertex2f(xPosT,yPosT);
		    glVertex2f(xPos2,yPos2);
		    glEnd();
		  
		  }
		
		
		} else {
		  // straight line

		  glBegin(GL_LINES);
		  glVertex2f(xPos,yPos);
		  glVertex2f(xPos2,yPos2);
		  glEnd();
		}

		if ((i >= wpt_miss1) && (i <= wpt_miss2)) {
		  glDisable(GL_LINE_STIPPLE);
		}

		} // not a Discontinuity in flight plan
		
		// draw waypoint symbol and name
		glTranslatef(xPosL, yPosL, 0.0);
		glRotatef(-1.0* heading_map, 0, 0, 1);

		if (i == wpt_current) {
		  glColor3ub(255, 0, 200);
		} else {
		  glColor3ub(255, 255, 255);
		}
		glLineWidth(0.8*lineWidth);
		glBegin(GL_LINE_LOOP);
		glVertex2f(0.15*ss, 0.15*ss);
		glVertex2f(1.0*ss,  0.0);
		glVertex2f(0.15*ss, -0.15*ss);
		glVertex2f(0.0, -1.0*ss);
		glVertex2f(-0.15*ss, -0.15*ss);
		glVertex2f(-1.0*ss, 0.0);
		glVertex2f(-0.15*ss, 0.15*ss);
		glVertex2f( 0.0, 1.0*ss);
		glEnd();

		if (strcmp(wpt[i].name,"DISCONTINUITY") != 0) {
		  m_pFontManager->SetSize(m_Font, 0.65*fontSize, 0.65*fontSize);
		  m_pFontManager->Print(4,-6, wpt[i].name, m_Font);
		  if (wpt[i].alt > 0) {
		    snprintf( buffer, sizeof(buffer), "%i", wpt[i].alt );
		    m_pFontManager->Print(4,-11, buffer, m_Font);
		  }
		}

		glPopMatrix();

		//}
		
	      }
	    }
	  } /* has waypoints */
	} /* we are on either UFMC or ZIBO FMC for 737 */
	
      } // valid acf coordinates
    
      glPopMatrix();

    }
    
  }

} // end namespace OpenGC

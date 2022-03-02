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

    char buffer[12];

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
 
    // These are ACF coordinates, except for Plan mode
    double MapCenterLon = m_NAVGauge->GetMapCtrLon();
    double MapCenterLat = m_NAVGauge->GetMapCtrLat();

    // ACF coordinates (Always ACF coordinates) 
    double *aircraftLat = link_dataref_dbl("sim/flightmodel/position/latitude",-4);
    double *aircraftLon = link_dataref_dbl("sim/flightmodel/position/longitude",-4);
    float *aircraftHdg = link_dataref_flt("sim/flightmodel/position/psi",-1);
    
    // What's the heading?
    float heading_map =  m_NAVGauge->GetMapHeading();
    float *magnetic_variation = link_dataref_flt("sim/flightmodel/position/magnetic_variation",-1);

    // What's the ground and air speed
    float *ground_speed = link_dataref_flt("sim/flightmodel/position/groundspeed",0);
    float *true_air_speed = link_dataref_flt("sim/flightmodel/position/true_airspeed",0);
     
    float *fmc_ok;
    float *fmc_lon;
    float *fmc_lat;
    float *fmc_turn;
    float *fmc_alt;
    float *fmc_type;
    float *fmc_hold_time;
    float *fmc_hold_dist;
    float *fmc_dist;
    float *fmc_eta;
    float *fmc_radii_ctr_lon;
    float *fmc_radii_ctr_lat;
    float *fmc_radii_lon;
    float *fmc_radii_lat;
    float *fmc_radii_radius;
    float *fmc_rad_lon;
    float *fmc_rad_lat;
    float *fmc_rad_turn;
    float *fmc_radius;
    float *fmc_brg;
    float *fmc_crs;
    int *fmc_miss1;
    int *fmc_miss2;
    int *fmc_cur;
    int *fmc_ctr;
    float *fmc_rnp;
    float *fmc_anp;
    float *fmc_vrnp;
    float *fmc_vanp;
    float *fmc_vnav_td_dist;
    float *fmc_vnav_err;
    int *fmc_has_vrnp;
    int *fmc_rnav_enable;
    float *fmc_rnav_alt;
    unsigned char *fmc_name;
    unsigned char *fmc_pth;
    int *fmc_idx;
    int *fmc_nidx;

    float *seg1_start_lon;
    float *seg1_start_lat;
    float *seg1_start_ang;
    float *seg1_end_lon;
    float *seg1_end_lat;
    float *seg1_end_ang;
    float *seg1_ctr_lon;
    float *seg1_ctr_lat;
    float *seg1_radius;
    float *seg1_turn;
    float *seg2_end_lon;
    float *seg2_end_lat;
    float *seg3_start_ang;
    float *seg3_end_lon;
    float *seg3_end_lat;
    float *seg3_end_ang;
    float *seg3_ctr_lon;
    float *seg3_ctr_lat;
    float *seg3_radius;
    float *seg3_turn;
    
    if (acf_type == 3) {
      /* ZIBO 737 FMC */
      fmc_name = link_dataref_byte_arr("laminar/B738/fms/legs",256,-1);
      fmc_pth = link_dataref_byte_arr("laminar/B738/fms/legs_pth",256,-1);
      fmc_lon = link_dataref_flt_arr("laminar/B738/fms/legs_lon",256,-1,-5);
      fmc_lat = link_dataref_flt_arr("laminar/B738/fms/legs_lat",256,-1,-5);
      fmc_alt = link_dataref_flt_arr("laminar/B738/fms/legs_alt_rest1",256,-1,0);
      fmc_ctr = link_dataref_int("laminar/B738/fms/legs_step_ctr"); 
      fmc_cur = link_dataref_int("laminar/B738/fms/vnav_idx"); 
      fmc_turn = link_dataref_flt_arr("laminar/B738/fms/legs_turn",256,-1,0);
      fmc_type = link_dataref_flt_arr("laminar/B738/fms/legs_type",256,-1,0);
      fmc_hold_time = link_dataref_flt_arr("laminar/B738/fms/legs_hold_time",256,-1,-1);
      fmc_hold_dist = link_dataref_flt_arr("laminar/B738/fms/legs_hold_dist",256,-1,-1);
      fmc_dist = link_dataref_flt_arr("laminar/B738/fms/legs_dist",256,-1,-2);
      fmc_eta = link_dataref_flt_arr("laminar/B738/fms/legs_eta",256,-1,-2);
      fmc_radii_ctr_lon = link_dataref_flt_arr("laminar/B738/fms/legs_radii_ctr_lon",256,-1,-4);
      fmc_radii_ctr_lat = link_dataref_flt_arr("laminar/B738/fms/legs_radii_ctr_lat",256,-1,-4);
      fmc_radii_lon = link_dataref_flt_arr("laminar/B738/fms/legs_radii_lon2",256,-1,-4);
      fmc_radii_lat = link_dataref_flt_arr("laminar/B738/fms/legs_radii_lat2",256,-1,-4);
      fmc_radii_radius = link_dataref_flt_arr("laminar/B738/fms/legs_radii_radius",256,-1,-1);
      fmc_rad_lon = link_dataref_flt_arr("laminar/B738/fms/legs_rad_lon",256,-1,-4);
      fmc_rad_lat = link_dataref_flt_arr("laminar/B738/fms/legs_rad_lat",256,-1,-4);
      fmc_rad_turn = link_dataref_flt_arr("laminar/B738/fms/legs_rad_turn",256,-1,0);
      fmc_radius = link_dataref_flt_arr("laminar/B738/fms/legs_radius",256,-1,-1);
      fmc_miss1 = link_dataref_int("laminar/B738/fms/missed_app_wpt_idx");
      fmc_miss2 = link_dataref_int("laminar/B738/fms/missed_app_wpt_idx2");
      fmc_brg = link_dataref_flt_arr("laminar/B738/fms/legs_brg_mag",256,-1,-5);
      fmc_crs = link_dataref_flt_arr("laminar/B738/fms/legs_crs_mag",256,-1,-5);
      fmc_rnp = link_dataref_flt("laminar/B738/fms/rnp",-2);
      fmc_anp = link_dataref_flt("laminar/B738/fms/anp",-2);
      fmc_vrnp = link_dataref_flt("laminar/B738/fms/vrnp",-2);
      fmc_vanp = link_dataref_flt("laminar/B738/fms/vanp",-2);
      fmc_vnav_td_dist = link_dataref_flt("laminar/B738/fms/vnav_td_dist",-1);
      fmc_vnav_err = link_dataref_flt("laminar/B738/fms/vnav_err_pfd",0);
      fmc_has_vrnp = link_dataref_int("laminar/B738/fms/vrnp_enable");
      fmc_rnav_enable = link_dataref_int("laminar/B738/fms/rnav_enable");
      fmc_rnav_alt = link_dataref_flt("laminar/B738/fms/rnav_alt",0);
      fmc_nidx = link_dataref_int("laminar/B738/fms/num_of_wpts");

      seg1_start_lon = link_dataref_flt_arr("laminar/B738/fms/legs_seg1_start_lon",256,-1,-4);
      seg1_start_lat = link_dataref_flt_arr("laminar/B738/fms/legs_seg1_start_lat",256,-1,-4);
      seg1_start_ang = link_dataref_flt_arr("laminar/B738/fms/legs_seg1_start_angle",256,-1,-4);
      seg1_end_lon = link_dataref_flt_arr("laminar/B738/fms/legs_seg1_end_lon",256,-1,-4);
      seg1_end_lat = link_dataref_flt_arr("laminar/B738/fms/legs_seg1_end_lat",256,-1,-4);
      seg1_end_ang = link_dataref_flt_arr("laminar/B738/fms/legs_seg1_end_angle",256,-1,-4);
      seg1_ctr_lon = link_dataref_flt_arr("laminar/B738/fms/legs_seg1_ctr_lon",256,-1,-4);
      seg1_ctr_lat = link_dataref_flt_arr("laminar/B738/fms/legs_seg1_ctr_lat",256,-1,-4);
      seg1_radius = link_dataref_flt_arr("laminar/B738/fms/legs_seg1_radius",256,-1,-4);
      seg1_turn = link_dataref_flt_arr("laminar/B738/fms/legs_seg1_turn",256,-1,0);
      seg2_end_lon = link_dataref_flt_arr("laminar/B738/fms/legs_seg2_end_lon",256,-1,-4);
      seg2_end_lat = link_dataref_flt_arr("laminar/B738/fms/legs_seg2_end_lat",256,-1,-4);
      seg3_start_ang = link_dataref_flt_arr("laminar/B738/fms/legs_seg3_start_angle",256,-1,-4);
      seg3_end_lon = link_dataref_flt_arr("laminar/B738/fms/legs_seg3_end_lon",256,-1,-4);
      seg3_end_lat = link_dataref_flt_arr("laminar/B738/fms/legs_seg3_end_lat",256,-1,-4);
      seg3_end_ang = link_dataref_flt_arr("laminar/B738/fms/legs_seg3_end_angle",256,-1,-4);
      seg3_ctr_lon = link_dataref_flt_arr("laminar/B738/fms/legs_seg3_ctr_lon",256,-1,-4);
      seg3_ctr_lat = link_dataref_flt_arr("laminar/B738/fms/legs_seg3_ctr_lat",256,-1,-4);
      seg3_radius = link_dataref_flt_arr("laminar/B738/fms/legs_seg3_radius",256,-1,-4);
      seg3_turn = link_dataref_flt_arr("laminar/B738/fms/legs_seg3_turn",256,-1,0);

    /* set center lat/lon of map to currently selected waypoint
	 in Plan Mode Map (Center Waypoint) */
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
      glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

      glPushMatrix();
      if (mapMode != 3) {
	// plot RNP/ANP
	glColor3ub(COLOR_BLACK);
	glBegin(GL_POLYGON);
	glVertex2f(m_PhysicalSize.x*0.3,m_PhysicalSize.y*0.0);
	glVertex2f(m_PhysicalSize.x*0.3,m_PhysicalSize.y*0.1);
	glVertex2f(m_PhysicalSize.x*0.7,m_PhysicalSize.y*0.1);
	glVertex2f(m_PhysicalSize.x*0.7,m_PhysicalSize.y*0.0);
	glEnd();
 	if ((acf_type == 2) || (acf_type == 3)) {
	  if (*fmc_rnp != FLT_MISS) {
	    glColor3ub(COLOR_GREEN);
	    m_pFontManager->SetSize(m_Font, 0.9*fontSize, 0.9*fontSize);
	    m_pFontManager->Print(0.4*m_PhysicalSize.x,0.05*m_PhysicalSize.y, "RNP", m_Font);
	    snprintf( buffer, sizeof(buffer), "%0.2f", *fmc_rnp );
	    m_pFontManager->Print(0.4*m_PhysicalSize.x,0.01*m_PhysicalSize.y, buffer, m_Font);	  
	  }
	  if (*fmc_anp != FLT_MISS) {
	    glColor3ub(COLOR_GREEN);
	    m_pFontManager->SetSize(m_Font, 0.9*fontSize, 0.9*fontSize);
	    m_pFontManager->Print(0.52*m_PhysicalSize.x,0.05*m_PhysicalSize.y, "ANP", m_Font);
	    snprintf( buffer, sizeof(buffer), "%0.2f", *fmc_anp );
	    m_pFontManager->Print(0.52*m_PhysicalSize.x,0.01*m_PhysicalSize.y, buffer, m_Font);	  
	  }
	  if ((*fmc_has_vrnp == 1) && (*fmc_vnav_td_dist == 0.0) && (*fmc_rnav_alt != 0.0)) {
	    glColor3ub(COLOR_WHITE);
	    glLineWidth(lineWidth);
	    glBegin(GL_LINES);
	    glVertex2f(0.95*m_PhysicalSize.x,0.40*m_PhysicalSize.y);
	    glVertex2f(0.95*m_PhysicalSize.x,0.20*m_PhysicalSize.y);
	    glEnd();
	    glBegin(GL_LINES);
	    glVertex2f(0.92*m_PhysicalSize.x,0.40*m_PhysicalSize.y);
	    glVertex2f(0.95*m_PhysicalSize.x,0.40*m_PhysicalSize.y);
	    glEnd();
	    glBegin(GL_LINES);
	    glVertex2f(0.92*m_PhysicalSize.x,0.30*m_PhysicalSize.y);
	    glVertex2f(0.95*m_PhysicalSize.x,0.30*m_PhysicalSize.y);
	    glEnd();
	    glBegin(GL_LINES);
	    glVertex2f(0.92*m_PhysicalSize.x,0.20*m_PhysicalSize.y);
	    glVertex2f(0.95*m_PhysicalSize.x,0.20*m_PhysicalSize.y);
	    glEnd();
	    
	    if (*fmc_vrnp != FLT_MISS) {
	      glColor3ub(COLOR_GREEN);
	      m_pFontManager->SetSize(m_Font, 0.9*fontSize, 0.9*fontSize);
	      m_pFontManager->Print(0.87*m_PhysicalSize.x,0.36*m_PhysicalSize.y, "RNP", m_Font);
	      snprintf( buffer, sizeof(buffer), "%03.0f", *fmc_vrnp );
	      m_pFontManager->Print(0.87*m_PhysicalSize.x,0.32*m_PhysicalSize.y, buffer, m_Font);	  
	    }
	    if (*fmc_vanp != FLT_MISS) {
	      glColor3ub(COLOR_GREEN);
	      m_pFontManager->SetSize(m_Font, 0.9*fontSize, 0.9*fontSize);
	      m_pFontManager->Print(0.87*m_PhysicalSize.x,0.26*m_PhysicalSize.y, "ANP", m_Font);
	      snprintf( buffer, sizeof(buffer), "%03.0f", *fmc_vanp );
	      m_pFontManager->Print(0.87*m_PhysicalSize.x,0.22*m_PhysicalSize.y, buffer, m_Font);	  
	    }
	    if (*fmc_vnav_err != FLT_MISS) {
	      glColor3ub(COLOR_WHITE);
	      m_pFontManager->SetSize(m_Font, 0.9*fontSize, 0.9*fontSize);
	      snprintf( buffer, sizeof(buffer), "%3.0f", *fmc_vnav_err );
	      m_pFontManager->Print(0.88*m_PhysicalSize.x,0.42*m_PhysicalSize.y, buffer, m_Font);
	    }
	      
	    if ((*fmc_vnav_err != FLT_MISS) && (*fmc_vrnp != FLT_MISS)) {
	      glPushMatrix();
	      glTranslatef(0.96*m_PhysicalSize.x, 0.30*m_PhysicalSize.y +
			   0.1*m_PhysicalSize.y * *fmc_vnav_err / *fmc_vrnp, 0.0);
	      glColor3ub(COLOR_VIOLET);
	      float ss = 0.01 * m_PhysicalSize.x;
	      glBegin(GL_LINE_LOOP);
	      glVertex2f(0.0,-ss);
	      glVertex2f(-ss,0.0);
	      glVertex2f(0.0,ss);
	      glVertex2f(ss,0.0);
	      glEnd();
	      glPopMatrix();

	    }
	  }
	}
      }
      glPopMatrix();
      
      /* valid coordinates ? */
      if ((MapCenterLon >= -180.0) && (MapCenterLon <= 180.0) &&
	  (MapCenterLat >= -90.0) && (MapCenterLat <= 90.0)) {
        
	// Set up circle for small symbols
	CircleEvaluator aCircle;
	aCircle.SetArcStartEnd(0,360);
	aCircle.SetDegreesPerPoint(10);

	double northing;
	double easting;
	double lon;
	double lat;
	double lon1;
	double lat1;
	double lon2;
	double lat2;
	double lonT;
	double latT;
	double distance;
	double heading;
	float xPos;
	float yPos;
	float xPos1;
	float yPos1;
	float xPos2;
	float yPos2;
	float xPos3;
	float yPos3;
	float xPos4;
	float yPos4;
	float xPosC;
	float yPosC;
	float xPosT;
	float yPosT;
	float xPosL;
	float yPosL;

	// define overall symbol size (for DME, FIX, VOR, APT etc.)
	// this scale gives the radius of the symbol in physical units
	float ss = m_PhysicalSize.y*0.02;
	
	//      printf("%i %i \n",*fmc_nidx,nwpt);
      
	/* Plot FMC waypoints */
	int wpt_current = 0;
	int wpt_miss1 = INT_MISS;
	int wpt_miss2 = INT_MISS;

	/* Currently only x737 and ZIBO Mod are supported */
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

		  /*
		    printf("%s %s %i %f %f %f %f %f\n",wpt[i].name,wpt[i].pth,fmc_turn[i],
		    fmc_radii_lon[i],fmc_radii_ctr_lon[i],fmc_radius[i],
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

	    // Shift center and rotate about heading
	    glPushMatrix();
	    glTranslatef(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y, 0.0);
	    glRotatef(heading_map, 0, 0, 1);

	    /* Draw Aircraft Symbol in Plan mode */
	    if (mapMode == 3) {
	      lonlat2gnomonic(aircraftLon, aircraftLat, &easting, &northing, &MapCenterLon, &MapCenterLat);
	      yPos = -northing / 1852.0 / mapRange * map_size; 
	      xPos = easting / 1852.0  / mapRange * map_size;
	      glPushMatrix();
	      glTranslatef(xPos, yPos, 0.0);
	      glRotatef(-*aircraftHdg+270.0, 0, 0, 1);

	      float sc = 3.1;
	      glLineWidth(lineWidth);
	      glColor3ub(COLOR_WHITE);
	      glBegin(GL_LINE_LOOP);
	      glVertex2f(-0.5*sc, 0.0*sc);
	      glVertex2f( 0.0*sc,-0.5*sc);
	      glVertex2f( 2.0*sc,-0.5*sc);
	      glVertex2f( 3.5*sc,-3.5*sc);
	      glVertex2f( 4.5*sc,-3.5*sc);
	      glVertex2f( 4.0*sc,-0.5*sc);
	      glVertex2f( 5.0*sc,-0.5*sc);
	      glVertex2f( 5.5*sc,-1.5*sc);
	      glVertex2f( 6.2*sc,-1.5*sc);
	      glVertex2f( 6.0*sc, 0.0*sc);
	      glVertex2f( 6.2*sc, 1.5*sc);
	      glVertex2f( 5.5*sc, 1.5*sc);
	      glVertex2f( 5.0*sc, 0.5*sc);
	      glVertex2f( 4.0*sc, 0.5*sc);
	      glVertex2f( 4.5*sc, 3.5*sc);
	      glVertex2f( 3.5*sc, 3.5*sc);
	      glVertex2f( 2.0*sc, 0.5*sc);
	      glVertex2f( 0.0*sc, 0.5*sc);
	      glEnd();
	      glPopMatrix();
	    }
	    
	    
	    for (int i=max(wpt_current-1,0);i<nwpt;i++) {
	    //for (int i=1;i<2;i++) {

	      int im1 = max(i-1,0);

	      // convert to azimuthal equidistant coordinates with acf in center
	      if ((wpt[im1].lon != FLT_MISS) && (wpt[im1].lat != FLT_MISS) &&
		  (wpt[i].lon != FLT_MISS) && (wpt[i].lat != FLT_MISS)) {

		
		/* convert all lonlat to azimuthal equidistant coordinates with acf in center */

		/* Start of Leg": Fix is start of straight leg */
		lon = (double) wpt[im1].lon;
		lat = (double) wpt[im1].lat;
		lonlat2gnomonic(&lon, &lat, &easting, &northing, &MapCenterLon, &MapCenterLat);
		yPos = -northing / 1852.0 / mapRange * map_size; 
		xPos = easting / 1852.0  / mapRange * map_size;

		/* End of Leg: Fix is Label Position and end of straight leg */
		/* But may also be off the fix and beginning of next arc leg */
		lon2 = (double) wpt[i].lon;
		lat2 = (double) wpt[i].lat;
		lonlat2gnomonic(&lon2, &lat2, &easting, &northing, &MapCenterLon, &MapCenterLat);
		yPosL = -northing / 1852.0 / mapRange * map_size; 
		xPosL = easting / 1852.0  / mapRange * map_size;

		/* End of leg may also be start of next arc not at 
		   the waypoint except for last wpt */	       
		if ((seg1_ctr_lon[i] != 0) && (seg1_ctr_lat[i] != 0) &&
		    (seg1_ctr_lon[i] != FLT_MISS) && (seg1_ctr_lat[i] != FLT_MISS) && (i < (nwpt-1))) {
		  lon1 = (double) seg1_ctr_lon[i];
		  lat1 = (double) seg1_ctr_lat[i];
		  distance = (double) seg1_radius[i];
		  heading = (double) seg1_start_ang[i];
		  latlon_at_dist_heading(&lon1, &lat1, &distance, &heading, &lon2, &lat2);

		  //printf("Leg %i Does not end at FIX %s: %f %f \n",i,wpt[i].name,wpt[i].lon,lon2);
		}
		  
		lonlat2gnomonic(&lon2, &lat2, &easting, &northing, &MapCenterLon, &MapCenterLat);
		yPos2 = -northing / 1852.0 / mapRange * map_size; 
		xPos2 = easting / 1852.0  / mapRange * map_size;

		// Only draw the waypoint if it's visible within the rendering area
		//if (( sqrt(xPos*xPos + yPos*yPos) < map_size) ||
		//    ( sqrt(xPos2*xPos2 + yPos2*yPos2) < map_size)) {

		glPushMatrix();
		
		/*
		glPushMatrix();		    
		glColor3ub(COLOR_GREEN);
		glPointSize(10.0);
		glBegin(GL_POINTS);
		glVertex2f(xPos, yPos);
		glEnd();		    
		glPopMatrix();
		
		glPushMatrix();		    
		glColor3ub(COLOR_RED);
		glPointSize(10.0);
		glBegin(GL_POINTS);
		glVertex2f(xPos2, yPos2);
		glEnd();		    
		glPopMatrix();
		*/
		
		/* only draw leg if it is not a Discontinuity in flight plan */
		if (strcmp(wpt[im1].name,"DISCONTINUITY") != 0) {
		  
		glLineWidth(lineWidth);
		if ((i >= wpt_miss1) && (i <= wpt_miss2)) {
		  // missed approach route
		  glColor3ub(COLOR_LIGHTBLUE);
		  glEnable(GL_LINE_STIPPLE);
		  glLineStipple( 4, 0x0F0F );
		} else {
		  // regular route
		  glColor3ub(COLOR_VIOLET);
		}

		// Draw Holding
		if ((strcmp(wpt[i].pth,"HM") == 0) ||
		    (strcmp(wpt[i].pth,"HF") == 0) ||
		    (strcmp(wpt[i].pth,"HA") == 0)) {
		  // holds are legs_pth
		  // "HA" (hold to altitude)
		  // "HF" (hold to a fix)
		  // "HM" (hold with manual termination)
		  // hold_radius = 1.5	-- 1.5 NM -> about 3 deg/sec at 250kts
		  // hold length is 3.0 NM for a 60 second hold
		  int holdtype = (int) fmc_rad_turn[i] * 2 - 1; // 1: Right -1: Left
		  /* hold speed */
		  float holdspeed = max(*ground_speed,210.f);
		  /* standard hold radius is around 1.5 nm */
		  /* 3deg/s curve (T=120s/360deg): r = v * T / 2pi */
		  float holdrad = pow(max(*true_air_speed,210.f),2.0) / 29127.0 / mapRange * map_size; // nm --> pixels
		  //float holdrad = 1.5 / mapRange * map_size; // nm --> pixels
		  float holdlen;
		  if (fmc_hold_time[i] != 0.0) {
		    holdlen = holdspeed * fmc_hold_time[i] / 3600.0 / mapRange * map_size;
		  } else if (fmc_hold_dist[i] != 0.0) {
		      holdlen = fmc_hold_dist[i] / mapRange * map_size;
		  } else {
		    /* default hold time per leg: 90 seconds */
		    holdlen = holdspeed * 90.0 / 3600.0 / mapRange * map_size;
		  }
	      
		  float gamma = (fmc_crs[i] - *magnetic_variation)*3.14/180.; // inbound direction (radians)
		  xPos1 = sin(gamma-3.14)*holdlen + xPos2; // start of inbound course
		  yPos1 = cos(gamma-3.14)*holdlen + yPos2; // Pos2 is end of inbound course
		  xPos3 = sin(gamma+0.5*3.14*holdtype)*holdrad*2.0 + xPos2; // start of outbound course 
		  yPos3 = cos(gamma+0.5*3.14*holdtype)*holdrad*2.0 + yPos2;
		  xPos4 = sin(gamma+0.5*3.14*holdtype)*holdrad*2.0 + xPos1; // end of outbound course 
		  yPos4 = cos(gamma+0.5*3.14*holdtype)*holdrad*2.0 + yPos1;

		  // printf("%i %i %i %f \n",i,holdtype, fmc_hold_time[i],fmc_crs[i]);
		  
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

		// draw curved track
		} else if ((seg1_ctr_lon[im1] != 0.0) &&
			   (seg1_ctr_lat[im1] != 0.0) &&
			   (seg1_end_lon[im1] != 0.0) &&
			   (seg1_end_lat[im1] != 0.0) &&
			   (seg1_radius[im1] != 0.0) &&
			   (seg1_start_ang[im1] != 0.0) &&
			   (seg1_end_ang[im1] != 0.0) &&
			   (seg1_turn[im1] != 0.0) &&
			   (seg1_ctr_lon[im1] != FLT_MISS) &&
			   (seg1_ctr_lat[im1] != FLT_MISS) &&
			   (seg1_end_lon[im1] != FLT_MISS) &&
			   (seg1_end_lat[im1] != FLT_MISS) &&
			   (seg1_radius[im1] != FLT_MISS) &&	
			   (seg1_start_ang[im1] != FLT_MISS) &&
			   (seg1_end_ang[im1] != FLT_MISS) &&
			   (seg1_turn[im1] != FLT_MISS) &&
			   (i > 1)) {

		  /* SEGMENT 1 */
		  
		  /* Center of Arc */
		  lon = (double) seg1_ctr_lon[im1];
		  lat = (double) seg1_ctr_lat[im1];
		  lonlat2gnomonic(&lon, &lat, &easting, &northing, &MapCenterLon, &MapCenterLat);
		  yPosC = -northing / 1852.0 / mapRange * map_size; 
		  xPosC = easting / 1852.0  / mapRange * map_size;		  

		  /* Start of Arc */
		  double start_angle = (double) seg1_start_ang[im1];
		  /* End of Arc */
		  double end_angle = (double) seg1_end_ang[im1];

		  if (0) {
		  double old_course = start_angle - 90.0;
		  if (old_course < 0.0) old_course += 360.0;
		  double new_course = end_angle - 90.0;
		  if (new_course < 0.0) new_course += 360.0;

		  double arc_angle;
		  if (fmc_turn[im1] == 2.0) {
		    /* 2: left turn */
		    arc_angle = old_course - new_course;
		  } else {
		    /* 3: right turn */
		    arc_angle = new_course - old_course;
		  }
		  if (arc_angle < 0) arc_angle = arc_angle + 360;

		  end_angle = start_angle;
		  if (fmc_turn[im1] == 2.0) {
		    /* left turn */
		    end_angle -= arc_angle;
		  } else {
		    /* right turn */
		    end_angle += arc_angle;
		  }

		  }
		  
		  //if (end_angle >= 360.0) end_angle -= 360.0;
		  //if (end_angle < 0.0) end_angle += 360.0;
		  
		  // draw curved leg (partial circle)
		  aCircle.SetDegreesPerPoint(2);
		  //if (((fmc_turn[im1] == 2) && (end_angle > start_angle)) ||
		  //    ((fmc_turn[im1] == 3) && (end_angle < start_angle))) {
		  //		  if ((abs(end_angle-start_angle) > 1.1*arc_angle) ||
		  //     (end_angle < start_angle)) {
		    /* turn over 0/360: reverse angle order to choose shorter arc */
		  if (seg1_turn[im1] == 2.0) {
		    /* left turn */
		    aCircle.SetArcStartEnd(end_angle,start_angle);
		  } else {
		    /* right turn */
		    aCircle.SetArcStartEnd(start_angle,end_angle);
		  }
		  aCircle.SetRadius(seg1_radius[im1] / mapRange * map_size);
		  aCircle.SetOrigin(xPosC,yPosC);
		  glBegin(GL_LINE_STRIP);
		  aCircle.Evaluate();
		  glEnd();

		  /* lon/lat at end of seg1 arc */
		  lonT = (double) seg1_end_lon[im1];
		  latT = (double) seg1_end_lat[im1];
		    		    
		  /* SEGMENT 2 */
		  if ((seg2_end_lon[im1] != 0) && (seg2_end_lat[im1] != 0) &&
		      (seg2_end_lon[im1] != FLT_MISS) && (seg2_end_lat[im1] != FLT_MISS)) {
		    
		    lon = (double) seg1_end_lon[im1];
		    lat = (double) seg1_end_lat[im1];
		    lonlat2gnomonic(&lon, &lat, &easting, &northing, &MapCenterLon, &MapCenterLat);
		    yPos3 = -northing / 1852.0 / mapRange * map_size; 
		    xPos3 = easting / 1852.0  / mapRange * map_size;
		    lon = (double) seg2_end_lon[im1];
		    lat = (double) seg2_end_lat[im1];
		    lonlat2gnomonic(&lon, &lat, &easting, &northing, &MapCenterLon, &MapCenterLat);
		    yPos4 = -northing / 1852.0 / mapRange * map_size; 
		    xPos4 = easting / 1852.0  / mapRange * map_size;
		    		    
		    glBegin(GL_LINES);
		    glVertex2f(xPos3,yPos3);
		    glVertex2f(xPos4,yPos4);
		    glEnd();

		    /* end lon/lat of segment 2 */
		    lonT = (double) seg2_end_lon[im1];
		    latT = (double) seg2_end_lat[im1];
		  
		  }

		  /* SEGMENT 3 */
		  if ((seg3_ctr_lon[im1] != 0.0) &&
		      (seg3_ctr_lat[im1] != 0.0) &&
		      (seg3_end_lon[im1] != 0.0) &&
		      (seg3_end_lat[im1] != 0.0) &&
		      (seg3_radius[im1] != 0.0) &&
		      (seg3_start_ang[im1] != 0.0) &&
		      (seg3_end_ang[im1] != 0.0) &&
		      (seg3_turn[im1] != 0.0) &&
		      (seg3_ctr_lon[im1] != FLT_MISS) &&
		      (seg3_ctr_lat[im1] != FLT_MISS) &&
		      (seg3_end_lon[im1] != FLT_MISS) &&
		      (seg3_end_lat[im1] != FLT_MISS) &&
		      (seg3_radius[im1] != FLT_MISS) &&	
		      (seg3_start_ang[im1] != FLT_MISS) &&
		      (seg3_end_ang[im1] != FLT_MISS) &&
		      (seg3_turn[im1] != FLT_MISS) &&
		      (i > 1)) {
		    
		    /* Center of Arc */
		    lon = (double) seg3_ctr_lon[im1];
		    lat = (double) seg3_ctr_lat[im1];
		    lonlat2gnomonic(&lon, &lat, &easting, &northing, &MapCenterLon, &MapCenterLat);
		    yPosC = -northing / 1852.0 / mapRange * map_size; 
		    xPosC = easting / 1852.0  / mapRange * map_size;		  

		    /* Start of Arc */
		    double start_angle = (double) seg3_start_ang[im1];
		    /* End of Arc */
		    double end_angle = (double) seg3_end_ang[im1];

		    aCircle.SetDegreesPerPoint(2);
		    if (seg3_turn[im1] == 2.0) {
		      /* left turn */
		      aCircle.SetArcStartEnd(end_angle,start_angle);
		    } else {
		      /* right turn */
		      aCircle.SetArcStartEnd(start_angle,end_angle);
		    }
		    aCircle.SetRadius(seg1_radius[im1] / mapRange * map_size);
		    aCircle.SetOrigin(xPosC,yPosC);
		    glBegin(GL_LINE_STRIP);
		    aCircle.Evaluate();
		    glEnd();

		    lonT = (double) seg3_end_lon[im1];
		    latT = (double) seg3_end_lat[im1];
		  }

		  /*
		  glPushMatrix();
		  glColor3ub(COLOR_BLUE);
		  glPointSize(8.0);
		  glBegin(GL_POINTS);
		  glVertex2f(xPosC, yPosC);
		  glEnd();		  
		  glPopMatrix();
		  */

		  /* Draw straight Line from End of Arcs to next start of Leg */	     
		  lonlat2gnomonic(&lonT, &latT, &easting, &northing, &MapCenterLon, &MapCenterLat);
		  yPosT = -northing / 1852.0 / mapRange * map_size; 
		  xPosT = easting / 1852.0  / mapRange * map_size;
		 
		  /*
		  glPushMatrix();
		  glColor3ub(COLOR_YELLOW);
		  glPointSize(12.0);
		  glBegin(GL_POINTS);
		  glVertex2f(xPosT, yPosT);
		  glEnd();		  
		  glPopMatrix();
		  */
		 
		  glBegin(GL_LINES);
		  glVertex2f(xPosT,yPosT);
		  glVertex2f(xPos2,yPos2);
		  glEnd();

		} else {
		  // draw straight line

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
		  glColor3ub(COLOR_VIOLET);
		} else {
		  glColor3ub(COLOR_WHITE);
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
		  if (fmc_alt[i] > 0.0) {
		    if (fmc_alt[i] >= 8000.0) {
		      snprintf( buffer, sizeof(buffer), "FL%03i", (int) fmc_alt[i]/100 );
		      m_pFontManager->Print(4,-11, buffer, m_Font);
		    } else {
		      snprintf( buffer, sizeof(buffer), "%i", (int) fmc_alt[i] );
		      m_pFontManager->Print(4,-11, buffer, m_Font);
		    }
		  }
		}

		glPopMatrix();
		
	      }
	      
	    } /* cycle through waypoints */
    
	    glPopMatrix();

	    if (wpt_current < nwpt) {	    
	      /* Draw currently active waypoint at UR corner of NAV display */
	      glPushMatrix();
	      glColor3ub(COLOR_VIOLET);
	      m_pFontManager->SetSize(m_Font, 0.9*fontSize, 1.0*fontSize);
	      m_pFontManager->Print(0.82*m_PhysicalSize.x,0.95*m_PhysicalSize.y, wpt[wpt_current].name, m_Font);
	      glColor3ub(COLOR_WHITE);
	      int hour=fmc_eta[wpt_current];
	      float minute=(fmc_eta[wpt_current] - (float) hour)*60.0;
	      snprintf( buffer, sizeof(buffer), "%02d%04.1f z", hour, minute );
	      m_pFontManager->Print(0.82*m_PhysicalSize.x,0.91*m_PhysicalSize.y, buffer, m_Font);
	      
	      lon = (double) wpt[wpt_current].lon;
	      lat = (double) wpt[wpt_current].lat;
	      lonlat2gnomonic(&lon, &lat, &easting, &northing, aircraftLon, aircraftLat);
	      northing *= 0.00054;
	      easting *= 0.00054;
	      float distance = sqrt(easting*easting + northing*northing);
	      
	      snprintf( buffer, sizeof(buffer), "%.1f NM", distance );
	      m_pFontManager->Print(0.82*m_PhysicalSize.x,0.87*m_PhysicalSize.y, buffer, m_Font);
	      glPopMatrix();
	    } 
	      
	  } else {
	    glPushMatrix();
	    glColor3ub(COLOR_WHITE);
	    snprintf( buffer, sizeof(buffer), "--------z");
	    m_pFontManager->Print(0.82*m_PhysicalSize.x,0.91*m_PhysicalSize.y, buffer, m_Font);
	    snprintf( buffer, sizeof(buffer), "----- NM");
	    m_pFontManager->Print(0.82*m_PhysicalSize.x,0.87*m_PhysicalSize.y, buffer, m_Font);
	    glPopMatrix();
	  } /* has waypoints */
	} /* we are on either UFMC or ZIBO FMC for 737 */
	
      } // valid acf coordinates

    }
    
  }

} // end namespace OpenGC

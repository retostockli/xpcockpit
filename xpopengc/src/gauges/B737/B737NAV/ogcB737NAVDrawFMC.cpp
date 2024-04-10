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

    // parameters for dashed lines (missed approach route)
    int nper100 = 16; /* number of dashed per 100 pixels */
    float ratio = 0.6; /* ratio of dashed to total length */

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
    //    float *fmc_turn;
    float *fmc_alt;
    //    float *fmc_type;
    float *fmc_hold_time;
    float *fmc_hold_dist;
    //    float *fmc_dist;
    float *fmc_eta;

    float *fmc_rad_lon;
    float *fmc_rad_lat;
    float *fmc_rad_turn;
    float *fmc_radius;
    //    float *fmc_brg;
    float *fmc_crs;
    int *fmc_miss1;
    int *fmc_miss2;
    int *fmc_cur;
    int *fmc_ctr;
    float *fmc_bypass;
    float *fmc_rnp;
    float *fmc_anp;
    float *fmc_vrnp;
    float *fmc_vanp;
    float *fmc_vnav_td_dist;
    float *fmc_vnav_err;
    int *fmc_has_vrnp;
    //    int *fmc_rnav_enable;
    float *fmc_rnav_alt;
    unsigned char *fmc_name;
    unsigned char *fmc_pth;
    int *fmc_idx;
    int *fmc_nidx;

    float *fmc_af_beg;
    float *fmc_af_end;
    
    //    float *seg1_start_lon;
    //    float *seg1_start_lat;
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

    unsigned char *ref_apt_name;
    //    float *ref_apt_alt;
    float *ref_rwy_lon0;
    float *ref_rwy_lon1;
    float *ref_rwy_lat0;
    float *ref_rwy_lat1;
    //    float *ref_rwy_len;
    float *ref_rwy_crs;
    unsigned char *des_apt_name;
    //    float *des_apt_alt;
    float *des_rwy_lon0;
    float *des_rwy_lon1;
    float *des_rwy_lat0;
    float *des_rwy_lat1;
    //    float *des_rwy_len;
    float *des_rwy_crs;

    float *tc_lon;
    float *tc_lat;
    float *tc_show;
    float *td_lon;
    float *td_lat;
    float *td_show;
    
    if (acf_type == 3) {
      /* ZIBO 737 FMC */
      fmc_name = link_dataref_byte_arr("laminar/B738/fms/legs",256,-1);
      fmc_pth = link_dataref_byte_arr("laminar/B738/fms/legs_pth",256,-1);
      fmc_lon = link_dataref_flt_arr("laminar/B738/fms/legs_lon",256,-1,-5);
      fmc_lat = link_dataref_flt_arr("laminar/B738/fms/legs_lat",256,-1,-5);
      fmc_alt = link_dataref_flt_arr("laminar/B738/fms/legs_alt_rest1",256,-1,0);
      fmc_ctr = link_dataref_int("laminar/B738/fms/legs_step_ctr"); 
      fmc_cur = link_dataref_int("laminar/B738/fms/vnav_idx"); 
      //      fmc_turn = link_dataref_flt_arr("laminar/B738/fms/legs_turn",256,-1,0);
      //      fmc_type = link_dataref_flt_arr("laminar/B738/fms/legs_type",256,-1,0);
      fmc_hold_time = link_dataref_flt_arr("laminar/B738/fms/legs_hold_time",256,-1,-1);
      fmc_hold_dist = link_dataref_flt_arr("laminar/B738/fms/legs_hold_dist",256,-1,-1);
      //      fmc_dist = link_dataref_flt_arr("laminar/B738/fms/legs_dist",256,-1,-2);
      fmc_eta = link_dataref_flt_arr("laminar/B738/fms/legs_eta",256,-1,-2);
      fmc_rad_lon = link_dataref_flt_arr("laminar/B738/fms/legs_rad_lon",256,-1,-4);
      fmc_rad_lat = link_dataref_flt_arr("laminar/B738/fms/legs_rad_lat",256,-1,-4);
      fmc_rad_turn = link_dataref_flt_arr("laminar/B738/fms/legs_rad_turn",256,-1,0);
      fmc_radius = link_dataref_flt_arr("laminar/B738/fms/legs_radius",256,-1,-1);
      fmc_miss1 = link_dataref_int("laminar/B738/fms/missed_app_wpt_idx");
      fmc_miss2 = link_dataref_int("laminar/B738/fms/missed_app_wpt_idx2");
      fmc_bypass = link_dataref_flt_arr("laminar/B738/fms/legs_bypass",256,-1,0);
      //      fmc_brg = link_dataref_flt_arr("laminar/B738/fms/legs_brg_mag",256,-1,-5);
      fmc_crs = link_dataref_flt_arr("laminar/B738/fms/legs_crs_mag",256,-1,-5);
      fmc_rnp = link_dataref_flt("laminar/B738/fms/rnp",-2);
      fmc_anp = link_dataref_flt("laminar/B738/fms/anp",-2);
      fmc_vrnp = link_dataref_flt("laminar/B738/fms/vrnp",-2);
      fmc_vanp = link_dataref_flt("laminar/B738/fms/vanp",-2);
      fmc_vnav_td_dist = link_dataref_flt("laminar/B738/fms/vnav_td_dist",-1);
      fmc_vnav_err = link_dataref_flt("laminar/B738/fms/vnav_err_pfd",0);
      //      fmc_has_vrnp = link_dataref_int("laminar/B738/fms/vrnp_enable");
      fmc_has_vrnp = link_dataref_int("laminar/B738/pfd/nd_vert_path");
      //      fmc_rnav_enable = link_dataref_int("laminar/B738/fms/rnav_enable");
      fmc_rnav_alt = link_dataref_flt("laminar/B738/fms/rnav_alt",0);
      fmc_nidx = link_dataref_int("laminar/B738/fms/num_of_wpts");

      fmc_af_beg = link_dataref_flt_arr("laminar/B738/fms/legs_af_beg",256,-1,-1);
      fmc_af_end = link_dataref_flt_arr("laminar/B738/fms/legs_af_end",256,-1,-1);

      //      seg1_start_lon = link_dataref_flt_arr("laminar/B738/fms/legs_seg1_start_lon",256,-1,-4);
      //      seg1_start_lat = link_dataref_flt_arr("laminar/B738/fms/legs_seg1_start_lat",256,-1,-4);
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

      ref_apt_name = link_dataref_byte_arr("laminar/B738/fms/ref_list_rwy_icao",20,-1);
      //      ref_apt_alt = link_dataref_flt("laminar/B738/fms/ref_icao_alt",0);
      ref_rwy_lon0 = link_dataref_flt("laminar/B738/fms/ref_runway_start_lon",-5);
      ref_rwy_lat0 = link_dataref_flt("laminar/B738/fms/ref_runway_start_lat",-5);
      ref_rwy_lon1 = link_dataref_flt("laminar/B738/fms/ref_runway_end_lon",-5);
      ref_rwy_lat1 = link_dataref_flt("laminar/B738/fms/ref_runway_end_lat",-5);
      //      ref_rwy_len = link_dataref_flt("laminar/B738/fms/ref_runway_len",-5);
      ref_rwy_crs = link_dataref_flt("laminar/B738/fms/ref_runway_crs",-5);

      des_apt_name = link_dataref_byte_arr("laminar/B738/fms/des_list_rwy_icao",20,-1);
      //      des_apt_alt = link_dataref_flt("laminar/B738/fms/des_icao_alt",0);
      des_rwy_lon0 = link_dataref_flt("laminar/B738/fms/dest_runway_start_lon",-5);
      des_rwy_lat0 = link_dataref_flt("laminar/B738/fms/dest_runway_start_lat",-5);
      des_rwy_lon1 = link_dataref_flt("laminar/B738/fms/dest_runway_end_lon",-5);
      des_rwy_lat1 = link_dataref_flt("laminar/B738/fms/dest_runway_end_lat",-5);
      //      des_rwy_len = link_dataref_flt("laminar/B738/fms/dest_runway_len",-5);
      des_rwy_crs = link_dataref_flt("laminar/B738/fms/dest_runway_crs",-5);

      tc_lon = link_dataref_flt("laminar/B738/nd/tc_lon",-4);
      tc_lat = link_dataref_flt("laminar/B738/nd/tc_lat",-4);
      tc_show = link_dataref_flt("laminar/B738/nd/tc_show",0);

      td_lon = link_dataref_flt("laminar/B738/nd/td_lon",-4);
      td_lat = link_dataref_flt("laminar/B738/nd/td_lat",-4);
      td_show = link_dataref_flt("laminar/B738/nd/td_show",0);
      
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
	float xPos0;
	float yPos0;
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
	    
	    /* We draw from last to present waypoint */
	    int i;
	    i = max(wpt_current-1,0);
	    while (i<nwpt) {

	      int i0 = max(i-1,0);
	      while (fmc_bypass[i0] == 1.0) {
		i0 -= 1;
		if (i0 < 0) break;
	      }
	      int i1 = i;
	      while (fmc_bypass[i1] == 1.0) {
		i1 += 1;
		if (i1 >= nwpt) break;
	      }

	      /* Do not draw from last waypoint to destination airport */
	      /* Do only draw within waypoint range */
	      if ((i0 >= 0) && (i1 < nwpt)) {

		//		printf("%i %i \n",i0,i1);
		
		// convert to azimuthal equidistant coordinates with acf in center
		if ((wpt[i0].lon != FLT_MISS) && (wpt[i0].lat != FLT_MISS) &&
		    (wpt[i1].lon != FLT_MISS) && (wpt[i1].lat != FLT_MISS)) {

		
		  /* convert all lonlat to azimuthal equidistant coordinates with acf in center */

		  /* Start of Leg": Fix is start of straight leg */
		  lon = (double) wpt[i0].lon;
		  lat = (double) wpt[i0].lat;
		  lonlat2gnomonic(&lon, &lat, &easting, &northing, &MapCenterLon, &MapCenterLat);
		  yPos = -northing / 1852.0 / mapRange * map_size; 
		  xPos = easting / 1852.0  / mapRange * map_size;

		  /* End of Leg: Fix is Label Position and end of straight leg */
		  /* But may also be off the fix and beginning of next arc leg */
		  lon2 = (double) wpt[i1].lon;
		  lat2 = (double) wpt[i1].lat;
		  lonlat2gnomonic(&lon2, &lat2, &easting, &northing, &MapCenterLon, &MapCenterLat);
		  yPosL = -northing / 1852.0 / mapRange * map_size; 
		  xPosL = easting / 1852.0  / mapRange * map_size;

		  /* End of leg may also be start of next arc not at 
		     the waypoint except for last wpt */	       
		  if ((seg1_ctr_lon[i1] != 0) && (seg1_ctr_lat[i1] != 0) &&
		      (seg1_ctr_lon[i1] != FLT_MISS) && (seg1_ctr_lat[i1] != FLT_MISS) && (i < (nwpt-1))) {
		    lon1 = (double) seg1_ctr_lon[i1];
		    lat1 = (double) seg1_ctr_lat[i1];
		    distance = (double) seg1_radius[i1];
		    heading = (double) seg1_start_ang[i1];
		    latlon_at_dist_heading(&lon1, &lat1, &distance, &heading, &lon2, &lat2);

		    //printf("Leg %i Does not end at FIX %s: %f %f \n",i,wpt[i1].name,wpt[i1].lon,lon2);
		  }
		  
		  lonlat2gnomonic(&lon2, &lat2, &easting, &northing, &MapCenterLon, &MapCenterLat);
		  yPos2 = -northing / 1852.0 / mapRange * map_size; 
		  xPos2 = easting / 1852.0  / mapRange * map_size;

		  // Only draw the waypoint if it's visible within the rendering area
		  //if (( sqrt(xPos*xPos + yPos*yPos) < map_size) ||
		  //    ( sqrt(xPos2*xPos2 + yPos2*yPos2) < map_size)) {

		  // Do not draw magenta line before current waypoint and not the last leg to the airport
		  if (i0>=(wpt_current-1) && (i1<(nwpt-1))) {
		  
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
		
		  /* only draw leg if it is not a Discontinuity/Vector/RW in flight plan */
		  if ((strcmp(wpt[i0].name,"DISCONTINUITY") != 0) && (strcmp(wpt[i0].name,"(VECTOR)") != 0) &&
		      (strncmp(wpt[i0].name,"RW",2) != 0)) {
		  
		    glLineWidth(lineWidth);
		    bool missed_app_wpt;
		    if ((i >= (wpt_miss1-1)) && (i <= wpt_miss2)) {
		      // missed approach route
		      missed_app_wpt = true;
		      glColor3ub(COLOR_LIGHTBLUE);
		      /* Does not work with OpenGL ES */
		      //glEnable(GL_LINE_STIPPLE);
		      //glLineStipple( 4, 0x0F0F );
		    } else {
		      // regular route
		      missed_app_wpt = false;
		      glColor3ub(COLOR_VIOLET);
		    }

		    // Draw Holding
		    if ((strcmp(wpt[i1].pth,"HM") == 0) ||
			(strcmp(wpt[i1].pth,"HF") == 0) ||
			(strcmp(wpt[i1].pth,"HA") == 0)) {
		      // holds are legs_pth
		      // "HA" (hold to altitude)
		      // "HF" (hold to a fix)
		      // "HM" (hold with manual termination)
		      // hold_radius = 1.5	-- 1.5 NM -> about 3 deg/sec at 250kts
		      // hold length is 3.0 NM for a 60 second hold
		      int holdtype = (int) fmc_rad_turn[i1] * 2 - 1; // 1: Right -1: Left
		      /* hold speed */
		      float holdspeed = max(*ground_speed,210.f);
		      /* standard hold radius is around 1.5 nm */
		      /* 3deg/s curve (T=120s/360deg): r = v * T / 2pi */
		      float holdrad = pow(max(*true_air_speed,210.f),2.0) / 29127.0 / mapRange * map_size; // nm --> pixels
		      //float holdrad = 1.5 / mapRange * map_size; // nm --> pixels
		      float holdlen;
		      if (fmc_hold_time[i1] != 0.0) {
			holdlen = holdspeed * fmc_hold_time[i1] / 3600.0 / mapRange * map_size;
		      } else if (fmc_hold_dist[i1] != 0.0) {
			holdlen = fmc_hold_dist[i1] / mapRange * map_size;
		      } else {
			/* default hold time per leg: 90 seconds */
			holdlen = holdspeed * 90.0 / 3600.0 / mapRange * map_size;
		      }
	      
		      float gamma = (fmc_crs[i1] - *magnetic_variation)*3.14/180.; // inbound direction (radians)
		      xPos1 = sin(gamma-3.14)*holdlen + xPos2; // start of inbound course
		      yPos1 = cos(gamma-3.14)*holdlen + yPos2; // Pos2 is end of inbound course
		      xPos3 = sin(gamma+0.5*3.14*holdtype)*holdrad*2.0 + xPos2; // start of outbound course 
		      yPos3 = cos(gamma+0.5*3.14*holdtype)*holdrad*2.0 + yPos2;
		      xPos4 = sin(gamma+0.5*3.14*holdtype)*holdrad*2.0 + xPos1; // end of outbound course 
		      yPos4 = cos(gamma+0.5*3.14*holdtype)*holdrad*2.0 + yPos1;

		      // printf("%i %i %i %f \n",i,holdtype, fmc_hold_time[i1],fmc_crs[i1]);

		      if (missed_app_wpt) {
			drawDashedLine(xPos1,yPos1,xPos2,yPos2,nper100,ratio);
		      } else {
			glBegin(GL_LINES);
			glVertex2f(xPos1,yPos1);
			glVertex2f(xPos2,yPos2);
			glEnd();
		      }

		      xPosC = 0.5*(xPos3-xPos2) + xPos2;
		      yPosC = 0.5*(yPos3-yPos2) + yPos2;
		      aCircle.SetDegreesPerPoint(5);
		      aCircle.SetArcStartEnd(180./3.14*(gamma-0.5*3.14),180./3.14*(gamma+0.5*3.14));
		      aCircle.SetRadius(holdrad);
		      aCircle.SetOrigin(xPosC,yPosC);
		      if (missed_app_wpt) {
			aCircle.SetDashed(nper100,ratio);
		      } else {
			aCircle.SetDashed(0,1.0);
		      }
		      glBegin(GL_LINE_STRIP);
		      aCircle.Evaluate();
		      glEnd();
  		  
		      if (missed_app_wpt) {
			drawDashedLine(xPos3,yPos3,xPos4,yPos4,nper100,ratio);
		      } else {
			glBegin(GL_LINES);
			glVertex2f(xPos3,yPos3);
			glVertex2f(xPos4,yPos4);
			glEnd();
		      }
		  
		      xPosC = 0.5*(xPos4-xPos1) + xPos1;
		      yPosC = 0.5*(yPos4-yPos1) + yPos1;
		      aCircle.SetDegreesPerPoint(5);
		      aCircle.SetArcStartEnd(180./3.14*(gamma+0.5*3.14),180./3.14*(gamma-0.5*3.14));
		      aCircle.SetRadius(holdrad);
		      aCircle.SetOrigin(xPosC,yPosC);
		      if (missed_app_wpt) {
			aCircle.SetDashed(nper100,ratio);
		      } else {
			aCircle.SetDashed(0,1.0);
		      }
		      glBegin(GL_LINE_STRIP);
		      aCircle.Evaluate();
		      glEnd();

		      // draw curved track (RADII Type with Segments 1-3)
		    } else if ((seg1_ctr_lon[i0] != 0.0) &&
			       (seg1_ctr_lat[i0] != 0.0) &&
			       (seg1_end_lon[i0] != 0.0) &&
			       (seg1_end_lat[i0] != 0.0) &&
			       (seg1_radius[i0] != 0.0) &&
			       (seg1_start_ang[i0] != 0.0) &&
			       (seg1_end_ang[i0] != 0.0) &&
			       (seg1_turn[i0] != 0.0) &&
			       (seg1_ctr_lon[i0] != FLT_MISS) &&
			       (seg1_ctr_lat[i0] != FLT_MISS) &&
			       (seg1_end_lon[i0] != FLT_MISS) &&
			       (seg1_end_lat[i0] != FLT_MISS) &&
			       (seg1_radius[i0] != FLT_MISS) &&	
			       (seg1_start_ang[i0] != FLT_MISS) &&
			       (seg1_end_ang[i0] != FLT_MISS) &&
			       (seg1_turn[i0] != FLT_MISS) &&
			       (i > 1)) {

		      /* SEGMENT 1 */
		  
		      /* Center of Arc */
		      lon = (double) seg1_ctr_lon[i0];
		      lat = (double) seg1_ctr_lat[i0];
		      lonlat2gnomonic(&lon, &lat, &easting, &northing, &MapCenterLon, &MapCenterLat);
		      yPosC = -northing / 1852.0 / mapRange * map_size; 
		      xPosC = easting / 1852.0  / mapRange * map_size;		  

		      /* Start of Arc */
		      double start_angle = (double) seg1_start_ang[i0];
		      /* End of Arc */
		      double end_angle = (double) seg1_end_ang[i0];
		  
		      // draw curved leg (partial circle)
		      aCircle.SetDegreesPerPoint(2);
		      if (seg1_turn[i0] == 2.0) {
			/* left turn */
			aCircle.SetArcStartEnd(end_angle,start_angle);
		      } else {
			/* right turn */
			aCircle.SetArcStartEnd(start_angle,end_angle);
		      }
		      aCircle.SetRadius(seg1_radius[i0] / mapRange * map_size);
		      aCircle.SetOrigin(xPosC,yPosC);
		      if (missed_app_wpt) {
			aCircle.SetDashed(nper100,ratio);
		      } else {
			aCircle.SetDashed(0,1.0);
		      }

		      glBegin(GL_LINE_STRIP);
		      aCircle.Evaluate();
		      glEnd();

		      /* lon/lat at end of seg1 arc */
		      lonT = (double) seg1_end_lon[i0];
		      latT = (double) seg1_end_lat[i0];
		    		    
		      /* SEGMENT 2 */
		      if ((seg2_end_lon[i0] != 0) && (seg2_end_lat[i0] != 0) &&
			  (seg2_end_lon[i0] != FLT_MISS) && (seg2_end_lat[i0] != FLT_MISS)) {
		    
			lon = (double) seg1_end_lon[i0];
			lat = (double) seg1_end_lat[i0];
			lonlat2gnomonic(&lon, &lat, &easting, &northing, &MapCenterLon, &MapCenterLat);
			yPos3 = -northing / 1852.0 / mapRange * map_size; 
			xPos3 = easting / 1852.0  / mapRange * map_size;
			lon = (double) seg2_end_lon[i0];
			lat = (double) seg2_end_lat[i0];
			lonlat2gnomonic(&lon, &lat, &easting, &northing, &MapCenterLon, &MapCenterLat);
			yPos4 = -northing / 1852.0 / mapRange * map_size; 
			xPos4 = easting / 1852.0  / mapRange * map_size;
		    		    
			if (missed_app_wpt) {
			  drawDashedLine(xPos3,yPos3,xPos4,yPos4,nper100,ratio);
			} else {
			  glBegin(GL_LINES);
			  glVertex2f(xPos3,yPos3);
			  glVertex2f(xPos4,yPos4);
			  glEnd();
			}

			/* end lon/lat of segment 2 */
			lonT = (double) seg2_end_lon[i0];
			latT = (double) seg2_end_lat[i0];
		  
		      }

		      /* SEGMENT 3 */
		      if ((seg3_ctr_lon[i0] != 0.0) &&
			  (seg3_ctr_lat[i0] != 0.0) &&
			  (seg3_end_lon[i0] != 0.0) &&
			  (seg3_end_lat[i0] != 0.0) &&
			  (seg3_radius[i0] != 0.0) &&
			  (seg3_start_ang[i0] != 0.0) &&
			  (seg3_end_ang[i0] != 0.0) &&
			  (seg3_turn[i0] != 0.0) &&
			  (seg3_ctr_lon[i0] != FLT_MISS) &&
			  (seg3_ctr_lat[i0] != FLT_MISS) &&
			  (seg3_end_lon[i0] != FLT_MISS) &&
			  (seg3_end_lat[i0] != FLT_MISS) &&
			  (seg3_radius[i0] != FLT_MISS) &&	
			  (seg3_start_ang[i0] != FLT_MISS) &&
			  (seg3_end_ang[i0] != FLT_MISS) &&
			  (seg3_turn[i0] != FLT_MISS) &&
			  (i > 1)) {
		    
			/* Center of Arc */
			lon = (double) seg3_ctr_lon[i0];
			lat = (double) seg3_ctr_lat[i0];
			lonlat2gnomonic(&lon, &lat, &easting, &northing, &MapCenterLon, &MapCenterLat);
			yPosC = -northing / 1852.0 / mapRange * map_size; 
			xPosC = easting / 1852.0  / mapRange * map_size;		  

			/* Start of Arc */
			double start_angle = (double) seg3_start_ang[i0];
			/* End of Arc */
			double end_angle = (double) seg3_end_ang[i0];

			aCircle.SetDegreesPerPoint(2);
			if (seg3_turn[i0] == 2.0) {
			  /* left turn */
			  aCircle.SetArcStartEnd(end_angle,start_angle);
			} else {
			  /* right turn */
			  aCircle.SetArcStartEnd(start_angle,end_angle);
			}
			aCircle.SetRadius(seg1_radius[i0] / mapRange * map_size);
			aCircle.SetOrigin(xPosC,yPosC);
			if (missed_app_wpt) {
			  aCircle.SetDashed(nper100,ratio);
			} else {
			  aCircle.SetDashed(0,1.0);
			}
			glBegin(GL_LINE_STRIP);
			aCircle.Evaluate();
			glEnd();

			lonT = (double) seg3_end_lon[i0];
			latT = (double) seg3_end_lat[i0];
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
		 
		      if (missed_app_wpt) {
			drawDashedLine(xPosT,yPosT,xPos2,yPos2,nper100,ratio);
		      } else {
			glBegin(GL_LINES);
			glVertex2f(xPosT,yPosT);
			glVertex2f(xPos2,yPos2);
			glEnd();
		      }

		      // Rad turn
		    } else if ((fmc_rad_turn[i1] != -1.0) &&
			       (fmc_rad_lon[i1] != 0.0) &&
			       (fmc_rad_lat[i1] != 0.0) &&
			       (fmc_radius[i1] >= 0.1)) {

		      //printf("%i %f %f %f \n",i1,fmc_rad_turn[i1],fmc_rad_lon[i1],fmc_rad_lat[i1]);
		      /* Center of Arc */
		      lon = (double) fmc_rad_lon[i1];
		      lat = (double) fmc_rad_lat[i1];
		      lonlat2gnomonic(&lon, &lat, &easting, &northing, &MapCenterLon, &MapCenterLat);
		      yPosC = -northing / 1852.0 / mapRange * map_size; 
		      xPosC = easting / 1852.0  / mapRange * map_size;		  
		      
		      /* Start of Arc */
		      double start_angle;
		      if (fmc_af_beg[i1] != 0.0) {
			start_angle = (double) fmc_af_beg[i1];
		      } else {
			lon2 = (double) wpt[i0].lon;
			lat2 = (double) wpt[i0].lat;
			start_angle = heading_from_a_to_b(&lon,&lat,&lon2,&lat2);
		      }
		      
		      /* End of Arc */
		      double end_angle;
		      if (fmc_af_end[i1] != 0.0) {
			end_angle = (double) fmc_af_end[i1];
		      } else {
			lon2 = (double) wpt[i1].lon;
			lat2 = (double) wpt[i1].lat;
			end_angle = heading_from_a_to_b(&lon,&lat,&lon2,&lat2);
		      }
		      
		      aCircle.SetDegreesPerPoint(2);
		      if (fmc_rad_turn[i1] == 2.0) {
			/* left turn */
			aCircle.SetArcStartEnd(end_angle,start_angle);
		      } else {
			/* right turn */
			aCircle.SetArcStartEnd(start_angle,end_angle);
		      }

		      aCircle.SetRadius(fmc_radius[i1] / mapRange * map_size);
		      aCircle.SetOrigin(xPosC,yPosC);
		      if (missed_app_wpt) {
			aCircle.SetDashed(nper100,ratio);
		      } else {
			aCircle.SetDashed(0,1.0);
		      }
		      glBegin(GL_LINE_STRIP);
		      aCircle.Evaluate();
		      glEnd();
		      

		    } else {
		      // draw straight line

		      if (missed_app_wpt) {
			drawDashedLine(xPos,yPos,xPos2,yPos2,nper100,ratio);
		      } else {
			glBegin(GL_LINES);
			glVertex2f(xPos,yPos);
			glVertex2f(xPos2,yPos2);
			glEnd();
		      }
		    }

		    /* Does not work with OpenGL ES */
		    //glDisable(GL_LINE_STIPPLE);

		  } // not a Discontinuity in flight plan

		  glPopMatrix();

		  } // do not draw magenta line to current waypoint and not to destination airport 

		  
		  // draw waypoint symbol and name
		  glPushMatrix();
		  glTranslatef(xPosL, yPosL, 0.0);
		  glRotatef(-1.0* heading_map, 0, 0, 1);

		  
		  if ((i1 == wpt_current) || (strcmp(wpt[i1].name,wpt[wpt_current].name) == 0)) {
		    /* print current waypoint or same waypoint in waypoint list which is the same
		       as the current waypoint in violett */
		    glColor3ub(COLOR_VIOLET);
		  } else {
		    glColor3ub(COLOR_WHITE);
		  }
		  if (strncmp(wpt[i1].name,"RW",2) != 0) {
		    /* draw waypoint symbol */
		    glLineWidth(lineWidth);
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
		  }
		
		  if ((strcmp(wpt[i1].name,"DISCONTINUITY") != 0) && (strcmp(wpt[i1].name,"(VECTOR)") != 0) &&
		      (strncmp(wpt[i1].name,"RW",2) != 0)) {
		    /* print waypoint name */
		    m_pFontManager->SetSize(m_Font, 0.65*fontSize, 0.65*fontSize);
		    m_pFontManager->Print(4,-6, wpt[i1].name, m_Font);
		    if (fmc_alt[i1] > 0.0) {
		      if (fmc_alt[i1] >= 8000.0) {
			snprintf( buffer, sizeof(buffer), "FL%03i", (int) fmc_alt[i1]/100 );
			m_pFontManager->Print(4,-11, buffer, m_Font);
		      } else {
			snprintf( buffer, sizeof(buffer), "%i", (int) fmc_alt[i1] );
			m_pFontManager->Print(4,-11, buffer, m_Font);
		      }
		    }
		  }

		  glPopMatrix();

		  /* draw runway symbol for reference airport */
		  if ((i0 < 3) && (strncmp(wpt[i1].name,"RW",2) == 0) &&
		      (strcmp(wpt[i0].name,(char*) ref_apt_name) == 0) &&
		      (*ref_rwy_lon0 != 0.0) && (*ref_rwy_lon1 != 0.0) &&
		      (*ref_rwy_lat0 != 0.0) && (*ref_rwy_lat0 != 0.0)) {

		    glPushMatrix();

		    double rwy_width = 0.15 * mapRange / 10.0;;
		    double rwy_hdg_L = *ref_rwy_crs - 90.0;
		    double rwy_hdg_R = *ref_rwy_crs + 90.0;

		    lon = (double) *ref_rwy_lon0;
		    lat = (double) *ref_rwy_lat0;
		    latlon_at_dist_heading(&lon, &lat, &rwy_width, &rwy_hdg_L, &lon2, &lat2);
		    lonlat2gnomonic(&lon2, &lat2, &easting, &northing, &MapCenterLon, &MapCenterLat);
		    yPos0 = -northing / 1852.0 / mapRange * map_size; 
		    xPos0 = easting / 1852.0  / mapRange * map_size;

		    lon = (double) *ref_rwy_lon1;
		    lat = (double) *ref_rwy_lat1;
		    latlon_at_dist_heading(&lon, &lat, &rwy_width, &rwy_hdg_L, &lon2, &lat2);
		    lonlat2gnomonic(&lon2, &lat2, &easting, &northing, &MapCenterLon, &MapCenterLat);
		    yPos1 = -northing / 1852.0 / mapRange * map_size; 
		    xPos1 = easting / 1852.0  / mapRange * map_size;

		    glLineWidth(lineWidth*1.5);
		    glBegin(GL_LINES);
		    glVertex2f(xPos0,yPos0);
		    glVertex2f(xPos1,yPos1);
		    glEnd();
		    
		    lon = (double) *ref_rwy_lon0;
		    lat = (double) *ref_rwy_lat0;
		    latlon_at_dist_heading(&lon, &lat, &rwy_width, &rwy_hdg_R, &lon2, &lat2);
		    lonlat2gnomonic(&lon2, &lat2, &easting, &northing, &MapCenterLon, &MapCenterLat);
		    yPos0 = -northing / 1852.0 / mapRange * map_size; 
		    xPos0 = easting / 1852.0  / mapRange * map_size;

		    lon = (double) *ref_rwy_lon1;
		    lat = (double) *ref_rwy_lat1;
		    latlon_at_dist_heading(&lon, &lat, &rwy_width, &rwy_hdg_R, &lon2, &lat2);
		    lonlat2gnomonic(&lon2, &lat2, &easting, &northing, &MapCenterLon, &MapCenterLat);
		    yPos1 = -northing / 1852.0 / mapRange * map_size; 
		    xPos1 = easting / 1852.0  / mapRange * map_size;

		    glLineWidth(lineWidth*1.5);
		    glBegin(GL_LINES);
		    glVertex2f(xPos0,yPos0);
		    glVertex2f(xPos1,yPos1);
		    glEnd();

		    glTranslatef(xPos0, yPos0, 0.0);
		    glRotatef(-1.0* heading_map, 0, 0, 1);
		    /* print waypoint name */
		    m_pFontManager->SetSize(m_Font, 0.65*fontSize, 0.65*fontSize);
		    m_pFontManager->Print(4,-1, wpt[i1].name, m_Font);

		    glPopMatrix();
		  }
		  
		  /* draw runway symbol for destination airport */
		  if ((i0 > 2) && (strncmp(wpt[i0].name,"RW",2) == 0) &&
		      (strcmp(wpt[nwpt-1].name,(char*) des_apt_name) == 0) &&
		      (*des_rwy_lon0 != 0.0) && (*des_rwy_lon1 != 0.0) &&
		      (*des_rwy_lat0 != 0.0) && (*des_rwy_lat0 != 0.0)) {

		    glPushMatrix();

		    double rwy_width = 0.15 * mapRange / 10.0;
		    double rwy_hdg_L = *des_rwy_crs - 90.0;
		    double rwy_hdg_R = *des_rwy_crs + 90.0;

		    lon = (double) *des_rwy_lon0;
		    lat = (double) *des_rwy_lat0;
		    latlon_at_dist_heading(&lon, &lat, &rwy_width, &rwy_hdg_L, &lon2, &lat2);
		    lonlat2gnomonic(&lon2, &lat2, &easting, &northing, &MapCenterLon, &MapCenterLat);
		    yPos0 = -northing / 1852.0 / mapRange * map_size; 
		    xPos0 = easting / 1852.0  / mapRange * map_size;

		    lon = (double) *des_rwy_lon1;
		    lat = (double) *des_rwy_lat1;
		    latlon_at_dist_heading(&lon, &lat, &rwy_width, &rwy_hdg_L, &lon2, &lat2);
		    lonlat2gnomonic(&lon2, &lat2, &easting, &northing, &MapCenterLon, &MapCenterLat);
		    yPos1 = -northing / 1852.0 / mapRange * map_size; 
		    xPos1 = easting / 1852.0  / mapRange * map_size;

		    glLineWidth(lineWidth*1.5);
		    glBegin(GL_LINES);
		    glVertex2f(xPos0,yPos0);
		    glVertex2f(xPos1,yPos1);
		    glEnd();
		    
		    lon = (double) *des_rwy_lon0;
		    lat = (double) *des_rwy_lat0;
		    latlon_at_dist_heading(&lon, &lat, &rwy_width, &rwy_hdg_R, &lon2, &lat2);
		    lonlat2gnomonic(&lon2, &lat2, &easting, &northing, &MapCenterLon, &MapCenterLat);
		    yPos0 = -northing / 1852.0 / mapRange * map_size; 
		    xPos0 = easting / 1852.0  / mapRange * map_size;

		    lon = (double) *des_rwy_lon1;
		    lat = (double) *des_rwy_lat1;
		    latlon_at_dist_heading(&lon, &lat, &rwy_width, &rwy_hdg_R, &lon2, &lat2);
		    lonlat2gnomonic(&lon2, &lat2, &easting, &northing, &MapCenterLon, &MapCenterLat);
		    yPos1 = -northing / 1852.0 / mapRange * map_size; 
		    xPos1 = easting / 1852.0  / mapRange * map_size;

		    glLineWidth(lineWidth*1.5);
		    glBegin(GL_LINES);
		    glVertex2f(xPos0,yPos0);
		    glVertex2f(xPos1,yPos1);
		    glEnd();

		    glTranslatef(xPos0, yPos0, 0.0);
		    glRotatef(-1.0* heading_map, 0, 0, 1);
		    /* print waypoint name */
		    m_pFontManager->SetSize(m_Font, 0.65*fontSize, 0.65*fontSize);
		    m_pFontManager->Print(4,-1, wpt[i0].name, m_Font);

		    glPopMatrix();
		  }
		
		}

	      } /* only draw if not past last missed approach wpt */

	      /* increase counter to next waypoint */
	      i = i1 + 1;
	      
	    } /* cycle through waypoints */

	    /* Draw T/C and T/D */
	    if (*tc_show == 1.0) {
	      glPushMatrix();
	      lon = (double) *tc_lon;
	      lat = (double) *tc_lat;
	      lonlat2gnomonic(&lon, &lat, &easting, &northing, &MapCenterLon, &MapCenterLat);
	      yPos = -northing / 1852.0 / mapRange * map_size; 
	      xPos = easting / 1852.0  / mapRange * map_size;
	      glTranslatef(xPos, yPos, 0.0);
	      glRotatef(-1.0* heading_map, 0, 0, 1);
	      aCircle.SetArcStartEnd(0,360);
	      aCircle.SetDegreesPerPoint(30);
	      aCircle.SetRadius(2);
	      aCircle.SetOrigin(0,0);
	      aCircle.SetDashed(0,1.0);
	      glColor3ub(COLOR_GREEN);
	      glLineWidth(lineWidth*1.5);
	      glBegin(GL_LINE_STRIP);
	      aCircle.Evaluate();
	      glEnd();
	      m_pFontManager->SetSize(m_Font, 0.65*fontSize, 0.65*fontSize);
	      m_pFontManager->Print(4,-2, "T/C", m_Font);
	      glPopMatrix();
	    }

	    if (*td_show == 1.0) {
	      glPushMatrix();
	      lon = (double) *td_lon;
	      lat = (double) *td_lat;
	      lonlat2gnomonic(&lon, &lat, &easting, &northing, &MapCenterLon, &MapCenterLat);
	      yPos = -northing / 1852.0 / mapRange * map_size; 
	      xPos = easting / 1852.0  / mapRange * map_size;
	      glTranslatef(xPos, yPos, 0.0);
	      glRotatef(-1.0* heading_map, 0, 0, 1);
	      aCircle.SetArcStartEnd(0,360);
	      aCircle.SetDegreesPerPoint(30);
	      aCircle.SetRadius(2);
	      aCircle.SetOrigin(0,0);
	      aCircle.SetDashed(0,1.0);
	      glColor3ub(COLOR_GREEN);
	      glLineWidth(lineWidth*1.5);
	      glBegin(GL_LINE_STRIP);
	      aCircle.Evaluate();
	      glEnd();
	      m_pFontManager->SetSize(m_Font, 0.65*fontSize, 0.65*fontSize);
	      m_pFontManager->Print(4,-2, "T/D", m_Font);
	      glPopMatrix();
	    }

	    
	    glPopMatrix(); // end of rotated and translated coordinate system


	    if (wpt_current < nwpt) {	    
	      /* Draw currently active waypoint at UR corner of NAV display */
	      glPushMatrix();
	      glColor3ub(COLOR_VIOLET);
	      m_pFontManager->SetSize(m_Font, 0.9*fontSize, 1.0*fontSize);
	      m_pFontManager->Print(0.82*m_PhysicalSize.x,0.95*m_PhysicalSize.y, wpt[wpt_current].name, m_Font);
	      glColor3ub(COLOR_WHITE);
	      if (fmc_eta[wpt_current] == 0.0) {
		snprintf( buffer, sizeof(buffer), "--------z");
	      } else {
		int hour=fmc_eta[wpt_current];
		float minute=(fmc_eta[wpt_current] - (float) hour)*60.0;
		snprintf( buffer, sizeof(buffer), "%02d%04.1f z", hour, minute );
	      }
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

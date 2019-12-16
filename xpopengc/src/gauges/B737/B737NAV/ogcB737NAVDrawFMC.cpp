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

    // define geometric stuff
    float fontSize = 4.0 * m_PhysicalSize.x / 150.0;
    float lineWidth = 1.5 * m_PhysicalSize.x / 100.0;

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
    double *aircraftLat = link_dataref_dbl("sim/flightmodel/position/latitude",-5);
    double *aircraftLon = link_dataref_dbl("sim/flightmodel/position/longitude",-5);
    //    double *aircraftx = link_dataref_dbl("sim/flightmodel/position/local_x",0);
    //    double *aircrafty = link_dataref_dbl("sim/flightmodel/position/local_y",0);
    //    double *aircraftz = link_dataref_dbl("sim/flightmodel/position/local_z",0);
    
    // What's the heading?
    float *heading_true = link_dataref_flt("sim/flightmodel/position/psi",-1);
     
    float *fmc_ok;
    float *fmc_lon;
    float *fmc_lat;
    int *fmc_turn;
    int *fmc_type;
    float *fmc_rad_ctr_lon;
    float *fmc_rad_ctr_lat;
    float *fmc_rad_lon2;
    float *fmc_rad_lat2;
    float *fmc_radius;
    int *fmc_miss1;
    int *fmc_miss2;
    int *fmc_cur;
    unsigned char *fmc_name;
    int *fmc_idx;
    int *fmc_nidx;
    if ((acf_type == 2) || (acf_type == 3)) {
      /* Laminar FMC for 737 */
      fmc_name = link_dataref_byte_arr("laminar/B738/fms/legs",255,-1);
      fmc_lon = link_dataref_flt_arr("laminar/B738/fms/legs_lon",128,-1,-5);
      fmc_lat = link_dataref_flt_arr("laminar/B738/fms/legs_lat",128,-1,-5);
      fmc_cur = link_dataref_int("laminar/B738/fms/legs_step_ctr"); /* link to active wpt */
      fmc_turn = link_dataref_int_arr("laminar/B738/fms/legs_turn",128,-1);
      fmc_type = link_dataref_int_arr("laminar/B738/fms/legs_type",128,-1);
      fmc_rad_ctr_lon = link_dataref_flt_arr("laminar/B738/fms/legs_radii_ctr_lon",128,-1,-5);
      fmc_rad_ctr_lat = link_dataref_flt_arr("laminar/B738/fms/legs_radii_ctr_lat",128,-1,-5);
      fmc_rad_lon2 = link_dataref_flt_arr("laminar/B738/fms/legs_radii_lon2",128,-1,-5);
      fmc_rad_lat2 = link_dataref_flt_arr("laminar/B738/fms/legs_radii_lat2",128,-1,-5);
      fmc_radius = link_dataref_flt_arr("laminar/B738/fms/legs_radii_radius",128,-1,-5);
      fmc_miss1 = link_dataref_int("laminar/B738/fms/missed_app_wpt_idx");
      fmc_miss2 = link_dataref_int("laminar/B738/fms/missed_app_wpt_idx2");
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
	    if (*fmc_cur != INT_MISS) {
	      wpt_current = *fmc_cur-1;
	    }
	    wpt_miss1 = *fmc_miss1;
	    wpt_miss2 = *fmc_miss2;
	    // printf("%s\n",fmc_name);
	    //	  printf("%f\n",fmc_lon[0]);
	    //	  printf("%f\n",fmc_lat[0]);
	    nwpt=0;
	    while (((fmc_lon[nwpt]!=0.0) || (fmc_lat[nwpt]!=0.0)) && (nwpt<128)) {
	      nwpt += 1;
	    }
	    //	  printf("%i \n",nwpt);

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
		if (pch != NULL) {
		  //		printf("%s \n",pch);
		  //		  if ((int) pch[0] != 40) {
		    /* do only name waypoints which are not in () brackets */
		    memcpy(&wpt[i].name,pch,sizeof(wpt[i].name));
		    //		  }
		    
		  pch = strtok(NULL," ");
		}
		wpt[i].lon = fmc_lon[i];
		wpt[i].lat = fmc_lat[i];
		wpt[i].rad_ctr_lon = fmc_rad_ctr_lon[i];
		wpt[i].rad_ctr_lat = fmc_rad_ctr_lat[i];
		wpt[i].rad_lon2 = fmc_rad_lon2[i];
		wpt[i].rad_lat2 = fmc_rad_lat2[i];
		//		printf("%s %i %i %f %f %f \n",wpt[i].name,fmc_type[i],fmc_turn[i],fmc_lon[i],fmc_rad_lon[i],fmc_radius[i]);
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
	    } */
	    

	  if (nwpt > 0) {
	    for (int i=wpt_current;i<=nwpt;i++) {
	  
	      // convert to azimuthal equidistant coordinates with acf in center
	      if ((wpt[max(i-1,0)].lon != FLT_MISS) && (wpt[max(i-1,0)].lat != FLT_MISS) &&
		  (wpt[i].lon != FLT_MISS) && (wpt[i].lat != FLT_MISS) &&
		  (strcmp(wpt[max(i-1,0)].name,"DISCONTINUITY") != 0)) {

		
		// convert to azimuthal equidistant coordinates with acf in center

		/*
		  if (i == wpt_current) {
		  lon = *aircraftLon;
		  lat = *aircraftLat;
		  northing = 0.0;
		  easting = 0.0;
		  } else {
		*/
		lon = (double) wpt[max(i-1,0)].lon;
		lat = (double) wpt[max(i-1,0)].lat;
		lonlat2gnomonic(&lon, &lat, &easting, &northing, aircraftLon, aircraftLat);
		/*} */

		if (i == nwpt) {
		  northing2 = northing;
		  easting2 = easting;
		} else {
		  lon = (double) wpt[i].lon;
		  lat = (double) wpt[i].lat;
		  lonlat2gnomonic(&lon, &lat, &easting2, &northing2, aircraftLon, aircraftLat);
		}
	      
		// Compute physical position relative to acf center on screen
		yPos = -northing / 1852.0 / mapRange * map_size; 
		xPos = easting / 1852.0  / mapRange * map_size;
		yPos2 = -northing2 / 1852.0 / mapRange * map_size; 
		xPos2 = easting2 / 1852.0  / mapRange * map_size;
	  
		// Only draw the waypoint if it's visible within the rendering area
		//	    if (( sqrt(xPos*xPos + yPos*yPos) < 10.0*map_size) &&
		//		( sqrt(xPos2*xPos2 + yPos2*yPos2) < 10.0*map_size)){
	    
		// calculate physical position on heading rotated nav map
		//  float rotateRad = -1.0 * *heading_true * dtor;

		glPushMatrix();

		glTranslatef(xPos, yPos, 0.0);

		glLineWidth(lineWidth);
		if ((i >= wpt_miss1) && (i <= wpt_miss2)) {
		  // missed approach route
		  glColor3ub(0, 150, 200);
		  glEnable(GL_LINE_STIPPLE);
		  glLineStipple( 4, 0x0F0F );
		} else {
		  // regular route
		  glColor3ub(255, 0, 200);
		}
		glBegin(GL_LINES);
		glVertex2f(0.0,0.0);
		glVertex2f(xPos2-xPos,yPos2-yPos);
		glEnd();
		if ((i >= wpt_miss1) && (i <= wpt_miss2)) {
		  glDisable(GL_LINE_STIPPLE);
		}
		
		glRotatef(-1.0* *heading_true, 0, 0, 1);
		glColor3ub(255, 255, 255);

		/*
		  float ss2 = 0.50*ss;
		  glColor3ub(0, 255, 255);
		  glLineWidth(1.5);
		  glBegin(GL_LINE_LOOP);
		  glVertex2f(-ss2, -1.0*ss2);
		  glVertex2f(ss2, -1.0*ss2);
		  glVertex2f(0.0, 1.0*ss2);
		  glEnd();
		*/
		float ss5 = ss;
		glLineWidth(3.0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(0.25*ss5, 0.25*ss5);
		glVertex2f(1.0*ss5,  0.0);
		glVertex2f(0.25*ss5, -0.25*ss5);
		glVertex2f(0.0, -1.0*ss5);
		glVertex2f(-0.25*ss5, -0.25*ss5);
		glVertex2f(-1.0*ss5, 0.0);
		glVertex2f(-0.25*ss5, 0.25*ss5);
		glVertex2f( 0.0, 1.0*ss5);
		glEnd();

		m_pFontManager->SetSize(m_Font, 0.65*fontSize, 0.65*fontSize);
		if (i > 0) {
		  m_pFontManager->Print(4,-4, wpt[i-1].name, m_Font);
		}	      
		glPopMatrix();

	     
		if ((wpt[max(i-1,0)].rad_ctr_lon != 0.0) && (wpt[max(i-1,0)].rad_ctr_lat != 0.0)) {
		  lon = (double) wpt[max(i-1,0)].rad_ctr_lon;
		  lat = (double) wpt[max(i-1,0)].rad_ctr_lat;
		  lonlat2gnomonic(&lon, &lat, &easting, &northing, aircraftLon, aircraftLat);
		  yPos = -northing / 1852.0 / mapRange * map_size; 
		  xPos = easting / 1852.0  / mapRange * map_size;

		  glPushMatrix();

		  glTranslatef(xPos, yPos, 0.0);

		  float ss2 = 0.50*ss;
		  glColor3ub(255, 100, 100);
		  glLineWidth(4.0);
		  glBegin(GL_LINE_LOOP);
		  glVertex2f(-ss2, -1.0*ss2);
		  glVertex2f(ss2, -1.0*ss2);
		  glVertex2f(0.0, 1.0*ss2);
		  glEnd();
		  
		  glPopMatrix();
		}
	       
		if ((wpt[max(i-1,0)].rad_lon2 != 0.0) && (wpt[max(i-1,0)].rad_lat2 != 0.0)) {
		  lon = (double) wpt[max(i-1,0)].rad_lon2;
		  lat = (double) wpt[max(i-1,0)].rad_lat2;
		  lonlat2gnomonic(&lon, &lat, &easting, &northing, aircraftLon, aircraftLat);
		  yPos = -northing / 1852.0 / mapRange * map_size; 
		  xPos = easting / 1852.0  / mapRange * map_size;

		  glPushMatrix();

		  glTranslatef(xPos, yPos, 0.0);

		  float ss2 = 0.50*ss;
		  glColor3ub(255, 255, 100);
		  glLineWidth(4.0);
		  glBegin(GL_LINE_LOOP);
		  glVertex2f(-ss2, -1.0*ss2);
		  glVertex2f(ss2, -1.0*ss2);
		  glVertex2f(0.0, 1.0*ss2);
		  glEnd();
		  
		  glPopMatrix();
		}
	       
	      }
	    }
	  } /* has waypoints */
	} /* we are on either UFMC or ZIBO FMC for 737 */
	
      } // valid acf coordinates
    
      glPopMatrix();

    }
    
  }

} // end namespace OpenGC

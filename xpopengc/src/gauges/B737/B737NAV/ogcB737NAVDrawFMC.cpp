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

    char buffer[6];

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
    float *fmc_brg;
    int *fmc_miss1;
    int *fmc_miss2;
    int *fmc_cur;
    float *fmc_rnp;
    float *fmc_anp;
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
      fmc_brg = link_dataref_flt_arr("laminar/B738/fms/legs_brg_true",128,-1,-5);
      fmc_rnp = link_dataref_flt("laminar/B738/fms/rnp",-2);
      fmc_anp = link_dataref_flt("laminar/B738/fms/anp",-2);
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
	    nwpt=0;
	    if ((*fmc_cur != INT_MISS) && (fmc_lon[0] != FLT_MISS)
		&& (fmc_lat[0] != FLT_MISS)) {
	      wpt_current = *fmc_cur-1;
	      wpt_miss1 = *fmc_miss1;
	      wpt_miss2 = *fmc_miss2;
	      // printf("%s\n",fmc_name);
	      //	  printf("%f\n",fmc_lon[0]);
	      //	  printf("%f\n",fmc_lat[0]);
	      while (((fmc_lon[nwpt]!=0.0) || (fmc_lat[nwpt]!=0.0)) && (nwpt<128)) {
		nwpt += 1;
	      }
	      // printf("%i \n",nwpt);
	      
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
		  wpt[i].brg = fmc_brg[i];
		  //		printf("%s %i %i %f %f %f \n",wpt[i].name,fmc_type[i],fmc_turn[i],fmc_lon[i],fmc_rad_lon[i],fmc_radius[i]);
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
	    } */
	    

	  if (nwpt > 0) {
	    for (int i=wpt_current;i<nwpt;i++) {

	      // printf("%i %i %f %f \n",i,nwpt,wpt[i].lon,wpt[i].lat);
	      
	      // convert to azimuthal equidistant coordinates with acf in center
	      if ((wpt[max(i-1,0)].lon != FLT_MISS) && (wpt[max(i-1,0)].lat != FLT_MISS) &&
		  (wpt[i].lon != FLT_MISS) && (wpt[i].lat != FLT_MISS) &&
		  (strcmp(wpt[max(i-1,0)].name,"DISCONTINUITY") != 0)) {

		
		// convert to azimuthal equidistant coordinates with acf in center


		lon = (double) wpt[max(i-1,0)].lon;
		lat = (double) wpt[max(i-1,0)].lat;
		lonlat2gnomonic(&lon, &lat, &easting, &northing, aircraftLon, aircraftLat);
		
		lon = (double) wpt[i].lon;
		lat = (double) wpt[i].lat;
		lonlat2gnomonic(&lon, &lat, &easting2, &northing2, aircraftLon, aircraftLat);
	      
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
		
		// holds are legs_pth "HA" or "HF" or "HM"
		// hold_radius = 1.5	-- 1.5 NM -> about 3 deg/sec at 250kts

		if ((wpt[max(i-1,0)].rad_ctr_lon != 0.0) && (wpt[max(i-1,0)].rad_ctr_lat != 0.0)) {
		  // draw curved track from waypoint i-1 to waypoint i
		  lon = (double) wpt[max(i-1,0)].rad_ctr_lon;
		  lat = (double) wpt[max(i-1,0)].rad_ctr_lat;
		  lonlat2gnomonic(&lon, &lat, &easting, &northing, aircraftLon, aircraftLat);
		  yPosC = -northing / 1852.0 / mapRange * map_size; 
		  xPosC = easting / 1852.0  / mapRange * map_size;

		  /*
		  glPushMatrix();
		  glColor3ub(255, 0, 0);
		  glPointSize(8.0);
		  glBegin(GL_POINTS);
		  glVertex2f(xPosC, yPosC);
		  glEnd();		  
		  glPopMatrix();
		  */
		  
		  if ((wpt[max(i-1,0)].rad_lon2 != 0.0) && (wpt[max(i-1,0)].rad_lat2 != 0.0)) {
		    lon = (double) wpt[max(i-1,0)].rad_lon2;
		    lat = (double) wpt[max(i-1,0)].rad_lat2;
		    lonlat2gnomonic(&lon, &lat, &easting, &northing, aircraftLon, aircraftLat);
		    yPos = -northing / 1852.0 / mapRange * map_size; 
		    xPos = easting / 1852.0  / mapRange * map_size;
		  } else {
		    // circle starts from xPos,yPos (last waypoint)
		  }

		  /*
		  glPushMatrix();		    
		  glColor3ub(0, 255, 0);
		  glPointSize(10.0);
		  glBegin(GL_POINTS);
		  glVertex2f(xPos, yPos);
		  glEnd();		    
		  glPopMatrix();
		  */
		  
		  if ((wpt[i].rad_lon2 != 0.0) && (wpt[i].rad_lat2 != 0.0)) {
		    lon = (double) wpt[i].rad_lon2;
		    lat = (double) wpt[i].rad_lat2;
		    lonlat2gnomonic(&lon, &lat, &easting, &northing, aircraftLon, aircraftLat);
		    yPos2 = -northing / 1852.0 / mapRange * map_size; 
		    xPos2 = easting / 1852.0  / mapRange * map_size;		    
		  } else {
		    // circle ends at xPos2,yPos2 (next waypoint)
		  }

		  /*
		  glPushMatrix();		    
		  glColor3ub(255, 255, 0);
		  glPointSize(10.0);
		  glBegin(GL_POINTS);
		  glVertex2f(xPos2, yPos2);
		  glEnd();		    
		  glPopMatrix();
		  */

		  /*
		  printf("%i %f %f %f %f %f %f \n",i,
			 (xPos-xPosC),(yPos-yPosC),(xPos2-xPosC),(yPos2-yPosC),
			 //pow(pow(xPos-xPosC,2) + pow(yPos-yPosC,2),0.5),
			 //pow(pow(xPos2-xPosC,2) + pow(yPos2-yPosC,2),0.5),
			 180.0/3.14*atan2((xPos-xPosC),(yPos-yPosC)),
			 180.0/3.14*atan2((xPos2-xPosC),(yPos2-yPosC))); 
		  */
		  float radius = pow(pow(xPos-xPosC,2) + pow(yPos-yPosC,2),0.5);
		  float D = pow(pow(xPos2-xPosC,2) + pow(yPos2-yPosC,2),0.5);

		  float ang = 180.0/3.14*atan2((xPos-xPosC),(yPos-yPosC));
		  float ang2;
		  float xPosT;
		  float yPosT;
		  if (D > radius) {
		    // the arc does not reach to the second point, so draw a tangent as well
		    float alpha = asin(radius/pow(pow(xPosC-xPos2,2) + pow(yPosC-yPos2,2),0.5));
		    float beta = atan2((yPosC-yPos2),(xPosC-xPos2));
		    float T = pow(D*D - radius*radius,0.5);

		    // the tangent can be put on two sides of the circle: beta +/- alpha
		    // check which circle goes into the right direction
		    // calculate relative crs: negative --> left, positive --> right
		    float relcrs = fmod(180./3.14*(wpt[i].brg-wpt[max(i-1,0)].brg) + 360.0, 360.0);
		    if (relcrs > 180.0) relcrs -= 360.0;
		    
		    float xPosT1 = cos(beta+alpha)*T + xPos2;
		    float yPosT1 = sin(beta+alpha)*T + yPos2;
		    float angT1 = 180.0/3.14*atan2((xPosT1-xPosC),(yPosT1-yPosC));		    
		    float relangT1 = fmod((angT1 - ang) + 360.0, 360.0);
		    if (relangT1 > 180.0) relangT1 -= 360.0;
		    //printf("1: %f %f %f \n",ang,angT1,relangT1);

		    /*
		    glPushMatrix();		    
		    glColor3ub(0, 100, 255);
		    glPointSize(10.0);
		    glBegin(GL_POINTS);
		    glVertex2f(xPosT1, yPosT1);
		    glEnd();		    
		    glPopMatrix();
		    */
		    
		    float xPosT2 = cos(beta-alpha)*T + xPos2;
		    float yPosT2 = sin(beta-alpha)*T + yPos2;
		    float angT2 = 180.0/3.14*atan2((xPosT2-xPosC),(yPosT2-yPosC));
		    float relangT2 = fmod((angT2 - ang) + 360.0, 360.0);
		    if (relangT2 > 180.0) relangT2 -= 360.0;
		    //printf("2: %f %f %f \n",ang, angT2, relangT2);

		    /*
		    glPushMatrix();		    
		    glColor3ub(0, 100, 255);
		    glPointSize(10.0);
		    glBegin(GL_POINTS);
		    glVertex2f(xPosT, yPosT);
		    glEnd();		    
		    glPopMatrix();
		    */
		    
		    float xPosT;
		    float yPosT;
		    if (((relcrs > 0.0) && (relangT1 > 0.0)) || ((relcrs <= 0.0) && (relangT1 <= 0.0))) {
		      ang2 = angT1;
		      xPosT = xPosT1;
		      yPosT = yPosT1;
		    } else {
		      ang2 = angT2;
		      xPosT = xPosT2;
		      yPosT = yPosT2;
		    }

		    // draw tangent
		    glBegin(GL_LINES);
		    glVertex2f(xPosT,yPosT);
		    glVertex2f(xPos2,yPos2);
		    glEnd();
		      
		  } else {
		      
		    /* with atan2(x,y) we get positive degrees CW from N to S and
		       negative degrees CCW from N to S */
		  
		    ang2 = 180.0/3.14*atan2((xPos2-xPosC),(yPos2-yPosC));

		  }

		  // draw curved route
		  CircleEvaluator aCircle;
		  aCircle.SetDegreesPerPoint(2);
		  if (ang2 > ang) {
		    if ((ang2-ang)>180.0) {
		      aCircle.SetArcStartEnd(ang2,ang+360.0);
		    } else {
		      aCircle.SetArcStartEnd(ang,ang2);
		    }
		  } else {
		    if ((ang-ang2)>180.0) {
		      aCircle.SetArcStartEnd(ang,ang2+360.0);
		    } else {
		      aCircle.SetArcStartEnd(ang2,ang);
		    }
		  }
		  //		  aCircle.SetRadius(pow(pow(xPos2-xPosC,2) + pow(yPos2-yPosC,2),0.5));
		  aCircle.SetRadius(radius);
		  aCircle.SetOrigin(xPosC,yPosC);
		  glBegin(GL_LINE_STRIP);
		  aCircle.Evaluate();
		  glEnd();

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


		// draw waypoint symbol and name
		glTranslatef(xPos2, yPos2, 0.0);
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

		m_pFontManager->SetSize(m_Font, 0.65*fontSize, 0.65*fontSize);
		m_pFontManager->Print(4,-6, wpt[i].name, m_Font);


		glPopMatrix();
	       
	      }
	    }
	  } /* has waypoints */
	} /* we are on either UFMC or ZIBO FMC for 737 */
	
      } // valid acf coordinates
    
      glPopMatrix();

    }
    
  }

} // end namespace OpenGC

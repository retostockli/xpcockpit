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
#include "B737/B737NAV/ogcB737NAVDrawDEM.h"

namespace OpenGC
{
  
  B737NAVDrawDEM::B737NAVDrawDEM()
  {
    printf("B737NAVDrawDEM constructed\n");
    
    m_Font = m_pFontManager->LoadDefaultFont();
    
    m_NAVGauge = NULL;

    m_dem_ncol = 0;
    m_dem_nlin = 0;

    dem_image = NULL;
  
  }

  B737NAVDrawDEM::~B737NAVDrawDEM()
  {
    // Destruction handled by base class
  }

 
  void B737NAVDrawDEM::Render()
  {
    GaugeComponent::Render();
   
    int acf_type = m_pDataSource->GetAcfType();
  
    bool is_captain = (this->GetArg() == 0);

    bool mapCenter = m_NAVGauge->GetMapCenter();
    int mapMode = m_NAVGauge->GetMapMode();
    float mapRange = m_NAVGauge->GetMapRange();
 
    // define geometric stuff
    float fontSize = 4.0 * m_PhysicalSize.x / 150.0;
    // float lineWidth = 3.0;

    int i;
    int j;

    unsigned char bval;

    int dem_lonmin, dem_lonmax, dem_latmin, dem_latmax;
    int dem_pixperlon, dem_pixperlat;
    int dem_ncol, dem_nlin;
    short int dem_miss = -500;

    /* Pointer to Terrain Database Object */
    TerrainData* pTerrainData = m_pNavDatabase->GetTerrainData();
    
    
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
    // What's the altitude? (feet)
    //float *pressure_altitude = link_dataref_flt("sim/flightmodel/misc/h_ind",0);
    
    int *nav_shows_dem;
    if ((acf_type == 2) || (acf_type == 3)) {
      if (is_captain) {
	nav_shows_dem = link_dataref_int("laminar/B738/EFIS/EFIS_vor_on");
      } else {
	nav_shows_dem = link_dataref_int("laminar/B738/EFIS/fo/EFIS_vor_on");
      }
    } else if (acf_type == 1) {
      nav_shows_dem = link_dataref_int("x737/cockpit/EFISCTRL_0/XXX_on");
    } else {
      nav_shows_dem = link_dataref_int("sim/cockpit2/EFIS/EFIS_vor_on");
    }
   
    // The input coordinates are in lon/lat, so we have to rotate against true heading
    // despite the NAV display is showing mag heading
    if ((heading_map != FLT_MISS) && (*nav_shows_dem == 1) &&
	(pTerrainData->dem_data_1deg) && (mapMode != 3) && (!mapCenter)) {
	        
    // Shift center and rotate about heading
      glMatrixMode(GL_MODELVIEW);

      /* valid coordinates and full radar image received */
      if ((aircraftLon >= -180.0) && (aircraftLon <= 180.0) &&
	  (aircraftLat >= -90.0) && (aircraftLat <= 90.0)) {
	

	pTerrainData->SetBounds(aircraftLon, aircraftLat);
	
	dem_lonmin = 7;
	dem_lonmax = 8;
	dem_latmin = 46;
	dem_latmax = 47;
	dem_pixperlon = 120;
	dem_pixperlat = 120;

	dem_ncol = (dem_lonmax - dem_lonmin)*dem_pixperlon;
	dem_nlin = (dem_latmax - dem_latmin)*dem_pixperlat;
	
	/* Copy DEM data into GL Texture Array */
	float textureCenterLon = (float) dem_lonmin + (float) dem_ncol / (float) dem_pixperlon * 0.5;
	float textureCenterLat = (float) dem_latmin + (float) dem_nlin / (float) dem_pixperlat * 0.5;

	/* miles per radar pixel. Each pixel .
	   Each degree lat is 111 km apart and each mile is 1.852 km */
	float mpplon =  111.0 / (float) dem_pixperlon / 1.852;
	float mpplat =  111.0 / (float) dem_pixperlat / 1.852;

	
	/* free DEM array and recreate it if dimensions have changed */
	if ((m_dem_ncol != dem_ncol) || (m_dem_nlin != dem_nlin)) {

	  pTerrainData->ReadDEMLonLat(dem_lonmin,dem_latmin);
	  
	  m_dem_ncol = dem_ncol;
	  m_dem_nlin = dem_nlin;
	  if (dem_image) free(dem_image);	    
	  dem_image = (unsigned char*)malloc(m_dem_nlin * m_dem_ncol * 4 * sizeof(unsigned char));
	}

	
	/* copy temporary DEM array to DEM array */
	for (j = 0; j < m_dem_nlin; j++) {
	  for (i = 0; i < m_dem_ncol; i++) {

	    if (pTerrainData->dem_data_1deg[j][i] == dem_miss) {
	      bval = 0;
	    } else {
	      bval = min(max(pTerrainData->dem_data_1deg[j][i]/15,1),255);
	    }
			     
	    dem_image[j*4*m_dem_ncol+i*4+0] = pTerrainData->dem_colortable[bval][0];
	    dem_image[j*4*m_dem_ncol+i*4+1] = pTerrainData->dem_colortable[bval][1];
	    dem_image[j*4*m_dem_ncol+i*4+2] = pTerrainData->dem_colortable[bval][2];	    
	    dem_image[j*4*m_dem_ncol+i*4+3] = 255; /* Non-Transparent */
	  }
	}
	
	glPushMatrix();
	
	glTranslatef(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y, 0.0);
	glRotatef(heading_map, 0, 0, 1);


	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Remove border line */
	GLfloat color[4]={0,0,0,1};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
	
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA,
		      m_dem_ncol,  m_dem_nlin, 0, GL_RGBA,
		      GL_UNSIGNED_BYTE, dem_image);

	float scx = 0.5 * ((float) m_dem_ncol) * mpplon / mapRange * map_size * cos(M_PI / 180.0 * aircraftLat);
	float scy = 0.5 * ((float) m_dem_nlin) * mpplat / mapRange * map_size;
	float tx = (textureCenterLon - aircraftLon) * ((float) dem_pixperlon) * mpplon / mapRange * map_size *
	  cos(M_PI / 180.0 * aircraftLat);
	float ty = (textureCenterLat - aircraftLat) * ((float) dem_pixperlat) * mpplat / mapRange * map_size;
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glBegin(GL_TRIANGLES);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-scx+tx,  scy+ty);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-scx+tx, -scy+ty);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( scx+tx,  scy+ty);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( scx+tx,  scy+ty);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-scx+tx, -scy+ty);
        glTexCoord2f(1.0f, 0.0f); glVertex2f( scx+tx, -scy+ty);
	glEnd();

	glDisable (GL_TEXTURE_2D);
	glFlush();

	/* end of down-shifted and rotated coordinate system */
	glPopMatrix();

	/* Delete radar image behind aircraft and beyond range of 60 NM*/
	glPushMatrix();

	glColor3ub(COLOR_BLACK);
	glBegin(GL_POLYGON);
	glVertex2f(0,0);
	glVertex2f(0,m_PhysicalSize.y*acf_y);
	glVertex2f(m_PhysicalSize.x,m_PhysicalSize.y*acf_y);
	glVertex2f(m_PhysicalSize.x,0);
	glEnd();
	
	glBegin(GL_POLYGON);
	glVertex2f(0,m_PhysicalSize.y);
	glVertex2f(0,m_PhysicalSize.y*map_y_max);
	glVertex2f(m_PhysicalSize.x,m_PhysicalSize.y*map_y_max);
	glVertex2f(m_PhysicalSize.x,m_PhysicalSize.y);
	glEnd();
	
	//-------------------Rounded NAV Gauge DEM Limit ------------------
	// The DEM image is blacked off on the top of the NAV gauge
	// The overlays are essentially the
	// remainder of a circle subtracted from a square, and are formed
	// by fanning out triangles from a point just off each corner
	// to an arc descrbing the curved portion of the art. horiz.
	CircleEvaluator aCircle;
	aCircle.SetRadius(m_PhysicalSize.y*(map_y_max-acf_y));
	
	// Upper Left quarter
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0.0,m_PhysicalSize.y*map_y_max);
	aCircle.SetOrigin(m_PhysicalSize.x*acf_x,m_PhysicalSize.y*acf_y);
	aCircle.SetArcStartEnd(270,360);
	aCircle.SetDegreesPerPoint(10);
	aCircle.Evaluate();
	glEnd();

	// Upper Right quarter
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(m_PhysicalSize.x,m_PhysicalSize.y*map_y_max);
	aCircle.SetOrigin(m_PhysicalSize.x*acf_x,m_PhysicalSize.y*acf_y);
	aCircle.SetArcStartEnd(0,90);
	aCircle.SetDegreesPerPoint(10);
	aCircle.Evaluate();
	glEnd();
       
	glPopMatrix();
 	
      } // valid acf coordinates

    }
    
    if ((*nav_shows_dem == 1) && (mapMode != 3) && (!mapCenter)) {   
      // plot map options
      glPushMatrix();
      m_pFontManager->SetSize( m_Font, 0.75*fontSize, 0.75*fontSize );
      glColor3ub(COLOR_LIGHTBLUE);
      m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.268 ,"DEM",m_Font);
      glPopMatrix();
    }
    
  }

} // end namespace OpenGC

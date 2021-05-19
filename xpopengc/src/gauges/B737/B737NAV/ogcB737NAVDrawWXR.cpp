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
#include "B737/B737NAV/ogcB737NAVDrawWXR.h"
extern "C" {
#include "udpdata.h"
#include "handleudp.h"
}

namespace OpenGC
{
  
  B737NAVDrawWXR::B737NAVDrawWXR()
  {
    printf("B737NAVDrawWXR constructed\n");
    
    m_Font = m_pFontManager->LoadDefaultFont();
    
    m_NAVGauge = NULL;

    for (int i=0;i<TEX_HEIGHT;i++) {
      for (int j=0;j<TEX_WIDTH;j++) {
     
	texture[i][j][0]=(GLubyte) 0;
	texture[i][j][1]=(GLubyte) 0;
	texture[i][j][2]=(GLubyte) 0;
	texture[i][j][3]=(GLubyte) 255;

	/*
	texture[i][j][0]=1.0;
	texture[i][j][1]=0.2;
	texture[i][j][2]=0.2;
	texture[i][j][3]=1.0;
	*/
      }
    }


    wxrnlon = 5;
    wxrnlat = 3;
    
  }

  B737NAVDrawWXR::~B737NAVDrawWXR()
  {
    // Destruction handled by base class
  }

  void B737NAVDrawWXR::Render()
  {
    GaugeComponent::Render();

    char *wxrBuffer;
    int wxrBufferLen;
    
    int acf_type = m_pDataSource->GetAcfType();
  
    bool is_captain = (this->GetArg() == 0);

    bool mapCenter = m_NAVGauge->GetMapCenter();
    int mapMode = m_NAVGauge->GetMapMode();
    float mapRange = m_NAVGauge->GetMapRange();
 
    // define geometric stuff
    float fontSize = 4.0 * m_PhysicalSize.x / 150.0;
    // float lineWidth = 3.0;

    
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
    
    int *nav_shows_wxr;
    if ((acf_type == 2) || (acf_type == 3)) {
      if (is_captain) {
	nav_shows_wxr = link_dataref_int("laminar/B738/EFIS/EFIS_wx_on");
      } else {
	nav_shows_wxr = link_dataref_int("laminar/B738/EFIS/fo/EFIS_wx_on");
      }
    } else if (acf_type == 1) {
      nav_shows_wxr = link_dataref_int("x737/cockpit/EFISCTRL_0/WXR_on");
    } else {
      nav_shows_wxr = link_dataref_int("sim/cockpit2/EFIS/EFIS_weather_on");
    }

    // The input coordinates are in lon/lat, so we have to rotate against true heading
    // despite the NAV display is showing mag heading
    if ((heading_map != FLT_MISS) && (*nav_shows_wxr == 1) &&
	(udpRecvBuffer != NULL) && (mapMode != 3)) {

      // Shift center and rotate about heading
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      glTranslatef(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y, 0.0);
      glRotatef(heading_map, 0, 0, 1);

      /* valid coordinates ? */
      if ((aircraftLon >= -180.0) && (aircraftLon <= 180.0) && (aircraftLat >= -90.0) && (aircraftLat <= 90.0)) {

	double northing;
	double easting;
	double dlon;
	double dlat;
	int x;
	int y;

	if (m_pDataSource->GetMaxRadar() > 0) {
	  wxrBufferLen = 5+13*m_pDataSource->GetMaxRadar();
	} else {
	  wxrBufferLen = 81;
	}
	wxrBuffer=(char*) malloc(wxrBufferLen);

	while (udpReadLeft > 0) {

	  pthread_mutex_lock(&exit_cond_lock);    
	  /* read from start of receive buffer */
	  memcpy(wxrBuffer,&udpRecvBuffer[0],wxrBufferLen);
	  /* shift remaining read buffer to the left */
	  memmove(&udpRecvBuffer[0],&udpRecvBuffer[wxrBufferLen],udpReadLeft-wxrBufferLen);    
	  /* decrease read buffer position and counter */
	  udpReadLeft -= wxrBufferLen;
	  pthread_mutex_unlock(&exit_cond_lock);
	
	/* check if we have new Radar data */
	if (strncmp(wxrBuffer,"RADR5",5)==0) {
	  
	  int i;
	  int nrad = (wxrBufferLen-5)/13;
	  
	  float lon;
	  float lat;
	  float hgt;
	  char lev;
	  
	  for (i=0;i<nrad;i++) {
	    memcpy(&lon,&wxrBuffer[5+i*13+0],sizeof(lon));
	    memcpy(&lat,&wxrBuffer[5+i*13+4],sizeof(lat));
	    memcpy(&lev,&wxrBuffer[5+i*13+8],sizeof(lev));
	    memcpy(&hgt,&wxrBuffer[5+i*13+9],sizeof(hgt));

	    dlon = (double) lon;
	    dlat = (double) lat;
	    
	    // convert to azimuthal equidistant coordinates with acf in center
	    lonlat2gnomonic(&dlon, &dlat, &easting, &northing, &aircraftLon, &aircraftLat);
	    
	    printf("%i lon %f lat %f hgt %f lev %d \n",i,lon,lat,hgt,lev);
	    
	    // Compute physical position relative to acf center on screen in texture coordinates
	    y = (int) (-northing / 1852.0 / MPP * 0.5 + 0.5 * (float) TEX_HEIGHT); 
	    x = (int) (easting / 1852.0 / MPP * 0.5 + 0.5 * (float) TEX_WIDTH);
	    
	    if ((x >= 0) && (x < TEX_WIDTH) && (y >= 0) && (y < TEX_HEIGHT)) {
	      
	      // fill texture map
	      if (lev<=45) {
		texture[y][x][0] = (GLubyte) 0;
		texture[y][x][1] = (GLubyte) 0;
		texture[y][x][2] = (GLubyte) 0;
	      } else if (lev<60) {
		texture[y][x][0] = (GLubyte) 0;
		texture[y][x][1] = (GLubyte) 200;
		texture[y][x][2] = (GLubyte) 0;
	      } else if (lev<75) {
		texture[y][x][0] = (GLubyte) 0;
		texture[y][x][1] = (GLubyte) 128;
		texture[y][x][2] = (GLubyte) 0;
	      } else if (lev<95) {
		texture[y][x][0] = (GLubyte) 255;
		texture[y][x][1] = (GLubyte) 255;
		texture[y][x][2] = (GLubyte) 100;
	      } else {
		texture[y][x][0] = (GLubyte) 255;
		texture[y][x][1] = (GLubyte) 50;
		texture[y][x][2] = (GLubyte) 50;
	      }
	      
	    }
	    
	  }
	  
	}
	
	/* check if we have new Radar data from CONTROL PAD Stream (Enable CONTROL PAD in the Net Ouput Screen) */
	if (strncmp(wxrBuffer,"xRAD",4)==0) {
	  
	  int i;
	  
	  int lon_deg_west;
	  int lat_deg_south;
	  int lat_min_south;
	  char rad;

	  memcpy(&lon_deg_west,&wxrBuffer[5],sizeof(lon_deg_west));
	  memcpy(&lat_deg_south,&wxrBuffer[5+4],sizeof(lat_deg_south));
	  memcpy(&lat_min_south,&wxrBuffer[5+4+4],sizeof(lat_min_south));
	  
	  printf("%.*s %i %i %i \n",4,wxrBuffer,lon_deg_west,lat_deg_south,lat_min_south);
	  
	  for (i=0;i<TEX_WIDTH;i++) {
	  }
	  

	}

	} /* read left ? */

	free(wxrBuffer);

	// render the Radar texture
	//	glPushMatrix();
	// GLuint texture_map;
	// glGenTextures(1, &texture_map);
	// glBindTexture(GL_TEXTURE_2D, texture_map);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA,
		      TEX_WIDTH,  TEX_HEIGHT, 0, GL_RGBA,
		      GL_UNSIGNED_BYTE, texture);
	/*
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA,
		      TEX_WIDTH,  TEX_HEIGHT, 0, GL_RGBA,
		      GL_FLOAT, texture);
	*/

	float scx = 0.5 * (float) TEX_WIDTH * MPP / mapRange * map_size;
	float scy = 0.5 * (float) TEX_HEIGHT * MPP / mapRange * map_size;

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glBindTexture(GL_TEXTURE_2D, texture_map);	  

	glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-scx, -scy);
        glTexCoord2f(1.0f, 0.0f); glVertex2f( scx, -scy);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( scx,  scy);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-scx,  scy);
	glEnd();

	glDisable (GL_TEXTURE_2D);
	glFlush();
//	glPopMatrix();
	
      } // valid acf coordinates

      /* end of down-shifted and rotated coordinate system */
      glPopMatrix();
    
      // plot map options
      glPushMatrix();
      m_pFontManager->SetSize( m_Font, 0.75*fontSize, 0.75*fontSize );
      glColor3ub(COLOR_LIGHTBLUE);
      m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.268 ,"WXR",m_Font);
      glPopMatrix();

    }
    
  }

} // end namespace OpenGC

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

	lltexture[i][j] = (GLubyte) 0;
      }
    }

    m_OldLat = INT_MISS;
    m_CenterLon = INT_MISS;
    m_CenterLat = INT_MISS;
    m_TextureCenterLon = FLT_MISS;
    m_TextureCenterLat = FLT_MISS;
    
  }

  B737NAVDrawWXR::~B737NAVDrawWXR()
  {
    // Destruction handled by base class
  }

  void B737NAVDrawWXR::Render()
  {
    GaugeComponent::Render();

    char *wxrBuffer;
    int wxrBufferLen = 81;
    wxrBuffer=(char*) malloc(wxrBufferLen);
    
    int acf_type = m_pDataSource->GetAcfType();
  
    bool is_captain = (this->GetArg() == 0);

    bool mapCenter = m_NAVGauge->GetMapCenter();
    int mapMode = m_NAVGauge->GetMapMode();
    float mapRange = m_NAVGauge->GetMapRange();
 
    // define geometric stuff
    float fontSize = 4.0 * m_PhysicalSize.x / 150.0;
    // float lineWidth = 3.0;

    double northing;
    double easting;
    double dlon;
    double dlat;
    int x;
    int y;
    
    
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

    // read buffer and fill it into regular lon/lat array with 60 minutes per lon/lat
    //printf("%i \n ",udpReadLeft);
	
    while (udpReadLeft > 0) {
      
      pthread_mutex_lock(&exit_cond_lock);    
      /* read from start of receive buffer */
      memcpy(wxrBuffer,&udpRecvBuffer[0],wxrBufferLen);
      /* shift remaining read buffer to the left */
      memmove(&udpRecvBuffer[0],&udpRecvBuffer[wxrBufferLen],udpReadLeft-wxrBufferLen);    
      /* decrease read buffer position and counter */
      udpReadLeft -= wxrBufferLen;
      pthread_mutex_unlock(&exit_cond_lock);
	
      /* check if we have new Radar data from CONTROL PAD Stream (Enable CONTROL PAD in the Net Ouput Screen) */
      if (strncmp(wxrBuffer,"xRAD",4)==0) {
	
	int i,j;
	
	int lon_deg_west;
	int lat_deg_south;
	int lat_min_south;

	/* valid coordinates ? */
	if ((aircraftLon >= -180.0) && (aircraftLon <= 180.0) && (aircraftLat >= -90.0) && (aircraftLat <= 90.0)) {


	  if (m_CenterLon == INT_MISS) m_CenterLon = (int) aircraftLon;
	  if (m_CenterLat == INT_MISS) m_CenterLat = (int) aircraftLat;
	
	  memcpy(&lon_deg_west,&wxrBuffer[5],sizeof(lon_deg_west));
	  memcpy(&lat_deg_south,&wxrBuffer[5+4],sizeof(lat_deg_south));
	  memcpy(&lat_min_south,&wxrBuffer[5+4+4],sizeof(lat_min_south));
	  
	  if ((lon_deg_west >= (m_CenterLon-(NLON-1)/2)) && (lon_deg_west <= (m_CenterLon+(NLON-1)/2)) &&
	      (lat_deg_south >= (m_CenterLat-(NLAT-1)/2)) && (lat_deg_south <= (m_CenterLat+(NLAT-1)/2)) &&
	      (lat_min_south != -1)) {

	    if ((m_OldLat > lat_deg_south) && (m_OldLat != INT_MISS)) {
	      /* scanning starts over from southern latitude, so put full scanned radar texture to GL */

	      printf("Storing Radar Image \n");
	      
	      for (x=0;x<TEX_WIDTH;x++) {
		for (y=0;y<TEX_HEIGHT;y++) {
		  if (lltexture[y][x] == 0) {
		    texture[y][x][0] = (GLubyte) 0;
		    texture[y][x][1] = (GLubyte) 0;
		    texture[y][x][2] = (GLubyte) 0;
		  } else if (lltexture[y][x] == 1) {
		    texture[y][x][0] = (GLubyte) 0;
		    texture[y][x][1] = (GLubyte) 0;
		    texture[y][x][2] = (GLubyte) 0;
		  } else if (lltexture[y][x] == 2) {
		    texture[y][x][0] = (GLubyte) 0;
		    texture[y][x][1] = (GLubyte) 0;
		    texture[y][x][2] = (GLubyte) 0;
		  } else if (lltexture[y][x] == 3) {
		    texture[y][x][0] = (GLubyte) 0;
		    texture[y][x][1] = (GLubyte) 0;
		    texture[y][x][2] = (GLubyte) 0;
		  } else if (lltexture[y][x] == 4) {
		    texture[y][x][0] = (GLubyte) 0;
		    texture[y][x][1] = (GLubyte) 100;
		    texture[y][x][2] = (GLubyte) 0;
		  } else if (lltexture[y][x] == 5) {
		    texture[y][x][0] = (GLubyte) 0;
		    texture[y][x][1] = (GLubyte) 140;
		    texture[y][x][2] = (GLubyte) 0;
		  } else if (lltexture[y][x] == 6) {
		    texture[y][x][0] = (GLubyte) 100;
		    texture[y][x][1] = (GLubyte) 140;
		    texture[y][x][2] = (GLubyte) 0;
		  } else if (lltexture[y][x] == 7) {
		    texture[y][x][0] = (GLubyte) 140;
		    texture[y][x][1] = (GLubyte) 140;
		    texture[y][x][2] = (GLubyte) 0;
		  } else if (lltexture[y][x] == 8) {
		    texture[y][x][0] = (GLubyte) 150;
		    texture[y][x][1] = (GLubyte) 100;
		    texture[y][x][2] = (GLubyte) 0;
		  } else {
		    texture[y][x][0] = (GLubyte) 150;
		    texture[y][x][1] = (GLubyte) 0;
		    texture[y][x][2] = (GLubyte) 0;
		  }
		}
	      }   

	      /* Store Current Center Lon/Lat as new Texture Center coordinates */
	      /* Need to add 0.5 lon/lat since radar data specifies ll edge of center pixel */
	      m_TextureCenterLon = ((float) m_CenterLon) + 0.5;
	      m_TextureCenterLat = ((float) m_CenterLat) + 0.5;
	      
	      /* Update Center Longitude/Latitude of texture to new Lon/Lat of acf */
	      m_CenterLon = (int) aircraftLon;
	      m_CenterLat = (int) aircraftLat;
	      
	    }

	    m_OldLat = lat_deg_south;
	    
	    i = (lon_deg_west - (m_CenterLon-(NLON-1)/2))*MINPERLON;
	    j = (lat_deg_south - (m_CenterLat-(NLAT-1)/2))*MINPERLAT + lat_min_south;
	  
	    //printf("%i %i %i %i %i %i %i \n",m_CenterLon,m_CenterLat,lon_deg_west,lat_deg_south,lat_min_south,i,j);

	    /* Store 61 bytes of 61 minutes west-east to current degree line */
	    memcpy(&lltexture[j][i],&wxrBuffer[5+4+4+4],MINPERLON+1);

	  }

	}
	
      }
      
    } /* read left ? */
    
    free(wxrBuffer);     

    // The input coordinates are in lon/lat, so we have to rotate against true heading
    // despite the NAV display is showing mag heading
    if ((heading_map != FLT_MISS) && (*nav_shows_wxr == 1) &&
	(udpRecvBuffer != NULL) && (mapMode != 3) && (!mapCenter)) {

	    
      // Shift center and rotate about heading
      glMatrixMode(GL_MODELVIEW);

      /* valid coordinates and full radar image received */
      if ((aircraftLon >= -180.0) && (aircraftLon <= 180.0) && (aircraftLat >= -90.0) && (aircraftLat <= 90.0) &&
	  (m_TextureCenterLon != FLT_MISS) && (m_TextureCenterLat != FLT_MISS)) {
      //	if ((aircraftLon >= -180.0) && (aircraftLon <= 180.0) && (aircraftLat >= -90.0) && (aircraftLat <= 90.0)) {

	glPushMatrix();
	
	glTranslatef(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y, 0.0);
	glRotatef(heading_map, 0, 0, 1);


	// render the Radar texture
	//	glPushMatrix();
	// GLuint texture_map;
	// glGenTextures(1, &texture_map);
	// glBindTexture(GL_TEXTURE_2D, texture_map);

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
		      TEX_WIDTH,  TEX_HEIGHT, 0, GL_RGBA,
		      GL_UNSIGNED_BYTE, texture);

	float scx = 0.5 * ((float) TEX_WIDTH) * MPP / mapRange * map_size * cos(M_PI / 180.0 * aircraftLat);
	float scy = 0.5 * ((float) TEX_HEIGHT) * MPP / mapRange * map_size;
	float tx = (m_TextureCenterLon - aircraftLon) * ((float) MINPERLON) * MPP / mapRange * map_size *
	  cos(M_PI / 180.0 * aircraftLat);
	float ty = (m_TextureCenterLat - aircraftLat) * ((float) MINPERLAT) * MPP / mapRange * map_size;
	
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glBindTexture(GL_TEXTURE_2D, texture_map);	  

	glBegin(GL_TRIANGLES);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-scx+tx,  scy+ty);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-scx+tx, -scy+ty);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( scx+tx,  scy+ty);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( scx+tx,  scy+ty);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-scx+tx, -scy+ty);
        glTexCoord2f(1.0f, 0.0f); glVertex2f( scx+tx, -scy+ty);
	glEnd();

	/*
	// DEPRECATED Do not use any more
	glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-scx+tx, -scy+ty);
        glTexCoord2f(1.0f, 0.0f); glVertex2f( scx+tx, -scy+ty);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( scx+tx,  scy+ty);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-scx+tx,  scy+ty);
	glEnd();
	*/

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
	
	//-------------------Rounded NAV Gauge WXR Limit ------------------
	// The WXR image is blacked off on the top of the NAV gauge
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
   
      // plot map options
      glPushMatrix();
      m_pFontManager->SetSize( m_Font, 0.75*fontSize, 0.75*fontSize );
      glColor3ub(COLOR_LIGHTBLUE);
      m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.268 ,"WXR",m_Font);
      glPopMatrix();

    }
    
  }

} // end namespace OpenGC

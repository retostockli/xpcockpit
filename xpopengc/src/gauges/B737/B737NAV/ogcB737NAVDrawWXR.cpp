/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Copyright (c) 2001-2024 Damion Shelton and Reto Stockli
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
#include "wxrdata.h"
}

namespace OpenGC
{
  
  B737NAVDrawWXR::B737NAVDrawWXR()
  {
    printf("B737NAVDrawWXR constructed\n");
    
    m_Font = m_pFontManager->LoadDefaultFont();
    
    m_NAVGauge = NULL;

    m_wxr_ncol = 0;
    m_wxr_nlin = 0;

    wxr_image = NULL;

    m_texture = 0;

    for (int n=0;n<NUM_HIST;n++) {
      m_wxr_gain[n] = 0.0;
      m_wxr_tilt[n] = 0.0;
    }
  
  }

  B737NAVDrawWXR::~B737NAVDrawWXR()
  {
    // Destruction handled by base class
  }

 
  void B737NAVDrawWXR::Render()
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
    int n;
    
    
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
    float *pressure_altitude = link_dataref_flt("sim/flightmodel/misc/h_ind",0);
    
    int *nav_shows_wxr;

    /*
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
    */

    if (is_captain) {
      nav_shows_wxr = link_dataref_int("xpserver/EFIS_capt_wxr");
    } else {
      nav_shows_wxr = link_dataref_int("xpserver/EFIS_fo_wxr");
    }

    //*nav_shows_wxr = 1;
    
    /* Sample Datarefs for controlling WXR gain and tilt */
    float *wxr_gain = link_dataref_flt("xpserver/wxr_gain",-1); /* Gain should go from 0.1 .. 2.0 */
    float *wxr_tilt = link_dataref_flt("xpserver/wxr_tilt",-1); /* Tilt in degrees up/down : not implemented yet */

    // The input coordinates are in lon/lat, so we have to rotate against true heading
    // despite the NAV display is showing mag heading
    if ((heading_map != FLT_MISS) && (*nav_shows_wxr == 1) &&
	(wxr_data) && (mapMode != 3) && (!mapCenter)) {
	        
    // Shift center and rotate about heading
      glMatrixMode(GL_MODELVIEW);

      /* valid coordinates and full radar image received */
      if ((aircraftLon >= -180.0) && (aircraftLon <= 180.0) &&
	  (aircraftLat >= -90.0) && (aircraftLat <= 90.0)) {
	
	/* Copy WXR data into GL Texture Array */
	float textureCenterLon = (float) wxr_lonmin + (float) wxr_ncol / (float) wxr_pixperlon * 0.5;
	float textureCenterLat = (float) wxr_latmin + (float) wxr_nlin / (float) wxr_pixperlat * 0.5;

	/* miles per radar pixel. Each pixel .
	   Each degree lat is 111 km apart and each mile is 1.852 km */
	float mpplon =  111.0 / (float) wxr_pixperlon / 1.852;
	float mpplat =  111.0 / (float) wxr_pixperlat / 1.852;

	/* free WXR array and recreate it if we have new WXR data */
	float mean_wxr_gain = Mean(NUM_HIST,m_wxr_gain);
	//printf("%f %f \n",*wxr_gain,mean_wxr_gain);
	if ((wxr_newdata == 1) || (*wxr_gain <= (mean_wxr_gain - 0.03)) || (*wxr_gain >= (mean_wxr_gain + 0.03))) {
	  printf("Plotting New WXR Data in NAV Display\n");
	  m_wxr_ncol = wxr_ncol;
	  m_wxr_nlin = wxr_nlin;
	  if (wxr_image) free(wxr_image);	    
	  wxr_image = (unsigned char*)malloc(m_wxr_nlin * m_wxr_ncol * 4 * sizeof(unsigned char));

	  float gain = *wxr_gain;
	  if (gain == FLT_MISS) gain = 1.0;
	  if (gain < 0.05) gain = 0.05;
	  if (gain > 2.0) gain = 2.0;
	  
	  /* copy temporary WXR array to WXR array */
	  /* TODO: OPENGL Transparency not working */
	  /* TODO: Only create image if data has changed */
	  for (i = 0; i < m_wxr_nlin; i++) {
	    for (j = 0; j < m_wxr_ncol; j++) {
	      if (wxr_data[i][j]*gain <= 10) {
		wxr_image[i*4*m_wxr_ncol+j*4+0] = 0;
		wxr_image[i*4*m_wxr_ncol+j*4+1] = 0;
		wxr_image[i*4*m_wxr_ncol+j*4+2] = 0;
		wxr_image[i*4*m_wxr_ncol+j*4+3] = 0; /* Transparent */
	      } else if (wxr_data[i][j]*gain <= 30) {
		wxr_image[i*4*m_wxr_ncol+j*4+0] = 0;
		wxr_image[i*4*m_wxr_ncol+j*4+1] = 255;
		wxr_image[i*4*m_wxr_ncol+j*4+2] = 0;
		wxr_image[i*4*m_wxr_ncol+j*4+3] = 255; /* Non-Transparent */
	      } else if (wxr_data[i][j]*gain <= 50) {
		wxr_image[i*4*m_wxr_ncol+j*4+0] = 250;
		wxr_image[i*4*m_wxr_ncol+j*4+1] = 250;
		wxr_image[i*4*m_wxr_ncol+j*4+2] = 0;
		wxr_image[i*4*m_wxr_ncol+j*4+3] = 255; /* Non-Transparent */
	      } else if (wxr_data[i][j]*gain <= 70) {
		wxr_image[i*4*m_wxr_ncol+j*4+0] = 250;
		wxr_image[i*4*m_wxr_ncol+j*4+1] = 150;
		wxr_image[i*4*m_wxr_ncol+j*4+2] = 0;
		wxr_image[i*4*m_wxr_ncol+j*4+3] = 255; /* Non-Transparent */
	      } else if (wxr_data[i][j]*gain <= 90) {
		wxr_image[i*4*m_wxr_ncol+j*4+0] = 250;
		wxr_image[i*4*m_wxr_ncol+j*4+1] = 0;
		wxr_image[i*4*m_wxr_ncol+j*4+2] = 0;
		wxr_image[i*4*m_wxr_ncol+j*4+3] = 255; /* Non-Transparent */
	      } else {
		/* Magenta for Turbulence, not implemented in X-Plane, but take the highest level */
		wxr_image[i*4*m_wxr_ncol+j*4+0] = 200;
		wxr_image[i*4*m_wxr_ncol+j*4+1] = 45;
		wxr_image[i*4*m_wxr_ncol+j*4+2] = 200;
		wxr_image[i*4*m_wxr_ncol+j*4+3] = 255; /* Non-Transparent */
	      }
	    }
	  }
	  wxr_newdata = 0;

	  /* Check if Texture already exist and free memory */
	  if (glIsTexture(m_texture)) glDeleteTextures(1, &m_texture);

	  glGenTextures(1, &m_texture);
	  glBindTexture(GL_TEXTURE_2D, m_texture);
	  
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
			m_wxr_ncol,  m_wxr_nlin, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, wxr_image);

	  glBindTexture(GL_TEXTURE_2D, 0);
	  
	}
	
	glPushMatrix();
	
	glTranslatef(m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y, 0.0);
	glRotatef(heading_map, 0, 0, 1);

	float scx = 0.5 * ((float) m_wxr_ncol) * mpplon / mapRange * map_size * cos(M_PI / 180.0 * aircraftLat);
	float scy = 0.5 * ((float) m_wxr_nlin) * mpplat / mapRange * map_size;
	float tx = (textureCenterLon - aircraftLon) * ((float) wxr_pixperlon) * mpplon / mapRange * map_size *
	  cos(M_PI / 180.0 * aircraftLat);
	float ty = (textureCenterLat - aircraftLat) * ((float) wxr_pixperlat) * mpplat / mapRange * map_size;
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glColor3ub(COLOR_WHITE);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_TRIANGLES);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-scx+tx,  scy+ty);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-scx+tx, -scy+ty);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( scx+tx,  scy+ty);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( scx+tx,  scy+ty);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-scx+tx, -scy+ty);
        glTexCoord2f(1.0f, 0.0f); glVertex2f( scx+tx, -scy+ty);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable (GL_TEXTURE_2D);
	//glFlush();

	/* end of down-shifted and rotated coordinate system */
	glPopMatrix();

	/* Delete radar image behind aircraft and beyond range of 60 NM*/
	glPushMatrix();

	glColor3ub(COLOR_BLACK);
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glVertex2f(0,0);
	glVertex2f(0,m_PhysicalSize.y*acf_y);
	glVertex2f(m_PhysicalSize.x,m_PhysicalSize.y*acf_y);
	glVertex2f(m_PhysicalSize.x,0);
	glEnd();
	
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
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

	/* Update History Values of WXR gain and tilt */
	if ((*wxr_gain != FLT_MISS) && (*wxr_tilt != FLT_MISS)) {
	  for (n=0;n<(NUM_HIST-1);n++) {
	    m_wxr_gain[n+1] = m_wxr_gain[n];
	    m_wxr_tilt[n+1] = m_wxr_tilt[n];
	    m_wxr_gain[0] = *wxr_gain;
	    m_wxr_tilt[0] = *wxr_tilt;
	  }
	}
   	
      } // valid acf coordinates

    }
    
    if ((*nav_shows_wxr == 1) && (mapMode != 3) && (!mapCenter)) {   
      // plot map options
      glPushMatrix();
      glColor3ub(COLOR_BLACK);
      glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
      glBegin(GL_POLYGON);
      glVertex2f(0,m_PhysicalSize.y*0.260);
      glVertex2f(0,m_PhysicalSize.y*0.300);
      glVertex2f(m_PhysicalSize.x*0.100,m_PhysicalSize.y*0.300);
      glVertex2f(m_PhysicalSize.x*0.100,m_PhysicalSize.y*0.260);
      glEnd();
      m_pFontManager->SetSize( m_Font, 0.75*fontSize, 0.75*fontSize );
      glColor3ub(COLOR_LIGHTBLUE);
      m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.268 ,"WX-A",m_Font);
      glPopMatrix();
    }
    
  }

} // end namespace OpenGC

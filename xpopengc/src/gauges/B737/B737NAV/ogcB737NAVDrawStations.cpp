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
#include "B737/B737NAV/ogcB737NAVDrawStations.h"

namespace OpenGC
{

  B737NAVDrawStations::B737NAVDrawStations()
  {
    printf("B737NAVDrawStations constructed\n");
  
    m_Font = m_pFontManager->LoadDefaultFont();

    m_NAVGauge = NULL;
    
  }

  B737NAVDrawStations::~B737NAVDrawStations()
  {
    // Destruction handled by base class
  }

  void B737NAVDrawStations::Render()
  {
    GaugeComponent::Render();

    int acf_type = m_pDataSource->GetAcfType();
  
    bool is_captain = (this->GetArg() == 0);

    bool mapCenter = m_NAVGauge->GetMapCenter();
    int mapMode = m_NAVGauge->GetMapMode();
    float mapRange = m_NAVGauge->GetMapRange();
 
    // define geometric stuff
    float fontSize = 4.0 * m_PhysicalSize.x / 150.0;
    float lineWidth = 3.0;

    int i;
    
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

    // Get information on what dynamic information we display on NAV MAP
 
    // Where is the aircraft?
    double aircraftLon = m_NAVGauge->GetMapCtrLon();
    double aircraftLat = m_NAVGauge->GetMapCtrLat();
   
    // What's the heading?
    float heading_map =  m_NAVGauge->GetMapHeading();
   
    // int *nav_shows_data;
    // int *nav_shows_pos;
    int *nav_shows_apt;
    int *nav_shows_fix;
    int *nav_shows_vor;
    int *nav_shows_ndb;
    if ((acf_type == 2) || (acf_type == 3)) {
      if (is_captain) {
	nav_shows_fix = link_dataref_int("laminar/B738/EFIS/EFIS_fix_on");
	nav_shows_apt = link_dataref_int("laminar/B738/EFIS/EFIS_airport_on");
	nav_shows_vor = link_dataref_int("laminar/B738/EFIS/EFIS_vor_on");
	nav_shows_ndb = link_dataref_int("laminar/B738/EFIS/EFIS_vor_on");
      } else {
	nav_shows_fix = link_dataref_int("laminar/B738/EFIS/fo/EFIS_fix_on");
	nav_shows_apt = link_dataref_int("laminar/B738/EFIS/fo/EFIS_airport_on");
	nav_shows_vor = link_dataref_int("laminar/B738/EFIS/fo/EFIS_vor_on");
	nav_shows_ndb = link_dataref_int("laminar/B738/EFIS/fo/EFIS_vor_on");
      }
    } else if (acf_type == 1) {
      nav_shows_vor = link_dataref_int("x737/cockpit/EFISCTRL_0/STA_on");
      nav_shows_ndb = link_dataref_int("x737/cockpit/EFISCTRL_0/STA_on");
      nav_shows_fix = link_dataref_int("x737/cockpit/EFISCTRL_0/WPT_on");
      nav_shows_apt = link_dataref_int("x737/cockpit/EFISCTRL_0/ARPT_on");
      // nav_shows_data = link_dataref_int("x737/cockpit/EFISCTRL_0/DATA_on");
      // nav_shows_pos = link_dataref_int("x737/cockpit/EFISCTRL_0/POS_on");
    } else {
      nav_shows_fix = link_dataref_int("sim/cockpit2/EFIS/EFIS_fix_on");
      nav_shows_apt = link_dataref_int("sim/cockpit2/EFIS/EFIS_airport_on");
      nav_shows_vor = link_dataref_int("sim/cockpit2/EFIS/EFIS_vor_on");
      nav_shows_ndb = link_dataref_int("sim/cockpit2/EFIS/EFIS_ndb_on");
    }
        
    // The input coordinates are in lon/lat, so we have to rotate against true heading
    // despite the NAV display is showing mag heading
    if (heading_map != FLT_MISS) {

      // Shift center and rotate about heading
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
    
      // plot map options
      m_pFontManager->SetSize( m_Font, 0.75*fontSize, 0.75*fontSize );
      if (*nav_shows_apt == 1) {
	glColor3ub(COLOR_BLACK);
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glVertex2f(0,m_PhysicalSize.y*0.380);
	glVertex2f(0,m_PhysicalSize.y*0.420);
	glVertex2f(m_PhysicalSize.x*0.100,m_PhysicalSize.y*0.420);
	glVertex2f(m_PhysicalSize.x*0.100,m_PhysicalSize.y*0.380);
	glEnd();
	glColor3ub(COLOR_LIGHTBLUE);
	m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.388 , "ARPT", m_Font );
      }
      if (*nav_shows_fix == 1) {
	glColor3ub(COLOR_BLACK);
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glVertex2f(0,m_PhysicalSize.y*0.340);
	glVertex2f(0,m_PhysicalSize.y*0.380);
	glVertex2f(m_PhysicalSize.x*0.100,m_PhysicalSize.y*0.380);
	glVertex2f(m_PhysicalSize.x*0.100,m_PhysicalSize.y*0.340);
	glEnd();
	glColor3ub(COLOR_LIGHTBLUE);
	m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.348 , "WPT", m_Font );
      }
      if ((*nav_shows_vor == 1) || (*nav_shows_ndb == 1)) {
	glColor3ub(COLOR_BLACK);
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glVertex2f(0,m_PhysicalSize.y*0.300);
	glVertex2f(0,m_PhysicalSize.y*0.340);
	glVertex2f(m_PhysicalSize.x*0.100,m_PhysicalSize.y*0.340);
	glVertex2f(m_PhysicalSize.x*0.100,m_PhysicalSize.y*0.300);
	glEnd();
	glColor3ub(COLOR_LIGHTBLUE);
 	m_pFontManager->Print( m_PhysicalSize.x*0.013, m_PhysicalSize.y*0.308 , "STA", m_Font );
      }

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
	double lon;
	double lat;
	float xPos;
	float yPos;
	float xPosR;
	float yPosR;
	float sideL;
	float sideR;

	// calculate approximate lon/lat range to search for given selected map range
	int nlat = ceil(mapRange * 1852.0 / (2.0 * 3.141592 * 6378137.0) * 360.0);
	int nlon;
	if (fabs(aircraftLat) < 89.5) {
	  nlon = ceil(mapRange * 1852.0 / (2.0 * 3.141592 * 6378137.0) * 360.0 / 
		      cos(aircraftLat * 0.0174533));
	} else {
	  // north/south pole: span full longitude range
	  nlon = 180;
	}

	double aircraftLon2;
	double aircraftLat2;
      
	// define overall symbol size (for DME, FIX, VOR, APT etc.)
	// this scale gives the radius of the symbol in physical units
	float ss = m_PhysicalSize.y*0.02;

	for (int la=-nlat; la <=nlat; la++) {
	  for (int lo=-nlon; lo <=nlon; lo++) {

	    aircraftLon2 = aircraftLon + (double) lo;
	    aircraftLat2 = aircraftLat + (double) la;

	    // REMOVED: only display Fixes with a map range <= 40 nm
	    if (*nav_shows_fix == 1) {

	      //----------Fixes-----------
	  
	      /*
		FixList::iterator fixIt;    
		FixList* pFixList = m_pNavDatabase->GetFixList();
	      */

	      GeographicObjectList::iterator fixIt;
	      GeographicObjectList::iterator iter;
	      GeographicHash* pFixHash = m_pNavDatabase->GetFixHash();
	      std::list<OpenGC::GeographicObject*>* pFixList = pFixHash->GetListAtLatLon(aircraftLat2,aircraftLon2);

	      for (fixIt = pFixList->begin(); fixIt != pFixList->end(); ++fixIt) {

		lon = (*fixIt)->GetDegreeLon();
		lat = (*fixIt)->GetDegreeLat();

		// convert to azimuthal equidistant coordinates with acf in center
		lonlat2gnomonic(&lon, &lat, &easting, &northing, &aircraftLon, &aircraftLat);

		// Compute physical position relative to acf center on screen
		yPos = -northing / 1852.0 / mapRange * map_size; 
		xPos = easting / 1852.0  / mapRange * map_size;
		    
		// Only draw the Fix if it's visible within the rendering area
		if ( sqrt(xPos*xPos + yPos*yPos) < map_size) {

		  // check for double matches of coordinates within the list
		  // do not draw over existing Fix
		  bool taken = false;
		  for (iter = pFixList->begin(); iter != fixIt ; ++iter) {
		    if (((*iter)->GetDegreeLon() == lon) && (*iter)->GetDegreeLat() == lat) {
		      taken = true;
		    }
		  }
		  
		  // calculate physical position on heading rotated nav map
		  float rotateRad = -1.0 * heading_map * dtor;

		  xPosR = cos(rotateRad) * xPos + sin(rotateRad) * yPos;
		  yPosR = -sin(rotateRad) * xPos + cos(rotateRad) * yPos;

		  sideL = (xPosR + 0.95 * map_size) * (-0.25 * map_size - 0.75 * map_size) -
		    (yPosR - 0.75 * map_size) * (-0.15 * map_size + 0.75 * map_size);
		  sideR = (xPosR - 0.95 * map_size) * (-0.25 * map_size - 0.75 * map_size) -
		    (yPosR - 0.75 * map_size) * (0.15 * map_size - 0.75 * map_size);

		  // Only draw the Fix if it is in the upper section of the screen
		  if ((sideL <= 0.0) && (sideR >= 0.0) && (!taken)) {
		
		    // We're going to be clever and "derotate" each label
		    // by the heading, so that everything appears upright despite
		    // the global rotation. This makes all of the labels appear at
		    // 0,0 in the local coordinate system
		    glPushMatrix();
		  
		    glTranslatef(xPos, yPos, 0.0);
		    glRotatef(-1.0* heading_map, 0, 0, 1);
		  
		    /* small triangle: white */
		    float ss2 = 0.50*ss;
		    for (i=0;i<2;i++) {
		      if (i==0) {
			glColor3ub(COLOR_BLACK);
			glLineWidth(lineWidth*2.0);
		      } else {
			glColor3ub(COLOR_WHITE);
			glLineWidth(lineWidth);
		      }
		      glBegin(GL_LINE_LOOP);
		      glVertex2f(-ss2, -1.0*ss2);
		      glVertex2f(ss2, -1.0*ss2);
		      glVertex2f(0.0, 1.0*ss2);
		      glEnd();
		      m_pFontManager->SetSize(m_Font, 0.65*fontSize, 0.65*fontSize);
		      if (i==0) {
			m_pFontManager->Print(1.35*ss2,-2.85*ss2, (*fixIt)->GetIdentification().c_str(), m_Font);
		      } else {
			m_pFontManager->Print(1.25*ss2,-2.75*ss2, (*fixIt)->GetIdentification().c_str(), m_Font);
		      }
		    }
		  
		    glPopMatrix();

		  
		  }
		}
	      }

	    }

	    //----------Navaids-----------

	    /*
	      NavaidList::iterator navIt;
	      NavaidList* pNavList = m_pNavDatabase->GetNavaidList();
	    */

	    GeographicObjectList::iterator navIt;
	    GeographicHash* pNavHash = m_pNavDatabase->GetNavaidHash();
	    std::list<OpenGC::GeographicObject*>* pNavList = pNavHash->GetListAtLatLon(aircraftLat2,aircraftLon2);

	    for (navIt = pNavList->begin(); navIt != pNavList->end(); ++navIt) {

	      lon = (*navIt)->GetDegreeLon();
	      lat = (*navIt)->GetDegreeLat();

	      // convert to azimuthal equidistant coordinates with acf in center
	      lonlat2gnomonic(&lon, &lat, &easting, &northing, &aircraftLon, &aircraftLat);

	      // Compute physical position relative to acf center on screen
	      yPos = -northing / 1852.0 / mapRange * map_size; 
	      xPos = easting / 1852.0  / mapRange * map_size;
		    
	      // Only draw the NAV if it's visible within the rendering area
	      if ( sqrt(xPos*xPos + yPos*yPos) < map_size) {

		// calculate physical position on heading rotated nav map
		float rotateRad = -1.0 * heading_map * dtor;

		xPosR = cos(rotateRad) * xPos + sin(rotateRad) * yPos;
		yPosR = -sin(rotateRad) * xPos + cos(rotateRad) * yPos;

		sideL = (xPosR + 0.95 * map_size) * (-0.25 * map_size - 0.75 * map_size) -
		  (yPosR - 0.75 * map_size) * (-0.15 * map_size + 0.75 * map_size);
		sideR = (xPosR - 0.95 * map_size) * (-0.25 * map_size - 0.75 * map_size) -
		  (yPosR - 0.75 * map_size) * (0.15 * map_size - 0.75 * map_size);

		// Only draw the nave if it is in the upper section of the screen
		if ((sideL <= 0.0) && (sideR >= 0.0)) {

		  int type = (*navIt)->GetNavaidType();

		  if (((*nav_shows_ndb == 1) || (*nav_shows_vor == 1)) &&
		      ((type == 2) || (type == 3) || (type == 12) || (type == 13))) {

		    // We're going to be clever and "derotate" each label
		    // by the heading, so that everything appears upright despite
		    // the global rotation. This makes all of the labels appear at
		    // 0,0 in the local coordinate system
		    glPushMatrix();

		    glTranslatef(xPos, yPos, 0.0);
		    glRotatef(-1.0* heading_map, 0, 0, 1);

		    for (i=0;i<2;i++) {
      
		      if (type == 2) {
			/* NDB: two circles violet */
			if (i==0) {
			  glColor3ub(COLOR_BLACK);
			  glLineWidth(lineWidth*2.0);
			} else {
			  glColor3ub(COLOR_VIOLET);
			  glLineWidth(lineWidth);
			}
			aCircle.SetRadius(ss);
			aCircle.SetOrigin(0.0, 0.0);
			glBegin(GL_LINE_LOOP);
			aCircle.Evaluate();
			glEnd();
			
			aCircle.SetRadius(0.5*ss);
			aCircle.SetOrigin(0.0,0.0);
			glBegin(GL_LINE_LOOP);
			aCircle.Evaluate();
			glEnd();
		      } else if (type == 3) {
			// VOR: six point polygon light blue
			float ss3 = 0.65*ss;
			if (i==0) {
			  glColor3ub(COLOR_BLACK);
			  glLineWidth(lineWidth*2.0);
			} else {
			  glColor3ub(COLOR_LIGHTBLUE);
			  glLineWidth(lineWidth);
			}
			glBegin(GL_LINE_LOOP);
			glVertex2f(-0.5*ss3, -0.866*ss3);
			glVertex2f(0.5*ss3, -0.866*ss3);
			glVertex2f(ss3, 0.0);
			glVertex2f(0.5*ss3, 0.866*ss3);
			glVertex2f(-0.5*ss3, 0.866*ss3);
			glVertex2f(-ss3, 0.0);
			glEnd();
		      } else {
			// DME or DME part of VOR/VORTAC: add three radial boxes to VOR symbol
			float ss4 = 0.65*ss;
			if (i==0) {
			  glColor3ub(COLOR_BLACK);
			  glLineWidth(lineWidth*2.0);
			} else {
			  glColor3ub(COLOR_GREEN);
			  glLineWidth(lineWidth);
			}		       
			glBegin(GL_LINE_LOOP);
			glVertex2f(-0.5*ss4, -0.866*ss4);
			glVertex2f(0.5*ss4, -0.866*ss4);
			glVertex2f(ss4, 0.0);
			glVertex2f(0.5*ss4, 0.866*ss4);
			glVertex2f(-0.5*ss4, 0.866*ss4);
			glVertex2f(-ss4, 0.0);
			glEnd();
			glBegin(GL_LINE_STRIP);
			glVertex2f(-0.5*ss4, -0.866*ss4);
			glVertex2f(-0.5*ss4, -1.732*ss4);
			glVertex2f(0.5*ss4, -1.732*ss4);
			glVertex2f(0.5*ss4, -0.866*ss4);
			glEnd();
			glBegin(GL_LINE_STRIP);
			glVertex2f(ss4, 0.0);
			glVertex2f(1.75*ss4, 0.433*ss4);
			glVertex2f(1.25*ss4, 1.366*ss4);
			glVertex2f(0.5*ss4, 0.866*ss4);
			glEnd();
			glBegin(GL_LINE_STRIP);
			glVertex2f(-0.5*ss4, 0.866*ss4);
			glVertex2f(-1.25*ss4, 1.366*ss4);
			glVertex2f(-1.75*ss4, 0.433*ss4);
			glVertex2f(-ss4, 0.0);
			glEnd();
		      }

		      m_pFontManager->SetSize(m_Font, 0.65*fontSize, 0.65*fontSize);
		      if (i==0) {
			m_pFontManager->Print(1.35*ss,-1.85*ss, (*navIt)->GetIdentification().c_str(), m_Font);
		      } else {
			m_pFontManager->Print(1.25*ss,-1.75*ss, (*navIt)->GetIdentification().c_str(), m_Font);
		      }

		    }
	
		    glPopMatrix();
		  }
		}
	      }
	    }

	    //----------Airports-----------

	    if (*nav_shows_apt == 1) {

	      GeographicObjectList::iterator aptIt;
	      GeographicHash* pAptHash = m_pNavDatabase->GetAirportHash();
	      std::list<OpenGC::GeographicObject*>* pAptList = pAptHash->GetListAtLatLon(aircraftLat2,aircraftLon2);

	      /*
		AirportList::iterator aptIt;
		AirportList* pAptList = m_pNavDatabase->GetAirportList();
	      */

	      for (aptIt = pAptList->begin(); aptIt != pAptList->end(); ++aptIt) {
						
		lon = (*aptIt)->GetDegreeLon();
		lat = (*aptIt)->GetDegreeLat();

		// convert to azimuthal equidistant coordinates with acf in center
		lonlat2gnomonic(&lon, &lat, &easting, &northing, &aircraftLon, &aircraftLat);

		// Compute physical position relative to acf center on screen
		yPos = -northing / 1852.0 / mapRange * map_size; 
		xPos = easting / 1852.0  / mapRange * map_size;
		    
		// Only draw the airport if it's visible within the rendering area
		if ( sqrt(xPos*xPos + yPos*yPos) < map_size) {
		
		  // calculate physical position on heading rotated nav map
		  float rotateRad = -1.0 * heading_map * dtor;

		  xPosR = cos(rotateRad) * xPos + sin(rotateRad) * yPos;
		  yPosR = -sin(rotateRad) * xPos + cos(rotateRad) * yPos;

		  sideL = (xPosR + 0.95 * map_size) * (-0.25 * map_size - 0.75 * map_size) -
		    (yPosR - 0.75 * map_size) * (-0.15 * map_size + 0.75 * map_size);
		  sideR = (xPosR - 0.95 * map_size) * (-0.25 * map_size - 0.75 * map_size) -
		    (yPosR - 0.75 * map_size) * (0.15 * map_size - 0.75 * map_size);

		  // Only draw the APT if it is in the upper section of the screen
		  if ((sideL <= 0.0) && (sideR >= 0.0)) {

		    // printf("APT: %s %f %f \n",(*aptIt)->GetIdentification().c_str(),lon,lat);

		    // We're going to be clever and "derotate" each label
		    // by the heading, so that everything appears upright despite
		    // the global rotation. This makes all of the labels appear at
		    // 0,0 in the local coordinate system
		    glPushMatrix();

		    glTranslatef(xPos, yPos, 0.0);
		    glRotatef(-1.0* heading_map, 0, 0, 1);

		    // Airports are light blue 
		    for (i=0;i<2;i++) {
		      if (i==0) {
			glColor3ub(COLOR_BLACK);
			glLineWidth(lineWidth*2.0);
		      } else {
			glColor3ub(COLOR_LIGHTBLUE);
			glLineWidth(lineWidth);
		      }		       
		      aCircle.SetRadius(ss);
		      aCircle.SetOrigin(0.0, 0.0);
		      glBegin(GL_LINE_LOOP);
		      aCircle.Evaluate();
		      glEnd();
		      m_pFontManager->SetSize(m_Font, 0.65*fontSize, 0.65*fontSize);
		      if (i==0) {
			m_pFontManager->Print(1.35*ss,-1.85*ss, (*aptIt)->GetIdentification().c_str(), m_Font);
		      } else {
			m_pFontManager->Print(1.25*ss,-1.75*ss, (*aptIt)->GetIdentification().c_str(), m_Font);
		      }
		    }
		    
		    glPopMatrix();

		  }
		}
	      }

	    }

	  } // loop through longitude range around acf
	} // loop through latitude range around acf

      } // valid acf coordinates

      /* end of down-shifted coordinate system */
      glPopMatrix();

    }
    
  }

} // end namespace OpenGC

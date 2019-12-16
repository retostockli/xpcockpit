/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcNavTestComponent.cpp,v $

  Copyright (C) 2001-2 by:
    Original author:
      Damion Shelton
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2003/05/09 05:39:12 $
    Version:   $Revision: 1.6 $
    Author:    $Author: damion $
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================*/

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string>

#include <DataSources/ogcDataSource.h>
#include "ogcNavTestComponent.h"

ogcNavTestComponent::ogcNavTestComponent()
{
  printf("ogcNavTestComponent constructed\n");
  
  m_Font = m_pFontManager->LoadDefaultFont();

  m_PhysicalPosition.x = 0;
  m_PhysicalPosition.y = 0;

  m_PhysicalSize.x = 180;
  m_PhysicalSize.y = 180;
  
  m_Scale.y = 1.0;
  m_Scale.x = 1.0;
  
  // Set initial gauge size in nautical miles per side
  m_SizeNM = 50.0;
}

ogcNavTestComponent::~ogcNavTestComponent()
{
  // Destruction handled by base class
}

void ogcNavTestComponent::Render()
{
	ogcGaugeComponent::Render();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  
  // Where is the aircraft?
  double aircraftLat = m_pDataSource->GetLatitude();
  double aircraftLon = m_pDataSource->GetLongitude();

  // cout << "Aircraft is at " << aircraftLat << "degrees lat and " << aircraftLon << "degrees lon." << endl;
  
  // What's the heading?
  double heading = m_pDataSource->True_Heading;

  // Rotate about that heading
  glTranslatef(90.0, 90.0, 0.0);
  glRotatef(heading, 0, 0, 1);

  // Convert the aircraft position to mercator coordinates
  bool isInSouthernHemisphere = false;
    
  aircraftLat *= (3.1415927 / 180.0);
  aircraftLon *= (3.1415927 / 180.0);
    
  if(aircraftLat < 0)
  {
    isInSouthernHemisphere = true;
    aircraftLat = aircraftLat * -1;
  }
  
  // approximate radius of the earth in meters
  double r = 6371000.0;
  
  // northing and easting in nm
  double aircraftEasting = r * aircraftLon / 1852.0;
  double aircraftNorthing = r * log(tan(3.1415927 / 4.0 + aircraftLat / 2.0)) / 1852.0;
    
  if(isInSouthernHemisphere)
    aircraftNorthing = aircraftNorthing * -1.0;

  // Ok, time to process some data
  ogcNavaidList* pNavList = m_pNavDatabase->GetNavaidList();
  ogcAirportList* pAptList = m_pNavDatabase->GetAirportList();

  double northing, easting;
  
  //----------First the navaids-----------

  ogcNavaidList::iterator navIt;
  
  // Point drawing parameters
  glColor3f(1.0, 1.0, 1.0);
  glPointSize(2.0);

  for (navIt = pNavList->begin(); navIt != pNavList->end(); ++navIt)
  {
    // Get mercator coordinates in meters
    (*navIt)->GetMercatorMeters(northing, easting);

    // Convert to nautical miles
    northing /= 1852.0;
    easting /= 1852.0;

    // Compute position relative to gauge center
    float yPos = (northing - aircraftNorthing) * 180.0 / m_SizeNM;
    float xPos = (easting - aircraftEasting) * 180.0 / m_SizeNM;
    
    // Only draw the navaid if it's visible within the rendering area
    if( (fabs(xPos) < 90.0)&&(fabs(yPos) < 90.0) )
    {
      // We're going to be clever and "derotate" each label
      // by the heading, so that everything appears upright despite
      // the global rotation. This makes all of the labels appear at
      // 0,0 in the local coordinate system
      glPushMatrix();

      glTranslatef(xPos, yPos, 0.0);
      glRotatef(-1.0*heading, 0, 0, 1);

      glBegin(GL_POINTS);
      glVertex2f( 0.0, 0.0);
      glEnd();

      m_pFontManager->SetSize(m_Font, 6.0, 6.0);
      m_pFontManager->Print(0.0, 0.0, (*navIt)->GetIdentification().c_str(), m_Font);
      
      glPopMatrix();
    }
  }
  
  //----------Airports second-----------

  ogcAirportList::iterator aptIt;
  
  // Point drawing parameters
  glColor3f(1.0, 0.0, 0.0);
  glPointSize(2.0);

  for (aptIt = pAptList->begin(); aptIt != pAptList->end(); ++aptIt)
  {
    // Get mercator coordinates in meters
    (*aptIt)->GetMercatorMeters(northing, easting);

    // Convert to nautical miles
    northing /= 1852.0;
    easting /= 1852.0;

    // Compute position relative to gauge center
    float yPos = (northing - aircraftNorthing) * 180.0 / m_SizeNM;
    float xPos = (easting - aircraftEasting) * 180.0 / m_SizeNM;
    
    // Only draw the airport if it's visible within the rendering area
    if( (fabs(xPos) < 90.0)&&(fabs(yPos) < 90.0) )
    {
      // We're going to be clever and "derotate" each label
      // by the heading, so that everything appears upright despite
      // the global rotation. This makes all of the labels appear at
      // 0,0 in the local coordinate system
      glPushMatrix();

      glTranslatef(xPos, yPos, 0.0);
      glRotatef(-1.0*heading, 0, 0, 1);

      glBegin(GL_POINTS);
      glVertex2f( 0.0, 0.0);
      glEnd();

      m_pFontManager->SetSize(m_Font, 6.0, 6.0);
      m_pFontManager->Print(0.0, 0.0, (*aptIt)->GetIdentification().c_str(), m_Font);
      
      glPopMatrix();
    }
  }
  
  glPopMatrix();
}


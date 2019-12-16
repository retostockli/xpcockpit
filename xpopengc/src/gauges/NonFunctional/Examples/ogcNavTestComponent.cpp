/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcNavTestComponent.cpp,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:10 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2003 Damion Shelton
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

#include "ogcDataSource.h"
#include "ogcNavTestComponent.h"

namespace OpenGC
{

NavTestComponent::NavTestComponent()
{
  printf("NavTestComponent constructed\n");
  
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

NavTestComponent::~NavTestComponent()
{
  // Destruction handled by base class
}

void NavTestComponent::Render()
{
	GaugeComponent::Render();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  
  // Where is the aircraft?
  double aircraftLat = m_pDataSource->GetAirframe()->GetLatitude();
  double aircraftLon = m_pDataSource->GetAirframe()->GetLongitude();

  // cout << "Aircraft is at " << aircraftLat << "degrees lat and " << aircraftLon << "degrees lon." << endl;
  
  // What's the heading?
  double heading = m_pDataSource->GetAirframe()->GetTrue_Heading();

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
  NavaidList* pNavList = m_pNavDatabase->GetNavaidList();
  AirportList* pAptList = m_pNavDatabase->GetAirportList();

  double northing, easting;
  
  //----------First the navaids-----------

  NavaidList::iterator navIt;
  
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

  AirportList::iterator aptIt;
  
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

// Message handler
void NavTestComponent::OnOpenGCMessage(Message message, void* data)
{
  // Send the appropriate message based on the button number
  switch(message)
  {
  case MSG_NAV_ZOOM_INCREASE:
    {
        m_SizeNM /= 2.0;
    }
    break;

  case MSG_NAV_ZOOM_DECREASE:
    {
        m_SizeNM *= 2.0;
    }
    break;
  }
}

} // end namespace OpenGC

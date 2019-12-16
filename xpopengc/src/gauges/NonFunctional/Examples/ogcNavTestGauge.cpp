/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcNavTestGauge.cpp,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:11 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2003 Damion Shelton
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <stdio.h>
#include <math.h>

#include "ogcDataSource.h"
#include "ogcNavTestComponent.h"
#include "ogcNavTestGauge.h"

namespace OpenGC
{

NavTestGauge::NavTestGauge()
{
  printf("NavTestGauge constructed\n");

  m_PhysicalSize.x = 180;
  m_PhysicalSize.y = 180;

  // We want to draw an outline
  this->SetGaugeOutline(true);
  
  NavTestComponent* pNavComp = new NavTestComponent();
  pNavComp->SetParentRenderObject(this);
  pNavComp->SetPosition(0,0);
  this->AddGaugeComponent(pNavComp);
    
  m_Font = m_pFontManager->LoadDefaultFont();
}

NavTestGauge::~NavTestGauge()
{
  // Destruction handled by base class
}

void NavTestGauge::Render()
{
  Gauge::Render();

  // Black background
  glColor3f(0.0, 0.0, 0.0);
  glBegin(GL_POLYGON);
  glVertex2f(0.0, 0.0);
  glVertex2f(0.0, 40.0);
  glVertex2f(120.0, 40.0);
  glVertex2f(120.0, 0.0);
  glEnd();

  // Lines dividing up the space in the gauge
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINES);
  // Center horiz
  glVertex2f(0.0, 20.0);
  glVertex2f(120.0, 20.0);
  // Center vert
  glVertex2f(20.0, 0.0);
  glVertex2f(20.0, 40.0);
  // Top horiz
  glVertex2f(0.0, 40.0);
  glVertex2f(120.0, 40.0);
  // Right vert
  glVertex2f(120.0, 0.0);
  glVertex2f(120.0, 40.0);
  glEnd();
  
  // Up arrow
  glBegin(GL_POLYGON);
  glVertex2f(5.0, 25.0);
  glVertex2f(10.0, 35.0);
  glVertex2f(15.0, 25.0);
  glEnd();
  
  // Down arrow
  glBegin(GL_POLYGON);
  glVertex2f(5.0, 15.0);
  glVertex2f(15.0, 15.0);
  glVertex2f(10.0, 5.0);
  glEnd();

  m_pFontManager->SetSize(m_Font, 10.0, 10.0);
  char buffer[20];
  
  sprintf(buffer, "TRACK %.0f\0", m_pDataSource->GetAirframe()->GetTrue_Heading() );
  m_pFontManager->Print(30.0, 25.0, &buffer[0], m_Font);
  
  sprintf(buffer, "GS %.0f\0", m_pDataSource->GetAirframe()->GetGround_Speed_K() );
  m_pFontManager->Print(30.0, 5.0, &buffer[0], m_Font);
}

void
NavTestGauge
::OnMouseDown(int button, double physicalX, double physicalY)
{
  if( (physicalX <= 20.0)&&(physicalY>0.0)&&(physicalY<20.0) )
  {
    this->DispatchOpenGCMessage(MSG_NAV_ZOOM_DECREASE, 0);
    return;
  }
  
  if( (physicalX <= 20.0)&&(physicalY>20.0)&&(physicalY<40.0) )
  {
    this->DispatchOpenGCMessage(MSG_NAV_ZOOM_INCREASE, 0);
    return;
  }  
}

} // end namespace OpenGC

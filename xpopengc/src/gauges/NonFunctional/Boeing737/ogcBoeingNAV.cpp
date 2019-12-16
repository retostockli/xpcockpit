/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcBoeingNAV.cpp,v $

  Copyright (C) 2001-2 by:
    Original author:
      John Wojnaroski
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2003/06/12 23:03:32 $
    Version:   $Revision: 1.3 $
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

/*
Base panel for defining the navigation displays. There are four basic display modes and
thirteen different displays selectable by the crew via the EFIS MCP
*/

#include <stdio.h>

#include "ogcGLHeaders.h"
#include "ogcBoeingNAV.h"
#include <DataSourcesogcDataSource.h>
#include <GaugeComponents/ogcNavVORCenter.h>

// The components listed below are planned...
//#include "ogcAPP_Mode.h"
//#include "ogcAPP_Cntr.h"
//#include "ogcVOR_Mode.h"
//#include "ogcMAP_Mode.h"
//#include "ogcMAP_Cntr.h"
//#include "ogcPLN_Mode.h'
//#include "ogcEFIS.h"

ogcBoeingNAV::ogcBoeingNAV()
{
  printf("ogcBoeingNAV constructed\n");

  m_Font = m_pFontManager->LoadDefaultFont();

  m_PhysicalSize.x = 180;
  m_PhysicalSize.y = 180;

  ogcNavVORCenter* pVOR_C = new ogcNavVORCenter();
  pVOR_C->SetParentRenderObject(this);
  pVOR_C->SetPosition(0,0);
  this->AddGaugeComponent(pVOR_C);
}

ogcBoeingNAV::~ogcBoeingNAV()
{
  // Destruction handled by base class
}

void ogcBoeingNAV::Render()
{
  // First thing to do is call base class setup
  ogcGauge::Render();

  char buffer[36];
  char wait;
  
  //printf("Panel # is %d\n", pEFIS->nav_panel );
  //cin >> wait;
  //ogcGauge::Select_Render( m_pDataSource->nav_panel );

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  
  glColor3ub( 0, 0, 0 );
 
  glLineWidth( 2.0 );
  glColor3ub( 0, 190, 190 );

  glBegin(GL_LINE_LOOP);
  glVertex2f( 0.0, 0.0 );
  glVertex2f( 0.0, 180.0 );
  glVertex2f( 180.0, 180.0 );
  glVertex2f( 180.0, 0.0 );
  glEnd();

  // Put in all the static data fields that appear on all the displays

  m_pFontManager->SetSize( m_Font, 4.0, 4.0 );
  glColor3ub( 0, 255, 0 );

  // just some reference index marks for building and testing
  //m_pFontManager->Print( 87,92, "", m_Font );
  //m_pFontManager->Print( 0, 90, " ---- ", m_Font );
  //m_pFontManager->Print( 90, 1, "|", m_Font );
  //m_pFontManager->Print( 90, 179, "|", m_Font );

  m_pFontManager->Print( 5,171,"GS            TAS", m_Font );

  // Set symbol for ILS or VOR station type tuned
  if ( m_pDataSource->ILS_Glideslope_Alive )
    m_pFontManager->Print( 129, 171, "ILS  L", m_Font);
  else
    m_pFontManager->Print( 131, 171,"VOR  L", m_Font );
 
  m_pFontManager->Print( 143, 163, "CRS", m_Font );
  m_pFontManager->Print( 143, 155, "DME", m_Font );

  m_pFontManager->SetSize( m_Font,5.0, 5.0);
  m_pFontManager->Print( 65,171, "HDG           MAG", m_Font);

  m_pFontManager->Print( 8,25, "VOR  L", m_Font);
  if ( m_pDataSource->Nav1_Valid )
    sprintf( buffer, "%s", m_pDataSource->Nav1_Ident );
  else sprintf( buffer, "%6.2f", m_pDataSource->Nav1_Freq );
    m_pFontManager->Print( 8,17, &buffer[0], m_Font );

  m_pFontManager->Print( 146, 25, "VOR  R", m_Font);
  if ( m_pDataSource->Nav2_Valid )
    sprintf( buffer, "%s", m_pDataSource->Nav2_Ident );
  else sprintf( buffer, "%6.2f", m_pDataSource->Nav2_Freq );
    m_pFontManager->Print( 146,17, &buffer[0], m_Font );

 
  // This always runs off the left selection or the fix in the flight plan
  glColor3ub( 255, 255, 255 );
  sprintf(buffer, "%6.2f", m_pDataSource->Nav1_Freq );
  m_pFontManager->Print( 151, 171, &buffer[0], m_Font );

  sprintf(buffer, "%03.0f", m_pDataSource->Nav1_Radial );
  m_pFontManager->Print( 161.5, 163, &buffer[0], m_Font );

  sprintf(buffer, "%5.1f", m_pDataSource->Nav1_DME );
  m_pFontManager->Print( 160, 155, &buffer[0], m_Font );

  sprintf(buffer, "%3.0f         %3.0f", m_pDataSource->Ground_Speed_K, m_pDataSource->TAS);
  m_pFontManager->Print( 18 , 171 , &buffer[0], m_Font);
 
  // always display leading zeros
  sprintf(buffer, "%03d", (int) m_pDataSource->Mag_Heading);
  m_pFontManager->Print( 85, 172, &buffer[0], m_Font);

  // just a placeholder for the wind field
  m_pFontManager->Print( 5, 163, "240/10", m_Font );

  m_pFontManager->SetSize( m_Font, 4.0, 4.0 );
  if ( m_pDataSource->Nav1_Has_DME )
  {
  sprintf( buffer, "DME %5.1f", m_pDataSource->Nav1_DME );
  m_pFontManager->Print( 8, 10, &buffer[0], m_Font );
  }

  if ( m_pDataSource->Nav2_Has_DME )
  {
  sprintf( buffer, "DME %5.1f", m_pDataSource->Nav2_DME );
  m_pFontManager->Print( 146, 10, &buffer[0], m_Font );
  }
 
	if (m_pDataSource->To_From_Flag < 0)
    m_pFontManager->Print( 113, 20, "FROM", m_Font);
	
  if (m_pDataSource->To_From_Flag > 0)
    m_pFontManager->Print( 115, 20, "TO", m_Font );

  // Draws the box around the mag heading number displayed at top-center
  glLineWidth( 1.5 );
  glBegin(GL_LINE_STRIP);
  glVertex2f( 82, 180 );
  glVertex2f( 82, 170 );
  glVertex2f( 98, 170 );
  glVertex2f( 98, 180 );
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex2f( 86,169 );
  glVertex2f( 90,164 );
  glVertex2f( 94,169 );
  glEnd();

  glPopMatrix();
}


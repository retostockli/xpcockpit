/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737LOWERDUSys.cpp,v $

  Copyright (C) 2024 by:
  Original author:
  Reto Stockli
  Contributors (in alphabetical order):

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
  The Boeing 737 LOWER DU SYS Component
*/

#include <stdio.h>
#include <math.h>

#include "B737/B737LOWERDU/ogcB737LOWERDUSys.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace OpenGC
{
  
  B737LOWERDUSys::B737LOWERDUSys()
  {
    printf("B737LOWERDUSys constructed\n");

    //  m_Font = m_pFontManager->LoadDefaultFont();
    m_Font = m_pFontManager->LoadFont((char*) "CockpitMCDU.ttf");
    
    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;
    
    m_PhysicalSize.x = 200;
    m_PhysicalSize.y = 200;
    
    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

  }

  B737LOWERDUSys::~B737LOWERDUSys()
  {
    
  }

  void B737LOWERDUSys::Render()
  {
    char buffer[10];
    GaugeComponent::Render();
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glColor3ub(COLOR_RED);
    snprintf( buffer, sizeof(buffer), "TEST" );
    this->m_pFontManager->SetSize(m_Font, 5, 5);
    this->m_pFontManager->Print(m_PhysicalSize.x/2, m_PhysicalSize.y/2, &buffer[0], m_Font);     
    glPopMatrix();
    
  }

}
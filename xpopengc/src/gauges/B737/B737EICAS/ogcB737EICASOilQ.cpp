/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737EICASOilQ.cpp,v $

  Copyright (C) 2002-2015 by:
  Original author:
  Michael DeFeyter
  Contributors (in alphabetical order):
  Reto Stockli

  Last modification:
  Date:      $Date: 2015/10/12 $
  Version:   $Revision: $
  Author:    $Author: stockli $
  
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

#include "B737/B737EICAS/ogcB737EICAS.h"
#include "B737/B737EICAS/ogcB737EICASOilQ.h"

namespace OpenGC
{

  B737EICASOilQ::B737EICASOilQ()
  {
    printf("B737EICASOilQ constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 16;
    m_PhysicalSize.y = 8;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    currentEngine = 1;
  }

  B737EICASOilQ::~B737EICASOilQ()
  {
  }

  void B737EICASOilQ::Render()
  {
    GaugeComponent::Render();

    float value;

    float *engn_oilquantity = link_dataref_flt_arr("sim/flightmodel/engine/ENGN_oil_quan",8,-1,-2);

    switch (currentEngine)
      {
      case 1: value = *(engn_oilquantity+0); break;
      case 2: value = *(engn_oilquantity+1); break;
      case 3: value = *(engn_oilquantity+2); break;
      case 4: value = *(engn_oilquantity+3); break;
      }
	
    if (value != FLT_MISS) {

      char buf[10];

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      glLineWidth(2.0);
      glEnable(GL_LINE_SMOOTH);
      glColor3ub(COLOR_WHITE);

      // white rectangle containing the text
      glBegin(GL_LINE_LOOP);
      glVertex2f( 0, 0 );
      glVertex2f( 0, 8 );
      glVertex2f( 16, 8 );
      glVertex2f( 16, 0 );
      glEnd();

      // text
      m_pFontManager->SetSize(m_Font, 5, 5);
      snprintf(buf, sizeof(buf), "%.0f", value*100.0);
      m_pFontManager->Print(4, 2, buf, m_Font);

      glPopMatrix();

    }
  }

  void B737EICASOilQ::setCurrentEngine(int engine)
  {
    currentEngine = engine;
  }

}

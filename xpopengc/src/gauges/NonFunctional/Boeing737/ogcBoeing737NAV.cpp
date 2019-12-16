/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcBoeing737NAV.cpp,v $

  Copyright (C) 2001-2 by:
    Original author:
      Jurgen Roeland
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2003/04/18 06:00:42 $
    Version:   $Revision: 1.1.1.1 $
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

#include "ogcBoeing737NAV.h"
#include "ogcBoeing737NAVComponent.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ogcBoeing737NAV::ogcBoeing737NAV()
{
	printf("ogcBoeing737NAV\n");

	m_Scale.y = 1.0;
	m_Scale.x = 1.0;

	m_PhysicalPosition.x = 0;
	m_PhysicalPosition.y = 0;

	m_PhysicalSize.x = 200;
	m_PhysicalSize.y = 230;

	m_PixelPosition.x = 0;
	m_PixelPosition.y = 0;

	m_PixelSize.x = 0;
	m_PixelSize.y = 0;

	m_NumGaugeComponents = 0;

	ogcBoeing737NAVComponent* FlC2 = new ogcBoeing737NAVComponent();
	FlC2->SetParentRenderObject(this);
	FlC2->SetPosition(0, 0);
	this->AddGaugeComponent(FlC2);
}

ogcBoeing737NAV::~ogcBoeing737NAV()
{

}

void ogcBoeing737NAV::Render()
{
	ogcGauge::Render();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLineWidth( 2.0 );
	glColor3ub( 20, 180, 180 );

	glBegin(GL_LINE_LOOP);
	glVertex2f( 0.0, 0.0 );
	glVertex2f( 0.0, 230.0 );
	glVertex2f( 200.0, 230.0 );
	glVertex2f( 200.0, 0.0 );
	glEnd();

	glPopMatrix();
}
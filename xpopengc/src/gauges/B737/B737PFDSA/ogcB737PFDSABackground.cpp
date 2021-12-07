/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Copyright (C) 2002-2021 by:
    Original author:
      Michael DeFeyter
    Contributors (in alphabetical order):
      Reto Stockli
  
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

#include "B737/B737PFDSA/ogcB737PFDSA.h"
#include "B737/B737PFDSA/ogcB737PFDSABackground.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace OpenGC
{

B737PFDSABackground::B737PFDSABackground()
{
	printf("B737PFDSABackground constructed\n");

	m_Font = m_pFontManager->LoadDefaultFont();

	m_PhysicalPosition.x = 0;
	m_PhysicalPosition.y = 0;

	m_PhysicalSize.x = 201;
	m_PhysicalSize.y = 201;

	m_Scale.x = 1.0;
	m_Scale.y = 1.0;

}

B737PFDSABackground::~B737PFDSABackground()
{

}

void B737PFDSABackground::Render()
{
	GaugeComponent::Render();

	/*
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLineWidth(2.0);
	glColor3ub(COLOR_WHITE);

	glBegin(GL_LINE_STRIP);
		glVertex2f(70, 186);
		glVertex2f(70, 197);
	glEnd();
	glBegin(GL_LINE_STRIP);
		glVertex2f(110, 186);
		glVertex2f(110, 197);
	glEnd();
	glBegin(GL_LINE_STRIP);
		glVertex2f(150, 186);
		glVertex2f(150, 197);
	glEnd();

	m_pFontManager->SetSize(m_Font, 5, 6);
	glColor3ub(COLOR_GREEN);


	if (m_pDataSource->GetAirframe()->GetAutopilot_Engaged())
		m_pFontManager->Print(154, 190, "CMD", m_Font);

	if (m_pDataSource->GetAirframe()->GetCmd_Heading_Engaged())
		m_pFontManager->Print(115, 190, "HDG HLD", m_Font);

	if (m_pDataSource->GetAirframe()->GetCmd_Altitude_Engaged())
		m_pFontManager->Print(76, 190, "ALT HLD", m_Font);

	if (m_pDataSource->GetAirframe()->GetCmd_Airspeed_Engaged())
		m_pFontManager->Print(52.5, 190, "SPD", m_Font);

	if (m_pDataSource->GetAirframe()->GetDirector_Engaged())
		m_pFontManager->Print(154, 190, "FD", m_Font);

	if (m_pDataSource->GetAirframe()->GetCmd_Approach_Engaged() || m_pDataSource->GetAirframe()->GetCmd_Nav1_Lock_Flag())
		m_pFontManager->Print(115, 181, "VOR/LOC", m_Font);

	if (m_pDataSource->GetAirframe()->GetCmd_VORLOC_Engaged())
		m_pFontManager->Print(83, 181, "G/S", m_Font);

	glPopMatrix();
	*/

}

}

/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  2024 Reto Stockli

  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "Test/ogcTestGauge.h"
#include "ogcGaugeComponent.h"

namespace OpenGC
{

TestGauge::TestGauge()
{
  if (verbosity > 0) printf("TestGauge constructed\n");

  m_Font = m_pFontManager->LoadDefaultFont();
  m_Font2 = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");
  //m_Font2 = m_pFontManager->LoadFont ((char*) "digital.ttf");

  // Specify our physical size and position
  m_PhysicalPosition.x = 0;
  m_PhysicalPosition.y = 0;
  
  m_PhysicalSize.x = 200;
  m_PhysicalSize.y = 200;
  
  // We want to draw an outline
  this->SetGaugeOutline(true);
}

TestGauge::~TestGauge()
{
  // Destruction handled by base class
}

void TestGauge::Render()
{
    Gauge::Render();

  // Rendering the test class
  float fontWidth = 24;
  float fontHeight = 36;
  float lineWidth = 4.0;

  glLineWidth(lineWidth);
  glColor3ub(255,100,200);

  char buffer[30];
  memset(buffer,0,sizeof(buffer));
  snprintf( buffer, sizeof(buffer), "%s", "ABCDEFG");

  m_pFontManager->SetOutline(false);
  m_pFontManager->SetSmooth(false);
  m_pFontManager->SetSize(m_Font, fontWidth, fontHeight);
  m_pFontManager->Print(5, 5, buffer, m_Font);

  m_pFontManager->SetOutline(true);
  m_pFontManager->SetSmooth(false);
  m_pFontManager->SetSize(m_Font2, fontWidth, fontHeight);
  m_pFontManager->Print(5, 60, buffer, m_Font2);

}

} // end namespace OpenGC

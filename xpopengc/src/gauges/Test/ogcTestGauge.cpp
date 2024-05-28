/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Gauge for testing fonts and windows ;-)

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
  //m_Font = m_pFontManager->LoadFont((char*) "CockpitMCDU.ttf");
  //  m_Font2 = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

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
  float fontWidth = 8;
  float fontHeight = 10;
  float lineWidth = 4.0;

  glLineWidth(lineWidth);
  glColor3ub(255,100,200);

  float dx = 50;
  float dy = 50;
  float x = 100;
  float y = 150;
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glBegin(GL_POLYGON);
  glVertex2f(x-0.5*dx,y-0.5*dy);
  glVertex2f(x+0.5*dx,y-0.5*dy);
  glVertex2f(x+0.5*dx,y+0.5*dy);
  glVertex2f(x-0.5*dx,y+0.5*dy);
  glEnd();

  char buffer[30];
  memset(buffer,0,sizeof(buffer));
  snprintf( buffer, sizeof(buffer), "%s", "ABCDEF");
  //snprintf( buffer, sizeof(buffer), "%s", "ABCDEF");
  buffer[3] = (char) 176;

  m_pFontManager->SetOutline(false);
  m_pFontManager->SetSmooth(true);
  glColor3ub(255,100,200);
  m_pFontManager->SetSize(m_Font, fontWidth, fontHeight);
  m_pFontManager->Print(5, 5, buffer, m_Font);

  /*
  glColor3ub(255,200,100);
  m_pFontManager->SetSize(m_Font2, fontWidth, fontHeight);
  m_pFontManager->Print(5, 150, buffer, m_Font2);
  */
}

} // end namespace OpenGC

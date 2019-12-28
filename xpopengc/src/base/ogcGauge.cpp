/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcGauge.cpp,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:27:53 $
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
#include <time.h>
#include "ogcGLHeaders.h"

#include "ogcFontManager.h"
#include "ogcDataSource.h"
#include "ogcGaugeComponent.h"
#include "ogcRenderObject.h"
#include "ogcGauge.h"

extern int verbosity;
//extern bool m_FrameTest; // how do we find that variable?

namespace OpenGC
{

Gauge
::Gauge()
{
  if (verbosity > 0) 
  {
    printf("====================\n");
    printf("Gauge - constructing\n");
  }

  if (verbosity > 1) printf ("Gauge - no parameters\n");

  m_NumGaugeComponents = 0;
  m_DrawGaugeOutline = false;

  m_Scale.y = 1.0;
  m_Scale.x = 1.0;

  m_PhysicalPosition.x = 0;
  m_PhysicalPosition.y = 0;

  m_PixelPosition.x = 0;
  m_PixelPosition.y = 0;

  m_PixelSize.x = 0;
  m_PixelSize.y = 0;

  if (verbosity > 1) 
  {
    printf("Gauge - constructed\n");
  }
}

Gauge
::~Gauge()
{
  // If there are gauge components, delete them
  if( m_NumGaugeComponents!=0 )
  {
    ComponentIteratorType it;
    for (it = m_GaugeComponentList.begin(); it != m_GaugeComponentList.end(); ++it)
    {
      delete *it;
    }
  }
}

void
Gauge
::AddGaugeComponent(GaugeComponent* pComponent)
{
  m_GaugeComponentList.push_back(pComponent);
  m_NumGaugeComponents++;
}

void
Gauge
::Render()
{
  // Overload this function in derived classes to render
  // parts of the gauge not defined by gauge components
  
  // BUT!!! you should always call the base class render function
  // as well in order to render the gauge components

  this->ResetGaugeCoordinateSystem();

  if(m_NumGaugeComponents > 0)
  {
    ComponentIteratorType it;
    for (it = m_GaugeComponentList.begin(); it != m_GaugeComponentList.end(); ++it)
    {
      (*it)->Render();
    }
  }

  this->ResetGaugeCoordinateSystem();

  if(m_DrawGaugeOutline)
    this->DrawGaugeOutline();
}

// Resets the gauge coordinate system before and after rendering components
void
Gauge
::ResetGaugeCoordinateSystem()
{
  this->RecalcWindowPlacement();

  // The viewport is established in order to clip things
  // outside the bounds of the gauge
  glViewport(m_PixelPosition.x, m_PixelPosition.y, m_PixelSize.x, m_PixelSize.y);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  // Define the projection so that we're drawing in "real" space
  glOrtho(0,m_PhysicalSize.x,0, m_PhysicalSize.y, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void
Gauge
::RecalcWindowPlacement()
{
  // Figure out where we're drawing in the window
  m_PixelPosition.x = (int) (m_PhysicalPosition.x / m_UnitsPerPixel);
  m_PixelPosition.y = (int) (m_PhysicalPosition.y / m_UnitsPerPixel);

  m_PixelSize.x = (int) (m_PhysicalSize.x / m_UnitsPerPixel)*m_Scale.x;
  m_PixelSize.y = (int) (m_PhysicalSize.y / m_UnitsPerPixel)*m_Scale.y;

  if (verbosity > 3)
  {
    printf("Gauge - Physical position: x %f, y %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
    printf("Gauge - Pixel position:    x %i, y %i\n", m_PixelPosition.x, m_PixelPosition.y);
    printf("Gauge - Physical size:     x %f, y %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
    printf("Gauge - Pixel size:        x %i, y %i\n", m_PixelSize.x, m_PixelSize.y);
  }
}

void
Gauge
::SetUnitsPerPixel(double unitsPerPixel)
{
  m_UnitsPerPixel = unitsPerPixel;

  if(m_NumGaugeComponents > 0)
  {
    ComponentIteratorType it;
    for (it = m_GaugeComponentList.begin(); it != m_GaugeComponentList.end(); ++it)
    {
      (*it)->SetUnitsPerPixel(m_UnitsPerPixel);
    }
  }
}

void
Gauge
::SetScale(double xScale, double yScale)
{
  // Set gauge scaling factors, must be greater than 0
  if( (xScale > 0) && (yScale > 0 ) )
  {
    m_Scale.x = xScale;
    m_Scale.y = yScale;

    if(m_NumGaugeComponents > 0)
    {
      ComponentIteratorType it;
      for (it = m_GaugeComponentList.begin(); it != m_GaugeComponentList.end(); ++it)
      {
        (*it)->SetScale(xScale, yScale);
      }
    }
  }
}

void
Gauge
::SetArg(int arg)
{
  // Set gauge optional argument
  if(m_NumGaugeComponents > 0)
    {
      m_Arg = arg;
      
      ComponentIteratorType it;
      for (it = m_GaugeComponentList.begin(); it != m_GaugeComponentList.end(); ++it)
	{
	  (*it)->SetArg(arg);
	}
    }
}

bool
Gauge
::ClickTest(int button, int state, int x, int y)
{
  if( (x >= (int) m_PixelPosition.x)&&(x <= (int) m_PixelPosition.x + (int) m_PixelSize.x)
      &&(y >= (int) m_PixelPosition.y)&&(y <= (int) m_PixelPosition.y + (int) m_PixelSize.y) )
  {
    if(m_NumGaugeComponents > 0)
    {
      ComponentIteratorType it;
      for (it = m_GaugeComponentList.begin(); it != m_GaugeComponentList.end(); ++it)
      {
        (*it)->HandleMouseButton(button, state, x, y);
      }
    }
    return true;
  }
  else
  {
    return false;
  }
}

// Called when a message is triggered somewhere
void 
Gauge
::OnMessage(Message message, void * data)
{
  if(m_NumGaugeComponents > 0)
  {
    ComponentIteratorType it;
    for (it = m_GaugeComponentList.begin(); it != m_GaugeComponentList.end(); ++it)
    {
      (*it)->OnOpenGCMessage(message, data);
    }
  }
}

void
Gauge
::DrawGaugeOutline()
{
  glLineWidth( 2.0 );
  glColor3ub( 0, 190, 190 );

  glBegin(GL_LINE_LOOP);
  glVertex2f( 0.0, 0.0 );
  glVertex2f( 0.0, m_PhysicalSize.y );
  glVertex2f( m_PhysicalSize.x, m_PhysicalSize.y );
  glVertex2f( m_PhysicalSize.x, 0.0 );
  glEnd();
}

} // end namespace OpenGC

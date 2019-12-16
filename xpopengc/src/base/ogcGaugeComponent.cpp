/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcGaugeComponent.cpp,v $

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
#include "ogcGLHeaders.h"

#include "ogcDataSource.h"
#include "ogcRenderObject.h"
#include "ogcGaugeComponent.h"

extern int verbosity;

namespace OpenGC
{

GaugeComponent
::GaugeComponent()
{
  if (verbosity > 0) printf("GaugeComponent - constructing\n");

  if (verbosity > 1) printf("GaugeComponent - constructed\n");
}

GaugeComponent
::~GaugeComponent()
{

}

/** handle positioning and clipping */
void
GaugeComponent
::setPosition ()
{
  double halfWidth = m_PhysicalSize.x * 0.5;
  double halfHeight = m_PhysicalSize.y * 0.5;
  double translHor = m_PixelPosition.x + halfWidth;
  double translVert = m_PixelPosition.y + halfHeight;
  // Translate to the center of the component
  glTranslated (translHor, translVert, 0);
}

void
GaugeComponent
::setClip ()
{
  double halfWidth = m_PhysicalSize.x * 0.5;
  double halfHeight = m_PhysicalSize.y * 0.5;
  GLdouble eqnlft[4] = {1.0, 0.0, 0.0, halfWidth}; // left plane
  GLdouble eqnrgt[4] = {-1.0, 0.0, 0.0, halfWidth};// right plane
  GLdouble eqntop[4] = {0.0, -1.0, 0.0, halfHeight}; // top plane
  GLdouble eqnbot[4] = {0.0, 1.0, 0.0, halfHeight}; // bottom plane

  glClipPlane (GL_CLIP_PLANE0, eqnlft);
  glEnable (GL_CLIP_PLANE0);
  glClipPlane (GL_CLIP_PLANE1, eqnrgt);
  glEnable (GL_CLIP_PLANE1);
  glClipPlane (GL_CLIP_PLANE2, eqntop);
  glEnable (GL_CLIP_PLANE2);
  glClipPlane (GL_CLIP_PLANE3, eqnbot);
  glEnable (GL_CLIP_PLANE3);
}

void
GaugeComponent
::unsetClip ()
{
    glDisable (GL_CLIP_PLANE0);
    glDisable (GL_CLIP_PLANE1);
    glDisable (GL_CLIP_PLANE2);
    glDisable (GL_CLIP_PLANE3);
}
 
void
GaugeComponent
::Render()
{
  // Overload this function in derived classes to render
  // the gauge component
  OrderedPair<double> parentPhysicalPosition = m_pParentRenderObject->GetPhysicalPosition();

  // The location in pixels is calculated based on the size of the
  // gauge component and the offset of the parent gauge
  m_PixelPosition.x = (int) ( (m_PhysicalPosition.x * m_Scale.x + parentPhysicalPosition.x ) / m_UnitsPerPixel);
  m_PixelPosition.y = (int) ( (m_PhysicalPosition.y * m_Scale.y + parentPhysicalPosition.y ) / m_UnitsPerPixel);

  // The size in pixels of the gauge is the physical size / mm per pixel
  m_PixelSize.x = (int) ( m_PhysicalSize.x / m_UnitsPerPixel * m_Scale.x);
  m_PixelSize.y = (int) ( m_PhysicalSize.y / m_UnitsPerPixel * m_Scale.y);
    
  // The viewport is established in order to clip things
  // outside the bounds of the GaugeComponent
  glViewport(m_PixelPosition.x, m_PixelPosition.y, m_PixelSize.x, m_PixelSize.y);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  // Define the projection so that we're drawing in "real" space
  glOrtho(0, m_Scale.x * m_PhysicalSize.x, 0, m_Scale.y * m_PhysicalSize.y, -1, 1);

  // Prepare the modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glScalef(m_Scale.x, m_Scale.y, 1.0f);
}

bool
GaugeComponent
::ClickTest(int button, int state, int x, int y)
{
  if( (x >= (int) m_PixelPosition.x)&&(x <= (int) m_PixelPosition.x + (int) m_PixelSize.x)
      &&(y >= (int) m_PixelPosition.y)&&(y <= (int) m_PixelPosition.y + (int) m_PixelSize.y) )
  {
    //cout << "Click is inside the GaugeComponent\n";
    return true;
  }
  else
  {
    //cout << "Click is NOT inside the GaugeComponent\n";
    return false;
  }
}

} // end namespace OpenGC

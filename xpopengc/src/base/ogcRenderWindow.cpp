/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcRenderWindow.cpp,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:27:55 $
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
#include <time.h>

#include "ogcGLHeaders.h"
#include "ogcGauge.h"
#include "ogcRenderWindow.h"

extern int verbosity;

namespace OpenGC
{

RenderWindow
::RenderWindow()
{
  if (verbosity > 0) printf("RenderWindow - constructing\n");

  m_Font = 0; 
// Nothing of the following works... :
// neither: m_Font = theApp->m_pFontManager->LoadFont((char*) "CockpitScreens.ttf");
// nor:     m_Font = OpenGC::RenderObject::m_pFontManager->LoadFont((char*) "CockpitScreens.ttf");
// nor:     m_Font = OpenGC::RenderObject::GetFontManager()->m_pFontManager->LoadFont((char*) "CockpitScreens.ttf");
// cannot access m_pFontManager here - but that works in all GaugeComponents!?!

  // Set the window size in pixels to 0 start with
  m_WindowSize.x = 0;               
  m_WindowSize.y = 0;

  // Initial pixel size, we're assuming that pixels are square
  // so this is both the x & y dimensions of a pixel in mm
   //  m_UnitsPerPixel = 0.25;
   // - hj:
   // This is more or less correct on my 17" widescreen laptop:
   //      width 370mm, with 1440 pixels -> 0.257
   // On a standard TFT monitor this becomes:
   // 15": width 305mm, with 1024 pixels -> 0.298
   //                   with 1280 pixels -> 0.238
   // 19": width 380mm, with 1024 pixels -> 0.371
   //                   with 1280 pixels -> 0.297
   // Shouldn't this parameter be found in the .ini file?
   // - hj end
  m_UnitsPerPixel = 1.0; 
  
  // No gauges by default
  m_NumGauges = 0;
  
  // We should setup the GL stuff the first time we try to render
  m_DisplayIsInitialized = false;

  // NOT initially ok to render, since a rendering context probably
  // isn't around
  m_IsOKToRender = false;

  if (verbosity > 1) printf("RenderWindow - constructed\n");
}

RenderWindow
::~RenderWindow()
{
  GaugeIteratorType it;

  // If there are gauges, delete them
  if( !m_NumGauges==0 )
  {    
    for (it = m_GaugeList.begin(); it != m_GaugeList.end(); ++it)
    {
      delete (*it);
    }
  }
}

  int frCnt = 0; time_t oldTime = 0, newTime = 0; long diffTime; char buffer[6];

// Shows the frame rate at top-left of window
void
RenderWindow
::ShowFrameRate()
{
  // Increment the frame counter
  frCnt++;
  newTime = time(NULL);
  if (frCnt == 100) {
    frCnt = 0;
    diffTime = difftime(newTime, oldTime);
    oldTime = newTime;
printf("frame rate: %i\n", (int) (100.0/diffTime));
/*
// Note: can't find m_pFontManager!
    theApp->m_pFontManager->SetSize(m_Font, 10, 12);
    snprintf( buffer, sizeof(buffer), "%i", (int) (100.0/diffTime));
    theApp->m_pFontManager->Print(0, -12, &buffer[0], m_Font);
*/
  }
}

void
RenderWindow
::SetupDisplay()
{
  // Clear The Background Color To Black
  glClearColor(0, 0, 0, 0);

  // Verify that we're drawing filled polys by default
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // Set smoothing on by default
  this->SetSmoothLines(true);
}

void
RenderWindow
::Resize(int cx, int cy)
{
  if(cx==0) // prevent div/0 (or it won't display)
    cx=1;
  if(cy==0) // prevent div/0
    cy=1;

  m_WindowSize.x = cx;
  m_WindowSize.y = cy;

  // create the viewport with the resized dimensions
  glViewport(0,0,cx,cy);
  
  // now change the clipping volume parameters
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  m_OrthoParams[0] = 0.0f; // left
  m_OrthoParams[1] = m_UnitsPerPixel * cx; // right
  m_OrthoParams[2] = 0.0f; // bottom
  m_OrthoParams[3] = m_UnitsPerPixel *cy; //top
  m_OrthoParams[4] = 1.0f; //near clipping
  m_OrthoParams[5] = -1.0f; //far clipping

  glOrtho(m_OrthoParams[0], m_OrthoParams[1], m_OrthoParams[2],
    m_OrthoParams[3], m_OrthoParams[4], m_OrthoParams[5]);

  glMatrixMode(GL_MODELVIEW);
}

void
RenderWindow
::Render()
{
  // First see if we're allowed to be doing this
  if(!m_IsOKToRender)
    return;

  // Next see if the display is set up
  if(!m_DisplayIsInitialized)
  {
    this->SetupDisplay();
    m_DisplayIsInitialized = true;
  }

  // Clear the frame buffer
  glClear(GL_COLOR_BUFFER_BIT);

  // load and reset the modelview matrix
//  glMatrixMode(GL_MODELVIEW);
//  glLoadIdentity();

  // Draw all of the gauges
//  glPushMatrix();	// hj ?
  this->RenderGauges();
//  glPopMatrix();	// hj ?

//  if(m_FrameTest) // NOTE: m_FrameTest not known outside ogcAppObject.cpp!
//    this->ShowFrameRate();

  // Clean up the rendering
  glFlush();
}

void
RenderWindow
::RenderGauges()
{
  GaugeIteratorType it;

  // Walk the list of gauges and render them
  for (it = m_GaugeList.begin(); it != m_GaugeList.end(); ++it)
  {
    (*it)->Render();
  }
}

void
RenderWindow
::AddGauge(Gauge* pGauge)
{
  m_GaugeList.push_back(pGauge);
  m_NumGauges++;
}

GLboolean
RenderWindow
::CheckExtension( char *extName )
{
  /*
  ** Search for extName in the extensions string.  Use of strstr()
  ** is not sufficient because extension names can be prefixes of
  ** other extension names.  Could use strtok() but the constant
  ** string returned by glGetString can be in read-only memory.
  */
  char *p = (char *) glGetString(GL_EXTENSIONS);
  char *end;
  int extNameLen;

  extNameLen = strlen(extName);
  end = p + strlen(p);
    
  while (p < end)
  {
    int n = strcspn(p, " ");
    if ((extNameLen == n) && (strncmp(extName, p, n) == 0))
    {
      return GL_TRUE;
    }
    p += (n + 1);
  }
  return GL_FALSE;
}

void
RenderWindow
::SetSmoothLines(bool flag)
{
  m_SmoothLines = flag;

  // Set line anti-aliasing if desired
  if(m_SmoothLines == true)
  {
    // Enable blending
    glEnable(GL_BLEND);

    // Select The Type Of Blending
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  
    // Enable line and polygon smoothing
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);

    // We want the prettiest smooth lines possible
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  }
  else
  {
    glDisable(GL_BLEND);
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_LINE_SMOOTH);
  }
}

void
RenderWindow
::CallBackKeyboardFunc(unsigned char key, int x, int y)
{

}

void
RenderWindow
::CallBackSpecialFunc(int key, int x, int y)
{
  
}

void
RenderWindow
::CallBackMouseFunc(int button, int state, int x, int y)
{
  // Since the coordinate system origin for the window is in the upper left
  // and that of our gauges is in the lower left, we need to invert y
  y = m_WindowSize.y - y;

  GaugeIteratorType it;

  // Walk the list of gauges and apply the mouse click
  for (it = m_GaugeList.begin(); it != m_GaugeList.end(); ++it)
  {
    (*it)->HandleMouseButton(button, state, x, y);
  }
}

// Send a message to member gauges, possibly take some local action
void
RenderWindow
::OnMessage(Message message, void* data)
{
  // ---
  // Add message handling code for the render window here
  // ---

  // Now pass the message to member gauges
  GaugeIteratorType it;

  for (it = m_GaugeList.begin(); it != m_GaugeList.end(); ++it)
  {
    (*it)->OnMessage(message, data);
  }
}

} // end namespace OpenGC

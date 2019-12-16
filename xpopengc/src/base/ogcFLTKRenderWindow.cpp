/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcFLTKRenderWindow.cpp,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:27:52 $
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

#include <FL/Fl.H>
#include "ogcFLTKRenderWindow.h"

extern int verbosity;

namespace OpenGC
{

FLTKRenderWindow
::FLTKRenderWindow(int X, int Y, int W, int H, const char *L) : Fl_Gl_Window(X, Y, W, H, L)
{
  if (verbosity > 0) printf("FLTKRenderWindow - constructing\n");

  // Safe to enable rendering
  m_IsOKToRender = true;

  if (verbosity > 1) printf("FLTKRenderWindow - constructed\n");
}

FLTKRenderWindow
::~FLTKRenderWindow()
{
}

void
FLTKRenderWindow
::draw()
{
  this->Render();
}

int
FLTKRenderWindow
::handle(int event)
{
  switch(event) {
  case FL_PUSH:
    //... mouse down event ...
    this->CallBackMouseFunc(0, 0, Fl::event_x(), Fl::event_y() );
    return 1;
  case FL_DRAG:
    //... mouse moved while down event ...
    return 1;
  case FL_RELEASE:   
    //... mouse up event ...
    this->CallBackMouseFunc(0, 1, Fl::event_x(), Fl::event_y() );
    return 1;
  case FL_FOCUS :
  case FL_UNFOCUS :
    //... Return 1 if you want keyboard events, 0 otherwise
    return 1;
  case FL_KEYBOARD:
    //... keypress, key is in Fl::event_key(), ascii in Fl::event_text()
    //... Return 1 if you understand/use the keyboard event, 0 otherwise...
    return 1;
  case FL_SHORTCUT:
    //... shortcut, key is in Fl::event_key(), ascii in Fl::event_text()
    //... Return 1 if you understand/use the shortcut event, 0 otherwise...
    return 1;
  default:
    // pass other events to the base class...
    return Fl_Gl_Window::handle(event);
  }
}

void
FLTKRenderWindow
::Render()
{
  // Check to see if any FL window stuff has changed
  if( !valid() )
  {
    this->Resize( w(), h() );
  }
  
  // Call the base class render to do everything else
  RenderWindow::Render();
}

} // end namespace OpenGC

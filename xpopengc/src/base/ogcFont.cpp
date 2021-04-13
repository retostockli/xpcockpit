/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcFont.cpp,v $

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

#include "ogcGLHeaders.h"
#include <stdio.h>

#include "ogcFont.h"

namespace OpenGC
{

Font
::Font()
{
  printf("Font constructed\n");

  // Default is a unit sized font
  m_Size.x = 1.0;
  m_Size.y = 1.0;
  m_Spacing = 1.0;

  m_PolygonFont = 0;
  //m_PixmapFont = 0;
  //m_TextureFont = 0;
  m_OutlineFont = 0;

  m_Name = new char[512];
}

Font
::~Font()
{

  if(m_PolygonFont != 0)
  {
    delete m_PolygonFont;
  }
  m_PolygonFont = 0;

  /*
  if(m_PixmapFont != 0)
  {
    delete m_PixmapFont;
  }
  m_PixmapFont = 0;
  */
  /*
  if(m_TextureFont != 0)
  {
    delete m_TextureFont;
  }
  m_TextureFont = 0;
  */
  
  if(m_OutlineFont != 0)
  {
    delete m_OutlineFont;
  }
  m_OutlineFont = 0;

  delete[] m_Name;
}

void
Font
::SetSize(double x, double y)
{
  m_Size.x = x;
  m_Size.y = y;
}

void
Font
::Print(double x, double y, const char *string)
{
  // Save the modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  
  // Set position and size
  glTranslated(x,y,0);
  glScaled(0.0135*m_Size.x, 0.0135*m_Size.y, 1);
  
  // Draw using the triangulated font
  m_PolygonFont->Render(string);
  //m_PixmapFont->Render(string);
  //m_TextureFont->Render(string);
  
  // Restore modelview matrix
  glPopMatrix();

  // Draw using the smoothing outline, if desired
  if(m_Smooth == true)
  {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    glTranslated(x,y,0);
    glScaled(0.0135*m_Size.x, 0.0135*m_Size.y, 1);
    
    glLineWidth(0.3);
    m_OutlineFont->Render(string);
    glLineWidth(1.0);
    
    glPopMatrix();
  }
 
}

bool
Font
::LoadFont(char* name)
{
  strcpy(m_Name, name);

  // Open the the font in both polygon and outline mode
  m_PolygonFont = new FTGLPolygonFont(name);
  //m_PixmapFont = new FTGLPixmapFont(name);
  //m_TextureFont = new FTGLTextureFont(name);
  m_OutlineFont = new FTGLOutlineFont(name);

  // The initial face size is large so that font sizing
  // will work correctly later on
  m_PolygonFont->FaceSize(100);
  //m_PixmapFont->FaceSize(100);
  //m_TextureFont->FaceSize(100);
  m_OutlineFont->FaceSize(100);
 
  // We succeeded at opening the fonts
  return true;
}

} // end namespace OpenGC

/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcFontManager.cpp,v $

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
#include <string.h>
#include "ogcFont.h"
#include "ogcFontManager.h"

extern int verbosity;

namespace OpenGC
{

FontManager
::FontManager()
{
  if (verbosity > 0) printf("FontManager constructed\n");

  m_NumFonts = 0;
  
  // Allocate space for the font paths
  m_NameWithPath = new char[512];
  m_FontPath = new char[128];
  
  // The default font path for all platforms is "/", which is most likely wrong
  strcpy( m_FontPath, "/" );

  // By default, the font is not smoothed
  m_Smooth = false;
}

FontManager
::~FontManager()
{
  delete[] m_NameWithPath;
  delete[] m_FontPath;

  // Clear calls the destructor on each list entry

  // Does not seem to work (FTPolygonFont and FTOutlineFont are not deleted). Delete however
  // works inside the constructor function. What is going wrong?
  for(int i = 0; i < m_NumFonts; i++) {
    delete m_FontList[i];
  }
  //  printf("BLABLA3\n");
  m_FontList.clear();

}

void
FontManager
::SetFontPath(char* fontPath)
{
  strcpy(m_FontPath, fontPath);
}

int
FontManager
::LoadDefaultFont()
{
  return this->LoadFont((char*) "Vera.ttf");
}

int
FontManager
::LoadFont(char* name)
{
  // Concatenate the font name onto the font path
  strcpy(m_NameWithPath, m_FontPath);
  strcat(m_NameWithPath, name);

  // First, check to see if the font is already loaded
  for(int i = 0; i < m_NumFonts; i++)
  {
    //char* loadedFont = m_FontList[i]->GetName();
     if (strcmp(m_NameWithPath, m_FontList[i]->GetName()) == 0)
    {
      return i;
    }
  }

  // Ok, the font isn't loaded, so create a new one
  Font* pFont = new Font;

  if( pFont->LoadFont(m_NameWithPath) )
  {    
    // Set whether or not to smooth the font
    pFont->SetSmooth(m_Smooth);

    // Add the font to the list
    m_FontList.push_back(pFont);
   
    // Update the number of stored fonts
    m_NumFonts++;
    
    // Return the index of the new font
    return (m_NumFonts - 1);
  }
  else
  {
    delete pFont;
    // Return the flag that means we failed
    return -1;
  }
}

void
FontManager
::SetSize(int font, double x, double y)
{
  if( (font >= 0) && (font < m_NumFonts) )
  {
    m_FontList[font]->SetSize(x,y);
  }
}

void
FontManager
::SetSpacing(int font, double spacing)
{
  if( (font >= 0) && (font < m_NumFonts) )
  {
    // does nothing (??)
  }
}

void
FontManager
::SetSmooth(bool flag)
{
  m_Smooth = flag;

  for(int i = 0; i < m_NumFonts; i++)
  {
    m_FontList[i]->SetSmooth(flag);
  }
}

void
FontManager
::Print(double x, double y, const char* text, int font)
{
  if( (font >= 0) && (font < m_NumFonts) )
  {
    m_FontList[font]->Print(x,y,text);
  }
}

} // end namespace OpenGC

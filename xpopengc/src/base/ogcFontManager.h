/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcFontManager.h,v $

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

/**
 * ogcFontManager allows sharing of fonts between different gauge
 * components; this is important so that memory-intensive fonts
 * are limited to a single instance in memory. If a gauge component
 * requests a font which is already present in memory, the font
 * is _not_ loaded again.
 *
 * Fonts are stored in a STL vector.
 *
 * Note: only one ogcFontManager should be used in an application!!
 * Nothing bad will happen if more than one is used, but it defeats
 * the purpose using it in the first place (i.e. memory conservation)
 */

#ifndef ogcFontManager_h
#define ogcFontManager_h

#include <vector> // STL vector include
#include "ogcFont.h"

namespace OpenGC
{

class FontManager  
{
public:
  FontManager();
  ~FontManager();

  /** Set the path for loading fonts */
  void SetFontPath(char* fontPath);
  
  /**
   * Request that we load the default gauge font
   * This is nice because we can globally change the default and
   * affect all of the gagues
   */
  int LoadDefaultFont();

  /**
   * Request that a new font be loaded; returns the index of the font
   * in the font list (vector)
   */
  int LoadFont(char* name);

  /** Set the size of a particular font in physical units */
  void SetSize(int font, double x, double y);

  /** Set the spacing between characters in fractions of a character width */
  void SetSpacing(int font, double spacing);

  /** Set whether or not to smooth */
  void SetSmooth(bool flag);

  /** Print a character string at location (x,y) using specified font */
  void Print(double x, double y, const char* text, int font);
  
protected:

  /** Path to Truetype fonts */
  char* m_FontPath;
  
  /**
    * String used to hold the value of the path concat'd
    * with the font name
    */
  char* m_NameWithPath;

  /** How many fonts are loaded */
  int m_NumFonts;

  /** TRUE if the fonts are smoothed using an anti-aliased outline */
  bool m_Smooth;

  /**
   * The vector that holds Font objects
   * We use a vector rather than a true linked list to allow random access
   * to fonts, rather than having to traverse the entire list each time
   */
  std::vector<Font*> m_FontList;
};

} // end namespace OpenGC

#endif // !defined(ogcFontMangager_h)

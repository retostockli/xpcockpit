/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcFont.h,v $

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

/**
 * Wraps the FTGL font toolkit to make its interface fit well into the
 * OpenGC environment. ogcFont's are accessed via the ogcFontManager object,
 * which all ogcRenderObject's contain a pointer to (thereby eliminating
 * redundant font storage).

 * FTGL is capable of reading any True-Type font, allowing the generation of
 * very high quality font renderings
 */

#ifndef ogcFont_h
#define ogcFont_h

#include "ogcOrderedPair.h"

// It's important to define this since we're linking against
// a static rather than shared version of FTGL
#define FTGL_LIBRARY_STATIC

#include <FTGL/ftgl.h>

namespace OpenGC
{

class Font
{
public:
  Font();
  ~Font();

  /** Load a font from a specified file */
  bool LoadFont(char* name);

  /** Print a character string at location (x,y) */
  void Print(double x, double y, const char *string);

  /**
    * Set the spacing of the font in fractional character widths
    * I.e. spacing = 0.5 means 1/2 character width
    */
  void SetSpacing(double spacing) {m_Spacing = spacing;};

  /** Set the size of the font (width and height) in physical units */
  void SetSize(double x, double y);

  /** Set whether or not to smooth */
  void SetSmooth(bool flag) {m_Smooth = flag;}

  /** Get the font name */
  char* GetName() {return m_Name;}
  
protected:

  /** The name of the font */
  char* m_Name;

  /** Spacing between characters in the font, in fractional character widths */
  double m_Spacing;

  /** The size of the font in physical units */
  OrderedPair<double> m_Size;

  /** True if the font is smoothed using an anti-aliased outline */
  bool m_Smooth;

  /** The triangulated FTGL font object */
  FTPolygonFont* m_PolygonFont;

  /** The pixmap FTGL font object */
  FTPixmapFont* m_PixmapFont;

  /** The texture FTGL font object */
  FTTextureFont* m_TextureFont;

  /** The outline FTGL font object used for smoothing */
  FTOutlineFont* m_OutlineFont;

};

} // end namespace OpenGC

#endif // !defined(ogcFont_h)

/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737NAVDrawWXR.h,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:10 $
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
 * Test gauge component for nav stuff
 */

#define NEWWXR

#ifndef ogcB737NAVDrawWXR_h
#define ogcB737NAVDrawWXR_h

#include "ogcGauge.h"
#include "ogcB737NAV.h"

#ifdef NEWWXR
#define TEX_WIDTH 61 // total radar pixels in horizontal direction
#define TEX_HEIGHT 61 // total radar pixels in vertical direction
#define MPP 0.5   // miles per radar pixel
#else
#define TEX_WIDTH 256 // total radar pixels in horizontal direction
#define TEX_HEIGHT 256 // total radar pixels in vertical direction
#define MPP 0.5   // miles per radar pixel
#endif
namespace OpenGC
{

class B737NAVDrawWXR : public GaugeComponent
{
public:

  B737NAVDrawWXR();
  virtual ~B737NAVDrawWXR();

  void Render();
  
  void SetNAVGauge(B737NAV* NAVGauge) {m_NAVGauge = NAVGauge;}
  
protected:
  
  /** The font number provided to us by the font manager */
  int m_Font;

  /* Pointer to the calling Navigation Gauge */
  B737NAV* m_NAVGauge;

  /* number of longitudes and latitudes of weather radar to store */
  int wxrnlon;
  int wxrnlat;
  
  GLubyte texture[TEX_HEIGHT][TEX_WIDTH][4];             
  // float texture[TEX_HEIGHT][TEX_WIDTH][4];             
  
  
};

} // end namespace OpenGC

#endif

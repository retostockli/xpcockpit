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


#ifndef ogcB737NAVDrawWXR_h
#define ogcB737NAVDrawWXR_h

#include "ogcGauge.h"
#include "ogcB737NAV.h"

  /* number of longitudes and latitudes of weather radar to store */
#define NLON 5
#define NLAT 5
#define MINPERLON 60
#define MINPERLAT 60
#define TEX_WIDTH MINPERLON * NLON + 1  // total radar pixels in horizontal direction
#define TEX_HEIGHT MINPERLAT * NLAT + 1 // total radar pixels in vertical direction
#define MPP 111.0 / 60.0 / 1.852 // miles per radar pixel. Each pixel is one arc minute. Each degree lat is 111 km apart and each mile is 1.852 km
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

  GLubyte lltexture[TEX_HEIGHT][TEX_WIDTH];
  
  GLubyte texture[TEX_HEIGHT][TEX_WIDTH][4];             
  // float texture[TEX_HEIGHT][TEX_WIDTH][4];             
  
  int m_OldLat;     /* check if incoming southern latitude starts from scratch in southern edge of radar image */
  int m_CenterLon;  /* western edge of center pixel of radar image */
  int m_CenterLat;  /* southern edge of center pixel of radar image */
  float m_TextureCenterLon; /* exact center lon of radar image */
  float m_TextureCenterLat; /* exact center lat of radar image */
  
  
};

} // end namespace OpenGC

#endif

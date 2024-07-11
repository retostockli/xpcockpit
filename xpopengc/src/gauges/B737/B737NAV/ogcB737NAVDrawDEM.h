/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Copyright (c) 2024 Reto Stockli

  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/**
 * Test gauge component for nav stuff
 */


#ifndef ogcB737NAVDrawDEM_h
#define ogcB737NAVDrawDEM_h

#include "ogcGauge.h"
#include "ogcB737NAV.h"

namespace OpenGC
{

class B737NAVDrawDEM : public GaugeComponent
{
public:

  B737NAVDrawDEM();
  virtual ~B737NAVDrawDEM();
  
  void Render();
  
  void SetNAVGauge(B737NAV* NAVGauge) {m_NAVGauge = NAVGauge;}
  
protected:
  
  /** The font number provided to us by the font manager */
  int m_Font;

  /* Pointer to the calling Navigation Gauge */
  B737NAV* m_NAVGauge;
  
  /* RGBA Array for later OpenGL Rendering
     Use single array instead of 3D array since it has to be contiguous memory */
  unsigned char *dem_image;
  float **dem_lon;
  float **dem_lat;

  int m_dem_lonmin;
  int m_dem_lonmax;
  int m_dem_latmin;
  int m_dem_latmax;

  unsigned int m_texture;  /* Texture Storage */
  
};

} // end namespace OpenGC

#endif

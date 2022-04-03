/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef ogcDrawWXR_h
#define ogcDrawWXR_h

#include "ogcGauge.h"
#include "ogcWXR.h"

namespace OpenGC
{

class DrawWXR : public GaugeComponent
{
public:

  DrawWXR();
  virtual ~DrawWXR();
  
  void Render();
  
  void SetWXRGauge(WXR* WXRGauge) {m_WXRGauge = WXRGauge;}
  
protected:
  
  /** The font number provided to us by the font manager */
  int m_Font;

  /* Pointer to the calling Navigation Gauge */
  WXR* m_WXRGauge;
  
  /* RGBA Array for later OpenGL Rendering
     Use single array instead of 3D array since it has to be contiguous memory */
  unsigned char *wxr_image;

  int m_wxr_ncol;
  int m_wxr_nlin;
  
};

} // end namespace OpenGC

#endif

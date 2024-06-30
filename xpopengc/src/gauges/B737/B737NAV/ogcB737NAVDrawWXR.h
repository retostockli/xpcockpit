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


#ifndef ogcB737NAVDrawWXR_h
#define ogcB737NAVDrawWXR_h

#define NUM_HIST 20

#include "ogcGauge.h"
#include "ogcB737NAV.h"

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
  
  /* RGBA Array for later OpenGL Rendering
     Use single array instead of 3D array since it has to be contiguous memory */
  unsigned char *wxr_image;

  int m_wxr_ncol;
  int m_wxr_nlin;

  unsigned int m_texture;  /* Texture Storage */

  float m_wxr_gain[NUM_HIST];
  float m_wxr_tilt[NUM_HIST];

  float Mean(int N, float data[])
  {
    // variable to store sum of the given data
    float sum = 0;
    for (int i = 0; i < N; i++) {
      sum += data[i];
    }
    
    // calculating mean
    return sum / N;
  }
  
  float Variance(int N, float data[])
  {
    // variable to store sum of the given data
    float sum = 0;
    for (int i = 0; i < N; i++) {
      sum += data[i];
    }
    
    // calculating mean
    float mean = sum / N;
    
    // temporary variable to store the summation of square
    // of difference between individual data items and mean
    float values = 0;
    
    for (int i = 0; i < N; i++) {
      values += pow(data[i] - mean, 2);
    }
    
    // variance is the square of standard deviation
    return values / N;

 
  }
  
  float standardDeviation(int N, float data[])
  {

    // calculating standard deviation by finding square root
    // of variance
    return sqrt(Variance(N,data));
    
  }
  
};

} // end namespace OpenGC

#endif

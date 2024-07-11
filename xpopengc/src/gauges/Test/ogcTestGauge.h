/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
    
  2024 Reto Stockli

  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/**
 * Test gauge used for testing Features
 */

#ifndef ogcTestGauge_h
#define ogcTestGauge_h

#include "ogcGauge.h"

extern int verbosity;

namespace OpenGC
{

class TestGauge : public Gauge  
{
public:

  TestGauge();
  virtual ~TestGauge();

  void Render();

protected:
  
  /** Font from the font manager */
  int m_Font;
  int m_Font2;
  
};

} // end namespace OpenGC

#endif

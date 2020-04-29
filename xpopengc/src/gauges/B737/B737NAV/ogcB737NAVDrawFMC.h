/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737MapModeExpanded.h,v $

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

#ifndef ogcB737NAVDrawFMC_h
#define ogcB737NAVDrawFMC_h

#include "ogcGauge.h"
#include "ogcB737NAV.h"

namespace OpenGC
{

class B737NAVDrawFMC : public GaugeComponent
{
public:

  B737NAVDrawFMC();
  virtual ~B737NAVDrawFMC();

  void Render();

  void SetNAVGauge(B737NAV* NAVGauge) {m_NAVGauge = NAVGauge;}

protected:
  
  /** The font number provided to us by the font manager */
  int m_Font;

  /* Pointer to the calling Navigation Gauge */
  B737NAV* m_NAVGauge;

  /* Structure holding FMC waypoints */
  struct wptstruct {
    char name[40];
    char pth[3];
    float lon;
    float lat;
    int alt;
    float rad_ctr_lon;
    float rad_ctr_lat;
    float rad_lon2;
    float rad_lat2;
    float rad_radius;
    float rad_dta;
    int rad_turn;
    int turn;
    int hold_time;
    float brg;
    float crs;
  };

  wptstruct *wpt;
  int nwpt;

};

} // end namespace OpenGC

#endif

/*=============================================================================

  This is the ogcSkeletonGauge.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  Created:
    Date:   2015-05-05
    Author: Hans Jansen

  Copyright (C) 2015      Hans Jansen (hansjansen@users.sourceforge.net)
  and/or                  Reto Stöckli (stockli@users.sourceforge.net)

  This program is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>. 

=============================================================================

  The OpenGC subproject has been derived from:
    OpenGC - The Open Source Glass Cockpit Project
    Copyright (c) 2001-2003 Damion Shelton

  This file is part of the xpiocards repository; it has NOT been incorporated
  in the build structure, as it is only an example for defining other gauges!

=============================================================================*/

#include <stdio.h>
#include <math.h>

#include "ogcGaugeComponent.h"
#include "ogcSkeletonGauge.h"
#include "ogcSkeletonGaugeComponent.h"

namespace OpenGC
{

  /** The Subwindows */
  SkeletonGaugeComponent* sgComp;

  SkeletonGauge::SkeletonGauge()
  {
    if (verbosity > 0) printf("SkeletonGauge - constructing\n");

    // Specify our physical size
    m_PhysicalSize.x = 460;
    m_PhysicalSize.y = 460;

    // We want to draw an outline
    this->SetGaugeOutline(true);

    // A Conponent of the Gauge
    sgComp = new SkeletonGaugeComponent();
    sgComp->SetParentRenderObject(this);
    sgComp->SetPosition(0,0);
    sgComp->SetSize(m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent(sgComp);

    if (verbosity > 1) printf("SkeletonGauge - constructed\n");
  }

  SkeletonGauge::~SkeletonGauge()
  {
  }

  void SkeletonGauge::Render()
  {
    Gauge::Render();

    if (verbosity > 1)
    {
      printf ("SkeletonGauge - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("SkeletonGauge -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("SkeletonGauge -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("SkeletonGauge -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }
  }

} // end namespace OpenGC

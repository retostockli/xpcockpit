/*=============================================================================

  This is the ogcSkeletonGaugeComponent.c file, part of the OpenGC subproject
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
#include "ogcCircleEvaluator.h"

namespace OpenGC
{

  SkeletonGaugeComponent::SkeletonGaugeComponent()
  {
    if (verbosity > 0) printf("SkeletonGaugeComponent - constructing\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf("SkeletonGaugeComponent - constructed\n");
  }

  SkeletonGaugeComponent::~SkeletonGaugeComponent()
  {
  }

  void SkeletonGaugeComponent::Render()
  {
    // Call base class to setup for size and position
    GaugeComponent::Render();

    // For drawing circles
    CircleEvaluator aCircle;

    float fontSize = 12;

    // (Re-)Define the datarefs we want to use (just a few examples here)
    int *zuluHours   = link_dataref_int("sim/cockpit2/clock_timer/zulu_time_hours");
    int *zuluMinutes = link_dataref_int("sim/cockpit2/clock_timer/zulu_time_minutes");
    int *zuluSeconds = link_dataref_int("sim/cockpit2/clock_timer/zulu_time_seconds");

    if (verbosity > 1)
    {
      printf ("SkeletonGaugeComponent - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("SkeletonGaugeComponent -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("SkeletonGaugeComponent -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("SkeletonGaugeComponent -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    double partSize = m_PhysicalSize.x;	// defines total component size (just for name simplification) (square gauge!)
    double partCenter = partSize / 2;		// defines component center

    m_pFontManager->SetSize(m_Font, fontSize, fontSize);

    glPushMatrix();

    // do any rendering here, using dataref values like this:
    char timeString[10];
    sprintf(timeString, "Zulu time now: %i:%i:%i", *zuluHours, *zuluMinutes, *zuluSeconds);
    m_pFontManager->Print( partCenter - d_w, partCenter - d_h - d_r  - d_f, timeString, m_Font );

    glPopMatrix();
}

} // end namespace OpenGC

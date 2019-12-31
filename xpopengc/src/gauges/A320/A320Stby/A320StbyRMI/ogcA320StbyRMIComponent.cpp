/*=============================================================================

  This is the ogcA320StbyRMIComponent.c file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  Created:
    Date:   2018-05-03
    Author: Hans Jansen

  Copyright (C) 2018      Hans Jansen (hansjansen@users.sourceforge.net)
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

=============================================================================*/

#include <stdio.h>
#include <math.h>

#include "ogcGaugeComponent.h"
#include "ogcA320StbyRMI.h"
#include "ogcA320StbyRMIComponent.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320StbyRMIComponent::A320StbyRMIComponent () {
  
    if (verbosity > 0) printf ("A320StbyRMIComponent - constructing\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 1) printf ("A320StbyRMIComponent - constructed\n");
  }

  A320StbyRMIComponent::~A320StbyRMIComponent () {}

  // Handle the cold&dark state for the display
  int coldDarkStRmi = 2;

  void A320StbyRMIComponent::Render () {
  
    // Call base class to setup for size and position
    GaugeComponent::Render ();

    // For drawing circles
    CircleEvaluator aCircle;

    float fontSize = 12;

    // Request the datarefs we want to use
    
    // Note: this dataref is maintained by the a320_overhead.c module
    int *cold_dark_st_rmi = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_dark_st_rmi == INT_MISS) coldDarkStRmi = 2; else coldDarkStRmi = *cold_dark_st_rmi;
    if (coldDarkStRmi == 0) {

      if (verbosity > 1) {
        printf ("A320StbyRMIComponent - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
        printf ("A320StbyRMIComponent -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
        printf ("A320StbyRMIComponent -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
        printf ("A320StbyRMIComponent -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
      }

      double partSize = m_PhysicalSize.x;	// defines total component size (just for name simplification) (square gauge!)
      double partCenter = partSize / 2;	// defines component center
    
      int d_r  = (int) partSize * 0.35;
      int d_rx = (int) d_r * 0.82;
      int d_ry = (int) d_r * 0.5;
      int d_h  = (int) fontSize * 0.5;
      int d_f  = (int) fontSize * 0.125;
      int d_w  = (int) fontSize;        // width of a single character (non-proportional font!)

      m_pFontManager->SetSize (m_Font, fontSize, fontSize);

      glPushMatrix ();

        // do any rendering here
        glColor3ub(COLOR_MAGENTA);
        m_pFontManager->Print (partCenter - 2 * d_w, partCenter, "RMI", m_Font);

      glPopMatrix ();

    } // end if (! coldDarkStRmi)

  } // end Render ()

} // end namespace OpenGC


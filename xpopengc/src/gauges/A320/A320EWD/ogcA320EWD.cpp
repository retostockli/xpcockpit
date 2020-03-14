/*=============================================================================

  This is the A320EWD.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Engine/Warning Display ===

  Created:
    Date:        2011-11-14
    Author:      Hans Jansen
    last change: 2020-02-06
    (see ogcSkeletonGauge.cpp for more details)

  Copyright (C) 2011-2020 Hans Jansen (hansjansen@users.sourceforge.net)
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

===============================================================================

  The OpenGC subproject has been derived from:
    OpenGC - The Open Source Glass Cockpit Project
    Copyright (c) 2001-2003 Damion Shelton

=============================================================================*/

#include <stdio.h>

#include "ogcDataSource.h"
#include "ogcCircleEvaluator.h"

#include "ogcA320EWD.h"
#include "ogcA320EWDBackground.h"
#include "ogcA320EWDEPR.h"
#include "ogcA320EWDRev.h"
#include "ogcA320EWDEGT.h"
#include "ogcA320EWDFF.h"
#include "ogcA320EWDN1.h"
#include "ogcA320EWDN2.h"
#include "ogcA320EWDFuel.h"
#include "ogcA320EWDSlatsFlaps.h"
#include "ogcA320EWDAnnunciator.h"

namespace OpenGC {

  /** The Subwindows */

  A320EWD::A320EWD() {
    if (verbosity > 1) printf ("A320EWD constructing\n");

    // Specify our physical size and rotation
    m_PhysicalSize.x = 460;
    m_PhysicalSize.y = 460;

    // We do not want to draw an outline
    this->SetGaugeOutline (false);

    // Create the background (to be painted first!)
    A320EWDBackground* pEWDBkgnd = new A320EWDBackground ();
    pEWDBkgnd->SetParentRenderObject (this);
    pEWDBkgnd->SetPosition (0, 0);
    pEWDBkgnd->SetSize (m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent (pEWDBkgnd);

    // Create the EPR dials
    A320EWDEPR* pEWDEPR1 = new A320EWDEPR ();
    pEWDEPR1->SetParentRenderObject (this);
    pEWDEPR1->SetPosition (100, 350);
    pEWDEPR1->SetSize (140, 100);
    pEWDEPR1->setEngine (1);
    this->AddGaugeComponent (pEWDEPR1);

    A320EWDEPR* pEWDEPR2 = new A320EWDEPR ();
    pEWDEPR2->SetParentRenderObject (this);
    pEWDEPR2->SetPosition (250, 350);
    pEWDEPR2->SetSize (140, 100);
    pEWDEPR2->setEngine (2);
    this->AddGaugeComponent (pEWDEPR2);

    // Create the Reverser Indicators
    A320EWDRev* pEWDRev1 = new A320EWDRev ();
    pEWDRev1->SetParentRenderObject (this);
    pEWDRev1->SetPosition (161, 398);
    pEWDRev1->SetSize (32, 16);
    pEWDRev1->setEngine (1);
    this->AddGaugeComponent (pEWDRev1);

    A320EWDRev* pEWDRev2 = new A320EWDRev ();
    pEWDRev2->SetParentRenderObject (this);
    pEWDRev2->SetPosition (311, 398);
    pEWDRev2->SetSize (32, 16);
    pEWDRev2->setEngine (2);
    this->AddGaugeComponent (pEWDRev2);

    // Create the EGT dials
    A320EWDEGT* pEWDEGT1 = new A320EWDEGT ();
    pEWDEGT1->SetParentRenderObject (this);
    pEWDEGT1->SetPosition (110, 300);
    pEWDEGT1->SetSize (100, 50);
    pEWDEGT1->setEngine (1);
    this->AddGaugeComponent (pEWDEGT1);

    A320EWDEGT* pEWDEGT2 = new A320EWDEGT ();
    pEWDEGT2->SetParentRenderObject (this);
    pEWDEGT2->SetPosition (260, 300);
    pEWDEGT2->SetSize (100, 50);
    pEWDEGT2->setEngine (2);
    this->AddGaugeComponent (pEWDEGT2);

    // Create the FF Indicators
    A320EWDFF* pEWDFF1 = new A320EWDFF ();
    pEWDFF1->SetParentRenderObject (this);
    pEWDFF1->SetPosition (40, 300);
    pEWDFF1->SetSize (48, 20);
    pEWDFF1->setEngine (1);
    this->AddGaugeComponent (pEWDFF1);

    A320EWDFF* pEWDFF2 = new A320EWDFF ();
    pEWDFF2->SetParentRenderObject (this);
    pEWDFF2->SetPosition (395, 300);
    pEWDFF2->SetSize (48, 20);
    pEWDFF2->setEngine (2);
    this->AddGaugeComponent (pEWDFF2);

    // Create the N1 dials
    A320EWDN1* pEWDN1_1 = new A320EWDN1 ();
    pEWDN1_1->SetParentRenderObject (this);
    pEWDN1_1->SetPosition (100, 200);
    pEWDN1_1->SetSize (120, 100);
    pEWDN1_1->setEngine (1);
    this->AddGaugeComponent (pEWDN1_1);

    A320EWDN1* pEWDN1_2 = new A320EWDN1 ();
    pEWDN1_2->SetParentRenderObject (this);
    pEWDN1_2->SetPosition (250, 200);
    pEWDN1_2->SetSize (120, 100);
    pEWDN1_2->setEngine (2);
    this->AddGaugeComponent (pEWDN1_2);

    // Create the N2 Indicators
    A320EWDN2* pEWDN2_1 = new A320EWDN2 ();
    pEWDN2_1->SetParentRenderObject (this);
    pEWDN2_1->SetPosition (40, 220);
    pEWDN2_1->SetSize (45, 20);
    pEWDN2_1->setEngine (1);
    this->AddGaugeComponent (pEWDN2_1);

    A320EWDN2* pEWDN2_2 = new A320EWDN2 ();
    pEWDN2_2->SetParentRenderObject (this);
    pEWDN2_2->SetPosition (400, 220);
    pEWDN2_2->SetSize (45, 20);
    pEWDN2_2->setEngine (2);
    this->AddGaugeComponent (pEWDN2_2);

   // Create the FOB Indicator
    A320EWDFuel* pEWDFuel = new A320EWDFuel ();
    pEWDFuel->SetParentRenderObject (this);
    pEWDFuel->SetPosition (5, 155);
    pEWDFuel->SetSize (140, 22);
    this->AddGaugeComponent (pEWDFuel);

   // Create the Slats/Flaps Indicator
    A320EWDSlatsFlaps* pEWDSlatsFlaps = new A320EWDSlatsFlaps ();
    pEWDSlatsFlaps->SetParentRenderObject (this);
    pEWDSlatsFlaps->SetPosition (220, 155);
    pEWDSlatsFlaps->SetSize (220, 45);
    this->AddGaugeComponent (pEWDSlatsFlaps);

    // Create the Annunciator field
    A320EWDAnnunciator* pEWDAnnunciator = new A320EWDAnnunciator ();
    pEWDAnnunciator->SetParentRenderObject (this);
    pEWDAnnunciator->SetPosition (5, 5);
    pEWDAnnunciator->SetSize (450, 140);
    this->AddGaugeComponent (pEWDAnnunciator);

    if (verbosity > 0) printf("A320EWD - constructed\n");
  }

  A320EWD::~A320EWD () {}

  void A320EWD::Render () {
    Gauge::Render ();

    if (verbosity > 2) {
      printf ("A320EWD - Physical position: x %f, y %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320EWD - Pixel position:    x %i, y %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320EWD - Physical size:     x %f, y %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320EWD - Pixel size:        x %i, y %i\n", m_PixelSize.x, m_PixelSize.y);
    }

  } // end Render ()

} // end namespace OpenGC

/*=============================================================================

  This is the ogcA320PFD.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Primary Flight Display ===

  Created:
    Date:        2011-11-14
    Author:      Hans Jansen
    last change: 2020-02-04
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
#include "ogcA320PFDFMA.h"
#include "ogcA320PFDAttitude.h"
#include "ogcA320PFDAltitudeTape.h"
#include "ogcA320PFDAltitudeTicker.h"
#include "ogcA320PFDGlideSlopeScale.h"
#include "ogcA320PFDSpeedTape.h"
#include "ogcA320PFDHeadingTape.h"
#include "ogcA320PFDLocalizerScale.h"
#include "ogcA320PFDVSI.h"
#include "ogcA320PFDBackground.h"
#include "ogcA320PFD.h"
#include "ogcCircleEvaluator.h"
#include "ogcA320PFDMiscInfo.h"

namespace OpenGC
{

  /** The Subwindows */
  A320PFDBackground* pBackground;
  A320PFDFMA* pFMA;
  A320PFDAttitude* pAttitude;
  A320PFDHeadingTape* pHeadingTape;
  A320PFDLocalizerScale* pLocScale;
  A320PFDAltitudeTape* pAltTape;
  A320PFDAltitudeTicker* pAltTicker;
  A320PFDGlideSlopeScale* pGsScale;
  A320PFDVSI* pVSI;
  A320PFDSpeedTape* pSpeedTape;
  A320PFDMiscInfo* pMisc;

  A320PFD::A320PFD()
  {
    if (verbosity > 0) printf("A320PFD - constructing\n");

    // Specify our physical size and rotation
    m_PhysicalSize.x = 460;
    m_PhysicalSize.y = 460;
    m_GaugeRotation = 0;

    // We do not want to draw an outline
    this->SetGaugeOutline(false);

    // The background of the PFD
    // NOTE: this subgauge must be painted first on the PFD!
    pBackground = new A320PFDBackground();
    pBackground->SetParentRenderObject(this);
    pBackground->SetPosition(0, 0);
    pBackground->SetSize(m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent(pBackground);

    // Create the Flight Mode Annunciator
    pFMA = new A320PFDFMA();
    pFMA->SetParentRenderObject(this);
    pFMA->SetPosition(10,380);
    pFMA->SetSize(440,75);
    this->AddGaugeComponent(pFMA);

    // Create an attitude indicator
    pAttitude = new A320PFDAttitude();
    pAttitude->SetParentRenderObject(this);
    pAttitude->SetPosition(80,110);
    pAttitude->SetSize(240,250);
    this->AddGaugeComponent(pAttitude);

    // Create a heading indicator
    pHeadingTape = new A320PFDHeadingTape();
    pHeadingTape->SetParentRenderObject(this);
    pHeadingTape->SetPosition(60,10);
    pHeadingTape->SetSize(280,70);
    this->AddGaugeComponent(pHeadingTape);

    // Create an altitude tape
    pAltTape = new A320PFDAltitudeTape();
    pAltTape->SetParentRenderObject(this);
    pAltTape->SetPosition(350,90);
    pAltTape->SetSize(80,290);
    this->AddGaugeComponent(pAltTape);

    // Create an altitude ticker
    pAltTicker = new A320PFDAltitudeTicker();
    pAltTicker->SetParentRenderObject(this);
    pAltTicker->SetPosition(360,207);
    pAltTicker->SetSize(70,50);
    this->AddGaugeComponent(pAltTicker);

    // Create a VSI
    pVSI = new A320PFDVSI();
    pVSI->SetParentRenderObject(this);
    pVSI->SetPosition(430,110);
    pVSI->SetSize(30,250);
    this->AddGaugeComponent(pVSI);

    // Create a speed tape
    pSpeedTape = new A320PFDSpeedTape();
    pSpeedTape->SetParentRenderObject(this);
    pSpeedTape->SetPosition(10,90);
    pSpeedTape->SetSize(70,290);
    this->AddGaugeComponent(pSpeedTape);

    // Create a localizer scale
    pLocScale = new A320PFDLocalizerScale();
    pLocScale->SetParentRenderObject(this);
    pLocScale->SetPosition(80,80);
    pLocScale->SetSize(240,30);
    this->AddGaugeComponent(pLocScale);

    // Create a glide-slope scale
    pGsScale = new A320PFDGlideSlopeScale();
    pGsScale->SetParentRenderObject(this);
    pGsScale->SetPosition(320,115);
    pGsScale->SetSize(30,240);
    this->AddGaugeComponent(pGsScale);

    // Create all other messages and indicators on the PDF
    // NOTE: this subgauge must be painted last on the PFD!
    pMisc = new A320PFDMiscInfo();
    pMisc->SetParentRenderObject(this);
    pMisc->SetPosition(0,0);
    pMisc->SetSize(m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent(pMisc);

    if (verbosity > 1) printf("A320PFD - constructed\n");
  }

  A320PFD::~A320PFD()
  {
    // Destruction handled by base class
  }

  void A320PFD::Render()
  {
    Gauge::Render();

    if (verbosity > 1)
    {
      printf("A320PFD - Physical position: x %f, y %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf("A320PFD - Pixel position:    x %i, y %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf("A320PFD - Physical size:     x %f, y %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf("A320PFD - Pixel size:        x %i, y %i\n", m_PixelSize.x, m_PixelSize.y);
    }

  }
} // end namespace OpenGC


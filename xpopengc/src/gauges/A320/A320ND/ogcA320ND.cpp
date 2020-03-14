/*=============================================================================

  This is the ogcA320ND.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Navigation Display ===
       (based on Reto's ogcB737NAV stuff)

  Created:
    Date:   2011-11-14
    Author: Hans Jansen
    last change: 2020-02-01
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
#include <math.h>

#include "ogcGauge.h"
#include "ogcDataSource.h"
#include "ogcCircleEvaluator.h"

#include "ogcA320ND.h"
#include "ogcA320NDWidget.h"
#include "ogcA320NDArc.h"
#include "ogcA320NDIls.h"
#include "ogcA320NDNav.h"
#include "ogcA320NDPlan.h"
#include "ogcA320NDVor.h"

namespace OpenGC {

  /** The Subwindows */
  A320NDArc* NDArcWidget;
  A320NDIls* NDIlsWidget;
  A320NDNav* NDNavWidget;
  A320NDPlan* NDPlanWidget;
  A320NDVor* NDVorWidget;
  A320NDWidget* NDWidget;

  A320ND::A320ND () {
    if (verbosity > 1) printf ("A320ND constructing\n");

    // Specify our physical size
    m_PhysicalSize.x = 500; // should be 460, i.e. scaled by 0.92!
    m_PhysicalSize.y = 500;

    // We do not want to draw an outline
    this->SetGaugeOutline (false);

    // Create the widgets
    NDArcWidget = new A320NDArc ();
    NDArcWidget->SetParentRenderObject (this);
    NDArcWidget->SetPosition (0, 0);
    NDArcWidget->SetSize (m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent (NDArcWidget);

    NDIlsWidget = new A320NDIls ();
    NDIlsWidget->SetParentRenderObject (this);
    NDIlsWidget->SetPosition (0, 0);
    NDIlsWidget->SetSize (m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent (NDIlsWidget);

    NDNavWidget = new A320NDNav ();
    NDNavWidget->SetParentRenderObject (this);
    NDNavWidget->SetPosition (0, 0);
    NDNavWidget->SetSize (m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent (NDNavWidget);

    NDPlanWidget = new A320NDPlan ();
    NDPlanWidget->SetParentRenderObject (this);
    NDPlanWidget->SetPosition (0, 0);
    NDPlanWidget->SetSize (m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent (NDPlanWidget);

    NDVorWidget = new A320NDVor ();
    NDVorWidget->SetParentRenderObject (this);
    NDVorWidget->SetPosition (0, 0);
    NDVorWidget->SetSize (m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent (NDVorWidget);

    NDWidget = new A320NDWidget ();
    NDWidget->SetParentRenderObject (this);
    NDWidget->SetPosition (0, 0);
    NDWidget->SetSize (m_PhysicalSize.x, m_PhysicalSize.y);
    this->AddGaugeComponent (NDWidget);

    if (verbosity > 0) printf("A320ND - constructed\n");
  }

  A320ND::~A320ND () {}

  void A320ND::Render ()
  {
    // Call base class to setup for size and position
    Gauge::Render ();

    if (verbosity > 2)
    {
      printf ("A320ND - Physical position: x %f, y %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320ND - Pixel position:    x %i, y %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320ND - Physical size:     x %f, y %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320ND - Pixel size:        x %i, y %i\n", m_PixelSize.x, m_PixelSize.y);
    }

  } // end Render()
  
} // end namespace OpenGC

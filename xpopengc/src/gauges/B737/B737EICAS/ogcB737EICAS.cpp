/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737EICAS.cpp,v $

  Copyright (C) 2001-2015 by:
  Original author:
  Michael DeFeyter
  Contributors (in alphabetical order):
  Reto Stockli

  Last modification:
  Date:      $Date: 2015/09/12 $
  Version:   $Revision: $
  Author:    $Author: stockli $
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  =========================================================================*/

#include "B737/B737EICAS/ogcB737EICAS.h"
#include "B737/B737EICAS/ogcB737EICASN1.h"
#include "B737/B737EICAS/ogcB737EICASEGT.h"
#include "B737/B737EICAS/ogcB737EICASN2.h"
#include "B737/B737EICAS/ogcB737EICASFF.h"
#include "B737/B737EICAS/ogcB737EICASOilT.h"
#include "B737/B737EICAS/ogcB737EICASOilP.h"
#include "B737/B737EICAS/ogcB737EICASOilQ.h"
#include "B737/B737EICAS/ogcB737EICASVib.h"
#include "B737/B737EICAS/ogcB737EICASHydP.h"
#include "B737/B737EICAS/ogcB737EICASHydQ.h"
#include "B737/B737EICAS/ogcB737EICASFuelQ.h"
#include "B737/B737EICAS/ogcB737EICASBackground.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace OpenGC
{

  B737EICAS::B737EICAS()
  {
    printf("B737EICAS constructed\n");

    m_PhysicalSize.x = 200;
    m_PhysicalSize.y = 200;
	
    // We want to draw an outline
    this->SetGaugeOutline(true);

    B737EICASN1* N1_1 = new B737EICASN1();
    N1_1->SetParentRenderObject(this);
    N1_1->SetPosition(8, 145);
    N1_1->setCurrentEngine(1);
    this->AddGaugeComponent(N1_1);

    B737EICASN1* N1_2 = new B737EICASN1();
    N1_2->SetParentRenderObject(this);
    N1_2->SetPosition(64, 145);
    N1_2->setCurrentEngine(2);
    this->AddGaugeComponent(N1_2);

    B737EICASEGT* EGT_1 = new B737EICASEGT();
    EGT_1->SetParentRenderObject(this);
    EGT_1->SetPosition(8, 110);
    EGT_1->setCurrentEngine(1);
    this->AddGaugeComponent(EGT_1);

    B737EICASEGT* EGT_2 = new B737EICASEGT();
    EGT_2->SetParentRenderObject(this);
    EGT_2->SetPosition(64, 110);
    EGT_2->setCurrentEngine(2);
    this->AddGaugeComponent(EGT_2);

    B737EICASN2* N2_1 = new B737EICASN2();
    N2_1->SetParentRenderObject(this);
    N2_1->SetPosition(8, 75);
    N2_1->setCurrentEngine(1);
    this->AddGaugeComponent(N2_1);

    B737EICASN2* N2_2 = new B737EICASN2();
    N2_2->SetParentRenderObject(this);
    N2_2->SetPosition(64, 75);
    N2_2->setCurrentEngine(2);
    this->AddGaugeComponent(N2_2);

    B737EICASFF* FF_1 = new B737EICASFF();
    FF_1->SetParentRenderObject(this);
    FF_1->SetPosition(8, 42);
    FF_1->setCurrentEngine(1);
    this->AddGaugeComponent(FF_1);

    B737EICASFF* FF_2 = new B737EICASFF();
    FF_2->SetParentRenderObject(this);
    FF_2->SetPosition(64, 42);
    FF_2->setCurrentEngine(2);
    this->AddGaugeComponent(FF_2);

    B737EICASOilP* OilP_1 = new B737EICASOilP();
    OilP_1->SetParentRenderObject(this);
    OilP_1->SetPosition(120, 131);
    OilP_1->setCurrentEngine(1);
    this->AddGaugeComponent(OilP_1);

    B737EICASOilP* OilP_2 = new B737EICASOilP();
    OilP_2->SetParentRenderObject(this);
    OilP_2->SetPosition(160, 131);
    OilP_2->setCurrentEngine(2);
    this->AddGaugeComponent(OilP_2);

    B737EICASOilT* OilT_1 = new B737EICASOilT();
    OilT_1->SetParentRenderObject(this);
    OilT_1->SetPosition(120, 96);
    OilT_1->setCurrentEngine(1);
    this->AddGaugeComponent(OilT_1);

    B737EICASOilT* OilT_2 = new B737EICASOilT();
    OilT_2->SetParentRenderObject(this);
    OilT_2->SetPosition(160, 96);
    OilT_2->setCurrentEngine(2);
    this->AddGaugeComponent(OilT_2);

    B737EICASOilQ* OilQ_1 = new B737EICASOilQ();
    OilQ_1->SetParentRenderObject(this);
    OilQ_1->SetPosition(125, 85);
    OilQ_1->setCurrentEngine(1);
    this->AddGaugeComponent(OilQ_1);

    B737EICASOilQ* OilQ_2 = new B737EICASOilQ();
    OilQ_2->SetParentRenderObject(this);
    OilQ_2->SetPosition(175, 85);
    OilQ_2->setCurrentEngine(2);
    this->AddGaugeComponent(OilQ_2);

    B737EICASVib* Vib_1 = new B737EICASVib();
    Vib_1->SetParentRenderObject(this);
    Vib_1->SetPosition(120, 50);
    Vib_1->setCurrentEngine(1);
    this->AddGaugeComponent(Vib_1);

    B737EICASVib* Vib_2 = new B737EICASVib();
    Vib_2->SetParentRenderObject(this);
    Vib_2->SetPosition(160, 50);
    Vib_2->setCurrentEngine(2);
    this->AddGaugeComponent(Vib_2);

    B737EICASHydP* HydP_1 = new B737EICASHydP();
    HydP_1->SetParentRenderObject(this);
    HydP_1->SetPosition(120, 15);
    HydP_1->setCurrentEngine(1);
    this->AddGaugeComponent(HydP_1);

    B737EICASHydP* HydP_2 = new B737EICASHydP();
    HydP_2->SetParentRenderObject(this);
    HydP_2->SetPosition(160, 15);
    HydP_2->setCurrentEngine(2);
    this->AddGaugeComponent(HydP_2);

    B737EICASHydQ* HydQ_1 = new B737EICASHydQ();
    HydQ_1->SetParentRenderObject(this);
    HydQ_1->SetPosition(125, 4);
    HydQ_1->setCurrentEngine(1);
    this->AddGaugeComponent(HydQ_1);

    B737EICASHydQ* HydQ_2 = new B737EICASHydQ();
    HydQ_2->SetParentRenderObject(this);
    HydQ_2->SetPosition(175, 4);
    HydQ_2->setCurrentEngine(2);
    this->AddGaugeComponent(HydQ_2);

    B737EICASFuelQ* FuelQ_0 = new B737EICASFuelQ();
    FuelQ_0->SetParentRenderObject(this);
    FuelQ_0->SetPosition(40, 8);
    FuelQ_0->setCurrentTank(0);
    this->AddGaugeComponent(FuelQ_0);

    B737EICASFuelQ* FuelQ_1 = new B737EICASFuelQ();
    FuelQ_1->SetParentRenderObject(this);
    FuelQ_1->SetPosition(2, 0);
    FuelQ_1->setCurrentTank(1);
    this->AddGaugeComponent(FuelQ_1);

    B737EICASFuelQ* FuelQ_2 = new B737EICASFuelQ();
    FuelQ_2->SetParentRenderObject(this);
    FuelQ_2->SetPosition(78, 0);
    FuelQ_2->setCurrentTank(2);
    this->AddGaugeComponent(FuelQ_2);

    B737EICASBackground* background = new B737EICASBackground();
    background->SetParentRenderObject(this);
    background->SetPosition(0, 0);
    this->AddGaugeComponent(background);
  }

  B737EICAS::~B737EICAS()
  {

  }

  void B737EICAS::Render()
  {
    int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");
    if (*avionics_on == 1) {
      Gauge::Render();
    }
  }

}

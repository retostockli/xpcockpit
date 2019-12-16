/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737MIP.cpp,v $

  Copyright (C) 2015 by:
    Original author:
      Reto Stockli
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2017/04/30 $
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

/*
 The Annunciators of the Boeing 737 MIP
*/

#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#include "ogcGLHeaders.h"
#include "ogcGaugeComponent.h"
#include "B737/B737MIP/ogcB737MIP.h"
#include "B737/B737MIP/ogcB737MIPAnn.h"

namespace OpenGC
{

B737MIP::B737MIP()
{
  printf("B737MIP constructed\n");
	
  // We want to draw an outline
  this->SetGaugeOutline(true);

  //  m_Font = m_pFontManager->LoadDefaultFont();
  m_Font = m_pFontManager->LoadFont((char*) "CockpitMCDU.ttf");

  // Specify our physical size and position
  m_PhysicalPosition.x = 0;
  m_PhysicalPosition.y = 0;
  
  m_PhysicalSize.x = 300;
  m_PhysicalSize.y = 60;


  // grid spacing between annunciators
  float deltax = 20;
  float deltay = 10;
  float deltax2 = 15;
  float deltay2 = 15;
  float deltax3 = 25;
  float deltay3 = 12.5;

  // add all annunciators

  B737MIPAnn* ann_nosegreen = new B737MIPAnn();
  ann_nosegreen->SetParentRenderObject(this);
  ann_nosegreen->SetPosition(10 + 7*deltax+0.5*deltax3, m_PhysicalSize.y - deltay3 - 5 - 1*deltay3);
  ann_nosegreen->setAnnName((char*) "NOSE GEAR");
  ann_nosegreen->setAnnForm(0);
  ann_nosegreen->setAnnSizeX(deltax3);
  ann_nosegreen->setAnnSizeY(deltay3);
  ann_nosegreen->setAnnFGColor(0,255,0);
  ann_nosegreen->setAnnDataRefType1(2);
  ann_nosegreen->setAnnDataRef1((char*) "x737/systems/landinggear/noseGearAnn1_green");
  ann_nosegreen->setAnnDataRef2((char*) "laminar/B738/annunciator/nose_gear_safe");
  this->AddGaugeComponent(ann_nosegreen);

  B737MIPAnn* ann_nosered = new B737MIPAnn();
  ann_nosered->SetParentRenderObject(this);
  ann_nosered->SetPosition(10 + 7*deltax+0.5*deltax3, m_PhysicalSize.y - deltay3 - 5 - 0*deltay3);
  ann_nosered->setAnnName((char*) "NOSE GEAR");
  ann_nosered->setAnnForm(0);
  ann_nosered->setAnnSizeX(deltax3);
  ann_nosered->setAnnSizeY(deltay3);
  ann_nosered->setAnnFGColor(255,100,0);
  ann_nosered->setAnnDataRefType1(2);
  ann_nosered->setAnnDataRef1((char*) "x737/systems/landinggear/noseGearAnn1_red");
  ann_nosered->setAnnDataRef2((char*) "laminar/B738/annunciator/nose_gear_transit");
  this->AddGaugeComponent(ann_nosered);

  B737MIPAnn* ann_leftgreen = new B737MIPAnn();
  ann_leftgreen->SetParentRenderObject(this);
  ann_leftgreen->SetPosition(10 + 7*deltax, m_PhysicalSize.y - deltay3 - 5 - 3*deltay3);
  ann_leftgreen->setAnnName((char*) "LEFT GEAR");
  ann_leftgreen->setAnnForm(0);
  ann_leftgreen->setAnnSizeX(deltax3);
  ann_leftgreen->setAnnSizeY(deltay3);
  ann_leftgreen->setAnnFGColor(0,255,0);
  ann_leftgreen->setAnnDataRefType1(2);
  ann_leftgreen->setAnnDataRef1((char*) "x737/systems/landinggear/leftGearAnn1_green");
  ann_leftgreen->setAnnDataRef2((char*) "laminar/B738/annunciator/left_gear_safe");
  this->AddGaugeComponent(ann_leftgreen);

  B737MIPAnn* ann_leftred = new B737MIPAnn();
  ann_leftred->SetParentRenderObject(this);
  ann_leftred->SetPosition(10 + 7*deltax, m_PhysicalSize.y - deltay3 - 5 - 2*deltay3);
  ann_leftred->setAnnName((char*) "LEFT GEAR");
  ann_leftred->setAnnForm(0);
  ann_leftred->setAnnSizeX(deltax3);
  ann_leftred->setAnnSizeY(deltay3);
  ann_leftred->setAnnFGColor(255,100,0);
  ann_leftred->setAnnDataRefType1(2);
  ann_leftred->setAnnDataRef1((char*) "x737/systems/landinggear/leftGearAnn1_red");
  ann_leftred->setAnnDataRef2((char*) "laminar/B738/annunciator/left_gear_transit");
  this->AddGaugeComponent(ann_leftred);

  B737MIPAnn* ann_rightgreen = new B737MIPAnn();
  ann_rightgreen->SetParentRenderObject(this);
  ann_rightgreen->SetPosition(10 + 7*deltax+1.0*deltax3, m_PhysicalSize.y - deltay3 - 5 - 3*deltay3);
  ann_rightgreen->setAnnName((char*) "RIGHT GEAR");
  ann_rightgreen->setAnnForm(0);
  ann_rightgreen->setAnnSizeX(deltax3);
  ann_rightgreen->setAnnSizeY(deltay3);
  ann_rightgreen->setAnnFGColor(0,255,0);
  ann_rightgreen->setAnnDataRefType1(2);
  ann_rightgreen->setAnnDataRef1((char*) "x737/systems/landinggear/rightGearAnn1_green");
  ann_rightgreen->setAnnDataRef2((char*) "laminar/B738/annunciator/right_gear_safe");
  this->AddGaugeComponent(ann_rightgreen);

  B737MIPAnn* ann_rightred = new B737MIPAnn();
  ann_rightred->SetParentRenderObject(this);
  ann_rightred->SetPosition(10 + 7*deltax+1.0*deltax3, m_PhysicalSize.y - deltay3 - 5 - 2*deltay3);
  ann_rightred->setAnnName((char*) "RIGHT GEAR");
  ann_rightred->setAnnForm(0);
  ann_rightred->setAnnSizeX(deltax3);
  ann_rightred->setAnnSizeY(deltay3);
  ann_rightred->setAnnFGColor(255,100,0);
  ann_rightred->setAnnDataRefType1(2);
  ann_rightred->setAnnDataRef1((char*) "x737/systems/landinggear/rightGearAnn1_red");
  ann_rightred->setAnnDataRef2((char*) "laminar/B738/annunciator/right_gear_transit");
  this->AddGaugeComponent(ann_rightred);

  B737MIPAnn* ann_atc = new B737MIPAnn();
  ann_atc->SetParentRenderObject(this);
  ann_atc->SetPosition(10 + 12*deltax, m_PhysicalSize.y - deltay - 5 - 3*deltay);
  ann_atc->setAnnName((char*) "ATC");
  ann_atc->setAnnForm(0);
  ann_atc->setAnnFGColor(255,100,0);
  ann_atc->setAnnDataRefType1(2);
  ann_atc->setAnnDataRef1((char*) "x737/systems/comm/ATC_mip_annunc");
  this->AddGaugeComponent(ann_atc);

  B737MIPAnn* ann_flapload = new B737MIPAnn();
  ann_flapload->SetParentRenderObject(this);
  ann_flapload->SetPosition(10 + 12*deltax, m_PhysicalSize.y - deltay - 5 - 4*deltay);
  ann_flapload->setAnnName((char*) "FLAP.LOAD RELIEF");
  ann_flapload->setAnnForm(0);
  ann_flapload->setAnnFGColor(255,100,0);
  ann_flapload->setAnnDataRefType1(2);
  ann_flapload->setAnnDataRef1((char*) "");
  this->AddGaugeComponent(ann_flapload);

  B737MIPAnn* ann_speedbrakes = new B737MIPAnn();
  ann_speedbrakes->SetParentRenderObject(this);
  ann_speedbrakes->SetPosition(10 + 11*deltax, m_PhysicalSize.y - deltay - 5 - 4*deltay);
  ann_speedbrakes->setAnnName((char*) "SPEEDBRAKES EXTENDED");
  ann_speedbrakes->setAnnForm(0);
  ann_speedbrakes->setAnnFGColor(255,100,0);
  ann_speedbrakes->setAnnDataRefType1(1);
  ann_speedbrakes->setAnnDataRef1((char*) "x737/systems/speedbrake/spdbrkExtendedLight");
  this->AddGaugeComponent(ann_speedbrakes);

  B737MIPAnn* ann_braketemp = new B737MIPAnn();
  ann_braketemp->SetParentRenderObject(this);
  ann_braketemp->SetPosition(10 + 10*deltax, m_PhysicalSize.y - deltay - 5 - 4*deltay);
  ann_braketemp->setAnnName((char*) "BRAKE TEMP");
  ann_braketemp->setAnnForm(0);
  ann_braketemp->setAnnFGColor(255,100,0);
  ann_braketemp->setAnnDataRefType1(2);
  ann_braketemp->setAnnDataRef1((char*) "");
  this->AddGaugeComponent(ann_braketemp);

  B737MIPAnn* ann_belowgs = new B737MIPAnn();
  ann_belowgs->SetParentRenderObject(this);
  ann_belowgs->SetPosition(10 + 13*deltax, m_PhysicalSize.y - deltay - 5 - 4*deltay);
  ann_belowgs->setAnnName((char*) "BELOW.G/S P.INHIBIT");
  ann_belowgs->setAnnForm(0);
  ann_belowgs->setAnnFGColor(255,100,0);
  ann_belowgs->setAnnDataRefType1(2);
  ann_belowgs->setAnnDataRef1((char*) "");
  ann_belowgs->setAnnDataRef2((char*) "laminar/B738/annunciator/below_gs");
  this->AddGaugeComponent(ann_belowgs);

  B737MIPAnn* ann_capt_aprst = new B737MIPAnn();
  ann_capt_aprst->SetParentRenderObject(this);
  ann_capt_aprst->SetPosition(10 + 10.5*deltax, m_PhysicalSize.y - deltay - 5 - 0.5*deltay);
  ann_capt_aprst->setAnnName((char*) "A/P P/RST");
  ann_capt_aprst->setAnnForm(0);
  ann_capt_aprst->setAnnSizeX(deltax2);
  ann_capt_aprst->setAnnSizeY(deltay2);
  ann_capt_aprst->setAnnFGColor(255,100,0);
  ann_capt_aprst->setAnnDataRefType1(1);
  ann_capt_aprst->setAnnDataRef1((char*) "x737/systems/afds/alert/capt/AP_PRST_amber");
  ann_capt_aprst->setAnnDataRef2((char*) "laminar/B738/annunciator/ap_disconnect");
  ann_capt_aprst->setAnnKeyDataRefType1(2);
  ann_capt_aprst->setAnnKeyDataRef1((char*) "x737/systems/afds/alert/capt/AP_PRST_reset");
  ann_capt_aprst->setAnnKeyDataRefType2(4);
  ann_capt_aprst->setAnnKeyDataRef2((char*) "laminar/B738/push_button/ap_light_pilot");
  this->AddGaugeComponent(ann_capt_aprst);

  B737MIPAnn* ann_capt_atrst = new B737MIPAnn();
  ann_capt_atrst->SetParentRenderObject(this);
  ann_capt_atrst->SetPosition(10 + 10.5*deltax+deltax2, m_PhysicalSize.y - deltay - 5 - 0.5*deltay);
  ann_capt_atrst->setAnnName((char*) "A/T P/RST");
  ann_capt_atrst->setAnnForm(0);
  ann_capt_atrst->setAnnSizeX(deltax2);
  ann_capt_atrst->setAnnSizeY(deltay2);
  ann_capt_atrst->setAnnFGColor(255,100,0);
  ann_capt_atrst->setAnnDataRefType1(1);
  ann_capt_atrst->setAnnDataRef1((char*) "x737/systems/afds/alert/capt/AT_PRST_amber");
  ann_capt_atrst->setAnnDataRef2((char*) "laminar/B738/annunciator/at_disconnect");
  ann_capt_atrst->setAnnKeyDataRefType2(4);
  ann_capt_atrst->setAnnKeyDataRef2((char*) "laminar/B738/push_button/at_light_pilot");
  this->AddGaugeComponent(ann_capt_atrst);

  B737MIPAnn* ann_capt_fmcrst = new B737MIPAnn();
  ann_capt_fmcrst->SetParentRenderObject(this);
  ann_capt_fmcrst->SetPosition(10 + 10.5*deltax+2*deltax2, m_PhysicalSize.y - deltay - 5 - 0.5*deltay);
  ann_capt_fmcrst->setAnnName((char*) "FMC P/RST");
  ann_capt_fmcrst->setAnnForm(0);
  ann_capt_fmcrst->setAnnSizeX(deltax2);
  ann_capt_fmcrst->setAnnSizeY(deltay2);
  ann_capt_fmcrst->setAnnFGColor(255,100,0);
  ann_capt_fmcrst->setAnnDataRefType1(1);
  ann_capt_fmcrst->setAnnDataRef1((char*) "x737/systems/afds/alert/capt/FMC_PRST_amber");
  this->AddGaugeComponent(ann_capt_fmcrst);

  B737MIPAnn* ann_flapsext = new B737MIPAnn();
  ann_flapsext->SetParentRenderObject(this);
  ann_flapsext->SetPosition(10 + 5.5*deltax, m_PhysicalSize.y - deltay - 5 - 4*deltay);
  ann_flapsext->setAnnName((char*) "LE.FLAPS EXT");
  ann_flapsext->setAnnForm(0);
  ann_flapsext->setAnnFGColor(0,255,0);
  ann_flapsext->setAnnDataRefType1(2);
  ann_flapsext->setAnnDataRef1((char*) "x737/systems/flaps/LE_EXT");
  this->AddGaugeComponent(ann_flapsext);

  B737MIPAnn* ann_flapstransit = new B737MIPAnn();
  ann_flapstransit->SetParentRenderObject(this);
  ann_flapstransit->SetPosition(10 + 4.5*deltax, m_PhysicalSize.y - deltay - 5 - 4*deltay);
  ann_flapstransit->setAnnName((char*) "LE.FLAPS TRANSIT");
  ann_flapstransit->setAnnForm(0);
  ann_flapstransit->setAnnFGColor(255,100,0);
  ann_flapstransit->setAnnDataRefType1(2);
  ann_flapstransit->setAnnDataRef1((char*) "x737/systems/flaps/LE_TRANSIT");
  this->AddGaugeComponent(ann_flapstransit);

  B737MIPAnn* ann_antiskid_inop = new B737MIPAnn();
  ann_antiskid_inop->SetParentRenderObject(this);
  ann_antiskid_inop->SetPosition(10 + 3.5*deltax, m_PhysicalSize.y - deltay - 5 - 4*deltay);
  ann_antiskid_inop->setAnnName((char*) "ANTISKID INOP");
  ann_antiskid_inop->setAnnForm(0);
  ann_antiskid_inop->setAnnFGColor(255,100,0);
  ann_antiskid_inop->setAnnDataRefType1(2);
  ann_antiskid_inop->setAnnDataRef1((char*) "x737/systems/brakes/antiskidInopOn");
  this->AddGaugeComponent(ann_antiskid_inop);

  B737MIPAnn* ann_autobrake_disarm = new B737MIPAnn();
  ann_autobrake_disarm->SetParentRenderObject(this);
  ann_autobrake_disarm->SetPosition(10 + 3.5*deltax, m_PhysicalSize.y - deltay - 5 - 0*deltay);
  ann_autobrake_disarm->setAnnName((char*) "AUTOBRAKE DISARM");
  ann_autobrake_disarm->setAnnForm(0);
  ann_autobrake_disarm->setAnnFGColor(255,100,0);
  ann_autobrake_disarm->setAnnDataRefType1(2);
  ann_autobrake_disarm->setAnnDataRef1((char*) "x737/systems/brakes/abrkDisarmedOn");
  this->AddGaugeComponent(ann_autobrake_disarm);
 
  B737MIPAnn* ann_speedbrake_arm = new B737MIPAnn();
  ann_speedbrake_arm->SetParentRenderObject(this);
  ann_speedbrake_arm->SetPosition(10 + 1.5*deltax, m_PhysicalSize.y - deltay - 5 - 3*deltay);
  ann_speedbrake_arm->setAnnName((char*) "SPEEDBRAKE ARMED");
  ann_speedbrake_arm->setAnnForm(0);
  ann_speedbrake_arm->setAnnFGColor(0,255,0);
  ann_speedbrake_arm->setAnnDataRefType1(2);
  ann_speedbrake_arm->setAnnDataRef1((char*) "x737/systems/speedbrake/spdbrkIsArmed_f");
  this->AddGaugeComponent(ann_speedbrake_arm);

  B737MIPAnn* ann_speedbrake_donotarm = new B737MIPAnn();
  ann_speedbrake_donotarm->SetParentRenderObject(this);
  ann_speedbrake_donotarm->SetPosition(10 + 1.5*deltax, m_PhysicalSize.y - deltay - 5 - 4*deltay);
  ann_speedbrake_donotarm->setAnnName((char*) "SPEEDBRAKE DO.NOT.ARM");
  ann_speedbrake_donotarm->setAnnForm(0);
  ann_speedbrake_donotarm->setAnnFGColor(255,100,0);
  ann_speedbrake_donotarm->setAnnDataRefType1(1);
  ann_speedbrake_donotarm->setAnnDataRef1((char*) "x737/systems/speedbrake/spdbrkDoNotArmLight");
  this->AddGaugeComponent(ann_speedbrake_donotarm);

  B737MIPAnn* ann_staboutoftrim = new B737MIPAnn();
  ann_staboutoftrim->SetParentRenderObject(this);
  ann_staboutoftrim->SetPosition(10 + 2.5*deltax, m_PhysicalSize.y - deltay - 5 - 3.75*deltay);
  ann_staboutoftrim->setAnnName((char*) "STAB OUT.OF TRIM");
  ann_staboutoftrim->setAnnForm(0);
  ann_staboutoftrim->setAnnSizeX(deltax2);
  ann_staboutoftrim->setAnnSizeY(deltay2);
  ann_staboutoftrim->setAnnFGColor(255,100,0);
  ann_staboutoftrim->setAnnDataRefType1(1);
  ann_staboutoftrim->setAnnDataRef1((char*) "x737/systems/stabtrim/stabOutOfTrimLight");
  this->AddGaugeComponent(ann_staboutoftrim);

  B737MIPAnn* ann_autoland = new B737MIPAnn();
  ann_autoland->SetParentRenderObject(this);
  ann_autoland->SetPosition(10 + 0.0*deltax, m_PhysicalSize.y - deltay - 5 - 4*deltay);
  ann_autoland->setAnnName((char*) "AUTOLAND");
  ann_autoland->setAnnForm(0);
  ann_autoland->setAnnFGColor(255,100,0);
  ann_autoland->setAnnDataRefType1(2);
  ann_autoland->setAnnDataRef1((char*) "x737/systems/afds/AUTOLAND_warning");
  this->AddGaugeComponent(ann_autoland);

}

B737MIP::~B737MIP()
{
  // Destruction handled by base class
}

void B737MIP::Render()
{

  int acf_type = m_pDataSource->GetAcfType();
  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");

  if ((*avionics_on == 1) &&
      ((acf_type == 1) || (acf_type == 2) || (acf_type == 3))) {
   
    // after drawing the background, draw components
    Gauge::Render();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // grey MIP plate
    /*
    glColor3ub( 88, 94, 100 );
    glPolygonMode(GL_FRONT,GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f( 0, 0);
    glVertex2f( m_PhysicalSize.x, 0);
    glVertex2f( m_PhysicalSize.y, m_PhysicalSize.y);
    glVertex2f( 0, m_PhysicalSize.y);
    glEnd();
    */
    

    // black rectangle of MIP display
    //    glColor3ub(255, 255, 255 );
    //    glBegin(GL_POLYGON);
    //    glBegin(GL_LINE_LOOP);
    //    glVertex2f( 10, m_PhysicalSize.y - 4 );
    //    glVertex2f( 98, m_PhysicalSize.y - 4 );
    //glVertex2f( 98, 4 );
    //glVertex2f( 10, 4 );
    //    glVertex2f( 98, m_PhysicalSize.y - 68 );
    //    glVertex2f( 10, m_PhysicalSize.y - 68 );
    //    glEnd();

    //glPolygonMode(GL_FRONT,GL_LINE);

    glPopMatrix();

  }
}

} // end namespace OpenGC


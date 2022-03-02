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
 The Annunciators of the Boeing 737 MIP and leftovers of the AFT OVERHEAD etc.
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
  m_PhysicalSize.y = 100;


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
  ann_nosegreen->setAnnDataRefType2(2);
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
  ann_nosered->setAnnDataRefType2(2);
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
  ann_leftgreen->setAnnDataRefType2(2);
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
  ann_leftred->setAnnDataRefType2(2);
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
  ann_rightgreen->setAnnDataRefType2(2);
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
  ann_rightred->setAnnDataRefType2(2);
  ann_rightred->setAnnDataRef2((char*) "laminar/B738/annunciator/right_gear_transit");
  this->AddGaugeComponent(ann_rightred);

  /* FO Side */
  B737MIPAnn* ann_cabinaltitude = new B737MIPAnn();
  ann_cabinaltitude->SetParentRenderObject(this);
  ann_cabinaltitude->SetPosition(10 + 10*deltax, m_PhysicalSize.y - deltay - 5 - 4*deltay);
  ann_cabinaltitude->setAnnName((char*) "CABIN ALTITUDE");
  ann_cabinaltitude->setAnnForm(0);
  ann_cabinaltitude->setAnnFGColor(255,100,0);
  ann_cabinaltitude->setAnnDataRefType2(2);
  ann_cabinaltitude->setAnnDataRef2((char*) "laminar/B738/annunciator/cabin_alt");
  this->AddGaugeComponent(ann_cabinaltitude);

  B737MIPAnn* ann_takeoffconfig = new B737MIPAnn();
  ann_takeoffconfig->SetParentRenderObject(this);
  ann_takeoffconfig->SetPosition(10 + 11*deltax, m_PhysicalSize.y - deltay - 5 - 4*deltay);
  ann_takeoffconfig->setAnnName((char*) "TAKEOFF CONFIG");
  ann_takeoffconfig->setAnnForm(0);
  ann_takeoffconfig->setAnnFGColor(255,100,0);
  ann_takeoffconfig->setAnnDataRefType2(2);
  ann_takeoffconfig->setAnnDataRef2((char*) "laminar/B738/annunciator/takeoff_config");
  this->AddGaugeComponent(ann_takeoffconfig);

  B737MIPAnn* ann_speedbrakes = new B737MIPAnn();
  ann_speedbrakes->SetParentRenderObject(this);
  ann_speedbrakes->SetPosition(10 + 12*deltax, m_PhysicalSize.y - deltay - 5 - 4*deltay);
  ann_speedbrakes->setAnnName((char*) "SPEEDBRAKES EXTENDED");
  ann_speedbrakes->setAnnForm(0);
  ann_speedbrakes->setAnnFGColor(255,100,0);
  ann_speedbrakes->setAnnDataRefType1(1);
  ann_speedbrakes->setAnnDataRef1((char*) "x737/systems/speedbrake/spdbrkExtendedLight");
  ann_speedbrakes->setAnnDataRefType2(2);
  ann_speedbrakes->setAnnDataRef2((char*) "laminar/B738/annunciator/speedbrake_extend");
  this->AddGaugeComponent(ann_speedbrakes);

  B737MIPAnn* ann_belowgs = new B737MIPAnn();
  ann_belowgs->SetParentRenderObject(this);
  ann_belowgs->SetPosition(10 + 13*deltax, m_PhysicalSize.y - deltay - 5 - 4*deltay);
  ann_belowgs->setAnnName((char*) "BELOW.G/S P.INHIBIT");
  ann_belowgs->setAnnForm(0);
  ann_belowgs->setAnnFGColor(255,100,0);
  ann_belowgs->setAnnDataRefType2(2);
  ann_belowgs->setAnnDataRef2((char*) "laminar/B738/annunciator/below_gs");
  this->AddGaugeComponent(ann_belowgs);

  /* Autopilot / Autothrottle RESET BUTTONS */
  
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
  ann_capt_aprst->setAnnDataRefType2(2);
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
  ann_capt_atrst->setAnnDataRefType2(2);
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
  ann_flapsext->setAnnDataRefType2(2);
  ann_flapsext->setAnnDataRef2((char*) "laminar/B738/annunciator/slats_extend");
  this->AddGaugeComponent(ann_flapsext);

  B737MIPAnn* ann_flapstransit = new B737MIPAnn();
  ann_flapstransit->SetParentRenderObject(this);
  ann_flapstransit->SetPosition(10 + 4.5*deltax, m_PhysicalSize.y - deltay - 5 - 4*deltay);
  ann_flapstransit->setAnnName((char*) "LE.FLAPS TRANSIT");
  ann_flapstransit->setAnnForm(0);
  ann_flapstransit->setAnnFGColor(255,100,0);
  ann_flapstransit->setAnnDataRefType1(2);
  ann_flapstransit->setAnnDataRef1((char*) "x737/systems/flaps/LE_TRANSIT");
  ann_flapstransit->setAnnDataRefType2(2);
  ann_flapstransit->setAnnDataRef2((char*) "laminar/B738/annunciator/slats_transit");
  this->AddGaugeComponent(ann_flapstransit);

  B737MIPAnn* ann_antiskid_inop = new B737MIPAnn();
  ann_antiskid_inop->SetParentRenderObject(this);
  ann_antiskid_inop->SetPosition(10 + 3.5*deltax, m_PhysicalSize.y - deltay - 5 - 4*deltay);
  ann_antiskid_inop->setAnnName((char*) "ANTISKID INOP");
  ann_antiskid_inop->setAnnForm(0);
  ann_antiskid_inop->setAnnFGColor(255,100,0);
  ann_antiskid_inop->setAnnDataRefType1(2);
  ann_antiskid_inop->setAnnDataRef1((char*) "x737/systems/brakes/antiskidInopOn");
  ann_antiskid_inop->setAnnDataRefType2(2);
  ann_antiskid_inop->setAnnDataRef2((char*) "laminar/B738/annunciator/anti_skid_inop");
  this->AddGaugeComponent(ann_antiskid_inop);

  B737MIPAnn* ann_autobrake_disarm = new B737MIPAnn();
  ann_autobrake_disarm->SetParentRenderObject(this);
  ann_autobrake_disarm->SetPosition(10 + 3.5*deltax, m_PhysicalSize.y - deltay - 5 - 0*deltay);
  ann_autobrake_disarm->setAnnName((char*) "AUTOBRAKE DISARM");
  ann_autobrake_disarm->setAnnForm(0);
  ann_autobrake_disarm->setAnnFGColor(255,100,0);
  ann_autobrake_disarm->setAnnDataRefType1(2);
  ann_autobrake_disarm->setAnnDataRef1((char*) "x737/systems/brakes/abrkDisarmedOn");
  ann_autobrake_disarm->setAnnDataRefType2(2);
  ann_autobrake_disarm->setAnnDataRef2((char*) "laminar/B738/annunciator/auto_brake_disarm");
  this->AddGaugeComponent(ann_autobrake_disarm);
 
  B737MIPAnn* ann_speedbrake_arm = new B737MIPAnn();
  ann_speedbrake_arm->SetParentRenderObject(this);
  ann_speedbrake_arm->SetPosition(10 + 1.5*deltax, m_PhysicalSize.y - deltay - 5 - 3*deltay);
  ann_speedbrake_arm->setAnnName((char*) "SPEEDBRAKE ARMED");
  ann_speedbrake_arm->setAnnForm(0);
  ann_speedbrake_arm->setAnnFGColor(0,255,0);
  ann_speedbrake_arm->setAnnDataRefType1(2);
  ann_speedbrake_arm->setAnnDataRef1((char*) "x737/systems/speedbrake/spdbrkIsArmed_f");
  ann_speedbrake_arm->setAnnDataRefType2(2);
  ann_speedbrake_arm->setAnnDataRef2((char*) "laminar/B738/annunciator/speedbrake_armed");
  this->AddGaugeComponent(ann_speedbrake_arm);

  B737MIPAnn* ann_speedbrake_donotarm = new B737MIPAnn();
  ann_speedbrake_donotarm->SetParentRenderObject(this);
  ann_speedbrake_donotarm->SetPosition(10 + 1.5*deltax, m_PhysicalSize.y - deltay - 5 - 4*deltay);
  ann_speedbrake_donotarm->setAnnName((char*) "SPEEDBRAKE DO.NOT.ARM");
  ann_speedbrake_donotarm->setAnnForm(0);
  ann_speedbrake_donotarm->setAnnFGColor(255,100,0);
  ann_speedbrake_donotarm->setAnnDataRefType1(1);
  ann_speedbrake_donotarm->setAnnDataRef1((char*) "x737/systems/speedbrake/spdbrkDoNotArmLight");
  ann_speedbrake_donotarm->setAnnDataRefType2(2);
  ann_speedbrake_donotarm->setAnnDataRef2((char*) "laminar/B738/annunciator/spd_brk_not_arm");
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
  ann_staboutoftrim->setAnnDataRefType2(2);
  ann_staboutoftrim->setAnnDataRef2((char*) "laminar/B738/annunciator/stab_out_of_trim");
  this->AddGaugeComponent(ann_staboutoftrim);

  /* AFT OVERHEAD LEFTOVERS */
  
  B737MIPAnn* ann_pseu = new B737MIPAnn();
  ann_pseu->SetParentRenderObject(this);
  ann_pseu->SetPosition(10 + 7*deltax, m_PhysicalSize.y - deltay - 5 - 7*deltay);
  ann_pseu->setAnnName((char*) "PSEU");
  ann_pseu->setAnnForm(0);
  ann_pseu->setAnnFGColor(255,100,0);
  ann_pseu->setAnnDataRefType2(2);
  ann_pseu->setAnnDataRef2((char*) "laminar/B738/annunciator/test");
  this->AddGaugeComponent(ann_pseu);

  B737MIPAnn* ann_gps = new B737MIPAnn();
  ann_gps->SetParentRenderObject(this);
  ann_gps->SetPosition(10 + 7*deltax, m_PhysicalSize.y - deltay - 5 - 8*deltay);
  ann_gps->setAnnName((char*) "GPS");
  ann_gps->setAnnForm(0);
  ann_gps->setAnnFGColor(255,255,255);
  ann_gps->setAnnDataRefType2(2);
  ann_gps->setAnnDataRef2((char*) "laminar/B738/annunciator/gps");
  this->AddGaugeComponent(ann_gps);

  B737MIPAnn* ann_irs_l_align = new B737MIPAnn();
  ann_irs_l_align->SetParentRenderObject(this);
  ann_irs_l_align->SetPosition(10 + 0*deltax, m_PhysicalSize.y - deltay - 5 - 7*deltay);
  ann_irs_l_align->setAnnName((char*) "ALIGN");
  ann_irs_l_align->setAnnForm(0);
  ann_irs_l_align->setAnnFGColor(255,255,255);
  ann_irs_l_align->setAnnDataRefType2(2);
  ann_irs_l_align->setAnnDataRef2((char*) "laminar/B738/annunciator/irs_align_left");
  this->AddGaugeComponent(ann_irs_l_align);

  B737MIPAnn* ann_irs_l_on_dc = new B737MIPAnn();
  ann_irs_l_on_dc->SetParentRenderObject(this);
  ann_irs_l_on_dc->SetPosition(10 + 1*deltax, m_PhysicalSize.y - deltay - 5 - 7*deltay);
  ann_irs_l_on_dc->setAnnName((char*) "ON.DC");
  ann_irs_l_on_dc->setAnnForm(0);
  ann_irs_l_on_dc->setAnnFGColor(255,100,0);
  ann_irs_l_on_dc->setAnnDataRefType2(2);
  ann_irs_l_on_dc->setAnnDataRef2((char*) "laminar/B738/annunciator/irs_on_dc_left");
  this->AddGaugeComponent(ann_irs_l_on_dc);

  B737MIPAnn* ann_irs_l_fail = new B737MIPAnn();
  ann_irs_l_fail->SetParentRenderObject(this);
  ann_irs_l_fail->SetPosition(10 + 0*deltax, m_PhysicalSize.y - deltay - 5 - 8*deltay);
  ann_irs_l_fail->setAnnName((char*) "FAULT");
  ann_irs_l_fail->setAnnForm(0);
  ann_irs_l_fail->setAnnFGColor(255,100,0);
  ann_irs_l_fail->setAnnDataRefType2(2);
  ann_irs_l_fail->setAnnDataRef2((char*) "laminar/B738/annunciator/irs_align_fail_left");
  this->AddGaugeComponent(ann_irs_l_fail);

  B737MIPAnn* ann_irs_l_dc_fail = new B737MIPAnn();
  ann_irs_l_dc_fail->SetParentRenderObject(this);
  ann_irs_l_dc_fail->SetPosition(10 + 1*deltax, m_PhysicalSize.y - deltay - 5 - 8*deltay);
  ann_irs_l_dc_fail->setAnnName((char*) "DC.FAIL");
  ann_irs_l_dc_fail->setAnnForm(0);
  ann_irs_l_dc_fail->setAnnFGColor(255,100,0);
  ann_irs_l_dc_fail->setAnnDataRefType2(2);
  ann_irs_l_dc_fail->setAnnDataRef2((char*) "laminar/B738/annunciator/irs_dc_fail_left");
  this->AddGaugeComponent(ann_irs_l_dc_fail);

  
  
  B737MIPAnn* ann_irs_r_align = new B737MIPAnn();
  ann_irs_r_align->SetParentRenderObject(this);
  ann_irs_r_align->SetPosition(10 + 3*deltax, m_PhysicalSize.y - deltay - 5 - 7*deltay);
  ann_irs_r_align->setAnnName((char*) "ALIGN");
  ann_irs_r_align->setAnnForm(0);
  ann_irs_r_align->setAnnFGColor(255,255,255);
  ann_irs_r_align->setAnnDataRefType2(2);
  ann_irs_r_align->setAnnDataRef2((char*) "laminar/B738/annunciator/irs_align_right");
  this->AddGaugeComponent(ann_irs_r_align);

  B737MIPAnn* ann_irs_r_on_dc = new B737MIPAnn();
  ann_irs_r_on_dc->SetParentRenderObject(this);
  ann_irs_r_on_dc->SetPosition(10 + 4*deltax, m_PhysicalSize.y - deltay - 5 - 7*deltay);
  ann_irs_r_on_dc->setAnnName((char*) "ON.DC");
  ann_irs_r_on_dc->setAnnForm(0);
  ann_irs_r_on_dc->setAnnFGColor(255,100,0);
  ann_irs_r_on_dc->setAnnDataRefType2(2);
  ann_irs_r_on_dc->setAnnDataRef2((char*) "laminar/B738/annunciator/irs_on_dc_right");
  this->AddGaugeComponent(ann_irs_r_on_dc);

  B737MIPAnn* ann_irs_r_fail = new B737MIPAnn();
  ann_irs_r_fail->SetParentRenderObject(this);
  ann_irs_r_fail->SetPosition(10 + 3*deltax, m_PhysicalSize.y - deltay - 5 - 8*deltay);
  ann_irs_r_fail->setAnnName((char*) "FAULT");
  ann_irs_r_fail->setAnnForm(0);
  ann_irs_r_fail->setAnnFGColor(255,100,0);
  ann_irs_r_fail->setAnnDataRefType2(2);
  ann_irs_r_fail->setAnnDataRef2((char*) "laminar/B738/annunciator/irs_align_fail_right");
  this->AddGaugeComponent(ann_irs_r_fail);

  B737MIPAnn* ann_irs_r_dc_fail = new B737MIPAnn();
  ann_irs_r_dc_fail->SetParentRenderObject(this);
  ann_irs_r_dc_fail->SetPosition(10 + 4*deltax, m_PhysicalSize.y - deltay - 5 - 8*deltay);
  ann_irs_r_dc_fail->setAnnName((char*) "DC.FAIL");
  ann_irs_r_dc_fail->setAnnForm(0);
  ann_irs_r_dc_fail->setAnnFGColor(255,100,0);
  ann_irs_r_dc_fail->setAnnDataRefType2(2);
  ann_irs_r_dc_fail->setAnnDataRef2((char*) "laminar/B738/annunciator/irs_dc_fail_right");
  this->AddGaugeComponent(ann_irs_r_dc_fail);

}

B737MIP::~B737MIP()
{
  // Destruction handled by base class
}

void B737MIP::Render()
{
  int fontSize = 0.05*m_PhysicalSize.y;

  int acf_type = m_pDataSource->GetAcfType();
  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");

  if ((*avionics_on == 1) &&
      ((acf_type == 1) || (acf_type == 2) || (acf_type == 3))) {
   
    // after drawing the background, draw components
    Gauge::Render();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    /* split between MIP and AFT OVH */
    glColor3ub(COLOR_GRAY40);
    glBegin(GL_LINE_STRIP);
    glVertex2f( 0, 0.4*m_PhysicalSize.y);
    glVertex2f( m_PhysicalSize.x, 0.4*m_PhysicalSize.y);
    glEnd();

    /* IRS Position */
    if ((acf_type == 2) || (acf_type == 3)) {
      m_pFontManager->SetSize( m_Font, fontSize, fontSize );
      glColor3ub(COLOR_ORANGE);

      unsigned char *irs_pos = link_dataref_byte_arr("laminar/B738/irs/irs_pos",40,-1);
      m_pFontManager->Print( 30.0, m_PhysicalSize.y - 70.0, (const char*) irs_pos, m_Font);
    }
    
    
    // black rectangle of MIP display
    //    glColor3ub(COLOR_WHITE);
    //    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    //    glBegin(GL_POLYGON);
    //    glBegin(GL_LINE_LOOP);
    //    glVertex2f( 10, m_PhysicalSize.y - 4 );
    //    glVertex2f( 98, m_PhysicalSize.y - 4 );
    //glVertex2f( 98, 4 );
    //glVertex2f( 10, 4 );
    //    glVertex2f( 98, m_PhysicalSize.y - 68 );
    //    glVertex2f( 10, m_PhysicalSize.y - 68 );
    //    glEnd();

    glPopMatrix();

  }
}

} // end namespace OpenGC


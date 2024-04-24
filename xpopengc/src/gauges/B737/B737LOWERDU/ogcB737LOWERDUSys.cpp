/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737LOWERDUSys.cpp,v $

  Copyright (C) 2024 by:
  Original author:
  Reto Stockli
  Contributors (in alphabetical order):

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
  The Boeing 737 LOWER DU SYS Component
*/

#include <algorithm>
#include <stdio.h>
#include <math.h>

#include "ogcCircleEvaluator.h"
#include "ogcRREvaluator.h"
#include "B737/B737LOWERDU/ogcB737LOWERDUSys.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace OpenGC
{
  
  B737LOWERDUSys::B737LOWERDUSys()
  {
    printf("B737LOWERDUSys constructed\n");

    //  m_Font = m_pFontManager->LoadDefaultFont();
    m_Font = m_pFontManager->LoadFont((char*) "CockpitMCDU.ttf");
    
    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;
    
    m_PhysicalSize.x = 200;
    m_PhysicalSize.y = 200;
    
    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

  }

  B737LOWERDUSys::~B737LOWERDUSys()
  {
    
  }

  void B737LOWERDUSys::Render()
  {
    char buffer[15];
    float dx,dy,r;
    float x,y;
    float px,py;
    float fontSize;
    RREvaluator RR;
    
    GaugeComponent::Render();

    int acf_type = m_pDataSource->GetAcfType();
    int rotate = this->GetArg();
 
    float *engn_hydq1;
    float *engn_hydq2;
    if ((acf_type == 2) || (acf_type == 3)) {
      engn_hydq1 = link_dataref_flt("laminar/B738/hydraulic/hyd_A_qty",2);
      engn_hydq2 = link_dataref_flt("laminar/B738/hydraulic/hyd_B_qty",2);
    } else {
      engn_hydq1 = link_dataref_flt("sim/cockpit2/hydraulics/indicators/hydraulic_fluid_ratio_1",-2);
      engn_hydq2 = link_dataref_flt("sim/cockpit2/hydraulics/indicators/hydraulic_fluid_ratio_2",-2);
    }
    float *engn_hydp1;
    float *engn_hydp2;
    if (acf_type == 1) {
      engn_hydp1 = link_dataref_flt("x737/systems/hydraulics/systemAHydPress",2);
      engn_hydp2 = link_dataref_flt("x737/systems/hydraulics/systemBHydPress",2);
    } else if ((acf_type == 2) || (acf_type == 3)) {
      engn_hydp1 = link_dataref_flt("laminar/B738/hydraulic/A_pressure",2);
      engn_hydp2 = link_dataref_flt("laminar/B738/hydraulic/B_pressure",2);
    } else {
      engn_hydp1 = link_dataref_flt("sim/cockpit2/hydraulics/indicators/hydraulic_pressure_1",2);
      engn_hydp2 = link_dataref_flt("sim/cockpit2/hydraulics/indicators/hydraulic_pressure_2",2);
    }
   
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glLineWidth(m_PhysicalSize.x/70.0);

    // ROTATE WHOLE GAUGE IF NEEDED (if we are on rotated screen)
    glTranslatef(m_PhysicalSize.x/2, m_PhysicalSize.y/2, 0.0);
    glRotatef(rotate, 0, 0, 1);
    glTranslatef(-m_PhysicalSize.x/2, -m_PhysicalSize.y/2, 0.0);

    // HYDRAULICS
    glColor3ub(COLOR_LIGHTBLUE);
    r = m_PhysicalSize.x/60.0;
    dx = m_PhysicalSize.x*5.5/10.0;
    dy = m_PhysicalSize.y*1.7/10.0;
    x = m_PhysicalSize.x/2.0;
    y = m_PhysicalSize.x*8.5/10.0;
    fontSize = m_PhysicalSize.x/40;
    RR.SetRadius(r);
    RR.SetSize(dx,dy);
    RR.SetOrigin(x,y);
    RR.Evaluate();

    snprintf( buffer, sizeof(buffer), "QTY %%" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(x - 0.95*dx/2, y - 0.08*dy, &buffer[0], m_Font);     
    snprintf( buffer, sizeof(buffer), "PRESS" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(x - 0.95*dx/2, y - 0.39*dy, &buffer[0], m_Font);     
    snprintf( buffer, sizeof(buffer), "A" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(x - 0.2*dx/2, y + 0.20*dy, &buffer[0], m_Font);     
    snprintf( buffer, sizeof(buffer), "B" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(x + 0.5*dx/2, y + 0.20*dy, &buffer[0], m_Font);     
    
    glColor3ub(COLOR_WHITE);
    if (*engn_hydq1 != FLT_MISS) {
      if ((acf_type == 2) || (acf_type == 3)) {
	snprintf(buffer, sizeof(buffer), "%.0f", *engn_hydq1);
      } else {
	snprintf(buffer, sizeof(buffer), "%.0f", *engn_hydq1 * 100.0);
      }
      this->m_pFontManager->SetSize(m_Font, 1.25*fontSize, 1.25*fontSize);
      this->m_pFontManager->Print(x - 0.2*dx/2-1.25*fontSize/2.0, y - 0.08*dy, &buffer[0], m_Font);     
    }
    if (*engn_hydq2 != FLT_MISS) {
      if ((acf_type == 2) || (acf_type == 3)) {
	snprintf(buffer, sizeof(buffer), "%.0f", *engn_hydq2);
      } else {
	snprintf(buffer, sizeof(buffer), "%.0f", *engn_hydq2 * 100.0);
      }
      this->m_pFontManager->SetSize(m_Font, 1.25*fontSize, 1.25*fontSize);
      this->m_pFontManager->Print(x + 0.5*dx/2-1.25*fontSize/2.0, y - 0.08*dy, &buffer[0], m_Font);     
    }
    if (*engn_hydp1 != FLT_MISS) {
      snprintf(buffer, sizeof(buffer), "%.0f", *engn_hydp1);
      this->m_pFontManager->SetSize(m_Font, 1.25*fontSize, 1.25*fontSize);
      this->m_pFontManager->Print(x - 0.2*dx/2-1.25*fontSize*1.25, y - 0.39*dy, &buffer[0], m_Font);     
    }
    if (*engn_hydp2 != FLT_MISS) {
      snprintf(buffer, sizeof(buffer), "%.0f", *engn_hydp2);
      this->m_pFontManager->SetSize(m_Font, 1.25*fontSize, 1.25*fontSize);
      this->m_pFontManager->Print(x + 0.5*dx/2-1.25*fontSize*1.25, y - 0.39*dy, &buffer[0], m_Font);     
    }
    
    glColor3ub(COLOR_BLACK);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_POLYGON);
    dx = 0.37*m_PhysicalSize.x*5.5/10.0;
    dy = 0.2*m_PhysicalSize.y*1.7/10.0;
    x = m_PhysicalSize.x/2.0;
    y = m_PhysicalSize.x*8.5/10.0+m_PhysicalSize.y*1.7/10.0/2.0;
    glVertex2f(x-0.5*dx,y-0.5*dy);
    glVertex2f(x+0.5*dx,y-0.5*dy);
    glVertex2f(x+0.5*dx,y+0.5*dy);
    glVertex2f(x-0.5*dx,y+0.5*dy);
    glEnd();

    glColor3ub(COLOR_LIGHTBLUE);
    snprintf( buffer, sizeof(buffer), "HYDRAULIC" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(x - 0.45*dx, y - 0.40*dy, &buffer[0], m_Font);     
  
    // WHEELS and BRAKES
    glColor3ub(COLOR_WHITE);
    glLineWidth(m_PhysicalSize.x/70.0);

    r = m_PhysicalSize.x/45.0;
    dx = m_PhysicalSize.x/16.0;
    dy = m_PhysicalSize.y/10.0;
    y = m_PhysicalSize.x*6.0/10.0;
    fontSize = m_PhysicalSize.x/40;
    RR.SetRadius(r);
    RR.SetSize(dx,dy);

    RR.SetOrigin(m_PhysicalSize.x*2.05/10.0,y);
    RR.Evaluate();
    RR.SetOrigin(m_PhysicalSize.x*3.00/10.0,y);
    RR.Evaluate();
    RR.SetOrigin(m_PhysicalSize.x*7.00/10.0,y);
    RR.Evaluate();
    RR.SetOrigin(m_PhysicalSize.x*7.95/10.0,y);
    RR.Evaluate();
    
    glBegin(GL_LINES);
    glVertex2f(m_PhysicalSize.x*2.05/10.0+dx/2.0,y);
    glVertex2f(m_PhysicalSize.x*3.00/10.0-dx/2.0,y);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(m_PhysicalSize.x*7.00/10.0+dx/2.0,y);
    glVertex2f(m_PhysicalSize.x*7.95/10.0-dx/2.0,y);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(m_PhysicalSize.x*2.05/10.0-dx/2.0,y+dy/2.0-r);
    glVertex2f(m_PhysicalSize.x*2.05/10.0-dx/2.0-m_PhysicalSize.x/80.0,y+dy/2.0-r);
    glVertex2f(m_PhysicalSize.x*2.05/10.0-dx/2.0-m_PhysicalSize.x/80.0,y-dy/2.0+r);
    glVertex2f(m_PhysicalSize.x*2.05/10.0-dx/2.0,y-dy/2.0+r);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(m_PhysicalSize.x*3.00/10.0+dx/2.0,y+dy/2.0-r);
    glVertex2f(m_PhysicalSize.x*3.00/10.0+dx/2.0+m_PhysicalSize.x/80.0,y+dy/2.0-r);
    glVertex2f(m_PhysicalSize.x*3.00/10.0+dx/2.0+m_PhysicalSize.x/80.0,y-dy/2.0+r);
    glVertex2f(m_PhysicalSize.x*3.00/10.0+dx/2.0,y-dy/2.0+r);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(m_PhysicalSize.x*7.00/10.0-dx/2.0,y+dy/2.0-r);
    glVertex2f(m_PhysicalSize.x*7.00/10.0-dx/2.0-m_PhysicalSize.x/80.0,y+dy/2.0-r);
    glVertex2f(m_PhysicalSize.x*7.00/10.0-dx/2.0-m_PhysicalSize.x/80.0,y-dy/2.0+r);
    glVertex2f(m_PhysicalSize.x*7.00/10.0-dx/2.0,y-dy/2.0+r);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(m_PhysicalSize.x*7.95/10.0+dx/2.0,y+dy/2.0-r);
    glVertex2f(m_PhysicalSize.x*7.95/10.0+dx/2.0+m_PhysicalSize.x/80.0,y+dy/2.0-r);
    glVertex2f(m_PhysicalSize.x*7.95/10.0+dx/2.0+m_PhysicalSize.x/80.0,y-dy/2.0+r);
    glVertex2f(m_PhysicalSize.x*7.95/10.0+dx/2.0,y-dy/2.0+r);
    glEnd();

    if ((acf_type == 2) || (acf_type == 3)) {
      glColor3ub(COLOR_WHITE);
      float *brake_temp_left_out = link_dataref_flt("laminar/B738/systems/brake_temp_left_out",2);
      if (*brake_temp_left_out != FLT_MISS) {
	snprintf(buffer, sizeof(buffer), "%.1f", *brake_temp_left_out);
	this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
	this->m_pFontManager->Print(m_PhysicalSize.x*2.05/10.0-dx-2.2*fontSize, y-0.5*fontSize, &buffer[0], m_Font);
      }
      float *brake_temp_left_in = link_dataref_flt("laminar/B738/systems/brake_temp_left_in",2);
      if (*brake_temp_left_in != FLT_MISS) {
	snprintf(buffer, sizeof(buffer), "%.1f", *brake_temp_left_in);
	this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
	this->m_pFontManager->Print(m_PhysicalSize.x*3.0/10.0+dx, y-0.5*fontSize, &buffer[0], m_Font);
      }
      float *brake_temp_right_in = link_dataref_flt("laminar/B738/systems/brake_temp_right_in",2);
      if (*brake_temp_right_in != FLT_MISS) {
	snprintf(buffer, sizeof(buffer), "%.1f", *brake_temp_right_in);
	this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
	this->m_pFontManager->Print(m_PhysicalSize.x*7.0/10.0-dx-2.2*fontSize, y-0.5*fontSize, &buffer[0], m_Font);
      }
      float *brake_temp_right_out = link_dataref_flt("laminar/B738/systems/brake_temp_right_out",2);
      if (*brake_temp_right_out != FLT_MISS) {
	snprintf(buffer, sizeof(buffer), "%.1f", *brake_temp_right_out);
	this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
	this->m_pFontManager->Print(m_PhysicalSize.x*7.95/10.0+dx, y-0.5*fontSize, &buffer[0], m_Font);
      }
    }

    glColor3ub(COLOR_LIGHTBLUE);
    y = m_PhysicalSize.x*6.7/10.0;

    glBegin(GL_LINES);
    glVertex2f(m_PhysicalSize.x*2.55/10.0+dx/2.0,y+fontSize/2.0);
    glVertex2f(m_PhysicalSize.x*4.20/10.0-dx/2.0,y+fontSize/2.0);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(m_PhysicalSize.x*5.9/10.0+dx/2.0,y+fontSize/2.0);
    glVertex2f(m_PhysicalSize.x*7.6/10.0-dx/2.0,y+fontSize/2.0);
    glEnd();

    snprintf( buffer, sizeof(buffer), "BRAKE TEMP" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(m_PhysicalSize.x/2 - fontSize*4, y, &buffer[0], m_Font);     
    snprintf( buffer, sizeof(buffer), "L" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(m_PhysicalSize.x*2.5/10.0, y, &buffer[0], m_Font);     
    snprintf( buffer, sizeof(buffer), "R" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(m_PhysicalSize.x*7.4/10.0, y, &buffer[0], m_Font);     

    // FLIGHT CONTROLS
 
    // ELEVATOR
    float *elev_dn = link_dataref_flt("sim/aircraft/controls/acf_elev_dn",-2);
    float *elev_up = link_dataref_flt("sim/aircraft/controls/acf_elev_up",-2);
    float *elev = link_dataref_flt_arr("sim/flightmodel2/wing/elevator1_deg",32,0,-1);
    glColor3ub(COLOR_WHITE);
    glLineWidth(m_PhysicalSize.x/70.0);
    x = 0.5*m_PhysicalSize.x;
    y = 0.4*m_PhysicalSize.y;
    dx = 0.015*m_PhysicalSize.x;
    dy = 0.2*m_PhysicalSize.y;
    px = 0.0;
    if ((*elev != FLT_MISS) && (*elev_up != FLT_MISS) && (*elev_dn != FLT_MISS)) {
      if (*elev < 0.0) {
	py = *elev / *elev_up*0.5*dy;
      } else {
	py = *elev / *elev_dn*0.5*dy;
      }
    } else {
      py = 0.0;
    }
    glBegin(GL_LINES);
    glVertex2f(x,y+0.5*dy);
    glVertex2f(x,y-0.5*dy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y);
    glVertex2f(x-0.5*dx,y);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y+0.5*dy);
    glVertex2f(x-0.5*dx,y+0.5*dy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y-0.5*dy);
    glVertex2f(x-0.5*dx,y-0.5*dy);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(x-0.75*dx,y+py);
    glVertex2f(x-1.75*dx,y+0.7*dx+py);
    glVertex2f(x-1.75*dx,y-0.7*dx+py);
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(x+0.75*dx,y+py);
    glVertex2f(x+1.75*dx,y+0.7*dx+py);
    glVertex2f(x+1.75*dx,y-0.7*dx+py);
    glEnd();

    glColor3ub(COLOR_LIGHTBLUE);
    snprintf( buffer, sizeof(buffer), "ELEV" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(x-fontSize*1.5,y-0.75*dy, &buffer[0], m_Font);     

    // RUDDER
    float *rudder_left = link_dataref_flt("sim/aircraft/controls/acf_rudd_lr",-2);
    float *rudder_right = link_dataref_flt("sim/aircraft/controls/acf_rudd_rr",-2);
    float *rudder = link_dataref_flt_arr("sim/flightmodel2/wing/rudder1_deg",32,0,-1);
    glColor3ub(COLOR_WHITE);
    glLineWidth(m_PhysicalSize.x/70.0);
    x = 0.5*m_PhysicalSize.x;
    y = 0.20*m_PhysicalSize.y;
    dx = 0.2*m_PhysicalSize.x;
    dy = 0.015*m_PhysicalSize.y;
    if ((*rudder != FLT_MISS) && (*rudder_left != FLT_MISS) && (*rudder_right != FLT_MISS)) {
      if (*rudder < 0.0) {
	px = *rudder / *rudder_left*0.5*dx;
      } else {
	px = *rudder / *rudder_right*0.5*dx;
      }
    } else {
      px = 0.0;
    }
    py = 0.0;
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y);
    glVertex2f(x-0.5*dx,y);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x,y+0.5*dy);
    glVertex2f(x,y-0.5*dy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y+0.5*dy);
    glVertex2f(x+0.5*dx,y-0.5*dy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x-0.5*dx,y+0.5*dy);
    glVertex2f(x-0.5*dx,y-0.5*dy);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(x+px,y-0.75*dy);
    glVertex2f(x+0.7*dy+px,y-1.75*dy);
    glVertex2f(x-0.7*dy+px,y-1.75*dy);
    glEnd();

    glColor3ub(COLOR_LIGHTBLUE);
    snprintf( buffer, sizeof(buffer), "RUDDER" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(x-fontSize*2.5,y-0.35*dx, &buffer[0], m_Font);     

    // NOSE WHEEL
    float *nosewheel_max = link_dataref_flt("sim/aircraft/gear/acf_nw_steerdeg1",-2);
    float *nosewheel = link_dataref_flt_arr("sim/flightmodel2/gear/tire_steer_actual_deg",10,0,-1);

    glColor3ub(COLOR_WHITE);
    glLineWidth(m_PhysicalSize.x/70.0);
    x = 0.5*m_PhysicalSize.x;
    y = 0.1*m_PhysicalSize.y;
    dx = 0.2*m_PhysicalSize.x;
    dy = 0.015*m_PhysicalSize.y;
    if ((*nosewheel != FLT_MISS) && (*nosewheel_max != FLT_MISS)) {
      px = -min(max(*nosewheel / *nosewheel_max,-1.0f),1.0f)*0.5*dx;
    } else {
      px = 0.0;
    }
    py = 0.0;
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y);
    glVertex2f(x-0.5*dx,y);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x,y+0.5*dy);
    glVertex2f(x,y-0.5*dy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y+0.5*dy);
    glVertex2f(x+0.5*dx,y-0.5*dy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x-0.5*dx,y+0.5*dy);
    glVertex2f(x-0.5*dx,y-0.5*dy);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(x+px,y-0.75*dy);
    glVertex2f(x+0.7*dy+px,y-1.75*dy);
    glVertex2f(x-0.7*dy+px,y-1.75*dy);
    glEnd();

    glColor3ub(COLOR_LIGHTBLUE);
    snprintf( buffer, sizeof(buffer), "NOSE WHEEL" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(x-fontSize*4.0,y-0.35*dx, &buffer[0], m_Font);     

    // LEFT AILERON
    float *ail_left_dn = link_dataref_flt("sim/aircraft/controls/acf_ail1_dn",-2);
    float *ail_left_up = link_dataref_flt("sim/aircraft/controls/acf_ail1_up",-2);
    float *ail_left = link_dataref_flt_arr("sim/flightmodel2/wing/aileron1_deg",32,0,-1);
    
    glColor3ub(COLOR_WHITE);
    glLineWidth(m_PhysicalSize.x/70.0);
    x = 0.125*m_PhysicalSize.x;
    y = 0.4*m_PhysicalSize.y;
    dx = 0.015*m_PhysicalSize.x;
    dy = 0.2*m_PhysicalSize.y;
    px = 0.0;
    if ((*ail_left != FLT_MISS) && (*ail_left_up != FLT_MISS) && (*ail_left_dn != FLT_MISS)) {
      if (*ail_left < 0.0) {
	py = -*ail_left / *ail_left_up*0.5*dy;
      } else {
	py = -*ail_left / *ail_left_dn*0.5*dy;
      }
    } else {
      py = 0.0;
    }
    glBegin(GL_LINES);
    glVertex2f(x,y+0.5*dy);
    glVertex2f(x,y-0.5*dy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y);
    glVertex2f(x-0.5*dx,y);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y+0.5*dy);
    glVertex2f(x-0.5*dx,y+0.5*dy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y-0.5*dy);
    glVertex2f(x-0.5*dx,y-0.5*dy);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(x-0.75*dx,y+py);
    glVertex2f(x-1.75*dx,y+0.7*dx+py);
    glVertex2f(x-1.75*dx,y-0.7*dx+py);
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(x+0.75*dx,y+py);
    glVertex2f(x+1.75*dx,y+0.7*dx+py);
    glVertex2f(x+1.75*dx,y-0.7*dx+py);
    glEnd();

    glColor3ub(COLOR_LIGHTBLUE);
    snprintf( buffer, sizeof(buffer), "AIL" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(x-fontSize*1.15,y-0.75*dy, &buffer[0], m_Font);     
    
    // LEFT FLT SPOILER
    float *spoil_left_up = link_dataref_flt("sim/aircraft/controls/acf_splr_up",-2);
    float *spoil_left = link_dataref_flt_arr("sim/flightmodel2/wing/spoiler1_deg",32,0,-1);
    
    glColor3ub(COLOR_WHITE);
    glLineWidth(m_PhysicalSize.x/70.0);
    x = 0.250*m_PhysicalSize.x;
    y = 0.45*m_PhysicalSize.y;
    dx = 0.015*m_PhysicalSize.x;
    dy = 0.1*m_PhysicalSize.y;
    px = 0.0;
    if ((*spoil_left != FLT_MISS) && (*spoil_left_up != FLT_MISS)) {
      py = *spoil_left / *spoil_left_up*dy;
    } else {
      py = 0.0;
    }
    glBegin(GL_LINES);
    glVertex2f(x,y+0.5*dy);
    glVertex2f(x,y-0.5*dy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y+0.5*dy);
    glVertex2f(x-0.5*dx,y+0.5*dy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y-0.5*dy);
    glVertex2f(x-0.5*dx,y-0.5*dy);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(x-0.75*dx,y-0.5*dy+py);
    glVertex2f(x-1.75*dx,y-0.5*dy+0.7*dx+py);
    glVertex2f(x-1.75*dx,y-0.5*dy-0.7*dx+py);
    glEnd();

    glColor3ub(COLOR_LIGHTBLUE);
    snprintf( buffer, sizeof(buffer), "FLT" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(x-fontSize*1.15,y-1.0*dy, &buffer[0], m_Font);     
    snprintf( buffer, sizeof(buffer), "SPLR" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(x-fontSize*1.5,y-1.4*dy, &buffer[0], m_Font);     
    
    // RIGHT AILERON
    float *ail_right_dn = link_dataref_flt("sim/aircraft/controls/acf_ail1_dn",-2);
    float *ail_right_up = link_dataref_flt("sim/aircraft/controls/acf_ail1_up",-2);
    float *ail_right = link_dataref_flt_arr("sim/flightmodel2/wing/aileron1_deg",32,1,-1);
    glColor3ub(COLOR_WHITE);
    glLineWidth(m_PhysicalSize.x/70.0);
    x = 0.875*m_PhysicalSize.x;
    y = 0.4*m_PhysicalSize.y;
    dx = 0.015*m_PhysicalSize.x;
    dy = 0.2*m_PhysicalSize.y;
    px = 0.0;
    if ((*ail_right != FLT_MISS) && (*ail_right_up != FLT_MISS) && (*ail_right_dn != FLT_MISS)) {
      if (*ail_right < 0.0) {
	py = -*ail_right / *ail_right_up*0.5*dy;
      } else {
	py = -*ail_right / *ail_right_dn*0.5*dy;
      }
    } else {
      py = 0.0;
    }
    glBegin(GL_LINES);
    glVertex2f(x,y+0.5*dy);
    glVertex2f(x,y-0.5*dy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y);
    glVertex2f(x-0.5*dx,y);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y+0.5*dy);
    glVertex2f(x-0.5*dx,y+0.5*dy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y-0.5*dy);
    glVertex2f(x-0.5*dx,y-0.5*dy);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(x-0.75*dx,y+py);
    glVertex2f(x-1.75*dx,y+0.7*dx+py);
    glVertex2f(x-1.75*dx,y-0.7*dx+py);
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(x+0.75*dx,y+py);
    glVertex2f(x+1.75*dx,y+0.7*dx+py);
    glVertex2f(x+1.75*dx,y-0.7*dx+py);
    glEnd();

    glColor3ub(COLOR_LIGHTBLUE);
    snprintf( buffer, sizeof(buffer), "AIL" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(x-fontSize*1.15,y-0.75*dy, &buffer[0], m_Font);     

    // RIGHT FLT SPOILER
    float *spoil_right_up = link_dataref_flt("sim/aircraft/controls/acf_splr_up",-2);
    float *spoil_right = link_dataref_flt_arr("sim/flightmodel2/wing/spoiler1_deg",32,1,-1);

    glColor3ub(COLOR_WHITE);
    glLineWidth(m_PhysicalSize.x/70.0);
    x = 0.750*m_PhysicalSize.x;
    y = 0.45*m_PhysicalSize.y;
    dx = 0.015*m_PhysicalSize.x;
    dy = 0.1*m_PhysicalSize.y;
    px = 0.0;
    if ((*spoil_right != FLT_MISS) && (*spoil_right_up != FLT_MISS)) {
      py = *spoil_right / *spoil_right_up*dy;
    } else {
      py = 0.0;
    }
    glBegin(GL_LINES);
    glVertex2f(x,y+0.5*dy);
    glVertex2f(x,y-0.5*dy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y+0.5*dy);
    glVertex2f(x-0.5*dx,y+0.5*dy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x+0.5*dx,y-0.5*dy);
    glVertex2f(x-0.5*dx,y-0.5*dy);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(x+0.75*dx,y-0.5*dy+py);
    glVertex2f(x+1.75*dx,y-0.5*dy+0.7*dx+py);
    glVertex2f(x+1.75*dx,y-0.5*dy-0.7*dx+py);
    glEnd();

    glColor3ub(COLOR_LIGHTBLUE);
    snprintf( buffer, sizeof(buffer), "FLT" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(x-fontSize*1.15,y-1.0*dy, &buffer[0], m_Font);     
    snprintf( buffer, sizeof(buffer), "SPLR" );
    this->m_pFontManager->SetSize(m_Font, fontSize, fontSize);
    this->m_pFontManager->Print(x-fontSize*1.5,y-1.4*dy, &buffer[0], m_Font);     
       
    glPopMatrix();
    
  }

}

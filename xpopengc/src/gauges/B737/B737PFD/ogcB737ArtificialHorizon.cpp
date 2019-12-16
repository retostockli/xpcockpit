/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737ArtificialHorizon.cpp,v $

  Copyright (C) 2001-2015 by:
  Original author:
  Damion Shelton
  Contributors (in alphabetical order):
  Reto Stockli

  Last modification:
  Date:      $Date: 2015/11/24 $
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

#include <stdio.h>

#include "ogcGaugeComponent.h"
#include "ogcCircleEvaluator.h"
#include "ogcB737ArtificialHorizon.h"

namespace OpenGC
{

  B737ArtificialHorizon::B737ArtificialHorizon()
  {
    printf("B737ArtificialHorizon constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 94;
    m_PhysicalSize.y = 98;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  B737ArtificialHorizon::~B737ArtificialHorizon()
  {

  }

  void B737ArtificialHorizon::Render()
  {
    // Call base class to setup viewport and projection
    GaugeComponent::Render();

    // For drawing circles
    CircleEvaluator aCircle;

    // First, store the "root" position of the gauge component
    glMatrixMode(GL_MODELVIEW);

    bool is_captain = (this->GetArg() == 1);
    bool is_copilot = (this->GetArg() == 2);
    
    int acf_type = m_pDataSource->GetAcfType();
    
    float *pitch = link_dataref_flt("sim/flightmodel/position/theta",-1);
    float *roll = link_dataref_flt("sim/flightmodel/position/phi",-1);

    int *fd_a_status;
    int *fd_b_status;
    float *fd_a_pitch;
    float *fd_b_pitch;
    float *fd_a_roll;
    float *fd_b_roll;
    if (acf_type == 1) {
      fd_a_status = link_dataref_int("x737/systems/afds/fdA_status");   
      fd_b_status = link_dataref_int("x737/systems/afds/fdB_status");
      fd_a_pitch = link_dataref_flt("x737/systems/afds/AP_A_pitch",0);
      fd_b_pitch = link_dataref_flt("x737/systems/afds/AP_B_pitch",0);
      fd_a_roll = link_dataref_flt("x737/systems/afds/AP_A_roll",-1);
      fd_b_roll = link_dataref_flt("x737/systems/afds/AP_B_roll",-1);
    } else {
      fd_a_status = link_dataref_int("sim/cockpit/autopilot/autopilot_mode");   
      fd_b_status = link_dataref_int("sim/cockpit/autopilot/autopilot_mode");
      fd_a_pitch = link_dataref_flt("sim/cockpit/autopilot/flight_director_pitch",0);
      fd_b_pitch = link_dataref_flt("sim/cockpit/autopilot/flight_director_pitch",0);
      fd_a_roll = link_dataref_flt("sim/cockpit/autopilot/flight_director_roll",0);
      fd_b_roll = link_dataref_flt("sim/cockpit/autopilot/flight_director_roll",0);
    }

    int *nav1_has_glideslope = link_dataref_int("sim/cockpit2/radios/indicators/nav1_flag_glideslope");
    float *nav1_hdef = link_dataref_flt("sim/cockpit/radios/nav1_hdef_dot",-2);
    float *nav1_vdef = link_dataref_flt("sim/cockpit/radios/nav1_vdef_dot",-2);
    //    int *nav2_has_glideslope = link_dataref_int("sim/cockpit2/radios/indicators/nav2_flag_glideslope");
    //    float *nav2_hdef = link_dataref_flt("sim/cockpit/radios/nav2_hdef_dot",-2);
    //    float *nav2_vdef = link_dataref_flt("sim/cockpit/radios/nav2_vdef_dot",-2);
   
    if ((*roll != FLT_MISS) && (*pitch != FLT_MISS)) {

      glPushMatrix();

      // Move to the center of the window
      glTranslatef(47,49,0);

      // Rotate based on the bank
      glRotatef(*roll, 0, 0, 1);

      // Translate in the direction of the rotation based
      // on the pitch. On the 737, a pitch of 1 degree = 2 mm
      glTranslatef(0, *pitch * -2.0, 0);

      //-------------------Gauge Background------------------
      // It's drawn oversize to allow for pitch and bank

      // The "ground" rectangle
      // Remember, the coordinate system is now centered in the gauge component
      glColor3ub(179,102,0);

      glBegin(GL_POLYGON);
      glVertex2f(-300,-300);
      glVertex2f(-300,0);
      glVertex2f(300,0);
      glVertex2f(300,-300);
      glVertex2f(-300,-300);
      glEnd();

      // The "sky" rectangle
      // Remember, the coordinate system is now centered in the gauge component
      glColor3ub(0,153,204);
  
      glBegin(GL_POLYGON);
      glVertex2f(-300,0);
      glVertex2f(-300,300);
      glVertex2f(300,300);
      glVertex2f(300,0);
      glVertex2f(-300,0);
      glEnd();


      //------------Draw the pitch markings--------------

      // Draw in white
      glColor3ub(255,255,255);
      // Specify line width
      glLineWidth(1.0);
      // The size for all pitch text
      m_pFontManager->SetSize(m_Font,4.0, 4.0);

      glBegin(GL_LINES);

      // The dividing line between sky and ground
      glVertex2f(-100,0);
      glVertex2f(100,0);

      // +2.5 degrees
      glVertex2f(-5,5);
      glVertex2f(5,5);

      // +5.0 degrees
      glVertex2f(-10,10);
      glVertex2f(10,10);

      // +7.5 degrees
      glVertex2f(-5,15);
      glVertex2f(5,15);

      // +10.0 degrees
      glVertex2f(-20,20);
      glVertex2f(20,20);

      // +12.5 degrees
      glVertex2f(-5,25);
      glVertex2f(5,25);

      // +15.0 degrees
      glVertex2f(-10,30);
      glVertex2f(10,30);

      // +17.5 degrees
      glVertex2f(-5,35);
      glVertex2f(5,35);

      // +20.0 degrees
      glVertex2f(-20,40);
      glVertex2f(20,40);

      // -2.5 degrees
      glVertex2f(-5,-5);
      glVertex2f(5,-5);

      // -5.0 degrees
      glVertex2f(-10,-10);
      glVertex2f(10,-10);

      // -7.5 degrees
      glVertex2f(-5,-15);
      glVertex2f(5,-15);

      // -10.0 degrees
      glVertex2f(-20,-20);
      glVertex2f(20,-20);

      // -12.5 degrees
      glVertex2f(-5,-25);
      glVertex2f(5,-25);

      // -15.0 degrees
      glVertex2f(-10,-30);
      glVertex2f(10,-30);

      // -17.5 degrees
      glVertex2f(-5,-35);
      glVertex2f(5,-35);

      // -20.0 degrees
      glVertex2f(-20,-40);
      glVertex2f(20,-40);

      glEnd();

      // +10
      m_pFontManager->Print(-27.5,18.0,"10",m_Font);
      m_pFontManager->Print(21.0,18.0,"10",m_Font);

      // -10
      m_pFontManager->Print(-27.5,-22.0,"10",m_Font);
      m_pFontManager->Print(21.0,-22.0,"10",m_Font);

      // +20
      m_pFontManager->Print(-27.5,38.0,"20",m_Font);
      m_pFontManager->Print(21.0,38.0,"20",m_Font);

      // -20
      m_pFontManager->Print(-27.5,-42.0,"20",m_Font);
      m_pFontManager->Print(21.0,-42.0,"20",m_Font);

      glPopMatrix();

      //-----The background behind the bank angle markings-------
      // Reset the modelview matrix
      glPushMatrix();

      // Draw in the sky color
      glColor3ub(0,153,204);

      aCircle.SetOrigin(47,49);
      aCircle.SetRadius(46);
      aCircle.SetDegreesPerPoint(5);
      aCircle.SetArcStartEnd(300.0,360.0);

      glBegin(GL_TRIANGLE_FAN);
      glVertex2f(0,98);
      glVertex2f(0,72);
      aCircle.Evaluate();
      glVertex2f(47,98);
      glEnd();

      aCircle.SetArcStartEnd(0.0,60.0);
      glBegin(GL_TRIANGLE_FAN);
      glVertex2f(94,98);
      glVertex2f(47,98);
      aCircle.Evaluate();
      glVertex2f(94,72);
      glEnd();

      glPopMatrix();

      //----------------The bank angle markings----------------

      // Left side bank markings
      // Reset the modelview matrix
      glPushMatrix();

      // Draw in white
      glColor3ub(255,255,255);

      // Move to the center of the window
      glTranslatef(47,49,0);

      // Draw the center detent
      glBegin(GL_POLYGON);
      glVertex2f(0.0f,46.0f);
      glVertex2f(-2.3f,49.0f);
      glVertex2f(2.3f,49.0f);
      glVertex2f(0.0f,46.0f);
      glEnd();

      glRotatef(10.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,49);
      glEnd();
  
      glRotatef(10.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,49);
      glEnd();

      glRotatef(10.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,53);
      glEnd();

      glRotatef(15.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,49);
      glEnd();

      glRotatef(15.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,51);
      glEnd();

      glPopMatrix();

      // Right side bank markings
      // Reset the modelview matrix
      glPushMatrix();
      // Move to the center of the window
      glTranslatef(47,49,0);

      glRotatef(-10.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,49);
      glEnd();
  
      glRotatef(-10.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,49);
      glEnd();

      glRotatef(-10.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,53);
      glEnd();

      glRotatef(-15.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,49);
      glEnd();

      glRotatef(-15.0,0,0,1);
      glBegin(GL_LINES);
      glVertex2f(0,46);
      glVertex2f(0,51);
      glEnd();

      glPopMatrix();

      //----------------End Draw Bank Markings----------------


      //----------------Bank Indicator----------------
      // Reset the modelview matrix
      glPushMatrix();
      // Move to the center of the window
      glTranslatef(47,49,0);
      // Rotate based on the bank
      glRotatef(*roll, 0, 0, 1);

      // Draw in white
      glColor3ub(255,255,255);
      // Specify line width
      glLineWidth(2.0);

      glBegin(GL_LINE_LOOP); // the bottom rectangle
      glVertex2f(-4.5, 39.5);
      glVertex2f(4.5, 39.5);
      glVertex2f(4.5, 41.5);
      glVertex2f(-4.5, 41.5);
      glEnd();

      glBegin(GL_LINE_STRIP); // the top triangle
      glVertex2f(-4.5, 41.5);
      glVertex2f(0, 46);
      glVertex2f(4.5, 41.5);
      glEnd();

      glPopMatrix();
      //--------------End draw bank indicator------------

      //----------------Attitude Indicator----------------
      // Reset the modelview matrix
      glPushMatrix();
      // Move to the center of the window
      glTranslatef(47,49,0);

      // The center axis indicator
      // Black background
      glColor3ub(0,0,0);
      glBegin(GL_POLYGON);
      glVertex2f(1.25,1.25);
      glVertex2f(1.25,-1.25);
      glVertex2f(-1.25,-1.25);
      glVertex2f(-1.25,1.25);
      glVertex2f(1.25,1.25);
      glEnd();
      // White lines
      glColor3ub(255,255,255);
      glLineWidth(2.0);
      glBegin(GL_LINE_LOOP);
      glVertex2f(1.25,1.25);
      glVertex2f(1.25,-1.25);
      glVertex2f(-1.25,-1.25);
      glVertex2f(-1.25,1.25);
      glEnd();

      // The left part
      // Black background
      glColor3ub(0,0,0);
      glBegin(GL_POLYGON);
      glVertex2f(-39,1.25);
      glVertex2f(-19,1.25);
      glVertex2f(-19,-1.25);
      glVertex2f(-39,-1.25);
      glVertex2f(-39,1.25);
      glEnd();
      glBegin(GL_POLYGON);
      glVertex2f(-19,1.25);
      glVertex2f(-19,-5.75);
      glVertex2f(-22,-5.75);
      glVertex2f(-22,1.25);
      glVertex2f(-19,1.25);
      glEnd();
  
      // White lines
      glColor3ub(255,255,255);
      glLineWidth(2.0);
      glBegin(GL_LINE_LOOP);
      glVertex2f(-39,1.25);
      glVertex2f(-19,1.25);
      glVertex2f(-19,-5.75);
      glVertex2f(-22,-5.75);
      glVertex2f(-22,-1.25);
      glVertex2f(-39,-1.25);
      glEnd();

      // The right part
      // Black background
      glColor3ub(0,0,0);
      glBegin(GL_POLYGON);
      glVertex2f(39,1.25);
      glVertex2f(19,1.25);
      glVertex2f(19,-1.25);
      glVertex2f(39,-1.25);
      glVertex2f(39,1.25);
      glEnd();
      glBegin(GL_POLYGON);
      glVertex2f(19,1.25);
      glVertex2f(19,-5.75);
      glVertex2f(22,-5.75);
      glVertex2f(22,1.25);
      glVertex2f(19,1.25);
      glEnd();
  
      // White lines
      glColor3ub(255,255,255);
      glLineWidth(2.0);
      glBegin(GL_LINE_LOOP);
      glVertex2f(39,1.25);
      glVertex2f(19,1.25);
      glVertex2f(19,-5.75);
      glVertex2f(22,-5.75);
      glVertex2f(22,-1.25);
      glVertex2f(39,-1.25);
      glEnd();

      glPopMatrix();
      //--------------End draw attitude indicator------------
      // Reset the modelview matrix
      glPushMatrix();

      aCircle.SetRadius(3.77);
      //-------------------Rounded corners------------------
      // The corners of the artificial horizon are rounded off by
      // drawing over them in black. The overlays are essentially the
      // remainder of a circle subtracted from a square, and are formed
      // by fanning out triangles from a point just off each corner
      // to an arc descrbing the curved portion of the art. horiz.

      glColor3ub(0,0,0);
      // Lower left
      glBegin(GL_TRIANGLE_FAN);
      glVertex2f(-1.0,-1.0);
      aCircle.SetOrigin(3.77,3.77);
      aCircle.SetArcStartEnd(180,270);
      aCircle.SetDegreesPerPoint(15);
      aCircle.Evaluate();
      glEnd();
      // Upper left
      glBegin(GL_TRIANGLE_FAN);
      glVertex2f(-1.0,99.0);
      aCircle.SetOrigin(3.77,94.23);
      aCircle.SetArcStartEnd(270,360);
      aCircle.SetDegreesPerPoint(15);
      aCircle.Evaluate();
      glEnd();
      // Upper right
      glBegin(GL_TRIANGLE_FAN);
      glVertex2f(95.0,99.0);
      aCircle.SetOrigin(90.23,94.23);
      aCircle.SetArcStartEnd(0,90);
      aCircle.SetDegreesPerPoint(15);
      aCircle.Evaluate();
      glEnd();
      //Lower right
      glBegin(GL_TRIANGLE_FAN);
      glVertex2f(95.0,-1);
      aCircle.SetOrigin(90.23,3.77);
      aCircle.SetArcStartEnd(90,180);
      aCircle.SetDegreesPerPoint(15);
      aCircle.Evaluate();
      glEnd();

      // Finally, restore the modelview matrix to what we received
      glPopMatrix();

    }

    //----------------Flight Director----------------
    
    if ((((*fd_a_status == 1) || (*fd_a_status == 2)) && (is_captain) &&
	 (*fd_a_roll != FLT_MISS) && (*fd_a_pitch != FLT_MISS)) ||
	(((*fd_b_status == 1) || (*fd_b_status == 2)) && (is_copilot) &&
	 (*fd_b_roll != FLT_MISS) && (*fd_b_pitch != FLT_MISS))) 
      {
	// Move to the center of the window
	glTranslatef(47,49,0);
	glColor3ub( 210, 5,  210 );
	glLineWidth(3.0);
    
	glPushMatrix();
	if (((*fd_a_status == 1) || (*fd_a_status == 2)) && (is_captain)) {
	  glTranslatef(0,*fd_a_pitch*2.0,0);
	}
	if (((*fd_b_status == 1) || (*fd_b_status == 2)) && (is_copilot)) {
	  glTranslatef(0,*fd_b_pitch*2.0,0);
	}
	glBegin(GL_LINES);
	glVertex2f(-20,0);
	glVertex2f(20,0);
	glEnd();

	glPopMatrix();
    
	glPushMatrix();
	if (((*fd_a_status == 1) || (*fd_a_status == 2)) && (is_captain)) {
	  glTranslatef(*fd_a_roll,0,0);
	}
	if (((*fd_b_status == 1) || (*fd_b_status == 2)) && (is_copilot)) {
	  glTranslatef(*fd_b_roll,0,0);
	}
	glBegin(GL_LINES);
	glVertex2f(0,-20);
	glVertex2f(0,20);
	glEnd();
	glPopMatrix();
      } else {

      // Draw the glideslope needles only if the flight director
      // isn't activated and the glideslope is alive
      // extend for nav2 and gps?
      if ((*nav1_has_glideslope == 0) && (*nav1_hdef != FLT_MISS) && (*nav1_vdef != FLT_MISS))
	{
	  // Move to the center of the window
	  glTranslatef(47,49,0);
	  glColor3ub( 210, 5,  210 );
	  glLineWidth(3.0);
	  
	  glPushMatrix();
	  glTranslatef(0,*nav1_vdef*-20, 0);
	  glBegin(GL_LINES);
	  glVertex2f(-20,0);
	  glVertex2f(20,0);
	  glEnd();
	  glPopMatrix();
	  
	  glPushMatrix();
	  glTranslatef(*nav1_hdef*20, 0, 0);
	  glBegin(GL_LINES);
	  glVertex2f(0,-20);
	  glVertex2f(0,20);
	  glEnd();
	  glPopMatrix();

	}
    }
    
  }
  
} // end namespace OpenGC
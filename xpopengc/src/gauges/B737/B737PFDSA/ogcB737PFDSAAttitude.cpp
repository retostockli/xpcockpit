/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Copyright (C) 2002-2021 by:
    Original author:
      Michael DeFeyter
    Contributors (in alphabetical order):
	  Damion Shelton
	  Reto Stockli
  
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

#include "ogcCircleEvaluator.h"
#include "B737/B737PFDSA/ogcB737PFDSA.h"
#include "B737/B737PFDSA/ogcB737PFDSAAttitude.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace OpenGC
{

B737PFDSAAttitude::B737PFDSAAttitude()
{
  printf("B737PFDSAAttitude constructed\n");

  m_Font = m_pFontManager->LoadDefaultFont();

  m_PhysicalPosition.x = 0;
  m_PhysicalPosition.y = 0;

  m_PhysicalSize.x = 94;
  m_PhysicalSize.y = 98;

  m_Scale.x = 1.0;
  m_Scale.y = 1.0;
}

B737PFDSAAttitude::~B737PFDSAAttitude()
{
}

//////////////////////////////////////////////////////////////////////

void B737PFDSAAttitude::Render()
{
  // Call base class to setup viewport and projection
  GaugeComponent::Render();

  /*
  
  // For drawing circles
  CircleEvaluator aCircle;

  // First, store the "root" position of the gauge component
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  // Move to the center of the window
  glTranslated(47,49,0);
  // Rotate based on the bank
  glRotated(m_pDataSource->GetAirframe()->GetRoll(), 0, 0, 1);

  // Translate in the direction of the rotation based
  // on the pitch. On the 777, a pitch of 1 degree = 2 mm
  glTranslated(0,m_pDataSource->GetAirframe()->GetPitch()*-2.0,0);

  //-------------------Gauge Background------------------
  // It's drawn oversize to allow for pitch and bank

  // The "ground" rectangle
  // Remember, the coordinate system is now centered in the gauge component
  glColor3ub(COLOR_GROUND);

  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  glBegin(GL_POLYGON);
  glVertex2f(-300,-300);
  glVertex2f(-300,0);
  glVertex2f(300,0);
  glVertex2f(300,-300);
  glVertex2f(-300,-300);
  glEnd();

  // The "sky" rectangle
  // Remember, the coordinate system is now centered in the gauge component
  glColor3ub(COLOR_SKY);
  
  glBegin(GL_POLYGON);
  glVertex2f(-300,0);
  glVertex2f(-300,300);
  glVertex2f(300,300);
  glVertex2f(300,0);
  glVertex2f(-300,0);
  glEnd();

  //------------Draw the pitch markings--------------

  // Draw in white
  glColor3ub(COLOR_WHITE);
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

  //-----The background behind the bank angle markings-------
  // Reset the modelview matrix
  glPopMatrix();
  glPushMatrix();

  // Draw in the sky color
  //glColor3ub(COLOR_SKY);
  glColor3ub(COLOR_BLACK);

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

  // bottom background
  aCircle.SetArcStartEnd(120.0,180.0);
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(94,0);
  glVertex2f(94,26);
  aCircle.Evaluate();
  glVertex2f(47,0);
  glEnd();

  aCircle.SetArcStartEnd(180.0,240.0);
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(0,0);
  glVertex2f(47,0);
  aCircle.Evaluate();
  glVertex2f(0,26);
  glEnd();

  // left background
  glBegin(GL_POLYGON);
  glVertex2f(0,0);
  glVertex2f(0,98);
  glVertex2f(7,98);
  glVertex2f(7,0);
  glVertex2f(0,0);
  glEnd();


  // right background
  glBegin(GL_POLYGON);
  glVertex2f(94,0);
  glVertex2f(94,98);
  glVertex2f(87,98);
  glVertex2f(87,0);
  glVertex2f(94,0);
  glEnd();

  //----------------The bank angle markings----------------

  // Left side bank markings
  // Reset the modelview matrix
  glPopMatrix();
  glPushMatrix();

  // Draw in white
  glColor3ub(COLOR_WHITE);
  glLineWidth(2.0);

  // Move to the center of the window
  glTranslated(47,49,0);

  // Draw the center detent
  glBegin(GL_POLYGON);
  glVertex2f(0.0f,46.0f);
  glVertex2f(-2.3f,49.0f);
  glVertex2f(2.3f,49.0f);
  glVertex2f(0.0f,46.0f);
  glEnd();

  glRotated(10.0,0,0,1);
  glBegin(GL_LINES);
  glVertex2f(0,46);
  glVertex2f(0,49);
  glEnd();
  
  glRotated(10.0,0,0,1);
  glBegin(GL_LINES);
  glVertex2f(0,46);
  glVertex2f(0,49);
  glEnd();

  glRotated(10.0,0,0,1);
  glBegin(GL_LINES);
  glVertex2f(0,46);
  glVertex2f(0,53);
  glEnd();

  glRotated(15.0,0,0,1);
  glBegin(GL_LINES);
  glVertex2f(0,46);
  glVertex2f(0,49);
  glEnd();

  glRotated(15.0,0,0,1);
  glBegin(GL_LINES);
  glVertex2f(0,46);
  glVertex2f(0,51);
  glEnd();

  // Right side bank markings
  // Reset the modelview matrix
  glPopMatrix();
  glPushMatrix();
  // Move to the center of the window
  glTranslated(47,49,0);

  glRotated(-10.0,0,0,1);
  glBegin(GL_LINES);
  glVertex2f(0,46);
  glVertex2f(0,49);
  glEnd();
  
  glRotated(-10.0,0,0,1);
  glBegin(GL_LINES);
  glVertex2f(0,46);
  glVertex2f(0,49);
  glEnd();

  glRotated(-10.0,0,0,1);
  glBegin(GL_LINES);
  glVertex2f(0,46);
  glVertex2f(0,53);
  glEnd();

  glRotated(-15.0,0,0,1);
  glBegin(GL_LINES);
  glVertex2f(0,46);
  glVertex2f(0,49);
  glEnd();

  glRotated(-15.0,0,0,1);
  glBegin(GL_LINES);
  glVertex2f(0,46);
  glVertex2f(0,51);
  glEnd();

  //----------------End Draw Bank Markings----------------


  //----------------Bank Indicator----------------
  // Reset the modelview matrix
  glPopMatrix();
  glPushMatrix();
  // Move to the center of the window
  glTranslated(47,49,0);
  // Rotate based on the bank
  glRotated(m_pDataSource->GetAirframe()->GetRoll(), 0, 0, 1);

  // Draw in white
  glColor3ub(COLOR_WHITE);
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

  //--------------End draw bank indicator------------

  //----------------Attitude Indicator----------------
  // Reset the modelview matrix
  glPopMatrix();
  glPushMatrix();
  // Move to the center of the window
  glTranslated(47,49,0);

  // The center axis indicator
  // Black background
  glColor3ub(COLOR_BLACK);
  glBegin(GL_POLYGON);
  glVertex2f(1.25,1.25);
  glVertex2f(1.25,-1.25);
  glVertex2f(-1.25,-1.25);
  glVertex2f(-1.25,1.25);
  glVertex2f(1.25,1.25);
  glEnd();
  // White lines
  glColor3ub(COLOR_WHITE);
  glLineWidth(2.0);
  glBegin(GL_LINE_LOOP);
  glVertex2f(1.25,1.25);
  glVertex2f(1.25,-1.25);
  glVertex2f(-1.25,-1.25);
  glVertex2f(-1.25,1.25);
  glEnd();

  // The left part
  // Black background
  glColor3ub(COLOR_BLACK);
  glBegin(GL_POLYGON);
  glVertex2f(-30,1.25);
  glVertex2f(-10,1.25);
  glVertex2f(-10,-1.25);
  glVertex2f(-30,-1.25);
  glVertex2f(-30,1.25);
  glEnd();
  glBegin(GL_POLYGON);
  glVertex2f(-10,1.25);
  glVertex2f(-10,-5.75);
  glVertex2f(-13,-5.75);
  glVertex2f(-13,1.25);
  glVertex2f(-10,1.25);
  glEnd();
  
  // White lines
  glColor3ub(COLOR_WHITE);
  glLineWidth(2.0);
  glBegin(GL_LINE_LOOP);
  glVertex2f(-30,1.25);
  glVertex2f(-10,1.25);
  glVertex2f(-10,-5.75);
  glVertex2f(-13,-5.75);
  glVertex2f(-13,-1.25);
  glVertex2f(-30,-1.25);
  glEnd();

  // The right part
  // Black background
  glColor3ub(COLOR_BLACK);
  glBegin(GL_POLYGON);
  glVertex2f(30,1.25);
  glVertex2f(10,1.25);
  glVertex2f(10,-1.25);
  glVertex2f(30,-1.25);
  glVertex2f(30,1.25);
  glEnd();
  glBegin(GL_POLYGON);
  glVertex2f(10,1.25);
  glVertex2f(10,-5.75);
  glVertex2f(13,-5.75);
  glVertex2f(13,1.25);
  glVertex2f(10,1.25);
  glEnd();
  
  // White lines
  glColor3ub(COLOR_WHITE);
  glLineWidth(2.0);
  glBegin(GL_LINE_LOOP);
  glVertex2f(30,1.25);
  glVertex2f(10,1.25);
  glVertex2f(10,-5.75);
  glVertex2f(13,-5.75);
  glVertex2f(13,-1.25);
  glVertex2f(30,-1.25);
  glEnd();
  //--------------End draw attitude indicator------------
  // Reset the modelview matrix
  glPopMatrix();

  //----------------Flight Director----------------
  // Reset the modelview matrix
  //glPopMatrix();
  //glPushMatrix();
  if (m_pDataSource->GetAirframe()->GetDirector_Engaged())
  {
    glPushMatrix();
 
    // Move to the center of the window
    glTranslated(47,49,0);

    glColor3ub(COLOR_MAGENTA);
    glLineWidth(3.0);
    
    glPushMatrix();
    glTranslated(0,(m_pDataSource->GetAirframe()->GetDirector_A_Pitch())*2.0,0);
    glBegin(GL_LINES);
    glVertex2f(-20,0);
    glVertex2f(20,0);
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated((m_pDataSource->GetAirframe()->GetDirector_A_Roll()-m_pDataSource->GetAirframe()->GetRoll()),0,0);
    glTranslated(m_pDataSource->GetAirframe()->GetDirector_A_Roll(),0,0);
    glBegin(GL_LINES);
    glVertex2f(0,-20);
    glVertex2f(0,20);
    glEnd();
    glPopMatrix();

    glPopMatrix();
  }

  // Draw the glideslope needles only if the flight director
  // isn't activated and the glideslope is alive
  if (m_pDataSource->GetAirframe()->GetNav1_Valid())
  {
    glPushMatrix();

    // Move to the center of the window
    glTranslated(47,0,0);

    glColor3ub(COLOR_GREEN;
    glLineWidth(2.4);

    glPushMatrix();
	float val = m_pDataSource->GetAirframe()->GetPressure_Alt_Feet();
	if (val > 400)
		val = 400;
	float tx = m_pDataSource->GetAirframe()->GetNav1_Localizer_Needle() * 36;
	float ty = (400 - val) / 400 * 44.5;
    glTranslated(tx, ty, 0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-14,0);
    glVertex2f(0,0);
    glVertex2f(0,4.5);
    glVertex2f(-12,4.5);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex2f(14,0);
    glVertex2f(0,0);
    glVertex2f(0,4.5);
    glVertex2f(12,4.5);
    glEnd();
    glPopMatrix();
    
    glColor3ub(COLOR_MAGENTA);
    glLineWidth(3.0);
    glPushMatrix();
    glBegin(GL_LINE_STRIP);
    glVertex2f(tx,0);
    glVertex2f(tx,ty);
	glEnd();
    glPopMatrix();

    glPopMatrix();
  }

  */

}

}

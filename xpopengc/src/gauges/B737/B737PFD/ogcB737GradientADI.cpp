/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737GradientADI.cpp,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:07 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2003 Damion Shelton
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <stdio.h>

#include "ogcGaugeComponent.h"
#include "ogcCircleEvaluator.h"
#include "ogcB737GradientADI.h"

namespace OpenGC
{

B737GradientADI::B737GradientADI()
{
  printf("B737GradientADI constructed\n");

  m_Font = m_pFontManager->LoadDefaultFont();

  m_PhysicalPosition.x = 0;
  m_PhysicalPosition.y = 0;

  m_PhysicalSize.x = 94;
  m_PhysicalSize.y = 98;

  m_Scale.x = 1.0;
  m_Scale.y = 1.0;
}

B737GradientADI::~B737GradientADI()
{

}

void B737GradientADI::Render()
{
  // Call base class to setup viewport and projection
  GaugeComponent::Render();

  /*
  
  // For drawing circles
  CircleEvaluator aCircle;

  // First, store the "root" position of the gauge component
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  double Roll = m_pDataSource->GetAirframe()->GetRoll();
  double Pitch = m_pDataSource->GetAirframe()->GetPitch();

  // Move to the center of the window
  glTranslated(47,49,0);

  // Rotate based on the bank
  glRotated(Roll, 0, 0, 1);

  // Translate in the direction of the rotation based
  // on the pitch. On the 777, a pitch of 1 degree = 2 mm
  glTranslated(0, Pitch, 0);

  //-------------------Gauge Background------------------
  // It's drawn oversize to allow for pitch and bank

  // The "ground" rectangle
  // Remember, the coordinate system is now centered in the gauge component

  glBegin(GL_POLYGON);
  
  glColor3ub(179,102,0);
  glVertex2f(-75,-75);
  
  glColor3ub(200,200,200);
  glVertex2f(-75,0);
  
  glColor3ub(200,200,200);
  glVertex2f(75,0);
  
  glColor3ub(179,102,0);
  glVertex2f(75,-75);
  
  glColor3ub(179,102,0);
  glVertex2f(-75,-75);
  glEnd();

  // The "sky" rectangle
  // Remember, the coordinate system is now centered in the gauge component
  
  glBegin(GL_POLYGON);
  
  glColor3ub(200,200,200);
  glVertex2f(-75,0);
  
  glColor3ub(0,153,204);
  glVertex2f(-75,75);
  
  glColor3ub(0,153,204);
  glVertex2f(75,75);
  
  glColor3ub(200,200,200);
  glVertex2f(75,0);
  
  glColor3ub(200,200,200);
  glVertex2f(-75,0);
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
  glColor3ub(0,0,0);
  glVertex2f(-100,0);
  glVertex2f(100,0);
  
  // Draw in white
  glColor3ub(255,255,255);

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

  /*
  //-----The background behind the bank angle markings-------
  // Reset the modelview matrix
  glPopMatrix();
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

  //----------------The bank angle markings----------------

  // Left side bank markings
  // Reset the modelview matrix
  glPopMatrix();
  glPushMatrix();

  // Draw in white
  glColor3ub(255,255,255);

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
  glRotated(Roll, 0, 0, 1);

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

  //--------------End draw bank indicator------------

  //----------------Attitude Indicator----------------
  // Reset the modelview matrix
  glPopMatrix();
  glPushMatrix();

  // Move to the center of the window
  glTranslated(47,49,0);

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
  //--------------End draw attitude indicator------------
  // Reset the modelview matrix
  glPopMatrix();
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

  //----------------Flight Director----------------
  
  /*
  if ((m_pDataSource->GetAirframe()->GetDirector_A_Engaged()==1) || (m_pDataSource->GetAirframe()->GetDirector_B_Engaged()==1))
  {
    // Move to the center of the window
    glTranslated(47,49,0);
    glColor3ub(255,0,255);
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
  }

  // Draw the glideslope needles only if the flight director
  // isn't activated and the glideslope is alive
  if (m_pDataSource->ILS_Glideslope_Alive && m_pDataSource->Director_Active == false)
  {
    // Move to the center of the window
    glTranslated(47,49,0);
    glColor3ub(255,0,255);
    glLineWidth(3.0);

    glPushMatrix();
    glTranslated(0, (m_pDataSource->Nav1_Glideslope_Needle)*-20, 0);
    glBegin(GL_LINES);
    glVertex2f(-20,0);
    glVertex2f(20,0);
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated( (m_pDataSource->Nav1_Course_Indicator)*20, 0, 0);
    glBegin(GL_LINES);
    glVertex2f(0,-20);
    glVertex2f(0,20);
    glEnd();
    glPopMatrix();
    
    // Undo the translation to the window center
    glPopMatrix();
  }

  */

}

} // end namespace OpenGC

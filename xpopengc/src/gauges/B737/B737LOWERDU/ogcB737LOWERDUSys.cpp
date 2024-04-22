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

    int rotate = this->GetArg();
    
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


    // CONTROL SURFACES
    glColor3ub(COLOR_WHITE);
    glLineWidth(m_PhysicalSize.x/70.0);

    // ELEVATOR
    px = 0.0;
    py = 0.0;
    x = 0.5*m_PhysicalSize.x;
    y = 0.4*m_PhysicalSize.y;
    dx = 0.015*m_PhysicalSize.x;
    dy = 0.2*m_PhysicalSize.y;
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
    
    glPopMatrix();
    
  }

}

/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  ogcRREvaluator.cpp

  Copyright (c) 2024 Reto Stockli

  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <stdio.h>
#include <math.h>
#include "ogcGLHeaders.h"
#include "ogcRREvaluator.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC
{

RREvaluator
::RREvaluator()
{
  m_XOrigin = 0.0;
  m_YOrigin = 0.0;

  m_DegreesPerPoint = 10;

  m_Radius = 1.0;

  m_dx = 10;
  m_dy = 15;
  
}

RREvaluator
::~RREvaluator()
{

}

void
RREvaluator
::SetOrigin(double x, double y)
{
  m_XOrigin = x;
  m_YOrigin = y;
}


void
RREvaluator
::SetDegreesPerPoint(double degreesPerPoint)
{
  m_DegreesPerPoint = degreesPerPoint;
}

void
RREvaluator
::SetRadius(double radius)
{
  m_Radius = radius;
}

void
RREvaluator
::SetSize(double x, double y)
{
  m_dx = x/2.0;
  m_dy = y/2.0;
}


void
RREvaluator
::Evaluate()
{
    CircleEvaluator aCircle;
    aCircle.SetDegreesPerPoint(m_DegreesPerPoint);
    aCircle.SetRadius(m_Radius);

    glPushMatrix();

    glTranslatef(m_XOrigin, m_YOrigin, 0.0);
    
    glBegin(GL_LINES);
    glVertex2f(-m_dx+m_Radius,m_dy);
    glVertex2f(+m_dx-m_Radius,m_dy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(+m_dx,m_dy-m_Radius);
    glVertex2f(+m_dx,-m_dy+m_Radius);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(-m_dx+m_Radius,-m_dy);
    glVertex2f(+m_dx-m_Radius,-m_dy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(-m_dx,m_dy-m_Radius);
    glVertex2f(-m_dx,-m_dy+m_Radius);
    glEnd();
    aCircle.SetArcStartEnd(-90,0);
    aCircle.SetOrigin(-m_dx+m_Radius, m_dy-m_Radius);
    glBegin(GL_LINE_STRIP);
    aCircle.Evaluate();
    glEnd();
    aCircle.SetArcStartEnd(0,90);
    aCircle.SetOrigin(+m_dx-m_Radius, m_dy-m_Radius);
    glBegin(GL_LINE_STRIP);
    aCircle.Evaluate();
    glEnd();
    aCircle.SetArcStartEnd(90,180);
    aCircle.SetOrigin(+m_dx-m_Radius, -m_dy+m_Radius);
    glBegin(GL_LINE_STRIP);
    aCircle.Evaluate();
    glEnd();
    aCircle.SetArcStartEnd(180,270);
    aCircle.SetOrigin(-m_dx+m_Radius, -m_dy+m_Radius);
    glBegin(GL_LINE_STRIP);
    aCircle.Evaluate();
    glEnd();
    
    glPopMatrix();
      
}

} // end namespace OpenGC

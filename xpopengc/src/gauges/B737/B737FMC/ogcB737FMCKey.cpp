/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737FMCKey.cpp,v $

  Copyright (C) 2018 by:
  Original author:
  Reto Stockli
  Contributors (in alphabetical order):

  Last modification:
  Date:      $Date: 2018/04/12 $
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
  The Boeing 737 Flight Management Computer Display Component
*/

#include <stdio.h>
#include <math.h>

#include "ogcCircleEvaluator.h"
#include "B737/B737FMC/ogcB737FMC.h"
#include "B737/B737FMC/ogcB737FMCKey.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace OpenGC
{
  
  B737FMCKey::B737FMCKey()
  {
    printf("B737FMCKey constructed\n");

    //  m_Font = m_pFontManager->LoadDefaultFont();
    m_Font = m_pFontManager->LoadFont((char*) "CockpitMCDU.ttf");
    
    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;
    
    m_PhysicalSize.x = (96.0 - 12.0)/8.0;
    m_PhysicalSize.y = (151.0 - 71.0)/8.0;
    
    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
    
    strcpy(m_KeyName,"\0");
    strcpy(m_KeyDataRef1,"\0");
    strcpy(m_KeyDataRef2,"\0");
    m_KeyForm = 0;

  }

  B737FMCKey::~B737FMCKey()
  {
    
  }

  void B737FMCKey::Render()
  {
    GaugeComponent::Render();

    char buffer[10];
    float fontSize;
    float xpos;
    float ypos;

    int acf_type = m_pDataSource->GetAcfType();

    float *fkey = NULL;
    int *ikey = NULL;
    if (acf_type == 1) {
      if (strcmp(m_KeyDataRef1,"") != 0) {
	fkey = link_dataref_flt(m_KeyDataRef1,0);
	*fkey = 0.0;
      }
    } else if ((acf_type == 2) || (acf_type == 3)) {
      if (strcmp(m_KeyDataRef2,"") != 0) {
	ikey = link_dataref_cmd_once(m_KeyDataRef2);
	*ikey = 0;
      }
    }

    if (fkey || ikey) {
    
      CircleEvaluator tCircle;
      tCircle.SetDegreesPerPoint(20);	    
      tCircle.SetArcStartEnd(0,360);
      tCircle.SetRadius(m_PhysicalSize.y/2*0.8);
    
      tCircle.SetOrigin(m_PhysicalSize.x/2,m_PhysicalSize.y/2);


      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      switch(m_KeyForm) {
      case 0:
	glColor3ub( COLOR_GRAY25 );
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glBegin(GL_POLYGON);
	glVertex2f( 0.1*m_PhysicalSize.x, 0.1*m_PhysicalSize.y);
	glVertex2f( 0.9*m_PhysicalSize.x, 0.1*m_PhysicalSize.y);
	glVertex2f( 0.9*m_PhysicalSize.x, 0.9*m_PhysicalSize.y);
	glVertex2f( 0.1*m_PhysicalSize.x, 0.9*m_PhysicalSize.y);
	glEnd();
	break;
      case 1:
	glColor3ub( COLOR_GRAY25 );
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glBegin(GL_POLYGON);
	glVertex2f( 0.1*m_PhysicalSize.x, 0.1*m_PhysicalSize.y);
	glVertex2f( 0.9*m_PhysicalSize.x, 0.1*m_PhysicalSize.y);
	glVertex2f( 0.9*m_PhysicalSize.x, 0.9*m_PhysicalSize.y);
	glVertex2f( 0.1*m_PhysicalSize.x, 0.9*m_PhysicalSize.y);
	glEnd();
	glColor3ub(COLOR_WHITE );
	glBegin(GL_LINE_LOOP);
	glVertex2f( 0.1*m_PhysicalSize.x, 0.1*m_PhysicalSize.y);
	glVertex2f( 0.9*m_PhysicalSize.x, 0.1*m_PhysicalSize.y);
	glVertex2f( 0.9*m_PhysicalSize.x, 0.9*m_PhysicalSize.y);
	glVertex2f( 0.1*m_PhysicalSize.x, 0.9*m_PhysicalSize.y);
	glEnd();
	break;
      case 2:
	glColor3ub( COLOR_GRAY25 );
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glBegin(GL_POLYGON);
	glVertex2f( 0.1*m_PhysicalSize.x, 0.1*m_PhysicalSize.y);
	glVertex2f( 0.9*m_PhysicalSize.x, 0.1*m_PhysicalSize.y);
	glVertex2f( 0.9*m_PhysicalSize.x, 0.9*m_PhysicalSize.y);
	glVertex2f( 0.1*m_PhysicalSize.x, 0.9*m_PhysicalSize.y);
	glEnd();
	break;
      case 3:
	glColor3ub(COLOR_GRAY25);
	glBegin(GL_POLYGON);
	tCircle.Evaluate();
	glEnd();
	break;
      default:
	glColor3ub( COLOR_GRAY25 );
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glBegin(GL_POLYGON);
	glVertex2f( 0.1*m_PhysicalSize.x, 0.1*m_PhysicalSize.y);
	glVertex2f( 0.9*m_PhysicalSize.x, 0.1*m_PhysicalSize.y);
	glVertex2f( 0.9*m_PhysicalSize.x, 0.9*m_PhysicalSize.y);
	glVertex2f( 0.1*m_PhysicalSize.x, 0.9*m_PhysicalSize.y);
	glEnd();      
	glColor3ub(COLOR_WHITE);
	glBegin(GL_LINE_LOOP);
	glVertex2f( 0.9*m_PhysicalSize.x, 0.5*m_PhysicalSize.y);
	glVertex2f( 1.0*m_PhysicalSize.x, 0.5*m_PhysicalSize.y);
	glEnd();      
	glBegin(GL_LINE_LOOP);
	glVertex2f( 0.0*m_PhysicalSize.x, 0.5*m_PhysicalSize.y);
	glVertex2f( 0.1*m_PhysicalSize.x, 0.5*m_PhysicalSize.y);
	glEnd();      
      }
      sprintf( buffer, "%s", m_KeyName );

      glColor3ub(COLOR_WHITE);

      /* wide keys: search for space, means line break */
      if ((m_PhysicalSize.x > 1.1*m_PhysicalSize.y) && (m_KeyForm != 4)) {
	fontSize = 0.2*m_PhysicalSize.y;
	m_pFontManager->SetSize( m_Font, fontSize, fontSize );

	char *token = strtok(buffer," ");
        
	if (token != NULL) {
	  int nc = strlen(token);
	  xpos = (0.5-0.06*nc)*m_PhysicalSize.x;
	  if (strchr(m_KeyName,' ') != NULL) {
	    ypos = 0.6*m_PhysicalSize.y;
	  } else {
	    ypos = 0.4*m_PhysicalSize.y;
	  }
	  m_pFontManager->Print( xpos, ypos, token, m_Font);
	  char *token = strtok(NULL," ");
	  if (token != NULL) {
	    int nc = strlen(token);
	    xpos = (0.5-0.06*nc)*m_PhysicalSize.x;
	    ypos = 0.2*m_PhysicalSize.y;
	    m_pFontManager->Print( xpos, ypos, token, m_Font);
	  }
	}
      } else {
	int nc = strlen(buffer);
	if (nc <= 2) {
	  fontSize = 0.4*m_PhysicalSize.y;
	  xpos = 0.35*m_PhysicalSize.x;
	  ypos = 0.3*m_PhysicalSize.y;
	} else {
	  fontSize = 0.3*m_PhysicalSize.y;
	  xpos = (0.5-0.12*nc)*m_PhysicalSize.x;
	  ypos = 0.35*m_PhysicalSize.y;
	}
	m_pFontManager->SetSize( m_Font, fontSize, fontSize );
	m_pFontManager->Print( xpos, ypos, buffer, m_Font);
      }

      glPopMatrix();

    }
    
  }

  void B737FMCKey::OnMouseDown(int button, double physicalX, double physicalY)
  {

    //  printf("BLA %f %f \n",physicalX,physicalY);
    int acf_type = m_pDataSource->GetAcfType();
  
    if (acf_type == 1) {
      float *fkey = link_dataref_flt(m_KeyDataRef1,0);  
      *fkey = 1.0;
    } else {
      int *ikey = link_dataref_cmd_once(m_KeyDataRef2);
      *ikey = 1;
    }

  }
 
}

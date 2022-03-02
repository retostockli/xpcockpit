/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737MIPKey.cpp,v $

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
 The Boeing 737 MIP Annunciator Gauge
*/

#include <stdio.h>
#include <math.h>

#include "ogcCircleEvaluator.h"
#include "B737/B737MIP/ogcB737MIP.h"
#include "B737/B737MIP/ogcB737MIPAnn.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace OpenGC
{
  
  B737MIPAnn::B737MIPAnn()
  {
    printf("B737MIPAnn constructed\n");

    //  m_Font = m_pFontManager->LoadDefaultFont();
    m_Font = m_pFontManager->LoadFont((char*) "CockpitMCDU.ttf");
    
    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;
    
    m_PhysicalSize.x = 20;
    m_PhysicalSize.y = 10;
    
    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
   
    strcpy(m_AnnName,"\0");
    strcpy(m_AnnDataRef1,"\0");
    strcpy(m_AnnKeyDataRef1,"\0");
    strcpy(m_AnnDataRef2,"\0");
    strcpy(m_AnnKeyDataRef2,"\0");

    m_AnnForm = 0;
    m_AnnBGColor[0] = 50;
    m_AnnBGColor[1] = 50;
    m_AnnBGColor[2] = 50;
    m_AnnFGColor[0] = 255;
    m_AnnFGColor[1] = 255;
    m_AnnFGColor[2] = 255;
    m_AnnDataRefType1 = 1;  // 1: int, 2: flt
    m_AnnKeyDataRefType1 = 1;
    m_AnnDataRefType2 = 1;  // 1: int, 2: flt
    m_AnnKeyDataRefType2 = 1;

  }

  B737MIPAnn::~B737MIPAnn()
  {
    
  }

  void B737MIPAnn::Render()
  {
    GaugeComponent::Render();

    char buffer[30];
    float fontSize;
    float xpos;
    float ypos;
    int nl;

    int acf_type = m_pDataSource->GetAcfType();

    char AnnDataRef[100];
    char AnnKeyDataRef[100];
    int AnnDataRefType;
    int AnnKeyDataRefType;
    if (acf_type == 1) {
      strcpy(AnnDataRef,m_AnnDataRef1);
      strcpy(AnnKeyDataRef,m_AnnKeyDataRef1);
      AnnDataRefType = m_AnnDataRefType1;
      AnnKeyDataRefType = m_AnnKeyDataRefType1;
    } else {
      strcpy(AnnDataRef,m_AnnDataRef2);
      strcpy(AnnKeyDataRef,m_AnnKeyDataRef2);
      AnnDataRefType = m_AnnDataRefType2;
      AnnKeyDataRefType = m_AnnKeyDataRefType2;
    }
    
    int *ann_i;
    float *ann_f;
    if (strcmp(AnnDataRef,"") != 0) {
      if (AnnDataRefType == 1) {
	ann_i = link_dataref_int(AnnDataRef);
      } else {
	ann_f = link_dataref_flt(AnnDataRef,-1);
      }
    }
    
    int *key_i;
    float *key_f;
    if (strcmp(AnnKeyDataRef,"") != 0) {
      if (AnnKeyDataRefType == 1) {
	key_i = link_dataref_int(AnnKeyDataRef);
	*key_i = 0;
      } else if (AnnKeyDataRefType == 2) {
	key_f = link_dataref_flt(AnnKeyDataRef,-1);
	*key_f = 0.0;
      } else if  (AnnKeyDataRefType == 3) {
	key_i = link_dataref_cmd_once(AnnKeyDataRef);
	*key_i = 0;
      } else {
	key_i = link_dataref_cmd_hold(AnnKeyDataRef);
	*key_i = 0;
      }
    }
     
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    switch(m_AnnForm) {
    case 0:
      glColor3ub( m_AnnBGColor[0], m_AnnBGColor[1], m_AnnBGColor[2] );
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glBegin(GL_POLYGON);
      glVertex2f( 0.05*m_PhysicalSize.x, 0.05*m_PhysicalSize.y);
      glVertex2f( 0.95*m_PhysicalSize.x, 0.05*m_PhysicalSize.y);
      glVertex2f( 0.95*m_PhysicalSize.x, 0.95*m_PhysicalSize.y);
      glVertex2f( 0.05*m_PhysicalSize.x, 0.95*m_PhysicalSize.y);
      glEnd();
      glColor3ub(COLOR_GRAY40);
      glBegin(GL_LINE_LOOP);
      glVertex2f( 0.05*m_PhysicalSize.x, 0.05*m_PhysicalSize.y);
      glVertex2f( 0.95*m_PhysicalSize.x, 0.05*m_PhysicalSize.y);
      glVertex2f( 0.95*m_PhysicalSize.x, 0.95*m_PhysicalSize.y);
      glVertex2f( 0.05*m_PhysicalSize.x, 0.95*m_PhysicalSize.y);
      glEnd();
      break;
    default:
      printf("DEFAULT ANNUNCIATOR NOT DEFINED YET");
    }

    //    printf("%f \n",*ann_f);

    glColor3ub(COLOR_BLACK);
    if (strcmp(AnnDataRef,"") != 0) {
      if (AnnDataRefType == 1) {
	if (*ann_i == 1) {
	  glColor3ub(m_AnnFGColor[0], m_AnnFGColor[1], m_AnnFGColor[2]);
	}
      } else {
	if (*ann_f > 0.5) {
	  glColor3ub(m_AnnFGColor[0], m_AnnFGColor[1], m_AnnFGColor[2]);
	}
      }
    }
    
    snprintf( buffer, sizeof(buffer), "%s", m_AnnName );

    int nctot = strlen(buffer);
    if (nctot > 10) {
      fontSize = 0.20*m_PhysicalSize.y;
    } else {
      fontSize = 0.26*m_PhysicalSize.y;
    }
    
    // count number of lines
    nl = 0;
    char *token = strtok(buffer," ");
    while (token != NULL) {
      nl++;
      token = strtok(NULL," ");
    }

    if (nl == 1) ypos = 0.35*m_PhysicalSize.y;
    if (nl == 2) ypos = 0.55*m_PhysicalSize.y;
    if (nl == 3) ypos = 0.70*m_PhysicalSize.y;

    m_pFontManager->SetSize( m_Font, fontSize, fontSize );
    
    snprintf( buffer, sizeof(buffer), "%s", m_AnnName );
    token = strtok(buffer," ");    
    while (token != NULL) {
      int nc = strlen(token);
      xpos = 0.5*m_PhysicalSize.x - fontSize*0.425*float(nc);
      m_pFontManager->Print( xpos, ypos, token, m_Font);
      token = strtok(NULL," ");
      if (nl == 3) {
	ypos -= 0.3*m_PhysicalSize.y;
      } else {
	ypos -= 0.35*m_PhysicalSize.y;
      }
    }

    glPopMatrix();
    
  }

void B737MIPAnn::OnMouseDown(int button, double physicalX, double physicalY)
{

  //  printf("BLA %f %f \n",physicalX,physicalY);

  int acf_type = m_pDataSource->GetAcfType();

  char AnnKeyDataRef[100];
  int AnnKeyDataRefType;
  if (acf_type == 1) {
    strcpy(AnnKeyDataRef,m_AnnKeyDataRef1);
    AnnKeyDataRefType = m_AnnKeyDataRefType1;
  } else {
    strcpy(AnnKeyDataRef,m_AnnKeyDataRef2);
    AnnKeyDataRefType = m_AnnKeyDataRefType2;
  }
    
  int *key_i;
  float *key_f;
  if (strcmp(AnnKeyDataRef,"") != 0) {
    if (AnnKeyDataRefType == 1) {
      key_i = link_dataref_int(AnnKeyDataRef);
      *key_i = 1;
    } else if (AnnKeyDataRefType == 2) {
      key_f = link_dataref_flt(AnnKeyDataRef,-1);
      *key_f = 1.0;
    } else if (AnnKeyDataRefType == 3) {
      key_i = link_dataref_cmd_once(AnnKeyDataRef);
      *key_i = 1;
    } else {
      key_i = link_dataref_cmd_hold(AnnKeyDataRef);
      *key_i = 1;      
    }
  }
  
}
 
}

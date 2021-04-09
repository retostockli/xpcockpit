/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737FMCLine.cpp,v $

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
  The Boeing 737 Flight Management Computer Display Component
*/

#include <stdio.h>
#include <math.h>

#include "B737/B737FMC/ogcB737FMCLine.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace OpenGC
{
  
  B737FMCLine::B737FMCLine()
  {
    printf("B737FMCLine constructed\n");

    //  m_Font = m_pFontManager->LoadDefaultFont();
    m_Font = m_pFontManager->LoadFont((char*) "CockpitMCDU.ttf");
    
    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;
    
    m_PhysicalSize.x = 117;
    m_PhysicalSize.y = (67.0-5.0)/14.0;
    
    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
    
    m_Line = 1;
    strcpy(m_LineDataRef1,"\0");
    strcpy(m_LineDataRef2,"\0");
    m_LineSizeX = 0.9;
    m_LineSizeY = 0.85;
    m_LineColor[0]= 255;
    m_LineColor[1]= 255;
    m_LineColor[2]= 255;
  }

  B737FMCLine::~B737FMCLine()
  {
    
  }

  void B737FMCLine::Render()
  {
    GaugeComponent::Render();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
 
    int nb = 40; // buffer length
    //    int nc = 24; // characters per line
    //    int nl = 14; // number of lines

    float fontSize = m_PhysicalSize.y;

    int acf_type = m_pDataSource->GetAcfType();
   
    unsigned char *line = NULL; // pointer to line dataref

    if (acf_type == 1) {
      /* we read the Javier Cortez FMC for x737 */
      if (strcmp(m_LineDataRef1,"") != 0) {
	line = link_dataref_byte_arr(m_LineDataRef1,nb,-1);
      }
    } else if ((acf_type == 2) || (acf_type == 3)) {
      /* we read the ZIBO 737 FMC */
      if (strcmp(m_LineDataRef2,"") != 0) {
	line = link_dataref_byte_arr(m_LineDataRef2,nb,-1);
      }
    }
    
    float xFontSize = m_LineSizeX * fontSize;
    float yFontSize = m_LineSizeY * fontSize;
    
    glColor3ub( m_LineColor[0], m_LineColor[1], m_LineColor[2] );
   
    if (line) {
      
      char buffer[nb];
      memset(buffer,0,sizeof(buffer));
      snprintf( buffer, nb, "%s", line );
      
      /*
      if (m_Line == 1) {
	printf("Line 1: %s \n",buffer);
      } 
      */
      
      //      if ((m_Line != 1) && (m_Line != nl)) {
      if (1 == 1) {

	/* fix degree sign */
	for (int i=0; i<nb; i++) {
	  if (((int) buffer[i]) == 61) buffer[i] = (char) 176; // X737
	  if (((int) buffer[i]) == 96) buffer[i] = (char) 176; // ZIBO
	}

	/* fix [ ] entries to single character X737 */
	for (int i=0; i<(nb-1); i++) {	    
	  if ((((int) buffer[i]) == 91) && (((int) buffer[i+1]) == 93)) {
	    buffer[i] = (char) 124;
	    for (int j=i+1; j<(nb-1); j++)
	      {
		buffer[j] = buffer[j+1];
	      }
	    buffer[nb-1] = (char) 0;
	  }
	}
	/* fix [ ] entries in ZIBO */
	for (int i=0; i<nb; i++) {
	  if (((int) buffer[i]) == 42) buffer[i] = (char) 124; // ZIBO
	}
	
	/* fix right adjustment */
	/*
	  int i = 0;
	  while ((buffer[i] != (char) 0) && (i < nc)) i++;
	  if ((i < nc) && (i > 12)) {
	  i--;;
	  //	  printf("%i %i %s \n",m_Line,i,buffer);
	  for (int j=i;j>=12;j--) {
	  //	    printf("%i %i %i \n",i,j,nc-1+j-i);
	  buffer[nc-1+j-i] = buffer[j];
	  }
	  
	  for (int j=12;j<(12+nc-1-i);j++) {
	  buffer[j] = (char) 32;
	  }
	  	
	  buffer[nc] = (char) 0;
	  //	  printf("%i %i %s\n",m_Line,i,buffer);
	  }
	*/
	
      }
      //      printf("%s \n",buffer);
  
      m_pFontManager->SetSize( m_Font, xFontSize, yFontSize );
      m_pFontManager->Print( 0, 0, buffer, m_Font);

    } /* line dataref valid */

    glPopMatrix();
    
  }

}

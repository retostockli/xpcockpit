/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Copyright (C) 2022  by:
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

#include <stdio.h>

#include "ogcB737RMI.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC
{

  B737RMI::B737RMI()
  {
    printf("B737RMI constructed\n");
 
    m_PhysicalSize.x = 100;
    m_PhysicalSize.y = 100;
  
    // We need a font to draw the ILS display
    m_Font = m_pFontManager->LoadDefaultFont();
  
    // We want to draw an outline
    this->SetGaugeOutline(true);

  }

  B737RMI::~B737RMI()
  {
    // Destruction handled by base class
  }

  void B737RMI::Render()
  {
  
    Gauge::Render();

    float tickWidth = 1.5;
    float fontHeight = 6;
    float fontWidth = 4.5;
    float lineWidth = 3.0;
	
    float R = m_PhysicalSize.x / 2.0 * 0.9;
    float center_x = m_PhysicalSize.x / 2.0;
    float center_y = m_PhysicalSize.y / 2.0;
    
    float *heading_mag = link_dataref_flt("sim/flightmodel/position/magpsi",-1);
	
    int *efis1_selector = link_dataref_int("sim/cockpit2/EFIS/EFIS_1_selection_pilot");
    float *adf1_bearing = link_dataref_flt("sim/cockpit2/radios/indicators/adf1_bearing_deg_mag",0);
    float *nav1_bearing = link_dataref_flt("sim/cockpit2/radios/indicators/nav1_bearing_deg_mag",0);
    unsigned char *nav1_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/nav1_nav_id",150,-1);
    unsigned char *adf1_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/adf1_nav_id",150,-1);
	
    int *efis2_selector = link_dataref_int("sim/cockpit2/EFIS/EFIS_2_selection_pilot");
    float *adf2_bearing = link_dataref_flt("sim/cockpit2/radios/indicators/adf2_bearing_deg_mag",0);
    float *nav2_bearing = link_dataref_flt("sim/cockpit2/radios/indicators/nav2_bearing_deg_mag",0);
    unsigned char *nav2_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/nav2_nav_id",150,-1);
    unsigned char *adf2_name = link_dataref_byte_arr("sim/cockpit2/radios/indicators/adf2_nav_id",150,-1);
	
    int i;
    char buf[10];


    bool has_heading;
    bool has_nav1;
    bool has_nav2;
    
    float heading;
    float nav1;
    float nav2;
    if (*heading_mag != FLT_MISS) {
      heading = *heading_mag;
      has_heading = true;
    } else {
      heading = 0.0;
      has_heading = false;
    }
    
    if ((((*efis1_selector == 0) && (*adf1_bearing != FLT_MISS) &&
	  (strcmp((const char*) adf1_name,"") != 0)) || 
	 ((*efis1_selector == 2) && (*nav1_bearing != FLT_MISS) &&
	  (strcmp((const char*) nav1_name,"") != 0))) && (*heading_mag != FLT_MISS)) {

      if (*efis1_selector == 0) {
	nav1 = *adf1_bearing;
      } else {
        nav1 = *nav1_bearing;
      }
      has_nav1 = true;
    } else {
      nav1 = 90.0 + heading;
      has_nav1 = false;
    }

    if ((((*efis2_selector == 0) && (*adf2_bearing != FLT_MISS) &&
	  (strcmp((const char*) adf2_name,"") != 0)) || 
	 ((*efis2_selector == 2) && (*nav2_bearing != FLT_MISS) &&
	  (strcmp((const char*) nav2_name,"") != 0))) && (*heading_mag != FLT_MISS)) {

      if (*efis2_selector == 0) {
	nav2 = *adf2_bearing;
      } else {
	nav2 = *nav2_bearing;
      }
      has_nav2 = true;
    } else {
      nav2 = 90.0 + heading;
      has_nav2 = false;
    }
      
    // Draw the background rectangle
    // Draw in dark gray
    glColor3ub(COLOR_GRAY15);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(0.0, 0.0);
    glVertex2f(0.0, m_PhysicalSize.y);
    glVertex2f(m_PhysicalSize.x, m_PhysicalSize.y);
    glVertex2f(m_PhysicalSize.x, 0.0);
    glVertex2f(0.0, 0.0);
    glEnd();

    /* Outer Circle */
    CircleEvaluator aCircle;
    aCircle.SetRadius(R);
    aCircle.SetArcStartEnd(0,360);
    aCircle.SetDegreesPerPoint(10);
        
    glColor3ub(COLOR_BLACK);
    aCircle.SetOrigin(center_x, center_y);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);
    aCircle.Evaluate();
    glEnd();
 
    // First, store the "root" position of the gauge component
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(center_x, center_y, 0); // move to center
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

    glPushMatrix();
    glRotated(*heading_mag, 0, 0, 1); // rotate with heading

    // draw marker lines & text
    m_pFontManager->SetSize(m_Font, fontWidth, fontHeight);

    float linelength;
    float linestart;
    float linewidth;
    for (i = 0; i > -360; i -= 5) {
      if (i % 10 == 0) {
	linelength = 6;
	linestart = 0;
	linewidth = 3;
      } else {
	linelength = 3;
	linestart = 3;
	linewidth = 2;
      }

      glPushMatrix();

      glRotated(i, 0, 0, 1);
      glLineWidth(linewidth);
      glColor3ub(COLOR_WHITE);
      glBegin(GL_LINE_STRIP);
      glVertex2f(0, R-linestart);
      glVertex2f(0, R-linestart-linelength);
      glEnd();

      if (i % 30 == 0) {
	snprintf(buf, sizeof(buf), "%d", -i/10);
	if (i < -90)
	  m_pFontManager->Print(-fontWidth*0.75, R-13, buf, m_Font);
	else
	  m_pFontManager->Print(-fontWidth*0.75/2, R-13, buf, m_Font);
      }

      glPopMatrix();
    }

    // draw VOR/ADF arrows

    /* NAV1/ADV1 */
    glPushMatrix();    
    glRotatef(-nav1, 0, 0, 1);
    glColor3ub(COLOR_WHITE);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(0, R-6);
    glVertex2f(-4, R-18);
    glVertex2f(4, R-18);
    glEnd();

    glLineWidth(lineWidth*2.0);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple( 5, 0x3F3F );
    glBegin(GL_LINES);
    glVertex2f(0.0,R-15);
    glVertex2f(0.0,-R+6);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    glPopMatrix();

    /* NAV2/ADV2 */
    glPushMatrix();
    glRotatef(-nav2, 0, 0, 1);  
    glLineWidth(lineWidth*2.0);
    glColor3ub(COLOR_WHITE);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0, R-6);
    glVertex2f(-6.0, R-19);
    glVertex2f(-3.0, R-19);
    glVertex2f(-3.0, -R+13);
    glVertex2f(-1.5, -R+13);
    glVertex2f(-1.5, -R+5);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0, R-6);
    glVertex2f(6.0, R-19);
    glVertex2f(3.0, R-19);
    glVertex2f(3.0, -R+13);
    glVertex2f(1.5, -R+13);
    glVertex2f(1.5, -R+5);
    glEnd();

    glPopMatrix();

    /* end of heading rotated coordinate system */
    glPopMatrix();

    /* end of center-translated coordinate system */
    glPopMatrix();
  
    /* Small Circle in the middle holding needles */

    CircleEvaluator bCircle;
    bCircle.SetRadius(R/6);
    bCircle.SetArcStartEnd(0,360);
    bCircle.SetDegreesPerPoint(20);

    glPushMatrix();
    glColor3ub(COLOR_GRAY15);
    bCircle.SetOrigin(center_x, center_y);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);
    bCircle.Evaluate();
    glEnd();
    glPopMatrix();

    /* missing nav 1 warning */
    if (!has_nav1) {
      glLineWidth(lineWidth*1.5);
      glPushMatrix();
      glTranslatef(0, center_y-10, 0); 
      glRotated(-15.0, 0, 0, 1);
      glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
      glColor3ub(COLOR_ORANGERED);
      glBegin(GL_POLYGON);
      glVertex2f(6, 0);
      glVertex2f(0, 15);
      glVertex2f(0, 35);
      glVertex2f(6, 35);
      glEnd();
      glColor3ub(COLOR_BLACK);
      glBegin(GL_POLYGON);
      glVertex2f(0, 35);
      glVertex2f(6, 32);
      glVertex2f(0, 29);
      glEnd();
      glBegin(GL_POLYGON);
      glVertex2f(0, 29);
      glVertex2f(6, 26);
      glVertex2f(0, 23);
      glEnd();
      glBegin(GL_POLYGON);
      glVertex2f(3, 23);
      glVertex2f(1, 18);
      glVertex2f(5, 18);
      glEnd();
      glBegin(GL_LINES);
      glVertex2f(3, 18);
      glVertex2f(3, 10);
      glEnd();
      glPopMatrix();
    }
      
    /* missing nav 2 warning */
    if (!has_nav2) {
      glLineWidth(lineWidth*1.5);
      glPushMatrix();
      glTranslatef(m_PhysicalSize.x-6, center_y-10, 0); 
      glRotated(15.0, 0, 0, 1);
      glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
      glColor3ub(COLOR_ORANGERED);
      glBegin(GL_POLYGON);
      glVertex2f(0, 0);
      glVertex2f(6, 15);
      glVertex2f(6, 35);
      glVertex2f(0, 35);
      glEnd();
      glColor3ub(COLOR_BLACK);
      glBegin(GL_POLYGON);
      glVertex2f(6, 35);
      glVertex2f(0, 32);
      glVertex2f(6, 29);
      glEnd();
      glBegin(GL_POLYGON);
      glVertex2f(6, 29);
      glVertex2f(0, 26);
      glVertex2f(6, 23);
      glEnd();
      glBegin(GL_LINE_LOOP);
      glVertex2f(3, 23);
      glVertex2f(1, 18);
      glVertex2f(5, 18);
      glEnd();
      glBegin(GL_LINES);
      glVertex2f(3.8, 18);
      glVertex2f(3.8, 10);
      glVertex2f(2.2, 18);
      glVertex2f(2.2, 10);
      glEnd();
      glPopMatrix();
    }
      
    /* missing heading */
    if (!has_heading) {
      glLineWidth(lineWidth*1.5);
      glPushMatrix();
      glTranslatef(center_x+13, m_PhysicalSize.y-6.5, 0); 
      glRotated(105.0, 0, 0, 1);
      glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
      glColor3ub(COLOR_ORANGERED);
      glBegin(GL_POLYGON);
      glVertex2f(0, 0);
      glVertex2f(6, 15);
      glVertex2f(6, 35);
      glVertex2f(0, 35);
      glEnd();
      glColor3ub(COLOR_BLACK);
      snprintf(buf, sizeof(buf), "HDG");
      glRotated(-90.0, 0, 0, 1);
      m_pFontManager->SetSize(m_Font, fontWidth*0.7, fontHeight*0.7);
      m_pFontManager->Print(-30,1, buf, m_Font);
      glPopMatrix();
    }

    glPushMatrix();

    glTranslatef(center_x, center_y, 0); // move to center
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

    // outer circle markers on -90, -45, 0, 45, 90
    for (i = 0; i < 360; i += 45) {
      glPushMatrix();
      glRotated(i, 0, 0, 1);
      glLineWidth(2.0);
      glColor3ub(COLOR_WHITE);
      glBegin(GL_POLYGON);
      glVertex2f(0, R-2);
      glVertex2f(2, R+3);
      glVertex2f(-2, R+3);
      glEnd();
      glPopMatrix();
    }

    glPopMatrix();
         
  }

} // end namespace OpenGC


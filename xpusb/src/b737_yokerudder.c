/* This is the B737 Yoke and Rudder Analog and Button Inputs Code

   Copyright (C) 2018  Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "libiocards.h"
#include "serverdata.h"
#include "b737_yokerudder.h"

#define max(A,B) ((A)>(B) ? (A) : (B)) 
#define min(A,B) ((A)<(B) ? (A) : (B))



void b737_yokerudder(void)
{

  int ret = 0;
  int device = 4;
  int card = 0;
  float minval = 0.0;
  float maxval = 100.0;

  float minbrake = 1.0;
  float maxbrake = 80.0;

  float brakescale;

  int button;
  float value1,value2;
  int ret1,ret2;

  float* left_brake;
  float* right_brake;
  if (acf_type <= 1) {
    brakescale = 1.0;
    left_brake = link_dataref_flt("sim/cockpit2/controls/left_brake_ratio",-3);
    right_brake = link_dataref_flt("sim/cockpit2/controls/right_brake_ratio",-3);
  } else {
    brakescale = 0.5;
    left_brake = link_dataref_flt("laminar/B738/axis/left_toe_brake",-3);
    right_brake = link_dataref_flt("laminar/B738/axis/right_toe_brake",-3);
  }
  
  int* viewmode = link_dataref_int("xpserver/viewmode");

  int* forward_with_nothing = link_dataref_cmd_once("sim/view/forward_with_nothing");
  int* forward_with_panel = link_dataref_cmd_once("sim/view/forward_with_panel");
  int* circle = link_dataref_cmd_once("sim/view/circle");
  int* free_camera = link_dataref_cmd_once("sim/view/free_camera");

  int* view_left = link_dataref_cmd_hold("sim/general/left");
  int* view_right = link_dataref_cmd_hold("sim/general/right");
  int* view_up = link_dataref_cmd_hold("sim/general/up");
  int* view_down = link_dataref_cmd_hold("sim/general/down");
  
  float *view_horizontal = link_dataref_flt("sim/graphics/view/field_of_view_horizontal_deg",0);
  float *view_vertical = link_dataref_flt("sim/graphics/view/field_of_view_vertical_deg",0);

  int *ap_engage = link_dataref_int("sim/cockpit/autopilot/autopilot_mode");    // AP engage/disengage mode

  int *stab_trim_up;
  int *stab_trim_down;
  int *ap_disconnect;
  if ((acf_type == 2) || (acf_type == 3)) {
    stab_trim_up = link_dataref_cmd_hold("sim/flight_controls/pitch_trim_up");
    stab_trim_down = link_dataref_cmd_hold("sim/flight_controls/pitch_trim_down");
    ap_disconnect = link_dataref_cmd_once("laminar/B738/autopilot/capt_disco_press");
  } else if (acf_type == 1) {
    stab_trim_up = link_dataref_cmd_hold("x737/trim/CAPT_STAB_TRIM_UP_ALL");
    stab_trim_down = link_dataref_cmd_hold("x737/trim/CAPT_STAB_TRIM_DOWN_ALL");
    ap_disconnect = link_dataref_cmd_once("x737/yoke/capt_AP_DISENG_BTN");
  } else {
    stab_trim_up = link_dataref_cmd_hold("sim/flight_controls/pitch_trim_up");
    stab_trim_down = link_dataref_cmd_hold("sim/flight_controls/pitch_trim_down");
    ap_disconnect = link_dataref_cmd_once("sim/autopilot/servos_toggle");
  }
  *ap_disconnect = 0;

  /* temporary dataref for stab trim main elec */
  int *stab_trim_me = link_dataref_int("xpserver/stab_trim_me");
  
  /* read potentiometer from analog input #1 on BU0836A card, scale it to the range 0-100 */
  ret1 = axis_input(device,3,&value1,minval,maxval);
  ret2 = axis_input(device,5,&value2,minval,maxval);
  value1 = min(max((value1 - minbrake) / (maxbrake - minbrake),0.0),1.0);
  value2 = min(max((value2 - minbrake) / (maxbrake - minbrake),0.0),1.0);
  *left_brake = brakescale * max(value1,value2);
  if ((ret1 == 1) || (ret2 == 1)) {
    // printf("Left Brake has value: %f %f %f \n",value1,value2,*left_brake);
  }

  ret1 = axis_input(device,4,&value1,minval,maxval);
  ret2 = axis_input(device,6,&value2,minval,maxval);
  value1 = min(max((value1 - minbrake) / (maxbrake - minbrake),0.0),1.0);
  value2 = min(max((value2 - minbrake) / (maxbrake - minbrake),0.0),1.0);
  *right_brake = brakescale * max(value1,value2);
  if ((ret1 == 1) || (ret2 == 1)) {
    //    printf("Right Brake has value: %f %f %f \n",value1,value2,*right_brake);
  }

  /* yoke buttons */
  ret = digital_input(device,card,0,&button,0);
  if (button == 1) {
    //printf("stab trim down \n");
    if (*stab_trim_me == 1) {
      *stab_trim_down = 1;
    }
  } else {
    *stab_trim_down = 0;
  }
   
  ret = digital_input(device,card,1,&button,0);
  if (button == 1) {
    //printf("stab trim up \n");
    if (*stab_trim_me == 1) {
      *stab_trim_up = 1;
    }
  } else {
    *stab_trim_up = 0;
  }

  /* AP disconnect button */
  if ((acf_type == 1) || (acf_type == 2) || (acf_type == 3)) {
    ret = digital_input(device,card,4,ap_disconnect,0);
  } else {
    if (*ap_engage == 2) {
      ret = digital_input(device,card,4,ap_disconnect,0);
    }
  }
  /* hat switch buttons */
  /* 
     6 up
     7 down
     8 left
     9 right
     10 pushbutton
  */

  if (*viewmode == INT_MISS) {
    *viewmode = 0;
  }
 
  /* INT MIC button on Yoke resets view to default */
  ret = digital_input(device,card,2,&button,0);
  if ((ret == 1) && (button == 1)) {
    *viewmode=0;
    *view_horizontal=0.0;
    *view_vertical=0.0;
    *forward_with_nothing=1;
    *forward_with_panel=0;
    *circle=0;
    *free_camera=0;
  }
  

  ret = digital_input(device,card,10,&button,0);
  if ((ret == 1) && (button == 1)) {
    *viewmode=*viewmode + 1;
    if (*viewmode == 3) {
      *viewmode = 0;
    }
    printf("Viewmode has value %i \n",*viewmode);

    if (*viewmode == 0) {
      *forward_with_nothing = 1;
      *forward_with_panel = 0;
      *circle = 0;
      *free_camera = 0;
    } else if (*viewmode == 1) {
      *forward_with_nothing = 0;
      *forward_with_panel = 1;
      *circle = 0;
      *free_camera = 0;
    } else if (*viewmode == 2) {
      *view_horizontal = 0.0;
      *view_vertical = 0.0;
      *forward_with_nothing = 0;
      *forward_with_panel = 0;
      *circle = 1;
      *free_camera = 0;
    } else if (*viewmode == 3) {
      *view_horizontal = 0.0;
      *view_vertical = 0.0;
      *forward_with_nothing = 0;
      *forward_with_panel = 0;
      *circle = 0;
      *free_camera = 1;
    }
  }


  /* forward with nothing */
  if ((*viewmode == 0) || (*viewmode == 1)) {
    
    ret = digital_input(device,card,8,&button,0);
    if ((ret == 1) && (button == 1)) {
      *view_horizontal -= 45.0;
    }
    ret = digital_input(device,card,9,&button,0);
    if ((ret == 1) && (button == 1)) {
      *view_horizontal += 45.0;
    }
    ret = digital_input(device,card,6,&button,0);
    if ((ret == 1) && (button == 1)) {
      *view_vertical += 22.5;
    }
    ret = digital_input(device,card,7,&button,0);
    if ((ret == 1) && (button == 1)) {
      *view_vertical -= 22.5;
    }

  } else if ((*viewmode == 2) || (*viewmode == 3)) {
    ret = digital_input(device,card,8,&button,0);
    if (button == 1) {
      *view_left = 1;
    } else {
      *view_left = 0;
    }
    ret = digital_input(device,card,9,&button,0);
    if (button == 1) {
      *view_right = 1;
    } else {
      *view_right = 0;
    }
    ret = digital_input(device,card,6,&button,0);
    if (button == 1) {
      *view_up = 1;
    } else {
      *view_up = 0;
    }
    ret = digital_input(device,card,7,&button,0);
    if (button == 1) {
      *view_down = 1;
    } else {
      *view_down = 0;
    }
 
  }
  
}

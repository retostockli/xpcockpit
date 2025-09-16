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
#include <sys/time.h>

#include "common.h"
#include "libleo.h"
#include "serverdata.h"
#include "b737_yokerudder.h"

#define max(A,B) ((A)>(B) ? (A) : (B)) 
#define min(A,B) ((A)<(B) ? (A) : (B))

struct timeval time_viewmodebutton; 

/* Yoke Buttons
   0: Stab Trim down
   1: Stab Trim up
   2: INT MIC CAPT
   3: MIC CAPT
   4: AP Disconnect
   5:
   6: HAT Up
   7: HAT Down
   8: HAT Left
   9: HAT Right
   10: HAT Pushbutton

   16: INT MIC F/O
   17: MIC F/O
*/

int view_is_copilot;


void b737_yokerudder(void)
{

  int ret = 0;
  int device = 0;
  float minval = 0.0;
  float maxval = 100.0;

  float minbrake = 5.0;
  float maxbrake = 80.0;

  float brakescale;

  struct timeval newtime;
  
  int button;
  float value1,value2;
  int ret1,ret2;

  int viewmode_changed = 0;

  if ((view_is_copilot != 0) && (view_is_copilot != 1)) view_is_copilot = 0;
  
  float* left_brake;
  float* right_brake;
  if ((acf_type == 2) || (acf_type == 3)) {
    brakescale = 0.5;
    left_brake = link_dataref_flt("laminar/B738/axis/left_toe_brake",-3);
    right_brake = link_dataref_flt("laminar/B738/axis/right_toe_brake",-3);
  } else {
    brakescale = 1.0;
    left_brake = link_dataref_flt("sim/cockpit2/controls/left_brake_ratio",-3);
    right_brake = link_dataref_flt("sim/cockpit2/controls/right_brake_ratio",-3);
  }

  int* override_auto_toe_brake = link_dataref_int("sim/operation/override/override_toe_brakes");
  *override_auto_toe_brake = 1;
  
  int* viewmode = link_dataref_int("xpserver/viewmode");

  // datarefs to adjust for captain and copilot eye position on curved screen
  float *pilot_position = link_dataref_flt("sim/graphics/view/pilots_head_x",-3);
  float *pilot_heading = link_dataref_flt("sim/graphics/view/dome_offset_heading",0);
  
  int* forward_with_nothing = link_dataref_cmd_once("sim/view/forward_with_nothing");
  int* forward_with_panel = link_dataref_cmd_once("sim/view/forward_with_panel");
  int* circle = link_dataref_cmd_once("sim/view/circle");
  int* free_camera = link_dataref_cmd_once("sim/view/free_camera");

  int* view_left = link_dataref_cmd_hold("sim/general/left");
  int* view_right = link_dataref_cmd_hold("sim/general/right");
  int* view_up = link_dataref_cmd_hold("sim/general/up");
  int* view_down = link_dataref_cmd_hold("sim/general/down");

  int* weapon_down = link_dataref_cmd_hold("sim/weapons/weapon_select_down");
  int* weapon_up = link_dataref_cmd_hold("sim/weapons/weapon_select_up");
  int* target_down = link_dataref_cmd_hold("sim/weapons/weapon_target_down");
  int* target_up = link_dataref_cmd_hold("sim/weapons/weapon_target_up");
  int* fire_weapon = link_dataref_cmd_hold("sim/weapons/fire_any_armed");
  int* fire_guns= link_dataref_cmd_hold("sim/weapons/fire_guns");
  int* fire_a2a = link_dataref_cmd_hold("sim/weapons/fire_air_to_air");

  //int *ap_engage = link_dataref_int("sim/cockpit/autopilot/autopilot_mode");    // AP engage/disengage mode

  int *stab_trim_up;
  int *stab_trim_down;
  int *ap_disconnect;
  if ((acf_type == 2) || (acf_type == 3)) {
    stab_trim_up = link_dataref_cmd_hold("sim/flight_controls/pitch_trim_up");
    stab_trim_down = link_dataref_cmd_hold("sim/flight_controls/pitch_trim_down");
    //    stab_trim_up = link_dataref_cmd_hold("laminar/B738/flight_controls/pitch_trim_up");
    //    stab_trim_down = link_dataref_cmd_hold("laminar/B738/flight_controls/pitch_trim_down");
    ap_disconnect = link_dataref_cmd_hold("laminar/B738/autopilot/capt_disco_press");
  } else if (acf_type == 1) {
    stab_trim_up = link_dataref_cmd_hold("x737/trim/CAPT_STAB_TRIM_UP_ALL");
    stab_trim_down = link_dataref_cmd_hold("x737/trim/CAPT_STAB_TRIM_DOWN_ALL");
    ap_disconnect = link_dataref_cmd_hold("x737/yoke/capt_AP_DISENG_BTN");
  } else {
    stab_trim_up = link_dataref_cmd_hold("sim/flight_controls/pitch_trim_up");
    stab_trim_down = link_dataref_cmd_hold("sim/flight_controls/pitch_trim_down");
    ap_disconnect = link_dataref_cmd_hold("sim/autopilot/servos_off_any");
  }

  //int *ptt = link_dataref_cmd_hold("xpilot/ptt");
  int *mic_capt_yoke = link_dataref_int("xpserver/mic_capt_yoke");
  int *mic_fo_yoke = link_dataref_int("xpserver/mic_fo_yoke");

  /* MIC Buttons */
  ret = digital_input(device,3,mic_capt_yoke,0);
  if (ret == 1) {
    printf("MIC CAPTAIN ON YOKE \n");
  }
  ret = digital_input(device,17,mic_fo_yoke,0);
  if (ret == 1) {
    printf("MIC COPILOT ON YOKE\n");
  }
  
  /* if ((*mic_capt_yoke == 1) || (*mic_fo_yoke == 1)) { */
  /*   *ptt = 1; */
  /* } else { */
  /*   *ptt = 0; */
  /* } */
  
  /* read potentiometer from analog input #1 on BU0836A card, scale it to the range 0-100 */
  ret1 = axis_input(device,3,&value1,minval,maxval);
  ret2 = axis_input(device,5,&value2,minval,maxval);
  value1 = min(max((value1 - minbrake) / (maxbrake - minbrake),0.0),1.0);
  value2 = min(max((value2 - minbrake) / (maxbrake - minbrake),0.0),1.0);
  *left_brake = brakescale * max(value1,value2);
  if ((ret1 == 1) || (ret2 == 1)) {
    //printf("Left Brake has value: %f %f %f \n",value1,value2,*left_brake);
  }

  ret1 = axis_input(device,4,&value1,minval,maxval);
  ret2 = axis_input(device,7,&value2,minval,maxval);
  value1 = min(max((value1 - minbrake) / (maxbrake - minbrake),0.0),1.0);
  value2 = min(max((value2 - minbrake) / (maxbrake - minbrake),0.0),1.0);
  *right_brake = brakescale * max(value1,value2);
  if ((ret1 == 1) || (ret2 == 1)) {
    //printf("Right Brake has value: %f %f %f \n",value1,value2,*right_brake);
  }

  /* yoke buttons */
  ret = digital_input(device,0,&button,0);
  if (button == 1) {
    printf("stab trim down \n");
    *stab_trim_down = 1;
 
  } else {
    *stab_trim_down = 0;
  }
   
  ret = digital_input(device,1,&button,0);
  if (button == 1) {
    printf("stab trim up \n");
    *stab_trim_up = 1;
  } else {
    *stab_trim_up = 0;
  }

  /* AP disconnect button */
    ret = digital_input(device,4,ap_disconnect,0);
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

  /* Fighter Jet */
  /* if (acf_type == 5) { */
  /*   ret = digital_input(device,3,&button,0); */
  /*   *fire_guns = button;       */
  /* } */
  
  /* INT MIC button on Yoke resets view to default */
  /* CAPTAIN */
  ret = digital_input(device,2,&button,0);
  if ((ret == 1) && (button == 1)) {
    view_is_copilot = 0;
    *viewmode=0;
    *forward_with_nothing=1;
    *forward_with_panel=0;
    *circle=0;
    *free_camera=0;
    viewmode_changed = 1;
  }
  /* COPILOT */
  ret = digital_input(device,16,&button,0);
  if ((ret == 1) && (button == 1)) {
    view_is_copilot = 1;
    *viewmode=0;
    *forward_with_nothing=1;
    *forward_with_panel=0;
    *circle=0;
    *free_camera=0;
    viewmode_changed = 1;
  }
  
  ret = digital_input(device,10,&button,0);
  if ((ret == 1) && (button == 1)) {
    /* we have a bouncing effect on the yoke viewmode pushbutton so check time since last press */
    gettimeofday(&newtime,NULL);
    float dt = ((newtime.tv_sec - time_viewmodebutton.tv_sec) +
		(newtime.tv_usec - time_viewmodebutton.tv_usec) / 1000000.0)*1000.0;
    /* only execute command if last press is more than 200 milliseconds away */
    if (dt > 200.0) {
      *viewmode=*viewmode + 1;
      if (*viewmode == 3) {
	*viewmode = 0;
      }
      printf("Viewmode has value %i \n",*viewmode);

      time_viewmodebutton.tv_sec = newtime.tv_sec;
      time_viewmodebutton.tv_usec = newtime.tv_usec;

      viewmode_changed = 1;
    }
      
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
     *forward_with_nothing = 0;
      *forward_with_panel = 0;
      *circle = 1;
      *free_camera = 0;
    } else if (*viewmode == 3) {
      *forward_with_nothing = 0;
      *forward_with_panel = 0;
      *circle = 0;
      *free_camera = 1;
    }
  }


  if (*viewmode == 0) {
    /* Only fix pilot position in forward with nothing */
    if (view_is_copilot == 0) {
      /* Captain View */
      *pilot_position = -0.51; // meters
      *pilot_heading = 14.5; // degrees
    } else {
      /* Copilot View */
      *pilot_position = 0.51;  // meters
      *pilot_heading = -14.5; // degrees
    }
 
  } else if ((*viewmode == 1) || (*viewmode == 2) || (*viewmode == 3)) {

    if (viewmode_changed == 1) {
      // Only set these values upon button press (let the user rotate and move in the cockpit afterwards)
      *pilot_position = 0.0;
      *pilot_heading = 0.0;
    }
    
    ret = digital_input(device,8,&button,0);
    if (button == 1) {
      *view_left = 1;
    } else {
      *view_left = 0;
    }
    ret = digital_input(device,9,&button,0);
    if (button == 1) {
      *view_right = 1;
    } else {
      *view_right = 0;
    }
    ret = digital_input(device,6,&button,0);
    if (button == 1) {
      *view_up = 1;
    } else {
      *view_up = 0;
    }
    ret = digital_input(device,7,&button,0);
    if (button == 1) {
      *view_down = 1;
    } else {
      *view_down = 0;
    }
 
  }

}

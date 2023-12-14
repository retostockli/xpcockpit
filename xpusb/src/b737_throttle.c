/* This is the b737_throttle.c code which uses a DCMotors PLUS and the BU0836X card
   to communicate to the Throttle Quadrant by cockpitforyou.com

   Copyright (C) 2009 - 2014  Reto Stockli

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

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "libiocards.h"
#include "serverdata.h"
#include "b737_throttle.h"

#define max(A,B) ((A)>(B) ? (A) : (B)) 
#define min(A,B) ((A)<(B) ? (A) : (B)) 

/* allocation of global variables defined in b737_throttle.h */
int speedbrake_mode; /* 0: no change; 1: H/W controlling; 2: X-Plane controlling */
int parkbrake_mode; /* 0: no change; 1: H/W controlling; 2: X-Plane controlling */
int stabilizer_mode; /* 0: no change; 1: H/W controlling; 2: X-Plane controlling */
float stabilizer_old; /* save value for stabilizer to dampen small changes from pots */


void b737_throttle(void)
{

  /* DCMotors PLUS Card DEVICE 2 */
  /* SERVO 0: STAB TRIM L */
  /* SERVO 1: STAB TRIM R */
  /* SERVO 2: PARK BRAKE */

  /* MOTOR 0: Engine Throttle 1 (output 1 = 1) */
  /* MOTOR 1: Engine Throttle 2 (output 2 = 1) */
  /* MOTOR 2: STAB TRIM (output 3 = 1) */
  /* MOTOR 3: Speed Brake */

  /* OUTPUT 0: activate or idle MOTOR 0 */
  /* OUTPUT 1: activate or idle MOTOR 1 */
  /* OUTPUT 2: activate or idle MOTOR 2 */

  
  /* BU0836 Card DEVICE 3 */
  /* AXIS 0: Throttle lever 1 */
  /* AXIS 1: Throttle lever 2 */
  /* AXIS 2: Reverse Thrust lever 1 */
  /* AXIS 3: Reverse Thrust lever 2 */
  /* AXIS 4: Speed Brake */
  /* AXIS 5: Flaps */
  /* AXIS 6: Stabilizer Trim*/

  /* INPUT 0: Park Brake */
  /* INPUT 1: Engine 1 start lever */
  /* INPUT 2: Engine 2 start lever */
  /* INPUT 3: Stab Trim "Main Elect" Switch */
  /* INPUT 4: Stab "Trim Auto Pilot" Switch */
  /* INPUT 5: Gear Warning Horn Cutout Button */
  /* INPUT 8: TO/GA Button(s) */
  /* INPUT 9: AT Disengage Button(s) */

  int device_dcmotor = 2;
  int device_bu0836 = 3; // BU0836X

  int card = 0;
  int ret;
  int axis;
  int input;
  int i;
  int en;

  float minval = 0.0;
  float maxval = 1.0;
  
  float throttle0=FLT_MISS;
  float throttle1=FLT_MISS;

  float reverser0=FLT_MISS;
  float reverser1=FLT_MISS;

  float speedbrake=FLT_MISS;
  float minspeedbrake;
  float maxspeedbrake;
  float minspeedbrake_x737 = 0.0;
  float maxspeedbrake_x737 = 1.0;
  float minspeedbrake_zibo = 0.0;
  float maxspeedbrake_zibo = 1.0;
  float minspeedbrake_xplane = -0.5;
  float maxspeedbrake_xplane = 1.0;
  float difspeedbrake = 0.05; /* minimum difference between x-plane and H/W to toggle a change */

  float stabilizer=FLT_MISS;
  float minstabilizer_x737 = -0.280;
  float maxstabilizer_x737 =  1.000;
  float minstabilizer_zibo = -1.000;
  float maxstabilizer_zibo =  0.719;
  float minstabilizer_xplane = -1.000;
  float maxstabilizer_xplane =  1.000;
  float minstabilizer;
  float maxstabilizer;
  float difstabilizer = 0.01; /* minimum difference between x-plane and H/W to toggle a change */
  
  int parkbrake;
  float difparkbrake = 0.05; /* minimum difference between x-plane and H/W to toggle a change */

  float flap=FLT_MISS;

  int ivalue;
  float value;

  int one = 1;
  int zero = 0;

  int *num_engines = link_dataref_int("sim/aircraft/engine/acf_num_engines");
  float *throttle = link_dataref_flt_arr("sim/cockpit2/engine/actuators/throttle_jet_rev_ratio", 16, -1, -2);
  float *throttle_actuator = link_dataref_flt_arr("sim/cockpit2/engine/actuators/throttle_ratio",16, -1, -2);

  float *zibo_throttle0;
  float *zibo_throttle1;
  float *zibo_reverser0;
  float *zibo_reverser1;
  if ((acf_type == 2) || (acf_type == 3)) {
    float *zibo_throttle0 = link_dataref_flt("laminar/B738/engine/thrust1_leveler", -2);
    float *zibo_throttle1 = link_dataref_flt("laminar/B738/engine/thrust2_leveler", -2);
    float *zibo_reverser0 = link_dataref_flt("laminar/B738/flt_ctrls/reverse_lever1", -2);
    float *zibo_reverser1 = link_dataref_flt("laminar/B738/flt_ctrls/reverse_lever2", -2);
  }
  
  float *flap_ratio;
  if ((acf_type == 2) || (acf_type == 3)) {
    flap_ratio = link_dataref_flt("laminar/B738/flt_ctrls/flap_lever", -4);
  } else {
    flap_ratio = link_dataref_flt("sim/flightmodel/controls/flaprqst", -4);
  }
  float *parkbrake_xplane;
  int *parkbrake_button;
  if (acf_type == 4) {
    parkbrake_xplane = link_dataref_flt("mgdornier/do328/parking_brake_ratio",-1);
  } else if (acf_type == 3) {
    parkbrake_xplane = link_dataref_flt("laminar/B738/parking_brake_pos",-1);
    parkbrake_button = link_dataref_cmd_once("laminar/B738/push_button/park_brake_on_off");
  } else {
    parkbrake_xplane = link_dataref_flt("sim/flightmodel/controls/parkbrake",-1);
  }
  

  float *speedbrake_xplane;
  if ((acf_type == 2) || (acf_type == 3)) {
    speedbrake_xplane = link_dataref_flt("laminar/B738/flt_ctrls/speedbrake_lever",-2);
  } else if (acf_type == 1) {
    speedbrake_xplane = link_dataref_flt("x737/systems/speedbrake/spdbrkLeverPos",-2);
  } else {
    speedbrake_xplane = link_dataref_flt("sim/cockpit2/controls/speedbrake_ratio",-2);
  }

  int *trim_up = link_dataref_cmd_hold("sim/flight_controls/pitch_trim_up_mech");
  int *trim_down = link_dataref_cmd_hold("sim/flight_controls/pitch_trim_down_mech");
  float *stabilizer_xplane = link_dataref_flt("sim/cockpit2/controls/elevator_trim",-3);

  int *toga_button;
  if ((acf_type == 2) || (acf_type == 3)) {
    toga_button = link_dataref_cmd_once("laminar/B738/autopilot/left_toga_press");
  } else if (acf_type == 1) {
    toga_button = link_dataref_int("x737/systems/athr/toggle_TOGA");
  } else {
    //    toga_button = link_dataref_cmd_once("sim/engines/TOGA_power");
    toga_button = link_dataref_cmd_once("sim/autopilot/take_off_go_around");
  }
  
  int *at_disconnect_button;
  if ((acf_type == 2) || (acf_type == 3)) {
    at_disconnect_button = link_dataref_cmd_once("laminar/B738/autopilot/left_at_dis_press");
  } else if (acf_type == 1) {
    at_disconnect_button = link_dataref_cmd_once("x737/mcp/ATHR_ARM_OFF");
  } else {
    at_disconnect_button = link_dataref_cmd_once("sim/autopilot/autothrottle_off");
  }
  
  int *autothrottle_on;
  float *speed_mode;
  float *lock_throttle;
  if ((acf_type == 2) || (acf_type == 3)) {
    // autothrottle_on = link_dataref_int("laminar/B738/autopilot/autothrottle_status");
    speed_mode = link_dataref_flt("laminar/B738/autopilot/speed_mode",0);
    lock_throttle = link_dataref_flt("laminar/B738/autopilot/lock_throttle",0);
  } else if (acf_type == 1) {
    autothrottle_on = link_dataref_int("x737/systems/athr/athr_active");
    lock_throttle = link_dataref_flt("xpserver/lock_throttle1",0);
    *lock_throttle = 1.0;
  } else {
    autothrottle_on = link_dataref_int("sim/cockpit2/autopilot/autothrottle_on");
    lock_throttle = link_dataref_flt("xpserver/lock_throttle1",0);
    *lock_throttle = 1.0;
  }
 
  float *fuel_mixture_left;
  float *fuel_mixture_right;
  float *fuel_mixture;
  if ((acf_type == 2) || (acf_type == 3)) { 
    fuel_mixture_left = link_dataref_flt("laminar/B738/engine/mixture_ratio1",-2);
    fuel_mixture_right = link_dataref_flt("laminar/B738/engine/mixture_ratio2",-2);
  } else if (acf_type == 1) {
    fuel_mixture_left = link_dataref_flt("x737/cockpit/tq/leftCutoffLeverPos",-2);
    fuel_mixture_right = link_dataref_flt("x737/cockpit/tq/rightCutoffLeverPos",-2);
  } else {
    fuel_mixture = link_dataref_flt_arr("sim/flightmodel/engine/ENGN_mixt",16,-1,-2);
  }
  
     
  int *horn_cutoff;
  if ((acf_type == 2) || (acf_type == 3)) { 
    horn_cutoff = link_dataref_cmd_once("laminar/B738/alert/gear_horn_cutout");
  } else if (acf_type == 1) {
    horn_cutoff = link_dataref_cmd_once("x737/TQ/HORN_CUTOFF");
  } else {
    horn_cutoff = link_dataref_int("xpserver/HORN_CUTOFF");
  }


  /* Covers of the Stab Trim Cutoff switches: open */
  if ((acf_type == 2) || (acf_type == 3)) { 
    int *main_elect_cover_toggle = link_dataref_cmd_once("laminar/B738/toggle_switch/el_trim_lock");
    float *main_elect_cover_pos = link_dataref_flt("laminar/B738/toggle_switch/el_trim_lock_pos",-3);
    ret = set_switch_cover(main_elect_cover_pos,main_elect_cover_toggle,1);     
    int *autopilot_cover_toggle = link_dataref_cmd_once("laminar/B738/toggle_switch/ap_trim_lock");
    float *autopilot_cover_pos = link_dataref_flt("laminar/B738/toggle_switch/ap_trim_lock_pos",-3);
    ret = set_switch_cover(autopilot_cover_pos,autopilot_cover_toggle,1);     
  }

  /* Stab Trim Cutoff Switches */
  
  int *main_elect_toggle;
  float *main_elect_pos;
  int *autopilot_toggle;
  float *autopilot_pos;
  if ((acf_type == 2) || (acf_type == 3)) { 
    main_elect_toggle = link_dataref_cmd_once("laminar/B738/toggle_switch/el_trim");
    main_elect_pos = link_dataref_flt("laminar/B738/toggle_switch/el_trim_pos",-3);
    autopilot_toggle = link_dataref_cmd_once("laminar/B738/toggle_switch/ap_trim");
    autopilot_pos = link_dataref_flt("laminar/B738/toggle_switch/ap_trim_pos",-3);
  }

  
  /*
  float *stabilizer_min = link_dataref_flt("sim/aircraft/controls/acf_min_trim_elev",-3);
  float *stabilizer_max = link_dataref_flt("sim/aircraft/controls/acf_max_trim_elev",-3);
  */

  /* fetch stabilizer setting from x737 dataref if available */
  if ((acf_type == 2) || (acf_type == 3)) {
    minstabilizer = minstabilizer_zibo;
    maxstabilizer = maxstabilizer_zibo;
  } else if (acf_type == 1) {
    minstabilizer = minstabilizer_x737;
    maxstabilizer = maxstabilizer_x737;
  } else {
    minstabilizer = minstabilizer_xplane;
    maxstabilizer = maxstabilizer_xplane;
  }

  if ((acf_type == 2) || (acf_type == 3)) {
    minspeedbrake = minspeedbrake_zibo;
    maxspeedbrake = maxspeedbrake_zibo;
  } else if (acf_type == 1) {
    minspeedbrake = minspeedbrake_x737;
    maxspeedbrake = maxspeedbrake_x737;
  } else {
    minspeedbrake = minspeedbrake_xplane;
    maxspeedbrake = maxspeedbrake_xplane;
  }
  
  /* BU0836 Card */
  axis = 0;
  ret = axis_input(device_bu0836,axis,&throttle0,minval,maxval);
  if (ret == 1) {
    //    printf("Thrust Lever 0, Analog Input %i has value: %f \n",axis,throttle0);
  }
  axis = 1;
  ret = axis_input(device_bu0836,axis,&throttle1,minval,maxval);
  if (ret == 1) {
    //    printf("Thrust Lever 1, Analog Input %i has value: %f \n",axis,throttle1);
  }
  axis = 2;
  ret = axis_input(device_bu0836,axis,&reverser0,minval,maxval);
  if (ret == 1) {
    //    printf("Reverser 1, Analog Input %i has value: %f \n",axis,reverser0);
  }
  axis = 3;
  ret = axis_input(device_bu0836,axis,&reverser1,minval,maxval);
  if (ret == 1) {
    //    printf("Reverser 2,Analog Input %i has value: %f \n",axis,reverser1);
  }
  axis = 4;
  ret = axis_input(device_bu0836,axis,&speedbrake,minval,maxval);    
  if (ret == 1) {
    //    printf("Speedbrake, Analog Input %i has value: %f \n",axis,speedbrake);
  }
   /* H/W Lever goes from 0.71 .. 0.0 (reverse) */
  if (speedbrake != FLT_MISS) {
    speedbrake = min(max(1.0 - speedbrake/0.7,0.0),1.0); 
  }
  /* Scale speedbrake value to X-Plane range */
  if (speedbrake != FLT_MISS) {
    speedbrake = speedbrake * (maxspeedbrake - minspeedbrake) + minspeedbrake;
  }
 
  if ((speedbrake_mode < 0) && (speedbrake_mode > 2)) speedbrake_mode = 0;
  if ((speedbrake_mode == 0) && (speedbrake != FLT_MISS) && (*speedbrake_xplane != FLT_MISS) &&
      (fabs(speedbrake - *speedbrake_xplane) > difspeedbrake)) {
    if (ret == 1) {
      /* H/W has changed: Manual Mode */
      speedbrake_mode = 1;
    } else {
      /* X-Plane has changed: AP Mode */
      speedbrake_mode = 2;
    }
  }  
  if ((speedbrake_mode != 0) && (speedbrake != FLT_MISS) && (*speedbrake_xplane != FLT_MISS) &&
      (fabs(speedbrake - *speedbrake_xplane) < difspeedbrake)) {
    /* Idle mode: H/W and X-Plane in same position */
    speedbrake_mode = 0;
  }
   
  axis = 5;
  ret = axis_input(device_bu0836,axis,&flap,minval,maxval);
  //  if (ret == 1) {
    if (flap < 0.5*(0.124+0.232)) {
      *flap_ratio = 0.000;
    } else if (flap < 0.5*(0.232+0.367)) {
      *flap_ratio = 0.125;
    } else if (flap < 0.5*(0.367+0.500)) {
      *flap_ratio = 0.250;
    } else if (flap < 0.5*(0.500+0.602)) {
      *flap_ratio = 0.375;
    } else if (flap < 0.5*(0.602+0.719)) {
      *flap_ratio = 0.500;
    } else if (flap < 0.5*(0.719+0.836)) {
      *flap_ratio = 0.625;
    } else if (flap < 0.5*(0.836+0.964)) {
      *flap_ratio = 0.750;
    } else if (flap < 0.5*(0.964+0.998)) {
      *flap_ratio = 0.875;
    } else {
      *flap_ratio = 1.000;
    }

    //    printf("Flaps Lever, Analog Input %i has value: %f %f \n",axis,flap,*flap_ratio);
    //  }
    
  axis = 6;
  ret = axis_input(device_bu0836,axis,&stabilizer,-maxstabilizer,-minstabilizer);
  stabilizer = -stabilizer;
  if (ret == 1) {
    //printf("Stabilizer Trim: %i %f %f \n",stabilizer_mode,stabilizer,*stabilizer_xplane);
  }

  /* STAB MODE: 0=IDLE, 1=HW Driving, 2=XP Driving */
  if ((stabilizer_mode < 0) || (stabilizer_mode > 2)) stabilizer_mode = 0;
  if ((stabilizer_mode == 0) && (stabilizer != FLT_MISS) && (*stabilizer_xplane != FLT_MISS) &&
      (fabs(stabilizer - *stabilizer_xplane) >= difstabilizer)) {
    if ((ret == 1) && ((fabs(stabilizer-stabilizer_old) >= 0.005))) {
      /* H/W has changed: Manual Mode */
      stabilizer_mode = 1;
    } else {
      /* X-Plane has changed: AP Mode */
      if ((acf_type == 2) || (acf_type == 3)) {
	//printf("AP SWITCH: %f \n",*autopilot_pos);
	if (*autopilot_pos == 0.0) {
	  /* Only drive stab trim actuator with A/P if stab trim cutout switch
	     on control column is inactive: prevents stabilizer runaway (see lion air crash) */
	  stabilizer_mode = 2;
	} else {
	  stabilizer_mode = 1;
	}
      } else {
	stabilizer_mode = 1;
      }
    }
  }  
  if ((stabilizer_mode != 0) && (stabilizer != FLT_MISS) && (*stabilizer_xplane != FLT_MISS) &&
      (fabs(stabilizer - *stabilizer_xplane) < difstabilizer)) {
    /* Idle mode: H/W and X-Plane in same position */
    stabilizer_mode = 0;
  }
  stabilizer_old = stabilizer;
    
  input = 0;
  ret = digital_input(device_bu0836,0,input,&parkbrake,0);
  if (ret == 1) {
    printf("Park Brake: %i \n",parkbrake);
  }

  if ((parkbrake_mode < 0) && (parkbrake_mode > 2)) parkbrake_mode = 0;
  if ((parkbrake_mode == 0) && (parkbrake != INT_MISS) && (*parkbrake_xplane != FLT_MISS) &&
      (fabs((float) parkbrake - *parkbrake_xplane) > difparkbrake)) {
    if (ret == 1) {
      /* H/W has changed */
      parkbrake_mode = 1;
    } else {
      /* X-Plane has changed */
      parkbrake_mode = 2;
    }
  }  
  if ((parkbrake_mode != 0) && (parkbrake != FLT_MISS) && (*parkbrake_xplane != FLT_MISS) &&
      (fabs((float) parkbrake - *parkbrake_xplane) < difparkbrake)) {
    /* Idle mode: H/W and X-Plane in same position */
    parkbrake_mode = 0;
  }

  /* Parkbrake Servo is out of order, so set Parkbrake Mode to 1 */
  parkbrake_mode = 1;
  
  //printf("%i: HW %i XP %f \n",parkbrake_mode,parkbrake,*parkbrake_xplane);
  
  if ((acf_type == 1) || (acf_type == 2) || (acf_type == 3)) {
    if (*fuel_mixture_left != FLT_MISS) {
      input = 1;
      ret = digital_input(device_bu0836,0,input,&ivalue,0);
      *fuel_mixture_left = (float) 1-ivalue;
      if (ret == 1) {
	printf("Fuel Mixture Engine 1: %i \n",ivalue);
      }
    }    
    if (*fuel_mixture_right != FLT_MISS) {
      input = 2;
      ret = digital_input(device_bu0836,0,input,&ivalue,0);
      *fuel_mixture_right = (float) 1-ivalue;
      if (ret == 1) {
	printf("Fuel Mixture Engine 2: %i \n",ivalue);
      }
    }    
  } else {
    if (*num_engines != INT_MISS) {
      for (i=0;i<*num_engines;i++) {
	if ((i<(*num_engines/2)) || (*num_engines == 1)) {
	  if (*(fuel_mixture+i) != FLT_MISS) {
	    input = 1;
	    ret = digital_input(device_bu0836,0,input,&ivalue,0);
	    *(fuel_mixture+i) = (float) 1-ivalue;
	    if (ret == 1) {
	      printf("Fuel Mixture Engine %i: %i \n",i+1,ivalue);
	    }
	  }
	} else {
	  if (*(fuel_mixture+i) != FLT_MISS) {
	    input = 2;
	    ret = digital_input(device_bu0836,0,input,&ivalue,0);
	    *(fuel_mixture+i) = (float) 1-ivalue;
	    if (ret == 1) {
	      printf("Fuel Mixture Engine %i: %i \n",i+1,ivalue);
	    }
	  }
	}
      }
    }
  }

  if ((acf_type == 2) || (acf_type == 3)) {
    input = 3;
    value = *main_elect_pos;
    if (value != FLT_MISS) {
      value = 1 - value;
    }
    ret = digital_inputf(device_bu0836,0,input,&value,0);
    if (value != FLT_MISS) {
      value = 1 - value;
    }
    if (ret == 1) {
      printf("Stabilizer Trim Main Elect: %f \n",value);
    }
    ret = set_state_togglef(&value,main_elect_pos,main_elect_toggle);     

    input = 4;  
    value = *autopilot_pos;
    ret = digital_inputf(device_bu0836,0,input,&value,0);
    if (ret == 1) {
      printf("Stabilizer Trim Auto Pilot: %f \n",value);
    }
    ret = set_state_togglef(&value,autopilot_pos,autopilot_toggle);     
  }
  
  input = 5;
  ret = digital_input(device_bu0836,0,input,horn_cutoff,0);
  if (ret == 1) {
    printf("Gear Warning Horn Cutout Button: %i \n",*horn_cutoff);
  }


  /* activate TO/GA */
  input = 8;
  ret = digital_input(device_bu0836,0,input,toga_button,0);
  if ((ret == 1) && (*toga_button == 1)) {
    printf("TOGA pressed \n");
  }

  /* disengage AT if armed */
  input = 9;
  ret = digital_input(device_bu0836,0,input,at_disconnect_button,0);
  if ((ret == 1) && (*at_disconnect_button == 1)) {
    printf("AT Disengaged: \n");
  }


  /* STAB TRIM CONTROL */
  if (stabilizer_mode == 0) {
    /* No Change */
    /* set motor of stab trim to idle */
    ret = digital_output(device_dcmotor,card,2,&zero);    
    value = 0.0;
    ret = motors_output(device_dcmotor,2,&value,maxval);

    *trim_up = 0;
    *trim_down = 0;
  } 
  if (stabilizer_mode == 1) {
    /* manual stabilizer trim */
    /* set motor of stab trim to idle */
    ret = digital_output(device_dcmotor,card,2,&zero);    
    value = 0.0;
    ret = motors_output(device_dcmotor,2,&value,maxval);

    /* set X-Plane value to H/W value by using commands */
    if (stabilizer > *stabilizer_xplane) {
      *trim_up = 1;
      *trim_down = 0;
    } else {
      *trim_up = 0;
      *trim_down = 1;
    }

    printf("Manual Stabilizer Mode: %f %f \n ",stabilizer,*stabilizer_xplane);
    
  } 
  if (stabilizer_mode == 2) {
    /* Auto Stabilizer Trim */

    /* DCMotors PLUS Card */
    
    if ((*stabilizer_xplane != FLT_MISS) && (stabilizer != FLT_MISS) &&
	(*stabilizer_xplane > minstabilizer) && (*stabilizer_xplane < maxstabilizer)) {

      value = 0.0;
      
      if (*stabilizer_xplane > (stabilizer+difstabilizer)) {
	if (stabilizer < maxstabilizer)	{
	  value = -max(min(pow((fabs(*stabilizer_xplane - stabilizer))*2.0,0.5),maxval),0.45*maxval);
	  //	  printf("Stabilizer Trim: %f of %f: motor %f \n",stabilizer,*stabilizer_xplane,value);
	  ret = digital_output(device_dcmotor,card,2,&one); /* activate motor for stabilizer trim */
	} else {
	  ret = digital_output(device_dcmotor,card,2,&zero); /* deactivate motor for stabilizer trim */
	}
      }
      if (*stabilizer_xplane < (stabilizer-difstabilizer)) {
	if (stabilizer > minstabilizer)	{
	  value = max(min(pow((fabs(*stabilizer_xplane - stabilizer))*2.0,0.5),maxval),0.45*maxval);
	  //	  printf("Stabilizer Trim: %f of %f: motor %f \n",stabilizer,*stabilizer_xplane,value);
	  ret = digital_output(device_dcmotor,card,2,&one); /* activate motor for stabilizer trim */
	} else {
	  ret = digital_output(device_dcmotor,card,2,&zero); /* deactivate motor for stabilizer trim */
	}
      }
      ret = motors_output(device_dcmotor,2,&value,maxval); /* move motor for stabilizer trim */
    } else {
      ret = digital_output(device_dcmotor,card,2,&zero); /* deactivate motor for stabilizer trim */
      value = 0.0;
      ret = motors_output(device_dcmotor,2,&value,maxval); /* move motor for stabilizer trim */
    }

    *trim_up = 0;
    *trim_down = 0;

    printf("Auto Stabilizer Mode: %f %f \n",stabilizer,*stabilizer_xplane);
  }

  int at = 0;
  if (acf_type == 3) {
    if (*speed_mode >= 1.0) at = 1;
    //printf("%f %f \n",*speed_mode,*lock_throttle);
  } else {
    if (*autothrottle_on >= 1) at = 1;
  }
  
  if ((at == 1) && (*lock_throttle == 1.0)) {
    /* on autopilot and autothrottle */
    
    /* DCMotors PLUS Card */
    
    /* Auto Throttle for engine 1 (first engine on left wing) */
    en = 0;
    if ((*(throttle_actuator+en) != FLT_MISS) && (throttle0 != FLT_MISS)) {
      
      value = 0.0;
      
      if (*(throttle_actuator+en) > (throttle0+0.02)) {
	if (throttle0 < maxval)	{
	  value = max(min(pow((fabs(*(throttle_actuator+en) - throttle0))*2.0,0.5),maxval),0.45*maxval);
	  //printf("+++ Engine 1 Throttle: %f of %f: motor %f \n",throttle0,*(throttle_actuator+en),value);
	  ret = digital_output(device_dcmotor,card,0,&one); /* activate motor for thrust lever for engine 1 */
	} else {
	  ret = digital_output(device_dcmotor,card,0,&zero); /* deactivate motor for thrust lever for engine 1 */
	}
      }
      if (*(throttle_actuator+en) < (throttle0-0.02)) {
	if (throttle0 > minval)	{
	  value = -max(min(pow((fabs(*(throttle_actuator+en) - throttle0))*2.0,0.5),maxval),0.45*maxval);
	  //printf("--- Engine 1 Throttle: %f of %f: motor %f \n",throttle0,*(throttle_actuator+en),value);
	  ret = digital_output(device_dcmotor,card,0,&one); /* activate motor for thrust lever for engine 1 */
	} else {
	  ret = digital_output(device_dcmotor,card,0,&zero); /* deactivate motor for thrust lever for engine 1 */
	}
      }
      ret = motors_output(device_dcmotor,0,&value,maxval); /* move motor for thrust lever for engine 1 */
    } else {
      ret = digital_output(device_dcmotor,card,0,&zero); /* deactivate motor for thrust lever for engine 1 */
      value = 0.0;
      ret = motors_output(device_dcmotor,0,&value,maxval); /* move motor for thrust lever for engine 1 */
    }

    /* Auto Throttle for engine 2 (first engine on right wing)*/
    if (*num_engines != INT_MISS) {
      en = (*num_engines + 1) / 2;
    } else {
      en = 1;
    }
    if ((*(throttle_actuator+en) != FLT_MISS) && (throttle1 != FLT_MISS)) {
      
      value = 0.0;
      
      if (*(throttle_actuator+en) > (throttle1+0.02)) {
	if (throttle1 < maxval)	{
	  value = max(min(pow((fabs(*(throttle_actuator+en) - throttle1))*2.0,0.5),maxval),0.45*maxval);
	  // printf("+++ Engine 2 Throttle: %f of %f: motor %f \n",throttle1,*(throttle_actuator+en),value);
	  ret = digital_output(device_dcmotor,card,1,&one); /* activate motor for thrust lever for engine 2 */
	} else {
	  ret = digital_output(device_dcmotor,card,1,&zero); /* deactivate motor for thrust lever for engine 2 */
	}
      }
      if (*(throttle_actuator+en) < (throttle1-0.02)) {
	if (throttle1 > minval)	{
	  value = -max(min(pow((fabs(*(throttle_actuator+en) - throttle1))*2.0,0.5),maxval),0.45*maxval);
	  // printf("--- Engine 2 Throttle: %f of %f: motor %f \n",throttle1,*(throttle_actuator+en),value);
	  ret = digital_output(device_dcmotor,card,1,&one); /* activate motor for thrust lever for engine 2 */
	} else {
	  ret = digital_output(device_dcmotor,card,1,&zero); /* deactivate motor for thrust lever for engine 2 */
	}
      }
      ret = motors_output(device_dcmotor,1,&value,maxval); /* move motor for thrust lever for engine 2 */
    } else {
      ret = digital_output(device_dcmotor,card,1,&zero); /* deactivate motor for thrust lever for engine 2 */
      value = 0.0;
      ret = motors_output(device_dcmotor,1,&value,maxval); /* move motor for thrust lever for engine 2 */
    }

  } else {
    /* on manual throttle */
    
    ret = digital_output(device_dcmotor,card,0,&zero); /* set motor of throttle 1 to idle */
    ret = digital_output(device_dcmotor,card,1,&zero); /* set motor of throttle 2 to idle */

    value = 0.0;
    ret = motors_output(device_dcmotor,0,&value,maxval);
    ret = motors_output(device_dcmotor,1,&value,maxval);

    if ((acf_type == 2) || (acf_type == 3)) {
      zibo_reverser0 = reverser0;
      zibo_throttle0 = throttle0;
      zibo_reverser1 = reverser1;
      zibo_throttle1 = throttle1;
    } else {
      if (*num_engines != INT_MISS) {
	for (i=0;i<*num_engines;i++) {
	  if ((i<(*num_engines/2)) || (*num_engines == 1)) {
	    /* engines on left wing */
	    /* or single engine */
	    /* or first half minus one if uneven # of engines */
	    if ((throttle0 < 0.05) && (reverser0 > 0.05)) {
	      *(throttle+i) = -reverser0;
	    } else {
	      *(throttle+i) = throttle0;
	    }
	  } else {
	    if ((throttle1 < 0.05) && (reverser1 > 0.05)) {
	      *(throttle+i) = -reverser1;
	    } else {
	      *(throttle+i) = throttle1;
	    }
	  }
	}
      }
    }
  }

  } // manual throttle

    
  if (speedbrake_mode == 0) {
    /* Set motors to idle */
    value = 0.0;
    ret = motors_output(device_dcmotor,3,&value,maxval);
  }
  if (speedbrake_mode == 1) {
    /* Manual Speed Brakes (H/W controlling) */
    
    *speedbrake_xplane = speedbrake; 
    
    value = 0.0;
    ret = motors_output(device_dcmotor,3,&value,maxval);
    
  }
  if (speedbrake_mode == 2) {
    /* Auto Speedbrakes (S/W controlling) */
    
    if ((speedbrake != FLT_MISS) && (*speedbrake_xplane != FLT_MISS)) {
      value = 0.0;
      if (!((speedbrake < 0.0) && (*speedbrake_xplane < 0.0))) {
	if ((*speedbrake_xplane > (speedbrake+difspeedbrake))) {
	  /* speedbrake up movement somehow needs more motor power than down movement ??? */
	  value = -max(min(pow((fabs(*speedbrake_xplane - speedbrake))*1.0,1.0),0.5*maxval),0.35*maxval);
	  //	  printf("Speedbrake: %f of %f: motor %f \n",speedbrake,*speedbrake_xplane,value);
	}
	if ((*speedbrake_xplane < (speedbrake-difspeedbrake))) {
	  value = max(min(pow((fabs(*speedbrake_xplane - speedbrake))*1.0,1.0),0.5*maxval),0.35*maxval);
	  //	  printf("Speedbrake: %f of %f: motor %f \n",speedbrake,*speedbrake_xplane,value);
	}
      }     
      ret = motors_output(device_dcmotor,3,&value,maxval);
    }
  }
  
  if (parkbrake_mode == 0) {
    /* disable Park Brake servo */
    value = -1.0;
    ret = servos_output(device_dcmotor,2,&value,minval,maxval,0,1023);
  }
  if (parkbrake_mode == 1) {
    /* Manual Park Brake */
    /* disable Park Brake servo */
    value = -1.0;
    ret = servos_output(device_dcmotor,2,&value,minval,maxval,0,1023);

    if ((acf_type == 2) || (acf_type == 3)) {
      float fval = (float) parkbrake;
      ret = set_state_togglef(parkbrake_xplane,&fval,parkbrake_button);
      if (ret != 0) {
	printf("Park Brake Toggle: %f %i %i \n",*parkbrake_xplane,parkbrake,*parkbrake_button);
      }

    } else {
      *parkbrake_xplane = (float) parkbrake;
    }
      
    //printf("M: %f \n",*parkbrake_xplane);
    
  }
  if (parkbrake_mode == 2) {
    /* Auto Park Brake */
    if (*parkbrake_xplane != FLT_MISS) {
      value = (1.0-(*parkbrake_xplane))*(0.6-0.25)+0.25;
      //printf("A: %f %f \n",*parkbrake_xplane, value);
      // MY SERVO IS OUT OF ORDER
      ret = servos_output(device_dcmotor,2,&value,minval,maxval,0,1023);
    }
  }

  /* park brake light */
  if (*parkbrake_xplane > 0.5) {
    ret = digital_output(device_dcmotor,card,3,&one); /* activate park brake light */
  } else {
    ret = digital_output(device_dcmotor,card,3,&zero); /* activate park brake light */
  }

  /* show stabilizer trim with servo-driven indicators */
  if (*stabilizer_xplane != FLT_MISS) {
    /* left */
    value = (1.0 - (*stabilizer_xplane - minstabilizer)/(maxstabilizer - minstabilizer));
    //    printf("%f %f %f %f \n",*stabilizer_xplane,minstabilizer,maxstabilizer,value);
    ret = servos_output(device_dcmotor,0,&value,minval,maxval,305,560);
    
    /* right */
    value = ((*stabilizer_xplane) - minstabilizer)/(maxstabilizer - minstabilizer);
    //    printf("%f %f %f %f \n",*stabilizer_xplane,minstabilizer,maxstabilizer,value);
    ret = servos_output(device_dcmotor,1,&value,minval,maxval,305,560);

  }

}

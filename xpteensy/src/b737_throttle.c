/* This is the b737_throttle.c code which handles the CFY Throttle Quadrant rebuilt
   with a Teensy 4.1, L298N motor controllers, a sound board and a AS5048 direction sensor.

   Copyright (C) 2025 Reto Stockli

   Additions for analog axes treatment by Hans Jansen 2011
   Also several cosmetic changes and changes for Linux compilation
   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>

#include "common.h"
#include "libteensy.h"
#include "serverdata.h"
#include "b737_throttle.h"

#define max(A,B) ((A)>(B) ? (A) : (B)) 
#define min(A,B) ((A)<(B) ? (A) : (B)) 

/* Min and Max Potentiometer Values for Levers */
#define THROTTLE_MIN 0.03
#define THROTTLE_MAX 0.98
#define SPEEDBRAKE_LOCK 0.330 // speedbrake mechanical detent preventing movement3
#define SPEEDBRAKE_MIN 0.400
#define SPEEDBRAKE_ARM 0.510 
#define SPEEDBRAKE_MAX 0.995
#define REVERSER_MIN 0.05
#define REVERSER_MAX 0.95
 
/* allocation of global variables defined in b737_throttle.h */
int speedbrake_mode; /* 0: no change; 1: H/W controlling; 2: X-Plane controlling */
int parkbrake_mode; /* 0: no change; 1: H/W controlling; 2: X-Plane controlling */
int stabilizer_mode; /* 0: no change; 1: H/W controlling; 2: X-Plane controlling */

int trim_up_ap_old;
int trim_down_ap_old;
struct timeval trim_time;
int trim_wheel_up_old;
int trim_wheel_down_old;

/* Setup code for the Boeing 737 Throttle Quadrant Teensy Controller */
void init_b737_tq(void)
{
  int te = 0;

  /* Thrust Lever 0 Motor */
  teensy[te].pinmode[0] = PINMODE_MOTOR; // motor EN
  teensy[te].arg1[0] = 1; // motor IN1 */
  teensy[te].arg2[0] = 2; // motor IN2 */
  teensy[te].arg3[0] = 16; // motor Current Sense */
  teensy[te].arg4[0] = 80; // minimum Motor Speed */
  teensy[te].arg5[0] = 255; // minimum Motor Speed */
  /* Thrust Lever 1 Motor */
  teensy[te].pinmode[3] = PINMODE_MOTOR; // motor EN
  teensy[te].arg1[3] = 4; // motor IN1 */
  teensy[te].arg2[3] = 5; // motor IN2 */
  teensy[te].arg3[3] = 17; // motor Current Sense */
  teensy[te].arg4[3] = 80; // minimum Motor Speed */
  teensy[te].arg5[3] = 255; // minimum Motor Speed */
  /* Stab Trim Wheel Motor */
  teensy[te].pinmode[8] = PINMODE_MOTOR; // motor EN
  teensy[te].arg1[8] = 9; // motor IN1 */
  teensy[te].arg2[8] = 10; // motor IN2 */
  teensy[te].arg3[8] = 20; // motor Current Sense */
  /* Speed Brake Lever Motor */
  teensy[te].pinmode[11] = PINMODE_MOTOR; // motor EN
  teensy[te].arg1[11] = 12; // motor IN1 */
  teensy[te].arg2[11] = 13; // motor IN2 */
  teensy[te].arg3[11] = 21; // motor Current Sense */
  teensy[te].arg4[11] = 160; // minimum Motor Speed */
  teensy[te].arg5[11] = 180; // minimum Motor Speed */


  /* This program simulates a servo by using a closed loop code with a motor and a potentiometer
     running inside the teensy */

  /* Thrust Lever 0 */
  program[te][0].type = PROGRAM_CLOSEDLOOP;
  program[te][0].val16[1] = THROTTLE_MIN * 1023; // minimum servo potentiometer value
  program[te][0].val16[2] = THROTTLE_MAX * 1023; // maximum servo potentiometer value
  program[te][0].val8[1] = 26;  // servo potentiometer pin number (needs to be defined separately above)
  program[te][0].val8[2] = 0;  // servo motor pin number (first pin, full motor separately defined above)

  /* Trust Lever 1 */
  program[te][1].type = PROGRAM_CLOSEDLOOP;
  program[te][1].val16[1] = THROTTLE_MIN * 1023; // minimum servo potentiometer value
  program[te][1].val16[2] = THROTTLE_MAX * 1023; // maximum servo potentiometer value
  program[te][1].val8[1] = 27;  // servo potentiometer pin number (needs to be defined separately above)
  program[te][1].val8[2] = 3;  // servo motor pin number (first pin, full motor separately defined above)
  
  /* Speed Brake Lever */
  program[te][2].type = PROGRAM_CLOSEDLOOP;
  program[te][2].val16[1] = SPEEDBRAKE_MIN * 1023; // minimum servo potentiometer value
  program[te][2].val16[2] = SPEEDBRAKE_MAX * 1023; // maximum servo potentiometer value
  program[te][2].val8[1] = 25;  // servo potentiometer pin number (needs to be defined separately above)
  program[te][2].val8[2] = 11;  // servo motor pin number (first pin, full motor separately defined above)

  /* PWM Lighting */
  teensy[te].pinmode[14] = PINMODE_PWM; // Background Lighting
  teensy[te].pinmode[15]= PINMODE_PWM; // Park Brake Light
  
  /* Analog Inputs */
  //teensy[te].pinmode[16] = PINMODE_ANALOGINPUTMEDIAN; // Motor 1 Current Measurement A0
  //teensy[te].pinmode[17] = PINMODE_ANALOGINPUTMEDIAN; // Motor 2 Current Measurement A1
  //teensy[te].pinmode[20] = PINMODE_ANALOGINPUTMEDIAN; // Motor 3 Current Measurement A2
  //teensy[te].pinmode[21] = PINMODE_ANALOGINPUTMEDIAN; // Motor 4 Current Measurement A3

  /* I2C */
  teensy[te].pinmode[18] = PINMODE_I2C; 
  teensy[te].pinmode[19] = PINMODE_I2C; 

  /* Sound Board */
  teensy[te].pinmode[6] = PINMODE_OUTPUT;
  //teensy[te].pinmode[7] = PINMODE_OUTPUT; /* UNUSED */
  
  /* Analog Inputs */
  teensy[te].pinmode[22] = PINMODE_ANALOGINPUTMEDIAN; // REVERSER 0 A8
  teensy[te].pinmode[23] = PINMODE_ANALOGINPUTMEDIAN; // REVERSER 1 A9
  teensy[te].pinmode[24] = PINMODE_ANALOGINPUTMEDIAN; // FLAPS A10
  teensy[te].pinmode[25] = PINMODE_ANALOGINPUTMEDIAN; // SPEED BRAKE A11
  teensy[te].pinmode[26] = PINMODE_ANALOGINPUTMEDIAN; // THRUST 0 A12
  teensy[te].pinmode[27] = PINMODE_ANALOGINPUTMEDIAN; // THRUST 1 A13

  /* Buttons / Inputs */
  teensy[te].pinmode[28] = PINMODE_INPUT; // Stab Trim Main Elect
  teensy[te].pinmode[29] = PINMODE_INPUT; // Stab Trim Auto Pilot
  teensy[te].pinmode[30] = PINMODE_INPUT; // Park Brake
  teensy[te].pinmode[31] = PINMODE_INPUT; // Cutoff 0
  teensy[te].pinmode[32] = PINMODE_INPUT; // Cutoff 1
  teensy[te].pinmode[34] = PINMODE_INPUT; // TOGA 0 and 1 (parallel)
  teensy[te].pinmode[35] = PINMODE_INPUT; // AT DISCONNECT 0 and 1 (parallel)
  //teensy[te].pinmode[38] = PINMODE_INPUT; /* UNUSED */
  teensy[te].pinmode[39] = PINMODE_INPUT; // HORN CUTOUT BUTTON
  teensy[te].pinmode[40] = PINMODE_INPUT; // HORN CUTOUT ENCODER 1
  teensy[te].pinmode[41] = PINMODE_INPUT; // HORN CUTOUT ENCODER 2
 
  /* Servos */
  teensy[te].pinmode[33] = PINMODE_SERVO; // Park Brake Servo
  teensy[te].pinmode[36] = PINMODE_SERVO; // Stab Trim 0 Servo
  teensy[te].pinmode[37] = PINMODE_SERVO; // Stab Trim 1 Servo

  /* Stab Trim Wheel direction and rotation counter */
  as5048b[te][0].nangle = 50;
  as5048b[te][0].type = 0;
  as5048b[te][0].wire = 0;
  as5048b[te][0].address = 0x40;

  /* initialize auto/manual modes */
  stabilizer_mode = 0;
  parkbrake_mode = 0;
  speedbrake_mode = 0;
  
}

/* Main Loop Code for the Boeing 737 Throttle Quadrant Teensy Controller */
void b737_tq(void)
{

  int ret;
  int te = 0; /* Teensy Number in ini file */
  int i;
  float fvalue;
  int en;

  struct timeval new_time;
  
  float throttle0=FLT_MISS;
  float throttle1=FLT_MISS;
   
  float reverser0=FLT_MISS;
  float reverser1=FLT_MISS;

  float speedbrake=FLT_MISS;
  float minspeedbrake_xplane;
  float maxspeedbrake_xplane;
  float minspeedbrake_x737 = 0.0;
  float maxspeedbrake_x737 = 1.0;
  float minspeedbrake_zibo = 0.0;
  float maxspeedbrake_zibo = 1.0;
  float minspeedbrake_default = -0.5;
  float maxspeedbrake_default = 1.0;
  float difspeedbrake = 0.05; /* minimum difference between x-plane and H/W to toggle a change */

  float minstabilizer_xplane;
  float maxstabilizer_xplane;
  float minstabilizer_x737 = -0.280;
  float maxstabilizer_x737 =  1.000;
  float minstabilizer_zibo = -1.000;
  //  float maxstabilizer_zibo =  0.719;
  float maxstabilizer_zibo =  1.000;
  float minstabilizer_default = -1.000;
  float maxstabilizer_default =  1.000;
  
  int parkbrake;
  float difparkbrake = 0.05; /* minimum difference between x-plane and H/W to toggle a change */

  float stab_trim_main_elect = FLT_MISS;
  float stab_trim_auto_pilot = FLT_MISS;
  int cutoff0 = INT_MISS;
  int cutoff1 = INT_MISS;
  int horn_encoder;

  float flap=FLT_MISS;

  /* X-Plane Datarefs and Commandrefs used in this code */
  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");

  int *num_engines = link_dataref_int("sim/aircraft/engine/acf_num_engines");
  /* XP12 */
  float *throttle = link_dataref_flt_arr("sim/cockpit2/engine/actuators/throttle_jet_rev_ratio", 16, -1, -2);
  float *throttle_actuator = link_dataref_flt_arr("sim/cockpit2/engine/actuators/throttle_ratio",16, -1, -2);
  /* XP11 */
  //float *throttle = link_dataref_flt_arr("sim/cockpit2/engine/actuators/throttle_jet_rev_ratio", 8, -1, -2);
  //float *throttle_actuator = link_dataref_flt_arr("sim/cockpit2/engine/actuators/throttle_ratio",8, -1, -2);

  float *zibo_throttle0;
  float *zibo_throttle1;
  float *zibo_reverser0;
  float *zibo_reverser1;
  if ((acf_type == 2) || (acf_type == 3)) {
    zibo_throttle0 = link_dataref_flt("laminar/B738/engine/thrust1_leveler", -2);
    zibo_throttle1 = link_dataref_flt("laminar/B738/engine/thrust2_leveler", -2);
    zibo_reverser0 = link_dataref_flt("laminar/B738/flt_ctrls/reverse_lever1", -2);
    zibo_reverser1 = link_dataref_flt("laminar/B738/flt_ctrls/reverse_lever2", -2);
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

  int *trim_up_cmd = link_dataref_cmd_hold("sim/flight_controls/pitch_trim_up_mech");
  int *trim_down_cmd = link_dataref_cmd_hold("sim/flight_controls/pitch_trim_down_mech");
  int *trim_up_ap = link_dataref_int("sim/cockpit2/annunciators/autopilot_trim_up");
  int *trim_down_ap = link_dataref_int("sim/cockpit2/annunciators/autopilot_trim_down");
  float *stabilizer_xplane = link_dataref_flt("sim/cockpit2/controls/elevator_trim",-3);

  int *toga_button;
  if ((acf_type == 2) || (acf_type == 3)) {
    toga_button = link_dataref_cmd_hold("laminar/B738/autopilot/left_toga_press");
  } else if (acf_type == 1) {
    toga_button = link_dataref_int("x737/systems/athr/toggle_TOGA");
  } else {
    //    toga_button = link_dataref_cmd_once("sim/engines/TOGA_power");
    toga_button = link_dataref_cmd_hold("sim/autopilot/take_off_go_around");
  }
  
  int *at_disconnect_button;
  if ((acf_type == 2) || (acf_type == 3)) {
    at_disconnect_button = link_dataref_cmd_hold("laminar/B738/autopilot/left_at_dis_press");
  } else if (acf_type == 1) {
    at_disconnect_button = link_dataref_cmd_hold("x737/mcp/ATHR_ARM_OFF");
  } else {
    at_disconnect_button = link_dataref_cmd_hold("sim/autopilot/autothrottle_off");
  }
  
  int *autothrottle_on;
  float *autothrottle_on_f;
  //float *speed_mode;
  float *lock_throttle;
  if ((acf_type == 2) || (acf_type == 3)) {
    autothrottle_on_f = link_dataref_flt("laminar/B738/autopilot/autothrottle_status1",0);
    //speed_mode = link_dataref_flt("laminar/B738/autopilot/speed_mode",0);
    //lock_throttle = link_dataref_flt("laminar/B738/autopilot/lock_throttle",0); // XP11
    lock_throttle = link_dataref_flt("laminar/B738/autopilot/autothrottle_status",0); // XP12
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
    /* XP12 */
    fuel_mixture = link_dataref_flt_arr("sim/flightmodel/engine/ENGN_mixt",16,-1,-2);
    /* XP11 */
    //fuel_mixture = link_dataref_flt_arr("sim/flightmodel/engine/ENGN_mixt",8,-1,-2);
  }
     
  int *horn_cutout_button;
  if ((acf_type == 2) || (acf_type == 3)) { 
    horn_cutout_button = link_dataref_cmd_hold("laminar/B738/alert/gear_horn_cutout");
  } else if (acf_type == 1) {
    horn_cutout_button = link_dataref_cmd_hold("x737/TQ/HORN_CUTOUT");
  } else {
    horn_cutout_button = link_dataref_int("xpserver/HORN_CUTOUT");
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

 
  /* assign correct stabilizer range depending on aircraft */
  if ((acf_type == 2) || (acf_type == 3)) {
    minstabilizer_xplane = minstabilizer_zibo;
    maxstabilizer_xplane = maxstabilizer_zibo;
  } else if (acf_type == 1) {
    minstabilizer_xplane = minstabilizer_x737;
    maxstabilizer_xplane = maxstabilizer_x737;
  } else {
    minstabilizer_xplane = minstabilizer_default;
    maxstabilizer_xplane = maxstabilizer_default;
  }

  /* assign correct speedbrake settings depending on aircraft */
  if ((acf_type == 2) || (acf_type == 3)) {
    minspeedbrake_xplane = minspeedbrake_zibo;
    maxspeedbrake_xplane = maxspeedbrake_zibo;
  } else if (acf_type == 1) {
    minspeedbrake_xplane = minspeedbrake_x737;
    maxspeedbrake_xplane = maxspeedbrake_x737;
  } else {
    minspeedbrake_xplane = minspeedbrake_default;
    maxspeedbrake_xplane = maxspeedbrake_default;
  }

  /*** Background Lighting ***/
  float background_lighting = 0.0;
  if (*avionics_on == 1) background_lighting = 25.0; /* set 75% intensity for now */
  ret = pwm_output(te, TEENSY_TYPE, 0, 14, &background_lighting,0.0,100.0);

  /*** Read Buttons ***/
  
  /* read TOGA Button */
  ret = digital_input(te, TEENSY_TYPE, 0, 34, toga_button, 0);
  if ((ret == 1) && (*toga_button == 1)) {
    printf("TOGA Button pressed \n");
  }

  /* read AT DISCONNECT Button */
  ret = digital_input(te, TEENSY_TYPE, 0, 35, at_disconnect_button, 0);
  if ((ret == 1) && (*at_disconnect_button == 1)) {
    printf("AT DISCONNECT Button pressed \n");
  }

  /* read HORN CUTOUT Button */
  ret = digital_input(te, TEENSY_TYPE, 0, 39, horn_cutout_button, 0);
  if ((ret == 1) && (*horn_cutout_button == 1)) {
    printf("HORN CUTOUT Button pressed \n");
  }

  /* read HORN CUTOUT Encoder (not present in real aircraft) */
  ret = encoder_input(te, TEENSY_TYPE, 0, 40, 41, &horn_encoder, 1, 1);
  if (ret == 1) {
    printf("HORN CUTOUT Encoder changed to: %i \n",horn_encoder);
  }
  if (horn_encoder > 100) horn_encoder = 100;
  if (horn_encoder < 0) horn_encoder = 0;

  
  /*** THROTTLE AND REVERSER LEVERS ***/
  
  /* read reverser 0 lever position */
  ret = analog_input(te,22,&reverser0,0.0,1.0);
  //printf("%f \n",reverser0);
  if (ret == 1) {
    printf("Reverser 0 changed to: %f \n",reverser0);
  }

  /* read reverser 1 lever position */
  ret = analog_input(te,23,&reverser1,0.0,1.0);
  if (ret == 1) {
    printf("Reverser 1 changed to: %f \n",reverser1);
  }

  /* read throttle 0 lever position */
  ret = analog_input(te,26,&throttle0,0.0,1.0);
  if (ret == 1) {
    printf("Throttle 0 changed to: %f \n",throttle0);
  }

  /* read throttle 1 lever position */
  ret = analog_input(te,27,&throttle1,0.0,1.0);
  if (ret == 1) {
    printf("Throttle 1 changed to: %f \n",throttle1);
  }

  /* diagnose whether on Auto Throttle (AT) or Manual mode */
  int at = 0;
  if ((acf_type == 2) || (acf_type == 3)) {
    if (*autothrottle_on_f == 1.0) at = 1;
    //printf("%f %f \n",*autothrottle_on_f,*lock_throttle);
  } else {
    if (*autothrottle_on >= 1) at = 1;
  }
  
  if ((at == 1) && (*lock_throttle == 1.0)) {
    /* on autothrottle */
       
    /* Auto Throttle for engine 1 (first engine on left wing) */
    en = 0;
    if ((*(throttle_actuator+en) != FLT_MISS) && (throttle0 != FLT_MISS)) { 
      ret = program_closedloop(te, 0, 1, (throttle_actuator+en), 0.0, 1.0);
    } else {
      ret = program_closedloop(te, 0, 0, (throttle_actuator+en), 0.0, 1.0);
    }

    /* Auto Throttle for engine 2 (first engine on right wing)*/
    if (*num_engines != INT_MISS) {
      en = (*num_engines + 1) / 2;
    } else {
      en = 1;
    }
    if ((*(throttle_actuator+en) != FLT_MISS) && (throttle1 != FLT_MISS)) {
      ret = program_closedloop(te, 1, 1, (throttle_actuator+en), 0.0, 1.0);
    } else {
      ret = program_closedloop(te, 1, 0, (throttle_actuator+en), 0.0, 1.0);
    }

  } else {
    /* on manual throttle */
    fvalue = 0.0;
    en = 0;
    ret = program_closedloop(te, 0, 0, &fvalue, 0.0, 1.0);
    en = 1;
    ret = program_closedloop(te, 1, 0, &fvalue, 0.0, 1.0);

    if ((reverser0 != FLT_MISS) && (reverser1 != FLT_MISS) &&
	(throttle0 != FLT_MISS) && (throttle1 != FLT_MISS)) {
    
      if ((acf_type == 2) || (acf_type == 3)) {
	*zibo_reverser0 = min(max((reverser0 - REVERSER_MIN)/(REVERSER_MAX - REVERSER_MIN),0.0),1.0);
	*zibo_throttle0 = min(max((throttle0 - THROTTLE_MIN)/(THROTTLE_MAX - THROTTLE_MIN),0.0),1.0);
	*zibo_reverser1 = min(max((reverser1 - REVERSER_MIN)/(REVERSER_MAX - REVERSER_MIN),0.0),1.0);
	*zibo_throttle1 = min(max((throttle1 - THROTTLE_MIN)/(THROTTLE_MAX - THROTTLE_MIN),0.0),1.0);

      } else {
	if (*num_engines != INT_MISS) {
	  for (i=0;i<*num_engines;i++) {
	    if ((i<(*num_engines/2)) || (*num_engines == 1)) {
	      /* engines on left wing */
	      /* or single engine */
	      /* or first half minus one if uneven # of engines */
	      if ((throttle0 < 0.05) && (reverser0 > 0.05)) {
		*(throttle+i) = -min(max((reverser0 - REVERSER_MIN)/(REVERSER_MAX - REVERSER_MIN),0.0),1.0);
	      } else {
		*(throttle+i) = min(max((throttle0 - THROTTLE_MIN)/(THROTTLE_MAX - THROTTLE_MIN),0.0),1.0);
	      }
	    } else {
	      if ((throttle1 < 0.05) && (reverser1 > 0.05)) {
		*(throttle+i) = -min(max((reverser1 - REVERSER_MIN)/(REVERSER_MAX - REVERSER_MIN),0.0),1.0);
	      } else {
		*(throttle+i) = min(max((throttle1 - THROTTLE_MIN)/(THROTTLE_MAX - THROTTLE_MIN),0.0),1.0);
	      }
	    }
	  }
	}
      }

    }

  } // manual throttle

  
  /*** PARK BRAKE Switch and Servo ***/
  ret = digital_input(te, TEENSY_TYPE, 0, 30, &parkbrake, 0);
  if (ret == 1) {
    printf("Park Brake changed to: %i \n",parkbrake);
  }
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
    /* Reset to idle mode: H/W and X-Plane in same position */
    parkbrake_mode = 0;
  }

  if (parkbrake_mode == 2) {
    /* Auto Park Brake: Only works to release Park Brake */
    if ((*parkbrake_xplane != FLT_MISS) && (*parkbrake_xplane == 0)) {
      fvalue = 1.0;
      //printf("A: %f %f \n",*parkbrake_xplane, fvalue);
      ret = servo_output(te, TEENSY_TYPE, 0, 33, &fvalue, 0.0, 1.0, 0.51, 0.77);
    } else {
      /* Cannot Auto-Set Parkbrake, thus change X-Plane Value */
      parkbrake_mode = 1;
    }
  }
  if (parkbrake_mode == 1) {
    /* Manual Park Brake */
    /* Set Park Brake Servo to neutral position */
    fvalue = 0.0;
    ret = servo_output(te, TEENSY_TYPE, 0, 33, &fvalue, 0.0, 1.0, 0.51, 0.77);

    if (((acf_type == 2) || (acf_type == 3)) &&
	(parkbrake != INT_MISS) && (*parkbrake_xplane != FLT_MISS)) {
      fvalue = (float) parkbrake;
      ret = set_state_togglef(parkbrake_xplane,&fvalue,parkbrake_button);
      if (ret != 0) {
	printf("Park Brake Toggle: XP: %f HW: %i \n",*parkbrake_xplane,parkbrake);
      }

    } else {
      if (parkbrake != INT_MISS) *parkbrake_xplane = (float) parkbrake;
    }      
  }
  if (parkbrake_mode == 0) {
    /* Set Park Brake servo to neutral position */
    fvalue = 0.0;
    ret = servo_output(te, TEENSY_TYPE, 0, 33, &fvalue, 0.0, 1.0, 0.51, 0.77);
  }

  /* Park Brake Light */
  float parkbrake_light = 0.0;
  if ((*avionics_on == 1) && (*parkbrake_xplane == 1.0)) parkbrake_light = 75.0;
  ret = pwm_output(te, TEENSY_TYPE, 0, 15, &parkbrake_light,0.0,100.0);


  
  /*** SPEED BRAKE LEVER ***/

  /* read speed brake lever position */
  ret = analog_input(te,25,&speedbrake,0.0,1.0);
  if (ret == 1) {
    printf("Speed Brake changed to: %f \n",speedbrake);
  }

  int speedbrake_locked = 0;
  if (speedbrake < SPEEDBRAKE_LOCK) speedbrake_locked = 1;
 
   /* Normalize H/W lever potentiometer range */
  if (speedbrake != FLT_MISS) {
    speedbrake = min(max((speedbrake - SPEEDBRAKE_MIN)/(SPEEDBRAKE_MAX - SPEEDBRAKE_MIN),0.0),1.0);
  }
  /* Scale normalized speedbrake range to X-Plane range */
  if (speedbrake != FLT_MISS) {
    speedbrake = speedbrake * (maxspeedbrake_xplane - minspeedbrake_xplane) + minspeedbrake_xplane;
  }
  
  if ((speedbrake_mode == 0) && (speedbrake != FLT_MISS) && (*speedbrake_xplane != FLT_MISS) &&
      (fabs(speedbrake - *speedbrake_xplane) > difspeedbrake)) {
    if (ret == 1) {
      /* H/W has changed: Manual Mode */
      printf("Speedbrake operating in Manual Mode\n");
      speedbrake_mode = 1;
    } else {
      /* X-Plane has changed: AP Mode */
      printf("Speedbrake operating in Auto Mode\n");
      speedbrake_mode = 2;
    }
  }
  if ((speedbrake_mode != 0) && (speedbrake != FLT_MISS) && (*speedbrake_xplane != FLT_MISS) &&
      (fabs(speedbrake - *speedbrake_xplane) < difspeedbrake)) {
    /* Idle mode: H/W and X-Plane in same position */
    speedbrake_mode = 0;
  }

  if ((speedbrake_mode == 0) || (speedbrake_mode == 1)) {

    ret = program_closedloop(te, 2, 0, speedbrake_xplane, minspeedbrake_xplane, maxspeedbrake_xplane);

    if (speedbrake_mode == 1) {
      *speedbrake_xplane = speedbrake; 
    }
    
  } else if (speedbrake_mode == 2) {

    if (speedbrake_locked == 0) {

      ret = program_closedloop(te, 2, 1, speedbrake_xplane, minspeedbrake_xplane, maxspeedbrake_xplane);

    } else {
      printf("Speedbrake operating in Manual Mode\n");
      speedbrake_mode = 1;
    }  
    
  }

  /*** FLAPS LEVER ***/
  
  /* read flap lever position */
  ret = analog_input(te,24,&flap,0.0,1.0);
  if (ret == 1) {
    printf("Flap Lever changed to: %f \n",flap);
  }
  
  if (flap < 0.5*(0.043+0.179)) {
    *flap_ratio = 0.000;
  } else if (flap < 0.5*(0.179+0.313)) {
    *flap_ratio = 0.125;
  } else if (flap < 0.5*(0.313+0.441)) {
    *flap_ratio = 0.250;
  } else if (flap < 0.5*(0.441+0.540)) {
    *flap_ratio = 0.375;
  } else if (flap < 0.5*(0.540+0.650)) {
    *flap_ratio = 0.500;
  } else if (flap < 0.5*(0.659+0.750)) {
    *flap_ratio = 0.625;
  } else if (flap < 0.5*(0.750+0.880)) {
    *flap_ratio = 0.750;
  } else if (flap < 0.5*(0.880+0.950)) {
    *flap_ratio = 0.875;
  } else {
    *flap_ratio = 1.000;
  }

  /*** TRIM SWITCHES ***/

  /* read STAB TRIM MAIN ELECT Switch */
  ret = digital_inputf(te, TEENSY_TYPE, 0, 28, &stab_trim_main_elect, 0);
  if (ret == 1) {
    printf("Stab Trim Main Elect changed to: %f \n",stab_trim_main_elect);
  }

  /* read STAB TRIM AUTO PILOT Switch */
  ret = digital_inputf(te, TEENSY_TYPE, 0, 29, &stab_trim_auto_pilot, 0);
  if (ret == 1) {
    printf("Stab Trim Auto Pilot changed to: %f \n",stab_trim_auto_pilot);
  }

  if ((acf_type == 2) || (acf_type == 3)) {
    ret = set_state_togglef(&stab_trim_main_elect,main_elect_pos,main_elect_toggle);
    ret = set_state_togglef(&stab_trim_auto_pilot,autopilot_pos,autopilot_toggle);     
  }
  
  /*** CUTOFF LEVERS ***/

  /* read CUTOFF 0 lever switch */
  ret = digital_input(te, TEENSY_TYPE, 0, 31, &cutoff0, 0);
  if (ret == 1) {
    printf("Cutoff 0 changed to: %i \n",cutoff0);
  }

  /* read CUTOFF 1 lever switch */
  ret = digital_input(te, TEENSY_TYPE, 0, 32, &cutoff1, 0);
  if (ret == 1) {
    printf("Cutoff 1 changed to: %i \n",cutoff1);
  }
  
  if ((acf_type == 1) || (acf_type == 2) || (acf_type == 3)) {
    if ((*fuel_mixture_left != FLT_MISS) && (cutoff0 != INT_MISS)) {
      *fuel_mixture_left = (float) (1-cutoff0);
    }    
    if ((*fuel_mixture_right != FLT_MISS) && (cutoff1 != INT_MISS)) {
      *fuel_mixture_right = (float) (1-cutoff1);
    }    
  } else {
    if (*num_engines != INT_MISS) {
      for (i=0;i<*num_engines;i++) {
	if ((i<(*num_engines/2)) || (*num_engines == 1)) {
	  if ((*(fuel_mixture+i) != FLT_MISS) && (cutoff0 != INT_MISS)) {
	    *(fuel_mixture+i) = (float) (1-cutoff0);
	  }
	} else {
	  if ((*(fuel_mixture+i) != FLT_MISS) && (cutoff1 != INT_MISS)) {
	    *(fuel_mixture+i) = (float) (1-cutoff1);
	  }
	}
      }
    }
  }

  /*** Stabilizer Indicators ***/
  if (*stabilizer_xplane != FLT_MISS) {
    /* left */
    fvalue = (1.0 - (*stabilizer_xplane - minstabilizer_xplane)/(maxstabilizer_xplane - minstabilizer_xplane));
    //    printf("%f %f %f %f \n",*stabilizer_xplane,minstabilizer,maxstabilizer,value);
    ret = servo_output(te, TEENSY_TYPE, 0, 36, &fvalue, 0.0, 1.0, 0.115, 0.64);
   
    /* right */
    fvalue = ((*stabilizer_xplane) - minstabilizer_xplane)/(maxstabilizer_xplane - minstabilizer_xplane);
    //    printf("%f %f %f %f \n",*stabilizer_xplane,minstabilizer,maxstabilizer,value);
    ret = servo_output(te, TEENSY_TYPE, 0, 37, &fvalue, 0.0, 1.0, 0.28, 0.825);

  }

  /*** Stabilizer Trim Wheel ***/
  /*
  float trim_current;
  ret = analog_input(te,20,&trim_current,0.0,1023.0);
  if (ret == 1) {
    printf("Trim Wheel Current: %f \n",trim_current);
  }
  */
  
  /* Manual Trim Detector (AS5048B magnetic rotary sensor) */
  int angle;
  int trim_up_wheel = 0;
  int trim_down_wheel = 0;
  ret = angle_input(te, AS5048B_TYPE, 0, 0, &angle);
  if ((ret == 1) && (trim_up_ap_old == 0) && (trim_down_ap_old == 0)) {
    //printf("Angle changed to: %i \n",angle);
    if (angle == 1) {
      printf("UP\n");
      trim_up_wheel = 1;
    } else {
      printf("DOWN\n");
      trim_down_wheel = 1;
    }
  }

  /* Does our stabilizer move come from X-Plane or from our Hardware? */
  if (stabilizer_mode == 0) {
    if ((trim_up_wheel == 1) || (trim_down_wheel == 1)) stabilizer_mode = 1; // Hand drives
    if ((*trim_up_ap == 1) || (*trim_down_ap == 1)) stabilizer_mode = 2; // X-Plane drives

    if (stabilizer_mode == 1) printf("Start Manual Trim. Switched to Stabilizer Mode: %i \n",stabilizer_mode);
    if (stabilizer_mode == 2) printf("Start Auto Trim. Switched to Stabilizer Mode: %i \n",stabilizer_mode);

    trim_time.tv_sec = INT_MISS;
  }

  if (stabilizer_mode == 1) {
    if (trim_up_wheel == 1) {
      trim_time.tv_sec = INT_MISS;
      *trim_up_cmd = 1;
      *trim_down_cmd = 0;
    }
    if (trim_down_wheel == 1) {
      trim_time.tv_sec = INT_MISS;
      *trim_up_cmd = 0;
      *trim_down_cmd = 1;
    }

    if ((trim_up_wheel == 0) && (trim_down_wheel == 0)) {
      if (trim_time.tv_sec == INT_MISS) {
	gettimeofday(&trim_time,NULL);
      } else {
	gettimeofday(&new_time,NULL);
	
	/* time passed since commanded stop [ms] */
	float dt = ((float) (new_time.tv_sec - trim_time.tv_sec) +
		  ((float) (new_time.tv_usec - trim_time.tv_usec)) / 1000000.0)*1000.0;
	
	/* hardware trim wheel quiet after x ms no up/down commands */
	if (dt > 100.0) {
	  *trim_up_cmd = 0;
	  *trim_down_cmd = 0;
	}
	/* wait until x-plane stopped turning trim wheel */
	if (dt > 1000.0) {
	  trim_time.tv_sec = INT_MISS;
	  stabilizer_mode = 0;
	  printf("Finish Manual Trim. Switched to Stabilizer Mode: %i \n",stabilizer_mode);
	}
      }
    }
  }

  if (stabilizer_mode == 2) {
    
    /* AP-controlled Stab Trim: use Trim Wheel Motor */
    /* Also applied for trim switches on Yoke */
    
    float motor_speed = 0;
    int motor_stop = 0;

    /* start timer after AP trim command stopped */
    if ((((*trim_up_ap == 0) && (trim_up_ap_old == 1)) ||
	 ((*trim_down_ap == 0) && (trim_down_ap_old == 1)))
	&& (trim_time.tv_sec == INT_MISS)) {
      gettimeofday(&trim_time,NULL);  
    }

    /* re-press up or down AP switch during stop operation */
    if ((*trim_up_ap == 1) || (*trim_down_ap == 1)) trim_time.tv_sec = INT_MISS;

    /* check timer and stop reverse if needed */
    if (trim_time.tv_sec == INT_MISS) {
      /* regular operation */
      /* turn wheel in respective direction if AP commanded trim is running */
      if (*trim_up_ap == 1) motor_speed = -0.9;
      if (*trim_down_ap == 1) motor_speed = 0.9;
    } else {
      /* stop operation */
      /* turn wheel in counter direction */
      if (trim_up_ap_old == 1) motor_speed = 0.6;
      if (trim_down_ap_old == 1) motor_speed = -0.6;

      gettimeofday(&new_time,NULL);

      /* time passed since commanded stop [ms] */
      float dt = ((float) (new_time.tv_sec - trim_time.tv_sec) +
		  ((float) (new_time.tv_usec - trim_time.tv_usec)) / 1000000.0)*1000.0;

      /* stop reverse after x milliseconds */
      if (dt > 200.0) {
	motor_stop = 1;
	motor_speed = 0.0;
      }
      /* trim motor quiet, no more false up/down manual commands */
      if (dt > 1500.0) {
	trim_time.tv_sec = INT_MISS;
	stabilizer_mode = 0;
	printf("Finish Auto Trim. Switched to Stabilizer Mode: %i\n",stabilizer_mode);
      }
    }
  
    ret = motor_output(te, TEENSY_TYPE, 0, 8, &motor_speed,0.0,1.0,motor_stop);
    
    /* reset old states if needed (only after short reverse has finished */
    if (trim_time.tv_sec == INT_MISS) {
      trim_up_ap_old = *trim_up_ap;
      trim_down_ap_old = *trim_down_ap;
    }
  }
 
  /* Trim Wheel Sound Trigger */
  /* How to Convert Audio File: sox -v 3.0 PMDG_Trim.wav -r 22000 -b 16 T00HOLDL.WAV */
  int sound_trigger = ((*trim_up_ap == 1) || (*trim_down_ap == 1));
  ret = digital_output(te, TEENSY_TYPE, 0, 6, &sound_trigger);

}

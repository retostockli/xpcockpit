/* This is the b737_tq.c code which handles the CFY Throttle Quadrant rebuilt
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

/* allocation of global variables defined in b737_throttle.h */
int speedbrake_mode; /* 0: no change; 1: H/W controlling; 2: X-Plane controlling */
int parkbrake_mode; /* 0: no change; 1: H/W controlling; 2: X-Plane controlling */
int stabilizer_mode; /* 0: no change; 1: H/W controlling; 2: X-Plane controlling */
float stabilizer_old; /* save value for stabilizer to dampen small changes from pots */


void init_b737_tq(void)
{
  int te = 0;


  teensy[te].pinmode[0] = PINMODE_MOTOR; // motor EN
  teensy[te].arg1[0] = 1; // motor IN1 */
  teensy[te].arg2[0] = 2; // motor IN2 */
  teensy[te].arg3[0] = 14; // motor Current Sense */
  teensy[te].pinmode[3] = PINMODE_MOTOR; // motor EN
  teensy[te].arg1[3] = 4; // motor IN1 */
  teensy[te].arg2[3] = 5; // motor IN2 */
  teensy[te].arg3[3] = 15; // motor Current Sense */
  teensy[te].pinmode[6] = PINMODE_MOTOR; // motor EN
  teensy[te].arg1[6] = 7; // motor IN1 */
  teensy[te].arg2[6] = 8; // motor IN2 */
  teensy[te].arg3[6] = 16; // motor Current Sense */
  teensy[te].pinmode[9] = PINMODE_MOTOR; // motor EN
  teensy[te].arg1[9] = 10; // motor IN1 */
  teensy[te].arg2[9] = 11; // motor IN2 */
  teensy[te].arg3[9] = 17; // motor Current Sense */

  /* PWM Lighting */
  teensy[te].pinmode[12] = PINMODE_PWM; // Background Lighting
  teensy[te].pinmode[13]= PINMODE_PWM; // Park Brake Light
  
  /* Analog Inputs */
  //teensy[te].pinmode[14] = PINMODE_ANALOGINPUTMEDIAN; // Motor 1 Current Measurement A0
  //teensy[te].pinmode[15] = PINMODE_ANALOGINPUTMEDIAN; // Motor 2 Current Measurement A1
  //teensy[te].pinmode[16] = PINMODE_ANALOGINPUTMEDIAN; // Motor 3 Current Measurement A2
  //teensy[te].pinmode[17] = PINMODE_ANALOGINPUTMEDIAN; // Motor 4 Current Measurement A3

  /* I2C */
  teensy[te].pinmode[18] = PINMODE_I2C; 
  teensy[te].pinmode[19] = PINMODE_I2C; 

  /* Sound Board */
  teensy[te].pinmode[20] = PINMODE_OUTPUT;
  
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
  teensy[te].pinmode[39] = PINMODE_INPUT; // HORN CUTOUT BUTTON
  teensy[te].pinmode[40] = PINMODE_INPUT; // HORN CUTOUT ENCODER 1
  teensy[te].pinmode[41] = PINMODE_INPUT; // HORN CUTOUT ENCODER 2
 
  /* Servos */
  teensy[te].pinmode[33] = PINMODE_SERVO; // Park Brake Servo
  teensy[te].pinmode[36] = PINMODE_SERVO; // Stab Trim 0 Servo
  teensy[te].pinmode[37] = PINMODE_SERVO; // Stab Trim 1 Servo

  /* as5048b[te][0].nangle = 10; */
  /* as5048b[te][0].type = 0; */
  /* as5048b[te][0].wire = 0; */
  /* as5048b[te][0].address = 0x40; */

  /* This program simulates a servo by using a closed loop code with a motor and a potentiometer
     running inside the teensy */
  program[te][0].type = PROGRAM_CLOSEDLOOP;
  program[te][0].val16[1] = 5; // minimum servo potentiometer value
  program[te][0].val16[2] = 990; // maximum servo potentiometer value
  program[te][0].val8[1] = 26;  // servo potentiometer pin number (needs to be defined separately above)
  program[te][0].val8[2] = 0;  // servo motor pin number (first pin, full motor separately defined above)
  
  program[te][1].type = PROGRAM_CLOSEDLOOP;
  program[te][1].val16[1] = 5; // minimum servo potentiometer value
  program[te][1].val16[2] = 990; // maximum servo potentiometer value
  program[te][1].val8[1] = 27;  // servo potentiometer pin number (needs to be defined separately above)
  program[te][1].val8[2] = 3;  // servo motor pin number (first pin, full motor separately defined above)
  
}

void b737_tq(void)
{

  int ret;
  int te = 0;

 
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

  int stab_trim_main_elect;
  int stab_trim_auto_pilot;
  int cutoff0;
  int cutoff1;
  int toga;
  int at_disconnect;
  int horn_cutout;
  int horn_encoder;

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
  float *autothrottle_on_f;
  //float *speed_mode;
  float *lock_throttle;
  if ((acf_type == 2) || (acf_type == 3)) {
    autothrottle_on_f = link_dataref_flt("laminar/B738/autopilot/autothrottle_status1",0);
    //speed_mode = link_dataref_flt("laminar/B738/autopilot/speed_mode",0);
    //lock_throttle = link_dataref_flt("laminar/B738/autopilot/lock_throttle",0);
    lock_throttle = link_dataref_flt("laminar/B738/autopilot/autothrottle_status",0);
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
   
  /* read reverser 0 lever position */
  ret = analog_input(te,22,&reverser0,0.0,1.0);
  if (ret == 1) {
    printf("Reverser 0 changed to: %f \n",reverser0);
  }

  /* read reverser 1 lever position */
  ret = analog_input(te,23,&reverser1,0.0,1.0);
  if (ret == 1) {
    printf("Reverser 1 changed to: %f \n",reverser1);
  }
  
  /* read flap lever position */
  ret = analog_input(te,24,&flap,0.0,1.0);
  if (ret == 1) {
    printf("Flap Lever changed to: %f \n",flap);
  }
  
  /* read speed brake lever position */
  ret = analog_input(te,25,&speedbrake,0.0,1.0);
  if (ret == 1) {
    printf("Speed Brake changed to: %f \n",speedbrake);
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

  /* read STAB TRIM MAIN ELECT Switch */
  ret = digital_input(te, TEENSY_TYPE, 0, 28, &stab_trim_main_elect, 0);
  if (ret == 1) {
    printf("Stab Trim Main Elect changed to: %i \n",stab_trim_main_elect);
  }

  /* read STAB TRIM AUTO PILOT Switch */
  ret = digital_input(te, TEENSY_TYPE, 0, 29, &stab_trim_auto_pilot, 0);
  if (ret == 1) {
    printf("Stab Trim Auto Pilot changed to: %i \n",stab_trim_auto_pilot);
  }

  /* read PARK BRAKE Switch */
  ret = digital_input(te, TEENSY_TYPE, 0, 30, &parkbrake, 0);
  if (ret == 1) {
    printf("Park Brake changed to: %i \n",parkbrake);
  }

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

  /* read TOGA Button */
  ret = digital_input(te, TEENSY_TYPE, 0, 34, &toga, 0);
  if (ret == 1) {
    printf("TOGA Button changed to: %i \n",toga);
  }

  /* read AT DISCONNECT Button */
  ret = digital_input(te, TEENSY_TYPE, 0, 35, &at_disconnect, 0);
  if (ret == 1) {
    printf("AT DISCONNECT Button changed to: %i \n",at_disconnect);
  }

  /* read HORN CUTOUT Button */
  ret = digital_input(te, TEENSY_TYPE, 0, 39, &horn_cutout, 0);
  if (ret == 1) {
    printf("HORN CUTOUT Button changed to: %i \n",horn_cutout);
  }

  /* read HORN CUTOUT Encoder (not present in real aircraft) */
  ret = encoder_input(te, TEENSY_TYPE, 0, 40, 41, &horn_encoder, 1, 1);
  if (ret == 1) {
    printf("HORN CUTOUT Encoder changed to: %i \n",horn_encoder);
  }
  if (horn_encoder > 100) horn_encoder = 100;
  if (horn_encoder < 0) horn_encoder = 0;

  /* steer PARK BRAKE Servo */
  //ret = servo_output(te, TEENSY_TYPE, 0, 33, &throttle0,-0.3,1.3);
  /* steer STAB TRIM Servo 0 */
  //ret = servo_output(te, TEENSY_TYPE, 0, 36, &throttle0,-0.3,1.3);
  /* steer STAB TRIM Servo 1 */
  //ret = servo_output(te, TEENSY_TYPE, 0, 37, &throttle0,-0.3,1.3);


  // closed loop motor operation test
  float fencodervalue = (float) horn_encoder;
  ret = program_closedloop(te, 0, stab_trim_main_elect, &fencodervalue, 0.0, 100.0);
  ret = program_closedloop(te, 1, stab_trim_main_elect, &fencodervalue, 0.0, 100.0);

}

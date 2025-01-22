/* This is the test.c code which can be used to test basic I/O of the Teensy

   Copyright (C) 2024 Reto Stockli

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
#include "test.h"

int digitalvalue;
float analogvalue;

int brake;
int direction;

void init_test(void)
{
  int te = 0;

  teensy[te].pinmode[37] = PINMODE_PWM;
  //  teensy[te].pinmode[0] = PINMODE_OUTPUT;
  teensy[te].pinmode[3] = PINMODE_INPUT;
  teensy[te].pinmode[4] = PINMODE_INPUT;
  //  teensy[te].pinmode[5] = PINMODE_INPUT;

  //  teensy[te].pinmode[6] = PINMODE_INTERRUPT;
  //  teensy[te].pinmode[7] = PINMODE_INTERRUPT;
  teensy[te].pinmode[14] = PINMODE_ANALOGINPUTMEAN;
  teensy[te].pinmode[15] = PINMODE_ANALOGINPUTMEAN;
  teensy[te].pinmode[16] = PINMODE_I2C;
  teensy[te].pinmode[17] = PINMODE_I2C;
  teensy[te].pinmode[23] = PINMODE_OUTPUT;
  teensy[te].pinmode[30] = PINMODE_INPUT; // direction
  teensy[te].pinmode[31] = PINMODE_INPUT; // brake
  teensy[te].pinmode[33] = PINMODE_MOTOR; // motor EN
  teensy[te].arg1[33] = 34; // motor IN1
  teensy[te].arg2[33] = 35; // motor IN1
  teensy[te].arg3[33] = 38; // motor Current Sense
  
  teensy[te].pinmode[24] = PINMODE_MOTOR; // motor EN
  teensy[te].arg1[24] = 26; // motor IN1
  teensy[te].arg2[24] = 25; // motor IN1
  //teensy[te].arg3[24] = 38; // motor Current Sense

  //teensy[te].pinmode[38] = PINMODE_ANALOGINPUTMEAN;
  
   

  
  /* mcp23017[te][0].pinmode[2] = PINMODE_INPUT; */
  /* mcp23017[te][0].pinmode[3] = PINMODE_INPUT; */
  /* mcp23017[te][0].pinmode[5] = PINMODE_OUTPUT; */
  /* mcp23017[te][0].pinmode[7] = PINMODE_INPUT; */
  /* mcp23017[te][0].pinmode[8] = PINMODE_OUTPUT; */
  /* mcp23017[te][0].pinmode[15] = PINMODE_INPUT; */
  /* mcp23017[te][0].intpin = 6; // also define pin 6 of teensy as INTERRUPT above! */
  /* mcp23017[te][0].wire = 0;  // I2C Bus: 0, 1 or 2 */
  /* mcp23017[te][0].address = 0x21; // I2C address of MCP23017 device */
  

  /* mcp23017[te][1].pinmode[2] = PINMODE_INPUT; */
  /* mcp23017[te][1].pinmode[3] = PINMODE_INPUT; */
  /* mcp23017[te][1].pinmode[4] = PINMODE_INPUT; */
  /* mcp23017[te][1].pinmode[10] = PINMODE_INPUT; */
  /* mcp23017[te][1].pinmode[11] = PINMODE_INPUT; */
  /* mcp23017[te][1].pinmode[12] = PINMODE_INPUT; */
  /* mcp23017[te][1].pinmode[5] = PINMODE_OUTPUT; */
  /* mcp23017[te][1].pinmode[7] = PINMODE_OUTPUT; */
  /* mcp23017[te][1].intpin = 7; // also define pin 6 of teensy as INTERRUPT above! */
  /* mcp23017[te][1].wire = 0;  // I2C Bus: 0, 1 or 2 */
  /* mcp23017[te][1].address = 0x20; // I2C address of MCP23017 device */

  /*
  pca9685[te][0].pinmode[0] = PINMODE_SERVO;
  pca9685[te][0].pinmode[2] = PINMODE_PWM;
  pca9685[te][0].wire = 0;
  pca9685[te][0].address = 0x40;
  */

  as5048b[te][0].nangle = 10;
  as5048b[te][0].type = 0;
  as5048b[te][0].wire = 0;
  as5048b[te][0].address = 0x40;

  /* This program simulates a servo by using a closed loop code with a motor and a potentiometer
     running inside the teensy */
  program[te][0].type = PROGRAM_CLOSEDLOOP;
  program[te][0].val16[1] = 5; // minimum servo potentiometer value
  program[te][0].val16[2] = 990; // maximum servo potentiometer value
  program[te][0].val8[1] = 14;  // servo potentiometer pin number (needs to be defined separately above)
  program[te][0].val8[2] = 33;  // servo motor pin number (first pin, full motor separately defined above)
  
  program[te][1].type = PROGRAM_CLOSEDLOOP;
  program[te][1].val16[1] = 5; // minimum servo potentiometer value
  program[te][1].val16[2] = 990; // maximum servo potentiometer value
  program[te][1].val8[1] = 15;  // servo potentiometer pin number (needs to be defined separately above)
  program[te][1].val8[2] = 24;  // servo motor pin number (first pin, full motor separately defined above)
  
}

void test(void)
{

  int ret;
  int te = 0;

  /* link integer data like a switch in the cockpit */
  //int *value = link_dataref_int("sim/cockpit/electrical/landing_lights_on");
  int *value = link_dataref_int("xpserver/digitalvalue");
  
  /* link floating point dataref with precision 10e-1 to local variable. This means
     that we only transfer the variable if changed by 0.1 or more */
  //  float *fvalue = link_dataref_flt("sim/flightmodel/controls/parkbrake",-3);
  float *fvalue = link_dataref_flt("xpserver/analogvalue",-3);

  /* link NAV1 Frequency to encoder value */
  //  int *encodervalue = link_dataref_int("sim/cockpit/radios/nav1_freq_hz");
  int *encodervalue = link_dataref_int("xpserver/encoder");

  /* not needed, only if you run without x-plane connection */
  if (*encodervalue == INT_MISS) *encodervalue = 0;
  //if (*fvalue == FLT_MISS) *fvalue = 0.0;
  if (*value == INT_MISS) *value = 1;

  //float *wind_speed = link_dataref_flt("sim/cockpit2/gauges/indicators/wind_speed_kts",0);
  //float *time = link_dataref_flt("sim/time/framerate_period",-3);

  /* read encoder at inputs 3 and 4 */
  ret = encoder_input(te, TEENSY_TYPE, 0, 3, 4, encodervalue, 1, 1);
  //ret = encoder_input(te, MCP23017_TYPE, 0, 2, 3, encodervalue, 1, 1);
  if (ret == 1) {
    printf("Encoder changed to: %i \n",*encodervalue);
  }
 
  
  /* read digital input (#3) */  
  //ret = digital_input(te, MCP23017_TYPE, 0, 7, &digitalvalue, 0);
  //ret = digital_input(te, TEENSY_TYPE, 0, 3, &digitalvalue, 0);
  //if (ret == 1) {
  //  printf("Digital Input changed to: %i \n",digitalvalue);
  //}


  ret = digital_input(te, TEENSY_TYPE, 0, 30, &direction, 0);
  if (ret == 1) {
    printf("Motor Direction changed to: %i \n",direction);
  }

  /*
  ret = digital_input(te, TEENSY_TYPE, 0, 31, &brake, 0);
  if (ret == 1) {
    printf("Motor Brake changed to: %i \n",brake);
  }
  */

  /* read analog input (#14) */
  ret = analog_input(te,15,fvalue,0.0,1023.0);
  if (ret == 1) {
    printf("Analog Input changed to: %f \n",*fvalue);
  }

  /* change Motor according to speed, direction and brake */
  /*
  float speed = FLT_MISS;
  if (*fvalue != FLT_MISS) {
    if (direction == 1) {
      speed = *fvalue;
    } else {
      speed = -*fvalue;
    }
  }
  ret = motor_output(te, TEENSY_TYPE, 0, 33, &speed,0.0,1.0,brake);
  */
  
  /* read analog input for motor current sense (#38) */
  /*
  ret = analog_input(te,38,&analogvalue,0.0,1.0);
  if (ret == 1) {
    printf("Motor Current changed to: %f \n",analogvalue*3.3/0.39);
  }
  */

  int angle;
  ret = angle_input(te, AS5048B_TYPE, 0, 0, &angle);
  if (ret == 1) {
    //printf("Angle changed to: %i \n",angle);
    if (angle == 1) {
      printf("UP\n");
    } else {
      printf("DOWN\n");
    }
  }

  // closed loop motor operation test
  float fencodervalue = 0.0;
  if (*encodervalue != FLT_MISS) fencodervalue = (float) *encodervalue;
  ret = program_closedloop(te, 0, direction, &fencodervalue, 0.0, 100.0);
  ret = program_closedloop(te, 1, direction, &fencodervalue, 0.0, 100.0);
  
  /* set LED connected to first output (#0) to value landing lights dataref */
  //digitalvalue = 1;
  //analogvalue = 0.5;
  //ret = digital_output(te, TEENSY_TYPE, 0, 23, &direction);
  //ret = digital_output(te, MCP23017_TYPE, 0, 8, &digitalvalue);
  ret = pwm_output(te, TEENSY_TYPE, 0, 37, fvalue,5.0,1023.0);

  /* change Servo according to rotary position */
  //ret = servo_output(te, TEENSY_TYPE, 0, 23, fvalue,0.0,1.0);
  //ret = servo_output(te, PCA9685_TYPE, 0, 0, fvalue,0.0,1.0);

  //ret = pwm_output(te, PCA9685_TYPE, 0, 2, fvalue,0.0,1.0);
  
}

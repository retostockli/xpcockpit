/* This is the dcmotorsplus_test.c code which contains a sample set-up for how to communicate with the 
   DCMotors Plus card from OpenCockpits

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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "libiocards.h"
#include "serverdata.h"
#include "dcmotorsplus_test.h"

void dcmotorsplus_test(void)
{

  int ret = 0;
  int device_dcmotor = 2;
  int device_bu0836 = 3; 
  int axis = 4;
  int input = 0;
  int servo = 2;
  int card = 0;
  int motor = 0;
  int output = 0;
  float motorvalue;
  float minval = 0.0;
  float maxval = 1.0;

  float speedbrake;
  ret = axis_input(device_bu0836,axis,&speedbrake,minval,maxval);    
  if (ret == 1) {
    printf("Speedbrake, Analog Input %i has value: %f \n",axis,speedbrake);
  }

  int parkbrake;
  ret = digital_input(device_bu0836,0,input,&parkbrake,0);
  if (ret == 1) {
    printf("Park Brake: %i \n",parkbrake);
  }

  /* activate/deactivate stab trim motor on B737 Throttle Quadrant */
  ret = digital_output(device_dcmotor,card,output,&parkbrake); 
  ret = digital_output(device_dcmotor,card,output+1,&parkbrake); 
  ret = digital_output(device_dcmotor,card,output+2,&parkbrake); 

  /* drive motor with speedbrake value */
  motorvalue = speedbrake - 0.35;
  ret = motors_output(device_dcmotor,motor,&motorvalue,0.35); /* move motor for stabilizer trim */
  ret = motors_output(device_dcmotor,motor+1,&motorvalue,0.35); /* move motor for stabilizer trim */
  ret = motors_output(device_dcmotor,motor+2,&motorvalue,0.35); /* move motor for stabilizer trim */
  
  /* read potentiometer from analog input #1 on DCmotors Plus card, scale it to the range 0-360 degrees */
  //ret = axis_input(device,axis,&value,minval,maxval);
  //if (ret == 1) {
  //  printf("Analog Input %i has value: %f \n",axis,value);
    /* steer servo according with potentiometer value */
    //    ret = servos_output(device,servo,&value,minval,maxval,0,1023);
  //}

  //  ret = servos_output(device,servo,&value,minval,maxval);
 
  /* activate motor 0 (only needed for Cockpitforyou Boeing 737 throttle quadrant) */
  //  ret = digital_output(device,motor,output,&one);
  //  ret = motors_output(device,motor,&motorvalue,100.0);
 


}

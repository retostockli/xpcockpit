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
  int device = 6;
  int servo = 2;
  int axis = 1;
  int motor = 0;
  int output = 0;
  float value = 700.0;
  float motorvalue = 70.0;
  float minval = 200.0;
  float maxval = 1023.0;
  int one = 1;

  /* read potentiometer from analog input #1 on DCmotors Plus card, scale it to the range 0-360 degrees */
  ret = axis_input(device,axis,&value,minval,maxval);
  if (ret == 1) {
    printf("Analog Input %i has value: %f \n",axis,value);
    /* steer servo according with potentiometer value */
    //    ret = servos_output(device,servo,&value,minval,maxval,0,1023);
  }

  //  ret = servos_output(device,servo,&value,minval,maxval);
 
  /* activate motor 0 (only needed for Cockpitforyou Boeing 737 throttle quadrant) */
  //  ret = digital_output(device,motor,output,&one);
  //  ret = motors_output(device,motor,&motorvalue,100.0);
 


}

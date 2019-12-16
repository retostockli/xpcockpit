/* This is the bu0836_test.c code which contains a sample set-up for how to communicate with the 
   BU0836X or BU0836A card from Leo Bodnar

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
#include "bu0836_test.h"

void bu0836_test(void)
{

  int ret = 0;
  int device = 5;
  int card = 0;
  int axis    ;
  int input    ;
  float minval = 0.0;
  float maxval = 100.0;

  int test;
  float value;

  /* read potentiometer from analog input #1 on BU0836X/A card, scale it to the range 0-100 */
  for (axis=0; axis<7; axis++) {
    //    axis = 0;
    ret = axis_input(device,axis,&value,minval,maxval);
    if (ret == 1) {
      //      printf("Analog Input %i has value: %f \n",axis,value);
    }
  }

  /* read buttons of BU0836X/A card */
  for (input=0; input<15; input++) {
    ret = digital_input(device,card,input,&test,0);
    if (ret == 1) {
      printf("Button %i has value %i \n",input,test);
    }
  }

}

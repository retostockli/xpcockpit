/* This is the test.c code which can be used to test basic I/O of the SISMO Master and Daughter
   cards

   Copyright (C) 2021 Reto Stockli

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
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>

#include "common.h"
#include "libsismo.h"
#include "serverdata.h"

int value;

void test(void)
{

  int ret;
  int card = 0;
  int input = 1;
  int output = 1;
  int displayvalue = 4;

  if ((value != 0) && (value != 1)) value = 0;

  /* read second input (1) */
  ret = digital_input(card, input, &value, 0);
  if (ret == 1) {
    /* ret is 1 only if input has changed */
    printf("Input %i changed to: %i \n",input,value);
  }

  /* set LED connected to second output (1) to value of above input */
  ret = digital_output(card, output, &value);

  /* set 7 segment displays 0-5 to the 5 digit value with a decimal point at digit 2 */
  ret = display_output(card, 0, 5, &displayvalue, 2, 0);

}

/* This is the usbkeys_test.c code which contains a sample set-up for how to communicate with the 
   usbkeys card from OpenCockpits

   Copyright (C) 2009 - 2013  Reto Stockli

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
#include "usbkeys_test.h"

void usbkeys_test(void)
{

  int ret = 0;
  int device = 2;
  int key;
  
  for (key=0;key<iocard[device].ninputs;key++) {
    ret = keys_input(device,key);
    if (ret == 1) {
      printf("Key %i pressed! \n",key);
    }
  }



}

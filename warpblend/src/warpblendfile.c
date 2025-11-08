/* This is the warpblendfile.c code which reads the warp and blend grids which were
   created by the python script buildwarpblend.py

   Copyright (C) 2024-2025 Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

#include "warpdata.h"
#include "warpblendfile.h"

int read_warpblendfile(const char warpfile[]) {

  /* NVIDIA Grids run from top to bottom and from left to right */
  
  FILE *fptr;
  char buf[256];
  int gx;
  int gy;
  int gyinv;
  int x;
  int y;
  float val;
  int ret;
  
  fptr = fopen(warpfile,"r");
  if (fptr == NULL) {
    printf("Error: Warpfile not found: %s \n",warpfile);
    return -1;
  }

  ret = fscanf(fptr, "%i", &nx);  // Display horizontal # pixels
  ret = fscanf(fptr, "%i", &ny);  // Display vertical # pixels
  ret = fscanf(fptr, "%i", &ngx); // horizontal # gridpoints
  ret = fscanf(fptr, "%i", &ngy); // vertical # gridpoints
  ret = fscanf(fptr, "%s", buf); // Text showing start of warp grid

  printf("Horizontal Screen Resolution: %i\n",nx);
  printf("Vertical Screen Resolution:   %i\n",ny);
  
  printf("Number of Warp Grid Columns:  %i \n",ngx);
  printf("Number of Warp Grid Rows:     %i \n",ngy);

  // read warp grid
  for (gy=0;gy<ngy;gy++) {
    gyinv = ngy - 1 - gy;
    for (gx=0;gx<ngx;gx++) {
      ret = fscanf(fptr, "%f", &val);
      vx[gyinv][gx][1] = (float) gx / (float) (ngx-1);
      vx[gyinv][gx][0] = val;
    }
  }
  for (gy=0;gy<ngy;gy++) {
    gyinv = ngy - 1 - gy;
    for (gx=0;gx<ngx;gx++) {
      ret = fscanf(fptr, "%f", &val);
      vy[gyinv][gx][1] = (float) gyinv / (float) (ngy-1);
      vy[gyinv][gx][0] = 1.0-val;
    }
  }
  
  // read blend grid
  ret = fscanf(fptr, "%s", buf); // Text showing start of blend grid

  blendData = (float*) malloc (nx * ny * sizeof(float));

  for (y=0;y<ny;y++) {
    for (x=0;x<nx;x++) {
      ret = fscanf(fptr, "%f", &val);
      blendData[x+nx*y] = val;
    }
  }
  
  fclose(fptr);

  return 0;
}

char** str_split(char* a_str, const char a_delim) {
  char** result    = 0;
  size_t count     = 0;
  char* tmp        = a_str;
  char* last_comma = 0;
  char delim[2];
  delim[0] = a_delim;
  delim[1] = 0;

  /* Count how many elements will be extracted. */
  while (*tmp)
    {
      if (a_delim == *tmp)
        {
	  count++;
	  last_comma = tmp;
        }
      tmp++;
    }

  /* Add space for trailing token. */
  count += last_comma < (a_str + strlen(a_str) - 1);

  /* Add space for terminating null string so caller
     knows where the list of returned strings ends. */
  count++;

  result = malloc(sizeof(char*) * count);

  if (result)
    {
      size_t idx  = 0;
      char* token = strtok(a_str, delim);

      while (token)
        {
	  assert(idx < count);
	  *(result + idx++) = strdup(token);
	  token = strtok(0, delim);
        }
      assert(idx == count - 1);
      *(result + idx) = 0;
    }

  return result;
}

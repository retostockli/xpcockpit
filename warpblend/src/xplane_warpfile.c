/* This is the xplane_warpfile.c code which reads X-Plane preferences that store the 
   Warp & Blend information for X-Plane Windows. More specifically the 
   'X-Plane Window Positions.prf' file located in X-Plane/Output/preferences

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
#include "xplane_warpfile.h"

int read_warpblendfile(const char warpfile[]) {

  FILE *fptr;
  char buf[256];
  int gx;
  int gy;
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
    for (gx=0;gx<ngx;gx++) {
      ret = fscanf(fptr, "%f", &val);
      vx[gy][gx][1] = (float) gx / (float) (ngx-1);
      vx[gy][gx][0] = val;
    }
  }
  for (gy=0;gy<ngy;gy++) {
    for (gx=0;gx<ngx;gx++) {
      ret = fscanf(fptr, "%f", &val);
      vy[gy][gx][1] = (float) gx / (float) (ngx-1);
      vy[gy][gx][0] = val;
    }
  }

  has_warp = True;
  
  // read blend grid
  ret = fscanf(fptr, "%s", buf); // Text showing start of blend grid

  blendData = (float*) malloc (nx * ny * sizeof(float));

  for (y=0;y<ny;y++) {
    for (x=0;x<nx;x++) {
      ret = fscanf(fptr, "%f", &val);
      blendData[x+nx*y] = val;
    }
  }

  has_blend = True;
  
  fclose(fptr);

  return 0;
}

int read_warpfile(const char warpfile[],const char smonitor[]) {

  bool is_xp12 = False;
  
  int dragx=0;
  int dragy=0;
  int stepx=0;
  int stepy=0;
  FILE *fptr;
  char buf[256];

  bool isX;
  bool isY;
  bool isT;
  bool isB;
  bool isA;
  
  int col;
  int row;
  float val;

  int gx;
  int gy;
  int i;

  int side;
  int dist;

  alpha[0][0] = 0.0;
  alpha[0][1] = 0.0;
  alpha[0][2] = 0.0;
  alpha[0][3] = 0.0;
  alpha[1][0] = 0.0;
  alpha[1][1] = 0.0;
  alpha[1][2] = 0.0;
  alpha[1][3] = 0.0;
 
  has_warp = True;
  has_blend = False;

  ngy = 0;
  ngx = 0;
  
  fptr = fopen(warpfile,"r");
  if (fptr == NULL) {
    printf("Error: Warpfile not found: %s \n",warpfile);
    return -1;
  }

  /***********************************************************************************************/
  /***** PLEASE MAKE NX/NY DYNAMIC AS X-PLANE PREFS MAY NOT BE THE ACTUAL MONITOR RESOLUTION *****/
  /***********************************************************************************************/
  
  gx = 0;
  gy = 0;
  while (fgets(buf,sizeof(buf),fptr)!=NULL) {
    buf[strlen(buf)-1] = 0;
    char **tokens;
    char *str;

    isX = False;
    isY = False;
    isT = False;
    isB = False;
    isA = False;
      
    tokens = str_split(buf, '/');
      
    if (tokens) {
      for (i = 0; *(tokens + i); i++) {
	  
	if ((i == 1) && (strcmp(*(tokens + i),smonitor)!=0)) break;
	if (i == 2) {
	  if (strstr(*(tokens + i),"m_x_res_full")!=0) {
	    str = strtok(*(tokens + i)," ");
	    str = strtok(NULL," ");
	    nx = atoi(str);
	    printf("X-Res: %i \n",nx);
	  } 
	  if (strstr(*(tokens + i),"m_y_res_full")!=0) {
	    str = strtok(*(tokens + i)," ");
	    str = strtok(NULL," ");
	    ny = atoi(str);
	    printf("Y-Res: %i \n",ny);
	  } 
	} else if (i == 3) {
	  if (strcmp(*(tokens + i),"proj_sphere 0")==0) is_xp12 = True;
	  if (is_xp12) {
	    if (strstr(*(tokens + i),"grid_ini_x")!=0) {
	      isX = True;
	      col = atoi(strtok(*(tokens + i),"grid_ini_x"));
	    }
	    if (strstr(*(tokens + i),"grid_ini_y")!=0) {
	      isY = True;
	      col = atoi(strtok(*(tokens + i),"grid_ini_y"));
	    }
	    if (strstr(*(tokens + i),"grid_drag_dim_i")) {
	      str = strtok(*(tokens + i)," ");
	      str = strtok(NULL," ");
	      dragx = atoi(str);
	    }
	    if (strstr(*(tokens + i),"grid_drag_dim_j")) {
	      str = strtok(*(tokens + i)," ");
	      str = strtok(NULL," ");
	      dragy = atoi(str);
	    }
	    if (strstr(*(tokens + i),"grid_step_dim_i")) {
	      str = strtok(*(tokens + i)," ");
	      str = strtok(NULL," ");
	      stepx = atoi(str);
	    }
	    if (strstr(*(tokens + i),"grid_step_dim_j")) {
	      str = strtok(*(tokens + i)," ");
	      str = strtok(NULL," ");
	      stepy = atoi(str);
	    }

	    /* We do not read full blending grid since it only contains blending information
	       on the low res grid also used for warping. For Blending it is better to know the
	       exact pixel positions */
	    /* PLEASE DEVELOP FURTHER */
	    if (strstr(*(tokens + i),"grid_mul_r0")) {
	      str = strtok(*(tokens + i)," ");
	      str = strtok(NULL," ");
	      has_blend = True;
	    }
	    
	    /* These Blend values are stored temporary at unused parameter positions */
	    /* This way we do not have to read the pre-warped blending grid for NVIDIA blending */
	    if (strstr(*(tokens + i),"edge_blend_fade_lft")) {
	      str = strtok(*(tokens + i)," ");
	      str = strtok(NULL," ");
	      top[0] = atof(str)*nx;
	    }
	    if (strstr(*(tokens + i),"edge_blend_fade_rgt")) {
	      str = strtok(*(tokens + i)," ");
	      str = strtok(NULL," ");
	      top[1] = atof(str)*nx;
	    }
	    if (strstr(*(tokens + i),"edge_blend_fade_bot")) {
	      str = strtok(*(tokens + i)," ");
	      str = strtok(NULL," ");
	      bot[0] = atof(str)*nx;
	    }
	    if (strstr(*(tokens + i),"edge_blend_fade_top")) {
	      str = strtok(*(tokens + i)," ");
	      str = strtok(NULL," ");
	      bot[1] = atof(str)*nx;
	    }
	  } else {
	    if (strcmp(*(tokens + i),"grid_os_x")==0) isX = True;
	    if (strcmp(*(tokens + i),"grid_os_y")==0) isY = True;
	    if (strcmp(*(tokens + i),"gradient_width_top")==0) isT = True;
	    if (strcmp(*(tokens + i),"gradient_width_bot")==0) isB = True;
	    if (strcmp(*(tokens + i),"gradient_alpha")==0) isA = True;
	    if (strstr(*(tokens + i),"gradient_on")) {
	      str = strtok(*(tokens + i)," ");
	      str = strtok(NULL," ");
	      has_blend = atoi(str);
	      printf("Blending Information in File: %i \n",has_blend);
	    }
	  }
	} else if (i == 4) {
	  // first col/row information: init ngx/ngy
	  if (ngy == 0) {
	    if (is_xp12) {
	      ngx = stepx * (dragx-1) + 1;
	      ngy = stepy * (dragy-1) + 1;
	    } else {
	      ngy = 101;
	      ngx = 101;
	    }
	  }
	  if (isX || isY) {
	    if (is_xp12) {
	      // We read horizontal or vertical shifts in pixels (xp12)
	      str = strtok(*(tokens + i)," ");
	      row = ngy - 1 - atoi(str); // Inverse Rows since X-Plane counts from bottom to top
	      str = strtok(NULL," ");
	      val = atof(str);
	      if (isX) {
		vx[row][col][1] = (float) col / (float) (ngx-1);
		vx[row][col][0] = val;
		//if ((col == 0) && (row == 0)) printf("X: %f %f \n",vx[row][col][0],vx[row][col][1]);
		gx++;
	      } else {
		vy[row][col][1] = (float) row / (float) (ngy-1);
		// Inverse vertical shift.
		vy[row][col][0] = 1.0-val ;
		//if ((col == 0) && (row == 0)) printf("Y: %f %f \n",vy[row][col][0],vy[row][col][1]);
		gy++;
	      }	      
	    } else {
	      col = atoi(*(tokens + i));
	    }
	  } else if (isA) {
	    str = strtok(*(tokens + i)," ");
	    side = atoi(str);
	  } else if (isT || isB) {
	    str = strtok(*(tokens + i)," ");
	    side = atoi(str);
	    str = strtok(NULL," ");
	    val = atof(str);
	    if (isT) {
	      top[side] = val;
	    } else {
	      bot[side] = val;
	    }
	  }
	} else if (i == 5) {
	  if (!is_xp12) {
	    // We read horizontal or vertical shifts in pixels (xp11)
	    if (isX || isY) {
	      str = strtok(*(tokens + i)," ");
	      row = ngy - 1 - atoi(str); // Inverse Rows since X-Plane counts from bottom to top
	      str = strtok(NULL," ");
	      val = atof(str);
	      if (isX) {
		vx[row][col][1] = (float) col / (float) (ngx-1);
		vx[row][col][0] = (val + (float) col / (float) (ngx-1) * (float) nx) / (float) nx;
		//if ((col == 0) && (row == 0)) printf("X: %f %f \n",vx[row][col][0],vx[row][col][1]);
		gx++;
	      } else {
		vy[row][col][1] = (float) row / (float) (ngy-1);
		// Inverse vertical shift.
		vy[row][col][0] = (-val + (float) row / (float) (ngy-1) * (float) ny) / (float) ny;
		//if ((col == 0) && (row == 0)) printf("Y: %f %f \n",vy[row][col][0],vy[row][col][1]);
		gy++;
	      }
	    } else if (isA) {
	      str = strtok(*(tokens + i)," ");
	      dist = atoi(str);
	      str = strtok(NULL," ");
	      val = atof(str);
	      alpha[side][dist] = val;
	    }
	  }
	}
	free(*(tokens + i));
      }
      free(tokens);
    } /* if we have / separated string */
  } /* while not EOF */
  
  fclose(fptr);

  printf("XP12 File Format: %i \n",is_xp12);

  //printf("%f %f %f %f \n",vx[0][0][0],vx[ngy-1][ngx-1][0],vy[0][0][0],vy[ngy-1][ngx-1][0]);
  
  if (has_blend) {
    for (side=0;side<=1;side++) {
      if (side == 0) {
	printf("Left Blending: \n");
      } else {
	printf("Right Blending: \n");
      }
      printf("Top Pixel Size: %f\n",top[side]);
      printf("Bot Pixel Size: %f\n",bot[side]);
      if (!is_xp12) {
	for (dist=0;dist<=3;dist++) {
	  printf("Alpha at Dist %i: %f\n",dist,alpha[side][dist]);
	}
      }
    }
  }
  
  gx /= ngx;
  gy /= ngy;

  has_warp = True;
  if (gx != ngx) has_warp = False;
  if (gy != ngy) has_warp = False;
  if (ngy == 0) has_warp = False;
  if (ngx == 0) has_warp = False;
  
  printf("Number of Warp Grid Cols read: %i (of %i)\n",gx,ngx);
  printf("Number of Warp Grid Rows read: %i (of %i)\n",gy,ngy);

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

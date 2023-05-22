/*
 * Copyright (c) 2013 NVIDIA Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Changes and additions for 3 Projector Warping and Blending with the X-Plane
   Warp and Blend Configuration file by Reto Stockli, 2023 */

/* NEEDED:
   libxnvctrl-dev
*/

/* Relative screen / texture coordinates go:
   y: top to bottom (0-1)
   x: left to right (0-1)

   triangles are constructed with:
   0: tl, 1: tr, 2: bl, 3: tr, 4: br, 5: bl
*/

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "warpblend.h"

typedef struct {
  float x, y;
} vertex2f;

typedef struct {
  vertex2f pos;
  vertex2f tex;
  vertex2f tex2;
} vertexDataRec;

vertexDataRec *warpData = NULL;
XImage *blendImage = NULL;
unsigned char *imageData = NULL;
Pixmap blendPixmap;

Display *xDpy = NULL;
int screenId;

float xval[8];
float yval[8];

int read_warpfile(const char warpfile[],const char smonitor[]);
char** str_split(char* a_str, const char a_delim);
float interpolate(float input);

static inline float transformPoint(vertex2f *vec) {
  float w, oneOverW;
  float x_in, y_in;

  // Sample projection matrix generated from a trapezoid projection
  static const float mat[3][3] =
    {
     { 0.153978257544863,-0.097906833257365,0.19921875 },
     { -0.227317623368679,0.222788944798964,0.25 },
     { -0.585236541598693,-0.135471643796181,1 }
    };

  x_in = vec->x;
  y_in = vec->y;

  vec->x = x_in * mat[0][0] + y_in * mat[0][1] + mat[0][2];
  vec->y = x_in * mat[1][0] + y_in * mat[1][1] + mat[1][2];
  w      = x_in * mat[2][0] + y_in * mat[2][1] + mat[2][2];

  oneOverW = 1.0 / w;

  vec->x *= oneOverW;
  vec->y *= oneOverW;

  return oneOverW;
}

int main(int ac, char **av) {
  char warpfile[] = "X-Plane Window Positions.prf";
  
  GC gc;
  XGCValues values;

  int nvDpyId = -1;
  bool blendAfterWarp = True;
  bool warp = False;
  bool unwarp = False;
  bool blend = False;
  bool unblend = False;
  bool test = False;
  int numVertices = 0;
  int monitor = -1;
  char smonitor[2];
  int ret;
  int a;


  xDpy = XOpenDisplay(NULL);
  if (!xDpy) {
    fprintf (stderr, "Could not open X Display %s!\n", XDisplayName(NULL));
    return 1;
  }

  screenId = XDefaultScreen(xDpy);

  for (a=1;a<ac;a++) {
    if (a == 1) {
      if (strspn(av[a], "0123456789") == strlen(av[a])) {
	nvDpyId = atoi(av[a]);
	printf("DPY ID: %i\n",nvDpyId);
      } else {
	if (strcmp("--help",av[a]) == 0) {
	  printf("Usage: ./warpblend DPI [monitor] [--warp] [--unwarp] [--blend] [--unblend] [--blend-after-warp] [--test]\n");
	  printf ("DPY is the Display Port, see 'nvidia-settings -q CurrentMetaMode' \n");
	  printf ("Monitor is the monitor number in X-Plane Window position.prf \n");
	  printf ("Option --test does not require any other option. It generates a Test warping and blending. \n");
	  return 1;
	} else {
	  printf("Please supply the DPY-# as first argument\n");
	  return 1;
	}
      }
    }
    if (a == 2) {
      if (strspn(av[a], "0123456789") == strlen(av[a])) {
	monitor = atoi(av[2]);
	printf("Monitor #: %i\n",monitor);
      }
    }
    if (strcmp("--warp", av[a]) == 0) warp = True;
    if (strcmp("--unwarp", av[a]) == 0) unwarp = True;
    if (strcmp("--blend", av[a]) == 0) blend = True;
    if (strcmp("--unblend", av[a]) == 0) unblend = True;
    if (strcmp("--blend-after-warp", av[a]) == 0) blendAfterWarp = True;
    if (strcmp("--test", av[a]) == 0) test = True;
  }

  if (!(unwarp || unblend || test) && (monitor == -1)) {
    printf("Please provide the X-Plane Monitor number as second argument \n");
    return 1;
  }

  sprintf(smonitor,"%d",monitor);

  if (warp || blend ) {
    printf("Reading X-Plane Monitor File with Warp / Blend Information: %s\n",warpfile);
    numVertices = read_warpfile(warpfile,smonitor);
    if (numVertices == 0) return 1;
  }
      
  /**** WARPING ****/
    
  if (unwarp) {
    /* Reset Warping to Regular Screen / Texture Coordinates */
    printf("Reset Warping ...\n");
    if(warpData != NULL)
      free(warpData);
    warpData = NULL;
    numVertices = 0;
  } else if (warp) {
    if (numVertices == 0) {
      printf("No Warp Information in X-Plane Monitor File ...\n");
    }
  } else if (test) {
    printf("Create Test Warping ...\n");
    /* Sample Warping Grid for Testing */
    numVertices = 6;
    warpData = (vertexDataRec*)malloc(numVertices * sizeof(vertexDataRec));      
      
    // Start with two screen-aligned triangles, and warp them using the sample
    // keystone matrix in transformPoint. Make sure we save W for correct
    // perspective and pass it through as the last texture coordinate component.
    warpData[0].pos.x = 0.0f;
    warpData[0].pos.y = 0.0f;
    warpData[0].tex.x = 0.0f;
    warpData[0].tex.y = 0.0f;
    warpData[0].tex2.x = 0.0f;
    warpData[0].tex2.y = transformPoint(&warpData[0].pos);
      
    warpData[1].pos.x = 1.0f;
    warpData[1].pos.y = 0.0f;
    warpData[1].tex.x = 1.0f;
    warpData[1].tex.y = 0.0f;
    warpData[1].tex2.x = 0.0f;
    warpData[1].tex2.y = transformPoint(&warpData[1].pos);
      
    warpData[2].pos.x = 0.0f;
    warpData[2].pos.y = 1.0f;
    warpData[2].tex.x = 0.0f;
    warpData[2].tex.y = 1.0f;
    warpData[2].tex2.x = 0.0f;
    warpData[2].tex2.y = transformPoint(&warpData[2].pos);
      
    warpData[3].pos.x = 1.0f;
    warpData[3].pos.y = 0.0f;
    warpData[3].tex.x = 1.0f;
    warpData[3].tex.y = 0.0f;
    warpData[3].tex2.x = 0.0f;
    warpData[3].tex2.y = transformPoint(&warpData[3].pos);
      
    warpData[4].pos.x = 1.0f;
    warpData[4].pos.y = 1.0f;
    warpData[4].tex.x = 1.0f;
    warpData[4].tex.y = 1.0f;
    warpData[4].tex2.x = 0.0f;
    warpData[4].tex2.y = transformPoint(&warpData[4].pos);
      
    warpData[5].pos.x = 0.0f;
    warpData[5].pos.y = 1.0f;
    warpData[5].tex.x = 0.0f;
    warpData[5].tex.y = 1.0f;
    warpData[5].tex2.x = 0.0f;
    warpData[5].tex2.y = transformPoint(&warpData[5].pos);            
  }

  // Prime the random number generator, since the helper functions need it.
  srand(time(NULL));
    
  // Apply our transformed warp data to the chosen display.
  if (warp || unwarp || test) {
    ret = XNVCTRLSetScanoutWarping(xDpy,
				   screenId,
				   nvDpyId,
				   NV_CTRL_WARP_DATA_TYPE_MESH_TRIANGLES_XYUVRQ,
				   //NV_CTRL_WARP_DATA_TYPE_MESH_TRIANGLESTRIP_XYUVRQ,
				   numVertices, // 6 vertices for two triangles
				   (float *)warpData);

    if ((ret != 0) && (!unwarp)) printf("Warp failed with return value: %i\n",ret);
  } 

  
  /**** BLENDING ****/

  if (unblend) {
    printf("Reset Blending ...\n");
  } else if (blend) {
    if (imageData == NULL) {
      printf("No Blend Information in X-Plane Monitor File ...\n");
    }
  } else if (test) {
    printf("Create Test Blending ...\n");
      
    // Test with a 32x32 pixmap.
    blendPixmap = XCreatePixmap(xDpy, RootWindow(xDpy, screenId),
				32, 32, DefaultDepth(xDpy,screenId));
      
    values.foreground = 0x77777777;
    gc = XCreateGC(xDpy, blendPixmap, GCForeground, &values);
      
    // Fill it fully with grey.
    XFillRectangle(xDpy, blendPixmap, gc, 0, 0, 32, 32);
      
    // Make white inner area (not blended)
    values.foreground = 0xffffffff;
    XChangeGC(xDpy, gc, GCForeground, &values);
      
    // Fill everything but a one-pixel border with white.
    XFillRectangle(xDpy, blendPixmap, gc, 1, 1, 30, 30);
  }

  if (blend || unblend || test) {
    printf("Blend After Warp Flag : %i \n",blendAfterWarp);
    //     Apply it to the display. blendAfterWarp is FALSE, so the edges will be
    //     blended in warped space.
    ret = XNVCTRLSetScanoutIntensity(xDpy,
				     screenId,
				     nvDpyId,
				     blendPixmap,
				     blendAfterWarp);
      
    if ((ret != 0) && (!unblend)) printf("Blend failed with return value: %i\n",ret);

  }
    
  return 0;
}

int read_warpfile(const char warpfile[],const char smonitor[]) {

  bool has_warp = True;
  bool has_blend = True;
  
  int ncol=101;
  int nrow=101;
  int nx;
  int ny;
  int nr;
  int nv;
  FILE *fptr;
  char buf[256];
  char str1[40];
  int ret;

  bool isX;
  bool isY;
  bool isT;
  bool isB;
  bool isA;
  
  int col;
  int row;
  float val;

  int c;
  int r;
  int i;
  int x;
  int y;
  unsigned long int pixval;
  unsigned char bytval;
  int nChannels = 4;
  
  /* first value is vertex coordinate and second value is warped coordinate */
  float vx[nrow][ncol][2];
  float vy[nrow][ncol][2];
  float x0;
  float x1;

  int side;
  int dist;
  float top[2];
  float bot[2];
  float alpha[2][4];

  fptr = fopen(warpfile,"r");
  if (fptr == NULL) {
    printf("Error: Warpfile not found: %s \n",warpfile);
    return -1;
  }

  /***********************************************************************************************/
  /***** PLEASE MAKE NX/NY DYNAMIC AS X-PLANE PREFS MAY NOT BE THE ACTUAL MONITOR RESOLUTION *****/
  /***********************************************************************************************/
  
  c = 0;
  r = 0;
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
	    //printf("%i : %s\n",i, *(tokens + i));
	    str = strtok(*(tokens + i)," ");
	    str = strtok(NULL," ");
	    nx = atoi(str);
	    printf("X-Res: %i \n",nx);
	  } 
	  if (strstr(*(tokens + i),"m_y_res_full")!=0) {
	    //printf("%i : %s\n",i, *(tokens + i));
	    str = strtok(*(tokens + i)," ");
	    str = strtok(NULL," ");
	    ny = atoi(str);
	    printf("Y-Res: %i \n",ny);
	  } 
	} else if (i == 3) {
	  if (strcmp(*(tokens + i),"grid_os_x")==0) isX = True;
	  if (strcmp(*(tokens + i),"grid_os_y")==0) isY = True;
	  if (strcmp(*(tokens + i),"gradient_width_top")==0) isT = True;
	  if (strcmp(*(tokens + i),"gradient_width_bot")==0) isB = True;
	  if (strcmp(*(tokens + i),"gradient_alpha")==0) isA = True;
	  if (strstr(*(tokens + i),"gradient_on")) {
	    str = strtok(*(tokens + i)," ");
	    str = strtok(NULL," ");
	    has_blend = atoi(str);
	    printf("Blending Enabled: %i \n",has_blend);
	  }
	} else if (i == 4) {
	  if (isX || isY) {
	    col = atoi(*(tokens + i));
	  } else if (isA) {
	    side = atoi(*(tokens + i));
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
	  // We read horizontal or vertical shifts in pixels
	  if (isX || isY) {
	    //printf("%i : %s\n",i, *(tokens + i));
	    str = strtok(*(tokens + i)," ");
	    row = nrow - 1 - atoi(str); // Inverse Rows since X-Plane counts from bottom to top
	    str = strtok(NULL," ");
	    val = atof(str);
	    if (isX) {
	      vx[row][col][1] = (float) col / (float) (ncol-1);
	      vx[row][col][0] = (val + (float) col / (float) (ncol-1) * (float) nx) / (float) nx;
	      //if ((col == 0) && (row == 0)) printf("X: %f %f \n",vx[row][col][0],vx[row][col][1]);
	      c++;
	    } else {
	      vy[row][col][1] = (float) row / (float) (nrow-1);
	      // Inverse vertical shift.
	      vy[row][col][0] = (-val + (float) row / (float) (nrow-1) * (float) ny) / (float) ny;
	      //if ((col == 0) && (row == 0)) printf("Y: %f %f \n",vy[row][col][0],vy[row][col][1]);
	      r++;
	    }
	  } else if (isA) {
	    str = strtok(*(tokens + i)," ");
	    dist = atoi(str);
	    str = strtok(NULL," ");
	    val = atof(str);
	    alpha[side][dist] = val;
	  }
	}
	free(*(tokens + i));
      }
      free(tokens);
    } /* if we have / separated string */
  } /* while not EOF */
  
  fclose(fptr);

  if (has_blend) {
    for (side=0;side<=1;side++) {
      if (side == 0) {
	printf("Left Blending: \n");
      } else {
	printf("Right Blending: \n");
      }
      printf("Top Pixel Size: %f\n",top[side]);
      printf("Bot Pixel Size: %f\n",bot[side]);
      for (dist=0;dist<=3;dist++) {
	printf("Alpha at Dist %i: %f\n",dist,alpha[side][dist]);
      }
    }
  }
  
  c /= nrow;
  r /= ncol;

  if (c != ncol) has_warp = False;
  if (r != nrow) has_warp = False;

  if (!has_warp) {
    printf("Number of Warp Grid Cols read: %i (of %i)\n",c,ncol);
    printf("Number of Warp Grid Rows read: %i (of %i)\n",r,nrow);
  }
    
  if (has_blend) {
    imageData = (unsigned char*) malloc (nx * ny * nChannels);
    int bitmap_pad = nChannels * 8;
    int bytes_per_line = nx*4; //displayWidth * nChannels;

    XImage *blendImage = XCreateImage(xDpy, CopyFromParent, DefaultDepth(xDpy, screenId), ZPixmap,
				      0, (char*)imageData, nx, ny, bitmap_pad , bytes_per_line);


    /* fill alpha values into linear interpolation abscissa vector */
    if ((top[0] == 0.0) && (bot[0] == 0.0)) {
      /* no blending on left side */
      yval[0] = 1.0;
      yval[1] = 1.0;
      yval[2] = 1.0;
      yval[3] = 1.0;
    } else {
      yval[0] = alpha[0][3];
      yval[1] = alpha[0][2];
      yval[2] = alpha[0][1];
      yval[3] = alpha[0][0];
    }
    if ((top[1] == 0.0) && (bot[1] == 0.0)) {
      /* no blending on right side */
      yval[4] = 1.0;
      yval[5] = 1.0;
      yval[6] = 1.0;
      yval[7] = 1.0;
    } else {
      yval[4] = alpha[1][0];
      yval[5] = alpha[1][1];
      yval[6] = alpha[1][2];
      yval[7] = alpha[1][3];
    }
     
    for (y=0;y<ny;y++) {
      /* fill screen coordinate values into linear interpolation ordinate vector */
      xval[0] = 0.0;
      xval[1] = 0.40 * (top[0] * (1.0 - (float) y / (float) (ny-1)) + bot[0] * (float) y / (float) (ny-1));
      xval[2] = 0.66 * (top[0] * (1.0 - (float) y / (float) (ny-1)) + bot[0] * (float) y / (float) (ny-1));
      xval[3] = 1.00 * (top[0] * (1.0 - (float) y / (float) (ny-1)) + bot[0] * (float) y / (float) (ny-1));
      xval[4] = (float) nx - 1.00 * (top[1] * (1.0 - (float) y / (float) (ny-1)) + bot[1] * (float) y / (float) (ny-1));
      xval[5] = (float) nx - 0.66 * (top[1] * (1.0 - (float) y / (float) (ny-1)) + bot[1] * (float) y / (float) (ny-1));
      xval[6] = (float) nx - 0.40 * (top[1] * (1.0 - (float) y / (float) (ny-1)) + bot[1] * (float) y / (float) (ny-1));
      xval[7] = (float) nx;
      //      printf("%i %f %f \n",y,xval[3],xval[4]);

      /* TRANSFORM BLENDING XVALS FROM DISPLAY TO TEXTURE COORDINATES SINCE BLENDAFTERWARP DOES NOT WORK */
      if (has_warp) {
	x0 = vx[0][0][0] * (1.0- (float) y / (float) (ny-1)) + vx[nrow-1][0][0] * (float) y / (float) (ny-1);
	x1 = vx[0][ncol-1][0] * (1.0- (float) y / (float) (ny-1)) + vx[nrow-1][ncol-1][0] * (float) y / (float) (ny-1);
	//printf("%f %f \n",x0,x1);
	
	for (i=0;i<8;i++) {
	  xval[i] = -x0 / (x1-x0) * (float) (nx-1) + xval[i] * 1.0 / (x1-x0);
	}
      }
	
      for (x=0;x<nx;x++) {
	/* perform linear interpolation of tranparency between x-plane's blending steps */
	bytval = (unsigned char) (interpolate((float) x) * 255.0);
	if (y == 500) printf("%i ",bytval);
	pixval = 16777216 * (unsigned long) bytval + 65536 * (unsigned long)  bytval +
	  256 * (unsigned long) bytval + (unsigned long) bytval;
	XPutPixel(blendImage, x, y, pixval);
      }
    }

    /* create empty pixmap */
    blendPixmap = XCreatePixmap(xDpy, RootWindow(xDpy, screenId),
				nx, ny, DefaultDepth(xDpy,screenId));

    /* copy xImage data to pixmap */
    XGCValues values; // graphics context values
    GC gc; // the graphics context
    gc = XCreateGC(xDpy, blendPixmap, GCForeground, &values);
    XPutImage(xDpy, blendPixmap, gc, blendImage, 0, 0, 0, 0, nx, ny);

  }

  
  if (c != ncol) return -1;
  if (r != nrow) return -1;

  if (True) {
    /* Triangles */
    
    nr = (nrow-1)*(ncol-1);
    nv = nr*6;
  
    if(warpData != NULL)
      free(warpData);
    warpData = (vertexDataRec*)malloc(nv * sizeof(vertexDataRec));

    printf("number of Vertices allocated: %d\n",nv);

    i=0;
    for(r=0; r<(nrow-1); r++) {
      for(c=0; c<(ncol-1); c++) {
      
	warpData[i * 6].pos.x = vx[r][c][0];
	warpData[i * 6].pos.y = vy[r][c][0];
	warpData[i * 6].tex.x = vx[r][c][1];
	warpData[i * 6].tex.y = vy[r][c][1];
	warpData[i * 6].tex2.x = 0.0f;	
	warpData[i * 6].tex2.y = 1.0f;
      
	warpData[i * 6 + 1].pos.x = vx[r][c+1][0];
	warpData[i * 6 + 1].pos.y = vy[r][c+1][0];
	warpData[i * 6 + 1].tex.x = vx[r][c+1][1];
	warpData[i * 6 + 1].tex.y = vy[r][c+1][1];
	warpData[i * 6 + 1].tex2.x = 0.0f;
	warpData[i * 6 + 1].tex2.y = 1.0f;
      
	warpData[i * 6 + 2].pos.x = vx[r+1][c][0];
	warpData[i * 6 + 2].pos.y = vy[r+1][c][0];
	warpData[i * 6 + 2].tex.x = vx[r+1][c][1];
	warpData[i * 6 + 2].tex.y = vy[r+1][c][1];
	warpData[i * 6 + 2].tex2.x = 0.0f;
	warpData[i * 6 + 2].tex2.y = 1.0f;
      
	warpData[i * 6 + 3].pos.x = vx[r][c+1][0];
	warpData[i * 6 + 3].pos.y = vy[r][c+1][0];
	warpData[i * 6 + 3].tex.x = vx[r][c+1][1];
	warpData[i * 6 + 3].tex.y = vy[r][c+1][1];
	warpData[i * 6 + 3].tex2.x = 0.0f;
	warpData[i * 6 + 3].tex2.y = 1.0f;
      
	warpData[i * 6 + 4].pos.x = vx[r+1][c+1][0];
	warpData[i * 6 + 4].pos.y = vy[r+1][c+1][0];
	warpData[i * 6 + 4].tex.x = vx[r+1][c+1][1];
	warpData[i * 6 + 4].tex.y = vy[r+1][c+1][1];
	warpData[i * 6 + 4].tex2.x = 0.0f;
	warpData[i * 6 + 4].tex2.y = 1.0f;
      
	warpData[i * 6 + 5].pos.x = vx[r+1][c][0];
	warpData[i * 6 + 5].pos.y = vy[r+1][c][0];
	warpData[i * 6 + 5].tex.x = vx[r+1][c][1];
	warpData[i * 6 + 5].tex.y = vy[r+1][c][1];
	warpData[i * 6 + 5].tex2.x = 0.0f;
	warpData[i * 6 + 5].tex2.y = 1.0f;

	i++;
      }
    }
    
    printf("Assigned %i of %i Warp Rectangles \n",i,nr);

  } else {
    /* Triangle Strip */
    
    nv = (ncol * (nrow-1) * 2) + 2 * (nrow - 2);

    if(warpData != NULL)
      free(warpData);
    warpData = (vertexDataRec*)malloc(nv * sizeof(vertexDataRec));

    printf("number of Vertices allocated: %d\n",nv);
  
    i = 0; //vertex index
    for(r=0; r<nrow-1; ++r)
      {
        double v1x,v1y,v2x,v2y,v3x,v3y,v4x,v4y;
        float t1x,t1y,t2x,t2y,t3x,t3y,t4x,t4y;
        float b1x,b1y,b2x,b2y,b3x,b3y,b4x,b4y;

        for(c=0; c<ncol; ++c)
	  {
            // vertex coordinates
            v1x=vx[r][c][0];
            v1y=vy[r][c][0];
            v2x=vx[r][c+1][0];
            v2y=vy[r][c+1][0];
            v3x=vx[r+1][c+1][0];
            v3y=vy[r+1][c+1][0];
            v4x=vx[r+1][c][0];
            v4y=vy[r+1][c][0];

            // texture coordinates of the visual image
            t1x=vx[r][c][1];
            t1y=vy[r][c][1];
            t2x=vx[r][c+1][1];
            t2y=vy[r][c+1][1];
            t3x=vx[r+1][c+1][1];
            t3y=vy[r+1][c+1][1];
            t4x=vx[r+1][c][1];
            t4y=vy[r+1][c][1];


            if(c == 0 && r > 0)
	      {
                // Add padding at beginning of row except the first
                //printf("Front padding: %d %d\n",c,r);
                warpData[i].pos.x = v1x;
                warpData[i].pos.y = v1y;
                warpData[i].tex.x = t1x;
                warpData[i].tex.y = t1y;
                warpData[i].tex2.x = 0.;
                warpData[i].tex2.y = 0.;//transformPoint(&warpData[i].pos);
                i++;
	      }

            // Add vertices to warpData
            warpData[i].pos.x = v1x;
            warpData[i].pos.y = v1y;
            warpData[i].tex.x = t1x;
            warpData[i].tex.y = t1y;
            warpData[i].tex2.x = 0.;
            warpData[i].tex2.y = 0.;//transformPoint(&warpData[i].pos);
            i++;
            warpData[i].pos.x = v4x;
            warpData[i].pos.y = v4y;
            warpData[i].tex.x = t4x;
            warpData[i].tex.y = t4y;
            warpData[i].tex2.x = 0.;
            warpData[i].tex2.y = 0.;//transformPoint(&warpData[i].pos);
            i++;
	  }

        if(r < nrow - 2)
	  {
            // Add padding at end of row except the last
            //printf("End padding: %d %d\n",c,r);
            warpData[i].pos.x = v4x;
            warpData[i].pos.y = v4y;
            warpData[i].tex.x = t4x;
            warpData[i].tex.y = t4y;
            warpData[i].tex2.x = 0.;
            warpData[i].tex2.y = 0.; //transformPoint(&warpData[i].pos);
            i++;
	  }

      }
    printf("Assigned %i vertices of %i \n",i,nv);

  }
  
  return nv;
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

float interpolate(float input)
{
  int left;
  int right;
  float val;
  int n=sizeof(xval)/sizeof(float);

  left = n-2;
  right = n-1;
  for (int i=1;i<(n-1);i++) {
    if (input < xval[i]) {
      left = i-1;
      right = i;
      break;
    }
  }

  if (input < xval[0]) {
    val = yval[0];
  } else if (input > xval[n-1]) {
    val = yval[n-1];
  } else {
    val = ((xval[right] - input) * yval[left] + (input - xval[left]) * yval[right]) / (xval[right] - xval[left]);
  }
  
  //  printf("%f %i %i %f \n",xval,left,right,val);

  return val;
}

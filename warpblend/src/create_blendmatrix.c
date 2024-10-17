/* This is the create_blendmatrix.c code which generates a NVIDIA blend matrix
   out of a blend grid.

   Copyright (C) 2024 Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */
    
  /* Create Blend Image */

#include "warpdata.h"
#include "create_blendmatrix.h"

#define min(A,B) ((A)<(B) ? (A) : (B))
#define max(A,B) ((A)>(B) ? (A) : (B))

float xval[8];
float yval[8];

float blend_exp = 0.75;

XImage *blendImage;
unsigned char *imageData;
Pixmap blendPixmap;

int create_blendmatrix(void) {

  /* first value is vertex coordinate and second value is warped coordinate */
  float x0;
  float x1;
  float xl;
  float xr;
  float xw;
  float wgt;

  int x;
  int y;

  float fcol;

  int col;
  int row;
  float val;

  int i;
  
  unsigned long int pixval;
  unsigned char bytval;
  int nChannels = 4;
  		      
  if (has_blend) {
    imageData = (unsigned char*) malloc (nx * ny * nChannels);
    int bitmap_pad = nChannels * 8;
    int bytes_per_line = nx*4; //displayWidth * nChannels;

    XImage *blendImage = XCreateImage(xDpy, CopyFromParent, DefaultDepth(xDpy, screenId), ZPixmap,
				      0, (char*)imageData, nx, ny, bitmap_pad , bytes_per_line);


    if ((alpha[0][0] == 0.0) && (alpha[1][0] == 0.0)) {
      /* XP12 has no predefined blending values but we use a curve */
      for (y=0;y<ny;y++) {
	//for (y=500;y<501;y++) {
	//x0 = vx[0][0][0] * (1.0- (float) y / (float) (ny-1)) + vx[nrow-1][0][0] * (float) y / (float) (ny-1);
	//x1 = vx[0][ncol-1][0] * (1.0- (float) y / (float) (ny-1)) + vx[nrow-1][ncol-1][0] * (float) y / (float) (ny-1);
	xl = y/(ny-1) * bot[0] + (1.0-y/(ny-1)) * top[0];
	xr = y/(ny-1) * bot[1] + (1.0-y/(ny-1)) * top[1];

	row = (int) ((float) y / (float) (ny-1) * (float) (nrow-1));
	
        for (x=0;x<nx;x++) {

	  /* linear interpolation of warping grid x positions */
	  /* TODO: Bilinear interpolation in x and y */
	  col = (int) ((float) x / (float) (nx-1) * (float) (ncol-1));
	  fcol = ((float) x / (float) (nx-1) * (float) (ncol-1));
	  wgt = fcol - (float) col;
	  
	  xw = (1.0 - wgt) * vx[row][col][0] + wgt * vx[row][col+1][0];
	  //printf("%i %i %f %f %f \n",x,col,fcol,wgt,xw);
	    //printf("%i %f %f\n",x,vx[row][col][0] * (float) (nx-1),val);
	  
	  if (x < nx/2) {
	    /* left blending until center of image */
	    if (xl == 0.0) {
	      val = 1.0;
	    } else {
	      val = min(powf(max(xw * (float) (nx-1),0.0)/xl,blend_exp),1.0);
	    } 
	  } else {
	    /* right blending starting at center of image */
	    if (xr == 0.0) {
	      val = 1.0;
	    } else {
	      val = min(powf(max(nx- (xw * (float) (nx-1)),0.0)/xr,blend_exp),1.0);
	    }
	  }
	  bytval = (unsigned char) (val * 255.0);
	  pixval = 16777216 * (unsigned long) bytval + 65536 * (unsigned long)  bytval +
	    256 * (unsigned long) bytval + (unsigned long) bytval;
	  XPutPixel(blendImage, x, y, pixval);	  
	}
      }
    } else {
    
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
	if (yval[2] == 0.0) {
	  /* blend test mode with hard blend at blend start */
	  xval[1] = 1.0;
	  xval[2] = 0.999 * (top[0] * (1.0 - (float) y / (float) (ny-1)) + bot[0] * (float) y / (float) (ny-1));
	} else {
	  xval[1] = 0.40 * (top[0] * (1.0 - (float) y / (float) (ny-1)) + bot[0] * (float) y / (float) (ny-1));
	  xval[2] = 0.66 * (top[0] * (1.0 - (float) y / (float) (ny-1)) + bot[0] * (float) y / (float) (ny-1));
	}
	xval[3] = 1.00 * (top[0] * (1.0 - (float) y / (float) (ny-1)) + bot[0] * (float) y / (float) (ny-1));
      
	xval[4] = (float) nx - 1.00 * (top[1] * (1.0 - (float) y / (float) (ny-1)) + bot[1] * (float) y / (float) (ny-1));
	if (yval[6] == 0.0) {
	  /* blend test mode with hard blend at blend start */
	  xval[5] = (float) nx - 0.999 * (top[1] * (1.0 - (float) y / (float) (ny-1)) + bot[1] * (float) y / (float) (ny-1));
	  xval[6] = (float) nx-1;
	} else {
	  xval[5] = (float) nx - 0.66 * (top[1] * (1.0 - (float) y / (float) (ny-1)) + bot[1] * (float) y / (float) (ny-1));
	  xval[6] = (float) nx - 0.40 * (top[1] * (1.0 - (float) y / (float) (ny-1)) + bot[1] * (float) y / (float) (ny-1));
	}
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
	  //if (y == 500) printf("%i ",bytval);
	  pixval = 16777216 * (unsigned long) bytval + 65536 * (unsigned long)  bytval +
	    256 * (unsigned long) bytval + (unsigned long) bytval;
	  XPutPixel(blendImage, x, y, pixval);
	}

      }
    }

    if (has_blend) {
      /* create empty pixmap */
      blendPixmap = XCreatePixmap(xDpy, RootWindow(xDpy, screenId),
				  nx, ny, DefaultDepth(xDpy,screenId));
      
      /* copy xImage data to pixmap */
      XGCValues values; // graphics context values
      GC gc; // the graphics context
      gc = XCreateGC(xDpy, blendPixmap, GCForeground, &values);
      XPutImage(xDpy, blendPixmap, gc, blendImage, 0, 0, 0, 0, nx, ny);
    }
      
  }

  return 0;
}

int create_testblendmatrix(void) {
  
  GC gc;
  XGCValues values;

  printf("Create Test Blending ...\n");
  
  // Test with a 32x32 pixmap.
  blendPixmap = XCreatePixmap(xDpy, RootWindow(xDpy, screenId),
			      64, 36, DefaultDepth(xDpy,screenId));
  //    blendPixmap = XCreatePixmap(xDpy, RootWindow(xDpy, screenId),
  //				1920, 1080, DefaultDepth(xDpy,screenId));
  
  //values.foreground = 0x77777777;
  values.foreground = 0x00000000;
  gc = XCreateGC(xDpy, blendPixmap, GCForeground, &values);
  
  // Fill it fully with grey.
  XFillRectangle(xDpy, blendPixmap, gc, 0, 0, 64, 36);
  //XFillRectangle(xDpy, blendPixmap, gc, 0, 0, 1920, 1080);
  
  // Make white inner area (not blended)
  values.foreground = 0xffffffff;
  XChangeGC(xDpy, gc, GCForeground, &values);
  
  // Fill everything but a one-pixel border with white.
  XFillRectangle(xDpy, blendPixmap, gc, 4, 6, 56, 24);
  //XFillRectangle(xDpy, blendPixmap, gc, 300, 150, 1500,700);

  return 0;
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

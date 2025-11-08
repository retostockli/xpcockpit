/* This is the create_blendmatrix.c code which generates a NVIDIA blend matrix
   out of a blend grid.

   Copyright (C) 2024-2025 Reto Stockli

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
  
  /* Blendmatrix consists of nx x ny (full screen resolution) */

  int x;
  int y;
  
  unsigned long int pixval;
  unsigned char bytval;
  int nChannels = 4;
  		      
  imageData = (unsigned char*) malloc (nx * ny * nChannels);
  int bitmap_pad = nChannels * 8;
  int bytes_per_line = nx*4; //displayWidth * nChannels;

  XImage *blendImage = XCreateImage(xDpy, CopyFromParent, DefaultDepth(xDpy, screenId), ZPixmap,
				    0, (char*)imageData, nx, ny, bitmap_pad , bytes_per_line);

  for (y=0;y<ny;y++) {
    for (x=0;x<nx;x++) {
      /* perform linear interpolation of tranparency between x-plane's blending steps */
      bytval = (unsigned char) (blendData[x+y*nx] * 255.0);
      //if ((x==0) && (y==0)) printf("%f %i \n",blendData[x+y*nx],bytval);
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
  
  return 0;
}

int create_testblendmatrix(void) {
  
  GC gc;
  XGCValues values;
  
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

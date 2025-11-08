/*
 * Copyright (c) 2013 NVIDIA Corporation
 *
 * Changes and additions for 3 Projector Warping and Blending with the X-Plane
 * Warp and Blend Configuration file by Reto Stockli, 2023-2025
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

/* 
   Depends on (Linux):
   libxnvctrl-dev
*/

/* 
   This code reads the warp and blend grids from an ascii file and applies the 
   warping and blending grid to the native NVIDIA API.
*/

/* 
   Relative screen / texture coordinates for NVIDIA are:
   y: top to bottom (0.0-1.0)
   x: left to right (0.0-1.0)

   triangles are constructed with these indices:
   0: tl, 1: tr, 2: bl, 3: tr, 4: br, 5: bl
*/

/*
  GTX4080 Output Numbering
  DP-3: DPY-5
  DP-5: DPY-7
  DP-1: DPY-2
  HDMI-0: DPY-0
*/

#include "warpdata.h"
#include "warpblendfile.h"
#include "create_warpmatrix.h"
#include "create_blendmatrix.h"

int main(int ac, char **av) {

  int nvDpyId = -1;
  bool blendAfterWarp = True;
  bool warp = False;
  bool unwarp = False;
  bool blend = False;
  bool unblend = False;
  bool test = False;
  int ret;
  int a;
  char *warpfile = NULL;
  int strsize;
  int numVertices;

  xDpy = XOpenDisplay(NULL);
  if (!xDpy) {
    printf ("Could not open X Display %s!\n", XDisplayName(NULL));
    return 1;
  }

  screenId = XDefaultScreen(xDpy);

  if (ac == 1) {
    printf("Please supply the DPY-# as first argument\n");
    return 1;
  }
  
  for (a=1;a<ac;a++) {
    if (a == 1) {
      if (strspn(av[a], "0123456789") == strlen(av[a])) {
	nvDpyId = atoi(av[a]);
	printf("DPY ID: %i\n",nvDpyId);
      } else {
	if (strcmp("--help",av[a]) == 0) {
	  printf("Usage: ./warpblend DPY [warpfile] [--warp] [--unwarp] [--blend] [--unblend] [--blend-after-warp] [--test] \n");
	  printf ("DPY is the Display Port, see 'nvidia-settings -q CurrentMetaMode' \n");
	  printf ("Warpfile is the Warp & Blend Grid file (e.g. nv_warpblend_grid_0.dat) \n");
	  printf ("Option --test does not require any other option. It generates a Test warping and blending. \n");
	  printf ("Options --unblend and --unwarp do not require a warp file. Just the DPY number \n");
	  printf ("Option --blend-after-warp does not work on GTX/RTX cards, just on Quadros \n");
	  return 1;
	} else {
	  printf("Please supply the DPY-# as first argument\n");
	  return 1;
	}
      }
    }
    if (a == 2) {
      if (strncmp(av[a],"--",2) != 0) {
	strsize = strlen(av[a]);
	warpfile = malloc(strsize+1);
	memset(warpfile,0,strsize+1);
	strncpy(warpfile, av[a],strsize);
	printf("Warp & Blend Grid Input File #: %s\n",warpfile);
      }
    }
    if (strcmp("--warp", av[a]) == 0) warp = True;
    if (strcmp("--unwarp", av[a]) == 0) unwarp = True;
    if (strcmp("--blend", av[a]) == 0) blend = True;
    if (strcmp("--unblend", av[a]) == 0) unblend = True;
    if (strcmp("--blend-after-warp", av[a]) == 0) blendAfterWarp = True;
    if (strcmp("--test", av[a]) == 0) test = True;
  }

  if (!(unwarp || unblend || test) && (warpfile == NULL)) {
    printf("Please provide the Warp & Blend Grid File as second argument \n");
    return 1;
  }


  if ((warp || blend ) && (!test)) {
    if (read_warpblendfile(warpfile)!=0) return 1;
  }
  
  if (test) {
    /* DOES NOT WORK YET TO GET DISPLAY / SCREEN SIZE */
    /*
    ret = XNVCTRLQueryStringAttribute(xDpy, screenId, 0,
				      //NV_CTRL_STRING_CURRENT_METAMODE_VERSION_2,
				      NV_CTRL_STRING_SCREEN_RECTANGLE,
				      &str);
    if (!ret) {
      printf("Display Coordinates: %s\n", str);
    }
    */
  }
  /**** WARPING ****/
    
  if (unwarp) {
    /* Reset Warping to Regular Screen / Texture Coordinates */
    printf("Reset Warping ...\n");
    if(warpData != NULL)
      free(warpData);
    warpData = NULL;
    numVertices = 0;
  } else if (warp && test) {
    printf("Create Test Warp Matrix ...\n");
    numVertices = create_testwarpmatrix();
    if (numVertices == 0) {
      printf("Unable to create Test Warp Matrix ... aborting \n");
      return 1;
    }
     
  } else if (warp) {
    printf("Create Warp Matrix...\n");
    numVertices = create_warpmatrix();
    if (numVertices == 0) {
      printf("Unable to create Warp Matrix ... aborting \n");
      return 1;
    }
  }

  // Prime the random number generator, since the helper functions need it.
  srand(time(NULL));
    
  // Apply our transformed warp data to the chosen display.
  if (warp || unwarp) {
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
  } else if (blend && test) {
    printf("Create Test Blend Matrix...\n");
    if (create_testblendmatrix()!=0) {
      printf("Unable to create Test Blend Image ... aborting \n");
      return 1;
    }
  } else if (blend) {
    printf("Create Blend Matrix...\n");
    if (create_blendmatrix()!=0) {
      printf("Unable to create Blend Image ... aborting \n");
      return 1;
    }
  }


  if (blend || unblend) {
    // printf("Blend After Warp Flag : %i \n",blendAfterWarp);
    //     Apply it to the display. blendAfterWarp is FALSE, so the edges will be
    //     blended in warped space.
    ret = XNVCTRLSetScanoutIntensity(xDpy,
				     screenId,
				     nvDpyId,
				     blendPixmap,
				     blendAfterWarp);
      
    if ((ret != 0) && (!unblend)) {
      printf("Blend failed with return value: %i\n",ret);
      return 1;
    }

  }
    
  return 0;
}

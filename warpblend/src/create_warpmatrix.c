/* This is the create_warpmatrix.c code which generates a NVIDIA warp matrix
   out of a warp grid.

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
#include "create_warpmatrix.h"

vertexDataRec *warpData;

int create_warpmatrix(void) {
  
  /* Warpmatrix consists of ngx x ngy grid points which cover the area nx x ny */
  
  int nr;
  int nv;

  int i,gx,gy;
 
  nv = 0;
  
  if (True) {
    /* Triangles */
    
    nr = (ngy-1)*(ngx-1);
    nv = nr*6;
  
    if(warpData != NULL)
      free(warpData);
    warpData = (vertexDataRec*)malloc(nv * sizeof(vertexDataRec));

    printf("Number of Warp Vertices allocated: %d\n",nv);

    i=0;
    for(gy=0; gy<(ngy-1); gy++) {
      for(gx=0; gx<(ngx-1); gx++) {
      
	warpData[i * 6].pos.x = vx[gy][gx][0];
	warpData[i * 6].pos.y = vy[gy][gx][0];
	warpData[i * 6].tex.x = vx[gy][gx][1];
	warpData[i * 6].tex.y = vy[gy][gx][1];
	warpData[i * 6].tex2.x = 0.0f;	
	warpData[i * 6].tex2.y = 1.0f;
      
	warpData[i * 6 + 1].pos.x = vx[gy][gx+1][0];
	warpData[i * 6 + 1].pos.y = vy[gy][gx+1][0];
	warpData[i * 6 + 1].tex.x = vx[gy][gx+1][1];
	warpData[i * 6 + 1].tex.y = vy[gy][gx+1][1];
	warpData[i * 6 + 1].tex2.x = 0.0f;
	warpData[i * 6 + 1].tex2.y = 1.0f;
      
	warpData[i * 6 + 2].pos.x = vx[gy+1][gx][0];
	warpData[i * 6 + 2].pos.y = vy[gy+1][gx][0];
	warpData[i * 6 + 2].tex.x = vx[gy+1][gx][1];
	warpData[i * 6 + 2].tex.y = vy[gy+1][gx][1];
	warpData[i * 6 + 2].tex2.x = 0.0f;
	warpData[i * 6 + 2].tex2.y = 1.0f;
      
	warpData[i * 6 + 3].pos.x = vx[gy][gx+1][0];
	warpData[i * 6 + 3].pos.y = vy[gy][gx+1][0];
	warpData[i * 6 + 3].tex.x = vx[gy][gx+1][1];
	warpData[i * 6 + 3].tex.y = vy[gy][gx+1][1];
	warpData[i * 6 + 3].tex2.x = 0.0f;
	warpData[i * 6 + 3].tex2.y = 1.0f;
      
	warpData[i * 6 + 4].pos.x = vx[gy+1][gx+1][0];
	warpData[i * 6 + 4].pos.y = vy[gy+1][gx+1][0];
	warpData[i * 6 + 4].tex.x = vx[gy+1][gx+1][1];
	warpData[i * 6 + 4].tex.y = vy[gy+1][gx+1][1];
	warpData[i * 6 + 4].tex2.x = 0.0f;
	warpData[i * 6 + 4].tex2.y = 1.0f;
      
	warpData[i * 6 + 5].pos.x = vx[gy+1][gx][0];
	warpData[i * 6 + 5].pos.y = vy[gy+1][gx][0];
	warpData[i * 6 + 5].tex.x = vx[gy+1][gx][1];
	warpData[i * 6 + 5].tex.y = vy[gy+1][gx][1];
	warpData[i * 6 + 5].tex2.x = 0.0f;
	warpData[i * 6 + 5].tex2.y = 1.0f;

	i++;
      }
    }
    
    printf("Assigned %i of %i Warp Rectangles \n",i,nr);

  } else {
    /* Triangle Strip */
    
    nv = (ngx * (ngy-1) * 2) + 2 * (ngy - 2);

    if(warpData != NULL)
      free(warpData);
    warpData = (vertexDataRec*)malloc(nv * sizeof(vertexDataRec));

    printf("Number of Vertices allocated: %d\n",nv);
  
    i = 0; //vertex index
    for(gy=0; gy<ngy-1; ++gy)
      {
	//double v1x,v1y,v2x,v2y,v3x,v3y,v4x,v4y;
	//float t1x,t1y,t2x,t2y,t3x,t3y,t4x,t4y;
	double v1x,v1y,v4x,v4y;
	float t1x,t1y,t4x,t4y;

	for(gx=0; gx<ngx; ++gx)
	  {
	    // vertex coordinates
	    v1x=vx[gy][gx][0];
	    v1y=vy[gy][gx][0];
	    //v2x=vx[gy][gx+1][0];
	    //v2y=vy[gy][gx+1][0];
	    //v3x=vx[gy+1][gx+1][0];
	    //v3y=vy[gy+1][gx+1][0];
	    v4x=vx[gy+1][gx][0];
	    v4y=vy[gy+1][gx][0];

	    // texture coordinates of the visual image
	    t1x=vx[gy][gx][1];
	    t1y=vy[gy][gx][1];
	    //t2x=vx[gy][gx+1][1];
	    //t2y=vy[gy][gx+1][1];
	    //t3x=vx[gy+1][gx+1][1];
	    //t3y=vy[gy+1][gx+1][1];
	    t4x=vx[gy+1][gx][1];
	    t4y=vy[gy+1][gx][1];


	    if(gx == 0 && gy > 0)
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

	if(gy < ngy - 2)
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

int create_testwarpmatrix(void) {

  /* Sample Warping Grid for Testing */
  int nv = 6;
  warpData = (vertexDataRec*)malloc(nv * sizeof(vertexDataRec));      
  
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

  return nv;
}


float transformPoint(vertex2f *vec) {

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

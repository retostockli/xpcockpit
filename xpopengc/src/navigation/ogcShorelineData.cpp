/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Now hosted by xpcockpit project on github.com
   
  Copyright (c) 2021-2024 Reto Stockli

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

  =========================================================================*/

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>    // std::min,max
#include <cmath>
#include "ogcShorelineData.h"

namespace OpenGC
{

  using namespace std;

  ShorelineData
  ::ShorelineData()
  {
    num_shorelines = 0;
    num_shorelinepoints = NULL;
    shoreline_lon = NULL;
    shoreline_lat = NULL;
  }

  ShorelineData
  ::~ShorelineData()
  {
    int i;
    if (num_shorelinepoints) free(num_shorelinepoints);
    num_shorelinepoints = NULL;
    if (shoreline_centerlon) free(shoreline_centerlon);
    shoreline_centerlon = NULL;
    if (shoreline_centerlat) free(shoreline_centerlat);
    shoreline_centerlat = NULL;
    if (shoreline_lon) {
      for (i=0;i<num_shorelines;i++) {
	free(shoreline_lon[i]);
      }
      free(shoreline_lon);
      shoreline_lon = NULL;
    }
    if (shoreline_lat) {
      for (i=0;i<num_shorelines;i++) {
	free(shoreline_lat[i]);
      }
      free(shoreline_lat);
      shoreline_lat = NULL;
    }
      
  }

  bool
  ShorelineData
  ::CheckFiles()
  {

    bool success = true;
    char filename[100];
    ifstream ifile;

    sprintf(filename,"%s/%s",m_PathToGSHHG.c_str(),"gshhs_h.b");
    ifile.open(filename);
    if(ifile) {
      printf("GSHHG Shoreline File exists: %s \n",filename);
      ifile.close();
    } else {
      printf("GSHHG Shoreline File doesn't exist: %s \n",filename);
      success = false;
    }

    if (!success) printf("GSHHG Shoreline Dataset incomplete. Please Check!\n");
    
    return success;
  }

  bool
  ShorelineData
  ::ReadShoreline()
  {
     
    char filename[100];
    ifstream ifile;
    int nshorelines;
    int level;
    int i,f,s;
    struct GSHHG_HEADER header;
    struct GSHHG_POINT points;
    
    polygon p;			/* input polygon */
    triangulation t;		/* output triangulation */

    sprintf(filename,"%s/%s",m_PathToGSHHG.c_str(),"gshhs_h.b");

    /* Read file twice: first for getting number of polygons, second read the polygons */
    for (f=0;f<2;f++) {
      nshorelines = 0;
      ifile.open (filename, ios::in | ios::binary);
      while (ifile.peek() != EOF) {
	ifile.read ((char*)&header, sizeof(header));
	header.id = swap_int32(header.id);
	header.n = swap_int32(header.n);
	header.flag = swap_int32(header.flag);
	header.west = swap_int32(header.west);
	header.east = swap_int32(header.east);
	header.south = swap_int32(header.south);
	header.north = swap_int32(header.north);
	header.area = swap_int32(header.area);
	header.area_full = swap_int32(header.area_full);
	header.container = swap_int32(header.container);
	header.ancestor = swap_int32(header.ancestor);
	level = header.flag & 255;
	if ((f==1) && (level == 2)) {
	  /* allocate polygon points for lakes */
	  num_shorelinepoints[nshorelines] = header.n;
	  shoreline_centerlon[nshorelines] = 0.0;
	  shoreline_centerlat[nshorelines] = 0.0;
	  shoreline_lon[nshorelines] = (float*)malloc(header.n * sizeof(float));
	  shoreline_lat[nshorelines] = (float*)malloc(header.n * sizeof(float));
	}
      
	for (i=0;i<header.n;i++) {
	  ifile.read ((char*)&points, sizeof(points));
	  points.x = swap_int32(points.x);
	  points.y = swap_int32(points.y);
	  //printf("%i %i %i \n",i,points.x,points.y);
	  if ((f==1) && (level == 2)) {
	    /* read polygon points for lakes */
	    if (points.x > 180000000) {
	      /* move all 180 - 360 degree lons to -180 to 0 degree lon */
	      shoreline_lon[nshorelines][i] = (float) (points.x - 360000000) / 1.e6;
	    } else {
	      shoreline_lon[nshorelines][i] = (float) points.x / 1.e6;
	    }
	    shoreline_lat[nshorelines][i] = (float) points.y / 1.e6;
	    shoreline_centerlon[nshorelines] += shoreline_lon[nshorelines][i];
	    shoreline_centerlat[nshorelines] += shoreline_lat[nshorelines][i];
	    //printf("%f %f \n",shoreline_lon[nshorelines][i],shoreline_lat[nshorelines][i]);
	  }
	}
	if ((f==1) && (level == 2)) {
	  shoreline_centerlon[nshorelines] /= (float) header.n;
	  shoreline_centerlat[nshorelines] /= (float) header.n;
	  //printf("%i %f %f \n",nshorelines,shoreline_centerlon[nshorelines],shoreline_centerlat[nshorelines]);
	}
       
	if (level == 2) {
	  nshorelines++;
	}
      }
      ifile.close();
      if (f==0) {
	num_shorelines = nshorelines;
	printf("Total number of Lakes: %i \n",num_shorelines);
	num_shorelinepoints = (int*)malloc(num_shorelines * sizeof(int));
	shoreline_centerlon = (float*)malloc(num_shorelines * sizeof(float));
	shoreline_centerlat = (float*)malloc(num_shorelines * sizeof(float));
	shoreline_lon = (float**)malloc(num_shorelines * sizeof(float*));
	shoreline_lat = (float**)malloc(num_shorelines * sizeof(float*));
	num_shorelinetriangles = (int*)malloc(num_shorelines * sizeof(int));
	shoreline_triangle1 = (int**)malloc(num_shorelines * sizeof(int*));
	shoreline_triangle2 = (int**)malloc(num_shorelines * sizeof(int*));
	shoreline_triangle3 = (int**)malloc(num_shorelines * sizeof(int*));
      }
    }

    if (num_shorelines > 0) {

      // start triangulation
      for (s=0;s<num_shorelines;s++) {
      //for (s=5000;s<5001;s++) {
	//for (s=0;s<1;s++) {
	p.n = num_shorelinepoints[s]-1;
	//printf("Shoreline: %i Points: %i \n",s,p.n);
	p.p = (point*)malloc(p.n * sizeof(point));
	for (i=0;i<p.n;i++) {
	  p.p[i][X] = shoreline_lon[s][i];
	  p.p[i][Y] = shoreline_lat[s][i];
	  //printf("%f %f \n",shoreline_lon[s][i],shoreline_lat[s][i]);
	}

	t.n = p.n-2;
	t.t = (triangle*) malloc(t.n * sizeof(triangle));
	if (!Process(&p, &t)) {
	  // printf("Bad Triangulation of Shoreline %i !!!\n",s);
	  t.n = 0;
	}

	num_shorelinetriangles[s] = t.n;
	if (num_shorelinetriangles[s] > 0) {
	  shoreline_triangle1[s] = (int*)malloc(num_shorelinetriangles[s] * sizeof(int));
	  shoreline_triangle2[s] = (int*)malloc(num_shorelinetriangles[s] * sizeof(int));
	  shoreline_triangle3[s] = (int*)malloc(num_shorelinetriangles[s] * sizeof(int));
	  for (i=0;i<num_shorelinetriangles[s];i++) {
	    shoreline_triangle1[s][i] = t.t[i][0];
	    shoreline_triangle2[s][i] = t.t[i][1];
	    shoreline_triangle3[s][i] = t.t[i][2];
	  }
	}

	free(p.p);
	free(t.t);
      }
      
      return true;
    } else {
      printf("No Lakes found! \n");
      return false;
    }
  }

  float
  ShorelineData
  ::Area(polygon *contour)
  {
    
    int n = contour->n;
    
    float A=0.0f;
    
    for(int p=n-1,q=0; q<n; p=q++)
      {
	A+= contour->p[p][X]*contour->p[q][Y] - contour->p[q][X]*contour->p[p][Y];
      }
    return A*0.5f;
  }

  bool
  ShorelineData
  ::InsideTriangle(float Ax, float Ay,
		   float Bx, float By,
		   float Cx, float Cy,
		   float Px, float Py)
    
  {
    float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
    float cCROSSap, bCROSScp, aCROSSbp;
    
    ax = Cx - Bx;  ay = Cy - By;
    bx = Ax - Cx;  by = Ay - Cy;
    cx = Bx - Ax;  cy = By - Ay;
    apx= Px - Ax;  apy= Py - Ay;
    bpx= Px - Bx;  bpy= Py - By;
    cpx= Px - Cx;  cpy= Py - Cy;
    
    aCROSSbp = ax*bpy - ay*bpx;
    cCROSSap = cx*apy - cy*apx;
    bCROSScp = bx*cpy - by*cpx;
    
    return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
  };
  
  bool
  ShorelineData
  ::Snip(polygon *contour,int u,int v,int w,int n,int *V)
  {
    int p;
    float Ax, Ay, Bx, By, Cx, Cy, Px, Py;

    Ax = contour->p[V[u]][X];
    Ay = contour->p[V[u]][Y];

    Bx = contour->p[V[v]][X];
    By = contour->p[V[v]][Y];

    Cx = contour->p[V[w]][X];
    Cy = contour->p[V[w]][Y];

    if ( EPSILON > (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax))) ) return false;

    for (p=0;p<n;p++)
      {
	if( (p == u) || (p == v) || (p == w) ) continue;
	Px = contour->p[V[p]][X];
	Py = contour->p[V[p]][Y];
	if (InsideTriangle(Ax,Ay,Bx,By,Cx,Cy,Px,Py)) return false;
      }

    return true;
  }
  
  bool
  ShorelineData
  ::Process(polygon *contour, triangulation *result)
  {
    /* allocate and initialize list of Vertices in polygon */

    int n = contour->n;
    if ( n < 3 ) return false;

    int *V = new int[n];

    /* we want a counter-clockwise polygon in V */

    if ( 0.0f < Area(contour) )
      for (int v=0; v<n; v++) V[v] = v;
    else
      for(int v=0; v<n; v++) V[v] = (n-1)-v;

    int nv = n;

    /*  remove nv-2 Vertices, creating 1 triangle every time */
    int count = 2*nv;   /* error detection */

    for(int m=0, v=nv-1; nv>2; )
      {
	/* if we loop, it is probably a non-simple polygon */
	if (0 >= (count--))
	  {
	    //** Triangulate: ERROR - probable bad polygon!
	    return false;
	  }

	/* three consecutive vertices in current polygon, <u,v,w> */
	int u = v  ; if (nv <= u) u = 0;     /* previous */
	v = u+1; if (nv <= v) v = 0;     /* new v    */
	int w = v+1; if (nv <= w) w = 0;     /* next     */

	if ( Snip(contour,u,v,w,nv,V) )
	  {
	    int a,b,c,s,t;

	    /* true names of the vertices */
	    a = V[u]; b = V[v]; c = V[w];

	    /* output Triangle */
	    result->t[m][0] = a;
	    result->t[m][1] = b;
	    result->t[m][2] = c;

	    m++;

	    /* remove v from remaining polygon */
	    for(s=v,t=v+1;t<nv;s++,t++) {
	      V[s] = V[t];
	    }
	    nv--;
    

	    /* resest error detection counter */
	    count = 2*nv;
	  }
      }



    delete V;

    return true;
  }

} // end namespace OpenGC

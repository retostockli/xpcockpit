/* This is the warpdata.h header as part of the warpblend.c code

   Copyright (C) 2024 Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include <warpblend.h>

extern bool has_warp;
extern bool has_blend;

extern Display *xDpy;
extern int screenId;

/* Screen Resolution in Pixels */
extern int nx;
extern int ny;

/* Warp / Blend Grid in points */
extern int ncol;
extern int nrow;
extern float vx[256][256][2]; /* Maximum Warp Grid Resolution X Positions */
extern float vy[256][256][2]; /* Maximum Warp Grid Resolution Y Positions */

/* Blend Data */
extern float top[2];
extern float bot[2];
extern float alpha[2][4];

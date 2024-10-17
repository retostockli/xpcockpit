/* This is the warpdata.c code

   Copyright (C) 2024 Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

#include "warpdata.h"

bool has_warp;
bool has_blend;

Display *xDpy;
int screenId;

/* Screen Resolution in Pixels */
int nx;
int ny;

/* Warp / Blend Grid in points */
int ncol;
int nrow;
float vx[256][256][2]; /* Maximum Warp Grid Resolution X Positions */
float vy[256][256][2]; /* Maximum Warp Grid Resolution Y Positions */

/* Blend Data */
float top[2];
float bot[2];
float alpha[2][4];


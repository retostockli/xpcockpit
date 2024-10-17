/* This is the create_warpmatrix.h header for the create_warpmatrix.c code

   Copyright (C) 2024 Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

typedef struct {
  float x, y;
} vertex2f;

typedef struct {
  vertex2f pos;
  vertex2f tex;
  vertex2f tex2;
} vertexDataRec;

extern vertexDataRec *warpData;

/* Prototypes */
int create_warpmatrix(void);
int create_testwarpmatrix(void);
float transformPoint(vertex2f *vec);

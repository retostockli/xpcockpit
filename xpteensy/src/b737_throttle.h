/* This is the b737_throttle.h header to the b737_throttle.c code

   Copyright (C) 2025 Reto Stockli

   Additions for analog axes treatment by Hans Jansen 2011
   Also several cosmetic changes and changes for Linux compilation
   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

extern int speedbrake_mode; /* 0: no change; 1: H/W controlling; 2: X-Plane controlling */
extern int parkbrake_mode; /* 0: no change; 1: H/W controlling; 2: X-Plane controlling */
extern int stabilizer_mode; /* 0: no change; 1: H/W controlling; 2: X-Plane controlling */

/* Prototype Functions */
void init_b737_tq(void);
void b737_tq(void);

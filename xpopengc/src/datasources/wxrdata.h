/* This is the wxrdata.h header to the wxrdata.c code

   Copyright (C) 2021  Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. 
*/

#define WXR_MISS -9999.0
#define MAXRADAR 50 /* maximum number of WXR pixels to retrieve in Type 2 */
#define WXR_UPSCALE 5  /* Upscaling Factor */

extern int wxr_type;
extern float wxr_lonmin;
extern float wxr_lonmax;
extern float wxr_latmin;
extern float wxr_latmax;

extern int wxr_pixperlon;
extern int wxr_pixperlat;
extern int wxr_ncol;
extern int wxr_nlin;

extern int wxr_phase; /* 0: getting bounds, 1: getting data */

extern unsigned char **wxr_data; /* storm level 0-100 */
extern int **wxr_height; /* top height of storm feet a.s.l */
extern int wxr_newdata;

/* prototype functions */

void init_wxr(int type, char server_ip[]);
void read_wxr(void);
void exit_wxr(void);
void nearest_uchar(unsigned char **data, unsigned char **newData, int width, int height, int newWidth, int newHeight);
void nearest_int(int **data, int **newData, int width, int height, int newWidth, int newHeight);
void bilinear_uchar(unsigned char **data, unsigned char **newData, int width, int height, int newWidth, int newHeight);
void bilinear_int(int **data, int **newData, int width, int height, int newWidth, int newHeight);
void bicubic_uchar(unsigned char **data, unsigned char **newData, int width, int height, int newWidth, int newHeight);
void bicubic_int(int **data, int **newData, int width, int height, int newWidth, int newHeight);

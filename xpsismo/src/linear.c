/* This is the linear.c code 

   Copyright (C) 2022 Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */
 
static float xval[9] = {0.0,0.125,0.250,0.375,0.500,0.625,0.750,0.875,1.0 }; /* Discrete steps in input space */
static float yval[9] = {0.07,0.15,0.26,0.34,0.45,0.53,0.60,0.67,0.75}; /* Corresponding steps in output space */

float linear_interpolate(float input)
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

/* This is the libsismo.c code which contains all functions to interact with
   the SISMO SOLUCIONES hardware

   Copyright (C) 2020 Reto Stockli

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>

#include "common.h"
#include "libsismo.h"
#include "handleudp.h"

int verbose;

char xpserver_ip[30];
int xpserver_port;
char clientname[100];

char sismoserver_ip[30];
int sismoserver_port;

unsigned char recvBuffer[RECVBUFLEN];
unsigned char sendBuffer[SENDBUFLEN];

int ncards;
sismocard_struct sismocard[MAXCARDS];


/*Function to return the digit of n-th position of num. Position starts from 0*/
int get_digit(int num, int n)
{
    int r;
    r = num / pow(10, n);
    r = r % 10; 
    return r;
}

/* Function to set a specific bit of a byte with a value 0 or 1 */
void set_bit(unsigned char *byte, int bit, int val)
{
  unsigned char mask = 1 << bit; // make sure bit n is 1
  if (val == 0) {
    mask ^= 0xff; // xor mask (only set bit n to 0
    *byte = *byte & mask;
  } else {
    *byte = *byte | mask;
  }
}

/* Function to get value of bit i from byte */
int get_bit(unsigned char byte, int bit)
{
  return ((byte >> bit) & 0x01);
}

/* Function to set the 7 segments of a display where
   the segments are ordered in 8 bits of a byte and 
   the last bit is the decimal point */
void set_7segment(unsigned char *byte, int val, int dp)
{
  switch(val) {
  case 0:
    *byte = 0x7E;
    break;
  case 1:
    *byte = 0x30;
    break;
  case 2:
    *byte = 0x6D;
    break;
  case 3:
    *byte = 0x79;
    break;
  case 4:
    *byte = 0x33;
    break;
  case 5:
    *byte = 0x5B;
    break;
  case 6:
    *byte = 0x5F;
    break;
  case 7:
    *byte = 0x70;
    break;
  case 8:
    *byte = 0x7F;
    break;
  case 9:
    *byte = 0x7B;
    break;
  case -1:
    *byte = 0x01; /* minus sign */
    break;
  default:
    *byte = 0x00; /* all other: display off */
  }
  if (dp == 1) *byte = *byte | 0x80;

}

void test() {
  int i,b;
  int val;
  int ret;
  
  unsigned char initstring[2] = "SC";
  unsigned char sendto_mb = 0x00; // Mother Board
  unsigned char outstype_out = 0x00; // Digital Outputs
  unsigned char outstype_dis = 0x01; // 7 Segment Displays
  int num_ana = 5;
  int num_inp = 64;
  int num_out = 64;
  int num_dis = 32;
  int ana[num_ana];
  int inp[num_inp];
  int out[num_out];
  int dis[num_dis];
  
  struct timeval tval_before, tval_after, tval_result;

  /* Send Outputs */
  memset(sendBuffer,0,SENDBUFLEN);

  sendBuffer[0] = initstring[0];
  sendBuffer[1] = initstring[1];
  sendBuffer[2] = sendto_mb;
  sendBuffer[3] = outstype_out;


  i=2; // set output number
  val=1; // set output value
  //  sendBuffer[4+(i-1)/8] = 0xff;
  set_bit(&sendBuffer[4+(i-1)/8],(i-1)%8,val);
  //  ret = send_udp();

  printf("Sent %i bytes \n", ret);

  gettimeofday(&tval_before, NULL);
  
  // ret = recv_udp();
  
  //if (ret > 0) {
    
  //printf("%i \n",ret);
    
    /*
      printf("N: %i INIT: %c %c \n",ret,recvBuffer[0],recvBuffer[1]);
      printf("MAC %02x:%02x\n",recvBuffer[2],recvBuffer[3]);
      printf("Inputs Type: %02x \n",recvBuffer[4]);
      printf("Activated Daughters: %02x \n",recvBuffer[5]);
      int port = recvBuffer[6] + 256 * recvBuffer[7];
      printf("UDP Port of Card: %i \n",port);
    */
    for (b=0;b<num_inp/8;b++) {
      for (i=0;i<8;i++) {
	inp[b*8+i] = get_bit(recvBuffer[8+b],i);
	//printf("%02x \n",recvBuffer[8+b]);
      }
    }
    
    //printf("Digital Input 1: %i 2: %i 3: %i 4: %i \n",inp[0],inp[1],inp[2],inp[3]);
    //printf("Digital Input 14: %i 16: %i \n",inp[13],inp[15]);

    /*
    for (i=0;i<num_ana;i++) {
      ana[i]= recvBuffer[16+i*2] + 256 * recvBuffer[17+i*2];
    }
    */
    //printf("Analog Input 1: %i \n",ana[0]);

    ana[0]=98765;
    
    /* Send Displays */
    
    memset(sendBuffer,0,SENDBUFLEN);
    
    sendBuffer[0] = initstring[0];
    sendBuffer[1] = initstring[1];
    sendBuffer[2] = sendto_mb;
    sendBuffer[3] = outstype_dis;      
    sendBuffer[4] = 1; // Display Group 1-4 (each display group has 8 displays)
    sendBuffer[13] = 0x0f; // brightness (0-15 or 0x00 - 0x0f)
    for (i=0;i<5;i++) {
      set_7segment(&sendBuffer[5+i%8],get_digit(ana[0],i),0);
    }
    ret = send_udp(sismocard[0].ip,sismocard[0].port,sendBuffer,SENDBUFLEN);
    
    printf("Sent %i bytes \n", ret);
    
    usleep(1000);
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    printf("Time elapsed: %ld.%06ld\n",
	   (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
    //}

}

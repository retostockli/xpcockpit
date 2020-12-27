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

unsigned char recvBuffer[RECVMSGLEN];
unsigned char sendBuffer[SENDMSGLEN];

int ncards;
sismo_struct sismo[MAXCARDS];


/*Function to return the digit of n-th position of num. Position starts from 0*/
int get_digit(int num, int n)
{
    int r;
    r = num / pow(10, n);
    r = r % 10; 
    return r;
}

/* Function to set a specific bit (0-7) of a byte with a value 0 or 1 */
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

/* Function to get value of bit i (0-7) from byte */
int get_bit(unsigned char byte, int bit)
{
  return ((byte >> bit) & 0x01);
}

/* Function to set the 7 segments of a display where
   the segments are ordered in 8 bits of a byte and 
   the last bit is the decimal point

   0..9 are printing those values 
   -10 is printing the minus sign
   10's values (10-19) have a decimal point with the value 0..9
   any other value is setting the display to blank */
void set_7segment(unsigned char *byte, int val)
{
  int dp = 0;
  if (val > 10) {
    dp = 1;
    val -=10;
  }
  
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
  case -10:
    *byte = 0x01; /* minus sign */
    break;
  default:
    *byte = 0x00; /* all other: display off */
  }
  if (dp == 1) *byte = *byte | 0x80;

}

int read_sismo() {
  
  int card;
  int i,b,s;
  int val;
  int input;
  int any;
  
  while (udpReadLeft >= RECVMSGLEN) {
    
    card = -1;
    
    /* empty UDP receive buffer instead of directly accessing the device */

    pthread_mutex_lock(&exit_cond_lock);    
    /* read from start of receive buffer */
    memcpy(recvBuffer,&udpRecvBuffer[0],RECVMSGLEN);
    /* shift remaining read buffer to the left */
    memmove(&udpRecvBuffer[0],&udpRecvBuffer[RECVMSGLEN],udpReadLeft-RECVMSGLEN);    
    /* decrease read buffer position and counter */
    udpReadLeft -= RECVMSGLEN;
    pthread_mutex_unlock(&exit_cond_lock);

    /* decode message */

    /* check init string */
    if ((recvBuffer[0] == 0x53) && (recvBuffer[1] == 0x43)) {
      /* check MAC Address and Port of Card which did send the message */
      
      for (i=0;i<MAXCARDS;i++) {
	if ((sismo[i].mac[0] == recvBuffer[2]) && (sismo[i].mac[1] == recvBuffer[3]) &&
	    (sismo[i].port == recvBuffer[6] + 256 * recvBuffer[7])) card = i;
      }
      
      /* card found */
      if (card >= 0) {

	if (sismo[card].connected == 0) {
	  /* initialize card configuration status with information we read from the UDP packet */
	  sismo[card].connected = 1;
	  
	  /* set # of inputs/outputs depending on activated daughters */
	  sismo[card].daughter_output1 = get_bit(recvBuffer[5],0);
	  sismo[card].daughter_output2 = get_bit(recvBuffer[5],1);
	  sismo[card].daughter_servo = get_bit(recvBuffer[5],2);
	  sismo[card].daughter_display1 = get_bit(recvBuffer[5],3);
	  sismo[card].daughter_analogoutput = get_bit(recvBuffer[5],4);
	  sismo[card].daughter_analoginput = get_bit(recvBuffer[5],5);
	  sismo[card].daughter_display2 = get_bit(recvBuffer[5],6);
	  
	  sismo[card].noutputs = 64 + 64*sismo[card].daughter_output1 + 64*sismo[card].daughter_output2;
	  sismo[card].nservos = 14*sismo[card].daughter_servo;
	  sismo[card].ndisplays = 32 + 32*sismo[card].daughter_display1 + 32*sismo[card].daughter_display2;
	  sismo[card].nanaloginputs = 5 + 10*sismo[card].daughter_analoginput;
	  sismo[card].nanalogoutputs = 0*sismo[card].daughter_analogoutput; /* Planned, but not available */

	  if (verbose > 0) {
	    printf("SISMO Card %i is connected with the following features: \n",card);
	    printf("Daughter Outputs 1:     %i\n",sismo[card].daughter_output1);
	    printf("Daughter Outputs 2:     %i\n",sismo[card].daughter_output2);
	    printf("Daughter Displays 1:    %i\n",sismo[card].daughter_display1);
	    printf("Daughter Displays 2:    %i\n",sismo[card].daughter_display2);
	    printf("Daughter Servo:         %i\n",sismo[card].daughter_servo);
	    printf("Daughter Analog Input:  %i\n",sismo[card].daughter_analoginput);
	    printf("Daughter Analog Output: %i\n",sismo[card].daughter_analogoutput);
	    printf("\n");
	    printf("This card thus has the following capabilities: \n");
	    printf("Number of Inputs:     64..%i\n",sismo[card].ninputs);
	    printf("Number of Outputs:        %i\n",sismo[card].noutputs);
	    printf("Number of Displays:       %i\n",sismo[card].ndisplays);
	    printf("Number of Servos:         %i\n",sismo[card].nservos);
	    printf("Number of Analog Inputs:  %i\n",sismo[card].nanaloginputs);
	    printf("Number of Analog Outputs: %i\n",sismo[card].nanalogoutputs);
	    printf("\n");
	  }
	  
	}
	  
	/* check type of input */
	if (recvBuffer[4] == 0x00) {
	  /* Input from Master */

	  /* Digital Inputs are ordered in 8 bytes with 8 bits each = 64 inputs */
	  /* Located in Bytes 8-15 */

	  /* check if any input has changed */
	  any = 0;
	  for (b=0;b<8;b++) {
	    for (i=0;i<8;i++) {
	      input = b*8+i;
	      val = get_bit(recvBuffer[8+b],i);
	      if (val != sismo[card].inputs[input][0]) any = 1;
	    }
	  }
	  /* if any input changed make sure we capture the whole history of changed
	     inputs, since during a single reception we can have several state changes */
	  if (any) {
	    for (b=0;b<8;b++) {
	      for (i=0;i<8;i++) {
		input = b*8+i;
		/* shift all inputs in history array by one */
		for (s=MAXSAVE-2;s>=0;s--) {
		  sismo[card].inputs[input][s+1] = sismo[card].inputs[input][s];
		}
		val = get_bit(recvBuffer[8+b],i);
		/* update changed flag for changed inputs */
		if (val != sismo[card].inputs[input][0]) {
		  sismo[card].inputs_changed[input] += 1;
		  if (verbose > 0) printf("Card %i Input %i Changed to: %i \n",card,input,val);
		}
		/* update all inputs */
		sismo[card].inputs[input][0] = val; 
	      }
	    }
	  }

	  /* Analog Inputs are ordered in 10 bytes with 2 bytes per input */
	  /* Located in bytes 16-25 */
	  for (i=0;i<5;i++) {
	    val = recvBuffer[16+i*2] + 256 * recvBuffer[17+i*2];
	    if (val != sismo[card].analoginputs[i]) {
	      sismo[card].analoginputs[i] = val;
	      sismo[card].analoginputs_changed[i] += 1;
	      if (verbose > 0) printf("Card %i Analog Input %i Changed to %i \n",card,i,sismo[card].analoginputs[i]);
	    }				      
	  }
	  
	} else if (recvBuffer[4] == 0x01) {
	  /* Input from Daughter Digital Inputs 1 */
	} else if (recvBuffer[4] == 0x02) {
	  /* Input from Daughter Digital Inputs 2 */
	} else if (recvBuffer[4] == 0x03) {
	  /* Inputs from Daughter Analog Inputs */
	} else {
	  printf("Unknown Inputs Type: %02x \n",recvBuffer[4]);
	}
	  
	
	//printf("Left to Read: %i \n",udpReadLeft);
	
      } else {
	printf("Card with MAC %02x:%02x or Port %i is not defined in ini file \n",
	       recvBuffer[2],recvBuffer[3],recvBuffer[6] + 256 * recvBuffer[7]);
      }
    } else {
      printf("Received wrong Init String: %02x %02x \n",recvBuffer[0],recvBuffer[1]);
    }

  } /* while UDP data present in receive buffer */

  return 0;
}

int write_sismo() {

  int ret;
  int card;
  int output;
  int group;
  int display;
  int anychanged;

  for (card=0;card<MAXCARDS;card++) {
    if (sismo[card].connected == 1) {
      
      /* check if master outputs have changed */
      anychanged = 0;
      memset(sendBuffer,0,SENDMSGLEN);
      sendBuffer[0] = 0x53;
      sendBuffer[1] = 0x43;
      sendBuffer[2] = 0x00;
      sendBuffer[3] = 0x00;
      for (output=0;output<64;output++) {
	set_bit(&sendBuffer[4+output/8],output%8,sismo[card].outputs[output]);
	if (sismo[card].outputs_changed[output] == 1) {
	  if (verbose > 0) printf("Card %i Output %i changed to: %i \n",card,output,sismo[card].outputs[output]);
	  anychanged = 1;
	}
      }
      if (anychanged) {
	ret = send_udp(sismo[card].ip,sismo[card].port,sendBuffer,SENDMSGLEN);
	printf("Sent %i bytes to card %i \n", ret,card);
      }
     
      
      /* check if master displays have changed */
      for (group=0;group<4;group++) {
	anychanged = 0;
	memset(sendBuffer,0,SENDMSGLEN);
	sendBuffer[0] = 0x53;
	sendBuffer[1] = 0x43;
	sendBuffer[2] = 0x00;
	sendBuffer[3] = 0x01;
	sendBuffer[4] = group;
	sendBuffer[13] = DISPLAYBRIGHTNESS;
	for (display=0;display<8;display++) {
	  set_7segment(&sendBuffer[5+display],sismo[card].displays[display+group*8]);
	  if (sismo[card].displays_changed[display+group*8] == 1) {
	    if (verbose > 0) printf("Card %i Display %i changed to: %i \n",card,display+group*8,
				    sismo[card].displays[display+group*8]);
	    anychanged = 1;
	  }
	}
	if (anychanged) {
	  ret = send_udp(sismo[card].ip,sismo[card].port,sendBuffer,SENDMSGLEN);
	  printf("Sent %i bytes to card %i \n", ret,card);
	}
      }

      /* check if daughter outputs1 have changed */

      /* check if daughter outputs2 have changed */

      /* check if daughter servos have changed */

      /* check if daughter displays1 have changed */

      /* check if daughter displays2 have changed */
      
    }
  }
  
  return 0;
}

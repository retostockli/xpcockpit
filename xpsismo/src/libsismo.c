/* This is the libsismo.c code which contains all functions to interact with
   the SISMO SOLUCIONES hardware

   Copyright (C) 2020-2023 Reto Stockli

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
#include <assert.h>
#include <pthread.h>

#include "common.h"
#include "libsismo.h"
#include "handleudp.h"
#include "serverdata.h"

struct timeval t2;
struct timeval t1;

#define max(A,B) ((A)>(B) ? (A) : (B)) 
#define min(A,B) ((A)<(B) ? (A) : (B)) 

char sismoserver_ip[30];
int sismoserver_port;

unsigned char sismoRecvBuffer[RECVMSGLEN];
unsigned char sismoSendBuffer[SENDMSGLEN];

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
  if (val >= 10) {
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
  char any;
  int bank;
  int firstinput;
  int ninputs;
  int firstbyte;

  // printf("Packets left to read %i \n",udpReadLeft/RECVMSGLEN);
  
  while (udpReadLeft >= RECVMSGLEN) {
    
    card = -1;
    
    /* empty UDP receive buffer instead of directly accessing the device */

    pthread_mutex_lock(&udp_exit_cond_lock);    
    /* read from start of receive buffer */
    memcpy(sismoRecvBuffer,&udpRecvBuffer[0],RECVMSGLEN);
    /* shift remaining read buffer to the left */
    memmove(&udpRecvBuffer[0],&udpRecvBuffer[RECVMSGLEN],udpReadLeft-RECVMSGLEN);    
    /* decrease read buffer position and counter */
    udpReadLeft -= RECVMSGLEN;
    pthread_mutex_unlock(&udp_exit_cond_lock);

    /* decode message */

    /* check init string */
    if ((sismoRecvBuffer[0] == 0x53) && (sismoRecvBuffer[1] == 0x43)) {
      /* check MAC Address and Port of Card which did send the message */
      
      for (i=0;i<MAXCARDS;i++) {
	if ((sismo[i].mac[0] == sismoRecvBuffer[2]) && (sismo[i].mac[1] == sismoRecvBuffer[3]) &&
	    (sismo[i].port == sismoRecvBuffer[6] + 256 * sismoRecvBuffer[7])) card = i;
      }
      
      /* card found */
      if (card >= 0) {

	
	if (sismo[card].connected == 0) {
	  /* initialize card configuration status with information we read from the UDP packet */
	  sismo[card].connected = 1;
	  
	  /* set # of inputs/outputs depending on activated daughters */
	  sismo[card].daughter_output1 = get_bit(sismoRecvBuffer[5],0);
	  sismo[card].daughter_output2 = get_bit(sismoRecvBuffer[5],1);
	  sismo[card].daughter_servo = get_bit(sismoRecvBuffer[5],2);
	  sismo[card].daughter_display1 = get_bit(sismoRecvBuffer[5],3);
	  sismo[card].daughter_analogoutput = get_bit(sismoRecvBuffer[5],4);
	  sismo[card].daughter_analoginput = get_bit(sismoRecvBuffer[5],5);
	  sismo[card].daughter_display2 = get_bit(sismoRecvBuffer[5],6);
	  
	  sismo[card].noutputs = 64 + 64*sismo[card].daughter_output1 + 64*sismo[card].daughter_output2;
	  sismo[card].nservos = 14*sismo[card].daughter_servo;
	  sismo[card].ndisplays = 32 + 32*sismo[card].daughter_display1 + 32*sismo[card].daughter_display2;
	  sismo[card].nanaloginputs = 5 + 10*sismo[card].daughter_analoginput;
	  sismo[card].nanalogoutputs = 0*sismo[card].daughter_analogoutput; /* Planned, but not available */

	  if (verbose > 0) {
	    printf("\n");
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
	    printf("\n");
	  }
	  
	}

	if (verbose > 3) printf("Read %i bytes from SISMO Card %i \n",RECVMSGLEN,card);
	
	/* check type of input */
	if ((sismoRecvBuffer[4] == 0x00) || (sismoRecvBuffer[4] == 0x01) ||
	    (sismoRecvBuffer[4] == 0x02)) {

	  /* Digital Input from Master (Inputs 0-63) */
	  if (sismoRecvBuffer[4] == 0x00) {
	    bank = 0;
	    firstinput = 0;

	    /*
	      gettimeofday(&t2,NULL);
	      float dt = ((t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1000000.0)*1000.0;
	      t1 = t2;
	      printf("D %i %f ms\n",card,dt);
	    */

	  }
	  /* Digital Input from Daughter Digital Input 1 (Inputs 64-127) */
	  if (sismoRecvBuffer[4] == 0x01) {
	    bank = 1;
	    firstinput = 64;

	  }
	  /* Digital Input from Daughter Digital Input 2 (Inputs 128-191) */
	  if (sismoRecvBuffer[4] == 0x02) {
	    bank = 2;
	    firstinput = 128;
	
	  }

	  /* Digital Inputs are ordered in 8 bytes with 8 bits each = 64 inputs */
	  /* Located in Bytes 8-15 */

	  /* check if any input has changed */
	  any = 0;
	  for (b=0;b<8;b++) {
	    for (i=0;i<8;i++) {
	      input = b*8+i + firstinput;
	      //val = get_bit(sismoRecvBuffer[8+b],i);
	      val = 1-get_bit(sismoRecvBuffer[8+b],i); // we now use 1: input pressed and 0: input free
	      if (val != sismo[card].inputs[input][0]) any = 1;
	    }
	  }
	  /* if any input changed make sure we capture the whole history of changed
	     inputs, since during a single reception we can have several state changes */
	  if (any) {
	    //	    printf("%i %i \n",1-get_bit(sismoRecvBuffer[8+0],0),1-get_bit(sismoRecvBuffer[8+0],1));
	    for (b=0;b<8;b++) {
	      for (i=0;i<8;i++) {
		input = b*8+i + firstinput;
		/* shift all inputs in history array by one */
		for (s=MAXSAVE-2;s>=0;s--) {
		  sismo[card].inputs[input][s+1] = sismo[card].inputs[input][s];
		}
		/* update input if changed */
		//val = get_bit(sismoRecvBuffer[8+b],i);
		val = 1-get_bit(sismoRecvBuffer[8+b],i); // we now use 1: input pressed and 0: input free
		if (val != sismo[card].inputs[input][0]) {
		  if (verbose > 2) printf("Card %i Input %i Changed from %i to: %i \n",card,input,sismo[card].inputs[input][0],val);
		  sismo[card].inputs[input][0] = val; 
		}
	      }
	    }
	    /* update number of history values per input bank */
	    if (sismo[card].inputs_nsave[bank] < MAXSAVE) {
	      sismo[card].inputs_nsave[bank] += 1;
	      if (verbose > 3) printf("Card %i Input Bank %i # of History Values %i \n",
				      card,bank,sismo[card].inputs_nsave[bank]);
	    } else {
	      if (verbose > 2) printf("Card %i Input Bank %i Maximum # of History Values %i Reached \n",
				      card,bank,MAXSAVE);
	    }
	      
	  }

	}
	  
	if ((sismoRecvBuffer[4] == 0x00) || (sismoRecvBuffer[4] == 0x03)) {

	  /*
	  gettimeofday(&t2,NULL);
	  float dt = ((t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1000000.0)*1000.0;
	  t1 = t2;
	  printf("A %i %f ms\n",card,dt);
	  */

	  /* Analog Input from Master (Inputs 0-4) */
	  if (sismoRecvBuffer[4] == 0x00) {
	    firstinput = 0;
	    ninputs = 5;
	    firstbyte = 16;
	  }
	  /* Analog Inputs from Analog Input Daughter (Inputs 5-14) 
	     Last Input of Analog Input Daughter does not work (Input 11 of this card) */
	  if (sismoRecvBuffer[4] == 0x03) {
	    firstinput = 5;
	    ninputs = 10;
	    firstbyte = 8;
	  }

	  /* Analog Inputs are ordered in 10 bytes with 2 bytes per input */
	  /* Located in bytes 16-25 on SC-MB and bytes 8-27 on Daughter */
	  for (input=0;input<ninputs;input++) {
	    val = sismoRecvBuffer[firstbyte+input*2] + 256 * sismoRecvBuffer[firstbyte+1+input*2];
	    /* shift all inputs in history array by one */
	    for (s=MAXSAVE-2;s>=0;s--) {
	      sismo[card].analoginputs[input+firstinput][s+1] = sismo[card].analoginputs[input+firstinput][s];
	    }
	    if (val != sismo[card].analoginputs[input+firstinput][0]) {
	      if (verbose > 3) printf("Card %i Analog Input %i Changed to %i \n",
				      card,input+firstinput,sismo[card].analoginputs[input+firstinput][0]);
	    }
	    sismo[card].analoginputs[input+firstinput][0] = val;			      
	  }
	  
	}
	
	//printf("Left to Read: %i \n",udpReadLeft);
	
      } else {
	if (verbose > 1) printf("Card with MAC %02x:%02x or Port %i is not defined in ini file \n",
	       sismoRecvBuffer[2],sismoRecvBuffer[3],sismoRecvBuffer[6] + 256 * sismoRecvBuffer[7]);
      }
    } else {
      printf("Received wrong Init String: %02x %02x \n",sismoRecvBuffer[0],sismoRecvBuffer[1]);
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
  int servo;
  int servoindex;
  int numservos;
  int anychanged;
  int bank; /* outputs bank: 0: master, 1: daughter 1, 2: daughter 2 */
  int firstoutput;
  int firstdisplay;
  int connected;
  char sendto;

  for (card=0;card<MAXCARDS;card++) {
    if (sismo[card].connected == 1) {
      
      /* check if any outputs have changed and send them to master or daughters */
      for (bank=0;bank<=2;bank++) {

	if (bank==0) {
	  /* master */
	  connected = 1;
	  firstoutput = 0;
	  sendto = 0;
	}
	if (bank==1) {
	  /* daughter 1 */
	  connected = sismo[card].daughter_output1;
	  firstoutput = 64;
	  sendto = 1;
	}
	if (bank==2) {
	  /* daughter 2 */
	  connected = sismo[card].daughter_output2;
	  firstoutput = 192;
	  sendto = 2;
	}

	if (connected) {
	  anychanged = 0;
	  memset(sismoSendBuffer,0,SENDMSGLEN);
	  sismoSendBuffer[0] = 0x53;
	  sismoSendBuffer[1] = 0x43;
	  sismoSendBuffer[2] = sendto;
	  sismoSendBuffer[3] = 0x00;
	  for (output=0;output<64;output++) {
	    set_bit(&sismoSendBuffer[4+output/8],output%8,sismo[card].outputs[output+firstoutput]);
	    if (sismo[card].outputs_changed[output+firstoutput] == CHANGED) {
	      if (verbose > 2) printf("Card %i Output %i changed to: %i \n",
				      card,output+firstoutput,sismo[card].outputs[output+firstoutput]);
	      anychanged = 1;
	      /* reset changed state since data will be sent to SISMO card */
	      sismo[card].outputs_changed[output+firstoutput] = UNCHANGED;
	    }
	  }
	  if (anychanged) {
	    ret = send_udp(sismo[card].ip,sismo[card].port,sismoSendBuffer,SENDMSGLEN);
	    if (verbose > 2) printf("Sent %i bytes to card %i \n", ret,card);
	  }
	}
      }
     
      /* check if any displays have changed and send them to master or daughters */
      for (bank=0;bank<=2;bank++) {

	if (bank==0) {
	  /* master */
	  connected = 1;
	  firstdisplay = 0;
	  sendto = 0;
	}
	if (bank==1) {
	  /* daughter 1 */
	  connected = sismo[card].daughter_display1;
	  firstdisplay = 32;
	  sendto = 4;
	}
	if (bank==2) {
	  /* daughter 2 */
	  connected = sismo[card].daughter_display2;
	  firstdisplay = 64;
	  sendto = 5;
	}

	if (connected) {
	  for (group=0;group<4;group++) {
	    anychanged = 0;
	    memset(sismoSendBuffer,0,SENDMSGLEN);
	    sismoSendBuffer[0] = 0x53;
	    sismoSendBuffer[1] = 0x43;
	    sismoSendBuffer[2] = sendto;
	    sismoSendBuffer[3] = 0x01;
	    sismoSendBuffer[4] = group+1;
	    sismoSendBuffer[13] = DISPLAYBRIGHTNESS; /* Can only be adjusted per group */
	    for (display=0;display<8;display++) {
	      if (sismo[card].displays_brightness[display+group*8+firstdisplay] != DISPLAYBRIGHTNESS) {
		/* Change Brightness if one display in the group does not have standard / maximum brightness */
		sismoSendBuffer[13] = sismo[card].displays_brightness[display+group*8+firstdisplay];
	      }
	      if ((sismo[card].displays_changed[display+group*8+firstdisplay] == UNCHANGEDBINARY) ||
		  (sismo[card].displays_changed[display+group*8+firstdisplay] == CHANGEDBINARY)) {
		sismoSendBuffer[5+display] = sismo[card].displays[display+group*8+firstdisplay];
	      } else {
		set_7segment(&sismoSendBuffer[5+display],sismo[card].displays[display+group*8+firstdisplay]);
	      }
	      if (sismo[card].displays_changed[display+group*8+firstdisplay] == CHANGEDBINARY) {
		if (verbose > 2) printf("Card %i Bank %i Display %i changed to: %04x \n",
					card,bank,display+group*8+firstdisplay,
					sismo[card].displays[display+group*8+firstdisplay]);
		anychanged = 1;
		/* reset changed state since data will be sent to SISMO card */
		sismo[card].displays_changed[display+group*8+firstdisplay] = UNCHANGEDBINARY;
	      }
	      if (sismo[card].displays_changed[display+group*8+firstdisplay] == CHANGED) {
		if (verbose > 2) printf("Card %i Bank %i Display %i changed to: %i \n",
					card,bank,display+group*8+firstdisplay,
					sismo[card].displays[display+group*8+firstdisplay]);
		anychanged = 1;
		/* reset changed state since data will be sent to SISMO card */
		sismo[card].displays_changed[display+group*8+firstdisplay] = UNCHANGED;
	      }
	    }
	    if (anychanged) {
	      ret = send_udp(sismo[card].ip,sismo[card].port,sismoSendBuffer,SENDMSGLEN);
	      if (verbose > 2) printf("Sent %i bytes to card %i \n", ret,card);
	    }
	  }

	}
      }

      /* check if daughter servos have changed */
      connected = sismo[card].daughter_servo;
      if (connected) {
	for (bank=0;bank<2;bank++) {
	  if (bank == 0) {
	    /* first bank has servos 0..7 */
	    numservos = 8;
	  } else {
	    /* second bank has servos 8..13 */
	    numservos = 6;
	  }
	  anychanged = 0;
	  memset(sismoSendBuffer,0,SENDMSGLEN);
	  sismoSendBuffer[0] = 0x53;
	  sismoSendBuffer[1] = 0x43;
	  sismoSendBuffer[2] = 0x03;
	  sismoSendBuffer[3] = bank;
	  for (servo=0;servo<numservos;servo++) {
	    if (bank == 0) {
	      servoindex = servo;
	    } else {
	      /* internal index goes from 10..15 */
	      servoindex = servo+2;
	    }
	    if (sismo[card].servos_changed[servo+bank*8] == CHANGED) {
	      if (verbose > 1) printf("Card %i Servo %i changed to: %i \n",card,servo+bank*8,
				      sismo[card].servos[servo+bank*8]);
	      set_bit(&sismoSendBuffer[4],servoindex,1);
	      sismoSendBuffer[5+servoindex] = (unsigned char) sismo[card].servos[servo+bank*8];
	      
	      anychanged = 1;
	      /* reset changed state since data will be sent to SISMO card */
	      sismo[card].servos_changed[servo+bank*8] = UNCHANGED;
	    }
	  }
	  if (anychanged) {
	    ret = send_udp(sismo[card].ip,sismo[card].port,sismoSendBuffer,SENDMSGLEN);
	    if (verbose > 1) printf("Sent %i bytes to card %i \n", ret,card);
	  }

	}
      }
     
    }
  }
  
  return 0;
}


/* wrapper for digital_input with floating point values */
int digital_inputf(int card, int input, float *fvalue, int type)
{
  int value = INT_MISS;
  if (*fvalue != FLT_MISS) value = (int) lroundf(*fvalue);
  int ret = digital_input(card, input, &value, type);
  if (value != INT_MISS) *fvalue = (float) value;
  return ret;
    
}

/* retrieve input value from given input position of SISMO card */
/* master card has 64 inputs (0..63)
   daughter card 1 and 2 have another 64 inputs (64..127 and 128..191) */
/* Two types : */
/* 0: pushbutton */
/* 1: toggle switch */

/* THIRD TYPE: -1: pushbutton inverse */
int digital_input(int card, int input, int *value, int type)
{

  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed, 
		     and -1 if something went wrong */

  int s;
  int bank;

  if (value != NULL) {

    if (card < MAXCARDS) {
      if (sismo[card].connected) {
	if ((input >= 0) && (input < sismo[card].ninputs)) {
	  bank = input/64;
	  /* history slot to read */
	  if (sismo[card].inputs_nsave[bank] != 0) {
	    s = sismo[card].inputs_nsave[bank] - 1;
	  } else {
	    s = 0;
	  }
	  if (type == -1) {
	    /* simple pushbutton / switch (inverse) */
	    if (sismo[card].inputs[input][s] != INPUTINITVAL) {
	      if ((1-*value) != sismo[card].inputs[input][s]) {
		*value = 1-sismo[card].inputs[input][s];
		retval = 1;
		if (verbose > 1) printf("Pushbutton: Card %i Input %i Changed to %i \n",
					card, input, *value);
	      }
	    }

	  } else if (type == 0) {
	    /* simple pushbutton / switch */
	    if (sismo[card].inputs[input][s] != INPUTINITVAL) {
	      if (*value != sismo[card].inputs[input][s]) {
		*value = sismo[card].inputs[input][s];
		retval = 1;
		if (verbose > 1) printf("Pushbutton: Card %i Input %i Changed to %i \n",
					card, input, *value);
	      }
	    }

	  } else if (type == 1) {
	    if (sismo[card].inputs_nsave[bank] != 0) {
	      /* toggle state everytime you press button */
	      if (s < (MAXSAVE-1)) {
		/* check if the switch state changed from 0 -> 1 */
		if ((sismo[card].inputs[input][s] == 1) && (sismo[card].inputs[input][s+1] == 0)) {
		  /* toggle */
		  if (*value != INT_MISS) {
		    if ((*value == 0) || (*value == 1)) {
		      *value = 1 - (*value);
		      retval = 1;
		      if (verbose > 1) printf("Toogle Switch: Card %i Input %i Changed to %i \n",
					      card, input, *value);
		    } else {
		      printf("Toogle Switch: Card %i Input %i Needs to be 0 or 1, but has value %i \n",
					      card, input, *value);
		      retval = -1;
		    }
		  }
		}
	      }
	    }

	  } else {
	    printf("Unknown Digital Input Type %i (only use 0 and 1) \n",type);
	  }
	} else {
	  if (verbose > 0) printf("Digital Input %i above maximum # of inputs %i of card %i \n",
				  input,sismo[card].ninputs,card);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Digital Input %i cannot be read. Card %i not connected \n",
				input,card);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Digital Input %i cannot be read. Card %i >= MAXCARDS\n",input,card);
      retval = -1;
    }
    
  }

  return retval;
}

/* wrapper for digital_output when supplying floating point value 
   Values < 0.15 are set to 0 output and values >= 0.15 are set to 1 output */
int digital_outputf(int card, int output, float *fvalue) {

  int value;
  
  if (*fvalue == FLT_MISS) {
    value = INT_MISS;
  } else if (*fvalue >= 0.025) {
    value = 1;
  } else {
    value = 0;
  }
    
  return digital_output(card, output, &value);
}

int digital_output(int card, int output, int *value)
{
  int retval = 0;

  if (value != NULL) {

    if (card < MAXCARDS) {
      if (sismo[card].connected) {
	if ((output >= 0) && (output < sismo[card].noutputs)) {

	  if (*value != INT_MISS) {
	    if ((*value == 1) || (*value == 0)) {
	      if (*value != sismo[card].outputs[output]) {
		sismo[card].outputs[output] = *value;
		sismo[card].outputs_changed[output] = CHANGED;
		if (verbose > 2) printf("Digital Output %i of card %i changed to %i \n",
					output,card,*value);
	      }
	    } else {
	      printf("Digital Output %i of card %i should be 0 or 1, but is %i \n",
		     output,card,*value);
	      retval = -1;
	    }
	  }
	} else {
	  if (verbose > 0) printf("Digital Output %i above maximum # of outputs %i of card %i \n",
				  output,sismo[card].noutputs,card);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Digital Output %i cannot be written. Card %i not connected \n",
				output,card);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Digital Output %i cannot be written. Card %i >= MAXCARDS\n",output,card);
      retval = -1;
    }

  }

  return retval;
}

/* wrapper for servo_output when supplying integer values */
int servo_output(int card, int servo, int *value, int minval, int maxval)
{
  float fvalue;
  float fminval;
  float fmaxval;

  if (*value == INT_MISS) {
    fvalue = FLT_MISS;
  } else {
    fvalue = (float) *value;
  }

  fminval = (float) minval;
  fmaxval = (float) maxval;
    
  return servo_outputf(card, servo, &fvalue, fminval, fmaxval);
}

int servo_outputf(int card, int servo, float *fvalue, float fminval, float fmaxval)
{
  int retval = 0;
  int data;
  int servominval = 0;
  int servomaxval = 254; // I think that 255 is park position

  if (fvalue != NULL) {

    if (card < MAXCARDS) {
      if (sismo[card].connected) {
	if ((servo >= 0) && (servo < sismo[card].nservos)) {

	  if (*fvalue != FLT_MISS) {
	    data = (int) ((*fvalue - fminval)/(fmaxval - fminval) *
			  ((float) (servomaxval - servominval)) + (float) servominval);
	    if (data > servomaxval) data = servomaxval;
	    if (data < servominval) data = servominval;
	    if (data != sismo[card].servos[servo]) {
	      sismo[card].servos[servo] = data;
	      sismo[card].servos_changed[servo] = CHANGED;
	      if (verbose > 2) printf("Servo %i of card %i changed to %i \n",
				      servo,card,data);
	    }
	  }
	} else {
	  if (verbose > 1) printf("Servo %i above maximum # of servos %i of card %i \n",
	 			  servo,sismo[card].nservos,card);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Servo %i cannot be written. Card %i not connected \n",
				servo,card);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Servo %i cannot be written. Card %i >= MAXCARDS\n",servo,card);
      retval = -1;
    }

  }

  return retval;
}


/* wrapper for floating point output to display */
int display_outputf(int card, int pos, int n, float *fvalue, int dp, int brightness)
{

  int value = INT_MISS;
  if (*fvalue != FLT_MISS) value = (int) lroundf(*fvalue);
  return display_output(card, pos, n, &value, dp, brightness);
}

/* fill 7 segment displays starting from display position pos and the next n displays  */
/* put a decimal point at position dp (or set dp < 0 for no decimal point */
/* brightness goes from 0 .. 15. Set it to 0 for blanking. Brightness can only */
/* be adjusted per display group of 8. Brightness will be adjusted according to the */
/* brightness of the last display in the group. Blanking works per display (range) */
/* addressed in this call */
/* Set dp to -10 and n to 1 to send a binary value to a single 7 segment display, e.g. */
/* to lighten individual segments. See SISMO manual */
int display_output(int card, int pos, int n, int *value, int dp, int brightness)
{
  int retval = 0;
  int tempval;
  int negative;
  int count;
  int single;
  
  if (value != NULL) {

    if (card < MAXCARDS) {
      if (sismo[card].connected) {

	if ((n>0) && (pos>=0) && ((pos+n-1)<sismo[card].ndisplays)) {

	  if (*value != INT_MISS) {
	    if ((brightness > 0) && (brightness <= 15)) {
	      for (count=0;count<n;count++) {
		if (sismo[card].displays_brightness[pos+count] != brightness) {
		  sismo[card].displays_brightness[pos+count] = brightness;
		  if ((n==1) && (dp==-10)) {
		    sismo[card].displays_changed[pos+count] = CHANGEDBINARY;
		  } else {
		    sismo[card].displays_changed[pos+count] = CHANGED;
		  }
		}
	      }
	    }
	    if (brightness == 0) {
	      /* blank all values in range */
	      for (count=0;count<n;count++) {
		if (sismo[card].displays[pos+count] != -1) {
		  sismo[card].displays[pos+count] = -1;
		  sismo[card].displays_changed[pos+count] = CHANGED;
		}
	      }
	    } else {
	      if ((n==1) && (dp==-10)) {
		/* Special Case for single digit driven by binary value 0x00-0xff */
		if (sismo[card].displays[pos] != *value) {
		  sismo[card].displays[pos] = *value;
		  sismo[card].displays_changed[pos] = CHANGEDBINARY;
		}
	      } else {
		/* Regular Case with integer number distributed over n 7-segment displays */
		/* generate temporary storage of input value */
		tempval = *value;

		/* reverse negative numbers: find treatment for - sign */
		/* use first digit for negative sign */
		if (tempval < 0) {
		  tempval = -tempval;
		  negative = 1;
		} else {
		  negative = 0;
		}

		/* read individual digits from integer */
		/* blank leftmost 0 values except if it is the first one */ 
		count = 0;
		while ((tempval) && (count<n))
		  {
		    single = tempval % 10;
		    if (dp == count) single += 10;
		    if (sismo[card].displays[pos+count] != single) {
		      sismo[card].displays[pos+count] = single;
		      sismo[card].displays_changed[pos+count] = CHANGED;
		    }
		    tempval /= 10;
		    count++;
		  }
		while (count<n)
		  {
		    if (negative) {
		      if (count > dp) {
			single = -10;
			if (sismo[card].displays[pos+count] != single) {
			  sismo[card].displays[pos+count] = single;
			  sismo[card].displays_changed[pos+count] = CHANGED;
			}
			negative = 0;
		      }
		    } else {
		      if ((count == 0) || (dp >= count)) {
			/* do not blank leftmost 0 display or if it has a decimal point */
			/* this allows to display for instance 0.04 */
			single = 0;
			if (dp == count) single += 10;
			if (sismo[card].displays[pos+count] != single) {
			  sismo[card].displays[pos+count] = single;
			  sismo[card].displays_changed[pos+count] = CHANGED;
			}
		      } else {
			/* blank all other displays in front of number */
			single = -1;
			if (sismo[card].displays[pos+count] != single) {
			  sismo[card].displays[pos+count] = single;
			  sismo[card].displays_changed[pos+count] = CHANGED;
			}
		      }
		    }
		    count++;
		  }

	      } /* Regular multi-segment or binary display access */
	    } /* do not blank, but print values */

	  }
	} else {
	  if (verbose > 0) printf("Displays %i-%i beyond range of 0-%i of card %i \n",
				  pos,pos+n-1,sismo[card].ndisplays-1,card);
	  retval = -1;

	}
	
      } else {
	if (verbose > 2) printf("Displays %i-%i cannot be written. Card %i not connected \n",
				pos,pos+n-1,card);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Displays %i-%i cannot be written. Card %i >= MAXCARDS\n",pos,pos+n-1,card);
      retval = -1;
    }

  }

  return retval;
}

/* retrieve value from given analog input between the range minval and maxval */
int analog_input(int card, int input, float *value, float minval, float maxval)
{

  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed, 
		     and -1 if something went wrong */
  int found = 0;

  if (value != NULL) {

    if (card < MAXCARDS) {
      if (sismo[card].connected) {
	if ((input >= 0) && (input < sismo[card].nanaloginputs)) {

	  /* Analog input values are flickering with +/-2 (out of a range of 1023)
	     which is because of imprecision of potentiometers etc.
	     A change does not necessarily mean that we turned the potentiometer,
	     so check whether the present value can be found in the history values.
	     if not: we have a real change */
	  /* TBD: maybe check each value against +/- 1 of all history values? */
	  for (int s=1;s<MAXSAVE;s++) {
	    if (sismo[card].analoginputs[input][0] == sismo[card].analoginputs[input][s]) found = 1;
	  }

	  /*
	  printf("%i %i %i %i %i %i %i \n",sismo[card].analoginputs[input][0],
		 sismo[card].analoginputs[input][1],sismo[card].analoginputs[input][2],
		 sismo[card].analoginputs[input][3],sismo[card].analoginputs[input][4],
		 sismo[card].analoginputs[input][5],sismo[card].analoginputs[input][6]
		 ); */
	  
	  *value = ((float) sismo[card].analoginputs[input][0]) / (float) pow(2,ANALOGINPUTNBITS) * 
	    (maxval - minval) + minval;
	    if (!found) {
	      retval = 1;
	    }
	  
	} else {
	  if (verbose > 0) printf("Analog Input %i above maximum # of analog inputs %i of card %i \n",
				  input,sismo[card].nanaloginputs,card);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Analog Input %i cannot be read. Card %i not connected \n",
				input,card);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Analog Input %i cannot be read. Card %i >= MAXCARDS\n",input,card);
      retval = -1;
    }
    
  }

  return retval;
}


/* wrapper for encoder_input using integer values and multiplier */
int encoder_input(int card, int input1,  int input2, int *value, int multiplier, int type)
{
  float fvalue = FLT_MISS;
  if (*value != INT_MISS) fvalue = (float) *value;
  int ret = encoder_inputf(card, input1, input2, &fvalue, (float) multiplier, type);
  if (fvalue != FLT_MISS) *value = (int) lroundf(fvalue);
  return ret;
    
}

/* retrieve encoder value and for given encoder type connected to inputs 1 and 2
   inputs 1 and 2 need to be on the same input bank (each bank has 64 inputs).
   Note that even though we try to capture every bit that changes, turning an 
   optical encoder too fast will result in loss of states since the SISMO card
   will not capture every state change correctly */
/* two types of encoders: */
/* 1: 2 bit optical rotary encoder (type 3 in xpusb) */
/* 2: 2 bit gray type mechanical encoder */
int encoder_inputf(int card, int input1, int input2, float *value, float multiplier, int type)
{

  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed, 
		     and -1 if something went wrong */

  char oldcount, newcount; /* encoder integer counters */
  char updown = 0; /* encoder direction */
  char obits[2]; /* bit arrays for 2 bit encoder */
  char nbits[2]; /* bit arrays for 2 bit encoder */
  int s;
  int bank;
  struct timeval newtime;
  float dt; /* time interval since last encoder read in milliseconds */

  if (value != NULL) {

    if (card < MAXCARDS) {
      if (sismo[card].connected) {
	if ((input1 >= 0) && (input1 < sismo[card].ninputs) &&
	    (input2 >= 0) && (input2 < sismo[card].ninputs)) {
	  if (input1/64 == input2/64) {

	    if (*value != FLT_MISS) {
	    
	      bank = input1/64;
	      if (sismo[card].inputs_nsave[bank] != 0) {
		s = sismo[card].inputs_nsave[bank] - 1; /* history slot to read */
		if (s < (MAXSAVE-1)) {
		  /* if not first read, and if any of the inputs have changed then the encoder was moved */
		  if (((sismo[card].inputs[input1][s] != sismo[card].inputs[input1][s+1]) ||
		       (sismo[card].inputs[input2][s] != sismo[card].inputs[input2][s+1])) &&
		      (sismo[card].inputs[input1][s+1] != INPUTINITVAL) &&
		      (sismo[card].inputs[input2][s+1] != INPUTINITVAL)) {

		    gettimeofday(&newtime,NULL);
		    dt = ((newtime.tv_sec - sismo[card].inputs_time[input1].tv_sec) +
			  (newtime.tv_usec - sismo[card].inputs_time[input1].tv_usec) / 1000000.0)*1000.0;
		  
		    if (type == 1) {
		      /* 2 bit optical encoder */

		      /* derive last encoder bit state */
		      obits[0] = sismo[card].inputs[input1][s+1];
		      obits[1] = sismo[card].inputs[input2][s+1];
		      /* derive new encoder bit state */
		      nbits[0] = sismo[card].inputs[input1][s];
		      nbits[1] = sismo[card].inputs[input2][s];
		      
		      if ((obits[0] == 0) && (obits[1] == 1) && (nbits[0] == 0) && (nbits[1] == 0)) {
			updown = 1;
		      } else if ((obits[0] == 0) && (obits[1] == 1) && (nbits[0] == 1) && (nbits[1] == 1)) {
			updown = -1;
		      } else if ((obits[0] == 1) && (obits[1] == 0) && (nbits[0] == 1) && (nbits[1] == 1)) {
			updown = 1;
		      } else if ((obits[0] == 1) && (obits[1] == 0) && (nbits[0] == 0) && (nbits[1] == 0)) {
			updown = -1;
			/* in case a) an input was missed or b) the two optical rotary inputs were transmitted 
			   concurrently due to timing issue, assume that the last direction is still valid.
			   ONLY USE WITH FAST TURNING DT < 100 ms */
		      } else if ((obits[0] == 1) && (obits[1] == 1) && (nbits[0] == 0) && (nbits[1] == 0)) {
			if (dt < 300.0) updown = sismo[card].inputs_updown[input1];
		      } else if ((obits[0] == 0) && (obits[1] == 0) && (nbits[0] == 1) && (nbits[1] == 1)) {
			if (dt < 300.0) updown = sismo[card].inputs_updown[input1];
		      }

		      if (updown != 0) {
			/* Update last encoder time with current postion after successful detent */
			sismo[card].inputs_time[input1].tv_sec = newtime.tv_sec;
			sismo[card].inputs_time[input1].tv_usec = newtime.tv_usec;
		      }
			
		      //printf("updn: %i %i %i %i %i %i %f \n",updown,s,obits[0],obits[1],nbits[0],nbits[1],dt);
		      //printf("%i %i %f \n",nbits[0],nbits[1],dt);
		      
		      if (updown != 0) {
			/* ADD ACCELERATION WITH SPEED OF TURNING ENCODER */
			*value = *value + ((float) updown)  * multiplier * (float) (1 + (int) (60.0/max(dt,1.0)));
			/* NO ACCELERATION WITH TURNING SPEED */
			//*value = *value + ((float) updown)  * multiplier;
			retval = 1;

			//printf("UPDN %i %f \n",updown,*value);
			
			/* store last updown value for later use */
			sismo[card].inputs_updown[input1] = updown;
		      }

		      
		    } else if (type == 2) {
		      /* 2 bit gray type mechanical encoder */

		      /* derive last encoder count */
		      oldcount = sismo[card].inputs[input1][s+1]+2*sismo[card].inputs[input2][s+1];
	  
		      /* derive new encoder count */
		      newcount = sismo[card].inputs[input1][s]+2*sismo[card].inputs[input2][s];

		      /* forward */
		      if (((oldcount == 0) && (newcount == 1)) ||
			  ((oldcount == 1) && (newcount == 3)) ||
			  ((oldcount == 3) && (newcount == 2)) ||
			  ((oldcount == 2) && (newcount == 0))) {
			updown = -1;
		      }
		    
		      /* backward */
		      if (((oldcount == 2) && (newcount == 3)) ||
			  ((oldcount == 3) && (newcount == 1)) ||
			  ((oldcount == 1) && (newcount == 0)) ||
			  ((oldcount == 0) && (newcount == 2))) {
			updown = 1;
		      }
		    
		      if (updown != 0) {
			/* add accelerator by using s as number of queued encoder changes */
			*value = *value + ((float) updown) * multiplier * (float) (s*2+1);
			retval = 1;
		      }		    
		    } else {
		      if (verbose > 0) printf("Encoder with Input %i,%i of card %i need to be of type 1 or 2 \n",
					      input1,input2,card);
		      retval = 1;
		    }
		  }
		}
	      }
	      if ((retval == 1) && (verbose > 2)) printf("Encoder with Input %i,%i of card %i changed to %f \n",
							 input1,input2,card,*value);
	    }
	    
	  } else {
	    if (verbose > 0) printf("Encoder with Input %i,%i need to be on same input bank of card %i \n",
				    input1,input2,card);
	    retval = -1;
	  }
	} else {
	  if (verbose > 0) printf("Encoder with Input %i,%i above maximum # of digital inputs %i of card %i \n",
				  input1,input2,sismo[card].ninputs,card);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Encoder with Input %i,%i cannot be read. Card %i not connected \n",
				input1,input2,card);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Encoder with Input %i,%i cannot be read. Card %i >= MAXCARDS\n",input1,input2,card);
      retval = -1;
    }
    
  }

  return retval;
}

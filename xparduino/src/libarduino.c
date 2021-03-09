/* This is the libarduino.c code which contains all functions to interact with
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
#include "libarduino.h"
#include "handleudp.h"
#include "serverdata.h"

int verbose;

//char xpserver_ip[30];
//int xpserver_port;
//char clientname[100];

char arduinoserver_ip[30];
int arduinoserver_port;

unsigned char arduinoRecvBuffer[RECVMSGLEN];
unsigned char arduinoSendBuffer[SENDMSGLEN];

int nards;
arduino_struct arduino[MAXCARDS];


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

int read_arduino() {
  
  int ard;
  int i,b,s;
  int val;
  int input;
  char any;
  int bank;
  int firstinput;
  int ninputs;
  int firstbyte;

  //printf("Packets left to read %i \n",udpReadLeft/RECVMSGLEN);
  
  while (udpReadLeft >= RECVMSGLEN) {
    
    ard = -1;
    
    /* empty UDP receive buffer instead of directly accessing the device */

    pthread_mutex_lock(&exit_cond_lock);    
    /* read from start of receive buffer */
    memcpy(arduinoRecvBuffer,&udpRecvBuffer[0],RECVMSGLEN);
    /* shift remaining read buffer to the left */
    memmove(&udpRecvBuffer[0],&udpRecvBuffer[RECVMSGLEN],udpReadLeft-RECVMSGLEN);    
    /* decrease read buffer position and counter */
    udpReadLeft -= RECVMSGLEN;
    pthread_mutex_unlock(&exit_cond_lock);

    /* decode message */

    /* check init string */
    if ((arduinoRecvBuffer[0] == 0x53) && (arduinoRecvBuffer[1] == 0x43)) {
      /* check MAC Address and Port of Card which did send the message */
      
      for (i=0;i<MAXCARDS;i++) {
	if ((arduino[i].mac[0] == arduinoRecvBuffer[2]) && (arduino[i].mac[1] == arduinoRecvBuffer[3]) &&
	    (arduino[i].port == arduinoRecvBuffer[6] + 256 * arduinoRecvBuffer[7])) ard = i;
      }
      
      /* ard found */
      if (ard >= 0) {

	if (arduino[ard].connected == 0) {
	  /* initialize ard configuration status with information we read from the UDP packet */
	  arduino[ard].connected = 1;
	  
	  /* set # of inputs/outputs depending on activated daughters */
	  arduino[ard].daughter_output1 = get_bit(arduinoRecvBuffer[5],0);
	  arduino[ard].daughter_output2 = get_bit(arduinoRecvBuffer[5],1);
	  arduino[ard].daughter_servo = get_bit(arduinoRecvBuffer[5],2);
	  arduino[ard].daughter_display1 = get_bit(arduinoRecvBuffer[5],3);
	  arduino[ard].daughter_analogoutput = get_bit(arduinoRecvBuffer[5],4);
	  arduino[ard].daughter_analoginput = get_bit(arduinoRecvBuffer[5],5);
	  arduino[ard].daughter_display2 = get_bit(arduinoRecvBuffer[5],6);
	  
	  arduino[ard].noutputs = 64 + 64*arduino[ard].daughter_output1 + 64*arduino[ard].daughter_output2;
	  arduino[ard].nservos = 14*arduino[ard].daughter_servo;
	  arduino[ard].ndisplays = 32 + 32*arduino[ard].daughter_display1 + 32*arduino[ard].daughter_display2;
	  arduino[ard].nanaloginputs = 5 + 10*arduino[ard].daughter_analoginput;
	  arduino[ard].nanalogoutputs = 0*arduino[ard].daughter_analogoutput; /* Planned, but not available */

	  if (verbose > 0) {
	    printf("\n");
	    printf("SISMO Card %i is connected with the following features: \n",ard);
	    printf("Daughter Outputs 1:     %i\n",arduino[ard].daughter_output1);
	    printf("Daughter Outputs 2:     %i\n",arduino[ard].daughter_output2);
	    printf("Daughter Displays 1:    %i\n",arduino[ard].daughter_display1);
	    printf("Daughter Displays 2:    %i\n",arduino[ard].daughter_display2);
	    printf("Daughter Servo:         %i\n",arduino[ard].daughter_servo);
	    printf("Daughter Analog Input:  %i\n",arduino[ard].daughter_analoginput);
	    printf("Daughter Analog Output: %i\n",arduino[ard].daughter_analogoutput);
	    printf("\n");
	    printf("This ard thus has the following capabilities: \n");
	    printf("Number of Inputs:     64..%i\n",arduino[ard].ninputs);
	    printf("Number of Outputs:        %i\n",arduino[ard].noutputs);
	    printf("Number of Displays:       %i\n",arduino[ard].ndisplays);
	    printf("Number of Servos:         %i\n",arduino[ard].nservos);
	    printf("Number of Analog Inputs:  %i\n",arduino[ard].nanaloginputs);
	    printf("Number of Analog Outputs: %i\n",arduino[ard].nanalogoutputs);
	    printf("\n");
	    printf("\n");
	  }
	  
	}

	if (verbose > 3) printf("Read %i bytes from SISMO Card %i \n",RECVMSGLEN,ard);
	
	/* check type of input */
	if ((arduinoRecvBuffer[4] == 0x00) || (arduinoRecvBuffer[4] == 0x01) ||
	    (arduinoRecvBuffer[4] == 0x02)) {

	  /* Digital Input from Master (Inputs 0-63) */
	  if (arduinoRecvBuffer[4] == 0x00) {
	    bank = 0;
	    firstinput = 0;
	  }
	  /* Digital Input from Daughter Digital Input 1 (Inputs 64-127) */
	  if (arduinoRecvBuffer[4] == 0x01) {
	    bank = 1;
	    firstinput = 64;
	  }
	  /* Digital Input from Daughter Digital Input 2 (Inputs 128-191) */
	  if (arduinoRecvBuffer[4] == 0x02) {
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
	      val = get_bit(arduinoRecvBuffer[8+b],i);
	      if (val != arduino[ard].inputs[input][0]) any = 1;
	    }
	  }
	  /* if any input changed make sure we capture the whole history of changed
	     inputs, since during a single reception we can have several state changes */
	  if (any) {
	    //	    printf("%i %i \n",get_bit(arduinoRecvBuffer[8+0],0),get_bit(arduinoRecvBuffer[8+0],1));
	    for (b=0;b<8;b++) {
	      for (i=0;i<8;i++) {
		input = b*8+i + firstinput;
		/* shift all inputs in history array by one */
		for (s=MAXSAVE-2;s>=0;s--) {
		  arduino[ard].inputs[input][s+1] = arduino[ard].inputs[input][s];
		}
		/* update input if changed */
		val = get_bit(arduinoRecvBuffer[8+b],i);
		if (val != arduino[ard].inputs[input][0]) {
		  arduino[ard].inputs[input][0] = val; 
		  if (verbose > 2) printf("Card %i Input %i Changed to: %i \n",ard,input,val);
		}
	      }
	    }
	    /* update number of history values per input bank */
	    if (arduino[ard].inputs_nsave[bank] < MAXSAVE) {
	      arduino[ard].inputs_nsave[bank] += 1;
	      if (verbose > 2) printf("Card %i Input Bank %i # of History Values %i \n",
				      ard,bank,arduino[ard].inputs_nsave[bank]);
	    } else {
	      if (verbose > 0) printf("Card %i Input Bank %i Maximum # of History Values %i Reached \n",
				      ard,bank,MAXSAVE);
	    }
	      
	  }

	}
	  
	if ((arduinoRecvBuffer[4] == 0x00) || (arduinoRecvBuffer[4] == 0x03)) {

	  /* Analog Input from Master (Inputs 0-4) */
	  if (arduinoRecvBuffer[4] == 0x00) {
	    firstinput = 0;
	    ninputs = 5;
	    firstbyte = 16;
	  }
	  /* Analog Inputs from Analog Input Daughter (Inputs 5-15) */
	  if (arduinoRecvBuffer[4] == 0x03) {
	    firstinput = 5;
	    ninputs = 11;
	    firstbyte = 8;
	  }

	  /* Analog Inputs are ordered in 10 bytes with 2 bytes per input */
	  /* Located in bytes 16-25 */
	  for (input=0;input<ninputs;input++) {
	    val = arduinoRecvBuffer[firstbyte+input*2] + 256 * arduinoRecvBuffer[firstbyte+1+input*2];
	    /* shift all inputs in history array by one */
	    for (s=MAXSAVE-2;s>=0;s--) {
	      arduino[ard].analoginputs[input+firstinput][s+1] = arduino[ard].analoginputs[input+firstinput][s];
	    }
	    if (val != arduino[ard].analoginputs[input+firstinput][0]) {
	      if (verbose > 2) printf("Card %i Analog Input %i Changed to %i \n",
				      ard,input+firstinput,arduino[ard].analoginputs[input+firstinput][0]);
	    }
	    arduino[ard].analoginputs[input+firstinput][0] = val;			      
	  }
	  
	}
	
	//printf("Left to Read: %i \n",udpReadLeft);
	
      } else {
	printf("Card with MAC %02x:%02x or Port %i is not defined in ini file \n",
	       arduinoRecvBuffer[2],arduinoRecvBuffer[3],arduinoRecvBuffer[6] + 256 * arduinoRecvBuffer[7]);
      }
    } else {
      printf("Received wrong Init String: %02x %02x \n",arduinoRecvBuffer[0],arduinoRecvBuffer[1]);
    }

  } /* while UDP data present in receive buffer */

  return 0;
}

int write_arduino() {

  int ret;
  int ard;
  int output;
  int group;
  int display;
  int anychanged;
  int bank; /* outputs bank: 0: master, 1: daughter 1, 2: daughter 2 */
  int firstoutput;
  int firstdisplay;
  int connected;
  char sendto;

  for (ard=0;ard<MAXCARDS;ard++) {
    if (arduino[ard].connected == 1) {
      
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
	  connected = arduino[ard].daughter_output1;
	  firstoutput = 64;
	  sendto = 1;
	}
	if (bank==2) {
	  /* daughter 2 */
	  connected = arduino[ard].daughter_output2;
	  firstoutput = 192;
	  sendto = 2;
	}

	if (connected) {
	  anychanged = 0;
	  memset(arduinoSendBuffer,0,SENDMSGLEN);
	  arduinoSendBuffer[0] = 0x53;
	  arduinoSendBuffer[1] = 0x43;
	  arduinoSendBuffer[2] = sendto;
	  arduinoSendBuffer[3] = 0x00;
	  for (output=0;output<64;output++) {
	    set_bit(&arduinoSendBuffer[4+output/8],output%8,arduino[ard].outputs[output+firstoutput]);
	    if (arduino[ard].outputs_changed[output+firstoutput] == CHANGED) {
	      if (verbose > 2) printf("Card %i Output %i changed to: %i \n",
				      ard,output,arduino[ard].outputs[output+firstoutput]);
	      anychanged = 1;
	      /* reset changed state since data will be sent to SISMO ard */
	      arduino[ard].outputs_changed[output+firstoutput] = UNCHANGED;
	    }
	  }
	  if (anychanged) {
	    ret = send_udp(arduino[ard].ip,arduino[ard].port,arduinoSendBuffer,SENDMSGLEN);
	    if (verbose > 1) printf("Sent %i bytes to ard %i \n", ret,ard);
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
	  connected = arduino[ard].daughter_display1;
	  firstdisplay = 32;
	  sendto = 4;
	}
	if (bank==2) {
	  /* daughter 2 */
	  connected = arduino[ard].daughter_display2;
	  firstdisplay = 64;
	  sendto = 5;
	}

	if (connected) {
	  for (group=0;group<4;group++) {
	    anychanged = 0;
	    memset(arduinoSendBuffer,0,SENDMSGLEN);
	    arduinoSendBuffer[0] = 0x53;
	    arduinoSendBuffer[1] = 0x43;
	    arduinoSendBuffer[2] = sendto;
	    arduinoSendBuffer[3] = 0x01;
	    arduinoSendBuffer[4] = group+1;
	    arduinoSendBuffer[13] = DISPLAYBRIGHTNESS; /* Todo: make user-adjustable */
	    for (display=0;display<8;display++) {
	      set_7segment(&arduinoSendBuffer[5+display],arduino[ard].displays[display+group*8]);
	      if (arduino[ard].displays_changed[display+group*8+firstdisplay] == CHANGED) {
		if (verbose > 2) printf("Card %i Display %i changed to: %i \n",ard,display+group*8,
					arduino[ard].displays[display+group*8+firstdisplay]);
		anychanged = 1;
		/* reset changed state since data will be sent to SISMO ard */
		arduino[ard].displays_changed[display+group*8+firstdisplay] = UNCHANGED;
	      }
	    }
	    if (anychanged) {
	      ret = send_udp(arduino[ard].ip,arduino[ard].port,arduinoSendBuffer,SENDMSGLEN);
	      if (verbose > 1) printf("Sent %i bytes to ard %i \n", ret,ard);
	    }
	  }

	}
      }

      /* check if daughter servos have changed */
     
    }
  }
  
  return 0;
}


/* wrapper for digital_input with floating point values */
int digital_inputf(int ard, int input, float *fvalue, int type)
{
  int value = INT_MISS;
  if (*fvalue != FLT_MISS) value = (int) lroundf(*fvalue);
  int ret = digital_input(ard, input, &value, type);
  if (value != INT_MISS) *fvalue = (float) value;
  return ret;
    
}

/* retrieve input value from given input position of SISMO ard */
/* master ard has 64 inputs (0..63)
   daughter ard 1 and 2 have another 64 inputs (64..127 and 128..191) */
/* Two types : */
/* 0: pushbutton */
/* 1: toggle switch */
int digital_input(int ard, int input, int *value, int type)
{

  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed, 
		     and -1 if something went wrong */

  int s;
  int bank;

  if (value != NULL) {

    if (ard < MAXCARDS) {
      if (arduino[ard].connected) {
	if ((input >= 0) && (input < arduino[ard].ninputs)) {
	  bank = input/64;
	  
	  if (arduino[ard].inputs_nsave[bank] != 0) {
	    s = arduino[ard].inputs_nsave[bank] - 1; /* history slot to read */
	    if (type == 0) {
	      /* simple pushbutton / switch */
	      if (*value != arduino[ard].inputs[input][s]) {
		*value = arduino[ard].inputs[input][s];
		retval = 1;
		if (verbose > 1) printf("Pushbutton: Card %i Input %i Changed to %i \n",
					ard, input, *value);
	      }

	    } else {
	      /* toggle state everytime you press button */
	      if (s < (MAXSAVE-1)) {
		/* check if the switch state changed from 0 -> 1 */
		if ((arduino[ard].inputs[input][s] == 1) && (arduino[ard].inputs[input][s+1] == 0)) {
		  /* toggle */
		  if (*value != INT_MISS) {
		    if ((*value == 0) || (*value == 1)) {
		      *value = 1 - (*value);
		      retval = 1;
		      if (verbose > 1) printf("Toogle Switch: Card %i Input %i Changed to %i \n",
					      ard, input, *value);
		    } else {
		      printf("Toogle Switch: Card %i Input %i Needs to be 0 or 1, but has value %i \n",
					      ard, input, *value);
		      retval = -1;
		    }
		  }
		}
	      }
	    }

	  }
	} else {
	  if (verbose > 0) printf("Digital Input %i above maximum # of inputs %i of ard %i \n",
				  input,arduino[ard].ninputs,ard);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Digital Input %i cannot be read. Card %i not connected \n",
				input,ard);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Digital Input %i cannot be read. Card %i >= MAXCARDS\n",input,ard);
      retval = -1;
    }
    
  }

  return retval;
}

/* wrapper for digital_output when supplying floating point value 
   Values < 0.5 are set to 0 output and values >= 0.5 are set to 1 output */
int digital_outputf(int ard, int output, float *fvalue) {

  int value;
  
  if (*fvalue == FLT_MISS) {
    value = INT_MISS;
  } else if (*fvalue >= 0.5) {
    value = 1;
  } else {
    value = 0;
  }
    
  return digital_output(ard, output, &value);
}

int digital_output(int ard, int output, int *value)
{
  int retval = 0;

  if (value != NULL) {

    if (ard < MAXCARDS) {
      if (arduino[ard].connected) {
	if ((output >= 0) && (output < arduino[ard].noutputs)) {

	  if (*value != INT_MISS) {
	    if ((*value == 1) || (*value == 0)) {
	      if (*value != arduino[ard].outputs[output]) {
		arduino[ard].outputs[output] = *value;
		arduino[ard].outputs_changed[output] = CHANGED;
		if (verbose > 2) printf("Digital Output %i of ard %i changed to %i \n",
					output,ard,*value);
	      }
	    } else {
	      printf("Digital Output %i of ard %i should be 0 or 1, but is %i \n",
		     output,ard,*value);
	      retval = -1;
	    }
	  }
	} else {
	  if (verbose > 0) printf("Digital Output %i above maximum # of outputs %i of ard %i \n",
				  output,arduino[ard].noutputs,ard);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Digital Output %i cannot be written. Card %i not connected \n",
				output,ard);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Digital Ouputt %i cannot be written. Card %i >= MAXCARDS\n",output,ard);
      retval = -1;
    }

  }

  return retval;
}


/* wrapper for floating point output to display */
int display_outputf(int ard, int pos, int n, float *fvalue, int dp, int blank)
{

  int value = INT_MISS;
  if (*fvalue != FLT_MISS) value = (int) lroundf(*fvalue);
  return display_output(ard, pos, n, &value, dp, blank);
}

/* fill 7 segment displays starting from display position pos and the next n displays  */
/* put a decimal point at position dp (or set dp < 0 for no decimal point */
/* set blank to 1 if you want to blank the range pos to pos+n-1 */
int display_output(int ard, int pos, int n, int *value, int dp, int blank)
{
  int retval = 0;
  int tempval;
  int negative;
  int count;
  int single;
  
  if (value != NULL) {

    if (ard < MAXCARDS) {
      if (arduino[ard].connected) {

	if ((n>0) && (pos>=0) && ((pos+n-1)<arduino[ard].ndisplays)) {

	  if (*value != INT_MISS) {
	  
	    if (blank == 1) {
	      /* blank all values in range */
	      for (count=0;count<n;count++) {
		if (arduino[ard].displays[pos+count] != -1) {
		  arduino[ard].displays[pos+count] = -1;
		  arduino[ard].displays_changed[pos+count] = CHANGED;
		}
	      }
	    } else {
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
		  if (arduino[ard].displays[pos+count] != single) {
		    arduino[ard].displays[pos+count] = single;
		    arduino[ard].displays_changed[pos+count] = CHANGED;
		  }
		  tempval /= 10;
		  count++;
		}
	      while (count<n)
		{
		  if (negative) {
		    if (count > dp) {
		      single = -10;
		      if (arduino[ard].displays[pos+count] != single) {
			arduino[ard].displays[pos+count] = single;
			arduino[ard].displays_changed[pos+count] = CHANGED;
		      }
		      negative = 0;
		    }
		  } else {
		    if ((count == 0) || (dp >= count)) {
		      /* do not blank leftmost 0 display or if it has a decimal point */
		      /* this allows to display for instance 0.04 */
		      single = 0;
		      if (dp == count) single += 10;
		      if (arduino[ard].displays[pos+count] != single) {
			arduino[ard].displays[pos+count] = single;
			arduino[ard].displays_changed[pos+count] = CHANGED;
		      }
		    } else {
		      /* blank all other displays in front of number */
		      single = -1;
		      if (arduino[ard].displays[pos+count] != single) {
			arduino[ard].displays[pos+count] = single;
			arduino[ard].displays_changed[pos+count] = CHANGED;
		      }
		    }
		  }
		  count++;
		}

	    } /* do not blank, but print values */

	  }
	} else {
	  if (verbose > 0) printf("Displays %i-%i beyond range of 0-%i of ard %i \n",
				  pos,pos+n-1,arduino[ard].ndisplays-1,ard);
	  retval = -1;

	}
	
      } else {
	if (verbose > 2) printf("Displays %i-%i cannot be written. Card %i not connected \n",
				pos,pos+n-1,ard);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Displays %i-%i cannot be written. Card %i >= MAXCARDS\n",pos,pos+n-1,ard);
      retval = -1;
    }

  }

  return retval;
}

/* retrieve value from given analog input between the range minval and maxval */
int analog_input(int ard, int input, float *value, float minval, float maxval)
{

  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed, 
		     and -1 if something went wrong */
  int found = 0;

  if (value != NULL) {

    if (ard < MAXCARDS) {
      if (arduino[ard].connected) {
	if ((input >= 0) && (input < arduino[ard].nanaloginputs)) {

	  /* Analog input values are flickering with +/-2 (out of a range of 1023)
	     which is because of imprecision of potentiometers etc.
	     A change does not necessarily mean that we turned the potentiometer,
	     so check whether the present value can be found in the history values.
	     if not: we have a real change */
	  /* TBD: maybe check each value against +/- 1 of all history values? */
	  for (int s=1;s<MAXSAVE;s++) {
	    if (arduino[ard].analoginputs[input][0] == arduino[ard].analoginputs[input][s]) found = 1;
	  }

	  /*
	  printf("%i %i %i %i %i %i %i \n",arduino[ard].analoginputs[input][0],
		 arduino[ard].analoginputs[input][1],arduino[ard].analoginputs[input][2],
		 arduino[ard].analoginputs[input][3],arduino[ard].analoginputs[input][4],
		 arduino[ard].analoginputs[input][5],arduino[ard].analoginputs[input][6]
		 ); */
	  
	  if (!found) {
	    *value = ((float) arduino[ard].analoginputs[input][0]) / (float) pow(2,ANALOGINPUTNBITS) * 
	    (maxval - minval) + minval;
	    retval = 1;
	  }
	  
	} else {
	  if (verbose > 0) printf("Analog Input %i above maximum # of analog inputs %i of ard %i \n",
				  input,arduino[ard].nanaloginputs,ard);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Analog Input %i cannot be read. Card %i not connected \n",
				input,ard);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Analog Input %i cannot be read. Card %i >= MAXCARDS\n",input,ard);
      retval = -1;
    }
    
  }

  return retval;
}


/* wrapper for encoder_input using integer values and multiplier */
int encoder_input(int ard, int input1,  int input2, int *value, int multiplier, int type)
{
  float fvalue = FLT_MISS;
  if (*value != INT_MISS) fvalue = (float) *value;
  int ret = encoder_inputf(ard, input1, input2, &fvalue, (float) multiplier, type);
  if (fvalue != FLT_MISS) *value = (int) lroundf(fvalue);
  return ret;
    
}

/* retrieve encoder value and for given encoder type connected to inputs 1 and 2
   inputs 1 and 2 need to be on the same input bank (each bank has 64 inputs).
   Note that even though we try to capture every bit that changes, turning an 
   optical encoder too fast will result in loss of states since the SISMO ard
   will not capture every state change correctly */
/* two types of encoders: */
/* 1: 2 bit optical rotary encoder (type 3 in xpusb) */
/* 2: 2 bit gray type mechanical encoder */
int encoder_inputf(int ard, int input1, int input2, float *value, float multiplier, int type)
{

  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed, 
		     and -1 if something went wrong */

  char oldcount, newcount; /* encoder integer counters */
  char updown = 0; /* encoder direction */
  char obits[2]; /* bit arrays for 2 bit encoder */
  char nbits[2]; /* bit arrays for 2 bit encoder */
  int s;
  int bank;

  if (value != NULL) {

    if (ard < MAXCARDS) {
      if (arduino[ard].connected) {
	if ((input1 >= 0) && (input1 < arduino[ard].ninputs) &&
	    (input2 >= 0) && (input2 < arduino[ard].ninputs)) {
	  if (input1/64 == input2/64) {

	    if (*value != FLT_MISS) {
	    
	      bank = input1/64;
	      if (arduino[ard].inputs_nsave[bank] != 0) {
		s = arduino[ard].inputs_nsave[bank] - 1; /* history slot to read */
		if (s < (MAXSAVE-1)) {
		  /* if not first read, and if any of the inputs have changed then the encoder was moved */
		  if (((arduino[ard].inputs[input1][s] != arduino[ard].inputs[input1][s+1]) ||
		       (arduino[ard].inputs[input2][s] != arduino[ard].inputs[input2][s+1])) &&
		      (arduino[ard].inputs[input1][s+1] != INPUTINITVAL) &&
		      (arduino[ard].inputs[input2][s+1] != INPUTINITVAL)) {
		  
		    if (type == 1) {
		      /* 2 bit optical encoder */

		      /* derive last encoder bit state */
		      obits[0] = arduino[ard].inputs[input1][s+1];
		      obits[1] = arduino[ard].inputs[input2][s+1];
		      /* derive new encoder bit state */
		      nbits[0] = arduino[ard].inputs[input1][s];
		      nbits[1] = arduino[ard].inputs[input2][s];
		      
		      if ((obits[0] == 0) && (obits[1] == 1) && (nbits[0] == 0) && (nbits[1] == 0)) {
			updown = -1;
		      } else if ((obits[0] == 0) && (obits[1] == 1) && (nbits[0] == 1) && (nbits[1] == 1)) {
			updown = 1;
		      } else if ((obits[0] == 1) && (obits[1] == 0) && (nbits[0] == 1) && (nbits[1] == 1)) {
			updown = -1;
		      } else if ((obits[0] == 1) && (obits[1] == 0) && (nbits[0] == 0) && (nbits[1] == 0)) {
			updown = 1;
		      }
	  
		      if (updown != 0) {
			/* add accelerator by using s as number of queued encoder changes */
			*value = *value + ((float) updown)  * multiplier * (float) (s*2+1);
			retval = 1;
		      }		    
		    } else if (type == 2) {
		      /* 2 bit gray type mechanical encoder */

		      /* derive last encoder count */
		      oldcount = arduino[ard].inputs[input1][s+1]+2*arduino[ard].inputs[input2][s+1];
	  
		      /* derive new encoder count */
		      newcount = arduino[ard].inputs[input1][s]+2*arduino[ard].inputs[input2][s];

		      /* forward */
		      if (((oldcount == 0) && (newcount == 1)) ||
			  ((oldcount == 1) && (newcount == 3)) ||
			  ((oldcount == 3) && (newcount == 2)) ||
			  ((oldcount == 2) && (newcount == 0))) {
			updown = 1;
		      }
		    
		      /* backward */
		      if (((oldcount == 2) && (newcount == 3)) ||
			  ((oldcount == 3) && (newcount == 1)) ||
			  ((oldcount == 1) && (newcount == 0)) ||
			  ((oldcount == 0) && (newcount == 2))) {
			updown = -1;
		      }
		    
		      if (updown != 0) {
			/* add accelerator by using s as number of queued encoder changes */
			*value = *value + ((float) updown) * multiplier * (float) (s+1);
			retval = 1;
		      }		    
		    } else {
		      if (verbose > 0) printf("Encoder with Input %i,%i of ard %i need to be of type 1 or 2 \n",
					      input1,input2,ard);
		      retval = -1;
		    }
		  }
		}
	      }
	      if ((retval == 1) && (verbose > 2)) printf("Encoder with Input %i,%i of ard %i changed to %f \n",
							 input1,input2,ard,*value);
	    }
	    
	  } else {
	    if (verbose > 0) printf("Encoder with Input %i,%i need to be on same input bank of ard %i \n",
				    input1,input2,ard);
	    retval = -1;
	  }
	} else {
	  if (verbose > 0) printf("Encoder with Input %i,%i above maximum # of digital inputs %i of ard %i \n",
				  input1,input2,arduino[ard].ninputs,ard);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Encoder with Input %i,%i cannot be read. Card %i not connected \n",
				input1,input2,ard);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Encoder with Input %i,%i cannot be read. Card %i >= MAXCARDS\n",input1,input2,ard);
      retval = -1;
    }
    
  }

  return retval;
}

// Client side implementation of UDP client-server model 


#include <math.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <signal.h>
#include <sys/time.h>

#include "handleudp.h"

volatile sig_atomic_t stop;

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

void inthand(int signum) {
  stop = 1;
}

// Driver code 
int main() {
  int ret;
  int i,b;
  int val;
  
  int num_ana = 5;
  int num_inp = 64;
  int num_out = 64;
  int num_dis = 32;
  int ana[num_ana];
  int inp[num_inp];
  int out[num_out];
  int dis[num_dis];
  
  struct timeval tval_before, tval_after, tval_result;

  stop = 0;
  signal(SIGINT, inthand);
  
  udpSendBufferLen = 28;
  udpRecvBufferLen = 30;

  /* The SISMO MB */
  strcpy(udpClientIP,"192.168.1.150");
  udpClientPort = 1024;
  unsigned char initstring[2] = "SC";
  unsigned char sendto_mb = 0x00; // Mother Board
  unsigned char outstype_out = 0x00; // Digital Outputs
  unsigned char outstype_dis = 0x01; // 7 Segment Displays
  
  /* Our Computer */
  strcpy(udpServerIP,"192.168.1.10");
  udpServerPort = 1026;

  if (init_udp() < 0) {
    return(-1);
  }


  // Try sending data first, then listen

  /* Send Outputs */
  memset(udpSendBuffer,0,udpSendBufferLen);

  udpSendBuffer[0] = initstring[0];
  udpSendBuffer[1] = initstring[1];
  udpSendBuffer[2] = sendto_mb;
  udpSendBuffer[3] = outstype_out;


  i=2; // set output number
  val=1; // set output value
  //  udpSendBuffer[4+(i-1)/8] = 0xff;
  set_bit(&udpSendBuffer[4+(i-1)/8],(i-1)%8,val);
  ret = send_udp();

  printf("Sent %i bytes \n", ret);
 
  while (!stop) {

    gettimeofday(&tval_before, NULL);
 
    ret = recv_udp();
      
    if (ret > 0) {

      printf("%i \n",ret);
      
     /*
      printf("N: %i INIT: %c %c \n",ret,udpRecvBuffer[0],udpRecvBuffer[1]);
      printf("MAC %02x:%02x\n",udpRecvBuffer[2],udpRecvBuffer[3]);
      printf("Inputs Type: %02x \n",udpRecvBuffer[4]);
      printf("Activated Daughters: %02x \n",udpRecvBuffer[5]);
      int port = udpRecvBuffer[6] + 256 * udpRecvBuffer[7];
      printf("UDP Port of Card: %i \n",port);
      */
      for (b=0;b<num_inp/8;b++) {
	for (i=0;i<8;i++) {
	  inp[b*8+i] = get_bit(udpRecvBuffer[8+b],i);
	  //printf("%02x \n",udpRecvBuffer[8+b]);
	}
      }

       //printf("Digital Input 1: %i 2: %i 3: %i 4: %i \n",inp[0],inp[1],inp[2],inp[3]);
      //printf("Digital Input 14: %i 16: %i \n",inp[13],inp[15]);
      
      for (i=0;i<num_ana;i++) {
	ana[i]= udpRecvBuffer[16+i*2] + 256 * udpRecvBuffer[17+i*2];
      }
      //printf("Analog Input 1: %i \n",ana[0]);


      /* Send Displays */
      
      memset(udpSendBuffer,0,udpSendBufferLen);
      
      udpSendBuffer[0] = initstring[0];
      udpSendBuffer[1] = initstring[1];
      udpSendBuffer[2] = sendto_mb;
      udpSendBuffer[3] = outstype_dis;      
      udpSendBuffer[4] = 1; // Display Group 1-4 (each display group has 8 displays)
      udpSendBuffer[13] = 0x0f; // brightness (0-15 or 0x00 - 0x0f)
      for (i=0;i<4;i++) {
	set_7segment(&udpSendBuffer[5+i%8],get_digit(ana[0],i),0);
      }
      ret = send_udp();

      //printf("Sent %i bytes \n", ret);
      
      //stop = 1;
    } else {
      printf("No UDP Data yet\n");
    }

    
    usleep(1000);
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    printf("Time elapsed: %ld.%06ld\n",
       (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
  }

  exit_udp();
  
  return 0; 
} 

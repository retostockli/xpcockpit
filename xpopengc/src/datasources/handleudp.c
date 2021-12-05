/* This is the handleudp.c code which communicates flight data to/from the X-Plane 
   flight simulator via UDP interface

   Copyright (C) 2009 - 2014  Reto Stockli
   Adaptation to Linux compilation by Hans Jansen

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>   
#include <stdlib.h>  
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>   
#include <string.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>

#ifdef WIN
#include <winsock2.h>
#else
#include <sys/socket.h> 
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#endif

#include "handleudp.h"
#include "udpdata.h"

/* allocation of global variables from handleudp.h */

pthread_t poll_thread;                /* read thread */
int poll_thread_exit_code;            /* read thread exit code */
pthread_mutex_t exit_cond_lock = PTHREAD_MUTEX_INITIALIZER;

char udpServerIP[30];
int udpServerPort;
int udpSocket;
struct sockaddr_in udpServerAddr;     /* Server address structure */
struct sockaddr_in udpClientAddr;     /* Client address structure */

/* set up udp server socket with given server address and port */
int init_udp_server()
{

  /* Create a UDP socket */
  if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("HANDLEUDP: Cannot initialize Server UDP socket \n");
    return -1; 
  } else {

    /* Construct the server address structure */
    memset(&udpServerAddr, 0, sizeof(udpServerAddr));           /* Zero out structure */
    udpServerAddr.sin_family    = AF_INET;                  /* Internet address family */
    udpServerAddr.sin_addr.s_addr = inet_addr(udpServerIP);   /* Server IP address */
    udpServerAddr.sin_port        = htons(udpServerPort);     /* Server port */

    if (bind(udpSocket, (struct sockaddr *) &udpServerAddr, sizeof(udpServerAddr)) < 0)
      {
	printf("bind() failed\n");
	return -1;
      }

    /* set to blocking (0). Non-blocking (1) is not suitable for the threaded reading */
    unsigned long nSetSocketType = 0;
#ifdef WIN
    if (ioctlsocket(udpSocket,FIONBIO,&nSetSocketType) < 0) {
#else
    if (ioctl(udpSocket,FIONBIO,&nSetSocketType) < 0) {
#endif
      printf("HANDLEUDP: Server set to non-blocking failed\n");
      return -1;
    } else {
      printf("HANDLEUDP: Server Socket ready\n");
    }
  }

  /* set a 1 s timeout so that the thread can be terminated if ctrl-c is pressed */
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
  
  return 0;
}

/* set up udp socket with given server address and port */
int init_udp_client(void)
{
  int ret = 0;

  /* Create a UDP socket */
  if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("HANDLEUDP: Cannot initialize client UDP socket \n");
    ret = -1; 
  } else {
    
    /* Construct the server address structure */
    memset(&udpServerAddr, 0, sizeof(udpServerAddr));           /* Zero out structure */
    udpServerAddr.sin_family      = AF_INET;                  /* Internet address family */
    udpServerAddr.sin_addr.s_addr = inet_addr(udpServerIP);   /* Server IP address */
    udpServerAddr.sin_port        = htons(udpServerPort);     /* Server port */


    //set to non-blocking (1) for direct read
    //set to blocking (0) for asynchronous read
    unsigned long nSetSocketType = 0;
#ifdef WIN
    if (ioctlsocket(udpSocket,FIONBIO,&nSetSocketType) < 0) {
#else
    if (ioctl(udpSocket,FIONBIO,&nSetSocketType) < 0) {
#endif
      printf("HANDLEUDP: Client set to non-blocking failed\n");
      ret = -1;
    } else {
      printf("HANDLEUDP: Client Socket ready\n");
    }
  }

  /* set a 1 s timeout so that the thread can be terminated if ctrl-c is pressed */
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
   
  return ret;
}


int init_udp_receive() {

  int ret;

  poll_thread_exit_code = 0;
  ret = pthread_create(&poll_thread, NULL, &poll_thread_main, NULL);
  if (ret>0) {
    printf("HANDLEUDP: poll thread could not be created.\n");
    return -1;
  }

  return 0;
}

/* end udp server connection and close down read thread */
void exit_udp_server(void)
{

  poll_thread_exit_code = 1;
  pthread_join(poll_thread, NULL);

#ifdef WIN
  closesocket(udpSocket);
#else
  close(udpSocket);
#endif
}


/* end udp client connection */
void exit_udp_client(void)
{

  poll_thread_exit_code = 1;
  printf("Joining Poll Thread\n");
  pthread_join(poll_thread, NULL);
  printf("Poll Thread Ended\n");

#ifdef WIN
  closesocket(udpSocket);
#else
  close(udpSocket);
#endif
}


void *poll_thread_main()
/* thread handles udp receive on the server socket by use of blocking read and a read buffer */
{
  int ret = 0;
  unsigned char buffer[udpRecvBufferLen];

  printf("HANDLEUDP: Receive thread running \n");

  while (!poll_thread_exit_code) {

    /* read call goes here (1 s timeout for blocking operation) */
    int addrlen = sizeof(udpServerAddr);
    /* We are the Client */
    //ret = recvfrom(udpSocket, buffer, udpRecvBufferLen, 
    //       0, (struct sockaddr *) &udpServerAddr, &addrlen);
    /* We are the Server */
    ret = recv(udpSocket, buffer, udpRecvBufferLen, 0);
    if (ret == -1) {
      if ((errno == EWOULDBLOCK) || (errno == EINTR)) { // just no data yet or our own timeout ;-)
	//printf("UDP Poll Timeout \n");
      } else {
	printf("HANDLEUDP: Receive Error %i \n",errno);
	//poll_thread_exit_code = 1;
	//break;
      } 
    } else if ((ret > 0) && (ret <= udpRecvBufferLen)) {
      /* read is ok */
      /* are we reading WXR data ? */
      if ((strncmp(buffer,"xRAD",4)==0) || (strncmp(buffer,"RADR",4)==0)) {
	  
	/* does it fit into read buffer? */
	if (ret <= (udpRecvBufferLen - udpReadLeft)) {
	  pthread_mutex_lock(&exit_cond_lock);
	  memcpy(&udpRecvBuffer[udpReadLeft],buffer,ret);
	  udpReadLeft += ret;
	  pthread_mutex_unlock(&exit_cond_lock);
	  
	  //printf("HANDLEUDP: receive buffer position: %i \n",udpReadLeft);
	} else {
	  // printf("HANDLEUDP: receive buffer full: %i \n",udpReadLeft);
	}
      }
    } else if ((ret > 0) && (ret > udpRecvBufferLen)) {
      /* too many bytes were read */
      printf("HANDLEUDP: too big UDP packet of %i bytes does not fit in read buffer \n",ret);
    } else {
      /* nothing read */
    }

    /* wait loop not needed since we have a timeout */
    //usleep(500);

  } /* while loop */
  
  /* thread was killed */
  printf("HANDLEUDP: Asynchronous UDP receive thread shutting down \n");

  return 0;
}


/* send datagram to UDP server */
int send_udp_to_server(void) {

  int n;

  //  n = sendto(udpSocket, udpSendBuffer, udpSendBufferLen, 
  //	 MSG_CONFIRM, (const struct sockaddr *) &udpServerAddr, sizeof(udpServerAddr));

  n = sendto(udpSocket, udpSendBuffer, udpSendBufferLen, 
	 0, (const struct sockaddr *) &udpServerAddr, sizeof(udpServerAddr));
  
  //  printf("Sent to X-Plane: %i \n",n); 

  return n;
}

/* receive datagram from UDP server */
int recv_udp_from_server(void) {

  int n; 
  
  //socklen_t addrlen = sizeof(udpServerAddr);

  /* Non-Blocking */
  //n = recvfrom(udpSocket, udpRecvBuffer, udpRecvBufferLen, 
  //	       MSG_DONTWAIT, (struct sockaddr *) &udpServerAddr, &addrlen);

  /* Blocking */ 
  //n = recvfrom(udpSocket, udpRecvBuffer, udpRecvBufferLen, 
  //	       0, (struct sockaddr *) &udpServerAddr, &addrlen);

  /* Nonblocking */
  n = recv(udpSocket, udpRecvBuffer, udpRecvBufferLen, MSG_DONTWAIT);

  /* Blocking */
  //n = recv(udpSocket, udpRecvBuffer, udpRecvBufferLen, 0);


  //printf("Received from X-Plane: %i \n",n);

  return n;
}

/* receive datagram from UDP client */
int recv_udp_from_client(void) {

  int n; 

  n = recv(udpSocket, udpRecvBuffer, udpRecvBufferLen, 0);

  //printf("Received from X-Plane: %i \n",n);

  return n;
}


/* send datagram to UDP client */
int send_udp_to_client(char client_ip[],int client_port,unsigned char data[], int len) {

  int n;

  /* Construct the client address structure */
 
  memset(&udpClientAddr, 0, sizeof(udpClientAddr));       /* Zero out structure */
  udpClientAddr.sin_family      = AF_INET;                /* Internet address family */
  udpClientAddr.sin_addr.s_addr = inet_addr(client_ip);   /* Server IP address */
  udpClientAddr.sin_port        = htons(client_port);     /* Server port */
 
  /*
  printf("%i \n",udpClientAddr.sin_family);
  printf("%i \n",udpClientAddr.sin_addr.s_addr);
  printf("%i \n",ntohs(udpClientAddr.sin_port));
  printf("%s \n",udpClientAddr.sin_zero);
  */
  
  n = sendto(udpSocket, data, len, 
	     MSG_CONFIRM, (struct sockaddr *) &udpClientAddr, 
	     sizeof(udpClientAddr));

  return n;
}

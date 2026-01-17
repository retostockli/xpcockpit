/* This is the handleudp.c code which communicates to Teensy via the UDP protocol

   Copyright (C) 2009 - 2024  Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, 
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
#include <assert.h>
#include <pthread.h>

#ifdef WIN
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h> 
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#endif

#include "handleudp.h"
#include "common.h"

#define UDPRECVBUFLEN 12*1000
#define UDPSENDBUFLEN 12

/* UDP CLIENT PARAMETERS */
char udpClientIP[30];
short int udpClientPort;
char udpServerIP[30];
short int udpServerPort;
int clientSocket;
int serverSocket;

struct sockaddr_in udpServerAddr;     /* Server address structure */
struct sockaddr_in udpClientAddr;     /* Client address structure */

unsigned char *udpSendBuffer;         /* buffer containing data to send to udp */
unsigned char *udpRecvBuffer;         /* buffer containing data that was read from udp */
int udpReadLeft;                      /* counter of bytes to read from receive thread */

pthread_t udp_poll_thread;                /* read thread */
int udp_poll_thread_exit_code;            /* read thread exit code */
pthread_mutex_t udp_exit_cond_lock = PTHREAD_MUTEX_INITIALIZER;

/* prototype functions */
void *udpclient_thread_main(void *);

/* set up udp server socket with given server address and port */
int init_udp_server(char server_ip[],int server_port)
{
#ifdef WIN
  WSADATA wsaData;
  if (WSAStartup (MAKEWORD(2, 0), &wsaData) != 0) {
    fprintf (stderr, "WSAStartup(): Couldn't initialize Winsock.\n");
    return -1;
  }
#endif
  
  /* Create a UDP socket */
  if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("TEENSYUDP: Cannot initialize client UDP socket \n");
    return -1; 
  } else {
    
    /* Construct the server address structure */
    memset(&udpServerAddr, 0, sizeof(udpServerAddr));           /* Zero out structure */
    udpServerAddr.sin_family      = AF_INET;                  /* Internet address family */
    if (!strcmp(server_ip,"ANY")) {
      udpServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);   /* Bind to any local interface */
    } else {
      udpServerAddr.sin_addr.s_addr = inet_addr(server_ip); /* Bind to specific IP address */
    }      
    udpServerAddr.sin_port        = htons(server_port);     /* Server port */

    if (bind(serverSocket, (struct sockaddr *) &udpServerAddr, sizeof(udpServerAddr)) < 0)
      {
	printf("bind() failed\n");
	return -1;
      }

    /* set to blocking (0). Non-blocking (1) is not suitable for the threaded reading */
    unsigned long nSetSocketType = 0;
#ifdef WIN
    if (ioctlsocket(serverSocket,FIONBIO,&nSetSocketType) < 0) {
#else
    if (ioctl(serverSocket,FIONBIO,&nSetSocketType) < 0) {
#endif
      printf("TEENSYUDP: Server set to non-blocking failed\n");
      return -1;
    } else {
      printf("TEENSYUDP: Server Socket ready\n");
    }
  }

  /* set a 1 s timeout so that the thread can be terminated if ctrl-c is pressed */
#ifdef WIN
  int tv = 1000;
  setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
#else
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
#endif

  return 0;
}

void *udpclient_thread_main(void *)
/* thread handles udp receive on the server socket by use of blocking read and a read buffer */
{
  int ret = 0;
  unsigned char buffer[UDPRECVBUFLEN];

  printf("TEENSYUDP: Receive thread running \n");

  while (!udp_poll_thread_exit_code) {

    /* read call goes here (1 s timeout for blocking operation) */
    ret = recv(serverSocket, buffer, UDPRECVBUFLEN, 0);
#ifdef WIN
    int wsaerr = WSAGetLastError();
#endif
    if (ret == -1) {
#ifdef WIN
      if ((wsaerr == WSAEWOULDBLOCK) || (wsaerr == WSAEINTR)) { // just no data yet ...
#else
      if ((errno == EWOULDBLOCK) || (errno == EINTR)) { /* just no data yet or our own timeout */
#endif
	if (verbose > 3) printf("TEENSYUDP: No data yet. \n");
      } else {
#ifdef WIN
	if (wsaerr != WSAETIMEDOUT) {
	  printf("TEENSYUDP: Receive Error %i \n",wsaerr);
	  //udp_poll_thread_exit_code = 1;
	  //break;
	}
#else
	printf("TEENSYUDP: Receive Error %i \n",errno);
	//udp_poll_thread_exit_code = 1;
	//break;
#endif
      } 
    } else if ((ret > 0) && (ret <= UDPRECVBUFLEN)) {
      /* read is ok */

	//printf("%i \n",ret);
	
      /* does it fit into read buffer? */
      if (ret <= (UDPRECVBUFLEN - udpReadLeft)) {
	pthread_mutex_lock(&udp_exit_cond_lock);
	memcpy(&udpRecvBuffer[udpReadLeft],buffer,ret);
	udpReadLeft += ret;
	pthread_mutex_unlock(&udp_exit_cond_lock);
	
	if (verbose > 3) printf("TEENSYUDP: receive buffer position: %i \n",udpReadLeft);
      } else {
	if (verbose > 0) printf("TEENSYUDP: receive buffer full: %i \n",udpReadLeft);
      }
    } else if ((ret > 0) && (ret > UDPRECVBUFLEN)) {
      /* too many bytes were read */
      printf("TEENSYUDP: too big udp packet of %i bytes does not fit in read buffer \n",ret);
    } else {
      /* nothing read */
    }

    /* wait loop needed to allow read buffer to be emptied by read code */
    usleep(100);

  } /* while loop */
  
  /* thread was killed */
  if (verbose > 0) printf("TEENSYUDP: Asynchronous receive thread shutting down \n");

  return 0;
}


int init_udp_receive() {

  int ret;
  
  // initialize udp buffers
  udpSendBuffer=malloc(UDPSENDBUFLEN);
  udpRecvBuffer=malloc(UDPRECVBUFLEN);
  memset(udpSendBuffer,0,UDPSENDBUFLEN);
  memset(udpRecvBuffer,0,UDPRECVBUFLEN);

  udpReadLeft=0;
 
  udp_poll_thread_exit_code = 0;
  ret = pthread_create(&udp_poll_thread, NULL, &udpclient_thread_main, NULL);
  if (ret>0) {
    printf("TEENSYUDP: poll thread could not be created.\n");
    return -1;
  }

  return 0;
}

/* end udp connection */
void exit_udp(void)
{
  udp_poll_thread_exit_code = 1;
  pthread_join(udp_poll_thread, NULL);

#ifdef WIN
  closesocket(serverSocket);
  WSACleanup();
#else
  close(serverSocket);
#endif
}


/* send datagram to UDP client */
int send_udp(char client_ip[], int client_port, unsigned char data[], int len) {

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
  
  n = sendto(serverSocket, data, len, 0,
	     (struct sockaddr *) &udpClientAddr, sizeof(udpClientAddr));

  return n;
}

/* receive datagram client */
int recv_udp() {

  int n; 

  // unsigned int len = sizeof(udpClientAddr);

  /* Nonblocking */
  /*
  n = recvfrom(serverSocket, udpRecvBuffer, udpRecvBufferLen, 
	       MSG_DONTWAIT, (struct sockaddr *) &udpClientAddr, 
	       &len);
  */
  /* Blocking */
  /*
  n = recvfrom(serverSocket, udpRecvBuffer, udpRecvBufferLen, 
	       0, (struct sockaddr *) &udpClientAddr, 
	       &len);
  */
  n = recv(serverSocket, udpRecvBuffer, UDPRECVBUFLEN, 0);

  return n;
}

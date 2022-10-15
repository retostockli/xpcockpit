/* This is the xplanewxr.c code which receives WXR data from the X-Plane 
   flight simulator via UDP interface

   Copyright (C) 2022  Reto Stockli
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
#include <ws2tcpip.h>
#else
#include <sys/socket.h> 
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#endif

#include "xplanewxr.h"

/* allocation of global variables from xplanewxr.h */

pthread_t wxr_poll_thread;                /* read thread */
int wxr_poll_thread_exit_code;            /* read thread exit code */
pthread_mutex_t wxr_exit_cond_lock = PTHREAD_MUTEX_INITIALIZER;

char wxrServerIP[30];
int wxrServerPort;
int wxrSocket;
struct sockaddr_in wxrServerAddr;     /* Server address structure */
struct sockaddr_in wxrClientAddr;     /* Client address structure */

char *wxrSendBuffer;
char *wxrRecvBuffer;
int wxrSendBufferLen;
int wxrRecvBufferLen;
int wxrReadLeft;                      /* counter of bytes to read from receive thread */
int wxr_is_xp12;

/* set up udp server socket with given server address and port */
int init_wxr_server()
{

  /* Create a UDP socket */
  if ((wxrSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("XPLANEWXR: Cannot initialize Server UDP socket \n");
    return -1; 
  } else {

    /* Construct the server address structure */
    memset(&wxrServerAddr, 0, sizeof(wxrServerAddr));           /* Zero out structure */
    wxrServerAddr.sin_family    = AF_INET;                  /* Internet address family */
    wxrServerAddr.sin_addr.s_addr = inet_addr(wxrServerIP);   /* Server IP address */
    wxrServerAddr.sin_port        = htons(wxrServerPort);     /* Server port */

    if (bind(wxrSocket, (struct sockaddr *) &wxrServerAddr, sizeof(wxrServerAddr)) < 0)
      {
	printf("bind() failed\n");
	return -1;
      }

    /* set to blocking (0). Non-blocking (1) is not suitable for the threaded reading */
    unsigned long nSetSocketType = 0;
#ifdef WIN
    if (ioctlsocket(wxrSocket,FIONBIO,&nSetSocketType) < 0) {
#else
    if (ioctl(wxrSocket,FIONBIO,&nSetSocketType) < 0) {
#endif
      printf("XPLANEWXR: Server set to non-blocking failed\n");
      return -1;
    } else {
      printf("XPLANEWXR: Server Socket ready\n");
    }
  }

  /* set a 1 s timeout so that the thread can be terminated if ctrl-c is pressed */
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  if (setsockopt(wxrSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0) {
      printf("XPLANEWXR: Client Socket set Timeout failed\n");
      return -1;
  }

  int optval;
  if (setsockopt(wxrSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
      printf("XPLANEWXR: Client Socket set Reuseaddr failed\n");
      return -1;
  } 

  /*
  if (setsockopt(wxrSocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) < 0) {
      printf("XPLANEWXR: Client Socket set Reuseport failed\n");
      return -1;
      }
  */
  
  return 0;
}

/* set up udp socket with given server address and port */
int init_wxr_client(void)
{
  
  /* Create a UDP socket */
  if ((wxrSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("XPLANEWXR: Cannot initialize client UDP socket \n");
    return -1; 
  } else {
    
    /* Construct the server address structure */
    memset(&wxrServerAddr, 0, sizeof(wxrServerAddr));           /* Zero out structure */
    wxrServerAddr.sin_family      = AF_INET;                  /* Internet address family */
    wxrServerAddr.sin_addr.s_addr = inet_addr(wxrServerIP);   /* Server IP address */
    wxrServerAddr.sin_port        = htons(wxrServerPort);     /* Server port */


    //set to non-blocking (1) for direct read
    //set to blocking (0) for asynchronous read
    unsigned long nSetSocketType = 0;
#ifdef WIN
    if (ioctlsocket(wxrSocket,FIONBIO,&nSetSocketType) < 0) {
#else
    if (ioctl(wxrSocket,FIONBIO,&nSetSocketType) < 0) {
#endif
      printf("XPLANEWXR: Client set to non-blocking failed\n");
      return -1;
    } else {
      printf("XPLANEWXR: Client Socket ready\n");
    }
  }

  /* set a 1 s timeout so that the thread can be terminated if ctrl-c is pressed */
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  if (setsockopt(wxrSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0) {
      printf("XPLANEWXR: Client Socket set Timeout failed\n");
      return -1;
  }

  int optval;
  if (setsockopt(wxrSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
      printf("XPLANEWXR: Client Socket set Reuseaddr failed\n");
      return -1;
  }

  /*
  if (setsockopt(wxrSocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) < 0) {
      printf("XPLANEWXR: Client Socket set Reuseport failed\n");
      return -1;
      }
  */
   
  return 0;
}


int init_wxr_receive() {

  int ret;

  wxr_poll_thread_exit_code = 0;
  ret = pthread_create(&wxr_poll_thread, NULL, &wxr_poll_thread_main, NULL);
  if (ret>0) {
    printf("XPLANEWXR: poll thread could not be created.\n");
    return -1;
  }

  return 0;
}

/* end udp server connection and close down read thread */
void exit_wxr_server(void)
{

  wxr_poll_thread_exit_code = 1;
  printf("XPLANEWXR: Joining Poll Thread\n");
  pthread_join(wxr_poll_thread, NULL);
  printf("XPLANEWXR: Poll Thread Ended\n");

#ifdef WIN
  closesocket(wxrSocket);
#else
  close(wxrSocket);
#endif
}


/* end udp client connection */
void exit_wxr_client(void)
{

  wxr_poll_thread_exit_code = 1;
  printf("XPLANEWXR: Joining Poll Thread\n");
  pthread_join(wxr_poll_thread, NULL);
  printf("XPLANEWXR: Poll Thread Ended\n");

#ifdef WIN
  closesocket(wxrSocket);
#else
  close(wxrSocket);
#endif
}


void *wxr_poll_thread_main()
/* thread handles udp receive on the server socket by use of blocking read and a read buffer */
{
  int ret = 0;
  unsigned char buffer[wxrRecvBufferLen];
  struct sockaddr_in wxrAddr;     /* address structure */

  printf("XPLANEWXR: Receive thread running \n");

  while (!wxr_poll_thread_exit_code) {

    /* read call goes here (1 s timeout for blocking operation) */
    socklen_t addrlen = sizeof(wxrAddr);
    ret = recvfrom(wxrSocket, buffer, wxrRecvBufferLen, 
           0, (struct sockaddr *) &wxrAddr, &addrlen);
    //ret = recv(wxrSocket, buffer, wxrRecvBufferLen, 0);
    if (ret == -1) {
      if ((errno == EWOULDBLOCK) || (errno == EINTR)) { // just no data yet or our own timeout ;-)
	//printf("UDP Poll Timeout \n");
      } else {
	printf("XPLANEWXR: Receive Error %i \n",errno);
	//wxr_poll_thread_exit_code = 1;
	//break;
      } 
    } else if ((ret > 0) && (ret <= wxrRecvBufferLen)) {
      /* read is ok */
      /* are we reading WXR data ? */
      if ((strncmp(buffer,"xRAD",4)==0) || (strncmp(buffer,"RADR",4)==0)) {

	/* CHECK IF WE HAVE XP12 or XP11 */
	/* assume not more than one packet per call is received */
	if ((ret-5)/50 == 13) {
	  wxr_is_xp12 = 0;
	} else {
	  wxr_is_xp12 = 1;
	}
	   
	/* does it fit into read buffer? */
	if (ret <= (wxrRecvBufferLen - wxrReadLeft)) {
	  pthread_mutex_lock(&wxr_exit_cond_lock);
	  memcpy(&wxrRecvBuffer[wxrReadLeft],buffer,ret);
	  wxrReadLeft += ret;
	  pthread_mutex_unlock(&wxr_exit_cond_lock);
	  
	  //printf("XPLANEWXR: receive buffer position: %i \n",wxrReadLeft);
	} else {
	  // printf("XPLANEWXR: receive buffer full: %i \n",wxrReadLeft);
	}
      }
    } else if ((ret > 0) && (ret > wxrRecvBufferLen)) {
      /* too many bytes were read */
      printf("XPLANEWXR: too big UDP packet of %i bytes does not fit in read buffer \n",ret);
    } else {
      /* nothing read */
    }

    /* wait loop not needed since we have a timeout */
    //usleep(500);

  } /* while loop */
  
  /* thread was killed */
  printf("XPLANEWXR: Asynchronous UDP receive thread shutting down \n");

  return 0;
}


/* send datagram to UDP server */
int send_wxr_to_server(void) {

  int n;

  //  n = sendto(wxrSocket, wxrSendBuffer, wxrSendBufferLen, 
  //	 MSG_CONFIRM, (const struct sockaddr *) &wxrServerAddr, sizeof(wxrServerAddr));

  n = sendto(wxrSocket, wxrSendBuffer, wxrSendBufferLen, 
	 0, (const struct sockaddr *) &wxrServerAddr, sizeof(wxrServerAddr));
  
  //  printf("Sent to X-Plane: %i \n",n); 

  return n;
}

/* receive datagram from UDP server */
int recv_wxr_from_server(void) {

  int n; 
  
  //socklen_t addrlen = sizeof(wxrServerAddr);

  /* Non-Blocking */
  //n = recvfrom(wxrSocket, wxrRecvBuffer, wxrRecvBufferLen, 
  //	       MSG_DONTWAIT, (struct sockaddr *) &wxrServerAddr, &addrlen);

  /* Blocking */ 
  //n = recvfrom(wxrSocket, wxrRecvBuffer, wxrRecvBufferLen, 
  //	       0, (struct sockaddr *) &wxrServerAddr, &addrlen);

  /* Nonblocking */
  //n = recv(wxrSocket, wxrRecvBuffer, wxrRecvBufferLen, MSG_DONTWAIT);

  /* Blocking */
  n = recv(wxrSocket, wxrRecvBuffer, wxrRecvBufferLen, 0);


  //printf("Received from X-Plane: %i \n",n);

  return n;
}

/* receive datagram from UDP client */
int recv_wxr_from_client(void) {

  int n; 

  n = recv(wxrSocket, wxrRecvBuffer, wxrRecvBufferLen, 0);

  //printf("Received from X-Plane: %i \n",n);

  return n;
}


/* send datagram to UDP client */
int send_wxr_to_client(char client_ip[],int client_port,unsigned char data[], int len) {

  int n;

  /* Construct the client address structure */
 
  memset(&wxrClientAddr, 0, sizeof(wxrClientAddr));       /* Zero out structure */
  wxrClientAddr.sin_family      = AF_INET;                /* Internet address family */
  wxrClientAddr.sin_addr.s_addr = inet_addr(client_ip);   /* Server IP address */
  wxrClientAddr.sin_port        = htons(client_port);     /* Server port */
 
  /*
  printf("%i \n",wxrClientAddr.sin_family);
  printf("%i \n",wxrClientAddr.sin_addr.s_addr);
  printf("%i \n",ntohs(wxrClientAddr.sin_port));
  printf("%s \n",wxrClientAddr.sin_zero);
  */
  
  n = sendto(wxrSocket, data, len, 
	     0, (struct sockaddr *) &wxrClientAddr, 
	     sizeof(wxrClientAddr));

  return n;
}

void allocate_wxrdata(int sendlen, int recvlen) {
  // initialize udp buffers
  wxrSendBufferLen = sendlen;
  wxrRecvBufferLen = recvlen;
  wxrSendBuffer=malloc(wxrSendBufferLen);
  wxrRecvBuffer=malloc(wxrRecvBufferLen);
}

void deallocate_wxrdata() {
  free(wxrSendBuffer);
  wxrSendBuffer=NULL;
  free(wxrRecvBuffer);
  wxrRecvBuffer=NULL;
}


void get_wxrdata(char *data, int len) {

  data = wxrRecvBuffer;
  len = wxrRecvBufferLen;

}

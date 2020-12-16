/* This is the handleserver.c code which communicates flight data to/from the X-Plane 
   flight simulator via TCP/IP interface

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
#include <sys/socket.h> 
#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>   
#include <string.h>
#include <errno.h>
#include <math.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "handleudp.h"

/* set up udp server socket with given server address and port */
int init_udp_server(char server_ip[],int server_port)
{

  /* Create a UDP socket */
  if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("HANDLEUDP: Cannot initialize client UDP socket \n");
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

    //set to blocking (0) or Non-blocking (1).
    unsigned long nSetSocketType = 0;
    if (ioctl(serverSocket,FIONBIO,&nSetSocketType) < 0) {
      printf("HANDLEUDP: Server set to non-blocking failed\n");
      return -1;
    } else {
      printf("HANDLEUDP: Server Socket ready\n");
    }
  }

  // initialize udp buffers
  udpSendBuffer=malloc(udpSendBufferLen);
  udpRecvBuffer=malloc(udpRecvBufferLen);
  
  return 0;
}

int init_udp_read_thread() {

  return 0;
}


/* end udp connection */
void exit_udp(void)
{
  close(serverSocket);
}


/* send datagram to X-Plane UDP server */
int send_udp(char client_ip[],int client_port,unsigned char data[], int len) {

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
  
  n = sendto(serverSocket, data, len, 
	     MSG_CONFIRM, (struct sockaddr *) &udpClientAddr, 
	     sizeof(udpClientAddr));

  return n;
}

/* receive datagram from X-Plane UDP server */
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
  n = recv(serverSocket, udpRecvBuffer, udpRecvBufferLen, 0);

  return n;
}

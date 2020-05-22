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
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>   
#include <string.h>
#include <errno.h>
#include <math.h>

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

/* set up udp socket with given server address and port */
int init_udp(void)
{
  int ret = 0;

  /* Create a UDP socket */
  if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("HANDLEUDP: Cannot initialize client UDP socket \n");
    ret = -1; 
  } else {
    
    /* Construct the server address structure */
    memset(&udpServAddr, 0, sizeof(udpServAddr));           /* Zero out structure */
    udpServAddr.sin_family      = AF_INET;                  /* Internet address family */
    udpServAddr.sin_addr.s_addr = inet_addr(udpServerIP);   /* Server IP address */
    udpServAddr.sin_port        = htons(udpServerPort);     /* Server port */


    //set to non-blocking
    unsigned long nSetSocketType = 1;
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
 
  return ret;
}

/* end udp connection */
void exit_udp(void)
{
#ifdef WIN
  closesocket(udpSocket);
#else
  close(udpSocket);
#endif
}


/* send datagram to X-Plane UDP server */
int send_udp(void) {

  int n;

  /*
  n = sendto(udpSocket, udpSendBuffer, udpSendBufferLen, 
	 MSG_CONFIRM, (const struct sockaddr *) &udpServAddr, 
	 sizeof(udpServAddr));
  */
  n = sendto(udpSocket, udpSendBuffer, udpSendBufferLen, 
	 0, (const struct sockaddr *) &udpServAddr, sizeof(udpServAddr));
  
  //  printf("Sent to X-Plane: %i \n",n); 

  return n;
}

/* receive datagram from X-Plane UDP server */
int recv_udp(void) {

  int n; 
  
  int addrlen = sizeof(udpServAddr);
  /*
  n = recvfrom(udpSocket, udpRecvBuffer, udpRecvBufferLen, 
	       MSG_DONTWAIT, (struct sockaddr *) &udpServAddr, 
	       &addrlen);
  */
  n = recvfrom(udpSocket, udpRecvBuffer, udpRecvBufferLen, 
	       0, (struct sockaddr *) &udpServAddr, &addrlen);

  //  printf("Received from X-Plane: %i \n",n);

  return n;
}

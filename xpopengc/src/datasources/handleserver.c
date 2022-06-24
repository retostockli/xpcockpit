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

#include <stdint.h>
#include <stdio.h>   
#include <stdlib.h>  
#include <sys/types.h>
#include <sys/time.h>
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

#include "handleserver.h"
#include "serverdata.h"
#include "common.h"

/* global variable allocation */
int verbose;

char server_ip[30];
short int server_port;

char recvBuffer[TCPBUFSIZE];        /* Buffer for receive string */
char sendBuffer[TCPBUFSIZE];        /* Buffer for send string */

struct sockaddr_in ServAddr;     /* Server address structure */
int socketStatus;                /* Socket status 1-5 */
int clntSock;                    /* client socket descriptor */
int check_tcpip_counter;  /* only check for server every x calls to check_server */

/* transfer floating point values from host to network byte order */
void htonf (float *src, float *dst)
{
  uint32_t lu1, lu2;
  
  memcpy (&lu1, src, sizeof (uint32_t));
  lu2 = htonl (lu1);
  memcpy (dst, &lu2, sizeof (uint32_t));
  return;
}

/* transfer floating point values from network to host byte order */
void ntohf (float *src, float *dst)
{
  uint32_t lu1, lu2;
  
  memcpy (&lu1, src, sizeof (uint32_t));
  lu2 = ntohl (lu1);
  memcpy (dst, &lu2, sizeof (uint32_t));
  return;
}

/* set up the tcp/ip interface with given server address and port */
int initialize_tcpip(void)
{
  int ret = 0;

  /* initialize network status */
  socketStatus = status_Disconnected;

  /* reset counter */
  check_tcpip_counter = 0;
  
#ifdef WIN
  WSADATA wsaData;
  if (WSAStartup (MAKEWORD(2, 0), &wsaData) != 0) {
    fprintf (stderr, "WSAStartup(): Couldn't initialize Winsock.\n");
    exit (EXIT_FAILURE);
  }
#endif
    
  /* Create a reliable, stream socket using TCP */
  if ((clntSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    printf("HANDLESERVER: Cannot initialize client TCP socket \n");
    ret = -41; 
  } else {
    
    /* Construct the server address structure */
    memset(&ServAddr, 0, sizeof(ServAddr));            /* Zero out structure */
    ServAddr.sin_family      = AF_INET;                /* Internet address family */
    ServAddr.sin_addr.s_addr = inet_addr(server_ip);   /* Server IP address */
    ServAddr.sin_port        = htons(server_port);     /* Server port */
    
    if (verbose > 1) printf("HANDLESERVER: Client Socket ready\n");
  }
  
  return ret;
}

/* end tcp connection */
void exit_tcpip(void)
{
  int sendMsgSize;  /* size of sent message */
  int send_left;
  char *message_ptr;
  int datai;

  if ((socketStatus == status_Connected)) { 

    /* Send Disconnect Marker */
    send_left = 0;

    /* add end of transmission marker */
    datai = MARK_DISCONNECT;
    memcpy(&sendBuffer[send_left],&datai,sizeof(int));
    send_left += sizeof(int);

    message_ptr = sendBuffer;
    while ((send_left > 0) && (socketStatus != status_Error)) {
      
      sendMsgSize = send(clntSock, message_ptr, send_left, 0);
      
      if (sendMsgSize == -1) {
	if (verbose > 0) printf("HANDLESERVER: Error Sending data to Server. \n");
	socketStatus = status_Error;	/* signal a transmission error */
      } else {
	if (verbose > 1) printf("HANDLESERVER: sending %i bytes \n",sendMsgSize);	      
	send_left -= sendMsgSize;
	message_ptr += sendMsgSize;
      }
    } 

  }

  /* Close TCP client socket */
#ifdef WIN
  closesocket(clntSock);
  WSACleanup();
#else
  close(clntSock); 
#endif
  
  socketStatus = status_Disconnected;

  if (verbose > 0) printf("HANDLESERVER: Client Socket closed\n");

}


/* establish tcp/ip connection to X-Plane server */
int check_server(void)
{

  int ret = 0;

  if (socketStatus == status_Error) {
    if (verbose > 0) printf("HANDLESERVER: Ignoring Error. Trying send/receive again ... \n");
    socketStatus = status_Connected;
  }
  
  if (socketStatus == status_Disconnected) { 

    if (check_tcpip_counter == (1000/INTERVAL)) {
      check_tcpip_counter=0;

      /* Check for and establish a connection to the X-Plane server */
      if (verbose > 2) printf("HANDLESERVER: Checking for X-Plane server \n");
      if (connect(clntSock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) < 0) {
	/* no server visible yet ... */
	if (verbose > 2) printf("HANDLESERVER: No TCP/IP connection to X-Plane yet. \n");
#ifdef WIN
	closesocket(clntSock);
#else
	close(clntSock); 
#endif
	if (initialize_tcpip() < 0) {
	  if (verbose > 0) printf("HANDLESERVER: Failed to initialize client socket \n");
	  ret = -43;
	}
	
	socketStatus = status_Disconnected;

      } else {
	/* yeah, we found the xpserver plugin running in X-Plane ... */
	if (verbose > 0) {
	  printf("HANDLESERVER: Connected to X-Plane. \n");
	  printf("X-Plane Server Plugin Address:Port is %s:%i \n",server_ip, server_port);
	}

	socketStatus = status_Connected;

	unsigned long nSetSocketType = NON_BLOCKING;
#ifdef WIN
	if (ioctlsocket(clntSock,FIONBIO,&nSetSocketType) < 0)
#else
	if (ioctl(clntSock,FIONBIO,&nSetSocketType) < 0)
#endif
	  {
	    if (verbose > 0) printf("HANDLESERVER: Client set to non-blocking failed\n");
	    socketStatus = status_Error;
	    ret = -42;
	  }     
      }
    }

    check_tcpip_counter++;

  }

  return ret;
}

/* receive data stream from X-Plane TCP/IP server */
int receive_server(void) {

  int recvMsgSize;                    /* Size of received completed message */
  int recv_left;
  char *message_ptr;
  int i,j;

  int offset;
  int nelements;
  char errorstring[100];

  int first;
  int datai;
  float dataf;
  double datad;
  float *datavf;
  int *datavi;
  unsigned char *datab;
  int disconnected;

  disconnected = 0;
  datai = 0;
  recvMsgSize = 0;
  recvBuffer[recvMsgSize] = 0;
  message_ptr = recvBuffer;

  /* reset received flag of datarefs */
  reset_received();
  
  while (socketStatus == status_Connected) {
    
    /* Check for new data from server */
    recv_left = recv(clntSock, message_ptr, TCPBUFSIZE, 0);
#ifdef WIN
    int wsaerr = WSAGetLastError();
#endif
    
    if (recv_left == 0) { // disconnection
      if (verbose > 0) printf("HANDLESERVER: X-Plane disconnected without notice from client socket. \n");
      socketStatus = status_Disconnected;
      disconnected = 1;
      recvMsgSize = 0;
      break;
    }
    
    if (recv_left == -1) { // nothing received: error?
#ifdef WIN
      if (wsaerr == WSAEWOULDBLOCK) { // just no data yet ...
#else
      if (errno == EWOULDBLOCK) { // just no data yet ...
#endif
	if (verbose > 2) printf("HANDLESERVER: Client Socket: no data yet. \n");
	if (recvMsgSize == 0) break; /* else continue waiting for the end of the packet */
      } else {
#ifdef WIN
	if ((wsaerr == WSAECONNABORTED) || (wsaerr == WSAECONNREFUSED) ||
	    (wsaerr == WSAECONNRESET)   || (wsaerr == WSAEHOSTUNREACH) ||
	    (wsaerr == WSAENETDOWN)     || (wsaerr == WSAENETRESET)    ||
	    (wsaerr == WSAENETUNREACH)  || (wsaerr == WSAETIMEDOUT)) { // now we have a network status
#else
	if ((errno == ECONNABORTED) || (errno == ECONNREFUSED) ||
	    (errno == ECONNRESET)   || (errno == EHOSTUNREACH) ||
	    (errno == ENETDOWN)     || (errno == ENETRESET)    ||
	    (errno == ENETUNREACH)  || (errno == ETIMEDOUT)) { // now we have a network status
#endif
	  if (verbose > 0) printf("HANDLESERVER: X-Plane disconnected with error from client socket. Error! \n");
	  socketStatus = status_Disconnected;
	  disconnected = 1;
	  recv_left = 0;
	  recvMsgSize = 0;
	  break;
        } else { // here we have a real error
	  if (verbose > 0) printf("HANDLESERVER: Client Socket Error. Received %i bytes \n",recv_left);
	  socketStatus = status_Error;
	  recv_left = 0; 
	  recvMsgSize = 0;
	  break;
	} 
      }
    } 

    if (recv_left > 0) {

      /* augment receive pointer and total size */
      recvMsgSize += recv_left;
      message_ptr += recv_left;

      
      /* check last transmitted value: is it end of transmission? */
      memcpy(&datai,&recvBuffer[recvMsgSize-sizeof(int)],sizeof(int));
      if ((datai == MARK_EOT) || (datai == MARK_DISCONNECT)) {	
	/* received full message with EOT or DISCONNECT marker at the end */
	if (verbose > 1) printf("HANDLESERVER: Received Total %i bytes \n",recvMsgSize);
	break;
      } else {
	/* only partial message received */
	if (verbose > 1) printf("HANDLESERVER: Received Partial %i bytes \n",recv_left);
      }
      
    }
    
  } /* while connected and data coming in (else break the loop) */
  
  /*  received a complete transmission */
  if (recvMsgSize > 0)  {
    
    recv_left = recvMsgSize;

    while (recv_left > 0) {

      /* decode first integer: type of transmission */
      memcpy(&first,&recvBuffer[recvMsgSize-recv_left],sizeof(int));
      recv_left -= sizeof(int);
	  
      if ((first >= MARK_DATA) && (first < MARK_LINK)) {
	offset = first - MARK_DATA;

	if (serverdata != NULL) {

	  if (offset<numalloc) {   

	    if (serverdata[offset].index == -1) {
	      nelements = serverdata[offset].nelements;
	    } else {
	      nelements = 1;
	    }

	    serverdata[offset].received = 1;
	    serverdata[offset].nrecv++;
	    
	    switch (serverdata[offset].type) {
	    case XPTYPE_INT:		
	      if (recv_left >= sizeof(int)) {
		if (serverdata[offset].status == XPSTATUS_VALID) {
		  memcpy(&datai,&recvBuffer[recvMsgSize-recv_left],sizeof(int));
		  if (datai != INT_MISS) {
		    memcpy(serverdata[offset].data,&datai,sizeof(int));
		    memcpy(serverdata[offset].data_old,&datai,sizeof(int));
		    if (verbose > 1) printf("HANDLESERVER: Received data for offset %i dataref %s: %i \n",
					    offset,serverdata[offset].datarefname,datai);
		  }
		} else {
		  if (verbose > 0) printf("HANDLESERVER: Received offset %i dataref %s not ready. Discarding data. \n",
					  offset,serverdata[offset].datarefname);
		}
		recv_left -= sizeof(int);
	      } else {
		if (verbose > 0) printf("HANDLESERVER: Data stream for offset %i incomplete. Discarding. \n",offset);
		recv_left = 0;
	      }
	      break;
	    case XPTYPE_FLT:
	      if (recv_left >= sizeof(float)) {
		if (serverdata[offset].status == XPSTATUS_VALID) {
		  memcpy(&dataf,&recvBuffer[recvMsgSize-recv_left],sizeof(float));
		  if (dataf != FLT_MISS) {
		    memcpy(serverdata[offset].data,&dataf,sizeof(float));
		    memcpy(serverdata[offset].data_old,&dataf,sizeof(float));
		    if (verbose > 1) printf("HANDLESERVER: Received data for offset %i dataref %s: %f \n",
					    offset,serverdata[offset].datarefname,dataf);
		  }
		} else {
		  if (verbose > 0) printf("HANDLESERVER: Received offset %i dataref %s not ready. Discarding data. \n",
					  offset,serverdata[offset].datarefname);
		}
		recv_left -= sizeof(float);
	      } else {
		if (verbose > 0) printf("HANDLESERVER: Data stream for offset %i incomplete. Discarding. \n",offset);
		recv_left = 0;
	      }
	      break;
	    case XPTYPE_DBL:
	      if (recv_left >= sizeof(double)) {
		if (serverdata[offset].status == XPSTATUS_VALID) {
		  memcpy(&datad,&recvBuffer[recvMsgSize-recv_left],sizeof(double));
		  if (datad != DBL_MISS) {
		    memcpy(serverdata[offset].data,&datad,sizeof(double));
		    memcpy(serverdata[offset].data_old,&datad,sizeof(double));
		    if (verbose > 1) printf("HANDLESERVER: Received data for offset %i dataref %s: %f \n",
					    offset,serverdata[offset].datarefname, datad);
		  }
		} else {
		  if (verbose > 0) printf("HANDLESERVER: Received offset %i dataref %s not ready. Discarding data. \n",
					  offset,serverdata[offset].datarefname);
		}
		recv_left -= sizeof(double);
	      } else {
		if (verbose > 0) printf("HANDLESERVER: Data stream for offset %i incomplete. Discarding. \n",offset);
		recv_left = 0;
	      }
	      break;
	    case XPTYPE_FLT_ARR:
	      if (recv_left >= (nelements*sizeof(float))) {
		if (serverdata[offset].status == XPSTATUS_VALID) {
		  datavf = malloc(nelements*sizeof(float));
		  if (datavf != NULL) {
		    memcpy(datavf,&recvBuffer[recvMsgSize-recv_left],nelements*sizeof(float));
		    if (serverdata[offset].index == -1) {
		      for (j=0;j<nelements;j++) {
			if (datavf[j] != FLT_MISS) {
			  memcpy(&((float*) serverdata[offset].data)[j],&datavf[j],sizeof(float));
			  memcpy(&((float*) serverdata[offset].data_old)[j],&datavf[j],sizeof(float));
			  if (verbose > 1) printf("HANDLESERVER: Received data for offset %i dataref %s index %i: %f \n",
						  offset,serverdata[offset].datarefname, j, datavf[j]);
			}
		      }
		    } else {
		      if (*datavf != FLT_MISS) {
			memcpy(serverdata[offset].data,datavf,sizeof(float));
			memcpy(serverdata[offset].data_old,datavf,sizeof(float));
			if (verbose > 1) printf("HANDLESERVER: Received data for offset %i dataref %s index %i: %f \n",
						offset,serverdata[offset].datarefname, serverdata[offset].index, *datavf);
		      }
		    }
		  }
		  free(datavf);
		} else {
		  if (verbose > 0) printf("HANDLESERVER: Received offset %i dataref %s not ready. Discarding data. \n",
					  offset, serverdata[offset].datarefname);
		}
		recv_left -= nelements*sizeof(float);
	      } else {
		if (verbose > 0) printf("HANDLESERVER: Data stream for offset %i dataref %s incomplete. Discarding. \n",
					offset, serverdata[offset].datarefname);
		recv_left = 0;
	      }
	      break;
	    case XPTYPE_INT_ARR:
	      if (recv_left >= (nelements*sizeof(int))) {
		if (serverdata[offset].status == XPSTATUS_VALID) {
		  datavi = malloc(nelements*sizeof(int));
		  if (datavi != NULL) {
		    memcpy(datavi,&recvBuffer[recvMsgSize-recv_left],nelements*sizeof(int));
		    if (serverdata[offset].index == -1) {
		      for (j=0;j<nelements;j++) {
			if (datavi[j] != INT_MISS) {
			  memcpy(&((int*) serverdata[offset].data)[j],&datavi[j],sizeof(int));
			  memcpy(&((int*) serverdata[offset].data_old)[j],&datavi[j],sizeof(int));
			  if (verbose > 1) printf("HANDLESERVER: Received data for offset %i dataref %s index %i: %i \n",
						  offset, serverdata[offset].datarefname, j, datavi[j]);
			}
		      }
		    } else {
		      if (*datavi != INT_MISS) {
			memcpy(serverdata[offset].data,datavi,sizeof(int));
			memcpy(serverdata[offset].data_old,datavi,sizeof(int));
			if (verbose > 1) printf("HANDLESERVER: Received data for offset %i dataref %s index %i: %i \n",
						offset, serverdata[offset].datarefname, serverdata[offset].index, *datavi);
		      }
		    }
		  }
		  free(datavi);
		} else {
		  if (verbose > 0) printf("HANDLESERVER: Received offset %i dataref %s not ready. Discarding data. \n",
					  offset, serverdata[offset].datarefname);
		}
		recv_left -= nelements*sizeof(int);
	      } else {
		if (verbose > 0) printf("HANDLESERVER: Data stream for offset %i dataref %s incomplete. Discarding. \n",
					offset, serverdata[offset].datarefname);
		recv_left = 0;
	      }
	      break;
	    case XPTYPE_BYTE_ARR:
	      if (recv_left >= (nelements*sizeof(unsigned char))) {
		if (serverdata[offset].status == XPSTATUS_VALID) {
		  datab = malloc(nelements*sizeof(unsigned char));
		  if (datab != NULL) {
		    memcpy(datab,&recvBuffer[recvMsgSize-recv_left],nelements*sizeof(unsigned char));
		    if (serverdata[offset].index == -1) {
		      for (j=0;j<nelements;j++) {
			memcpy(&((unsigned char*) serverdata[offset].data)[j],&datab[j],sizeof(unsigned char));
			memcpy(&((unsigned char*) serverdata[offset].data_old)[j],&datab[j],sizeof(unsigned char));
			if (verbose > 1) printf("HANDLESERVER: Received data for offset %i dataref %s index %i: %u \n",
						offset, serverdata[offset].datarefname, j, datab[j]);
		      }
		    } else {
		      memcpy(serverdata[offset].data,datab,sizeof(unsigned char));
		      memcpy(serverdata[offset].data_old,datab,sizeof(unsigned char));
		      if (verbose > 1) printf("HANDLESERVER: Received data for offset %i dataref %s index %i: %u \n",
					      offset, serverdata[offset].datarefname, serverdata[offset].index, *datab);
		    }
		  }
		  free(datab);
		} else {
		  if (verbose > 0) printf("HANDLESERVER: Received offset %i dataref %s not ready. Discarding data. \n",
					  offset, serverdata[offset].datarefname );
		}
		recv_left -= nelements*sizeof(unsigned char);
	      } else {
		if (verbose > 0) printf("HANDLESERVER: Data stream for offset %i dataref %s incomplete. Discarding. \n",
					offset, serverdata[offset].datarefname);
		recv_left = 0;
	      }
	      break;
	    case XPTYPE_CMD_ONCE:
	    case XPTYPE_CMD_HOLD:
	      if (recv_left >= sizeof(int)) {
		if (serverdata[offset].status == XPSTATUS_VALID) {
		  memcpy(&datai,&recvBuffer[recvMsgSize-recv_left],sizeof(int));
		  if (datai != INT_MISS) {
		    memcpy(serverdata[offset].data,&datai,sizeof(int));
		    memcpy(serverdata[offset].data_old,&datai,sizeof(int));
		    if (verbose > 1) printf("HANDLESERVER: Received data for offset %i dataref %s: %i \n",
					    offset,serverdata[offset].datarefname,datai);
		  }
		} else {
		  if (verbose > 0) printf("HANDLESERVER: Received offset %i dataref %s not ready. Discarding data. \n",
					  offset,serverdata[offset].datarefname);
		}
		recv_left -= sizeof(int);
	      } else {
		if (verbose > 0) printf("HANDLESERVER: Data stream for offset %i incomplete. Discarding. \n",offset);
		recv_left = 0;
	      }
	      break;
	    default:
	      break;
	    }
		  
	  }

	}

      }

      if ((first >= MARK_LINK) && (first < MARK_UNLINK)) {

	if (recv_left >= (sizeof(errorstring))) {
	  offset = first - MARK_LINK;

	  memcpy(&errorstring,&recvBuffer[recvMsgSize-recv_left],sizeof(errorstring));
	  recv_left -= sizeof(errorstring);
	      
	  if (strncmp(serverdata[offset].datarefname,errorstring,sizeof(errorstring))) {
	    if (verbose > 0) printf("HANDLESERVER: X-Plane cannot link offset %i dataref %s: %s \n",
				    offset,serverdata[offset].datarefname,errorstring);
	  } else {
	    if (verbose > 0) printf("HANDLESERVER: X-Plane linked offset %i dataref %s \n",
				    offset, serverdata[offset].datarefname);

	    serverdata[offset].status = XPSTATUS_VALID;
	    serverdata[offset].received = 1;
	    count_dataref();
	  }

	} else {
	  if (verbose > 0) printf("HANDLESERVER: X-Plane link confirmation for offset %i dataref %s incomplete. \n",
				  offset, serverdata[offset].datarefname);
	  recv_left = 0;
	}
	    
      }

      if ((first >= MARK_UNLINK) && (first < MARK_EOT)) {

	if (recv_left >= (sizeof(errorstring))) {
	  offset = first - MARK_UNLINK;
	      
	  memcpy(&errorstring,&recvBuffer[recvMsgSize-recv_left],sizeof(errorstring));
	  recv_left -= sizeof(errorstring);
	  if (strncmp(serverdata[offset].datarefname,errorstring,sizeof(errorstring))) {
	    if (verbose > 0) printf("HANDLESERVER: X-Plane cannot unlink offset %i dataref %s: %s \n",
				    offset,serverdata[offset].datarefname,errorstring);
	  } else {
	    if (verbose > 0) printf("HANDLESERVER: X-Plane unlinked offset %i dataref %s \n",
				    offset, serverdata[offset].datarefname);
	    memset(serverdata[offset].datarefname,0,sizeof(serverdata[offset].datarefname));

	    serverdata[offset].status = XPSTATUS_UNINITIALIZED;
	    serverdata[offset].received = 1;
	    count_dataref();
	  }

	} else {
	  if (verbose > 0) printf("HANDLESERVER: X-Plane unlink confirmation for offset %i dataref %s incomplete. \n",
				  offset, serverdata[offset].datarefname);
	  recv_left = 0;
	}
	    
      }


      if (first == MARK_EOT) {
	if (verbose > 1) {printf("HANDLESERVER: Received EOT Marker \n"); fflush(stdout);}
      }

      if (first == MARK_DISCONNECT) {
	if (verbose > 0) printf("HANDLESERVER: Received Disconnect Marker \n");
	    
	socketStatus = status_Disconnected;
	disconnected = 1;
      }
	  
    } /* elements present in receive buffer */
	
  } /* if client connected to server */


  /* disconnect happened during receive call */
  if (disconnected == 1) {
    
    /* reset x-plane data status to allocated */
    for (i=0;i<numalloc;i++) {
     if (strcmp("",serverdata[i].datarefname)) {
	serverdata[i].status = XPSTATUS_ALLOC;
      }
    }
    /* valid dataref, check status */
    
#ifdef WIN
    closesocket(clntSock);
#else
    close(clntSock); 
#endif
    if (initialize_tcpip() < 0) {
      if (verbose > 0) printf("HANDLESERVER: Failed to initialize client socket \n");
      recvMsgSize = -123;
    }
    
  } 

  /* count number of received datarefs */
  count_received();
  
  return recvMsgSize;
}

/* send data stream to X-Plane TCP/IP server */
int send_server(void) {

  //  struct timeval tval_before, tval_after, tval_result;
  
  int sendMsgSize;  /* size of sent message */
  int send_left;
  char *message_ptr;
  int i,j;
  int first;
  int nelements;
  int changed;
  float scale;
  
  int datai;
  
  /* initialize or send updated data to X-Plane */
  if ((socketStatus == status_Connected)) { 
    
    if (serverdata != NULL) {
      /* loop through datarefs */

      //gettimeofday(&tval_before, NULL);
     
      send_left = 0;
      
      for (i=0;i<numalloc;i++) {

	if (strcmp("",serverdata[i].datarefname)) {
	  /* valid dataref, check status */
	  /* check for buffer overflow! */
	  
	  switch (serverdata[i].status) {

	  case XPSTATUS_ALLOC: 
	    /* serverdata for dataref allocated: send dataref link command to X-Plane */
	    if ((send_left+6*sizeof(int)+sizeof(serverdata[i].datarefname)+sizeof(clientname)) <= TCPBUFSIZE) {
	      if (verbose > 1) printf("HANDLESERVER: sending definition for offset %i dataref %s \n",i, serverdata[i].datarefname);

	      first = MARK_LINK + i;
	      memcpy(&sendBuffer[send_left],&first,sizeof(int));
	      send_left += sizeof(int);
	      
	      memcpy(&sendBuffer[send_left],&serverdata[i].type,sizeof(int));
	      send_left += sizeof(int);
	      
	      memcpy(&sendBuffer[send_left],&serverdata[i].nelements,sizeof(int));
	      send_left += sizeof(int);
	      
	      memcpy(&sendBuffer[send_left],&serverdata[i].index,sizeof(int));
	      send_left += sizeof(int);
	      
	      memcpy(&sendBuffer[send_left],&serverdata[i].precision,sizeof(int));
	      send_left += sizeof(int);
	      
	      memcpy(&sendBuffer[send_left],&serverdata[i].datarefname,sizeof(serverdata[i].datarefname));
	      send_left += sizeof(serverdata[i].datarefname);
	      
	      memcpy(&sendBuffer[send_left],&clientname,sizeof(clientname));
	      send_left += sizeof(clientname);

	      serverdata[i].status = XPSTATUS_LINK;
	    } else {
	      if (verbose > 0) printf("HANDLESERVER: TCP buffer overflow sending initial offset %i dataref %s \n",
				      i, serverdata[i].datarefname);
	    }
	    break;
	    
	  case XPSTATUS_DEALLOC:
	    /* serverdata for dataref deallocated: send dataref unlink command to X-Plane */
	    if ((send_left+sizeof(int)+sizeof(serverdata[i].datarefname)) <= TCPBUFSIZE) {
	      first = MARK_UNLINK + i;
	      memcpy(&sendBuffer[send_left],&first,sizeof(int));
	      send_left += sizeof(int);
	      memcpy(&sendBuffer[send_left],&serverdata[i].datarefname,sizeof(serverdata[i].datarefname));
	      send_left += sizeof(serverdata[i].datarefname);
	      
	      serverdata[i].status = XPSTATUS_UNLINK;
	    } else {
	      if (verbose > 0) printf("HANDLESERVER: TCP buffer overflow sending unlink offset %i dataref %s \n",
				      i, serverdata[i].datarefname);
	    }
	    break;
	  case XPSTATUS_VALID:

	    changed = 0;

	    if (serverdata[i].index == -1) {
	      nelements = serverdata[i].nelements;
	    } else {
	      nelements = 1;
	    }

	    if ((serverdata[i].precision > 10) || (serverdata[i].precision < -10)) {
	      scale = 1.0;
	    } else {
	      scale = pow(10,-serverdata[i].precision);
	    }

	    switch (serverdata[i].type) {
	    case XPTYPE_INT:
	      if (*(int *) serverdata[i].data != *(int *) serverdata[i].data_old) changed = 1;
	      if (changed == 1) {
		if ((send_left+3*sizeof(int)) <= TCPBUFSIZE) {
		  first = MARK_DATA + i;
		  memcpy(&sendBuffer[send_left],&first,sizeof(int));
		  send_left += sizeof(int);

		  if (verbose > 1) printf("HANDLESERVER: sending data for offset %i dataref %s: %i \n", 
					  i, serverdata[i].datarefname,*(int *) serverdata[i].data);
		  
		  memcpy(&sendBuffer[send_left],serverdata[i].data,sizeof(int));
		  memcpy(serverdata[i].data_old,serverdata[i].data,sizeof(int));
		  send_left += sizeof(int);
		  
		} else {
		  if (verbose > 0) printf("HANDLESERVER: TCP buffer overflow sending data for offset %i dataref %s \n",
					  i, serverdata[i].datarefname);
		}
	      }
	      break;
	    case XPTYPE_FLT:
	      if ((serverdata[i].precision > 10) || (serverdata[i].precision < -10)) {
		if (*(float *) serverdata[i].data != *(float *) serverdata[i].data_old) changed = 1;
	      } else {
		if ((int) roundf(*(float *) serverdata[i].data * scale) != (int) roundf(*(float *) serverdata[i].data_old * scale)) changed = 1;
	      }
	      if (changed == 1) {
		if ((send_left+2*sizeof(int)+sizeof(float)) <= TCPBUFSIZE) {
		  first = MARK_DATA + i;
		  memcpy(&sendBuffer[send_left],&first,sizeof(int));
		  send_left += sizeof(int);

		  if (verbose > 1) printf("HANDLESERVER: sending data for offset %i dataref %s: %f \n", 
					  i, serverdata[i].datarefname, *(float *) serverdata[i].data);
		  
		  memcpy(&sendBuffer[send_left],serverdata[i].data,sizeof(float));
		  memcpy(serverdata[i].data_old,serverdata[i].data,sizeof(float));
		  send_left += sizeof(float);
		  
		} else {
		  if (verbose > 0) printf("HANDLESERVER: TCP buffer overflow sending data for offset %i dataref %s \n",
					  i, serverdata[i].datarefname);
		}
	      }
	      break;
	    case XPTYPE_DBL:
	      if ((serverdata[i].precision > 10) || (serverdata[i].precision < -10)) {
		if (*(double *) serverdata[i].data != *(double *) serverdata[i].data_old) changed = 1;
	      } else {
		if ((int) round(*(double *) serverdata[i].data * scale) != (int) round(*(double *) serverdata[i].data_old * scale)) changed = 1;
	      }
	      if (changed == 1) {
		if ((send_left+2*sizeof(int)+sizeof(double)) <= TCPBUFSIZE) {
		  first = MARK_DATA + i;
		  memcpy(&sendBuffer[send_left],&first,sizeof(int));
		  send_left += sizeof(int);

		  if (verbose > 1) printf("HANDLESERVER: sending data for offset %i dataref %s: %f \n", 
					  i, serverdata[i].datarefname,*(double *) serverdata[i].data);

		  memcpy(&sendBuffer[send_left],serverdata[i].data,sizeof(double));
		  memcpy(serverdata[i].data_old,serverdata[i].data,sizeof(double));
		  send_left += sizeof(double);
		  
		} else {
		  if (verbose > 0) printf("HANDLESERVER: TCP buffer overflow sending data for offset %i dataref %s \n",
					  i, serverdata[i].datarefname);
		}
	      }
	      break;
	    case XPTYPE_FLT_ARR:
	      changed = 0;
	      if (serverdata[i].index == -1) {
		for (j=0;j<nelements;j++) {
		  if ((serverdata[i].precision > 10) || (serverdata[i].precision < -10)) {
		    if (((float *) serverdata[i].data)[j] != ((float *) serverdata[i].data_old)[j]) changed = 1;
		  } else {
		    if ((int) roundf(((float *) serverdata[i].data)[j] * scale) != 
			(int) roundf(((float *) serverdata[i].data_old)[j] * scale)) changed = 1;
		  }

		  if ((changed == 1) && (verbose > 1)) 
		    printf("HANDLESERVER: sending data for offset %i dataref %s index %i: %f \n", 
			   i, serverdata[i].datarefname,j,((float *) serverdata[i].data)[j]);
		}
	      } else {
		if ((serverdata[i].precision > 10) || (serverdata[i].precision < -10)) {
		  if (*(float *) serverdata[i].data != *(float *) serverdata[i].data_old) changed = 1;
		} else {
		  if ((int) roundf(*(float *) serverdata[i].data * scale) != (int) roundf(*(float *) serverdata[i].data_old * scale)) changed = 1;
		}

		if ((changed == 1) && (verbose > 1)) 
		  printf("HANDLESERVER: sending data for offset %i dataref %s index %i: %f \n", 
			 i, serverdata[i].datarefname, serverdata[i].index, *(float *) serverdata[i].data);
	      }
	      if (changed == 1) {
		if ((send_left+2*sizeof(int)+nelements*sizeof(float)) <= TCPBUFSIZE) {
		  first = MARK_DATA + i;
		  memcpy(&sendBuffer[send_left],&first,sizeof(int));
		  send_left += sizeof(int);

		  memcpy(&sendBuffer[send_left],serverdata[i].data,nelements*sizeof(float));
		  memcpy(serverdata[i].data_old,serverdata[i].data,nelements*sizeof(float));
		  send_left += nelements*sizeof(float);

		} else {
		  if (verbose > 0) printf("HANDLESERVER: TCP buffer overflow sending data for offset %i dataref %s \n",
					  i, serverdata[i].datarefname);
		}
	      }
	      break;
	    case XPTYPE_INT_ARR:
	      changed = 0;
	      if (serverdata[i].index == -1) {
		for (j=0;j<nelements;j++) {
		  if (((int *) serverdata[i].data)[j] != ((int *) serverdata[i].data_old)[j]) changed = 1;

		  if ((changed == 1) && (verbose > 1)) 
		    printf("HANDLESERVER: sending data for offset %i dataref %s index %i: %i \n", 
			   i, serverdata[i].datarefname,j,((int *) serverdata[i].data)[j]);
		}
	      } else {
		if (*(int *) serverdata[i].data != *(int *) serverdata[i].data_old) changed = 1;


		if ((changed == 1) && (verbose > 1)) 
		  printf("HANDLESERVER: sending data for offset %i dataref %s index %i: %i \n", 
			 i, serverdata[i].datarefname, serverdata[i].index, *(int *) serverdata[i].data);
	      }
	      if (changed == 1) {
		if ((send_left+2*sizeof(int)+nelements*sizeof(int)) <= TCPBUFSIZE) {
		  first = MARK_DATA + i;
		  memcpy(&sendBuffer[send_left],&first,sizeof(int));
		  send_left += sizeof(int);

		  memcpy(&sendBuffer[send_left],serverdata[i].data,nelements*sizeof(int));
		  memcpy(serverdata[i].data_old,serverdata[i].data,nelements*sizeof(int));
		  send_left += nelements*sizeof(int);

		} else {
		  if (verbose > 0) printf("HANDLESERVER: TCP buffer overflow sending data for offset %i dataref %s \n",
					  i, serverdata[i].datarefname);
		}
	      }
	      break;
	    case XPTYPE_BYTE_ARR:
	      changed = 0;
	      if (serverdata[i].index == -1) {
		for (j=0;j<nelements;j++) {
		  if (((unsigned char *) serverdata[i].data)[j] != ((unsigned char *) serverdata[i].data_old)[j]) changed = 1;

		  if ((changed == 1) && (verbose > 1)) printf("HANDLESERVER: sending data for offset %i dataref %s index %i: %u \n",
							      i, serverdata[i].datarefname,j,((unsigned char *) serverdata[i].data)[j]);
		}
	      } else {
		if (*(unsigned char *) serverdata[i].data != *(unsigned char *) serverdata[i].data_old) changed = 1;

		if ((changed == 1) && (verbose > 1)) printf("HANDLESERVER: sending data for offset %i dataref %s index %i: %u \n",
							    i, serverdata[i].datarefname, serverdata[i].index, *(unsigned char *) serverdata[i].data);
	      }
	      if (changed == 1) {
		if ((send_left+2*sizeof(int)+nelements*sizeof(unsigned char)) <= TCPBUFSIZE) {
		  first = MARK_DATA + i;
		  memcpy(&sendBuffer[send_left],&first,sizeof(int));
		  send_left += sizeof(int);

		  memcpy(&sendBuffer[send_left],serverdata[i].data,nelements*sizeof(unsigned char));
		  memcpy(serverdata[i].data_old,serverdata[i].data,nelements*sizeof(unsigned char));
		  send_left += nelements*sizeof(unsigned char);

		} else {
		  if (verbose > 0) printf("HANDLESERVER: TCP buffer overflow sending data for offset %i dataref %s \n",
					  i, serverdata[i].datarefname);
		}
	      }
	      break;
	    case XPTYPE_CMD_ONCE:
	    case XPTYPE_CMD_HOLD:
	      if (*(int *) serverdata[i].data != *(int *) serverdata[i].data_old) changed = 1;
	      if (changed == 1) {
		if ((send_left+3*sizeof(int)) <= TCPBUFSIZE) {
		  first = MARK_DATA + i;
		  memcpy(&sendBuffer[send_left],&first,sizeof(int));
		  send_left += sizeof(int);

		  if (verbose > 1) printf("HANDLESERVER: sending data for offset %i command %s: %i \n", 
					  i, serverdata[i].datarefname,*(int *) serverdata[i].data);
		  
		  memcpy(&sendBuffer[send_left],serverdata[i].data,sizeof(int));
		  memcpy(serverdata[i].data_old,serverdata[i].data,sizeof(int));
		  send_left += sizeof(int);
		  
		} else {
		  if (verbose > 0) printf("HANDLESERVER: TCP buffer overflow sending data for offset %i command %s \n",
					  i, serverdata[i].datarefname);
		}
	      }
	      break;
	    default:
	      break;
	    }

	    if (changed == 1) serverdata[i].nsend++;
	    
	    break;

	  default:
	    break;

	  } /* status of serverdata element */

	} /* valid dataref */

      } /* loop allocated elements of serverdata */

      if (send_left > 0) {
	
	/* add end of transmission marker */
	datai = MARK_EOT;
	memcpy(&sendBuffer[send_left],&datai,sizeof(int));
	send_left += sizeof(int);

	if (verbose > 1) printf("HANDLESERVER: TOTAL sending %i bytes \n",send_left);
	
	message_ptr = sendBuffer;
	while ((send_left > 0) && (socketStatus == status_Connected)) {
	  
	  sendMsgSize = send(clntSock, message_ptr, send_left, 0);
#ifdef WIN
	  int wsaerr = WSAGetLastError();
#endif
	  if (sendMsgSize == -1) {
#ifdef WIN
	    if (wsaerr == WSAEWOULDBLOCK) {
#else
	    if (errno == EWOULDBLOCK) {
#endif
	      if (verbose > 1) printf("HANDLESERVER: Caught EAGAIN signal sending data to Server\n");
	    } else {
	      if (verbose > 0) printf("HANDLESERVER: Error Sending data to Server \n");
	      socketStatus = status_Error;	/* signal a transmission error */
	    }
	  } else {
	    if (verbose > 2) printf("HANDLESERVER: Partial sending %i bytes \n",sendMsgSize);
	    send_left -= sendMsgSize;
	    message_ptr += sendMsgSize;
	  }

	} 
	  
	  
      } /* data to send */

      /*
      gettimeofday(&tval_after, NULL);
      timersub(&tval_after, &tval_before, &tval_result);
      printf("Time elapsed: %li\n",tval_result.tv_usec);
      */
      
    } /* if serverdata is allocated */
    
  } /* if we have a valid connection */
  
  return socketStatus;

}

/* handleclient.c sends and Receives flight data via TCP/IP to/from clients connected to the server.

   Copyright (C) 2009 - 2014  Reto Stockli

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
#include <unistd.h>    
#include <string.h>
#include <math.h>
#include <stdbool.h>

#ifdef WIN
#include <winsock2.h>
#else
#include <errno.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#endif

#include "common.h"

#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"

#include "clientdata.h"
#include "customdata.h"
#include "handleclient.h"

/* allocation of global variables defined in handleclient.h */
int socketStatus;

void htonf (float *src, float *dst)
/* transfer floating point values from host to network byte order */
{
  uint32_t lu1, lu2;

  memcpy (&lu1, src, sizeof (uint32_t));
  lu2 = htonl (lu1);
  memcpy (dst, &lu2, sizeof (uint32_t));
  return;
}

void ntohf (float *src, float *dst)
/* transfer floating point values from network to host byte order */
{
  uint32_t lu1, lu2;

  memcpy (&lu1, src, sizeof (uint32_t));
  lu2 = ntohl (lu1);
  memcpy (dst, &lu2, sizeof (uint32_t));
  return;
}

/* send final connection terminator string to client if Plugin or X-Plane quits */
void close_client(int clntSock) {
  
  int sendMsgSize;                    /* Size of sent message */
  int send_left = 0;
  char sendBuffer[TCPBUFSIZE];        /* Buffer for send string */
  char *message_ptr;

  int datai;

  if (socketStatus == status_Connected) {

    /* append connection terminator */
    datai = MARK_DISCONNECT;
    memcpy(&sendBuffer[send_left],&datai,sizeof(int));
    send_left += sizeof(int);
    
    if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Sending connection terminator to client. \n");
    
    message_ptr = sendBuffer;
    while ((send_left > 0) && (socketStatus != status_Error)) {
      
      sendMsgSize = send(clntSock, message_ptr, send_left, 0);
      
      if (sendMsgSize == -1) {
	if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Error Sending data to client socket %i \n",clntSock);
	socketStatus = status_Error; // signal a real error.
      } else {
	send_left -= sendMsgSize;
	message_ptr += sendMsgSize;
      }
    } 	  
 
  } /* if connected */

}

void receive_client(int clntSock) {

  int recvMsgSize;                    /* Size of received message */   
  int recv_left;   
  char recvBuffer[TCPBUFSIZE];        /* Buffer for receive string */
  char *message_ptr;
  
  int retval;
  int j;
  int offset;
  int type;
  int nelements;
  int index;
  int precision;
  char datarefname[100];

  int first;
  int datai;
  float dataf;
  double datad;
  float *datavf;
  int *datavi;
  unsigned char *datab;
  
  datai = 0;
  recvMsgSize = 0;
  recvBuffer[recvMsgSize] = 0;
  message_ptr = recvBuffer;

  while (socketStatus == status_Connected) {
    
    /* Check for new data from client */
    recv_left = recv(clntSock, message_ptr, TCPBUFSIZE, 0);
#ifdef WIN
    int wsaerr = WSAGetLastError();
#endif

    if (recv_left == 0) { // disconnection
      if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Disconnected. \n", clntSock);
      socketStatus = status_Disconnected;
      recvMsgSize = 0;
      break;
    }
    if (recv_left == -1) { // nothing received: error?
#ifdef WIN
      if (wsaerr == WSAEWOULDBLOCK) { // just no data yet ...
#else
      if (errno == EWOULDBLOCK) { // just no data yet ...
#endif
	if (verbose > 2) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : No data yet. \n", clntSock);
	if (recvMsgSize == 0) break; /* else continue waiting for the end of the packet */
      } else {
#ifdef WIN
	if ((wsaerr == WSAECONNABORTED) || (wsaerr == WSAECONNREFUSED) ||
	    (wsaerr == WSAECONNRESET)   || (wsaerr == WSAEHOSTUNREACH) ||
	    (wsaerr == WSAENETDOWN)     || (wsaerr == WSAENETRESET)) { // now we have a network status
#else
	if ((errno == ECONNABORTED) || (errno == ECONNREFUSED) ||
	    (errno == ECONNRESET)   || (errno == EHOSTUNREACH) ||
	    (errno == ENETDOWN)     || (errno == ENETRESET)) { // now we have a network status
#endif
	  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Disconnected!\n",clntSock);
	  socketStatus = status_Disconnected;
	  recv_left = 0;
	  recvMsgSize = 0;
	  break;
	} else { // here a real error
	  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Error!\n",clntSock);
	  socketStatus = status_Error;	// signal a real error.
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
	if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received Total %i bytes \n",clntSock, recvMsgSize);
	break;
      } else {
	/* only partial message received */
	if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received Partial %i bytes \n",clntSock, recv_left);
      }

    }
    
  } // while (socketStatus)

  /*  received a complete transmission */
  if (recvMsgSize > 0)  {
	
    recv_left = recvMsgSize;
	
    while (recv_left > 0) {
	  
      /* decode first integer: type of transmission */
      memcpy(&first,&recvBuffer[recvMsgSize-recv_left],sizeof(int));
      recv_left -= sizeof(int);
	  
      if ((first >= MARK_DATA) && (first < MARK_LINK)) {
	/* regular data stream */
	offset = first - MARK_DATA;

	if (clientdata != NULL) {

	  if (offset<numalloc) {   

	    if (clientdata[offset].index == -1) {
	      nelements = clientdata[offset].nelements;
	    } else {
	      nelements = 1;
	    }

	    switch (clientdata[offset].type) {
	    case XPTYPE_INT:		
	      if (recv_left >= sizeof(int)) {
		if ((clientdata[offset].status == XPSTATUS_VALID) && (clientdata[offset].write == 1)) {
		  memcpy(&datai,&recvBuffer[recvMsgSize-recv_left],sizeof(int));
		  if (datai != INT_MISS) {
		    memcpy(clientdata[offset].data,&datai,sizeof(int));
		    XPLMSetDatai(clientdata[offset].dataref, datai);
		    if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received data for offset %i dataref %s: %i \n", clntSock, offset, clientdata[offset].datarefname, datai);
		  }
		} else {
		  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received offset %i dataref %s not ready/writable. Discarding data. \n", clntSock, offset, clientdata[offset].datarefname);
		}
		recv_left -= sizeof(int);
	      } else {
		if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Data stream for offset %i dataref %s incomplete. Discarding. \n", clntSock, offset, clientdata[offset].datarefname);
		recv_left = 0;
	      }
	      break;
	    case XPTYPE_FLT:
	      if (recv_left >= sizeof(float)) {
		if ((clientdata[offset].status == XPSTATUS_VALID) && (clientdata[offset].write == 1)) {
		  memcpy(&dataf,&recvBuffer[recvMsgSize-recv_left],sizeof(float));
		  if (dataf != FLT_MISS) {
		    memcpy(clientdata[offset].data,&dataf,sizeof(float));
		    XPLMSetDataf(clientdata[offset].dataref, dataf);
		    if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received data for offset %i dataref %s: %f \n", clntSock, offset, clientdata[offset].datarefname, dataf);
		  }
		} else {
		  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received offset %i dataref %s not ready/writable. Discarding data. \n", clntSock, offset, clientdata[offset].datarefname);
		}
		recv_left -= sizeof(float);
	      } else {
		if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Data stream for offset %i dataref %s incomplete. Discarding. \n", clntSock, offset, clientdata[offset].datarefname);
		recv_left = 0;
	      }
	      break;
	    case XPTYPE_DBL:
	      if (recv_left >= sizeof(double)) {
		if ((clientdata[offset].status == XPSTATUS_VALID) && (clientdata[offset].write == 1)) {
		  memcpy(&datad,&recvBuffer[recvMsgSize-recv_left],sizeof(double));
		  if (datad != DBL_MISS) {
		    memcpy(clientdata[offset].data,&datad,sizeof(double));
		    XPLMSetDatad(clientdata[offset].dataref, datad);
		    if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received data for offset %i dataref %s: %f \n", clntSock, offset, clientdata[offset].datarefname, datad);
		  }
		} else {
		  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received offset %i dataref %s not ready/writable. Discarding data. \n", clntSock, offset, clientdata[offset].datarefname);
		}
		recv_left -= sizeof(double);
	      } else {
		if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Data stream for offset %i dataref %s incomplete. Discarding. \n", clntSock, offset, clientdata[offset].datarefname);
		recv_left = 0;
	      }
	      break;
	    case XPTYPE_FLT_ARR:
	      if (recv_left >= (nelements*sizeof(float))) {
		if ((clientdata[offset].status == XPSTATUS_VALID) && (clientdata[offset].write == 1)) {
		  datavf = malloc(nelements*sizeof(float));
		  if (datavf != NULL) {
		    memcpy(datavf,&recvBuffer[recvMsgSize-recv_left],nelements*sizeof(float));
		    if (clientdata[offset].index == -1) {
		      for (j=0;j<nelements;j++) {
			if (datavf[j] != FLT_MISS) {
			  XPLMSetDatavf(clientdata[offset].dataref,&datavf[j],j,1);
			  memcpy(&((float*) clientdata[offset].data)[j],&datavf[j],sizeof(float));
			  if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received data for offset %i dataref %s index %i: %f \n", clntSock, offset, clientdata[offset].datarefname, j, datavf[j]);
			}
		      }
		    } else {
		      if (*datavf != FLT_MISS) {
			XPLMSetDatavf(clientdata[offset].dataref,datavf,clientdata[offset].index,1);
			memcpy(clientdata[offset].data,datavf,sizeof(float));
			if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received data for offset %i dataref %s index %i: %f \n", clntSock, offset, clientdata[offset].datarefname, clientdata[offset].index, *datavf);
		      }
		    }
		  }
		  free(datavf);
		} else {
		  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received offset %i dataref %s not ready/writable. Discarding data. \n", clntSock, offset, clientdata[offset].datarefname);
		}
		recv_left -= nelements*sizeof(float);
	      } else {
		if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Data stream for offset %i dataref %s incomplete. Discarding. \n", clntSock, offset, clientdata[offset].datarefname);
		recv_left = 0;
	      }
	      break;
	    case XPTYPE_INT_ARR:
	      if (recv_left >= (nelements*sizeof(int))) {
		if ((clientdata[offset].status == XPSTATUS_VALID) && (clientdata[offset].write == 1)) {
		  datavi = malloc(nelements*sizeof(int));
		  if (datavi != NULL) {
		    memcpy(datavi,&recvBuffer[recvMsgSize-recv_left],nelements*sizeof(int));
		    if (clientdata[offset].index == -1) {
		      for (j=0;j<nelements;j++) {
			if (datavi[j] != INT_MISS) {
			  XPLMSetDatavi(clientdata[offset].dataref,&datavi[j],j,1);
			  memcpy(&((int*) clientdata[offset].data)[j],&datavi[j],sizeof(int));
			  if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received data for offset %i dataref %s index %i: %i \n", clntSock, offset, clientdata[offset].datarefname, j, datavi[j]);
			}
		      }
		    } else {
		      if (*datavi != INT_MISS) {
			XPLMSetDatavi(clientdata[offset].dataref,datavi,clientdata[offset].index,1);
			memcpy(clientdata[offset].data,datavi,sizeof(int));
			if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received data for offset %i dataref %s index %i: %i \n", clntSock, offset, clientdata[offset].datarefname, clientdata[offset].index, *datavi);
		      }
		    }
		  }
		  free(datavi);
		} else {
		  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received offset %i dataref %s not ready/writable. Discarding data. \n",clntSock, offset, clientdata[offset].datarefname);
		}
		recv_left -= nelements*sizeof(int);
	      } else {
		if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Data stream for offset %i dataref %s incomplete. Discarding. \n", clntSock, offset, clientdata[offset].datarefname);
		recv_left = 0;
	      }
	      break;
	    case XPTYPE_BYTE_ARR:
	      if (recv_left >= (nelements*sizeof(unsigned char))) {
		if ((clientdata[offset].status == XPSTATUS_VALID) && (clientdata[offset].write == 1)) {
		  datab = malloc(nelements*sizeof(unsigned char));
		  if (datab != NULL) {
		    memcpy(datab,&recvBuffer[recvMsgSize-recv_left],nelements*sizeof(unsigned char));
		    if (clientdata[offset].index == -1) {
		      for (j=0;j<nelements;j++) {
			XPLMSetDatab(clientdata[offset].dataref,&datab[j],j,1);
			memcpy(&((unsigned char*) clientdata[offset].data)[j],&datab[j],sizeof(unsigned char));
			if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received data for offset %i dataref %s index %i: %u \n", clntSock, offset, clientdata[offset].datarefname, j, datab[j]);
		      }
		    } else {
		      XPLMSetDatab(clientdata[offset].dataref,datab,clientdata[offset].index,1);
		      memcpy(clientdata[offset].data,datab,sizeof(unsigned char));
		      if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received data for offset %i dataref %s index %i: %u \n",clntSock, offset, clientdata[offset].datarefname, clientdata[offset].index, *datab);
		    }
		  }
		  free(datab);
		} else {
		  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received offset %i dataref %s not ready/writable. Discarding data. \n", clntSock, offset, clientdata[offset].datarefname);
		}
		recv_left -= nelements*sizeof(unsigned char);
	      } else {
		if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Data stream for offset %i dataref %s incomplete. Discarding. \n", clntSock, offset, clientdata[offset].datarefname);
		recv_left = 0;
	      }
	      break;
	    case XPTYPE_CMD_ONCE:
	      if (recv_left >= sizeof(int)) {
		if (clientdata[offset].status == XPSTATUS_VALID) {
		  memcpy(&datai,&recvBuffer[recvMsgSize-recv_left],sizeof(int));
		  if (datai != INT_MISS) {
		    memcpy(clientdata[offset].data,&datai,sizeof(int));
		    if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received data for offset %i command %s: %i \n", clntSock, offset, clientdata[offset].datarefname, datai);
		    if (datai == 1) {
		      XPLMCommandOnce(clientdata[offset].dataref);
		    }
		  }
		} else {
		  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received offset %i command %s not ready. Discarding data. \n",clntSock, offset, clientdata[offset].datarefname);
		}
		recv_left -= sizeof(int);
	      } else {
		if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Data stream for offset %i command %s incomplete. Discarding. \n", clntSock, offset, clientdata[offset].datarefname);
		recv_left = 0;
	      }
	      break;
	    case XPTYPE_CMD_HOLD:
	      if (recv_left >= sizeof(int)) {
		if (clientdata[offset].status == XPSTATUS_VALID) {
		  memcpy(&datai,&recvBuffer[recvMsgSize-recv_left],sizeof(int));
		  if (datai != INT_MISS) {
		    memcpy(clientdata[offset].data,&datai,sizeof(int));
		    if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received data for offset %i command %s: %i \n", clntSock, offset, clientdata[offset].datarefname, datai);
		    if (datai == 1) {
		      XPLMCommandBegin(clientdata[offset].dataref);
		    } else {
		      XPLMCommandEnd(clientdata[offset].dataref);
		    }
		  }
		} else {
		  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received offset %i command %s not ready. Discarding data. \n", clntSock, offset, clientdata[offset].datarefname);
		}
		recv_left -= sizeof(int);
	      } else {
		if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Data stream for offset %i command %s incomplete. Discarding. \n", clntSock, offset, clientdata[offset].datarefname);
		recv_left = 0;
	      }
	      break;
	    default:
	      break;
	    }
		  
	  }

	}

      }
	  
      /* dataref link request */
      if ((first >= MARK_LINK) && (first < MARK_UNLINK)) {
	if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received LINK Marker \n",clntSock);
	    
	if (recv_left >= (4*sizeof(int)+sizeof(datarefname)+sizeof(clientname))) {
	  offset = first - MARK_LINK;
	  if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Offset: %i \n",clntSock,offset);
	      
	  memcpy(&type,&recvBuffer[recvMsgSize-recv_left],sizeof(int));
	  if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : data type: %i \n",clntSock,type);
	  recv_left -= sizeof(int);
	      
	  memcpy(&nelements,&recvBuffer[recvMsgSize-recv_left],sizeof(int));
	  if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : number of elements: %i \n",clntSock,nelements);
	  recv_left -= sizeof(int);
	      
	  memcpy(&index,&recvBuffer[recvMsgSize-recv_left],sizeof(int));
	  if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : index of element: %i \n",clntSock,index);
	  recv_left -= sizeof(int);
	      
	  memcpy(&precision,&recvBuffer[recvMsgSize-recv_left],sizeof(int));
	  if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : precision: %i \n",clntSock,precision);
	  recv_left -= sizeof(int);
	      
	  memcpy(&datarefname,&recvBuffer[recvMsgSize-recv_left],sizeof(datarefname));
	  if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : dataref/command: %s \n",clntSock,datarefname);
	  recv_left -= sizeof(datarefname);
	      
	  memcpy(&clientname,&recvBuffer[recvMsgSize-recv_left],sizeof(clientname));
	  if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : client name: %s \n",clntSock, clientname);
	  recv_left -= sizeof(clientname);
	      
	  /* allocate data */
	  if (allocate_clientdata(offset, type, nelements, index, precision, datarefname) == 0) {
	    /* checking client dataref info for consistency with x-plane dataref */
	    retval = check_clientdata(offset);

	    if (retval == 1) {
	      /* dataref not found in X-Plane, try allocating custom dataref */

	      /* a custom dataref which is shared from this plugin to other plugins and registered clients
		 has to start with the package name: "xpserver" */
	      if (!strncmp(datarefname,PACKAGE_NAME,strlen(PACKAGE_NAME))) {
		if (allocate_customdata(type, nelements, datarefname) == 0) {
		  /* checking client dataref info for consistency with custom dataref */
		  retval = check_clientdata(offset);
		} else {
		  snprintf(clientdata[offset].datarefname,sizeof(clientdata[offset].datarefname),
			   "Failed to create custom dataref");
		  retval = -1;
		}
	      }	else {
		snprintf(clientdata[offset].datarefname,sizeof(clientdata[offset].datarefname),
			 "Dataref not found in X-Plane and not qualified as custom dataref.");
		retval = -1;
	      }	
	    }

	    if (retval == -1) {
	      /* check failed, unset dataref pointer */
	      /* dataref name will now contain an error message */
	      /* thus wait for deallocation of data for offset until the error message has been sent */
	      clientdata[offset].dataref = NULL;
	      clientdata[offset].status = XPSTATUS_LINK;
	    }
	    
	    if (retval == 0) {
	      /* X-Plane or custom dataref found and checked */
	      clientdata[offset].status = XPSTATUS_LINK;
	    }
	  } else {
	    if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Allocation failed for dataref/command %s. \n",clntSock, datarefname);
	  }
	      	      
	} else {
	  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Link data for offset %i dataref/command %s incomplete. \n", clntSock, offset, clientdata[offset].datarefname);
	  recv_left = 0;
	}
	    
      }

      /* dataref unlink request */
      if ((first >= MARK_UNLINK) && (first < MARK_EOT)) {
	if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received UNLINK Marker \n",clntSock);
	    
	if (recv_left >= (sizeof(int)+sizeof(datarefname))) {
	  offset = first - MARK_UNLINK;
	  if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Offset: %i \n",clntSock, offset);
	      
	  memcpy(&datarefname,&recvBuffer[recvMsgSize-recv_left],sizeof(datarefname));
	  if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : dataref/command: %s \n",clntSock, datarefname);
	  recv_left -= sizeof(datarefname);

	  clientdata[offset].status = XPSTATUS_UNLINK;

	} else {
	  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Unlink data for offset %i dataref/command %s incomplete. \n", clntSock, offset, clientdata[offset].datarefname);
	  recv_left = 0;
	}
	    
      }
	  
      if (first == MARK_EOT) {
	if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received EOT Marker \n",clntSock);
      }
	  
      if (first == MARK_DISCONNECT) {
	if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Received Disconnect Marker \n",clntSock);
	socketStatus = status_Disconnected;
      }
	  
    } /* still elements present in receive buffer */
	
  } /* if connected */
  
}

void send_client(int clntSock) {

  int sendMsgSize;                    /* Size of sent message */
  char *message_ptr;
  
  int send_left;
  
  char sendBuffer[TCPBUFSIZE];        /* Buffer for send string */
  
  int i,j;
  int first;
  int datai;
  float dataf;
  double datad;
  float *datavf;
  int *datavi;
  unsigned char *datab;

  int nelements;
  int changed;
  float scale;
  int retval;
  
  send_left = 0;

  if (socketStatus == status_Connected) {

    if (clientdata != NULL) {
      
      for (i=0;i<numalloc;i++) {

	if (strcmp("",clientdata[i].datarefname)) {
	  /* check for buffer overflow writing data to TCP buffer */

	  switch (clientdata[i].status) {

	  case XPSTATUS_LINK: 
	    /* validity of dataref checked. Send result to client */
	    if ((send_left+sizeof(int)+sizeof(clientdata[i].datarefname)) <= TCPBUFSIZE) {

	      if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : sending link status: %s \n",
				       clntSock, clientdata[i].datarefname);
	      
	      first = MARK_LINK + i;
	      memcpy(&sendBuffer[send_left],&first,sizeof(int));
	      send_left += sizeof(int);
	      
	      memcpy(&sendBuffer[send_left],&clientdata[i].datarefname,sizeof(clientdata[i].datarefname));
	      send_left += sizeof(clientdata[i].datarefname);
	      
	      if (clientdata[i].dataref != NULL) {
		/* dataref checked and ok. update status to valid */
		clientdata[i].status = XPSTATUS_VALID;
		count_clientdata();
	      } else {
		/* deallocate data for invalid or inexistent dataref */
		if (deallocate_clientdata(i) != 0) {
		  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : failed to deallocate offset %i \n",clntSock, i);
		}
	      }
	    } else {
	      if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : TCP buffer overflow sending link offset %i dataref/command %s \n", clntSock, i, clientdata[i].datarefname);
	    }
	    break;

	  case XPSTATUS_UNLINK: 
	    /* dataref deallocated. Send result to client */
	    if ((send_left+sizeof(int)+sizeof(clientdata[i].datarefname)) <= TCPBUFSIZE) {
	      
	      first = MARK_UNLINK + i;
	      memcpy(&sendBuffer[send_left],&first,sizeof(int));
	      send_left += sizeof(int);
	      
	      memcpy(&sendBuffer[send_left],&clientdata[i].datarefname,sizeof(clientdata[i].datarefname));
	      send_left += sizeof(clientdata[i].datarefname);
	      
	      if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : confirming unlinking dataref/command: %s \n", clntSock, clientdata[i].datarefname);
	      /* deallocate data */
	      if (deallocate_clientdata(i) != 0) {
		if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : failed to deallocate offset %i \n",clntSock, i);
	      }
	      count_clientdata();

	    } else {
	      if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : TCP buffer overflow sending unlink offset %i dataref/command %s \n",clntSock, i, clientdata[i].datarefname);
	    }
	    break;

	  case XPSTATUS_VALID:
	    changed = 0;

	    if (clientdata[i].index == -1) {
	      nelements = clientdata[i].nelements;
	    } else {
	      nelements = 1;
	    }

	    if ((clientdata[i].precision > 10) || (clientdata[i].precision < -10)) {
	      scale = 1.0;
	    } else {
	      scale = pow(10,-clientdata[i].precision);
	    }

	    switch (clientdata[i].type) {
	    case XPTYPE_INT:
	      datai=XPLMGetDatai(clientdata[i].dataref);
	      if (*(int *) clientdata[i].data != datai) changed = 1;
	      if (changed == 1) {
		if ((send_left+3*sizeof(int)) <= TCPBUFSIZE) {
		  first = MARK_DATA + i;
		  memcpy(&sendBuffer[send_left],&first,sizeof(int));
		  send_left += sizeof(int);
		  
		  memcpy(&sendBuffer[send_left],&datai,sizeof(int));
		  memcpy(clientdata[i].data,&datai,sizeof(int));
		  send_left += sizeof(int);
		  
		  if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Sending data for offset %i dataref %s: %i \n", clntSock, i, clientdata[i].datarefname, datai);
		} else {
		  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : TCP buffer overflow sending data for offset %i dataref %s \n",clntSock, i, clientdata[i].datarefname);
		}
	      }
	      break;
	    case XPTYPE_FLT:
	      dataf=XPLMGetDataf(clientdata[i].dataref);
	      if ((clientdata[i].precision > 10) || (clientdata[i].precision < -10)) {
		if (*(float *) clientdata[i].data != dataf) changed = 1;
	      } else {	
		if ((int) roundf(*(float *) clientdata[i].data * scale) != (int) roundf(dataf * scale)) changed = 1;
	      }
	      if (changed == 1) {
		if ((send_left+2*sizeof(int)+sizeof(float)) <= TCPBUFSIZE) {
		  first = MARK_DATA + i;
		  memcpy(&sendBuffer[send_left],&first,sizeof(int));
		  send_left += sizeof(int);
		  
		  memcpy(&sendBuffer[send_left],&dataf,sizeof(float));
		  memcpy(clientdata[i].data,&dataf,sizeof(float));
		  send_left += sizeof(float);
		  
		  if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Sending data for offset %i dataref %s: %f \n", clntSock, i, clientdata[i].datarefname, dataf);
		} else {
		  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : TCP buffer overflow sending data for offset %i dataref %s \n",clntSock, i, clientdata[i].datarefname);
		}
	      }
	      break;
	    case XPTYPE_DBL:
	      datad=XPLMGetDatad(clientdata[i].dataref);
	      if ((clientdata[i].precision > 10) || (clientdata[i].precision < -10)) {
		if (*(double *) clientdata[i].data != datad) changed = 1;
	      } else {
		if ((int) round(*(double *) clientdata[i].data * scale) != (int) round(datad * scale)) changed = 1;
	      }
	      if (changed == 1) {
		if ((send_left+2*sizeof(int)+sizeof(double)) <= TCPBUFSIZE) {
		  first = MARK_DATA + i;
		  memcpy(&sendBuffer[send_left],&first,sizeof(int));
		  send_left += sizeof(int);
		  
		  memcpy(&sendBuffer[send_left],&datad,sizeof(double));
		  memcpy(clientdata[i].data,&datad,sizeof(double));
		  send_left += sizeof(double);
		  
		  if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Sending data for offset %i dataref %s: %f \n", clntSock, i, clientdata[i].datarefname, datad);
		} else {
		  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : TCP buffer overflow sending data for offset %i dataref %s \n", clntSock, i, clientdata[i].datarefname);
		}
	      }
	      break;
	    case XPTYPE_FLT_ARR:
	      datavf = malloc(nelements*sizeof(float));
	      if (datavf != NULL) {
		if (clientdata[i].index == -1) {
		  retval = XPLMGetDatavf(clientdata[i].dataref,datavf,0,nelements);
		  for (j=retval;j<nelements;j++) {
		    datavf[j] = FLT_MISS;
		  }
		  for (j=0;j<nelements;j++) {
		    if ((clientdata[i].precision > 10) || (clientdata[i].precision < -10)) {
		      if (((float *) clientdata[i].data)[j] != datavf[j]) changed = 1;
		    } else {
		      if ((int) roundf(((float *) clientdata[i].data)[j] * scale) != (int) roundf(datavf[j] * scale)) changed = 1;
		    }
		  }
		  if ((changed == 1) && (verbose > 1)) {
		    for (j=0;j<nelements;j++) {
		      fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Sending data for offset %i dataref %s index %i: %f \n", clntSock, i, clientdata[i].datarefname,j,datavf[j]);
		    }
		  }
		} else {
		  retval = XPLMGetDatavf(clientdata[i].dataref,datavf,clientdata[i].index,1);
		  if ((clientdata[i].precision > 10) || (clientdata[i].precision < -10)) {
		    if (*(float *) clientdata[i].data != *datavf) changed = 1;
		  } else {
		    if ((int) roundf(*(float *) clientdata[i].data * scale) != (int) roundf(*datavf * scale)) changed = 1;
		  }
		  if ((changed == 1) && (verbose > 1)) 
		    fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Sending data for offset %i dataref %s index %i: %f \n", clntSock, i, clientdata[i].datarefname, clientdata[i].index, *datavf);
		}

		if (changed == 1) {
		  if ((send_left+2*sizeof(int)+nelements*sizeof(float)) <= TCPBUFSIZE) {
		    first = MARK_DATA + i;
		    memcpy(&sendBuffer[send_left],&first,sizeof(int));
		    send_left += sizeof(int);

		    memcpy(&sendBuffer[send_left],datavf,nelements*sizeof(float));
		    memcpy(clientdata[i].data,datavf,nelements*sizeof(float));
		    send_left += nelements*sizeof(float);
		    
		  } else {
		    if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : TCP buffer overflow sending data for offset %i dataref %s \n",clntSock,i, clientdata[i].datarefname);
		  }
		}
	      }
	      free(datavf);
	      break;
	    case XPTYPE_INT_ARR:
	      datavi = malloc(nelements*sizeof(int));
	      if (datavi != NULL) {
		if (clientdata[i].index == -1) {
		  retval = XPLMGetDatavi(clientdata[i].dataref,datavi,0,nelements);
		  for (j=retval;j<nelements;j++) {
		    datavi[j] = INT_MISS;
		  }
		  for (j=0;j<nelements;j++) {
		    if (((int *) clientdata[i].data)[j] != datavi[j]) {
		      changed = 1;
		    }
		  }
		  if ((changed == 1) && (verbose > 1)) {
		    for (j=0;j<nelements;j++) {
		      fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Sending data for offset %i dataref %s index %i: %i \n", clntSock, i, clientdata[i].datarefname,j,datavi[j]);
		    }
		  }

		} else {
		  retval = XPLMGetDatavi(clientdata[i].dataref,datavi,clientdata[i].index,1);
		  if (*(int *) clientdata[i].data != *datavi) {
		    changed = 1;
		    if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Sending data for offset %i dataref %s index %i: %i \n", clntSock, i, clientdata[i].datarefname, clientdata[i].index, *datavi);
		  }
		}

		if (changed == 1) {
		  if ((send_left+2*sizeof(int)+nelements*sizeof(int)) <= TCPBUFSIZE) {
		    first = MARK_DATA + i;
		    memcpy(&sendBuffer[send_left],&first,sizeof(int));
		    send_left += sizeof(int);

		    memcpy(&sendBuffer[send_left],datavi,nelements*sizeof(int));
		    memcpy(clientdata[i].data,datavi,nelements*sizeof(int));
		    send_left += nelements*sizeof(int);
		    
		  } else {
		    if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : TCP buffer overflow sending data for offset %i dataref %s \n",
					     clntSock,i, clientdata[i].datarefname);
		  }
		}
	      }
	      free(datavi);
	      break;
	    case XPTYPE_BYTE_ARR:
	      datab = malloc(nelements*sizeof(unsigned char));
	      if (datab != NULL) {
		if (clientdata[i].index == -1) {
		  retval = XPLMGetDatab(clientdata[i].dataref,datab,0,nelements);
		  for (j=retval;j<nelements;j++) {
		    datab[j] = 0;
		  }
		  for (j=0;j<nelements;j++) {
		    if (((unsigned char *) clientdata[i].data)[j] != datab[j]) {
		      changed = 1;
		    }
		  }
		  if ((changed == 1) && (verbose > 1)) {
		    for (j=0;j<nelements;j++) {
		      fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Sending data for offset %i dataref %s index %i: %u \n", clntSock,i, clientdata[i].datarefname,j,datab[j]);
		    }
		  }
		} else {
		  retval = XPLMGetDatab(clientdata[i].dataref,datab,clientdata[i].index,1);
		  if (*(unsigned char *) clientdata[i].data != *datab) {
		    changed = 1;
		    if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Sending data for offset %i dataref %s index %i: %u \n", clntSock, i, clientdata[i].datarefname, clientdata[i].index, *datab);
		  }
		}

		if (changed == 1) {
		  if ((send_left+2*sizeof(int)+nelements*sizeof(unsigned char)) <= TCPBUFSIZE) {
		    first = MARK_DATA + i;
		    memcpy(&sendBuffer[send_left],&first,sizeof(int));
		    send_left += sizeof(int);

		    memcpy(&sendBuffer[send_left],datab,nelements*sizeof(unsigned char));
		    memcpy(clientdata[i].data,datab,nelements*sizeof(unsigned char));
		    send_left += nelements*sizeof(unsigned char);
		    
		  } else {
		    if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : TCP buffer overflow sending data for offset %i dataref %s \n",clntSock,i, clientdata[i].datarefname);
		  }
		}
	      }
	      free(datab);
	      break;
	    case XPTYPE_CMD_ONCE:
	      /* we need to report back that the command was executed
		 and reset the command from 1 to 0. But in order to allow for
		 state changes in other plugins we first set it to 2 and only to 0
		 after the next callback so that our external plugins know
		 that they can only execute another command after the command is
		 back to 0. The state 2 is kept internally here. It is not
		 sent to the client. */
	      datai = *(int *) clientdata[i].data;
	      if (datai == 2) {
		 // second cycle: reset to 0
		changed = 1;
		datai = 0;
		memcpy(clientdata[i].data,&datai,sizeof(int));
	      }
	      if (datai == 1) {
		// first cycle: report 2
		datai = 2; 
		memcpy(clientdata[i].data,&datai,sizeof(int));
	      }
	      if (changed == 1) {
		if ((send_left+3*sizeof(int)) <= TCPBUFSIZE) {
		  first = MARK_DATA + i;
		  memcpy(&sendBuffer[send_left],&first,sizeof(int));
		  send_left += sizeof(int);
		  
		  memcpy(&sendBuffer[send_left],&datai,sizeof(int));
		  memcpy(clientdata[i].data,&datai,sizeof(int));
		  send_left += sizeof(int);
		  
		  if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Sending data for offset %i commandref %s: %i \n", clntSock, i, clientdata[i].datarefname, datai);
		} else {
		  if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : TCP buffer overflow sending data for offset %i commandref %s \n",clntSock, i, clientdata[i].datarefname);
		}
	      }
	      break;
	    case XPTYPE_CMD_HOLD:
	      // nothing to be done here since the user releases the button at some point
	      break;
	    default:
	      break;
	    }
	    
	    break;
	      
	  default:
	    break;

	  } /* status of clientdata element */
	  
	} /* valid dataref */

      } /* loop allocated clientdata elements */

      if (send_left > 0) {
	
	/* add end of transmission marker */
	datai = MARK_EOT;
	memcpy(&sendBuffer[send_left],&datai,sizeof(int));
	send_left += sizeof(int);

	if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : TOTAL Sending %i bytes \n",
				 clntSock,send_left);	      
	
	message_ptr = sendBuffer;
	while ((send_left > 0) && (socketStatus != status_Error)) {
	  
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
	      if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Caught EAGAIN signal sending data.\n",
				       clntSock);
	    } else {
	      if (verbose > 0) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Error sending data. \n", 
				       clntSock);
	      socketStatus = status_Error;	/* signal a transmission error */
	    }
	  } else {
	    if (verbose > 1) fprintf(logfileptr,"HANDLECLIENT: Client Socket %i : Partial Sending %i bytes \n",
				     clntSock,sendMsgSize);	      
	    send_left -= sendMsgSize;
	    message_ptr += sendMsgSize;
	  }
	} 
	
      } /* data to send */

    } /* clientdata structure allocated */
    
  } /* if connected */
  
}

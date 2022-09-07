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
#include <arpa/inet.h>
#include <sys/socket.h> 
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#endif

#include "xplanebeacon.h"

#define POLL_MAX_TIMEOUT 15 /* Maximum # of seconds we accept as timeout for X-Plane to send no beacon */

/* allocation of global variables */
pthread_t poll_thread;                /* read thread */
int poll_thread_exit_code;            /* read thread exit code */
pthread_mutex_t exit_cond_lock = PTHREAD_MUTEX_INITIALIZER;
int poll_timeout_counter;

int xplanebeacon_verbose;

/* allocation of global variables from externals in header file */
char XPlaneBeaconIP[30];
short unsigned int XPlaneBeaconPort;
int XPlaneBeaconSocket;

/* definition of prototype functions */
void *poll_thread_main();

/* HOW TO TEST MULTICAST UDP RECEPTION FROM CMD LINE:
   iperf -s -u -B 239.255.1.1 -p 49707 -i 1
*/

/* set up udp socket with given server address and port */
int initialize_beacon_client(int init_verbose)
{

  xplanebeacon_verbose = init_verbose;
  
  struct sockaddr_in clientAddr;     /* Client address structure */
  struct ip_mreq mreq;
  
  // initialize Beacon Info
  memset(XPlaneBeaconIP,0,sizeof(XPlaneBeaconIP));
  XPlaneBeaconPort = 0;
  poll_timeout_counter = 0;

  /* Create a UDP socket */
  if ((XPlaneBeaconSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    printf("Cannot initialize X-Plane Beacon UDP client socket \n");
    return -1; 
  } else {
  
    u_int one = 1;
    if (setsockopt(XPlaneBeaconSocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0) {
      printf("X-Plane Beacon Client Reusing ADDR failed");
      return -1;
    }    

    /* Construct the server address structure */
    memset(&clientAddr, 0, sizeof(clientAddr));            /* Zero out structure */
    clientAddr.sin_family      = AF_INET;                  /* Internet address family */
    clientAddr.sin_addr.s_addr = inet_addr("239.255.1.1"); /* Server IP address (X-Plane Broadcast Group) */
    clientAddr.sin_port        = htons(49707);             /* Server port (X-Plane Broadcast Port) */

    /* bind socket to broadcast group */
    if (bind(XPlaneBeaconSocket, (struct sockaddr *) &clientAddr, sizeof(clientAddr)) < 0) {
      printf("X-Plane Beacon Client Bind failed\n");
      return -1;
    }

    /* construct an IGMP join request structure */
    memset(&mreq, 0, sizeof(mreq));            /* Zero out structure */
    mreq.imr_multiaddr.s_addr = inet_addr("239.255.1.1");
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    /* send an ADD MEMBERSHIP message via setsockopt */
    if ((setsockopt(XPlaneBeaconSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
		    (void*) &mreq, sizeof(mreq))) < 0) {
      printf("X-Plane Beacon Client Add to UDP Multicast Group failed");
      return -1;
    }
    
   /* set to non-blocking (1) for direct read
      set to blocking (0) for asynchronous read */
    unsigned long nSetSocketType = 0;
#ifdef WIN
    if (ioctlsocket(XPlaneBeaconSocket,FIONBIO,&nSetSocketType) < 0) {
#else
    if (ioctl(XPlaneBeaconSocket,FIONBIO,&nSetSocketType) < 0) {
#endif
      printf("X-Plane Beacon Client set to non-blocking failed\n");
      return -1;
    } else {
      if (xplanebeacon_verbose > 0) printf("X-Plane Beacon Client Socket ready\n");
    }
 
    /* set a 1 s timeout so that the thread can be terminated if ctrl-c is pressed */
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(XPlaneBeaconSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
   
    poll_thread_exit_code = 0;
    if (pthread_create(&poll_thread, NULL, &poll_thread_main, NULL)>0) {
      printf("X-Plane Beacon Client Read Poll thread could not be created.\n");
      return -1;
    }
 
    return 0; 
  }

  return 0;
}


/* end udp client connection */
void exit_beacon_client(void)
{

  poll_thread_exit_code = 1;
  if (xplanebeacon_verbose > 0) printf("X-Plane Beacon Client Joining Poll Thread\n");
  pthread_join(poll_thread, NULL);
  if (xplanebeacon_verbose > 0) printf("X-Plane Beacon Client Poll Thread Ended\n");

#ifdef WIN
  closesocket(XPlaneBeaconSocket);
#else
  close(XPlaneBeaconSocket);
#endif

}

void *poll_thread_main()
/* thread handles udp receive on the server socket by use of blocking read and a read buffer */
{

  struct becn_struct {
    unsigned char beacon_major_version;	// 1 at the time of X-Plane 10.40
    unsigned char beacon_minor_version;	// 1 at the time of X-Plane 10.40
    int application_host_id;		// 1 for X-Plane, 2 for PlaneMaker
    int version_number;			// 104103 for X-Plane 10.41r3
    unsigned int role;		        // 1 for master, 2 for extern visual, 3 for IOS
    unsigned short port;		// port number X-Plane is listening on, 49000 by default
    char computer_name[500];		// the hostname of the computer, e.g. “Joe’s Macbook”
  };
  struct becn_struct becn;

  int ret = 0;
  int bufferlen = 1000;
  unsigned char buffer[bufferlen];
  struct sockaddr_in serverAddr;     /* Server address structure */
  int addrlen = sizeof(serverAddr);

  if (xplanebeacon_verbose > 0) printf("X-Plane Beacon Client Receive thread running \n");

  while (!poll_thread_exit_code) {

    /* read call goes here (1 s timeout for blocking operation) */
    ret = recvfrom(XPlaneBeaconSocket, buffer, bufferlen, 
		   0, (struct sockaddr *) &serverAddr, &addrlen);
    if (ret == -1) {
      if ((errno == EWOULDBLOCK) || (errno == EINTR)) { /* just no data yet or our own timeout */
	//printf("UDP Poll Timeout \n");
	poll_timeout_counter += 1;
	if ((poll_timeout_counter == POLL_MAX_TIMEOUT) && (XPlaneBeaconPort != 0)) {
	  if (xplanebeacon_verbose > 0) printf("No X-Plane Beacon for %i seconds: Resetting X-Plane IP and listening ...\n",POLL_MAX_TIMEOUT);
	  memset(XPlaneBeaconIP,0,sizeof(XPlaneBeaconIP));
	  XPlaneBeaconPort = 0;
	}
      } else {
	printf("X-Plane Beacon Client Receive Error %i \n",errno);
	//poll_thread_exit_code = 1;
	//break;
      } 
    } else if (ret > 0) {
      /* read is ok */
      /* are we reading BEACON data ? */
      if (strncmp(buffer,"BECN",4)==0) {

	/* Fill X-Plane Beacon Structure */
	memcpy(&becn.beacon_major_version,&buffer[5],sizeof(becn.beacon_major_version));
	if (xplanebeacon_verbose > 2) printf("X-Plane Major Version Number: %d\n",becn.beacon_major_version);
	memcpy(&becn.beacon_minor_version,&buffer[6],sizeof(becn.beacon_minor_version));
	if (xplanebeacon_verbose > 2) printf("X-Plane Minor Version Number: %d\n",becn.beacon_minor_version);
	memcpy(&becn.application_host_id,&buffer[7],sizeof(becn.application_host_id));
	if (xplanebeacon_verbose > 2) printf("X-Plane Application ID (1=X-Plane, 2=Plane Maker): %d\n",becn.application_host_id);
	memcpy(&becn.version_number,&buffer[11],sizeof(becn.version_number));
	if (xplanebeacon_verbose > 2) printf("X-Plane Version Number: %d\n",becn.version_number);
	memcpy(&becn.role,&buffer[15],sizeof(becn.role));
	if (xplanebeacon_verbose > 2) printf("X-Plane Role (1=master,2=slave,3=IOS): %d\n",becn.role);
	memcpy(&becn.port,&buffer[19],sizeof(becn.port));
	if (xplanebeacon_verbose > 2) printf("X-Plane UDP Port: %d\n",becn.port);
	if (ret > 21) {
	  memcpy(&becn.computer_name,&buffer[21],ret-21);
	  if (xplanebeacon_verbose > 2) printf("X-Plane Computer Name: %s\n",becn.computer_name);
	}

	/* we only want the IP of the master and of X-Plane */

	if ((becn.application_host_id == 1) && (becn.role == 1)) {
	  poll_timeout_counter = 0; /* reset timeout counter */
	  
	  strcpy(XPlaneBeaconIP,inet_ntoa(serverAddr.sin_addr));   /* Server IP address */
	  XPlaneBeaconPort = ntohs(serverAddr.sin_port);     /* Server port */
	  
	  if (xplanebeacon_verbose > 1) printf("X-Plane Beacon IP: %s and Port: %i \n",XPlaneBeaconIP,XPlaneBeaconPort);
	}
      }
    } else {
      /* nothing read */
    }

    /* wait loop not needed since we have a timeout */
    //usleep(500);

  } /* while loop */
  
  /* thread was killed */
  if (xplanebeacon_verbose > 0) printf("X-Plane Beacon Client receive thread shutting down \n");

  return 0;
}


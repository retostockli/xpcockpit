/* This plugin reads & writes x-plane data to a remote client via TCP/IP using an open source data protocol
   described in the README as part of this package 

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
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/types.h> 
#include <sys/time.h>   
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#ifdef WIN
#include <winsock2.h>
#else
#include <arpa/inet.h>  
#include <sys/socket.h> 
#include <sys/ioctl.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#endif

#include "common.h"   

#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include "XPLMMenus.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"

#include "iniparser.h"    
#include "clientdata.h"    
#include "handleclient.h"
#include "xpserver.h"   

/* allocate global variables defined in common.h */
int verbose;
FILE *logfileptr;     

/* prototype functions */

void	xpserverMenuHandlerCallback(
                                   void *               inMenuRef,    
                                   void *               inItemRef);    

float	xpserverLoopCallback(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon);    

void xpserverAboutWindowCallback(
				  XPLMWindowID         inWindowID,    
				  void *               inRefcon);    
void xpserverClientWindowCallback(
				    XPLMWindowID         inWindowID,    
				    void *               inRefcon);    

void xpserverKeyCallback(
                                   XPLMWindowID         inWindowID,    
                                   char                 inKey,    
                                   XPLMKeyFlags         inFlags,    
                                   char                 inVirtualKey,    
                                   void *               inRefcon,    
                                   int                  losingFocus);    

int xpserverMouseClickCallback(
                                   XPLMWindowID         inWindowID,    
                                   int                  x,    
                                   int                  y,    
                                   XPLMMouseStatus      inMouse,    
                                   void *               inRefcon);    

PLUGIN_API int XPluginStart(
						char *		outName,
						char *		outSig,
						char *		outDesc)
{

  XPLMMenuID	xpserverMenu;
  int		xpserverMenuItem;

  int on =  1;
  int i;

  char	        XPlanePath[512];

  /* structures to parse the xpserver.ini file */
  dictionary *ini;
  char filename[] = "Resources/plugins/xpserver/xpserver.ini";
  int default_server_port = 8091;
  int default_verbose = 0;
  int server_port;

  ini = iniparser_load(filename);

  if (ini != NULL) {

    server_port = iniparser_getint(ini,"network:Server Port", default_server_port);

    verbose = iniparser_getint(ini,"general:Verbosity", default_verbose);
        
    iniparser_freedict(ini);

  } else {
    server_port = default_server_port;
    verbose = default_verbose;
  }
 
#ifdef WIN
  WSADATA wsaData;
  if (WSAStartup (MAKEWORD(2, 0), &wsaData) != 0) {
    fprintf (stderr, "WSAStartup(): Couldn't initialize Winsock.\n");
    exit (EXIT_FAILURE);
  }
#endif

  FD_ZERO(&socketSetRead);
  FD_ZERO(&socketSetMaster);

  /* define the xpserver Plugin Menu */
  strcpy(outName, "xpserver");
  strcpy(outSig, "xplanesdk.xpserver");
  strcpy(outDesc, "This plugin communicates X-Plane data to OpenCockpits, SISMO, Rasperry, Leo Bodnar and OpenGC via TCP/IP.");

  xpserverMenuItem = XPLMAppendMenuItem( XPLMFindPluginsMenu(), "xpserver", 0, 1);		
  xpserverMenu = XPLMCreateMenu( "xpserver", XPLMFindPluginsMenu(), xpserverMenuItem, xpserverMenuHandlerCallback,0);

  XPLMAppendMenuItem(xpserverMenu, "About", (void *) 0, 1);
  XPLMAppendMenuItem(xpserverMenu, "Clients", (void *) 1, 1);

  XPLMGetSystemPath(XPlanePath); 

  /* Open the log file */
  if (verbose > 0) logfileptr = fopen("xpserver.log","w+");

  if (verbose > 0) fprintf(logfileptr,"XPSERVER: xpserver log file \n");
  if (verbose > 0) fprintf(logfileptr,"XPSERVER: X-PLANE PATH: %s \n",XPlanePath);
  
  /* Create socket for incoming connections */
  socketStatus = status_Ready;

  if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
      if (verbose > 0) fprintf(logfileptr,"XPSERVER: socket() failed\n");
      socketStatus = status_Error;
    }

  if (setsockopt(servSock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(on))<0)
    {
      if (verbose > 0) fprintf(logfileptr,"XPSERVER: Could not set TCP_NODELAY \n");
      socketStatus = status_Error;
    }

  /* Construct local address structure */
  memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
  echoServAddr.sin_family = AF_INET;                /* Internet address family */
  echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
  echoServAddr.sin_port = htons(server_port);       /* Local port */

  /* lose the "bind() failed: Address already in use" error message */
  if (setsockopt(servSock,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on))) 
    { 
      if (verbose > 0) fprintf(logfileptr,"XPSERVER: setsockopt for reusing address failed\n"); 
      socketStatus = status_Error;
    }   
 
  /* Bind to the local address */
  if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
    {
      if (verbose > 0) fprintf(logfileptr,"XPSERVER: bind() failed\n");
      socketStatus = status_Error;
    }

  unsigned long nSetSocketType = NON_BLOCKING;
#ifdef WIN
  if (ioctlsocket(servSock,FIONBIO,&nSetSocketType) < 0)
#else
  if (ioctl(servSock,FIONBIO,&nSetSocketType) < 0)
#endif
    {
      if (verbose > 0) fprintf(logfileptr,"XPSERVER: Server set to non-blocking failed\n");
      socketStatus = status_Error;
    }

  /* Mark the socket so it will listen for incoming connections */
  if (listen(servSock, MAXPENDING) < 0)
    {
      if (verbose > 0) fprintf(logfileptr,"XPSERVER: listen() failed\n");
      socketStatus = status_Error;
    }

  /* add the listener to the master set */
  FD_SET(servSock,&socketSetMaster);
  /* keep track of the biggest file descriptor */
  servSockMax = servSock; /* so far, it's only this one*/
  nConnect = 0; /* number of current connections */
  
  /* reset connection slots */
  for (i=0; i<MAXCONNECT; i++) {
    strncpy(clntAddrArray[i],"", sizeof(clntAddrArray[i]));
    clntSockArray[i] = -1;
  }
  
  if (verbose > 0) fprintf(logfileptr,"XPSERVER: Server socket ready: %i (on port: %i)\n",
			   servSock, ntohs(echoServAddr.sin_port));

  /* Register our callback for once a second.  Positive intervals
   * are in seconds, negative are the negative of sim frames.  Zero
   * registers but does not schedule a callback for time. */  
  XPLMRegisterFlightLoopCallback( xpserverLoopCallback, 1, NULL);

  /* initialize clientdata structure */
  for (i=0;i<MAXCONNECT;i++) {
    clientdataArray[i] = NULL;
    numallocArray[i] = 0;
    numlinkArray[i] = 0;
    strncpy(clientnameArray[i],"", sizeof(clientnameArray[i]));
  }

  return 1;
}


PLUGIN_API void	XPluginStop(void)
{
  int j;
  int clntSock;

  XPLMUnregisterFlightLoopCallback(xpserverLoopCallback, NULL);
  
  for(j = 0; j < MAXCONNECT; j++)
    {
      /* loop through open connections */
      if (clntSockArray[j]>=0)
	{	  
	  /* close conneted client sockets */
	  clntSock = clntSockArray[j];

	  clientdata = clientdataArray[j];
	  numalloc = numallocArray[j];
	  close_client(clntSock);
	  clear_clientdata();
	  clientdataArray[j] = clientdata;
	  numallocArray[j] = numalloc;
	  numlinkArray[j] = numlink;
	  strncpy(clientnameArray[j],"",sizeof(clientnameArray[j]));

	  if (verbose > 0) fprintf(logfileptr,"XPSERVER: Clear client socket: %i \n",clntSockArray[j]);
#ifdef WIN
	  closesocket(clntSockArray[j]);
#else
	  close(clntSockArray[j]);
#endif
	}
    }
  /* at the end: also close the server socket */
  if (verbose > 0) fprintf(logfileptr,"XPSERVER: Clear server socket: %i \n",servSock);
#ifdef WIN
  closesocket(servSock);
  WSACleanup();
#else
  close(servSock);
#endif

  if (verbose > 0) fclose(logfileptr);

  /* close any remaining window */
  if (xpserverWindow) {
    XPLMDestroyWindow(xpserverWindow);
    xpserverWindow = NULL;
    xpserverCloseWindow = 0;
  }

}

PLUGIN_API void XPluginDisable(void)
{
}

PLUGIN_API int XPluginEnable(void)
{
  return 1;
}

PLUGIN_API void XPluginReceiveMessage(
					XPLMPluginID	inFromWho,
					long		inMessage,
					void *		inParam)
{
}

void	xpserverMenuHandlerCallback(
                                   void *               inMenuRef,    
                                   void *               inItemRef)
{

  if (xpserverWindow) {
    XPLMDestroyWindow(xpserverWindow);
    xpserverWindow = NULL;
    xpserverCloseWindow = 0;
  }

  switch((long) inItemRef) {
  case 0: 
    {
      /* create the "About" window */
      xpserverWindow = 
	XPLMCreateWindow( 50, 600, 250, 540, 1,				
			  xpserverAboutWindowCallback,	
			  xpserverKeyCallback,
			  xpserverMouseClickCallback,
			  NULL);			      	
    }
    break;
  case 1: 
    {
      /* create the "Client" list window */
      xpserverWindow = 
	XPLMCreateWindow( 50, 600, 400, 400, 1,				
			  xpserverClientWindowCallback,	
			  xpserverKeyCallback,
			  xpserverMouseClickCallback,
			  NULL);		
    }
    break;
  }

}

float	xpserverLoopCallback(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon)
{


  int i,j;
  int clntLen = sizeof (echoClntAddr);            /* Length of client address data structure */
  int on =  1;

  int clntSock;
  struct timeval timeout = {0}; /* TCP/IP Timeout parameters */
  timeout.tv_sec = 0;		//no timeout!
  timeout.tv_usec = 0;		//no timeout!

  /* copy Master into temporary file descriptor list */
  socketSetRead = socketSetMaster;

  /* First in the loop, check for a connecting client */
  /* work with temporary descriptor list */
  if ((select(servSockMax+1,&socketSetRead,NULL,NULL,&timeout)>0) && 
      FD_ISSET(servSock,&socketSetRead) && (nConnect < MAXCONNECT))
    {
      clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen);
      
      if (clntSock >= 0) {      
	if (verbose > 0) fprintf(logfileptr,"XPSERVER: Handling client socket %i from address %s on port %i\n", 
				 clntSock, inet_ntoa(echoClntAddr.sin_addr), ntohs(echoClntAddr.sin_port));
	
	FD_SET(clntSock, &socketSetMaster); /* add new client to master descriptor list */
	if(clntSock > servSockMax)
	  { /* keep track of the maximum socket descriptor (new client socket in this case) */
	    servSockMax = clntSock;
	  }

	/* keep track of the new connection */
	i=0;
	while ((i < MAXCONNECT) && (clntSockArray[i]>=0)) i++;
	clntSockArray[i] = clntSock;
	strncpy(clntAddrArray[i],inet_ntoa(echoClntAddr.sin_addr),sizeof(clntAddrArray[i]));
	nConnect++;
				
	/* initialize new connection with non-blocking and tcp/ip */
	unsigned long nSetSocketType = NON_BLOCKING;
#ifdef WIN
	if (ioctlsocket(clntSock,FIONBIO,&nSetSocketType) < 0)
#else
	if (ioctl(clntSock,FIONBIO,&nSetSocketType) < 0)
#endif
	  {
	    if (verbose > 0) fprintf(logfileptr,"XPSERVER: Client set to non-blocking failed\n");
	  }
	if (setsockopt(clntSock, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on))<0)
	  {
	    if (verbose > 0) fprintf(logfileptr,"XPSERVER: Could not set Client TCP_NODELAY \n");
	  }
	
	
      } else {
	if (verbose > 0) fprintf(logfileptr,"XPSERVER: Error Connecting %i\n", errno);
      }
    }

  
  for (j = 0; j < MAXCONNECT; j++) {
    /* loop through open connections */
    if (clntSockArray[j]>=0) {	  
      
      /* set data pointers to current client */

      /* assume client is still connected */
      socketStatus = status_Connected;
      
      /* exchange data with client */
      clientdata = clientdataArray[j];
      numalloc = numallocArray[j];
      numlink = numlinkArray[j];
      strncpy(clientname,clientnameArray[j],sizeof(clientname));

      receive_client(clntSockArray[j]);
      send_client(clntSockArray[j]);

      /* check for terminated connections or get rid of faulty connections (reconnect later) */
      if ((socketStatus == status_Disconnected) || (socketStatus == status_Error)) {

	/* socket not available - disconnected? */
	/* close it... */	
#ifdef WIN
	closesocket(clntSockArray[j]);
#else
	close(clntSockArray[j]);
#endif

	/* remove from master set */
	if (verbose > 0) fprintf(logfileptr,"XPSERVER: Clear client socket: %i \n",clntSockArray[j]);
	FD_CLR(clntSockArray[j], &socketSetMaster);
	
	/* remove connection socket from list */
	clntSockArray[j] = -1;
	strncpy(clntAddrArray[j],"", sizeof(clntAddrArray[j]));
	nConnect--;
	socketStatus = status_Disconnected;
	
	/* clear list of active offsets for this client */
	/* new connection will then fill this list again */
	clear_clientdata();
	strncpy(clientname,"", sizeof(clientname));
      }
      
      clientdataArray[j] = clientdata;
      numallocArray[j] = numalloc;
      numlinkArray[j] = numlink;
      strncpy(clientnameArray[j],clientname,sizeof(clientname));
    }
  }

  if ((xpserverWindow) && (xpserverCloseWindow == 1)) {
    XPLMDestroyWindow(xpserverWindow);
    xpserverWindow = NULL;
    xpserverCloseWindow = 0;
  }
  
  /* force write buffer to log file */
  if (verbose > 0) fflush(logfileptr);

  // sleep(1);

  /* Register the next callback in 0.02 seconds (20 ms) from now */
  return 0.02;

}

/*
 * AboutWindowCallback
 * 
 * This callback does the work of drawing our window once per sim cycle each time
 * it is needed.  It dynamically changes the text depending on the saved mouse
 * status.  Note that we don't have to tell X-Plane to redraw us when our text
 * changes; we are redrawn by the sim continuously.
 * 
 */
void xpserverAboutWindowCallback(
                                   XPLMWindowID         inWindowID,    
                                   void *               inRefcon)
{
	int	left, top, right, bottom;
	float	color[] = { 1.0, 1.0, 1.0 }; 	/* RGB White */
	char    versionstring[40] = "Version ";

	strncat(versionstring,VERSION,10);

	/* First we get the location of the window passed in to us. */
	XPLMGetWindowGeometry(inWindowID, &left, &top, &right, &bottom);
	
	/* We now use an XPLMGraphics routine to draw a translucent dark
	 * rectangle that is our window's shape. */
	XPLMDrawTranslucentDarkBox(left, top, right, bottom);

	/* Finally we draw the text into the window, also using XPLMGraphics
	 * routines.  The NULL indicates no word wrapping. */
	XPLMDrawString(color, left + 5, top - 20, 
		(char*)("XPSERVER"), NULL, xplmFont_Basic);
	XPLMDrawString(color, left + 5, top - 35, 
		(char*) versionstring, NULL, xplmFont_Basic);
	XPLMDrawString(color, left + 5, top - 50, 
		(char*)("by Reto Stöckli and Hans Jansen"), NULL, xplmFont_Basic);
}                                   

/*
 * ClientWindowCallback
 * 
 * This callback does the work of drawing our window once per sim cycle each time
 * it is needed.  It dynamically changes the text depending on the saved mouse
 * status.  Note that we don't have to tell X-Plane to redraw us when our text
 * changes; we are redrawn by the sim continuously.
 * 
 */
void xpserverClientWindowCallback(
                                   XPLMWindowID         inWindowID,    
                                   void *               inRefcon)
{
  int left, top, right, bottom;
  int i, j;
  char str[6];
  float	colorwhite[] = { 1.0, 1.0, 1.0 }; 	/* RGB White */
  float	colorred[] = { 1.0, 0.2, 0.2 }; 	/* RGB Red */
  
  /* First we get the location of the window passed in to us. */
  XPLMGetWindowGeometry(inWindowID, &left, &top, &right, &bottom);
  
  /* We now use an XPLMGraphics routine to draw a translucent dark
   * rectangle that is our window's shape. */
  XPLMDrawTranslucentDarkBox(left, top, right, bottom);
  
  /* Finally we draw the text into the window, also using XPLMGraphics
   * routines.  The NULL indicates no word wrapping. */
  XPLMDrawString(colorwhite, left + 5, top - 20, 
		 (char*)("Connected Clients:"), NULL, xplmFont_Basic);
  XPLMDrawString(colorwhite, left + 5, top - 35, 
		 (char*)("#"), NULL, xplmFont_Basic);
  XPLMDrawString(colorwhite, left + 25, top - 35, 
		 (char*)("Client Name"), NULL, xplmFont_Basic);
  XPLMDrawString(colorwhite, left + 175, top - 35, 
		 (char*)("Client IP"), NULL, xplmFont_Basic);
  XPLMDrawString(colorwhite, left + 270, top - 35, 
		 (char*)("# Datarefs"), NULL, xplmFont_Basic);
  
  i = 0;
  for(j = 0; j < MAXCONNECT; j++)
    {
      /* loop through open connections */
      if (clntSockArray[j]>=0) {
	i++;
	snprintf(str, 6, "%d.", i);
	XPLMDrawString(colorred, left + 5, top - 35 - 15*i, 
		       (char*) str, NULL, xplmFont_Basic);
	XPLMDrawString(colorred, left + 25, top - 35 - 15*i, 
		       (char*) clientnameArray[j], NULL, xplmFont_Basic);
	XPLMDrawString(colorred, left + 175, top - 35 - 15*i, 
		       (char*) clntAddrArray[j], NULL, xplmFont_Basic);
	snprintf(str, 6, "%d", numlinkArray[j]);
	XPLMDrawString(colorred, left + 270, top - 35 - 15*i, 
		       (char*) str, NULL, xplmFont_Basic);

      }
    }
		
}                                   

/*
 * xpserverKeyCallback
 * 
 * Our key handling callback does nothing in this plugin.  This is ok; 
 * we simply don't use keyboard input.
 * 
 */
void xpserverKeyCallback(
                                   XPLMWindowID         inWindowID,    
                                   char                 inKey,    
                                   XPLMKeyFlags         inFlags,    
                                   char                 inVirtualKey,    
                                   void *               inRefcon,    
                                   int                  losingFocus)
{
}                                   

/*
 * xpserverMouseClickCallback
 * 
 * Our mouse click callback toggles the status of our mouse variable 
 * as the mouse is clicked. We don't use the mouse as input.
 * 
 */
int xpserverMouseClickCallback(
                                   XPLMWindowID         inWindowID,    
                                   int                  x,    
                                   int                  y,    
                                   XPLMMouseStatus      inMouse,    
                                   void *               inRefcon)
{
  int left, top, right, bottom;
  int retval = 0;
   
  if (xpserverWindow) {

    if (inMouse == xplm_MouseDown) {
      
      /* First we get the location of the window passed in to us. */
      XPLMGetWindowGeometry(xpserverWindow, &left, &top, &right, &bottom);
      
      if ((x >= left) && (x <= right) && (y >= bottom) && (y <= top)) {
	/* set flag to close the window if clicked */
	xpserverCloseWindow = 1;
	retval = 1;
      } 
      
    }

  }
  
  return retval;

}                                      

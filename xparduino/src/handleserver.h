/* This is the handleserver.h header to the handleserver.c code

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

#ifdef WIN
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

/* TCP/IP DEFINE STATEMENTS */
#define BLOCKING 0
#define NON_BLOCKING 1

#define TCPBUFSIZE      1000000   /* how much memory (bytes) do we need at maximum for a single transmission */

#define MARK_DATA       1000000
#define MARK_LINK       2000000
#define MARK_UNLINK     3000000
#define MARK_EOT        4000000
#define MARK_DISCONNECT 5000000

/* The socket is instantiated, bound, and ready	to connect. */
#define status_Ready 1  
/* The socket is attempting to connect, but has not yet.  Or
 * an incoming connection has come in. */
#define status_Connecting 2
/* The socket is connected and ready to send data. */
#define status_Connected 3
/* The other side is disconnected, either because the connection
   failed, the connection was disconnected in a disorderly manner
   from either side, or because both sides issued an orderly
   disconnect. */
#define status_Disconnected 4
/* An unknown error has happened; the socket is doomed. */
#define status_Error 5

/* TCP/IP CLIENT PARAMETERS */
/* Global variable declaration */
extern char server_ip[30];
extern short int server_port;

extern char recvBuffer[TCPBUFSIZE];        /* Buffer for receive string */
extern char sendBuffer[TCPBUFSIZE];        /* Buffer for send string */

extern struct sockaddr_in ServAddr;     /* Server address structure */
extern int socketStatus;                /* Socket status 1-5 */
extern int clntSock;                    /* client socket descriptor */
extern int check_tcpip_counter;  /* only check for server every x calls to check_server */


/* TCP/IP PROTOTYPE FUNCTIONS */

/* Prototype functions for Network communication */
int initialize_tcpip(void);
void exit_tcpip(void);
int check_server(void);
int send_server(void);
int receive_server(void);

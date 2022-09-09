/* This is the handleserver.h header to the handleserver.c code

   Copyright (C) 2009 - 2022  Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. 
*/

#define MARK_DATA       1000000
#define MARK_LINK       2000000
#define MARK_UNLINK     3000000
#define MARK_EOT        4000000
#define MARK_DISCONNECT 5000000

/* The socket is connected to XPServer and ready to send/receive data. */
#define status_Connected 1
/* The socket is instantiated, bound, and ready	to connect. */
#define status_Ready 2  
/* The socket is not ready */
#define status_Init 3
/* An unknown error has happened; the socket is doomed. */
#define status_Error 4

/* TCP/IP CLIENT PARAMETERS */
/* Global variable declaration */
extern char XPlaneServerIP[30];
extern short unsigned int XPlaneServerPort;

/* TCP/IP PROTOTYPE FUNCTIONS */

/* Prototype functions for Network communication */
int initialize_tcpip_client(int init_verbose);
int create_tcpip_socket(void);
void exit_tcpip_client(void);
int check_xpserver(void);
int send_xpserver(void);
int receive_xpserver(void);

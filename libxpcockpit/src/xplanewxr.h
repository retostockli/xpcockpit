/* This is the xplanewxr.h header to the xplanewxr.c code

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

#ifdef WIN
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

/* UDP CLIENT PARAMETERS */
extern char wxrServerIP[30];
extern int wxrServerPort;
extern int wxrSocket;

/* thread parameters */
extern pthread_t wxr_poll_thread;                /* read thread */
extern int wxr_poll_thread_exit_code;            /* read thread exit code */
extern pthread_mutex_t wxr_exit_cond_lock;

extern struct sockaddr_in wxrServerAddr;     /* Server address structure */
extern struct sockaddr_in wxrClientAddr;     /* Client address structure */

extern int wxrSendBufferLen;
extern int wxrRecvBufferLen;
extern char *wxrSendBuffer;
extern char *wxrRecvBuffer;
extern int wxrReadLeft;                      /* counter of bytes to read from receive thread */
extern int wxr_is_xp12;   /* WXR data is from XP12 instead from XP11 */

/* Prototype functions */
int init_wxr_server(void);
int init_wxr_client(void);
int init_wxr_receive();
void *wxr_poll_thread_main();
void exit_wxr_server(void);
void exit_wxr_client(void);
int send_wxr_to_server(void);
int recv_wxr_from_server(void);
int send_wxr_to_client(char client_ip[],int client_port,unsigned char data[], int len);
int recv_wxr_from_client(void);

void allocate_wxrdata(int sendlen,int recvlen);
void get_wxrdata(char *data, int len);
void deallocate_wxrdata();

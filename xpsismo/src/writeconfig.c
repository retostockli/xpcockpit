/*
 * Write Network and Daughter Configuration to the SISMO SC-MB card with the new Firmware
 * Will flash this data onto the PIC18F86J60 Chip. Boards needs to be re-powered in order
 * for the new network configuration to become effective.
 *
 * Run:
 *     ./writeconfig CFGFILE BOARD-IP BOARD-PORT
 *
 * Where:
 *   - CFGFILE: the prefix of the .cfg file located in ../inidata
 *   - BOARD-IP: the IP Address where the SC-MB Card is currently reachable. Format: 192.168.1.55
 *   - BOARD-PORT: the UDP Port where the SC-MB Card is currently reachable. Format: 1024
 *
 * The board will respond with a confirmation that this data was written. Without confirmation the code will print an error
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "iniparser.h"


#define SERVER_PORT 1026
#define CLIENT_PORT 1024
#define CLIENT_IP "192.168.1.55"
#define SENDBUFFER_SIZE 28
#define RECVBUFFER_SIZE 30

int serverSocket;

/* send datagram to UDP client */
int send_udp(char client_ip[], int client_port, unsigned char data[], int len) {

  struct sockaddr_in udpClientAddr;     /* Client address structure */
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
  
  n = sendto(serverSocket, data, len, 0,
	     (struct sockaddr *) &udpClientAddr, sizeof(udpClientAddr));

  return n;
}

int main(int argc, char **argv)
{
  struct sockaddr_in serverAddr;
  struct sockaddr_in clientAddr;

  socklen_t clientLen = sizeof(clientAddr);

  unsigned char sendbuffer[SENDBUFFER_SIZE];
  unsigned char recvbuffer[RECVBUFFER_SIZE];

  int ret;
  int len;

  char cfgfile[200];
  char sismo_ip[30];
  int16_t sismo_port;

  dictionary *cfg;

  char client_ipaddress[30];
  char client_subnetmask[30];
  char client_gateway[30];
  char client_macaddress[30];
  char server_ipaddress[30];
  int16_t client_port;
  int16_t server_port;
  int8_t daughter_input1;
  int8_t daughter_input2;
  int8_t daughter_analoginput;
  int8_t daughter_output1;
  int8_t daughter_output2;
  int8_t daughter_servo;
  int8_t daughter_display1;
  int8_t daughter_display2;


  /* evaluate command line arguments */
  argc--;
  if (argc != 3) {
    printf("Invalid number of arguments. Please run with : \n");
    printf("./writeconfig CFGFILE BOARD-IP BOARD-PORT \n");
    printf(" \n");
    printf(" Where: \n");
    printf("   - CFGFILE: the prefix of the .cfg file located in ../inidata \n");
    printf("   - BOARD-IP: the IP Address where the SC-MB Card is currently reachable. Format: 192.168.1.55 \n");
    printf("   - BOARD-PORT: the UDP Port where the SC-MB Card is currently reachable. Format: 1024 \n");      
    exit(-1);
  }

  snprintf(cfgfile, sizeof(cfgfile), "../inidata/%s.cfg",argv[1]);
  strncpy(sismo_ip,argv[2],sizeof(sismo_ip));
  sismo_port = (uint16_t) atoi(argv[3]);
    
  printf("CFG File:   %s \n",cfgfile);
  printf("SISMO IP:   %s \n",sismo_ip);
  printf("SISMO PORT: %i \n",sismo_port);
    
  cfg = iniparser_load(cfgfile);

  if (cfg != NULL) {
    //verbose = iniparser_getint(ini,"general:verbose", default_verbose);
 
    strncpy(client_ipaddress,iniparser_getstring(cfg,":CLIENT_IPADDRESS", ""),sizeof(client_ipaddress));
    strncpy(client_subnetmask,iniparser_getstring(cfg,":CLIENT_SUBNETMASK", ""),sizeof(client_subnetmask));
    strncpy(client_gateway,iniparser_getstring(cfg,":CLIENT_GATEWAY", ""),sizeof(client_gateway));
    strncpy(client_macaddress,iniparser_getstring(cfg,":CLIENT_MACADDRESS", ""),sizeof(client_macaddress));
    strncpy(server_ipaddress,iniparser_getstring(cfg,":SERVER_IPADDRESS", ""),sizeof(server_ipaddress));
    client_port = iniparser_getint(cfg,":CLIENT_PORT", 0);
    server_port = iniparser_getint(cfg,":SERVER_PORT", 0);
    daughter_input1 = iniparser_getint(cfg,":DAUGHTER_INPUT1", 0);
    daughter_input2 = iniparser_getint(cfg,":DAUGHTER_INPUT2", 0);
    daughter_analoginput = iniparser_getint(cfg,":DAUGHTER_ANALOGINPUT", 0);
    daughter_output1 = iniparser_getint(cfg,":DAUGHTER_OUTPUT1", 0);
    daughter_output2 = iniparser_getint(cfg,":DAUGHTER_OUTPUT2", 0);
    daughter_servo = iniparser_getint(cfg,":DAUGHTER_SERVO", 0);
    daughter_display1 = iniparser_getint(cfg,":DAUGHTER_DISPLAY1", 0);
    daughter_display2 = iniparser_getint(cfg,":DAUGHTER_DISPLAY2", 0);

    printf("CLIENT IPADDRESS:   %s \n",client_ipaddress);
    printf("CLIENT SUBNETMASK:  %s \n",client_subnetmask);
    printf("CLIENT GATEWAY:     %s \n",client_gateway);
    printf("CLIENT MACADDRESS:  %s \n",client_macaddress);
    printf("SERVER IPADDRESS:   %s \n",client_ipaddress);
    printf("CLIENT PORT:        %i \n",client_port);
    printf("SERVER PORT:        %i \n",server_port);
    printf("DAUGHTER INPUT1:    %i \n",daughter_input1);
    printf("DAUGHTER INPUT2:    %i \n",daughter_input2);
    printf("DAUGHTER ANA INPUT: %i \n",daughter_analoginput);
    printf("DAUGHTER OUTPUT1:   %i \n",daughter_output1);
    printf("DAUGHTER OUTPUT2:   %i \n",daughter_output2);
    printf("DAUGHTER SERVO:     %i \n",daughter_servo);
    printf("DAUGHTER DISPLAY1:  %i \n",daughter_display1);
    printf("DAUGHTER DISPLAY2:  %i \n",daughter_display2);

    
    printf("\n");
    iniparser_freedict(cfg);
  } else {
    printf("Configuration File %s not found. Exiting.\n",cfgfile);
    exit(-1);
  }
    
  //----------------------------------------------------------------------
  // Create UDP socket
  //----------------------------------------------------------------------

  serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

  if(serverSocket < 0)
    {
      perror("socket");
      return 1;
    }

  exit(0);
  
  //----------------------------------------------------------------------
  // Configure server address
  //----------------------------------------------------------------------

  memset(&serverAddr, 0, sizeof(serverAddr));

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(SERVER_PORT);

  //----------------------------------------------------------------------
  // Bind socket
  //----------------------------------------------------------------------

  if(bind(serverSocket,
	  (struct sockaddr*)&serverAddr,
	  sizeof(serverAddr)) < 0)
    {
      perror("bind");
      close(serverSocket);
      return 1;
    }

  printf("UDP server listening on port %d...\n", SERVER_PORT);

  len = sizeof(sendbuffer);
  ret = send_udp(CLIENT_IP,CLIENT_PORT,sendbuffer,len);
  if (ret == len) {
    printf("COMPLETE: Sent %i of %i bytes to Client \n", ret,len);
  } else {
    printf("INCOMPLETE: Sent %i of %i bytes to Client \n", ret,len);
  }

  //----------------------------------------------------------------------
  // Receive loop
  //----------------------------------------------------------------------
    
  while(1)
    {
      ssize_t receivedBytes;

      memset(recvbuffer, 0, RECVBUFFER_SIZE);

      receivedBytes = recvfrom(serverSocket,
			       recvbuffer,
			       RECVBUFFER_SIZE - 1,
			       0,
			       (struct sockaddr*)&clientAddr,
			       &clientLen);

      if(receivedBytes < 0)
        {
	  perror("recvfrom");
	  continue;
        }

      //------------------------------------------------------------------
      // Print sender info
      //------------------------------------------------------------------

      /* printf("From %s:%d\n", */
      /*        inet_ntoa(clientAddr.sin_addr), */
      /*        ntohs(clientAddr.sin_port)); */

      /* printf("Message: %s\n", recvbuffer); */

      //len = strlen(recvbuffer);

    }

  close(serverSocket);

  return 0;
}

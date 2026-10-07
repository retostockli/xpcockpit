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


#define SERVER_PORT 1026  // Port of this UDP Server here
#define SENDBUFFER_SIZE 28 // Messages sent to SISMO SC-MB
#define RECVBUFFER_SIZE 30 // Messages received from SISMO SC-MB

#define MAKE_IPV4_ADDRESS(a,b,c,d) \
    ((uint32_t)(((uint32_t)(a) << 24) | \
                ((uint32_t)(b) << 16) | \
                ((uint32_t)(c) << 8)  | \
                (uint32_t)(d)))

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

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

bool StringToIPv4(const char *str, uint32_t *ip)
{
    uint32_t parts[4];
    char *end;

    for (uint8_t i = 0; i < 4; i++)
    {
        parts[i] = strtoul(str, &end, 10);

        if (end == str || parts[i] > 255)
            return false;

        if (i < 3)
        {
            if (*end != '.')
                return false;

            str = end + 1;
        }
        else
        {
            if (*end != '\0')
                return false;
        }
    }

    *ip = MAKE_IPV4_ADDRESS(parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);

    return true;
}

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

bool StringToMAC(const char *str, uint8_t mac[6])
{
    unsigned int m[6];
    char extra;

    if (sscanf(str,
               "%2x:%2x:%2x:%2x:%2x:%2x%c",
               &m[0], &m[1], &m[2],
               &m[3], &m[4], &m[5],
               &extra) != 6)
    {
        return false;
    }

    for (int i = 0; i < 6; i++)
    {
        mac[i] = (uint8_t)m[i];
    }

    return true;
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
  
  uint32_t ip;
  uint8_t mac[6];
  uint8_t daughtercards;
  
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
    printf("SERVER IPADDRESS:   %s \n",server_ipaddress);
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

  printf("This UDP server is listening on port %d...\n", SERVER_PORT);

  sendbuffer[0] = 0xFF; // Marker for Configuration Packet
  StringToIPv4(client_ipaddress, &ip);
  memcpy(&sendbuffer[1],&ip,sizeof(ip));
  StringToIPv4(client_subnetmask, &ip);
  memcpy(&sendbuffer[5],&ip,sizeof(ip));
  StringToIPv4(client_gateway, &ip);
  memcpy(&sendbuffer[9],&ip,sizeof(ip));
  StringToMAC(client_macaddress, mac);
  memcpy(&sendbuffer[13],&mac,sizeof(mac));
  StringToIPv4(server_ipaddress, &ip);
  memcpy(&sendbuffer[19],&ip,sizeof(ip));
  memcpy(&sendbuffer[23],&client_port,sizeof(client_port));
  memcpy(&sendbuffer[25],&server_port,sizeof(server_port));

  // Pack daughtercard state into a single byte (see SimCards Ehternet - UDP Protocol Document)
  // MOD: Inputs 1 is added in bit 4 and Inputs 2 is added in bit 7 (unused in original SISMO protocol)
  daughtercards =
    (daughter_output1 << 0) |
    (daughter_output2 << 1) |
    (daughter_servo << 2) |
    (daughter_display1 << 3) |
    (daughter_input1 << 4) |
    (daughter_analoginput << 5) |
    (daughter_display2 << 6) |
    (daughter_input2 << 7);
  
  sendbuffer[27] = daughtercards;
  
  len = sizeof(sendbuffer);
  ret = send_udp(sismo_ip,sismo_port,sendbuffer,len);
  if (ret == len) {
    printf("Sent Configuration to SISMO SC-MB with IP %s Port %i \n", sismo_ip,sismo_port);
    printf("Waiting for Confirmation Message from Client...\n");
  } else {
    printf("ERROR: Sent %i of %i Configuration bytes to SISMO SC-MB IP %s Port %i \n", ret,len,sismo_ip,sismo_port);
    exit(-1);
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

      if (recvbuffer[0] == 0xFF) {
	printf("Received confirmation from IP %s Port %d\n",
	       inet_ntoa(clientAddr.sin_addr),
	       ntohs(clientAddr.sin_port));
	printf("Cycle Power on SISMO SC-MB in order to make configuration effective\n");

	break;
      }

    }

  close(serverSocket);

  return 0;
}

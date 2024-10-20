/*
   UDP Client for communicating to xpteensy
 */

#define PORT 1030
#define IP "192.168.1.66"
#define MASK "255.255.255.0"
#define GATEWAY "192.168.1.1"

#include <QNEthernet.h>
#include "common.h"
#include "teensy_config.h"

using namespace qindesign::network;


// data storage
teensy_struct teensy_data;

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;


void pauseCode(const char message[]) {
  while (1) {
    Serial.println(message);
    delay(1000);
  }
}

void getAddr(const char saddr[], uint8_t addr[4]) {
	const char s[2] = ".";
  char tmp[30];
  strcpy(tmp,saddr);
  char *token;
  int i = 0;
  token = strtok(tmp, s);
  while (token) {
	  addr[i] = atoi(token);
	  token=strtok(NULL,s); 
    i++;
	}
}

void setup() {
 
  // define storage for MAC address
  uint8_t mac[6];
  uint8_t add[4];

  // Open serial communications and wait for port to open:
  if (DEBUG) {
    Serial.begin(115200);
    Serial.println("Starting Setup ...");
  }

  // start the Ethernet and fetch MAC address of interface

  // define network parameters
  getAddr(IP,add);
  IPAddress ip(add[0],add[1],add[2],add[3]);
  getAddr(MASK,add);
  IPAddress nm(add[0],add[1],add[2],add[3]);
  getAddr(GATEWAY,add);
  IPAddress gw(add[0],add[1],add[2],add[3]);
  
  Ethernet.begin(ip, nm, gw);
  Ethernet.macAddress(mac);

  // print some setup information
  if (DEBUG) {
    Serial.print("The IP address is: ");
    Serial.println(ip);
    Serial.print("The MAC address is: ");
    for (byte octet = 0; octet < 6; octet++) {
      Serial.print(mac[octet] < 16 ? "0" : "");
      Serial.print(mac[octet], HEX);
      if (octet < 5) {
        Serial.print('-');
      }
    }
    Serial.println("");

  }

  // start UDP service
  teensy_data.port = PORT;  // local port to listen on
  teensy_data.mac[0] = mac[4];
  teensy_data.mac[1] = mac[5];
  Udp.begin(teensy_data.port);
  Udp.setReceiveQueueSize(10);
  if (DEBUG) Serial.println("UDP initialized.");

  // init teensy data structure
  init();

}

void loop() {

  udp_receive();

  //udp_send();

  delay(1); /* We need a maximum of 1 ms delay to capture fast changing encoders */
}

/*
   Firmware to communicate to the xpteensy X-Plane Client.
   Made for Teensy 4.1 with ethernet controller on board.
*/

/* USER PARAMETERS PER TEENSY CONTROLLER */
#define PORT 1030
#define IP "192.168.1.66"
#define MASK "255.255.255.0"
#define GATEWAY "192.168.1.1"

//#include <Ethernet.h>
//#include <EthernetUdp.h>
#include <QNEthernet.h>
#include <Servo.h>
#include "common.h"
#include "teensy_config.h"
#include <Adafruit_MCP23X17.h>
#include <Wire.h>

using namespace qindesign::network;

extern TwoWire Wire0;
extern TwoWire Wire1;
extern TwoWire Wire2;

// data storage
teensy_struct teensy_data;
mcp23017_struct mcp23017_data[MAX_DEV];

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

IPAddress remoteIP;
int16_t remotePort;

unsigned long StartTime;
unsigned long CurrentTime;
unsigned long ElapsedTime;

void getAddr(const char saddr[], uint8_t addr[4]) {
  const char s[2] = ".";
  char tmp[30];
  strcpy(tmp, saddr);
  char *token;
  int i = 0;
  token = strtok(tmp, s);
  while (token) {
    addr[i] = atoi(token);
    token = strtok(NULL, s);
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
    Serial.printf("Starting Setup ...\n");
  }

  // start the Ethernet and fetch MAC address of interface

  // define network parameters
  getAddr(IP, add);
  IPAddress ip(add[0], add[1], add[2], add[3]);
  getAddr(MASK, add);
  IPAddress nm(add[0], add[1], add[2], add[3]);
  getAddr(GATEWAY, add);
  IPAddress gw(add[0], add[1], add[2], add[3]);

  Ethernet.onLinkState([](bool state) {
    Serial.printf("Ethernet: Link %s\n", state ? "ON" : "OFF");
  });

  Ethernet.onAddressChanged([]() {
    uint8_t mac[6];
    IPAddress ip = Ethernet.localIP();
    bool hasIP = (ip != INADDR_NONE);
    if (hasIP) {
      IPAddress subnet = Ethernet.subnetMask();
      IPAddress broadcast = Ethernet.broadcastIP();
      IPAddress gw = Ethernet.gatewayIP();
      IPAddress dns = Ethernet.dnsServerIP();
      Ethernet.macAddress(mac);

      Serial.printf("Ethernet: New IP Address:\n"
                    "    Local IP     = %u.%u.%u.%u\n"
                    "    Subnet       = %u.%u.%u.%u\n"
                    "    Broadcast IP = %u.%u.%u.%u\n"
                    "    Gateway      = %u.%u.%u.%u\n"
                    "    DNS          = %u.%u.%u.%u\n"
                    "    MAC          = %02X-%02X-%02X-%02X-%02X-%02X\n",
                    ip[0], ip[1], ip[2], ip[3],
                    subnet[0], subnet[1], subnet[2], subnet[3],
                    broadcast[0], broadcast[1], broadcast[2], broadcast[3],
                    gw[0], gw[1], gw[2], gw[3],
                    dns[0], dns[1], dns[2], dns[3],
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    } else {
      Serial.printf("Ethernet: No IP Address\n");
    }
  });

  if (Ethernet.begin(ip, nm, gw)) {
    if (DEBUG > 0) Serial.printf("Ethernet: Started.\n");
  } else {
    if (DEBUG > 0) Serial.printf("Ethernet: Failed to start\n");
  }


  Ethernet.macAddress(mac);

  // start UDP service
  teensy_data.port = PORT;  // local port to listen on
  teensy_data.mac[0] = mac[4];
  teensy_data.mac[1] = mac[5];
  Udp.begin(teensy_data.port);
  Udp.setReceiveQueueSize(100);
  if (DEBUG > 0) Serial.printf("UDP: initialized.\n");

  // init teensy data structure
  init_data();

  // Start I2C busses (one for now)
  Wire.begin();
  // Wire1.begin();
  //Wire2.begin();
  Wire.setClock(1000000);
  //Wire1.setClock(1000000);
  //Wire2.setClock(1000000);
}

void loop() {

  StartTime = micros();

  udp_receive();

  teensy_read();

  CurrentTime = micros();
  ElapsedTime = CurrentTime - StartTime;

  if ((ElapsedTime > 1000) && (DEBUG > 0)) {
    Serial.printf("Long Loop Time: %i us\n", ElapsedTime);
  }

  // Ethernet.loop();

  delay(1); /* We need a maximum of 1 ms delay to capture fast changing encoders */
}

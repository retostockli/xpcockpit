/*
 DESCRIBE THIS CODE!
 */


#include <QNEthernet.h>
using namespace qindesign::network;

#define DEBUG 1
#define MAXPACKETRECV 10
#define MAXPACKETSEND 1
#define RECVMSGLEN 4 + 4 * MAXPACKETRECV
#define SENDMSGLEN 4 + 4 * MAXPACKETSEND
#define INITVAL -1
#define NINPUTS 42
#define NANALOGINPUTS 18


// define network parameters
IPAddress ip(192, 168, 1, 66);
IPAddress nm(255, 255, 255, 1);
IPAddress gw(192, 168, 1, 1);
unsigned int localPort = 1030;  // local port to listen on

// define buffers for receiving and sending data
char recvBuffer[RECVMSGLEN];  // buffer to hold receive packet
char sendBuffer[SENDMSGLEN];  // buffer to hold send packet

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

// define storage for MAC address
byte mac[6];

// define storage for inputs
int isinput[NINPUTS];
int input[NINPUTS];
int input_old[NINPUTS];
int analoginput[NANALOGINPUTS];
int analoginput_old[NANALOGINPUTS];
int isanaloginput[NANALOGINPUTS];

void pauseCode(char message[]) {
  while (1) {
    Serial.println(message);
    delay(1000);
  }
}

void setup() {
  // Open serial communications and wait for port to open:
  if (DEBUG) {
    Serial.begin(9600);
    Serial.println("Starting Setup ...");
  }

  // start the Ethernet and fetch MAC address of interface
  Ethernet.begin(ip, nm, gw);
  Ethernet.macAddress(mac);

  // print some setup information
  if (DEBUG) {
    Serial.print("The MAC address is: ");
    for (byte octet = 0; octet < 6; octet++) {
      Serial.print(mac[octet] < 16 ? "0" : "");
      Serial.print(mac[octet], HEX);
      if (octet < 5) {
        Serial.print('-');
      }
    }
    Serial.println("");

    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      pauseCode("Error: Ethernet shield was not found.");
    }

    // That does not work, always off
    /*
    if (!Ethernet.linkState()) {
      Serial.println("Ethernet cable is not connected.");
    }
    */
  }

  // start UDP service
  Udp.begin(localPort);
  if (DEBUG) Serial.println("UDP initialized.");

  // assume no pins are selected as inputs, will be changed during initialization
  for (int i = 0; i < NINPUTS; i++) {
    isinput[i] = 0;
  }

  // Put all analog inputs to ground if not used in order to avoid just garbage transfers
  for (int i = 0; i < NANALOGINPUTS; i++) {
    isanaloginput[i] = 0;
  }
}

void loop() {

  int ivalue;
  int p;

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize > 0) {
    if (DEBUG) {
      Serial.print("Received UDP Packet with Size ");
      Serial.println(packetSize);
      Serial.print("from ");
      IPAddress remote = Udp.remoteIP();
      for (int i = 0; i < 4; i++) {
        Serial.print(remote[i], DEC);
        if (i < 3) {
          Serial.print(".");
        }
      }
      Serial.print(", port ");
      Serial.print(Udp.remotePort());
    }

    // read the packet into packetBufffer
    Udp.read(recvBuffer, RECVMSGLEN);

    // Copy payload 16 bit Integer into variable
    memcpy(&ivalue, &recvBuffer[6], 2);

    if (DEBUG) {
      Serial.print(" with Content: ");
      for (int i = 0; i < 2; i++) {
        Serial.print(recvBuffer[i]);
        Serial.print(" ");
      }
      for (int i = 4; i < 6; i++) {
        Serial.print(recvBuffer[i], DEC);
        Serial.print(" ");
      }
      Serial.println(ivalue);
    }

    if ((recvBuffer[0] == 0x41) && (recvBuffer[1] == 0x52)) {

      /* loop through data packets */
      for (p = 0; p < MAXPACKETRECV; p++) {

        memcpy(&ivalue, &recvBuffer[p * 4 + 6], 2);

        if (recvBuffer[p * 4 + 5] == 0x01) {
          /* Digital Output */
          pinMode(recvBuffer[p * 4 + 4], OUTPUT);
          digitalWrite(recvBuffer[p * 4 + 4], ivalue);
          if (DEBUG) {
            Serial.print(p, DEC);
            Serial.print(" Digital Output ");
            Serial.print(recvBuffer[p * 4 + 4], DEC);
            Serial.print(" received value: ");
            Serial.println(ivalue, DEC);
          }
        } else if (recvBuffer[p * 4 + 5] == 0x02) {
          /* Analog Output */
          pinMode(recvBuffer[p * 4 + 4], OUTPUT);
          analogWrite(recvBuffer[p * 4 + 4], ivalue);
          if (DEBUG) {
            Serial.print(p, DEC);
            Serial.print(" Analog Output ");
            Serial.print(recvBuffer[p * 4 + 4], DEC);
            Serial.print(" received value: ");
            Serial.println(ivalue, DEC);
          }
        } else if (recvBuffer[p * 4 + 5] == 0x03) {
          /* define pin mode and initialize input to send data back to server */
          isinput[recvBuffer[p * 4 + 4]] = 1;
          pinMode(recvBuffer[p * 4 + 4], INPUT_PULLUP);
          input_old[recvBuffer[p * 4 + 4]] = INITVAL;
          input[recvBuffer[p * 4 + 4]] = INITVAL;
          if (DEBUG) {
            Serial.print(p, DEC);
            Serial.print(" Initializing: Digital Input ");
            Serial.print(recvBuffer[p * 4 + 4], DEC);
            Serial.println(" will send data.");
          }
        } else if (recvBuffer[p * 4 + 5] == 0x04) {
          /* define analog input to send data back to server */
          isanaloginput[recvBuffer[p * 4 + 4]] = 1;
          analoginput_old[recvBuffer[p * 4 + 4]] = INITVAL;
          analoginput[recvBuffer[p * 4 + 4]] = INITVAL;
          if (DEBUG) {
            Serial.print(p, DEC);
            Serial.print(" Initializing: Analog Input ");
            Serial.print(recvBuffer[p * 4 + 4], DEC);
            Serial.println(" will send data.");
          }
        } else {
          /* Any other command: IGNORE */
          if (p == 0) {
            Serial.println("Transmission without content!!!");
          }
        }
      } /* loop through data packets */
    }   /* Correct receive buffer initiator string */
  }     /* Packet was received */

  /* We can only send something if we have previously received something */
  if (Udp.remotePort() != 0) {

    // Read selected digital inputs and send changed ones to host
    for (int i = 0; i < NINPUTS; i++) {
      if (isinput[i]) {
        input[i] = digitalRead(i);
      }
    }
    for (int i = 0; i < NINPUTS; i++) {
      if (isinput[i]) {
        if (input[i] != input_old[i]) {
          input_old[i] = input[i];

          if (DEBUG) {
            Serial.print("Digital Input ");
            Serial.print(i);
            Serial.print(" changed to: ");
            Serial.println(input[i]);
          }

          memset(sendBuffer, 0, SENDMSGLEN);
          sendBuffer[0] = 0x41;
          sendBuffer[1] = 0x52;
          sendBuffer[2] = mac[4];
          sendBuffer[3] = mac[5];
          sendBuffer[4] = i;
          sendBuffer[5] = 0x01;
          memcpy(&sendBuffer[6], &input[i], sizeof(ivalue));
          Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
          int ret = Udp.write(sendBuffer, SENDMSGLEN);
          Udp.endPacket();
        }
      }
    }

    // Read Analog inputs and send changed ones to host
    for (int i = 0; i < NANALOGINPUTS; i++) {
      if (isanaloginput[i] == 1) {
        ivalue = analogRead(NINPUTS + i);

        if (ivalue != analoginput[i]) {
          analoginput[i] = ivalue;

          if (DEBUG) {
            Serial.print("Analog Input ");
            Serial.print(i);
            Serial.print(" changed to: ");
            Serial.println(ivalue);
          }

          memset(sendBuffer, 0, SENDMSGLEN);
          sendBuffer[0] = 0x41;
          sendBuffer[1] = 0x52;
          sendBuffer[2] = mac[4];
          sendBuffer[3] = mac[5];
          sendBuffer[4] = i;
          sendBuffer[5] = 0x02;
          memcpy(&sendBuffer[6], &ivalue, sizeof(ivalue));

          Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
          int ret = Udp.write(sendBuffer, SENDMSGLEN);
          Udp.endPacket();
        }
      }
    }
  }
  delay(1); /* We need a maximum of 1 ms delay to capture fast changing encoders */
}

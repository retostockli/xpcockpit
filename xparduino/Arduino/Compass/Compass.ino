/*
 UDPSendReceiveString:
 This sketch receives UDP message strings, prints them to the serial port
 and sends an "acknowledge" string back to the sender

 A Processing sketch is included at the end of file that can be used to send
 and received messages for testing with a computer.

 created 21 Aug 2010
 by Michael Margolis

 This code is in the public domain.
 Pins 4, 10,11,12,13 cannot be used !!!
 */


#include <Ethernet.h>
#include <EthernetUdp.h>

#define DEBUG 1
#define BUFFERSIZE 8
#define INITVAL -1
#define NINPUTS 14
#define NANALOGINPUTS 2

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0x00, 0x00, 0x00, 0x00, 0x12, 0xFE
};
IPAddress ip(192, 168, 1, 152);

unsigned int localPort = 1030;      // local port to listen on

// buffers for receiving and sending data
char recvBuffer[BUFFERSIZE];  // buffer to hold receive packet
char sendBuffer[BUFFERSIZE];  // buffer to hold send packet

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

/* storage for inputs */
int isinput[NINPUTS];
int input[NINPUTS];
int analoginput[NANALOGINPUTS];

void setup() {
 
  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(10);  // Most Arduino shields

  // start the Ethernet
  Ethernet.begin(mac, ip);

  // Open serial communications and wait for port to open:
  if (DEBUG) {
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }

    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
  }
  
  // start UDP
  Udp.begin(localPort);
  if (DEBUG) Serial.println("UDP initialized.");

  /* assume no pins are selected as inputs, will be
   *  changed during initialization
   */
   for (int i=0;i<NINPUTS;i++) {
     isinput[i] = 0;
   }
  
   /* Put all analog inputs to ground if not used in order to avoid
      just garbage transfers */
   for (int i=0;i<NANALOGINPUTS;i++) {
     analoginput[i] = INITVAL;
   }
  
}

void loop() {

  int ivalue;

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    if (DEBUG) {
      Serial.print("Received packet of size ");
      Serial.println(packetSize);
      Serial.print("From ");
      IPAddress remote = Udp.remoteIP();
      for (int i=0; i < 4; i++) {
        Serial.print(remote[i], DEC);
        if (i < 3) {
          Serial.print(".");
        }
      }
      Serial.print(", port ");
      Serial.println(Udp.remotePort());
    }

    // read the packet into packetBufffer
    Udp.read(recvBuffer, BUFFERSIZE);

    /* Copy payload 16 bit Integer into variable */
    memcpy(&ivalue,&recvBuffer[6],2);

    if (DEBUG) {
      Serial.print("Content: ");
      for (int i=0; i < 2; i++) {
       Serial.print(recvBuffer[i]);
       Serial.print(" ");
      }
      for (int i=4; i < 6; i++) {
       Serial.print(recvBuffer[i],DEC);
       Serial.print(" ");
      }
      Serial.println(ivalue);
    }

    if ((recvBuffer[0]==0x41) && (recvBuffer[1]==0x52)) {
      if (recvBuffer[5] == 0) {
        /* Digital Output */
        pinMode(recvBuffer[4], OUTPUT );
        digitalWrite(recvBuffer[4],ivalue);
      } else if (recvBuffer[5] == 1) {
        /* Analog Output */
        pinMode(recvBuffer[4], OUTPUT );
        analogWrite(recvBuffer[4],ivalue);
      } else if (recvBuffer[5] == 2) { 
        /* define pin mode and initialize input if a pin is requested as input */ 
        isinput[recvBuffer[4]]=1;
        pinMode(recvBuffer[4], INPUT_PULLUP);
        input[recvBuffer[4]] = INITVAL;
      } else if (recvBuffer[5] == 3) { 
        /* Compass output of degrees */ 
        int y = 255.0 * cos(((float) ivalue)/10.0*3.14/180.0);
        int x = 255.0 * sin(((float) ivalue)/10.0*3.14/180.0);
        pinMode(3, OUTPUT );
        pinMode(5, OUTPUT );
        pinMode(6, OUTPUT );
        pinMode(7, OUTPUT );
        if (x > 0) {
          analogWrite(5,abs(x));
          digitalWrite(3,LOW);
        } else {
          analogWrite(5,255-abs(x));
          digitalWrite(3,HIGH);
        }

        if (y < 0) {
          analogWrite(6,abs(y));
          digitalWrite(7,LOW);
        } else {
          analogWrite(6,255-abs(y));
          digitalWrite(7,HIGH);
        }      
      }
      
    }
  } /* Packet was received */

  /* We can only send something if we have previously received something */
  if (Udp.remotePort() != 0) {

    // Read Analog inputs and send changed ones to host
    for (int i=0;i<NANALOGINPUTS;i++) {
      ivalue = analogRead(NINPUTS+i);
 
      if (ivalue != analoginput[i]) {
        analoginput[i] = ivalue;

//        if (DEBUG) {
        if (0) {
          Serial.print("Analog Input ");
          Serial.print(i);
          Serial.print(" changed to: ");
          Serial.println(ivalue);
        }
   
        sendBuffer[0] = 0x41;
        sendBuffer[1] = 0x52;
        sendBuffer[2] = mac[4];
        sendBuffer[3] = mac[5];
        sendBuffer[4] = i;
        sendBuffer[5] = 1;
        memcpy(&sendBuffer[6],&ivalue,sizeof(ivalue));
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        int ret = Udp.write(sendBuffer,BUFFERSIZE);
        Udp.endPacket();
      }
    }
    
    // Read selected digital inputs and send changed ones to host
    for (int i=0;i<NINPUTS;i++) {
      if (isinput[i]) {
        ivalue = digitalRead(i);
 
        if (ivalue != input[i]) {
          input[i] = ivalue;

          if (DEBUG) {
            Serial.print("Digital Input ");
            Serial.print(i);
            Serial.print(" changed to: ");
            Serial.println(ivalue);
          }
   
          sendBuffer[0] = 0x41;
          sendBuffer[1] = 0x52;
          sendBuffer[2] = mac[4];
          sendBuffer[3] = mac[5];
          sendBuffer[4] = i;
          sendBuffer[5] = 0;
          memcpy(&sendBuffer[6],&ivalue,sizeof(ivalue));
          Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
          int ret = Udp.write(sendBuffer,BUFFERSIZE);
          Udp.endPacket();
        }
      }
    }

  }
  delay(10);
}

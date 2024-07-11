
void udp_receive(void) {

  int16_t ivalue16;
  int8_t ivalue8;

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

    
    if (packetSize != RECVMSGLEN) {
       Serial.print("Wrong Packet Size. Should be: ");
       Serial.println(RECVMSGLEN);
    }
    
    if (DEBUG) {
      Serial.print(" with Content: ");
      for (int i = 0; i < 2; i++) {
        Serial.print(recvBuffer[i]);
        Serial.print(" ");
      }
      for (int i = 2; i < 8; i++) {
        memcpy(&ivalue8, &recvBuffer[i], 1);
        Serial.print(ivalue8);
        Serial.print(" ");
      }
      // Copy payload 16 bit Integer into variable
      memcpy(&ivalue16, &recvBuffer[8], 2);
      Serial.println(ivalue16);
    }

    // Identifier for Teensy: Characters T and E
    if ((recvBuffer[0] == TEENSY_ID1) && (recvBuffer[1] == TEENSY_ID2)) {

    }   /* Correct receive buffer initiator string */
  }     /* Packet was received */

}
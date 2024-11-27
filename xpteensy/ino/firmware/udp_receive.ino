/* Receive UDP Packets and Parse its contents */
/* Store received data on Teensy Outputs and I2C / SPI daughter boards */
void udp_receive(void) {

  int16_t ivalue16;
  int8_t ivalue8;

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize > 0) {
    remoteIP = Udp.remoteIP();
    remotePort = Udp.remotePort();
    if (DEBUG > 1) {
      Serial.printf("UDP: Received Packet with Size %i from %u.%u.%u.%u Port %i \n",
        packetSize,remoteIP[0],remoteIP[1],remoteIP[2],remoteIP[3],remotePort);
    }

    // read the packet into packetBufffer
    Udp.read(recvBuffer, RECVMSGLEN);

    if (packetSize != RECVMSGLEN) {
      Serial.printf("UDP: Wrong Packet Size. Should be: %i \n",RECVMSGLEN);
    } else {
      /* Parse Packet Content */
      if (DEBUG > 3) {
        Serial.print(" with Content: ");
        for (int i = 0; i < 2; i++) {
          Serial.print(recvBuffer[i]);
          Serial.print(" ");
        }
        for (int i = 2; i < RECVMSGLEN; i++) {
          memcpy(&ivalue8, &recvBuffer[i], 1);
          Serial.print(ivalue8);
          Serial.print(" ");
        }
        Serial.println("");
      }

      // Identifier for Teensy: Characters T and E
      if ((recvBuffer[0] == TEENSY_ID1) && (recvBuffer[1] == TEENSY_ID2)) {

        if ((recvBuffer[5] == TEENSY_TYPE) && (recvBuffer[6] == 0)) {
          if (recvBuffer[4] == TEENSY_PING) {
            /* send reply to ping request */
            ivalue16 = 0;
            udp_send(TEENSY_TYPE, 0, INITVAL, ivalue16);
          } else if (recvBuffer[4] == TEENSY_INIT) {
            /* initialize pin or daughter board as requested */
            memcpy(&ivalue16, &recvBuffer[8], 2);
            teensy_init(recvBuffer[7], recvBuffer[10], ivalue16);
          } else if (recvBuffer[4] == TEENSY_REGULAR) {
            /* set output to requested value */
            memcpy(&ivalue16, &recvBuffer[8], 2);
            teensy_write(recvBuffer[7], ivalue16);
          }
        } else if (recvBuffer[5] == MCP23017_TYPE) {
          /* It is a MCP23017 daughter board */
          /* Init or Regular Data Packet */
          if (recvBuffer[4] == TEENSY_INIT) {
            memcpy(&ivalue16, &recvBuffer[8], 2);
            mcp23017_init(recvBuffer[6], recvBuffer[7], recvBuffer[10], recvBuffer[2], recvBuffer[3], recvBuffer[11], ivalue16);
          } else if (recvBuffer[4] == TEENSY_REGULAR) {
            memcpy(&ivalue16, &recvBuffer[8], 2);
            mcp23017_write(recvBuffer[6], recvBuffer[7], ivalue16);
          }
        } else if (recvBuffer[5] == PCA9685_TYPE) {
          /* It is a PCA9685 daughter board */
          /* Init or Regular Data Packet */
          if (recvBuffer[4] == TEENSY_INIT) {
            memcpy(&ivalue16, &recvBuffer[8], 2);
            pca9685_init(recvBuffer[6], recvBuffer[7], recvBuffer[10], recvBuffer[2], recvBuffer[3], recvBuffer[11], ivalue16);
          } else if (recvBuffer[4] == TEENSY_REGULAR) {
            memcpy(&ivalue16, &recvBuffer[8], 2);
            pca9685_write(recvBuffer[6], recvBuffer[7], ivalue16);
          }
        }
      } /* Correct receive buffer initiator string */
    }   /* Correct Packet Size */
  }     /* Packet was received */
}
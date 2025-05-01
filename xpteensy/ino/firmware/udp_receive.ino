/* Receive UDP Packets and Parse its contents */
/* Store received data on Teensy Outputs and I2C / SPI daughter boards */
void udp_receive(void) {

  int16_t ivalue16;
  int16_t ivalue16b;
  int8_t ivalue8;
  int8_t prog;

  int16_t retval;

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize > 0) {
    remoteIP = Udp.remoteIP();
    remotePort = Udp.remotePort();
    if (DEBUG > 1) {
      Serial.printf("UDP: Received Packet with Size %i from %u.%u.%u.%u Port %i \n",
                    packetSize, remoteIP[0], remoteIP[1], remoteIP[2], remoteIP[3], remotePort);
    }

    // read the packet into packetBufffer
    Udp.read(recvBuffer, RECVMSGLEN);

    if (packetSize != RECVMSGLEN) {
      Serial.printf("UDP: Wrong Packet Size. Should be: %i \n", RECVMSGLEN);
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
            if (DEBUG > 0) {
              Serial.printf("RECV: PING Request\n");
            }
            ivalue16 = 0;
            udp_send(TEENSY_TYPE, 0, TEENSY_PING, INITVAL, ivalue16);
          } else if (recvBuffer[4] == TEENSY_INIT) {
            /* initialize pin or daughter board as requested */
            memcpy(&ivalue16, &recvBuffer[8], 2);
            memcpy(&ivalue16b, &recvBuffer[2], 2);
            teensy_init(recvBuffer[7], recvBuffer[10], ivalue16, recvBuffer[11], recvBuffer[12], recvBuffer[13],recvBuffer[14], recvBuffer[15], ivalue16b);
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
            retval = mcp23017_init(recvBuffer[6], recvBuffer[7], recvBuffer[10], recvBuffer[11], recvBuffer[12], recvBuffer[13], ivalue16);
            if (retval != 0) udp_send(MCP23017_TYPE, recvBuffer[6], TEENSY_ERROR, recvBuffer[7], retval);
          } else if (recvBuffer[4] == TEENSY_REGULAR) {
            memcpy(&ivalue16, &recvBuffer[8], 2);
            retval = mcp23017_write(recvBuffer[6], recvBuffer[7], ivalue16);
            if (retval != 0) udp_send(MCP23017_TYPE, recvBuffer[6], TEENSY_ERROR, recvBuffer[7], retval);
          }
        } else if (recvBuffer[5] == PCA9685_TYPE) {
          /* It is a PCA9685 daughter board */
          /* Init or Regular Data Packet */
          if (recvBuffer[4] == TEENSY_INIT) {
            memcpy(&ivalue16, &recvBuffer[8], 2);
            retval = pca9685_init(recvBuffer[6], recvBuffer[7], recvBuffer[10], recvBuffer[12], recvBuffer[13], ivalue16);
            if (retval != 0) udp_send(PCA9685_TYPE, recvBuffer[6], TEENSY_ERROR, recvBuffer[7], retval);
            Serial.printf("RETVAL: %i\n",retval);

           } else if (recvBuffer[4] == TEENSY_REGULAR) {
            memcpy(&ivalue16, &recvBuffer[8], 2);
            retval = pca9685_write(recvBuffer[6], recvBuffer[7], ivalue16);
            if (retval != 0) udp_send(PCA9685_TYPE, recvBuffer[6], TEENSY_ERROR, recvBuffer[7], retval);
         }
        } else if (recvBuffer[5] == HT16K33_TYPE) {
          /* It is a HT16K33 daughter board */
          /* Init or Regular Data Packet */
          if (recvBuffer[4] == TEENSY_INIT) {
            retval = ht16k33_init(recvBuffer[6], recvBuffer[12], recvBuffer[13]);
            if (retval != 0) udp_send(HT16K33_TYPE, recvBuffer[6], TEENSY_ERROR, recvBuffer[7], retval);
          } else if (recvBuffer[4] == TEENSY_REGULAR) {
            memcpy(&ivalue16, &recvBuffer[8], 2);
            retval = ht16k33_write(recvBuffer[6], recvBuffer[7], ivalue16, recvBuffer[10], recvBuffer[11]);
            if (retval != 0) udp_send(HT16K33_TYPE, recvBuffer[6], TEENSY_ERROR, recvBuffer[7], retval);
          }
        } else if (recvBuffer[5] == AS5048B_TYPE) {
          /* It is a AS5048B daughter board */
          /* Init or Regular Data Packet */
          if (recvBuffer[4] == TEENSY_INIT) {
            memcpy(&ivalue16, &recvBuffer[8], 2);
            as5048b_init(recvBuffer[6], recvBuffer[10], recvBuffer[11], recvBuffer[12], recvBuffer[13], ivalue16);
          }
        } else if (recvBuffer[5] == PROGRAM_TYPE) {
          /* It is data for a program */
          prog = recvBuffer[7];  // number of program
          if (recvBuffer[4] == TEENSY_INIT) {
            program_data[prog].connected = 1;
            program_data[prog].type = recvBuffer[6];

            program_data[prog].val8[3] = false;
            program_data[prog].val8[4] = false;
          }
          memcpy(&program_data[prog].val16[0], &recvBuffer[8], 2);
          memcpy(&program_data[prog].val16[1], &recvBuffer[10], 2);
          memcpy(&program_data[prog].val16[2], &recvBuffer[12], 2);
          program_data[prog].val8[0] = recvBuffer[2];
          program_data[prog].val8[1] = recvBuffer[3];
          program_data[prog].val8[2] = recvBuffer[14];
         }

      } /* Correct receive buffer initiator string */
    }   /* Correct Packet Size */
  }     /* Packet was received */
}
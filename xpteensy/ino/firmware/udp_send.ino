
int udp_send(int8_t dev_type, int8_t dev_num, int8_t pin_num, int16_t val) {

  int ret = 0;

  /* We can only send something if we have previously received something */
  if (Udp.remotePort() != 0) {
    memset(sendBuffer, 0, SENDMSGLEN);
    sendBuffer[0] = TEENSY_ID1; /* T */
    sendBuffer[1] = TEENSY_ID2; /* E */
    sendBuffer[2] = teensy_data.mac[0];
    sendBuffer[3] = teensy_data.mac[1];
    if (pin_num == INITVAL) {
      sendBuffer[4] = TEENSY_PING;
      sendBuffer[5] = dev_type;
      sendBuffer[6] = dev_num;
    } else {
      sendBuffer[4] = TEENSY_REGULAR;
      sendBuffer[5] = dev_type;
      sendBuffer[6] = dev_num;
      sendBuffer[7] = pin_num;
      memcpy(&sendBuffer[8], &val, sizeof(val));
    }
    //ret = Udp.send(remoteIP, remotePort,sendBuffer, SENDMSGLEN);
    Udp.beginPacket(remoteIP, remotePort);
    ret = Udp.write(sendBuffer, SENDMSGLEN);
    if (DEBUG>0) {
      Serial.printf("UDP_SEND: Sent UDP Packet with length %i\n",ret);
    }
    Udp.endPacket();
  }

  return ret;
}
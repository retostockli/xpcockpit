
void udp_send(void) {

  
  /* We can only send something if we have previously received something */
  if (Udp.remotePort() != 0) {
/*
          Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
          int ret = Udp.write(sendBuffer, SENDMSGLEN);
          if (DEBUG) {
            Serial.print("Sent UDP Packet with length: ");
            Serial.println(ret,DEC);
          }
          Udp.endPacket();
        */
    
  
  }
}
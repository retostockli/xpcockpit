void init_data() {

  int k, l;
  int dev;

  teensy_data.num_servo = 0;

  for (k = 0; k < MAX_PINS; k++) {
    for (l = 0; l < MAX_HIST; l++) {
      teensy_data.val[k][l] = INITVAL;
    }
    teensy_data.val_save[k] = INITVAL;
    teensy_data.pinmode[k] = INITVAL;
    teensy_data.val_time[k].tv_sec = current_time.tv_sec;
    teensy_data.val_time[k].tv_usec = current_time.tv_usec;
    teensy_data.arg1[k] = INITVAL;
    teensy_data.arg2[k] = INITVAL;
    teensy_data.arg3[k] = INITVAL;
    teensy_data.arg4[k] = 0;
    teensy_data.arg5[k] = 0;
    teensy_data.arg6[k] = INITVAL;
  }
  teensy_data.connected = 1;


  Serial.printf("Teensy Data initialized\n");


  for (dev = 0; dev < MAX_DEV; dev++) {
    for (k = 0; k < MCP23017_MAX_PINS; k++) {
      mcp23017_data[dev].val[k] = INITVAL;
      mcp23017_data[dev].val_save[k] = INITVAL;
      mcp23017_data[dev].pinmode[k] = INITVAL;
    }
    mcp23017_data[dev].intpin = INITVAL;
    mcp23017_data[dev].wire = INITVAL;
    mcp23017_data[dev].address = 0x00;
    mcp23017_data[dev].connected = 0;
  }


  Serial.printf("MCP23017 Data initialized\n");


  for (dev = 0; dev < MAX_DEV; dev++) {
    for (k = 0; k < PCA9685_MAX_PINS; k++) {
      pca9685_data[dev].val[k] = INITVAL;
      pca9685_data[dev].val_save[k] = INITVAL;
      pca9685_data[dev].pinmode[k] = INITVAL;
    }
    pca9685_data[dev].wire = INITVAL;
    pca9685_data[dev].address = 0x00;
    pca9685_data[dev].connected = 0;
  }

  Serial.printf("PCA9685 Data initialized\n");


  for (dev = 0; dev < MAX_DEV; dev++) {
    for (k = 0; k < HT16K33_MAX_DIG; k++) {
      ht16k33_data[dev].val[k] = INITVAL;
      ht16k33_data[dev].val_save[k] = INITVAL;
      ht16k33_data[dev].decimalpoint[k] = INITVAL;
    }
    ht16k33_data[dev].brightness = INITVAL;
    ht16k33_data[dev].wire = INITVAL;
    ht16k33_data[dev].address = 0x00;
    ht16k33_data[dev].connected = 0;
  }

  Serial.printf("HT16K33 Data initialized\n");


  for (dev = 0; dev < MAX_DEV; dev++) {
    for (l = 0; l < MAX_HIST; l++) {
      as5048b_data[dev].val[l] = INITVAL;
    }
    as5048b_data[dev].val_save = INITVAL;
    as5048b_data[dev].type = INITVAL;
    as5048b_data[dev].nangle = INITVAL;

    as5048b_data[dev].wire = INITVAL;
    as5048b_data[dev].address = 0x00;
    as5048b_data[dev].connected = 0;
  }

  Serial.printf("AS5048B Data initialized\n");


  for (dev = 0; dev < MAX_DEV; dev++) {
    for (k = 0; k < PGA2311_MAX_CHANNELS; k++) {
      pga2311_data[dev].val[k] = INITVAL;
    }
    pga2311_data[dev].spi = INITVAL;
    pga2311_data[dev].cs = INITVAL;
    pga2311_data[dev].connected = 0;
  }

  Serial.printf("PGA2311 Data initialized\n");

}

int init_ethernet(void) {

  uint8_t add[4];

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
                    "    MAC          = %02x-%02x-%02x-%02x-%02x-%02x\n",
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
    Serial.printf("Ethernet: Started.\n");
  } else {
    Serial.printf("Ethernet: Failed to start\n");
    return -1;
  }

  return 0;
}
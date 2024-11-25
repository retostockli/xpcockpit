void init_data() {

  int k,l;
  int dev;

  teensy_data.num_servo = 0;

  for (k=0;k<MAX_PINS;k++) {
	  for (l=0;l<MAX_HIST;l++) {
	    teensy_data.val[k][l] = INITVAL;
    }
	  teensy_data.val_save[k] = INITVAL;
	  teensy_data.pinmode[k] = INITVAL;
	  teensy_data.arg1[k] = INITVAL;
	  teensy_data.arg2[k] = INITVAL;
  }
	teensy_data.connected = 1;

  // SET for TEENSY 4.1 for now
	teensy_data.num_pins = 42;
	
  if (DEBUG) {
	  Serial.printf("Teensy Data initialized\n");
  }

  for (dev=0;dev<MAX_DEV;dev++) {
    for (k=0;k<MAX_MCP23017_PINS;k++) {
      mcp23017_data[dev].val[k] = INITVAL;
      mcp23017_data[dev].val_save[k] = INITVAL;
      mcp23017_data[dev].pinmode[k] = INITVAL;
    }
    mcp23017_data[dev].intpin = INITVAL;
    mcp23017_data[dev].wire = INITVAL;
    mcp23017_data[dev].address = 0x00;
    mcp23017_data[dev].connected = 0;
  } 

  if (DEBUG) {
	  Serial.printf("MCP23017 Data initialized\n");
  }	
}
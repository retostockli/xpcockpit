void init() {

  int k,l;

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
	  Serial.println("Teensy Data initialized");
  }
	
}
void teensy_init(int8_t pin, int8_t pinmode, int16_t initval) {
  if ((pin >=0) && (pin < teensy_data.num_pins)) {
    teensy_data.pinmode[pin] = pinmode;
    if (pinmode == PINMODE_INPUT) {
      pinMode(pin, INPUT_PULLUP);
    } else if (pinmode == PINMODE_OUTPUT) {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, initval);
    } else if (pinmode == PINMODE_PWM) {
      pinMode(pin, OUTPUT); 
      // See: https://www.pjrc.com/teensy/td_pulse.html
      // You can't have the chicken and egg at the same time ...
      //analogWriteFrequency(pin, 375000); // Teensy 3.0 pin 3 also changes to 375 kHz
      //analogWriteResolution(12);  // analogWrite value 0 to 4095, or 4096 for high
      analogWrite(pin, initval);
    } else if (pinmode == PINMODE_ANALOGINPUT) {
      pinMode(pin, INPUT);
    } else if (pinmode == PINMODE_INTERRUPT) {
      pinMode(pin, INPUT_PULLUP);
    } else if (pinmode == PINMODE_I2C) {

    } else {
       if (DEBUG) {
        Serial.print("Init: Wrong Teensy PinMode for pin ");
        Serial.println(pin);
       }
    }
  } else {
    if (DEBUG) {
      Serial.print("Init: Teensy Pin Number out of range: ");
      Serial.println(pin);
    }    
  }

}

void teensy_recv(int8_t pin, int16_t val) {
  if ((pin >=0) && (pin < teensy_data.num_pins)) {
    if (teensy_data.pinmode[pin] == PINMODE_OUTPUT) {
      if (DEBUG) {
        Serial.print("RECV: Teensy Digital Output pin ");
        Serial.print(pin);
        Serial.print(" has value: ");
        Serial.println(val);
      }
      teensy_data.val[pin][0] = val;
      digitalWrite(pin, val);
    } else if (teensy_data.pinmode[pin] == PINMODE_PWM) {
     if (DEBUG) {
        Serial.print("RECV: Teensy Analog Output pin ");
        Serial.print(pin);
        Serial.print(" has value: ");
        Serial.println(val);
      }
      teensy_data.val[pin][0] = val;
      analogWrite(pin, val);
    } else {
      if (DEBUG) {
        Serial.print("RECV: Teensy Pin Number not set for Output or PWM: ");
        Serial.println(pin);
      }
    }
  } else {
    if (DEBUG) {
      Serial.print("RECV: Teensy Pin Number out of range: ");
      Serial.println(pin);
    }    
  }
 
}
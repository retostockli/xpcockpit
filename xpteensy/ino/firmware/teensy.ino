// Create an instance of the Servo library
Servo servo[MAX_SERVO];

void teensy_init(int8_t pin, int8_t pinmode, int16_t initval) {
  if ((pin >= 0) && (pin < teensy_data.num_pins)) {

    if (pinmode == PINMODE_INPUT) {
      pinMode(pin, INPUT_PULLUP);
      teensy_data.val[pin][0] = INITVAL;
      teensy_data.val_save[pin] = INITVAL;
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
      teensy_data.val[pin][0] = INITVAL;
      teensy_data.val_save[pin] = INITVAL;
    } else if (pinmode == PINMODE_INTERRUPT) {
      pinMode(pin, INPUT_PULLUP);
    } else if (pinmode == PINMODE_I2C) {

    } else if (pinmode == PINMODE_SERVO) {
      if (teensy_data.pinmode[pin] != pinmode) {
        /* Specify the pin number where the motor's control signal is connected. */
        //servo[teensy_data.num_servo].attach(pin);
        /* Specify the pin number where the motor's control signal is connected, and the motor's minimum and maximum control pulse width, in microseconds. */
        servo[teensy_data.num_servo].attach(pin, SERVO_MINPULSE, SERVO_MAXPULSE);
        teensy_data.arg1[pin] = teensy_data.num_servo;
        /* store servo instance number for this pint */
        teensy_data.num_servo++;
      }


    } else {
      if (DEBUG) {
        Serial.print("Init: Wrong Teensy PinMode for pin ");
        Serial.println(pin);
      }
    }
    /* store pinmode */
    teensy_data.pinmode[pin] = pinmode;
  } else {
    if (DEBUG) {
      Serial.print("Init: Teensy Pin Number out of range: ");
      Serial.println(pin);
    }
  }
}

void teensy_write(int8_t pin, int16_t val) {
  if ((pin >= 0) && (pin < teensy_data.num_pins)) {
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
    } else if (teensy_data.pinmode[pin] == PINMODE_SERVO) {
      if ((teensy_data.arg1[pin] < teensy_data.num_servo) && (teensy_data.arg1[pin] >= 0)) {
        if (DEBUG) {
          Serial.print("RECV: Teensy Servo Output pin ");
          Serial.print(pin);
          Serial.print(" has value: ");
          Serial.println(val);
        }
        servo[teensy_data.arg1[pin]].write(val);
      }
    } else {
      if (DEBUG) {
        Serial.print("RECV: Teensy Pin Number not set for Output, Servo or PWM: ");
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

void teensy_read() {
  int ret;
  int16_t val;
  int h;
  for (int8_t pin = 0; pin < teensy_data.num_pins; pin++) {
    if (teensy_data.pinmode[pin] == PINMODE_INPUT) {
      if (digitalRead(pin)) {
        teensy_data.val[pin][0] = 0; /* If a pin is "HIGH", the switch is open */
      } else {
        teensy_data.val[pin][0] = 1; /* If a pin is "LOW", the switch is connected to ground, thus closed */
      }
      if (teensy_data.val[pin][0] != teensy_data.val_save[pin]) {
        if (DEBUG) {
          Serial.print("POLL: Teensy Pin Digital Value ");
          Serial.print(pin);
          Serial.print(" changed to: ");
          Serial.println(teensy_data.val[pin][0]);
        }
        ret = udp_send(TEENSY_TYPE, 0, pin, teensy_data.val[pin][0]);
        if (ret == SENDMSGLEN) {
          teensy_data.val_save[pin] = teensy_data.val[pin][0];
        }
      }
    } else if (teensy_data.pinmode[pin] == PINMODE_INTERRUPT) {
      if (!digitalRead(pin)) {
        if (teensy_data.arg1[pin] == MCP23017_TYPE) {
          mcp23017_read(teensy_data.arg2[pin]);
        } else if (teensy_data.arg1[pin] == INITVAL) {
          /* Interrupt signaling for not yet initialized board */
        } else {
          if (DEBUG) {
            Serial.print("POLL: Teensy Interrupt on Pin # ");
            Serial.println(pin);
            Serial.println(" for unknown device. Currently only implemented for MCP23017 ");
          }
        }
        if (DEBUG) {
          Serial.print("POLL: Teensy Interrupt on Pin # ");
          Serial.println(pin);
        }
      }
    } else if (teensy_data.pinmode[pin] == PINMODE_ANALOGINPUT) {
      val = analogRead(pin);

      /* Filter input noise of +/- X values. Only send if there is a significant change */
      int16_t valmax = -1;
      int16_t valmin = 10000;
      int16_t noise = 1;
      for (h = 0; h < MAX_HIST; h++) {
        valmax = max(teensy_data.val[pin][h], valmax);
        valmin = min(teensy_data.val[pin][h], valmin);
      }

      //Serial.println(val);
      if ((val > (valmax + noise)) || (val < (valmin - noise)) || ((valmax - valmin) > 3 * noise)) {
        if (DEBUG) {
          Serial.print("POLL: Teensy Pin Analog Value ");
          Serial.print(pin);
          Serial.print(" changed to: ");
          Serial.println(val);
        }
        ret = udp_send(TEENSY_TYPE, 0, pin, val);

        /* Shift History of analog inputs and update current value */
        for (h = 0; h < MAX_HIST - 1; h++) {
          teensy_data.val[pin][h + 1] = teensy_data.val[pin][h];
        }
        teensy_data.val[pin][0] = val;
      }
    }
  }
}
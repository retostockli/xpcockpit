int program_closedloop(int prog) {

  int ret = 0;

  int16_t precision = pow(2, ANALOGINPUT_NBITS) / 100;  // servo precision: set to 1%

  if ((prog >= 0) && (prog < MAX_PROG)) {

    int8_t active = program_data[prog].val8[0];
    int8_t pot_pin = program_data[prog].val8[1];
    int8_t mot_pin = program_data[prog].val8[2];
  

    int16_t servo_request = program_data[prog].val16[0];
    //int16_t servo_min = program_data[prog].val16[1];
    //int16_t servo_max = program_data[prog].val16[2];

    if ((teensy_data.pinmode[pot_pin] != PINMODE_ANALOGINPUTMEDIAN) && (teensy_data.pinmode[pot_pin] != PINMODE_ANALOGINPUTMEAN)) {
      if (DEBUG > 0) Serial.printf("Program %i Closed Loop Motor: Pin %i not defined as Anlog Input\n", prog, pot_pin);
    }

    if (teensy_data.pinmode[mot_pin] != PINMODE_MOTOR) {
      if (DEBUG > 0) Serial.printf("Program %i Closed Loop Motor: Pin %i not defined as Motor Output\n", prog, mot_pin);
    }


    /* closed loop active? */
    if ((active == 1) && (servo_request != INITVAL)) {

      int16_t servo_actual = analogRead(pot_pin);

      bool forward = false;
      bool backward = false;

      /* MIN and MAX speeds may need to be made dynamic */
      uint8_t min_speed = 140;   /* minimum speed at which motors do still run with load */
      //uint8_t max_speed = 255;   /* maximum speed for this motor */
      uint8_t max_speed = (uint8_t) program_data[prog].val8[3];

      bool forward_save = program_data[prog].val8[5];
      bool backward_save = program_data[prog].val8[6];

      /* Hysteresis: if we are stopped, we need more inertia for starting again */
      /* This resolves stopping at a precision boundary and trying to move because of potentiometer noise */
      int16_t hysteresis;
      if ((forward_save == false) && (backward_save == false)) {
        hysteresis = precision * 2;
      } else {
        hysteresis = precision;
      }
      if (servo_request > (servo_actual + hysteresis)) forward = true;
      if (servo_request < (servo_actual - hysteresis)) backward = true;

      /* decrease speed if we get to requested servo position for not overshooting */
      uint8_t speed = min(max(fabsf(servo_request - servo_actual) / pow(2, ANALOGINPUT_NBITS) * 15.0,0.0),1.0) *
                      float(max_speed - min_speed) + min_speed;

      /* save last state of motor */
      program_data[prog].val8[5] = forward;
      program_data[prog].val8[6] = backward;

      if (program_data[prog].val16_save[0] != servo_request) {
        if (DEBUG > 0) {
          Serial.printf("Program %i New Closed Loop Motor Servo Value: %i MAX Speed: \n", prog, servo_request, max_speed);
        }
        program_data[prog].val16_save[0] = servo_request;
      }

      if (forward) {
        /* move motor forward */
        if (DEBUG > 0) {
          Serial.printf("Program %i New Closed Loop Motor Servo run FORWARD with Speed %i \n", prog, speed);
          Serial.printf("Servo REQ: %i Servo ACT: %i \n", servo_request, servo_actual);
        }
        analogWrite(mot_pin, speed);
        digitalWrite(teensy_data.arg1[mot_pin], 1);
        digitalWrite(teensy_data.arg2[mot_pin], 0);
      } else if (backward) {
        /* move motor backward */
        if (DEBUG > 0) {
          Serial.printf("Program %i New Closed Loop Motor Servo run BACKWARD with Speed %i \n", prog, speed);
          Serial.printf("Servo REQ: %i Servo ACT: %i \n", servo_request, servo_actual);
        }
        analogWrite(mot_pin, speed);
        digitalWrite(teensy_data.arg1[mot_pin], 0);
        digitalWrite(teensy_data.arg2[mot_pin], 1);
      } else {
        /* hard stop */
        //digitalWrite(mot_pin, 1);
        analogWrite(mot_pin, 255);
        digitalWrite(teensy_data.arg1[mot_pin], 0);
        digitalWrite(teensy_data.arg2[mot_pin], 0);
      }

    } else {
      /* disable closed loop and have the motor in free running mode (EN=0, IN1=0, IN2=0) */
      //digitalWrite(mot_pin, 0);
      analogWrite(mot_pin, 0);
      digitalWrite(teensy_data.arg1[mot_pin], 0);
      digitalWrite(teensy_data.arg2[mot_pin], 1);
    }
  } else {
    ret = -1;
  }

  return ret;
}
#include "ams_as5048b.h"

#define AS5048B_POLL_TIME 20 /* number of milliseconds to poll the board */

//AMS_AS5048B as5048b(0x40);
AMS_AS5048B as5048b[MAX_DEV];

unsigned long as5048b_time;

void as5048b_init(int8_t dev, int8_t nangle, int8_t type, int8_t wirenum, uint8_t address, int16_t val) {

  if ((dev >= 0) && (dev < MAX_DEV)) {
    // Connect board to I2C bus if not already connected
    if (as5048b_data[dev].connected == 0) {

      // TODO: IMPLEMENT FOR WIRE 1 and 2 if needed
      if (wirenum == 0) {
        as5048b[dev] = AMS_AS5048B(address);
        as5048b_data[dev].connected = 1;
        as5048b_data[dev].wire = wirenum;
        as5048b_data[dev].address = address;
      } else if (wirenum == 1) {
        Serial.printf("INIT: as5048b Device %i Wire 1 NOT SUPPORTED YET \n", dev);
      } else if (wirenum == 2) {
        Serial.printf("INIT: as5048b Device %i Wire 2NOT SUPPORTED YET \n", dev);
      } else {
        if (DEBUG > 0) {
          Serial.printf("INIT: as5048b Dev %i Wire Number %i Out of Range. Wire Number can only be 0, 1 or 2 \n", dev, wirenum);
        }
      }
    }

    /* Only initialize connected devices */
    if (as5048b_data[dev].connected == 1) {

      as5048b_data[dev].val_save = INITVAL;  // reset pin state so that it will be sent to client after read
      as5048b_data[dev].type = type;
      as5048b_data[dev].nangle = nangle;

      as5048b[dev].begin();
      //consider the current position as zero
      as5048b[dev].setZeroReg();

      if (DEBUG > 0) {
        if (type == 0) {
          Serial.printf("INIT: as5048b Device %i initialized as Direction Count Reporting \n", dev);
        } else {
          Serial.printf("INIT: as5048b Device %i initialized as Angle Reporting \n", dev);
        }
      }

    } else {
      if (DEBUG > 0) {
        Serial.printf("INIT: as5048b Device Number %i out of range \n", dev);
      }
    }
  }

  /* reset last time */
  as5048b_time = micros();
}

void as5048b_read() {

  int ret = 0;
  int h;
  unsigned long time = micros();

  if (time > (as5048b_time + AS5048B_POLL_TIME * 1000)) {

    as5048b_time = time;

    for (int8_t dev = 0; dev < MAX_DEV; dev++) {
      if (as5048b_data[dev].connected == 1) {

        bool up = false;
        bool dn = false;
        int16_t angle = as5048b[dev].angleR(3, true);         // read angle
        angle = angle % 360;                                  // set 360 degrees to 0 degrees
        int16_t angle_previous = as5048b_data[dev].val_save;  // previous median value

        /* Shift History of angle inputs and update current value */
        for (h = MAX_HIST - 2; h >= 0; h--) {
          as5048b_data[dev].val[h + 1] = as5048b_data[dev].val[h];
        }
        as5048b_data[dev].val[0] = angle;

        /* Median Filter input noise of angle measurement */
        /* Can have a time lag since the median filter needs to be filled with new values first */
        int16_t temparr[MAX_HIST];
        int16_t noise = 1;

        memcpy(temparr, as5048b_data[dev].val, sizeof(as5048b_data[dev].val[0]) * MAX_HIST);

        /* check if we just crossed the 360 degree border and adjust angle for comparison */
        /* we only compare positive angles in order to get each nangle step */
        for (h = 0; h < MAX_HIST; h++) {
          if ((temparr[h] != INITVAL) && (angle_previous != INITVAL)) {
            if (angle_previous > (temparr[h] + 300)) temparr[h] += 360;
            if (angle_previous < (temparr[h] - 300)) temparr[h] -= 360;
          }
        }

        quicksort(temparr, 0, MAX_HIST - 1);
        int16_t median = temparr[MAX_HIST / 2];

        if ((median != INITVAL) && (angle_previous != INITVAL)) {
          /* only send current value if it is outside median and noise */
          if ((median < (angle_previous - noise)) || (median > (angle_previous + noise))) {
            if ((median * as5048b_data[dev].nangle / 360) > (angle_previous * as5048b_data[dev].nangle / 360)) up = true;
            if ((median * as5048b_data[dev].nangle / 360) < (angle_previous * as5048b_data[dev].nangle / 360)) dn = true;

            if (up || dn) {

              //Serial.printf("%i %i %i\n", angle, median, angle_previous);

              if (as5048b_data[dev].type == 1) {
                /* send current angle value */
                ret = udp_send(AS5048B_TYPE, dev, TEENSY_REGULAR, 0, angle);
              } else {
                /* send up / down value */
                if (up) {
                  ret = udp_send(AS5048B_TYPE, dev, TEENSY_REGULAR, 0, 1);
                } else {
                  ret = udp_send(AS5048B_TYPE, dev, TEENSY_REGULAR,  0, -1);
                }
              }
              if (DEBUG > 0) {
                Serial.printf("READ: as5048b Device Number %i Angle %i \n", dev, angle);
              }

              /* update stored angle upon successful send (not used right now) */
              if (ret == SENDMSGLEN) {
                if (median < 0) median += 360;
                if (median > 359) median -= 360;
                as5048b_data[dev].val_save = median % 360;
              }
            }  // angle changed more than nangle so we can send update
          }    // angle change outside noise
        }      // median and save value not INITVAL
        if ((as5048b_data[dev].val_save == INITVAL) && (median != INITVAL)) {
          if (median < 0) median += 360;
          if (median > 359) median -= 360;
          as5048b_data[dev].val_save = median;
        }

      }  // connected
    }    // loop through devices
  }      // time to poll devices
}
#include "ams_as5048b.h"

//AMS_AS5048B as5048b(0x40);
AMS_AS5048B as5048b[MAX_DEV];

void as5048b_init(int8_t dev, int8_t nangle, int8_t type, int8_t wirenum, uint8_t address, int16_t val) {

  if ((dev >= 0) && (dev < MAX_DEV)) {
    // Connect board to I2C bus if not already connected
    if (as5048b_data[dev].connected == 0) {

      // TODO: IMPLEMENT FOR WIRE 1 and 2
      if (wirenum == 0) {
       as5048b[dev] = AMS_AS5048B(address);
      } else if (wirenum == 1) {
       as5048b[dev] = AMS_AS5048B(address);
      } else if (wirenum == 2) {
       as5048b[dev] = AMS_AS5048B(address);
      } else {
        if (DEBUG > 0) {
          Serial.printf("INIT: as5048b Dev %i Wire Number %i Out of Range. Wire Number can only be 0, 1 or 2 \n", dev, wirenum);
        }
      }

      as5048b_data[dev].connected = 1;
      as5048b_data[dev].wire = wirenum;
      as5048b_data[dev].address = address;
    }

    /* Only initialize connected devices */
    if (as5048b_data[dev].connected == 1) {

      as5048b_data[dev].val_save = INITVAL;  // reset pin state so that it will be sent to client after read
      as5048b_data[dev].type = type;
      as5048b_data[dev].nangle = nangle;

      as5048b[dev].begin();
      //consider the current position as zero
      as5048b[dev].setZeroReg();

      //as5048b.begin();
      //consider the current position as zero
      //as5048b.setZeroReg();

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
}

void as5048b_read() {

  for (int8_t dev = 0; dev < MAX_DEV; dev++) {
    if (as5048b_data[dev].connected == 1) {

      float angle = as5048b[dev].angleR(3, true);

      if (DEBUG > 0) {
        Serial.printf("READ: as5048b Device Number %i Angle %f \n", dev,angle);
      }

    }
  }
}
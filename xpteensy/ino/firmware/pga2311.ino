#include <SPI.h>

/* NOTES FOR THE PGA2311
   Pin 8 MUTE --> HIGH for unmute
   Pin 1 ZCEN --> HIGH for enable zero crossing (advised)

   Have a 10uF and 0.1 uF to Digital and Analog Power inputs towards ground close to the Chip

*/

int16_t pga2311_init(int8_t dev, int8_t channel, int8_t spinum, int8_t cspin, int16_t val) {

  int16_t retval = 0;

  if ((channel >= 0) && (channel < PGA2311_MAX_CHANNELS)) {
    if ((dev >= 0) && (dev < MAX_DEV)) {
      // Initialize SPI bus if not already initialized

      pga2311_data[dev].spi = spinum;
      pga2311_data[dev].cs = cspin;

      if (!spi_initialized) { 
        if (spi_init(spinum) != 0) {
          retval = ERROR_SPI_INIT;
          if (DEBUG > 0) {
            Serial.printf("INIT: PGA2311 Dev %i SPI initialization failed \n", dev);
          }
        }
      }

      /* SPI Bus ok, continue with device initialization */
      if (retval == 0) {
       
        if (pga2311_data[dev].connected == 0) {
 
          if (teensy_data.pinmode[cspin] != INITVAL) {
            retval = ERROR_INIT;
            if (DEBUG > 0) {
              Serial.printf("INIT: PGA2311 Device %i could not be connected to SPI bus %i with CS pin %i \n", dev, spinum, cspin);
            }
          } else {
            teensy_data.pinmode[cspin] = PINMODE_SPI;
            pinMode(cspin, OUTPUT);
            pga2311_data[dev].connected = 1;
          }

        }   

      } /* SPI device could be initialized */

      /* Only continue on connected devices */
      if (pga2311_data[dev].connected == 1) {

        if (DEBUG > 0) {
          Serial.printf("INIT: PGA2311 Device %i initialized \n", dev);
        }
        pga2311_write(dev, channel, val);
      } 
    } else {
      retval = ERROR_DEV_RANGE;
      if (DEBUG > 0) {
        Serial.printf("INIT: PGA2311 Device %i out of range \n", dev);
      }
    }
  } else {
    retval = ERROR_PIN_RANGE;
    if (DEBUG > 0) {
      Serial.printf("INIT: PGA2311 Device %i Channel %i out of range \n", dev, channel);
    }
  }

  return retval;
}


int16_t pga2311_write(int8_t dev, int8_t channel, int16_t val) {

  int16_t retval = 0;
  
  Serial.printf("WRITE: PGA2311 Device %i Channel %i Volume %i \n", dev, channel, val);

  if ((channel >= 0) && (channel < PGA2311_MAX_CHANNELS)) {
    if ((dev >= 0) && (dev < MAX_DEV)) {
      if (pga2311_data[dev].connected == 1) {
        if (val < PGA2311_MINVAL) val = PGA2311_MINVAL;
        if (val > PGA2311_MAXVAL) val = PGA2311_MAXVAL;
        if (val != pga2311_data[dev].val[channel]) {

          pga2311_data[dev].val[channel] = val;

          uint8_t left = (uint8_t) pga2311_data[dev].val[0];
          uint8_t right = (uint8_t) pga2311_data[dev].val[1];

          //SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
          digitalWrite(pga2311_data[dev].cs, LOW);     //CS = on
          //delayMicroseconds(10);
          SPI.transfer(right);
          SPI.transfer(left);
          digitalWrite(pga2311_data[dev].cs, HIGH);    //CS = off
          //delayMicroseconds(10);
          //SPI.endTransaction(); 
          //delay(100);

          Serial.printf("WRITE: PGA2311 Device %i Channel %i Volume %i \n", dev, channel, val);
        }    // value has changed
      } else {
        retval = ERROR_NOT_CONNECTED;
        if (DEBUG > 0) {
          Serial.printf("WRITE: PGA2311 Device %i not connected \n", dev);
        }
      }
    } else {
      retval = ERROR_DEV_RANGE;
      if (DEBUG > 0) {
        Serial.printf("WRITE: PGA2311 Device Number %i out of range \n", dev);
      }
    }
  } else {
    retval = ERROR_PIN_RANGE;
    if (DEBUG > 0) {
      Serial.printf("WRITE: PGA2311 Device %i Channel %i out of range \n", dev, channel);
    }
  }

  return retval;
}
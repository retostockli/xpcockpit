#include <SPI.h>

#define SCLK 13  // Pin 6 of PGA2311
#define MISO 12  // Pin 7 of PGA2311, Optional, since no output from IC
#define MOSI 11  // Pin 3 of PGA2311

#define MAX_SPI 1 // SPI #0 Interface pins (not currently supporting SPI # 1)

int spi_init(int spinum) {
    
  int retval = 0;

  if ((spinum >=0) && (spinum < MAX_SPI)) {

    if (teensy_data.pinmode[SCLK] != INITVAL) {
      retval = -1;
      if (DEBUG > 0) {
        Serial.printf("INIT: SPI Bus # %i SPI Pin %i for SCLK already used \n", spinum, SCLK);
      }
    }

    if (teensy_data.pinmode[MISO] != INITVAL) {
      retval = -1;
      if (DEBUG > 0) {
        Serial.printf("INIT: SPI Bus # %i SPI Pin %i for MISO already used \n", spinum, MISO);
      }
    }

    if (teensy_data.pinmode[MOSI] != INITVAL) {
      retval = -1;
      if (DEBUG > 0) {
        Serial.printf("INIT: SPI Bus # %i SPI Pin %i for MOSI already used \n", spinum, MOSI);
      }
    }

    if (retval == 0) {
      // set up SPI
      SPI.setMOSI(MOSI); 
      SPI.setMISO(MISO);
      SPI.setSCK(SCLK);
      SPI.begin();

      teensy_data.pinmode[SCLK] = PINMODE_SPI;
      teensy_data.pinmode[MISO] = PINMODE_SPI;
      teensy_data.pinmode[MOSI] = PINMODE_SPI;
      
      spi_initialized = true;
    } 
  } else {
    retval = -1;
    if (DEBUG > 0) {
      Serial.printf("INIT: SPI Bus # %i Out of Range \n", spinum);
    }
   
  }

  return retval;
}

void spi_exit() {
    SPI.end();
}
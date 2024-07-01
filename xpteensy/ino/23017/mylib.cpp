#include <Arduino.h>
#include "mylib.h"

int encoder(uint8_t obits[2], uint8_t nbits[2]) {

  int updown = 0;

  if ((obits[0] == 0) && (obits[1] == 1) && (nbits[0] == 0) && (nbits[1] == 0)) {
    updown = 1;
  } else if ((obits[0] == 0) && (obits[1] == 1) && (nbits[0] == 1) && (nbits[1] == 1)) {
    updown = -1;
  } else if ((obits[0] == 1) && (obits[1] == 0) && (nbits[0] == 1) && (nbits[1] == 1)) {
    updown = 1;
  } else if ((obits[0] == 1) && (obits[1] == 0) && (nbits[0] == 0) && (nbits[1] == 0)) {
    updown = -1;
    /* in case a) an input was missed or b) the two optical rotary inputs were transmitted 
			   concurrently due to timing issue, assume that the last direction is still valid.
			   ONLY USE WITH FAST TURNING DT < 100 ms */
  }

  return updown;
}

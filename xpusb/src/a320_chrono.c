/* This is the a320_chrono.c code which contains code for how to communicate with my Airbus 
   A320 hardware (switches, LED's, displays) connected to the OpenCockpits IOCARDS USB device.

   To be used with the QPAC Basic A320 Airbus V2.04.

   Copyright (C) 2014  Hans Jansen, inspired by Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "libiocards.h"
#include "serverdata.h"
#include "a320_chrono.h"

void a320_chrono (void);

/* variables */

  static int coldAndDark = 0, lightsTest = 0;
  int chrHrs, chrMins, chrSecs;		// CHR: chronometer
  int localHrs, localMins, localSecs;	// UTC: time/date
  int zuluHrs, zuluMins, zuluSecs;
  int utcYear = 20, utcMonth, utcDay;	//     year: read from ini file?
  int timerHrs, timerMins, ETSwitch;	// ET: elapsed time
  int *chronoReset, *chronoStartStop, *timerShowDate;

  int timerDateShows, timerRuns;
//  int  timerGmt, timerMode;

  char tempStr[] = "  \0";		// temporary string for formatting 4+4 bits into unsigned char (is there a better way?)
  unsigned char outBuf[] = { 0xAA, 0xAA, 0x00, 0x00, 0x00, 0xAA, 0xAA, 0x0F }; /* normal initial value */
  unsigned char  inBuf[] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x0F };
  int bufferSize = 8;

extern int verbose;

void a320_chrono(void) {

  /* Request X-Plane variables */

  // Note: these two datarefs are maintained by the a320_overhead.c module
  int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
  if (*cold_and_dark == INT_MISS) coldAndDark = 2; else coldAndDark = *cold_and_dark;
  int *lights_test = link_dataref_int ("xpserver/lights_test");
  if (*lights_test == INT_MISS) lightsTest = 0; else lightsTest = *lights_test;

  int *timer_cycle = link_dataref_cmd_once ("sim/instruments/timer_cycle");
  int *timer_gmt = link_dataref_cmd_once ("sim/instruments/timer_is_GMT");
  int *timer_mode = link_dataref_cmd_once ("sim/instruments/timer_mode");
  chronoReset = link_dataref_cmd_once ("sim/instruments/timer_reset");		// CHR RST button
  chronoStartStop = link_dataref_cmd_once ("sim/instruments/timer_start_stop");	// CHR button
  timerShowDate = link_dataref_cmd_once ("sim/instruments/timer_show_date");	// Date SET button
  int *et_start_stop = link_dataref_int ("AirbusFBW/ClockETSwitch");		 // ET RUN switch (! seems not-writable !)
  if (*et_start_stop == INT_MISS) ETSwitch = 0; else ETSwitch = *et_start_stop;

//  int *timer_cur_year = link_dataref_int ("sim/cockpit2/clock_timer/current_year"); // ! Not in simulator !!
//  if (*timer_cur_year == INT_MISS) utcYear = 0; else utcYear = *timer_cur_year;
  int *timer_cur_month = link_dataref_int ("sim/cockpit2/clock_timer/current_month");
  if (*timer_cur_month == INT_MISS) utcMonth = 0; else utcMonth = *timer_cur_month;
  int *timer_cur_day = link_dataref_int ("sim/cockpit2/clock_timer/current_day");
  if (*timer_cur_day == INT_MISS) utcDay = 0; else utcDay = *timer_cur_day;
//  int *timer_chr_hrs = link_dataref_int ("sim/cockpit2/clock_timer/elapsed_time_hours"); // Not in plane
//  if (*timer_chr_hrs == INT_MISS) chrHrs = 0; else chrHrs = *timer_chr_hrs;
  int *timer_chr_mins = link_dataref_int ("sim/cockpit2/clock_timer/elapsed_time_minutes");
  if (*timer_chr_mins == INT_MISS) chrMins = 0; else chrMins = *timer_chr_mins;
  int *timer_chr_secs = link_dataref_int ("sim/cockpit2/clock_timer/elapsed_time_seconds");
  if (*timer_chr_secs == INT_MISS) chrSecs = 0; else chrSecs = *timer_chr_secs;
  int *timer_local_hrs = link_dataref_int ("sim/cockpit2/clock_timer/local_time_hours");
  if (*timer_local_hrs == INT_MISS) localHrs = 0; else localHrs = *timer_local_hrs;
  int *timer_local_mins = link_dataref_int ("sim/cockpit2/clock_timer/local_time_minutes");
  if (*timer_local_mins == INT_MISS) localMins = 0; else localMins = *timer_local_mins;
  int *timer_local_secs = link_dataref_int ("sim/cockpit2/clock_timer/local_time_seconds");
  if (*timer_local_secs == INT_MISS) localSecs = 0; else localSecs = *timer_local_secs;
  int *timer_zulu_hrs = link_dataref_int ("sim/cockpit2/clock_timer/zulu_time_hours");
  if (*timer_zulu_hrs == INT_MISS) zuluHrs = 0; else zuluHrs = *timer_zulu_hrs;
  int *timer_zulu_mins = link_dataref_int ("sim/cockpit2/clock_timer/zulu_time_minutes");
  if (*timer_zulu_mins == INT_MISS) zuluMins = 0; else zuluMins = *timer_zulu_mins;
  int *timer_zulu_secs = link_dataref_int ("sim/cockpit2/clock_timer/zulu_time_seconds");
  if (*timer_zulu_secs == INT_MISS) zuluSecs = 0; else zuluSecs = *timer_zulu_secs;
  int *timer_shows_date = link_dataref_int ("sim/cockpit2/clock_timer/date_is_showing");
  if (*timer_shows_date == INT_MISS) timerDateShows = 0; else timerDateShows = *timer_shows_date;
//  int *timer_gmt = link_dataref_int ("sim/cockpit2/clock_timer/timer_is_GMT"); // Not in plane
//  if (*timer_gmt == INT_MISS) timerGmt = 0; else timerGmt = *timer_gmt;
//  int *timer_mode = link_dataref_int ("sim/cockpit2/clock_timer/timer_mode"); // ??
//  if (*timer_mode == INT_MISS) timerMode = 0; else timerMode = *timer_mode;
  int *timer_runs = link_dataref_int ("sim/cockpit2/clock_timer/timer_running");
  if (*timer_runs == INT_MISS) timerRuns = 0; else timerRuns = *timer_runs;
  int *timer_et_hrs = link_dataref_int ("AirbusFBW/ClockETHours");
  if (*timer_et_hrs == INT_MISS) timerHrs = 0; else timerHrs = *timer_et_hrs;
  int *timer_et_mins = link_dataref_int ("AirbusFBW/ClockETMinutes");
  if (*timer_et_mins == INT_MISS) timerMins = 0; else timerMins = *timer_et_mins;

}

/* Compose the 8-byte output record */
/* This function returns the complete output record. 
   Note that that has to be freed by the caller! */
unsigned char* output_chrono320 () {
  int bufferSize = 8;
  unsigned char *outBuf = malloc (bufferSize); // Note: must be freed by caller!

  /* fill the buffer */
  if (coldAndDark == 2) {	// cold&dark: blank all display digits and decimal points
				// otherwise, check and prepare output for the module
    outBuf[0] = 0xaa; outBuf[1] = 0xaa; outBuf[2] = 0xaa; outBuf[3] = 0xaa;
    outBuf[4] = 0xaa; outBuf[5] = 0xaa; outBuf[6] = 0xaa; outBuf[7] = 0x00;
  } else {
    if (lightsTest == 1) {
    outBuf[0] = 0x88; outBuf[1] = 0x88; outBuf[2] = 0x88; outBuf[3] = 0x88;
    outBuf[4] = 0x88; outBuf[5] = 0x88; outBuf[6] = 0x88; outBuf[7] = 0xff;
    } else {
      if (timerDateShows == 0) {
        outBuf[7] = 0xff;
//      if (ETSwitch == 0)
//        outBuf[7] = outBuf[7] & 0x7f; /* outBuf[6] = 0xaa; outBuf[5] = 0xaa; */ // (switch non-functional)
//      else
//        outBuf[7] = outBuf[7] | 0x80;
        // display chrono, daytime and elapsed-time
        sprintf (tempStr, "%02d", timerHrs);  outBuf[6] = ((tempStr[0]&15)<<4) | tempStr[1]&15;
        sprintf (tempStr, "%02d", timerMins); outBuf[5] = ((tempStr[0]&15)<<4) | tempStr[1]&15;
        sprintf (tempStr, "%02d", zuluMins);  outBuf[3] = ((tempStr[0]&15)<<4) | tempStr[1]&15;
        sprintf (tempStr, "%02d", zuluHrs);   outBuf[4] = ((tempStr[0]&15)<<4) | tempStr[1]&15;
        sprintf (tempStr, "%02d", zuluSecs);  outBuf[2] = ((tempStr[0]&15)<<4) | tempStr[1]&15;
        sprintf (tempStr, "%02d", chrMins);   outBuf[1] = ((tempStr[0]&15)<<4) | tempStr[1]&15;
        sprintf (tempStr, "%02d", chrSecs);   outBuf[0] = ((tempStr[0]&15)<<4) | tempStr[1]&15;
      } else {
        // blank chrono and elapsed-time, display date
        outBuf[7] = 0x6f; outBuf[6] = 0xaa; outBuf[5] = 0xaa; outBuf[1] = 0xaa; outBuf[0] = 0xaa;
        sprintf (tempStr, "%02d", utcDay);    outBuf[3] = ((tempStr[0]&15)<<4) | tempStr[1]&15;
        sprintf (tempStr, "%02d", utcMonth);  outBuf[4] = ((tempStr[0]&15)<<4) | tempStr[1]&15;
        sprintf (tempStr, "%02d", utcYear);   outBuf[2] = ((tempStr[0]&15)<<4) | tempStr[1]&15;
      } // end (timerDateShows)
    } // end (lightsTest)
  } // end (coldDark)
  return (outBuf);
}

/* Dissemble the 8-byte input record */
int input_chrono320 (unsigned char inbuf[]) { /* Chrono320 raw input data */
//  printf ("input contains %01x %01x %01x %01x  %01x %01x %01x %01x   %01x %01x %01x %01x  %01x %01x %01x %01x  \n",
//           inbuf[0] >> 7 & 0x01, inbuf[0] >> 6 & 0x01, inbuf[0] >> 5 & 0x01, inbuf[0] >> 4 & 0x01, 
//           inbuf[0] >> 3 & 0x01, inbuf[0] >> 2 & 0x01, inbuf[0] >> 1 & 0x01, inbuf[0] >> 0 & 0x01, 
//           inbuf[1] >> 7 & 0x01, inbuf[1] >> 6 & 0x01, inbuf[1] >> 5 & 0x01, inbuf[1] >> 4 & 0x01, 
//           inbuf[1] >> 3 & 0x01, inbuf[1] >> 2 & 0x01, inbuf[1] >> 1 & 0x01, inbuf[1] >> 0 & 0x01);
  /* the Chrono RST button - timer reset */
  int rstBut = inbuf[1] >> 3 & 0x01; // 0 = button pressed
  if (rstBut == 0) *chronoReset = 1;
  /* the Chrono CHR button - timer stop/start */
  int chrBut = inbuf[0] >> 0 & 0x01; // 0 = button pressed
  if (chrBut == 0) *chronoStartStop = 1;
//  printf ("CHR: RST = %i, CHR = %i\n", rstBut, chrBut);

  /* the UTC encoder - no function (yet) */
  int utcEnc = inbuf[1] >> 1 & 0x03;
  // t.b.s.
  /* the UTC SET button - show the date */
  int utcSet = inbuf[1] >> 0 & 0x01; // 0 = button pressed
  if (utcSet == 0) *timerShowDate = 1;
//  printf ("UTC: ENC = %i,  SET = %i, utcEnc, utcSet);

  /* the UTC GPS switch  - no function */
  int gpsSwt = inbuf[0] >> 5 & 0x03;
  // t.b.s.
//  printf ("GPS: GPS = %i,\n", gpsSwt);
  
  /* the ET RUN switch - ET start/stop only (no reset...) */
  int runSwt = inbuf[0] >> 1 & 0x03; // 3 = run, 2 = stop, 1 = reset
  if (runSwt == 3) ETSwitch = 1; else ETSwitch = 0;
//  printf (" ET:: RUN:: %i\n", runSwt);
//  but: non-functional!

}


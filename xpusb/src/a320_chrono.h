/* This is the a320_chrono.h file which contains definitions for communication with my Airbus
   A320 hardware (switches, LED's, displays) connected to the OpenCockpits IOCARDS USB device.

   Copyright (C) 2020  Hans Jansen, inspired by Reto Stockli */

#ifndef _A320_OVERHEAD_H
#define _A320_OVERHEAD_H	1

/* INCLUDES */

/* definitions */

/* Prototype Functions */

void a320_chrono (void);

int input_chrono320 (unsigned char*);
unsigned char* output_chrono320 (void);

#endif


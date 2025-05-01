/* This is the usb_interface.h header to the usb_interface.c code

   Copyright (C) 2010  Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

/* posix tread headers */
#include <assert.h>
#include <pthread.h>

/* libusb-1.0 header */
#include <libusb-1.0/libusb.h>

/* USB DEFINE STATEMENTS */
# define MAXDEVICES 10
# define READBUFFERSIZE 1024
# define WRITEBUFFERSIZE 1024
# define USB_DEBUG 2 /* usb debugging option */

/* USB PARAMETERES */
typedef struct {
  libusb_device_handle *handle;         /* device handle */
  unsigned char epIn;                   /* input endpoint */
  unsigned char epOut;                  /* output endpoint */
  struct libusb_transfer *readTransfer; /* libusb read transfer handle */
  struct libusb_transfer *writeTransfer;/* libusb write transfer handle */
  uint16_t inBufferSize;                /* input endpoint buffer size */
  uint16_t outBufferSize;               /* output endpoint buffer size */
  unsigned char *inBuffer;              /* pointer to input endpoint buffer */
  unsigned char *outBuffer;             /* pointer to output endpoint buffer */
  uint16_t readBufferSize;              /* read buffer size */
  uint16_t writeBufferSize;             /* write buffer size */
  unsigned char *readBuffer;            /* pointer to read buffer */
  unsigned char *writeBuffer;           /* pointer to write buffer */
  int readLeft;                         /* counter for yet to be read data packets */
  int writeLeft;                        /* counter for yet to be written data packets */
  int status;                           /* device status (-1: not present, 0: present and ready) */
  int number;                           /* device number (0 ... MAXDEVICES-1)*/
  int iswriting;                        /* device is currently processing write operation */
} device_struct;

device_struct device[MAXDEVICES];

/* USB PROTOTYPE FUNCTIONS */
int initialize_usb(void);
void exit_usb(void);
int check_usb(int number, uint16_t vendor, uint16_t product, uint8_t bus, uint8_t address);
int write_usb(int number, unsigned char *bytes, int size);
int read_usb(int number, unsigned char *bytes, int size);

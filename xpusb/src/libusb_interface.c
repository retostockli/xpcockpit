/* This is the usb_interface.c code which handles threaded asynchronous read and standard asynchronous
   write operations on LINUX to a IOCARDS USB device.

   Copyright (C) 2009  Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

/* 2009/06/07 : added support for OSX with libusb-1.0.1, thus usb_darwin.c becomes obsolete */

/* DEPENDENCIES: libusb-1.0 (NOT TO BE CONFUSED WITH libusb-0.1) */
/* tested with libusb-1.0.2 on 2009/07/19 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef LIBUSB

/* standard unix headers */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/* libusb_interface headers */
#include <libusb_interface.h>

/* usb interface thread parameters */
pthread_t poll_thread;
pthread_mutex_t exit_cond_lock = PTHREAD_MUTEX_INITIALIZER;
int thread_exit_code = 0; /* thread exit code */

/* prototype functions */
void *poll_thread_main(void *arg);

void *poll_thread_main(void *arg)
/* single thread handles all asynchronous USB transfers */
{
  int ret, i;

  if (USB_DEBUG > 1) printf("LIBUSB_INTERFACE: Asynchronous poll thread running\n");

  while (!thread_exit_code) {
    struct timeval tv = { 1, 0 };
    ret = libusb_handle_events_timeout(NULL, &tv);
    if (ret < 0) {
      thread_exit_code = 1;
      break;
    }
  }
  
  if (USB_DEBUG > 1) printf("LIBUSB_INTERFACE: Asynchronous poll thread shutting down\n");

  /*  release interfaces and close open devices */
  for (i=0;i<MAXDEVICES;i++) {
    if ((device[i].status == 0) && (device[i].handle)) {
      printf("Closing device %i \n",i);

      /* cancel read transfer and free transfer descriptor */
      if (device[i].readTransfer) {
	if (libusb_cancel_transfer(device[i].readTransfer) < 0) {
	  printf("LIBUSB_INTERFACE: read transfer could not be cancelled\n");
	}

	printf("read transfer status: %i \n",device[i].readTransfer->status);

	/* wait until read transfer has shut down */
	while (device[i].readTransfer->status != 0) 
	  if (libusb_handle_events(NULL) < 0)
	    break;

	libusb_free_transfer(device[i].readTransfer);
      }
  

      /* cancel write transfer and free transfer descriptor */
      if (device[i].writeTransfer) {
	if (libusb_cancel_transfer(device[i].writeTransfer) < 0) {
	  printf("LIBUSB_INTERFACE: write transfer could not be cancelled\n");
	}

	printf("write transfer status: %i \n",device[i].writeTransfer->status);

	/* wait until write transfer has shut down */
	while (device[i].writeTransfer->status != 0) 
	  if (libusb_handle_events(NULL) < 0)
	    break;

	libusb_free_transfer(device[i].writeTransfer);
      }

      /* deallocate read/write buffers */
      /*
      if (device[i].inBuffer) free(device[i].inBuffer);
      if (device[i].outBuffer) free(device[i].outBuffer);
      if (device[i].readBuffer) free(device[i].readBuffer);
      if (device[i].writeBuffer) free(device[i].writeBuffer);
      */

      /* release usb interface */
      libusb_release_interface(device[i].handle, 0);

      /* release usb device */
      libusb_close(device[i].handle);

      /* reset device descriptors */
      device[i].handle = NULL;
      device[i].epIn = 0xFF;
      device[i].epOut = 0xFF;
      device[i].readTransfer = NULL;
      device[i].writeTransfer = NULL;
      device[i].inBufferSize = 0x0;
      device[i].outBufferSize = 0x0;
      device[i].inBuffer = NULL;
      device[i].outBuffer = NULL;
      device[i].readBufferSize = 0;
      device[i].writeBufferSize = 0;
      device[i].readBuffer = NULL;
      device[i].writeBuffer = NULL;
      device[i].readLeft = 0;
      device[i].writeLeft = 0;
      device[i].status = -1;
      device[i].number = -1;
      device[i].iswriting = 0;
    }
  }

  if (USB_DEBUG > 0) printf("LIBUSB_INTERFACE: Stopping USB driver\n");
  libusb_exit(NULL);


  return(0);
}

static void read_callback(struct libusb_transfer *transfer)
{
  /* get user data */
  int *number = transfer->user_data;

  if (transfer->status != LIBUSB_TRANSFER_COMPLETED) {
    /* read callback was not successful: exit! */

    if (USB_DEBUG > 1) printf("LIBUSB_INTERFACE: device %i read transfer error %d\n",*number,transfer->status);
    device[*number].readTransfer = NULL;
    thread_exit_code = 1;
    return;

  } else {
    /* read callback was successful: fill read buffer to the right of the last position */

    if ((device[*number].inBufferSize > 0) && 
	((device[*number].readLeft + device[*number].inBufferSize) 
	 <= device[*number].readBufferSize)) {

      pthread_mutex_lock(&exit_cond_lock);
      memcpy(device[*number].readBuffer,device[*number].inBuffer,device[*number].inBufferSize);
      device[*number].readLeft += device[*number].inBufferSize;
      device[*number].readBuffer += device[*number].inBufferSize;
      pthread_mutex_unlock(&exit_cond_lock);

      if (USB_DEBUG > 1) printf("LIBUSB_INTERFACE: device %i read buffer position=%i \n",*number, device[*number].readLeft);
    }
    
    if (USB_DEBUG > 1) printf("LIBUSB_INTERFACE: device %i submit new read transfer\n",*number);
    
    if (libusb_submit_transfer(device[*number].readTransfer) < 0)
      thread_exit_code = 1;

    if (USB_DEBUG > 1) printf("LIBUSB_INTERFACE: device %i read transfer complete\n",*number);

  }

  return;
}

static void write_callback(struct libusb_transfer *transfer)
{
  /* get user data */
  int *number = transfer->user_data;

  if (transfer->status != LIBUSB_TRANSFER_COMPLETED) {
    /* write callback was not successful: exit! */

    if (USB_DEBUG > 1) printf("LIBUSB_INTERFACE: device %i write transfer error %d\n",*number, transfer->status);
    device[*number].writeTransfer = NULL;
    thread_exit_code = 1;
    return;

  } else {
    /* write callback was successful */

    /* resubmit write call if there is new data in write buffer */   
    if (device[*number].writeLeft > 0) {
      
      if (USB_DEBUG > 1) printf("LIBUSB_INTERFACE: device %i write buffer position=%i \n",
				*number,device[*number].writeLeft);

      pthread_mutex_lock(&exit_cond_lock);
      /* decrease write buffer position and count */
      device[*number].writeLeft -= device[*number].outBufferSize;
      device[*number].writeBuffer -= device[*number].outBufferSize;
      /* copy rightmost write buffer to output buffer */
      memcpy(device[*number].outBuffer,device[*number].writeBuffer,device[*number].outBufferSize);   
      pthread_mutex_unlock(&exit_cond_lock);

      if (USB_DEBUG > 1) printf("LIBUSB_INTERFACE: device %i submit new write transfer\n",*number);    
      if (libusb_submit_transfer(device[*number].writeTransfer) < 0)
	thread_exit_code = 1;

    } else {
      /* reset write flag so that new write submission can be done */
      device[*number].iswriting = 0;
    }

    if (USB_DEBUG > 1) printf("LIBUSB_INTERFACE: device %i write transfer complete\n",*number);

  }

  return;
}

// empty the read buffer with input data. The usb asynchronous read thread above fills
// the read buffer everytime data is available on the usb device
int read_usb (int number, unsigned char *bytes, int size)
{

  int result = 0;
          
  if (device[number].status != 0) {
    if (USB_DEBUG > 1) printf("LIBUSB_INTERFACE: device %i is not ready or not connected. \n",number);
    result = -1;
    return result;
  }

  if (device[number].inBufferSize != size) {
    if (USB_DEBUG > 1) printf("LIBUSB_INTERFACE: device %i has read buffer size %i Bytes and not %i Bytes \n",
			      number,device[number].inBufferSize, size);
    result = -2;
    return result;
  }

  if (device[number].readLeft > 0) {
    /* read from start of read buffer */
    memcpy(bytes,device[number].readBuffer-device[number].readLeft,device[number].inBufferSize);
    /* shift remaining read buffer to the left */
    if (device[number].readLeft-device[number].inBufferSize > 0) {
      memmove(device[number].readBuffer-device[number].readLeft,
	      device[number].readBuffer-device[number].readLeft+device[number].inBufferSize,
	      device[number].readLeft-device[number].inBufferSize);
    }
    /* decrease read buffer position and counter */
    device[number].readLeft -= device[number].inBufferSize;
    device[number].readBuffer -= device[number].inBufferSize;

    if (USB_DEBUG > 1) {
      printf("LIBUSB_INTERFACE: read buffer %02x %02x %02x %02x %02x %02x %02x %02x \n",
	     bytes[0],bytes[1],bytes[2],bytes[3],bytes[4],bytes[5],bytes[6],bytes[7]);
    }

    result = device[number].inBufferSize;
  } 

  return result;
}

// fill the write buffer with output data and send it to the usb device
int write_usb (int number, unsigned char *bytes, int size)
{
 
  int result = 0;
         
  if (device[number].status != 0) {
    if (USB_DEBUG > 1) printf("LIBUSB_INTERFACE: device %i is not ready or not connected. \n",number);
    result = -1;
    return result;
  }

  if (device[number].outBufferSize != size) {
    if (USB_DEBUG > 1) printf("LIBUSB_INTERFACE: device %i has write buffer size %i Bytes and not %i Bytes \n",
			      number,device[number].outBufferSize, size);
    result = -2;
    return result;
  }

  if ((device[number].writeLeft+device[number].outBufferSize) <= device[number].writeBufferSize) {
    
    /* shift existing write buffer to the right */
    if (device[number].writeLeft > 0) {
      memmove(device[number].writeBuffer-device[number].writeLeft+device[number].outBufferSize,
	      device[number].writeBuffer-device[number].writeLeft,
	      device[number].writeLeft);
    }
    /* fill start of write buffer with new write data */
    memcpy(device[number].writeBuffer-device[number].writeLeft,bytes,device[number].outBufferSize);
    /* increase write buffer position and counter */
    device[number].writeBuffer += device[number].outBufferSize;
    device[number].writeLeft += device[number].outBufferSize;


    /* do not fill the output buffer while the past write call is not over */
    while (device[number].iswriting) {
      usleep(1000);
    }
    
    /* submit asynchronous write call if this is the first write call to the device */
    if (device[number].writeLeft > 0) {
      if (USB_DEBUG > 1) printf("LIBUSB_INTERFACE: device %i write buffer position=%i \n",number,device[number].writeLeft);

      device[number].writeLeft -= device[number].outBufferSize;
      device[number].writeBuffer -= device[number].outBufferSize;
      memcpy(device[number].outBuffer,device[number].writeBuffer,device[number].outBufferSize);

      /* submit first asynchronous write call */
      device[number].iswriting = 1;
      if (libusb_submit_transfer(device[number].writeTransfer) < 0)
	thread_exit_code = 1;
    }

    result = device[number].outBufferSize;

    if (USB_DEBUG > 1) {
      printf("LIBUSB_INTERFACE: write buffer %02x %02x %02x %02x %02x %02x %02x %02x \n",
	     bytes[0],bytes[1],bytes[2],bytes[3],bytes[4],bytes[5],bytes[6],bytes[7]);
    }
  }

  return result;
}


/* find suitable USB devices and set up asynchronous read and write calls */
/* device path not used in libusb interface. Please use bus/address combination */
/* or set bus / address to 0xFF to open any device with given product / vendor # combination */
int check_usb(int number, uint16_t vendor, uint16_t product, uint8_t bus, uint8_t address, const char *path)
{
  int result = 0;
  int ret;

  libusb_device **list;
  libusb_device *dev;
  libusb_device *found = NULL;
  ssize_t i = 0;

  if (number>=MAXDEVICES)
    return -4;

  // discover devices
  ssize_t cnt = libusb_get_device_list(NULL, &list);
  if (cnt < 0)
    return -1;
  
  for (i = 0; i < cnt; i++) {
    dev = list[i];

    struct libusb_device_descriptor desc;  
    ret = libusb_get_device_descriptor(dev, &desc);
    if (ret < 0) {
      printf("LIBUSB_INTERFACE: Failed to get device descriptor %i \n",ret);
      result = -2;
      break;
    }

    if (desc.idVendor == vendor && desc.idProduct == product) found = dev;

    if (found && (bus != 0xFF) && (address != 0xFF)) {
      if ((bus != libusb_get_bus_number(dev)) || (address != libusb_get_device_address(dev))) {
	found = NULL;
      }
    }

    if (found) {
      printf("LIBUSB_INTERFACE: Found Device idVendor:idProduct %04x:%04x (bus %d, dev %d)\n",
	     desc.idVendor, desc.idProduct,
	     libusb_get_bus_number(dev), libusb_get_device_address(dev));
      break;
    }
  }
  
  if (found) {    

    /* open device */
    libusb_device_handle *handle;
    ret = libusb_open(found, &handle);
    if (ret < 0) {
      printf("LIBUSB_INTERFACE: Failed to open device\n");
      result = -3;
    } else {
      /* check if a kernel driver is active on the device */
      ret = libusb_kernel_driver_active(handle, 0);
      if (ret == 1) {
	printf("LIBUSB_INTERFACE: Kernel driver is active on interface 0 \n");
	/* kernel driver is active, so try detaching it */
	ret = libusb_detach_kernel_driver(handle, 0);
	if (ret == 0) {
	  printf("LIBUSB_INTERFACE: Kernel driver successfully detached \n");
	} else {
	  printf("LIBUSB_INTERFACE: Could not detach kernel driver: %i \n",ret);
	}
      }

      /* claim device interface */
      ret = libusb_claim_interface(handle, 0);
      if (ret < 0) {
	printf("LIBUSB_INTERFACE: Could not claim interface 0 \n");
	result = -4;
      } else {
	
	/* retrieve device information */
	struct libusb_config_descriptor *config;
	ret = libusb_get_config_descriptor(dev, 0, &config);
	if (ret < 0) {
	  printf("LIBUSB_INTERFACE: Failed to get config descriptor: %i \n",ret);
	  result = -5;
	} else {
	  	  	  
	  device[number].handle = handle;
	  device[number].status = 0;
	  	  
	  printf("LIBUSB_INTERFACE: Number of Interfaces %d \n",config->bNumInterfaces);    
	  
	  const struct libusb_interface *iface = &config->interface[0];
	  printf("LIBUSB_INTERFACE: Number of Altsettings %d \n",iface->num_altsetting);  
	  
	  const struct libusb_interface_descriptor *altsetting = &iface->altsetting[0];
	  printf("LIBUSB_INTERFACE: Number of Endpoints %d \n", altsetting->bNumEndpoints);
	  
	  int ep_idx;
	  for (ep_idx = 0; ep_idx < altsetting->bNumEndpoints; ep_idx++) {
	    const struct libusb_endpoint_descriptor *ep = &altsetting->endpoint[ep_idx];
	    printf("LIBUSB_INTERFACE: Endpoint Address %i 0x%02x \n",ep_idx,ep->bEndpointAddress); 

	    /* cycle through endpoints and set up asynchronous transfers */

	    if (ep->bEndpointAddress >= 0x80) {
	      /* input endpoint, usually 0x81 */
	      device[number].epIn = ep->bEndpointAddress;
	      device[number].inBufferSize = ep->wMaxPacketSize;

	      printf("LIBUSB_INTERFACE: Allocating read transfer\n");
	      device[number].readTransfer = libusb_alloc_transfer(0);
	      if (!device[number].readTransfer) result = -6;

	      device[number].inBuffer = malloc(device[number].inBufferSize);
	      device[number].readBuffer = malloc(device[number].readBufferSize);


	      printf("LIBUSB_INTERFACE: Filling read transfer\n");
	      libusb_fill_interrupt_transfer(device[number].readTransfer, device[number].handle, 
					     device[number].epIn,device[number].inBuffer,
					     device[number].inBufferSize, read_callback, &device[number].number, 0);

	      printf("LIBUSB_INTERFACE: Submitting read transfer\n");
	      if (libusb_submit_transfer(device[number].readTransfer) < 0) result = -7;

	    } else {
	      /* output endpoint, usually: 0x01 */
	      device[number].epOut = ep->bEndpointAddress;
	      device[number].outBufferSize = ep->wMaxPacketSize;

	      printf("LIBUSB_INTERFACE: Allocating write transfer\n");
	      device[number].writeTransfer = libusb_alloc_transfer(0);
	      if (!device[number].writeTransfer) result = -8;

	      device[number].outBuffer = malloc(device[number].outBufferSize);
	      device[number].writeBuffer = malloc(device[number].writeBufferSize);

	      printf("LIBUSB_INTERFACE: Filling write transfer\n"); 
	      libusb_fill_interrupt_transfer(device[number].writeTransfer, device[number].handle, 
					     device[number].epOut,device[number].outBuffer,
					     device[number].outBufferSize, write_callback, &device[number].number, 0);
	    }
 
	  }
	  
	  libusb_free_config_descriptor(config);
	}
      }
    }
  } else {
    /* no device found */
    printf("LIBUSB_INTERFACE: Device %i not found.\n",number);
    result = -6;
  }

  libusb_free_device_list(list, 1);

  return result;

}


/* initialize usb connection */  
int initialize_usb(void) 
{
  int ret,i;

  if (USB_DEBUG > 0) printf("LIBUSB_INTERFACE: Starting USB driver\n");

  /* general: initialize USB interface */
  ret = libusb_init(NULL);

  /* set debugging option */
  libusb_set_debug(NULL,USB_DEBUG);
      
  /* start a thread that handles the asynchronous USB communication */
  ret = pthread_create(&poll_thread, NULL, &poll_thread_main, NULL);
  if (ret>0) {
    printf("LIBUSB_INTERFACE: thread could not be created\n");
  }

  /* reset device info */
  for (i=0;i<MAXDEVICES;i++) {
    device[i].handle = NULL;
    device[i].epIn = 0xFF;
    device[i].epOut = 0xFF;
    device[i].readTransfer = NULL;
    device[i].writeTransfer = NULL;
    device[i].inBufferSize = 0x0;
    device[i].outBufferSize = 0x0;
    device[i].inBuffer = NULL;
    device[i].outBuffer = NULL;
    device[i].readBufferSize = READBUFFERSIZE;
    device[i].writeBufferSize = WRITEBUFFERSIZE;
    device[i].readBuffer = NULL;
    device[i].writeBuffer = NULL;
    device[i].readLeft = 0;
    device[i].writeLeft = 0;
    device[i].status = -1;
    device[i].number = i;
    device[i].iswriting = 0;
  }

  return  ret;
}


/* terminate usb thread and close usb connections */
void exit_usb()
{
  if (USB_DEBUG > 0) printf("LIBUSB_INTERFACE: Exiting USB driver\n");

  /* stop thread (includes deallocating all usb data) */  
  thread_exit_code = 1;
  pthread_join(poll_thread, NULL);

}

#endif

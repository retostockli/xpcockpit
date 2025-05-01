#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HIDAPI

/* standard unix headers */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <hidapi_interface.h>

/* allocation of global variables defined in hidapi_interface.h */
device_struct device[MAXDEVICES];

/* thread parameters */
pthread_mutex_t exit_cond_lock = PTHREAD_MUTEX_INITIALIZER;

void *poll_thread_main(void *arg)
/* thread handles all HID read calls by use of blocking read and a read buffer per device */
{
  int ret = 0;
  int *number = (int*) arg;

  if (HID_DEBUG > 1) printf("HIDAPI_INTERFACE: Poll thread running for device: %i \n",*number);

  if ((*number>=0) && (*number<MAXDEVICES)) {

    /* run thread loop until killed */
    while (!device[*number].poll_thread_exit_code) {

      if (device[*number].inBuffer != NULL) {

	/* read call goes here (100 ms timeout for blocking operation) */
	if (USE_BLOCKING == 1) {
	  ret = hid_read_timeout(device[*number].handle,&device[*number].inBuffer[0],
				 device[*number].inBufferSize,100);
	} else {
	  ret = hid_read(device[*number].handle, &device[*number].inBuffer[0],
			 device[*number].inBufferSize);
	}

	if (ret < 0) {
	  printf("HIDAPI_INTERFACE: device: %i Read Error Code: %i \n",*number,ret);
	  //device[*number].poll_thread_exit_code = 1;
	  //break;
	} else if ((ret > 0) && (ret <= device[*number].inBufferSize)) {
	  /* read is ok */
	  
	  /* does it fit into read buffer? */
	  if ( ((device[*number].readLeft + device[*number].inBufferSize) 
		<= device[*number].readBufferSize) ) {
	    
	    pthread_mutex_lock(&exit_cond_lock);
	    
	    memcpy(&device[*number].readBuffer[device[*number].readLeft],
		   &device[*number].inBuffer[0],device[*number].inBufferSize);
	    device[*number].readLeft += device[*number].inBufferSize;
	    pthread_mutex_unlock(&exit_cond_lock);
	    
	    if (HID_DEBUG > 2) printf("HIDAPI_INTERFACE: device %i read buffer position: %i \n",
				      *number, device[*number].readLeft);
	  } else {
	    if (HID_DEBUG > 0) printf("HIDAPI_INTERFACE: device %i read buffer full: %i \n",
		     *number, device[*number].readLeft);
	  }
	} else if ((ret > 0) && (ret > device[*number].inBufferSize)) {
	  /* too many bytes were read */
	  printf("HIDAPI_INTERFACE: device %i %i out of %i bytes read \n",*number,ret,device[*number].inBufferSize);
	} else {
	  /* nothing read */
	}
	/*  break for 1 ms (only in non-blocking operation) */
	if (USE_BLOCKING == 0) usleep(1000);
      } 
    } /* while loop */
 
    /* thread was killed */
    if (HID_DEBUG > 1) printf("HIDAPI_INTERFACE: Asynchronous poll thread shutting down for device: %i \n",*number);

  } else {
    if (HID_DEBUG > 1) printf("HIDAPI_INTERFACE: device number %i out of range. \n",*number);
  }

  return 0;
}

/* read from HID device */
int read_usb (int number, unsigned char *bytes, int size)
{
  int result = 0;

  if ((number>=0) && (number<MAXDEVICES)) {

    if (device[number].inBuffer != NULL) {

      if (device[number].status != 0) {
	if (HID_DEBUG > 1) printf("HIDAPI_INTERFACE: device %i is not ready or not connected. \n",number);
	result = -1;
	return result;
      }

      if (device[number].inBufferSize != size) {
	if (HID_DEBUG > 1) printf("HIDAPI_INTERFACE: device %i has read buffer size %i Bytes and not %i Bytes \n",
				  number,device[number].inBufferSize, size);
	result = -2;
	return result;
      }

      if (USE_THREADS == 1) {
	/* empty read buffer instead of directly accessing the device */
	
	if (device[number].readLeft > 0) {
	  pthread_mutex_lock(&exit_cond_lock);
	  /* read from start of read buffer */
	  memcpy(bytes,&device[number].readBuffer[0],device[number].inBufferSize);
	  /* shift remaining read buffer to the left */
	  if ((device[number].readLeft-device[number].inBufferSize) > 0) {
	    memmove(&device[number].readBuffer[0],
		    &device[number].readBuffer[device[number].inBufferSize],
		    device[number].readLeft-device[number].inBufferSize);
	  }
	  /* decrease read buffer position and counter */
	  device[number].readLeft -= device[number].inBufferSize;
	  pthread_mutex_unlock(&exit_cond_lock);
      
	  if (HID_DEBUG > 1) {
	    printf("HIDAPI_INTERFACE: read buffer %02x %02x %02x %02x %02x %02x %02x %02x \n",
		   bytes[0],bytes[1],bytes[2],bytes[3],bytes[4],bytes[5],bytes[6],bytes[7]);
	  }

	  /* check if we get buffer overflow due to too many incoming messages */
	  //printf("%i of %i %i \n",device[number].readLeft,device[number].inBufferSize,device[number].readBufferSize);
	  
	  result = device[number].inBufferSize;

	}

      } else {
	/* directly read from the device */
          
	result = hid_read(device[number].handle, &device[number].inBuffer[0], device[number].inBufferSize);
    
	if (result > 0) {
      
	  if (result == device[number].inBufferSize) {
	    memcpy(bytes,&device[number].inBuffer[0],size);   
	
	    if (HID_DEBUG > 1) {
	      printf("HIDAPI_INTERFACE: read buffer %02x %02x %02x %02x %02x %02x %02x %02x \n",
		     bytes[0],bytes[1],bytes[2],bytes[3],bytes[4],bytes[5],bytes[6],bytes[7]);
	    }
	  } else {
	    printf("HIDAPI_INTERFACE: Device %i: only %i out of %i bytes read \n",number,result,device[number].inBufferSize);
	  }
	}

      }

    } else {
      printf("HIDAPI_INTERFACE: Please allocate device input buffer before reading \n");
      result = -3;
    }
  } else {
    if (HID_DEBUG > 1) printf("HIDAPI_INTERFACE: device number %i out of range. \n",number);
    result = -3;
  }

  return result;
}

/* write to HID device */
int write_usb (int number, unsigned char *bytes, int size)
{
  /* directly write output to device (no write buffering, no write thread) */
  
  int result = 0;

  if ((number>=0) && (number<MAXDEVICES)) {

    if (device[number].outBuffer != NULL) {

      /* write with HIDAPI: first byte of output buffer contains report number (0x0) */
         
      if (device[number].status != 0) {
	if (HID_DEBUG > 1) printf("HIDAPI_INTERFACE: device %i is not ready or not connected. \n",number);
	result = -1;
	return result;
      }

      if ((device[number].outBufferSize-1) != size) {
	if (HID_DEBUG > 1) printf("HIDAPI_INTERFACE: device %i has write buffer size %i Bytes and not %i Bytes \n",
				  number,device[number].outBufferSize-1, size);
	result = -2;
	return result;
      }

      memcpy(device[number].outBuffer+1,bytes,size);   
      device[number].outBuffer[0] = 0x0;
      result = hid_write(device[number].handle, device[number].outBuffer, device[number].outBufferSize);

      if (result == device[number].outBufferSize) {

	if (HID_DEBUG > 1) {
	  printf("HIDAPI_INTERFACE: write buffer %02x %02x %02x %02x %02x %02x %02x %02x \n",
		 bytes[0],bytes[1],bytes[2],bytes[3],bytes[4],bytes[5],bytes[6],bytes[7]);
	}

	/* first byte is a control byte */
	result -= 1;
      } else {
	printf("HIDAPI_INTERFACE: Device %i: only %i out of %i bytes written \n",number,result,device[number].outBufferSize);
      }

    } else {
      printf("HIDAPI_INTERFACE: Please allocate device output buffer before writing \n");
      result = -3;
    }
  } else {
    if (HID_DEBUG > 1) printf("HIDAPI_INTERFACE: device number %i out of range. \n",number);
    result = -3;
  }

  return result;
}

/* allocate input and output buffer for device */
int setbuffer_usb(int number, int bufferSize)
{

  int result = 0;

  if ((number>=0) && (number<MAXDEVICES)) {

    if (device[number].status == -1) {
      if (HID_DEBUG > 1) printf("HIDAPI_INTERFACE: device %i is not ready or not connected. \n",number);
      result = -1;
    } else {
	
      if ((device[number].inBuffer == NULL) && (device[number].outBuffer == NULL)) {

	device[number].inBufferSize = bufferSize;
	device[number].outBufferSize = bufferSize+1;
	
	device[number].inBuffer = malloc(device[number].inBufferSize);
	device[number].outBuffer = malloc(device[number].outBufferSize);
	
      } else {
	if (HID_DEBUG > 1) printf("HIDAPI_INTERFACE: device %i in/out buffer already allocated. \n",number);
	result = -2;
      }
    }
  } else {
    if (HID_DEBUG > 1) printf("HIDAPI_INTERFACE: device number %i out of range. \n",number);
    result = -3;
  }

  return result;
}

/* find suitable HID devices and set up asynchronous read and write calls */
/* bus / address not used in hidapi interface, please use path to address device */
/* or set path to "" to open any device with given product / vendor # combination */
/* please supply Product name as first argument to perform a suitable matching */
int check_usb (const char *name, int number, uint16_t vendor, uint16_t product,
	       uint8_t bus, uint8_t address, const char *path, const char *serial)
{
  int result = 0;
  int ret;


#define MAX_STR 255
  hid_device *handle;
  wchar_t wstr[MAX_STR];
  char str[10*MAX_STR];

  // Open the device using the VID, PID,
  // and optionally, if available by the Device Path
  // The latter is recommended if you have many cards
  // of the same type running and want to be sure you
  // address the correct one

   if (strcmp(path,"") == 0) {
     if (strcmp(serial,"") == 0) {
       handle = hid_open(vendor,product, NULL);
     } else {
       swprintf(wstr,MAX_STR,L"%hs",serial);
       handle = hid_open(vendor,product, wstr);
     }
   } else {
     handle = hid_open_path(path);
   }
  
  if (handle) {
    
    device[number].handle = handle;
    device[number].status = 0;
    device[number].number = number;
    printf("HIDAPI_INTERFACE: Device %i found.\n",number);

    // Read the Manufacturer String
    wstr[0] = 0x0000;
    ret = hid_get_manufacturer_string(handle, wstr, MAX_STR);
    printf("Manufacturer String: %ls\n", wstr);

    // Read the Product String and see if it matches device name supplied by user
    wstr[0] = 0x0000;
    ret = hid_get_product_string(handle, wstr, MAX_STR);
    sprintf(str,"%ls",wstr);
    printf("Product String: %s \n",str);

    //   if (strcmp(str,"Bootloader")==0) {
    //    sprintf(str,"BU0836X Interface");
    //  } 

    if (!strncmp(str,name,strlen(name))) {
    
      device[number].readBuffer = malloc(device[number].readBufferSize);
      device[number].writeBuffer = malloc(device[number].writeBufferSize);
      
      /* printf("ALLOC: %i %p %p \n",number,&device[number].readBuffer[0],
	 &device[number].readBuffer[device[number].readBufferSize-1]); */
      
      if (USE_THREADS == 1) {
	
	/* set read to blocking type for threaded read access */
	/* --> Added timeout in blocking read of thread */
	if (USE_BLOCKING == 1) {
	  ret = hid_set_nonblocking(handle,0);
	} else {
	  ret = hid_set_nonblocking(handle,1);
	}
	
	if (ret < 0) {
	  printf("HIDAPI_INTERFACE: Device %i set to non-blocking failed.\n",number);
	  result = -5;
	} else {
	  /* start a thread that handles the read polling to the HID device */
	  device[number].poll_thread_exit_code = 0;
	  ret = pthread_create(&device[number].poll_thread, NULL, &poll_thread_main, &device[number].number);
	  if (ret>0) {
	    printf("HIDAPI_INTERFACE: poll thread could not be created.\n");
	    result = -7;
	  }
	}
	
      } else {
	
	/* set read to non-blocking type for direct device access */
	ret = hid_set_nonblocking(handle,1);
	
	if (ret < 0) {
	  printf("HIDAPI_INTERFACE: Device %i set to non-blocking failed.\n",number);
	  result = -5;
	} 
      }
    } else {
      /* product string does not match user supplied string */
      printf("HIDAPI_INTERFACE: Device %i name (%s) does not match product name: %s\n",number,name,str);
      result = -6;
    }
  } else {
    /* no device found */
    printf("HIDAPI_INTERFACE: Device %i not found.\n",number);
    result = -7;
  }

  return result;

}

/* initialize HID device variables */  
int init_usb (void) 
{
  int result = 0;
  int i;

  if (HID_DEBUG > 0) printf("HID_INTERFACE: Starting HID driver\n");

  /* reset device info */
  for (i=0;i<MAXDEVICES;i++) {
    device[i].handle = NULL;
    device[i].inBufferSize = 0;
    device[i].outBufferSize = 0;
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
    //    device[i].poll_thread = NULL; // not necessary to initialize a thread
    device[i].poll_thread_exit_code = 1;
  }

  return result;
}

/* close hid connections */
void exit_usb ()
{
  int i;

  if (HID_DEBUG > 0) printf("HIDAPI_INTERFACE: Exiting HID driver\n");

  for (i=0;i<MAXDEVICES;i++) {
    if (device[i].handle != NULL) {

      if (USE_THREADS == 1) {
	/* stop read thread  */  
	device[i].poll_thread_exit_code = 1;
	pthread_join(device[i].poll_thread, NULL);
      }

      /* close hid device */
      hid_close(device[i].handle);
      device[i].handle = NULL;
      device[i].status = -1;
      if (device[i].inBuffer != NULL) {
	free(device[i].inBuffer);
	device[i].inBuffer = NULL;
      }
      if (device[i].outBuffer != NULL) {
	free(device[i].outBuffer);
	device[i].outBuffer = NULL;
      }
      
      if (device[i].readBuffer != NULL) {
	free(device[i].readBuffer);
	device[i].readBuffer = NULL;
      }
      if (device[i].writeBuffer != NULL) {
	free(device[i].writeBuffer);
	device[i].writeBuffer = NULL;
      }
	
    }
  }
}

#endif


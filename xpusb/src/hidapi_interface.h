/* standard headers */
#include <stdint.h>

/* posix tread headers */
#include <assert.h>
#include <pthread.h>

/* hidapi header */
#include <hidapi.h>

/* hid define statements */
# define MAXDEVICES 10
# define READBUFFERSIZE 1024*16
# define WRITEBUFFERSIZE 1024*16
# define HID_DEBUG 1 /* hid debugging option */
# define USE_THREADS 1 /* use threadded read */
# define USE_BLOCKING 1 /* use blocking read with timeout or not */

/* hid device parameters */
typedef struct {
  hid_device *handle;                   /* device handle */
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
  pthread_t poll_thread;                /* read thread */
  int poll_thread_exit_code;            /* read thread exit code */
} device_struct;

device_struct device[MAXDEVICES];

/* prototype functions */
int init_usb(void);
void exit_usb(void);
int setbuffer_usb(int number, int bufferSize);
int check_usb(int number, uint16_t vendor, uint16_t product, uint8_t bus, uint8_t address, const char *path);
int write_usb(int number, unsigned char *bytes, int size);
int read_usb(int number, unsigned char *bytes, int size);

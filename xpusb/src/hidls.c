#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HIDAPI
#include "hidapi.h"
#endif

int main(int argc, char* argv[])
{
  
#ifdef HIDAPI
  // Enumerate and print the HID devices on the system
  struct hid_device_info *devs, *cur_dev;
  
  devs = hid_enumerate(0x0, 0x0);
  cur_dev = devs;	
  while (cur_dev) {
    printf("Device Found\n");
    printf("  Vendor id:     %04hx\n",cur_dev->vendor_id);
    printf("  Prouct id:     %04hx\n",cur_dev->product_id);
    printf("  Device path:   %s\n",cur_dev->path);
    printf("  Serial_number: %ls\n",cur_dev->serial_number);
    printf("  Manufacturer:  %ls\n", cur_dev->manufacturer_string);
    printf("  Product:       %ls\n", cur_dev->product_string);
    printf("\n");
    cur_dev = cur_dev->next;
  }
  hid_free_enumeration(devs);

#else
  printf("HIDAPI not compiled (please make sure HIDAPI is defined in config.h to use this code)\n");
#endif

  return 0;

}

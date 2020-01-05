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

/*

#define MAX_STR 255

int main(int argc, char* argv[])
{
	int res;
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	const char path[] = "/dev/hidraw10";
	
	// Initialize the hidapi library
	res = hid_init();

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	//	handle = hid_open(0x4d9, 0xa131, L"", path);
	// handle = hid_open(0x0, 0x0, L"");
	handle = hid_open_path(path);
	
	// Read the Manufacturer String
	wstr[0] = 0x0000;
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if (res < 0) printf("Unable to read manufacturer string\n");
	printf("Manufacturer String: %ls\n", wstr);

	// Read the Product String
	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	if (res < 0)	printf("Unable to read product string\n");
	printf("Product String: %ls\n", wstr);

	// Read the Serial Number String
	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	if (res < 0) printf("Unable to read serial number string\n");
	printf("Serial Number String: (%d) %ls\n", wstr[0], wstr);

	// Finalize the hidapi library
	res = hid_exit();

	return 0;
}

*/

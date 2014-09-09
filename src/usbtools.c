/*
 * usbtools.c
 *
 *  Created on: 06.09.2014
 *      Author: veni
 */

#include "usbtools.h"
#ifdef USE_GANDALF
#include "gandalf.h"

//extern void *handle;
extern GANDALFconfig cfg;
#endif

int is_usbdevblock( libusb_device *dev,int vid,int pid )
{
	struct libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor( dev, &desc );

	if( desc.idVendor == vid && desc.idProduct == pid){
		return 1;
	}

	return 0;
}

uint32_t array2int(char *a ) {
	uint32_t b=0;
	b = a[0]<<24 | a[1]<<16 | a[2]<<8 | a[3];
	return b;
}



int int2array(int i,char a[4]) {
  a[0] =  i>>24 & 0xFF;
  a[1] =  i>>16 & 0xFF;
  a[2] =  i>>8  & 0xFF;
  a[3] =  i     & 0xFF;
  return i;
}

#ifdef USE_GANDALF

int writeUSB(int addr,int val) {
  char a[4];
  int timeout = 500; //ms
  int bmRequestType = LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_DEVICE | LIBUSB_REQUEST_TYPE_VENDOR;
  int bRequest = 0xC0;
  int wValue = addr;
  int wIndex = 0;
  int2array(val,a);
  printf("Writing to addr: %08X value %08X\n",addr,val);
  libusb_control_transfer(cfg.handle,bmRequestType,bRequest, wValue, wIndex, a, 4, timeout);
  val = array2int(a);
  printf("Written to addr: %08X value %08X\n",addr,val);
  return val;
}

unsigned int readUSB(int addr) {
  char a[4];
  int timeout = 500; //ms
  int bmRequestType = LIBUSB_ENDPOINT_IN | LIBUSB_RECIPIENT_DEVICE | LIBUSB_REQUEST_TYPE_VENDOR;
  int bRequest;
  if(addr<0x1000) {
    bRequest = 0x40;  // destination: cpld config block
  } else {
    bRequest = 0xC0;  // destination: protocol block (cpld <-> fpga interface)
  }
  int wValue = addr;
  int wIndex = 0;
  int val=0;
  // printf("bmRequestType: \t 0x%2X\n" , bmRequestType);
  // printf("bRequest: \t 0x%02X\n" , bRequest);

  libusb_control_transfer(cfg.handle,bmRequestType,bRequest, wValue, wIndex, a, 4, timeout);
  val = array2int(a);
  printf("Adress: 0x%02X: \t  0x%8X\n",wValue,val);

  return val;
}


int sendControlCommand(int addr){
  //All the control commands are sent to the cpld
  //Only the address is needed
  int timeout = 500; //ms
  int rcpt;
  if(addr == 0) {
    rcpt =LIBUSB_RECIPIENT_ENDPOINT;
  } else {
    rcpt = LIBUSB_RECIPIENT_DEVICE;
  }
  int bmRequestType = LIBUSB_ENDPOINT_OUT | rcpt | LIBUSB_REQUEST_TYPE_VENDOR;
  int bRequest = 0x40; // destination: cpld config block
  int wValue = addr;
  int wIndex = 0;
  //  char
  libusb_control_transfer(cfg.handle,bmRequestType,bRequest, wValue, wIndex, 0, 0, timeout);
  return 0;
}
#endif

int listUSBDevices(){
	  libusb_device **list;
	  libusb_context *ctx = NULL;

	  libusb_init(&ctx);
	  libusb_set_debug(ctx,3);
	  ssize_t cnt = libusb_get_device_list(ctx, &list);
	  int i,j,k;
	  int err = 0;
	  if (cnt < 0){
	    printf( "no usb devices found\n" );
	    error();
	  }

	  for(i = 0; i < cnt; i++){
	    libusb_device *device = list[i];
	    struct libusb_device_descriptor desc;
	    int r = libusb_get_device_descriptor( device, &desc );
	    printf(" desc.idVendor: %04X  , desc.idProduct: %04X\n", desc.idVendor, desc.idProduct);
	  }


	  libusb_free_device_list(list, 1);
	  libusb_exit( ctx );


	return 0;
}



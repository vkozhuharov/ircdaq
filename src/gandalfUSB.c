/*
 * gandalfUSB.c
 *
 *  Created on: 06.09.2014
 *      Author: veni
 */

#include "usbtools.h"
#include "gandalf.h"

int GANDALFCleanConfig(GANDALFconfig *cfg){
	cfg->dev = NULL;
	cfg->ctx = NULL;
	cfg->handle = NULL;
	cfg->attached = 0;
	return 0;
}


libusb_device * findGANDALF(GANDALFconfig *cfg) {
	int i;
	ssize_t cnt = libusb_get_device_list(cfg->ctx, &cfg->list);
	if (cnt < 0){
	    printf( "no usb devices found\n" );
	    error();
	}
	  // find our device
    for(i = 0; i < cnt; i++){
	   libusb_device *device = cfg->list[i];
	    if( is_usbdevblock(cfg->list[i],GANDALF_VID,GANDALF_PID) ){
	      cfg->dev = cfg->list[i];
	      break;
	    }
	  }
    return cfg->dev;

	return NULL;
}

//Initialize the board and check communication
int GANDALFInit(GANDALFconfig *cfg) {
	listUSBDevices();

	libusb_init(&cfg->ctx);
	libusb_set_debug(cfg->ctx,3);

	if (findGANDALF(cfg) == NULL) {
		printf("GANDALF not found\n");
		return 0;
	}

	//From here on GANDALF is OK, start playing with it
    int err = libusb_open(cfg->dev, &cfg->handle);
    if (err){
      printf("Unable to open usb device\n");
      error();
    }
    printf("GANDALF opened successfully\n");

    //Try to detach the device before opening it
    if ( libusb_kernel_driver_active(cfg->handle,0) ){
      printf("Device busy...detaching...\n");
      libusb_detach_kernel_driver(cfg->handle,0);
      cfg->attached = 1;
    }else printf("Device free from kernel\n");

    //Let's see if we can finally claim the device for us
    err = libusb_claim_interface( cfg->handle, 0 );
    if (err){
      printf( "Failed to claim interface. " );
      switch( err ){
      case LIBUSB_ERROR_NOT_FOUND:	printf( "not found\n" );	break;
      case LIBUSB_ERROR_BUSY:		printf( "busy\n" );		break;
      case LIBUSB_ERROR_NO_DEVICE:	printf( "no device\n" );	break;
      default:			printf( "other\n" );		break;
      }
      error();
    }

	return 1;
}



int GANDALFFirmwareOK(GANDALFconfig *cfg){
	int val = readUSB(BOARDSTATUS);
	cfg->sn = val & 0x3FF;
	cfg->hex = (val>>20) & 0xFF;
	cfg->ga = (val>>12) & 0x1F;
	cfg->init = (val>>28) & 0xF;
	printf("  SN  HEX  GA  INIT\n");
	printf("-------------------\n");
	printf( "%4d  %02Xh  %2d  %x\n",
			cfg->sn, cfg->hex, cfg->ga, cfg->init);
	if(cfg->init == FIRMWARE_OK)
		return 1;

	return 0;
}

int GANDALFConfigInterfaces(GANDALFconfig *cfg) {
    libusb_get_configuration(cfg->handle,&cfg->cfg);
    printf("Configuration value %d, %08X\n",cfg->cfg,cfg->cfg);



	return 0;
}


//Confugure the digitizer
int GANDALFConfig(GANDALFconfig *cfg) {
	if(! GANDALFFirmwareOK(cfg) ) {
		return 0;
	}
	if(!GANDALFConfigInterfaces(cfg) ) {
		return 0;
	}


	return 0;
}



//Free all the configuration
int GANDALFExit(GANDALFconfig *cfg){
    //if we detached kernel driver, reattach.
    if( cfg->attached == 1 ){
      libusb_attach_kernel_driver( cfg->handle, 0 );
    }
    libusb_close( cfg->handle );

	if(cfg->list)
		libusb_free_device_list( cfg->list, 1);
	if(cfg->ctx )
		libusb_exit( cfg->ctx );

	return 0;
}

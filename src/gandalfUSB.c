/*
 * gandalfUSB.c
 *
 *  Created on: 06.09.2014
 *      Author: veni
 */

#ifdef USE_GANDALF
#include "usbtools.h"
#include "gandalf.h"


int GANDALFCleanConfig(GANDALFconfig *cfg){
	cfg->dev = NULL;
	cfg->ctx = NULL;
	cfg->handle = NULL;
	cfg->attached = 0;

	//Endpoints cleaning
	cfg->desc_out = NULL;
	cfg->desc_in  = NULL;
	cfg->timeout = 100; //ms
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

	const struct libusb_endpoint_descriptor *epdesc;
	const struct libusb_interface_descriptor *interdesc;
	const struct libusb_interface *inter;

	int i,j,k;

	libusb_get_configuration(cfg->handle,&cfg->cfg);
	printf("Configuration value %d, %08X\n",cfg->cfg,cfg->cfg);

	libusb_get_config_descriptor(cfg->dev, 0, &cfg->config);
	printf("Number of interfaces: %d\n",cfg->config->bNumInterfaces);
	for(i=0; i<(int)cfg->config->bNumInterfaces; i++) {
		inter = &cfg->config->interface[i];
		printf("Number of alternate settings: %d \n", inter->num_altsetting);
		for(j=0; j<inter->num_altsetting; j++) {
			interdesc = &inter->altsetting[j];
			printf("Interface Number: %d\n",(int)interdesc->bInterfaceNumber);
			printf("Number of endpoints: %d\n",(int)interdesc->bNumEndpoints);
			for(k=0; k<(int)interdesc->bNumEndpoints; k++) {
				epdesc = &interdesc->endpoint[k];
				printf("Descriptor Type: %d\n",(int)epdesc->bDescriptorType);
				printf("EP Address: %X\n",(int)epdesc->bEndpointAddress);
				if( (epdesc->bEndpointAddress == 2) && ((epdesc->bEndpointAddress & 0x80) == LIBUSB_ENDPOINT_OUT )  ) {
					printf("Found output endpoint descriptor at address 2\n");
					//This is an
					cfg->desc_out = epdesc;
				}
				if( ((epdesc->bEndpointAddress & 0x80) == LIBUSB_ENDPOINT_OUT )  ) {
					printf("Found output endpoint descriptor at address %d\n",epdesc->bEndpointAddress);
				}
				if( ((epdesc->bEndpointAddress & 0x80) == LIBUSB_ENDPOINT_IN )  ) {
					printf("Found input endpoint descriptor at address %d\n",epdesc->bEndpointAddress);
				}

				if(((epdesc->bEndpointAddress & 0xF) == 6) && ((epdesc->bEndpointAddress & 0x80) ==LIBUSB_ENDPOINT_IN)  ) {
					printf("Found input endpoint descriptor at address 6\n");
					//This is an
					cfg->desc_in = epdesc;
				}
			}
		}
	}

	if (cfg->desc_out == NULL) {
		printf("Output endpoint not found\n");
		return 0;
	}
	if (cfg->desc_in == NULL) {
		printf("Input endpoint not found\n");
		return 0;
	}
	printf("Endpoints for data transfer identified successfully\n");
	return 1;
}


int GANDALFConfigIdentity(GANDALFconfig *cfg) {
	int val;
	//Board identity:
	// IDENTITY 1 ( 9 dnto 0)  Defines the COMPASS experiment source ID
	// for the GANDALF module.

	// IDENTITY 1 ( 19 dnto 10)  If second chip definitions like chipF1,
	// chip Scaler etc are implemented this defines the COMPASS experiment
	// source ID of the second implementation.

	// IDENTITY 1 ( 29 dnto 20) If data in SMUX format is produced by the
	// GANDALF module this defines its source ID.
	val = 0x00000324;
	val =  writeUSB( CMR + BOFFSET + IDENTITY1 , val);
	printf("Identity0 content - U card: 0x%08x\n",val = readUSB(CMR + MUOFFSET + IDENTITY0));


	printf("Identity0 content - D card: 0x%08x\n",readUSB(CMR + MDOFFSET + IDENTITY0));


	return val;
}


int GANDALFConfigBaseline(GANDALFconfig *cfg) {
	int val;

	if(1){
		val=0;
		//      val = (0x1<<30) | (0x1 <<14) ;
		writeUSB(CMR + MUOFFSET + BASL_SET,val); //
		writeUSB(CMR + MUOFFSET + BASL_SET+4,val); //
		writeUSB(CMR + MUOFFSET + BASL_SET+8,val); //
		writeUSB(CMR + MUOFFSET + BASL_SET+12,val); //

	}
	if(1){

		writeUSB(CMR + MDOFFSET + BASL_SET,val); // */
		writeUSB(CMR + MDOFFSET + BASL_SET+4,val); // */
		writeUSB(CMR + MDOFFSET + BASL_SET+8,val); // */
		writeUSB(CMR + MDOFFSET + BASL_SET+12,val); // */
	}


	return val;
}

int GANDALFConfigDAC(GANDALFconfig *cfg) {
	int val;
    val = 0x04ff;

    //Up mezzanine
    writeUSB(CMR + MUOFFSET + DAC_VAL,val); //
    writeUSB(CMR + MUOFFSET + DAC_VAL + 4,val); //
    writeUSB(CMR + MUOFFSET + DAC_VAL + 8,val); //
    writeUSB(CMR + MUOFFSET + DAC_VAL + 12,val); //
    //Down mezzanine
     writeUSB(CMR + MDOFFSET + DAC_VAL,val); //
     writeUSB(CMR + MDOFFSET + DAC_VAL + 4,val); //
     writeUSB(CMR + MDOFFSET + DAC_VAL + 8,val); //
     writeUSB(CMR + MDOFFSET + DAC_VAL + 12,val); //




	return val;
}

void GANDALFPrintConfig(GANDALFconfig *cfg) {
    readUSB(0x6B00);
    readUSB(0x6B14);
    readUSB(0x6804);
    readUSB(0x6B18);
    readUSB(0x6B20);

}

//Configure the digitizer
int GANDALFConfig(GANDALFconfig *cfg) {
	int val;
	if(! GANDALFFirmwareOK(cfg) ) {
		return 0;
	}
	if(!GANDALFConfigInterfaces(cfg) ) {
		return 0;
	}
	GANDALFConfigIdentity(cfg);


	writeUSB(FRC + DAC_CALIB, 0x1);
	//  No idea    - General configuration 6
	val = 0x00000150;
	writeUSB(0x2b18, val);

	GANDALFConfigBaseline(cfg);

	GANDALFConfigDAC(cfg);

    writeUSB(FRC + 0x02C ,0x2);

    usleep(100000);

    //Window size;
    val  = 0x2  << 0;   //2
    val |= 0x20 << 16; //148 window size
    //      val = 0x00500002;
    //      writeUSB(CMR + GEN_CONF0, val);
    writeUSB(0x2B00, val);

    usleep(100000);

    GANDALFPrintConfig(cfg);


	return 1;
}

//Ask GANDALF for data and get what is returned
int GANDALFGetDATA(GANDALFconfig *cfg,char *data, int maxsize){
	int transferred=0;
	int res = 0;
    res = libusb_bulk_transfer(cfg->handle,cfg->desc_in->bEndpointAddress,
    		data, maxsize, &transferred, cfg->timeout );

//    The timeout value is given in milliseconds.  Returns 0 on suc-
//         cess, LIBUSB_ERROR_TIMEOUT	if the transfer	timed out, LIBUSB_ERROR_PIPE
//         if	the control request was	not supported, LIBUSB_ERROR_OVERFLOW if	the
//         device offered more data, LIBUSB_ERROR_NO_DEVICE if the device has	been
//         disconnected and a	LIBUSB_ERROR code on other failure.
    // printf("Bulk transfer result: %d\n",res);
    switch (res) {
    case 0:
    	break;
    case LIBUSB_ERROR_TIMEOUT:
    	//printf("Transfer timed out\n");
    	break;
    case LIBUSB_ERROR_PIPE:
    	printf("LIBUSB_ERROR_PIPE \n");
    	break;
    case LIBUSB_ERROR_OVERFLOW:
    	printf("LIBUSB_ERROR_OVERFLOW \n");
    	break;
    case LIBUSB_ERROR_NO_DEVICE:
    	printf("LIBUSB_ERROR_NO_DEVICE \n");
    	break;
//    case LIBUSB_ERROR:
//    	printf("LIBUSB_ERROR \n");
//    	break;

    }

	return transferred;
}

int GANDALFStartRun(GANDALFconfig *cfg){
	int val;
	val = 1;

    sendControlCommand(0x08);
    usleep(100000);

    writeUSB(0x7034, 2); //load conf
    usleep(100000);

    writeUSB(0x7044, 2); //BOS
   // usleep(100000);


    return 0;
}

int GANDALFEndRun(GANDALFconfig *cfg){
	int val;
	val = 1;

	//Send EOB
	writeUSB(0x7048, 2);
	usleep(1000000);
	sendControlCommand(0x08);
	usleep(1000000);

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
#endif

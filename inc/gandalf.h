/*
 * gandalf.h
 *
 *  Created on: 06.09.2014
 *      Author: veni
 */
#include <libusb-1.0/libusb.h>
#include <sys/types.h>

#ifndef GANDALF_H_
#define GANDALF_H_

#define GANDALF_VID	        0x04b4
#define GANDALF_PID			0x1002

//GANDALF addresses definition:
#define BOARDSTATUS  0x00fc
#define R_SPY_FIFO   0x3000
#define ARMBROADCAST 0x0010
#define BC_FPGA_CFG  0x8000
#define BC_SWITCH    0x0014
#define DISPLAY_W    0x0004
#define CMR          0x2000  //Configuration memory registers offset
#define FRC          0x7000  //Fast register commands offset


//GANDALF configuration
#define FIRMWARE_OK  			0xf
#define FIRMWARE_NOT_PRESENT  	0xe



//Fast Register Commands.
#define DAC_CALIB       0x00c //Values: 0x0, 0x1
#define TRG_TEMP_RDOUT  0x010

#define EXT_BOS 		0x044


//Configuration Memory Registers
#define BOFFSET        0x800
#define MUOFFSET       0x000
#define MDOFFSET       0x400

#define IDENTITY0     0x000
#define IDENTITY1     0x004
#define STATUS0       0x008
#define STATUS1       0x00c
#define PROD_DATE     0x010
#define DAC_VAL       0x0c0
#define BASL_SET      0x0d0

#define GEN_CONF0       0xb00
#define GEN_CONF1       0xb04
#define GEN_CONF2       0xb08





//Main structure for GANDALF configuration and communication
typedef struct GANDALFconfig {
	libusb_device **list;
	libusb_device *dev;
	libusb_context *ctx;
	libusb_device_handle *handle;
    const struct libusb_endpoint_descriptor *desc_in;
    const struct libusb_endpoint_descriptor *desc_out;
    struct libusb_config_descriptor *config;

	short int attached;
	short int sn;
	short int hex;
	short int ga;
	short int init;
	short int cfg;



} GANDALFconfig;



#endif /* GANDALF_H_ */

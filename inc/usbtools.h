/*
 * usbtools.h
 *
 *  Created on: 06.09.2014
 *      Author: veni
 */

#ifndef USBTOOLS_H_
#define USBTOOLS_H_
#include "globals.h"

#include <libusb-1.0/libusb.h>
#include <stdio.h>

int is_usbdevblock( libusb_device *dev,int,int );
uint32_t array2int(char *a);
int int2array(int i,char a[4]);

int writeUSB(int addr,int val);
unsigned int readUSB(int addr);
int listUSBDevices();



#endif /* USBTOOLS_H_ */

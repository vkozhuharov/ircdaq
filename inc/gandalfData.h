/*
 * gandalfData.h
 *
 *  Created on: 09.09.2014
 *      Author: veni
 */

#ifndef GANDALFDATA_H_
#define GANDALFDATA_H_
#include "globals.h"


#define GAN_EV_START 0x0
#define GAN_EOV_CHECK 0xcfed120

#define GAN_HDR_EVN    0x3f000000
#define GAN_HDR_CHID   0x00f00000
#define GAN_HDR_SYSMON 0x000f8000
#define GAN_HDR_FSIZE  0x00007ff0
#define GAN_HDR_RDM    0x0000000f

#define OFF_GAN_HDR_EVN     24
#define OFF_GAN_HDR_CHID    20
#define OFF_GAN_HDR_SYSMON  15
#define OFF_GAN_HDR_FSIZE   4
#define OFF_GAN_HDR_RDM     0


#define GAN_SLINK_ERR      0x80000000
#define GAN_SLINK_EVSIZE   0x0000ffff
#define GAN_SLINK_SID      0x03ff0000
#define GAN_SLINK_EVTYPE   0x7c000000

#define OFF_GAN_SLINK_ERR      31
#define OFF_GAN_SLINK_EVSIZE   0
#define OFF_GAN_SLINK_SID      16
#define OFF_GAN_SLINK_EVTYPE   26

//#define GAN_SLINK_
//#define GAN_SLINK_
//#define GAN_SLINK_
//#define GAN_SLINK_
//#define GAN_SLINK_
//

#define GAN_CH_HDR_EVN      0x3f000000
#define GAN_CH_HDR_CHID     0x00f00000
#define GAN_CH_HDR_SYSMON   0x000f8000
#define GAN_CH_HDR_FRSIZE   0x00007ff0
#define GAN_CH_HDR_RDM      0x0000000f

#define OFF_GAN_CH_HDR_EVN      24
#define OFF_GAN_CH_HDR_CHID     20
#define OFF_GAN_CH_HDR_SYSMON   15
#define OFF_GAN_CH_HDR_FRSIZE   4
#define OFF_GAN_CH_HDR_RDM      0

#define GAN_CH_ADC1       0x00003fff
#define GAN_CH_ADC2       0x3fff0000
#define GAN_CH_ADC_DATA   0xc0000000

#define OFF_GAN_CH_ADC1       0
#define OFF_GAN_CH_ADC2       16
#define OFF_GAN_CH_ADC_DATA   30



//Gandalf structures
struct GAN_slink_begin {
	uint32_t ctrl:4;
	uint32_t tmp:28;
} __attribute__ ((__packed__));

struct GAN_slink_header {
	uint32_t evSize:16;
	uint32_t sID:10;
	uint32_t evType:5;
	uint32_t err:1;
	uint32_t evN:20;
	uint32_t spillN:11;
	uint32_t stat:1;
	uint32_t status:8;
	uint32_t tscErr:8;
	uint32_t errWds:8;
	uint32_t format:8;
}__attribute__ ((__packed__));

struct GAN_slink_end{
	uint32_t ctrl:4;
	uint32_t cfed:28;
} __attribute__ ((__packed__));


//All previous should be already aligned to 32 bits
struct GANRawEvent{
	struct GAN_slink_begin beg;
	struct GAN_slink_header slhdr;
	struct GAN_slink_end end;
	uint32_t data[MAX_EV_SIZE];
};





#endif /* GANDALFDATA_H_ */

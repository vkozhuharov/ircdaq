/*
 * DataStructures.h
 *
 *  Created on: 28.08.2014
 *      Author: veni
 */

#ifndef DATASTRUCTURES_H_
#define DATASTRUCTURES_H_
#include <sys/types.h>
#include <stdint.h>
#include "globals.h"

#define MAX_N_EVENTS 1

struct MEP_HDR {
	// Number of L0 triggers since start of burst
	uint32_t firstEventNum:24;
	uint8_t sourceID;
	uint16_t mepLength; //  Total length of the MEP in bytes including the header
	uint8_t eventCount;
	uint8_t sourceSubID;
} __attribute__ ((__packed__)) ;

struct dataProcessHeader{
	uint32_t evN:24;
	uint32_t source:8;
	uint32_t size:16; //in words!
	uint32_t type:16;
}__attribute__ ((__packed__)) ;


struct rawEvent{
	uint16_t length;
	uint8_t  evN;
	uint8_t flag;
	uint32_t timestamp;
	//uint32_t sample[8];
	uint32_t data[1000];
} __attribute__ ((__packed__)) ;


struct MEP {
	struct MEP_HDR hdr;
	uint8_t data[MAXDATA];
	//struct rawEvent evts[MAX_N_EVENTS];
};

#define IRC_EVENT_SIZE(a)  ((a) & 0xFFFF )
#define IRC_EVENT_TYPE(a)  (((a)>> 16)&0xF )

#define IRC_EVTYPE_STD       0x0
#define IRC_EVTYPE_SOB_TRIG  0x1
#define IRC_EVTYPE_EOB_TRIG  0x2
#define IRC_EVTYPE_SOB_SIG   0x4
#define IRC_EVTYPE_EOB_SIG   0x8


#define L0_SOB_TRIG   8   // 0100
#define L0_EOB_TRIG   12  // 1100
#define L0_SOB_SIG    3   // 0011
#define L0_EOB_SIG    2   // 0010
#define L0_TRIG       0   // Normal trigger for the moment .... (should be different)

#endif /* DATASTRUCTURES_H_ */

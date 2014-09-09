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



struct rawEvent{
	uint16_t length;
	uint8_t  evN;
	uint8_t flag;
	uint32_t timestamp;
	uint32_t sample[8];
} __attribute__ ((__packed__)) ;

struct MEP {
	struct MEP_HDR hdr;
	struct rawEvent evts[MAX_N_EVENTS];
};



#endif /* DATASTRUCTURES_H_ */

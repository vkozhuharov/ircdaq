#include "gandalfData.h"
#include "DataStructures.h"
#include <stdio.h>
#include <string.h>

int convertGanEvent(struct GANRawEvent *ganEvt,void  *ev){
	struct rawEvent * evt = (struct rawEvent *)ev;
	evt->evN  = (ganEvt->slhdr.evN & 0xFF);
	if( ganEvt->slhdr.evType == 2 ) {
		evt->flag = 0x80;
	}else {
		evt->flag = 0;
	}
	evt->timestamp = 0;
	memcpy(&(evt->data[0]), &(ganEvt->slhdr),4*( ganEvt->slhdr.evSize + 2));
	evt->length = 4*( ganEvt->slhdr.evSize +  // The real event
					2 +   // begin/end markers of the SLINK
					2 );  // mep event header
	return evt->length;
}

int prepareHeader(struct GANRawEvent *ganEvt,struct dataProcessHeader *hdr){
	hdr->source = DETECTOR_SOURCE_ID;
	hdr->evN    = ganEvt->slhdr.evN;
	switch(ganEvt->slhdr.evType){
	case L0_EOB_SIG:
		hdr->type = IRC_EVTYPE_EOB_SIG;
		break;
	case L0_TRIG:  //Normal triggers
		hdr->type = IRC_EVTYPE_STD;
		break;
	case L0_SOB_TRIG:
		hdr->type = IRC_EVTYPE_SOB_TRIG;
		break;
	case L0_EOB_TRIG:
		hdr->type = IRC_EVTYPE_EOB_TRIG;
		break;
	case L0_SOB_SIG:
		hdr->type = IRC_EVTYPE_SOB_SIG;
		break;
	default:
		hdr->type = IRC_EVTYPE_STD;
	}
	return 0;
}

int printGanSLHdr(struct GAN_slink_header *hdr) {
	printf("EvSize \t SourceID\t EvType \t ERR \n "
			"%4d \t %5d \t %5d %4d\n",hdr->evSize,hdr->sID,hdr->evType,hdr->err);
	return 0;

}

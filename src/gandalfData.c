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
	case 2:
		hdr->type = 2;
		break;
	case 0:
		hdr->type = 0;
		break;
	case 8:
		hdr->type = 1;
		break;
	case 12:
		hdr->type = 8;
		break;
	case 3:
		hdr->type = 4;
		break;
	default:
		hdr->type = 8;
	}
	return 0;
}

int printGanSLHdr(struct GAN_slink_header *hdr) {
	printf("EvSize \t SourceID\t EvType \t ERR \n "
			"%4d \t %5d \t %5d %4d\n",hdr->evSize,hdr->sID,hdr->evType,hdr->err);
	return 0;

}

#include "gandalfData.h"
#include "DataStructures.h"
#include <stdio.h>

int convertGanEvent(struct GANRawEvent *ganEvt,void *ev){
	struct rawEvent * evt = (struct rawEvent *)ev;


	return 0;
}

int printGanSLHdr(struct GAN_slink_header *hdr) {
	printf("EvSize \t SourceID\t EvType \t ERR \n "
			"%4d \t %5d \t %5d %4d\n",hdr->evSize,hdr->sID,hdr->evType,hdr->err);
	return 0;

}

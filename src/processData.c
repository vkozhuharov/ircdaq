#include <stdio.h>
#include <string.h>
#include "usbtools.h"
#include "DataStructures.h"
#include "gandalfData.h"


static struct rawEvent evt;
static struct GANRawEvent ganEvt;

int processData(int fdin, int fdout) {
  int i=0;
  int evsize=0;
  char buf[128];

  uint32_t tmp;
  uint32_t word;

  //unsigned int word;

  printf("Data processing thread started, PID:  %d, PPID: %d \n",getpid(), getppid());

  //prctl(PR_SET_PDEATHSIG, SIGHUP);
  // prctl(PR_SET_PDEATHSIG, SIG_IGN);
  int res;
  
  //  while((res = read(fdin,buf,4)) > 0 ) {
  while((res = read(fdin, &tmp, 4 )) == 4 ) {
	  tmp = swap(tmp); printf("DATA:  %08x \n",tmp);
	  //wait for the beginning of some event.
	  if ( tmp != GAN_EV_START) continue;

	  printf("===========New event===============\n");
	  memcpy(&tmp,&ganEvt.beg,4);

	  //Now we have the start of the event here, let's get the rest
	  //The three header words of the SLINK header
	  read(fdin, &tmp, 4 );tmp=swap(tmp);printf("DATA:  %08x \n",tmp);
	  memcpy(&ganEvt.slhdr,&tmp,4);
	  read(fdin, &tmp, 4 );tmp=swap(tmp);printf("DATA:  %08x \n",tmp);
	  memcpy(((char *) &ganEvt.slhdr)  + 4 ,&tmp, 4);
	  read(fdin, &tmp, 4 );tmp=swap(tmp);printf("DATA:  %08x \n",tmp);
	  memcpy(((char *) &ganEvt.slhdr)  + 4 ,&tmp, 4);

	  printGanSLHdr(&ganEvt.slhdr);

	  printf("Received new event with size %d\n",ganEvt.slhdr.evSize);
	  if(ganEvt.slhdr.evSize > MAX_EV_SIZE) {
		  printf("*******ERROR*******: please increase the event size\n");
	  }

	  for(i=0;(i<ganEvt.slhdr.evSize-3) && (i<MAX_EV_SIZE);i++){
		  read(fdin, &tmp, 4 ); tmp=swap(tmp); printf("DATA:  %08x \n",tmp);
		  ganEvt.data[i] = tmp;
	  }
	  //Skip the rest of the event that we cannot put in the buffer
	  for(i=0;i<ganEvt.slhdr.evSize-3;i++){
		  read(fdin, &tmp, 4 );
	  }

	  read(fdin, &tmp, 4 ); tmp=swap(tmp); printf("DATA:  %08x \n",tmp);
	  memcpy(&ganEvt.end,&tmp,4);

	  if(ganEvt.end.cfed != GAN_EOV_CHECK) {
		  printf("Received corrupted event ..... SKIPPING\n");
		  continue;
	  }

	  printf("===========END of event=============\n");


	  //Prepare the event to be sent to the DataSending module

	 evsize = convertGanEvent(&ganEvt,&evt);

//	  word = swap(tmp);
	 // printf("DATA: %08x \t %08x \n",tmp, word);

	  //	  decodeGandalEvent();
	 //test of git

    //buf[0] = toupper(buf[0]);
    //write(fdout,buf,res);
    //write(fdout,&word,res);
	 write(fdout,&evt,evsize);

 }

  //Exit since the buffer is over
  printf("EOF received: terminate the processing ....");
  close(fdout);
  printf("DONE \n");

  return 0;
}

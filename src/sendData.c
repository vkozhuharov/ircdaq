#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "DataStructures.h"


static struct MEP mep;

int sendDataToPCFarm(void*,uint16_t);

//test of the GITHUB


// Send data to its final destination
// either disk or network ... depends on what we want to do with it
int sendData(int fdin) {
  int i=0;
  char buf[128];
  int res;
  int fout;
  char fname[256];

  printf("Data sending thread started, PID:  %d, PPID: %d \n",getpid(), getppid());


  sprintf(fname,"data/output");
  if( (fout = open(fname,O_CREAT |O_WRONLY,S_IRUSR|S_IWUSR)) == -1) {
    printf("Cannot open output file %s\n",fname);
    return 1;
  }
  
  printf("Size of MEP_HDR: %d\n",sizeof(struct MEP_HDR));

  int evSize = sizeof(struct rawEvent);
  printf("Size of Event: %d\n",evSize);
  mep.hdr.sourceID = 0x20;

  for(i = 0; i < MAX_N_EVENTS; i++){
	  mep.evts[i].length = evSize;
  }

  int nEvts = 1;
  mep.hdr.eventCount = nEvts;
  mep.hdr.mepLength = sizeof(struct MEP_HDR) + (mep.hdr.eventCount * sizeof(struct rawEvent));
  mep.hdr.firstEventNum = 0;


  while((res = read(fdin,buf,2)) > 0 ) {
	//putchar(buf[0]);
    write(fout,buf,res);

    if(1) {
    	sendDataToPCFarm(&mep,mep.hdr.mepLength);
    	mep.hdr.firstEventNum += 0;
    }

  }
  
  //Exit since the buffer is over
  printf("EOF received: terminate the output....");
  close(fout);
  printf("DONE \n");
  
  
  return 0;
}

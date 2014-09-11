#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "DataStructures.h"
#include <sys/time.h>
#include <time.h>


static struct MEP mep;
static struct dataProcessHeader hdr;
static struct rawEvent evt;

int sendDataToPCFarm(void*,uint16_t);

//test of the GITHUB


// Send data to its final destination
// either disk or network ... depends on what we want to do with it
int sendData(int fdin) {
	int i=0;
	char buf[128];
	int res;
	int fout = -1;
	char fname[256];
	uint32_t word;
	struct tm *tt_beg;
	time_t tim;

	printf("Data sending thread started, PID:  %d, PPID: %d \n",getpid(), getppid());


	/* sprintf(fname,"data/output"); */
	/* if( (fout = open(fname,O_CREAT |O_WRONLY,S_IRUSR|S_IWUSR)) == -1) { */
	/* 	printf("Cannot open output file %s\n",fname); */
	/* 	return 1; */
	/* } */

	printf("Size of MEP_HDR: %d\n",sizeof(struct MEP_HDR));

	//	int evSize = sizeof(struct rawEvent);
	//	printf("Size of Event: %d\n",evSize);
	mep.hdr.sourceID = 0x20;

	/* for(i = 0; i < MAX_N_EVENTS; i++){ */
	/* 	mep.evts[i].length = evSize; */
	/* } */

	/* //	int nEvts = 1; */
	/* mep.hdr.eventCount = nEvts; */
	/* mep.hdr.mepLength = sizeof(struct MEP_HDR) + (mep.hdr.eventCount * sizeof(struct rawEvent)); */
	/* mep.hdr.firstEventNum = 0; */

	//start receiving the data


	//  while((res = read(fdin,buf,4)) > 0 ) {
	// while((res = read(fdin,&word,4)) > 0 ) {
	//initialize
	//	nEvts = 0;
	int pos = 0;
	int first = 1;

	while((res = read(fdin,&hdr,8)) == 8 ) {
		//We have the header of the event
		printf("Received header for event with length %d\n",hdr.size);

		//Do we have to send something ?
		if( mep.hdr.eventCount == NEVENTS_TO_PCFARM || hdr.type == IRC_EVTYPE_EOB_SIG) {
		  //All events that had to be received were received so nothing else should be done!
			sendDataToPCFarm(&mep,mep.hdr.mepLength);
			first = 1;
			if(hdr.type == IRC_EVTYPE_EOB_SIG) {
			  printf("EOB signal received\n");
			  //Close the output dump file
			}
		}



		if(hdr.type == IRC_EVTYPE_SOB_TRIG ) {
		  //Open a new file to write the dump
		  tim = time(0);
		  tt_beg  = localtime(&tim);
		  sprintf(fname,"data/gandalf_rawdata_%d_%.2d_%.2d_%.2d_%.2d_%.2d",
			  tt_beg->tm_year+1900,tt_beg->tm_mon+1,tt_beg->tm_mday,
			  tt_beg->tm_hour,tt_beg->tm_min, tt_beg->tm_sec);

		  if(fout != -1) {
		    //The file that we were writing to was not closed ... no EOB received ...
		    close(fout);
		  }

		  //sprintf(fname,"data/output");
		  if( (fout = open(fname,O_CREAT |O_WRONLY,S_IRUSR|S_IWUSR)) == -1) {
		    printf("Cannot open output file %s\n",fname);
		    return 1;
		  }		  
		}
		
		//The next arriving event is the first event in the MEP
		if(first) {
			//Prepare the part of the MEP header
			mep.hdr.sourceID = 0x20;
			mep.hdr.firstEventNum = hdr.evN;
			mep.hdr.sourceSubID = 0;
			mep.hdr.mepLength = sizeof (struct MEP_HDR);
			mep.hdr.eventCount = 0;
			pos = 0;
			first = 0;
		}
		
		
		// Now get the event and put it in the MEP
		res = read(fdin,&mep.data[pos], hdr.size );
		//Check for the consistency of the received data:
		if(res != hdr.size) {
		  printf("***** ERROR: Not able to get the event ..., skipping\n");
		  continue;
		}

		if(hdr.type == IRC_EVTYPE_EOB_TRIG) {
		  printf("EOB trigger received .... waiting for EOB signal\n");
		}
		
		if(hdr.type == IRC_EVTYPE_SOB_SIG) {  
		  printf("Command trigger: skipping\n");
		  continue;
		}
		
		if(hdr.type == IRC_EVTYPE_EOB_SIG  ) {
		  printf("Start of burst command trigger: skipping\n");
		  continue;
		}

		//Write the event to disk for spying
		write(fout, &mep.data[pos], res);
		// Set that we have to go to next position
		pos += hdr.size;
		//update the MEP length
		mep.hdr.mepLength += hdr.size;
		mep.hdr.eventCount ++;
		
		
		//putchar(buf[0]);
		//  printf("DATA:  %08x \n", word);
		
	}




	//Exit since the buffer is over
	printf("EOF received: terminate the output....");
	close(fout);
	printf("DONE \n");


	return 0;
}

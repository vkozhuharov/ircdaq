#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <signal.h>
#include "globals.h"

#ifdef USE_GANDALF
#include "gandalf.h"
extern GANDALFconfig cfg;
#endif

//extern int *pstatus;


int getData( int fdout ) {

  int i=0;
  char buf[MAXDATA];
  int state;
  int nbytes; //

	struct tm *tt_beg;
	time_t tim1;
	time_t tim2;
	long unsigned int transferred = 0;


//  printf("getData: Status pointer: %x\n",pstatus);


  printf("Data retrieval thread started, PID:  %d, PPID: %d \n",getpid(), getppid());
  prctl(PR_SET_PDEATHSIG, SIGHUP);
  // prctl(PR_SET_PDEATHSIG, SIG_IGN);

  // while(i<10) {
  tim1 = time(0);
  tim2 = time(0);


  while(1) {
//	  state = *pstatus;
//	  //printf("State as seen from getData: %d\n",state);
//	  while(state == 2 || 1 ) {
//		  sprintf (buf,"Thread 1 alive: %d\n",i++);
//		  write(fdout,buf,strlen(buf));
//		  sleep(1);
//	  }
//	  if(state == 1)
//		  i = 0;
//	  sleep(1);
#ifdef USE_GANDALF

	  nbytes = GANDALFGetDATA(&cfg, buf, MAXDATA);
	  transferred+=nbytes;
//	  if (nbytes > 0)
//		  printf("Transferred %d bytes\n",nbytes);
	  //write(fdout,buf,nbytes);
	  tim2 = time(0);

	  if(tim2 - tim1 == 1) {
		  printf("Transfer speed: ~%lf MB/s\n",(double) transferred/1e6);
		  tim1 = time(0);transferred = 0;
	  }
//	  usleep(USBTRANSFER_TIMEOUT*1000*1);

#else
	  sprintf (buf,"Thread 1 alive: %d\n",i++);
	  write(fdout,buf,strlen(buf));
	  sleep(1);
#endif


  }
  printf("Closing the output buffer.... ");
  close(fdout);
  printf("DONE\n");
  return 0;
}

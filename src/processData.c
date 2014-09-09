#include <stdio.h>
#include <string.h>
#include "usbtools.h"

int processData(int fdin, int fdout) {
  int i=0;
  char buf[128];

  uint32_t tmp;
//  uint32_t word;

  unsigned int word;

  printf("Data processing thread started, PID:  %d, PPID: %d \n",getpid(), getppid());

  //prctl(PR_SET_PDEATHSIG, SIGHUP);
  // prctl(PR_SET_PDEATHSIG, SIG_IGN);
  int res;
  
  //  while((res = read(fdin,buf,4)) > 0 ) {
  while((res = read(fdin,&tmp, 4 )) == 4 ) {
	  //tmp = buf[0] << 24
	  //word = array2int((char *) &tmp);
	  //word = array2int(buf);

	  word = swap(tmp);
	  printf("DATA: %08x \t %08x \n",tmp, word);



    //buf[0] = toupper(buf[0]);
    write(fdout,buf,res);
  }

  //Exit since the buffer is over
  printf("EOF received: terminate the processing ....");
  close(fdout);
  printf("DONE \n");

  return 0;
}

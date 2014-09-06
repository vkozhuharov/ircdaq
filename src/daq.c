#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/prctl.h>
#include <signal.h>

#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <fcntl.h>              /* Obtain O_* constant definitions */

#include "daq.h"

int status;
int *pstatus;

int main(int argc, char *argv[]) {
  
  //Configuration part
  
  status = -1;
  pstatus = &status;
  

  // Starting the necessary data processing units

  int rawdata[2];

  printf("Creating INPUT data flow FIFO ..... ");

  if( pipe(rawdata) == -1) {
    //perror("pipe");
    printf("FAIL\n");
    exit(EXIT_FAILURE);
  }
  printf("DONE\n");

  pid_t pth1;
  pth1 = fork();
  
  //printf("FORK result: process 1 with ID: %d\n",pth1);

  if(pth1 == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);  
  }
  
  // printf("Process with PID %d working\n",getpid());
  // printf("Process with parrent PID %d working\n",getppid());
  if(pth1 == 0) {
    // We are int the child process now 
    // writing data to the pipe, close the unused read end:

    close(rawdata[0]);
    getData( rawdata[1] );
    
  }
  if(pth1 > 0 ) {
    // We are in the parent process
    //First close the unused write end of the pipe of th1:
    close(rawdata[1]);
    
    pid_t pth2;
    int readydata[2];

    printf("Creating OUTPUT data flow FIFO ..... ");
    if( pipe(readydata) == -1) {
      //perror("pipe");
      printf("FAIL\n");
      exit(EXIT_FAILURE);
    }
    printf("DONE\n");


    pth2 = fork();
    //  printf("FORK result: process 2 with ID: %d\n",pth1);

    if(pth2 == -1) {
      perror("pipe");
    }
    
    //   printf("Process with PID %d working\n",getpid());
    //  printf("Process with parrent PID %d working\n",getppid());
    
    if(pth2 == 0) {
      // We are in the child process now 
      close(readydata[0]);
      processData(rawdata[0],readydata[1]);
    }
    
    
    if(pth2 > 0 ) {
      // We are in the parent process
      //We do not need any FIFO fd here
      close(readydata[1]);
      close(rawdata[0]);

      pid_t pth3;
      
      pth3 = fork();
      //printf("FORK result: process 3 with ID: %d\n",pth1);

      if(pth3 == -1) {
	perror("pipe");
      }
      
      /* printf("Process with PID %d working\n",getpid()); */
      /* printf("Process with parrent PID %d working\n",getppid()); */
      
      if(pth3 == 0) {
	// We are in the child process now
	sendData(readydata[0]);
      }
      
      
      if(pth3 > 0 ) {
	// We are in the parent process
    	 close(readydata[0]);

    	 pid_t pth4;
    	 //Configuration thread
    	 pth4 = fork();
         if(pth4 == -1) {
        	 perror("thread4");
         }
         if(pth4 == 0) {
        	 runControl();
         }
         if(pth4 > 0 ) {
   	// We are in the parent process


	int status;
	pid_t p;
	//    wait(NULL);
	p = waitpid(pth3,&status,0); 
	printf("PID Status: %d, ERRNO: %d\n",WEXITSTATUS(status) , errno);
	printf("Child thread %d finished, better exit\n",pth3); 
         }
      }
    }
  }
  
  
}

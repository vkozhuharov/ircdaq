#ifndef _DAQ_H_
#define _DAQ_H_
#include "globals.h"


int getData(int fd);
int processData(int fdin,int fdout);
int sendData(int fdout);
int runcontrol();


#endif

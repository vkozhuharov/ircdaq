#include "daq.h"

#ifdef USE_GANDALF
#include "gandalf.h"

extern GANDALFconfig cfg;
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



//Pre-defined states:
// IDLE
// INIT
// RUNNING

//Status file:
//stat/INIT

char *finit = "stat/INIT";
char *fidle = "stat/IDLE";
char *frun = "stat/RUN";

//Actions are defined only in a given state!


int clearAll(){
	unlink(finit);
	unlink(fidle);
	unlink(frun);
	return 0;
}

int Initialize(char *pars){

	clearAll();

#ifdef USE_GANDALF
	//GANDALFInit(&cfg);
//	GANDALFConfig(&cfg);
#endif

	creat(finit,S_IRUSR|S_IWUSR);

	return 0;
}

int StartRun(char *pars){
	clearAll();

#ifdef USE_GANDALF
//	GANDALFStartRun(&cfg);
#endif

	creat(frun,S_IRUSR|S_IWUSR);
	return 0;
}

int EndRun(char *pars){
	clearAll();

#ifdef USE_GANDALF
//	GANDALFEndRun(&cfg);
#endif

	creat(fidle,S_IRUSR|S_IWUSR);

	return 0;
}

int ResetState(char *pars){
	clearAll();

	creat(fidle,S_IRUSR|S_IWUSR);
	return 0;

}

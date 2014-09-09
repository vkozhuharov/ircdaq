#include "daq.h"

#ifdef USE_GANDALF
#include "gandalf.h"

extern GANDALFconfig cfg;
#endif

int Initialize(char *pars){

#ifdef USE_GANDALF
	GANDALFInit(&cfg);
	GANDALFConfig(&cfg);
#endif

	return 0;
}

int StartRun(char *pars){
#ifdef USE_GANDALF
	GANDALFStartRun(&cfg);
#endif

	return 0;
}

int EndRun(char *pars){
#ifdef USE_GANDALF
	GANDALFEndRun(&cfg);
#endif


	return 0;
}

int ResetState(char *pars){

	return 0;
}

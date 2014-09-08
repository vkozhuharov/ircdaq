#include "daq.h"
#include "gandalf.h"

extern GANDALFconfig cfg;

int Initialize(char *pars){
	GANDALFInit(&cfg);
	GANDALFConfig(&cfg);

	return 0;
}

int StartRun(char *pars){
	GANDALFStartRun(&cfg);

	return 0;
}

int EndRun(char *pars){


	return 0;
}

int ResetState(char *pars){

	return 0;
}

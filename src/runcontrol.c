#include "runcontrol.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/prctl.h>
#include <time.h>
#include <signal.h>


#define STRING_MAX_LENGTH 128
#define CONFIG_MAX_LENGTH 128


//Define the state values:
#define STATE_ERROR		-1
#define STATE_IDLE		0
#define STATE_INIT		1
#define STATE_READY		2


extern int *pstatus;
static int state;

//This function has to be rewritten - not safe!
int tokenize(char tok[][STRING_MAX_LENGTH], char*s, const char *delim){
	char *t;
	int i=0;
	t = strtok(s, delim);
	while(t != NULL){
		strcpy(tok[i], t);
		t = strtok(NULL, delim);
		i++;
	}
	return i;
}


#ifdef USE_DIM
//DIM server part of the run control

static int idState;
static int idInfo;
static int idLogging;
static int idConfig;
static int idVersion;

static char Info[STRING_MAX_LENGTH];		/*!< Buffer for dimServerName/Info */
static char Logging[STRING_MAX_LENGTH];	/*!< Buffer for dimServerName/Logging */
static char Config[CONFIG_MAX_LENGTH];	    /*!< Buffer for dimServerName/Config */


int sendInfo(char *msg){
	time_t tt;
	time(&tt);

	sprintf(Info,"NA62IRC: %s %s\n",ctime(&tt),msg);

	dis_update_service(idInfo);
	printf("INFO: %s\n",Info);
	return 0;
}


//Choosing the proper action
//Updating the state based on the result
int SelectCommand(char commandName[STRING_MAX_LENGTH], char tok[5][STRING_MAX_LENGTH]){
	int res;
	if(strcmp(commandName, "initialize")==0) {
		res=Initialize(tok);
		if(res == 0) {
			state = STATE_INIT;
		} else {
			state = STATE_ERROR;
		}
	} else if(strcmp(commandName, "startrun")==0) {
		if(state == STATE_INIT) {
			res=StartRun(tok);
			if(res == 0) {
				state = STATE_READY;
			} else {
				state = STATE_ERROR;
			}
		} else {
			state = STATE_ERROR;
		}
	} else if(strcmp(commandName, "endrun")==0) {
		res=EndRun(tok);
		if(res == 0) {
			state = STATE_INIT;
		} else {
			state = STATE_ERROR;
		}
	} else if(strcmp(commandName, "resetstate")==0) {
		res=ResetState(tok);
		if(res == 0) {
			state = STATE_IDLE;
		} else {
			state = STATE_ERROR;
		}
	}
	else {
		printf("Unknown command ...... %s\n",commandName);
		printf("Going into error state\n");
		state = STATE_ERROR;
	}

	printf("Going to state: %d\n",state);
	*pstatus = state;
	dis_update_service(idState);
	return 0;
}




void dimCommand(long* tag, char* cmnd_buffer, int* size){
//	print("Command port receiving: ");
//	println(cmnd_buffer);
	printf("DIM command received:  %s\n",cmnd_buffer);

	char args[5][STRING_MAX_LENGTH];

	if(tokenize(args, cmnd_buffer, " ")>0){
		SelectCommand(args[0], args);
	}
}


void dimFileContent(long* tag, char* cmnd_buffer, int* size){
//	print("Command port receiving: ");
//	println(cmnd_buffer);
	printf("DIM config file path received:  %s\n",cmnd_buffer);

}

void dimRequestConfig(long* tag, char* cmnd_buffer, int* size){
//	print("Command port receiving: ");
//	println(cmnd_buffer);

	printf("DIM config file requested: %s\n",cmnd_buffer);


}


int runControlDIM(){

	char hostname[64];
	char dimServerName[128];

	gethostname(hostname,64);
	printf("Running DIM server on host: %s\n",hostname);
	sprintf(dimServerName,"%s",hostname);
	printf("DIM server name: %s\n",dimServerName);


	//Provided DIM services ...
	char stateName[STRING_MAX_LENGTH];
	char infoName[STRING_MAX_LENGTH];
	char loggingName[STRING_MAX_LENGTH];
	char configName[CONFIG_MAX_LENGTH];
	char versionName[CONFIG_MAX_LENGTH];


	sprintf(stateName, "%s/%s", dimServerName, "State");
	sprintf(infoName, "%s/%s", dimServerName, "Info");
	sprintf(loggingName, "%s/%s", dimServerName, "Logging");
	sprintf(configName, "%s/%s", dimServerName, "Config");
	sprintf(versionName, "%s/%s", dimServerName, "NA62_VERSION");

	// ... and commands
	char commandName[CONFIG_MAX_LENGTH];
	char fileContentName[CONFIG_MAX_LENGTH];
	char requestConfigName[CONFIG_MAX_LENGTH];

	int idCommand;
	int idFile;
	int idRequest;

	sprintf(commandName, "%s/%s", dimServerName, "Command");
	sprintf(fileContentName, "%s/%s", dimServerName, "FileContent");
	sprintf(requestConfigName, "%s/%s", dimServerName, "RequestConfig");



	//Adding the services
	idState   = dis_add_service(stateName,"I",&state,sizeof(int),0,0);
	idInfo    = dis_add_service(infoName, "C", &Info, sizeof(Info), 0, 0);
	idLogging = dis_add_service(loggingName, "C", &Logging, sizeof(Logging), 0, 0);
	idConfig  = dis_add_service(configName, "C", &Config, sizeof(Config), 0, 0);
//	idVersion = dis_add_service(versionName, "I:3", &server_version, sizeof(server_version), 0, 0);

	//Adding the commands:
	dis_add_cmnd(commandName, "C", dimCommand, 0);
	dis_add_cmnd(fileContentName, "C", dimFileContent, 0);
	dis_add_cmnd(requestConfigName, "C", dimRequestConfig, 0);


	//Connecting them to DIM
	dis_start_serving(dimServerName);

	printf("runcontrol: Status pointer: %x\n",pstatus);

	state = 0;
	//*pstatus = state;

	char msg[256];
	while(1){
		//printf("Runcontrol alive .... in state %d, status: %d\n",state);
		sprintf(msg,"Runcontrol alive .... in state %d\n",state);
		sendInfo(msg);
		sleep(5);
	}
	return 0;
}

#else
int runControlDAQ(){
	return 0;
}
#endif


int runControl(){
	prctl(PR_SET_PDEATHSIG, SIGHUP);
	state = -1;
	*pstatus = state;
#ifdef USE_DIM
	printf("Using central DIM service\n");
	runControlDIM();
#else
	printf("Standalone run control");
	runControlDAQ();
#endif

	return 0;
}




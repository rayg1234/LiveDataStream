#include "NiDAQServer.h"
#include "CommandDef.h"
#include "NiDAQServer.h"

using namespace std;

int CMD_GETAIN_READING(Command cmd, SOCKET s,int NumParams,int NumOptions) {
	

	string channel = "";
	string numdata = "";
	int thechan = 0;
	int thenumdata = 0;
	string channelstr = "Dev1/ai";

	//printf("numparams %d\n",NumParams);

	if(NumParams == 2) {
		channel = cmd.params[0];
		numdata = cmd.params[1];
		thechan = atoi(channel.c_str());
		thenumdata = atoi(numdata.c_str());
	} else {
		return -1;
	} 


	if(thechan >= 0 && thechan <=8) {
		channelstr = channelstr + channel;
		//channelstr = channelstr.append(channel);
		//sprintf(newc,"%s",channelstr.c_str);
	} else {
		return -1;
	}

	
	//cout << channelstr << endl;

	int32       error=0;
	TaskHandle  taskHandle=0;
	int32       read;
	float64     data[1000];
	char        errBuff[2048]={'\0'};


	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,channelstr.c_str(),"",DAQmx_Val_Cfg_Default,-5.0,5.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",10000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,thenumdata));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
	DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,1000,10.0,DAQmx_Val_GroupByChannel,data,1000,&read,NULL));

	//cout <<sizeof(float);
	//cout <<sizeof(float64);

	SndData2(s,(char*)data,read*sizeof(float64));

	//printf("Acquired %d points from DAQ %s\n",read,channelstr.c_str());
	//printf("data1 is %f\n",data[1]);
	//printf("size of float is %d\n",sizeof(float64));


	Error:
		if( DAQmxFailed(error) )
			DAQmxGetExtendedErrorInfo(errBuff,2048);
		if( taskHandle!=0 )  {
			/*********************************************/
			// DAQmx Stop Code
			/*********************************************/
			DAQmxStopTask(taskHandle);
			DAQmxClearTask(taskHandle);
		}
		if( DAQmxFailed(error) )
			printf("DAQmx Error: %s\n",errBuff);
	
	return 0;
}

int CMD_GETAIN_READING_EXT(Command cmd, SOCKET s,int NumParams,int NumOptions) {
	

	string channel = "";
	string numdata = "";
	int thechan = 0;
	int thenumdata = 0;
	string channelstr = "Dev1/ai";

	//printf("numparams %d\n",NumParams);

	if(NumParams == 2) {
		channel = cmd.params[0];
		numdata = cmd.params[1];
		thechan = atoi(channel.c_str());
		thenumdata = atoi(numdata.c_str());
	} else {
		return -1;
	} 


	if(thechan >= 0 && thechan <=8) {
		channelstr = channelstr + channel;
		//channelstr = channelstr.append(channel);
		//sprintf(newc,"%s",channelstr.c_str);
	} else {
		return -1;
	}

	
	//cout << channelstr << endl;

	int32       error=0;
	TaskHandle  taskHandle=0;
	int32       read;
	float64     data[1000];
	char        errBuff[2048]={'\0'};


	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,channelstr.c_str(),"",DAQmx_Val_Cfg_Default,-5.0,5.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"/Dev1/PFI0",100.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,thenumdata));
	//DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",10000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,thenumdata));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandle));

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
	DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,-1,10.0,DAQmx_Val_GroupByChannel,data,thenumdata,&read,NULL));
	//DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,1000,10.0,DAQmx_Val_GroupByChannel,data,1000,&read,NULL));

	//cout <<sizeof(float);
	//cout <<sizeof(float64);

	SndData2(s,(char*)data,read*sizeof(float64));

	//printf("Acquired %d points from DAQ %s\n",read,channelstr.c_str());
	//printf("data1 is %f\n",data[1]);
	//printf("size of float is %d\n",sizeof(float64));


	Error:
		if( DAQmxFailed(error) )
			DAQmxGetExtendedErrorInfo(errBuff,2048);
		if( taskHandle!=0 )  {
			/*********************************************/
			// DAQmx Stop Code
			/*********************************************/
			DAQmxStopTask(taskHandle);
			DAQmxClearTask(taskHandle);
		}
		if( DAQmxFailed(error) )
			printf("DAQmx Error: %s\n",errBuff);
	
	return 0;
}


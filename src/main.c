/*
 ============================================================================
 Name        : main.c
 Author      : zhich
 Version     :
 Copyright   : Your copyright notice
 Description :
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "Global.h"
//#include "LinkedList.h"
#include "Network.h"
#include "GeneralAgent.h"
#include "GroupAgent.h"
#include "MajorAgent.h"





extern Status			status;
extern FlagsGroup 		Flags;
extern int				isCallInit;
extern int				isWakeup;
extern void 			Init();



void JudgeCharactor(int value)
{
	switch (value)
	{
		case 10:
			MajorAgent();
			break;
		case 11:
			GroupAgent();
			break;
		case 12:
			GeneralAgent();
			break;
	}
}


void DefineCharactor()
{

	int res;
	sResOutput(">>>		Main Thread Runing 	","    >>>");

	while(1)
	{
		usleep(SENDINTERVAL);
		
		res = strlen(status.Character);

		if (Flags.isReset)		Flags.isReset = 0;

		if (res == 0)			continue;

		if (isWakeup)			{ isWakeup = 0; Init();  isEndMission = 0; Flags.isStopMission = 0; Flags.isRestart = 0; Flags.isStartMission = 0; }

		if (Flags.RecvPcMode) 	{ Flags.GlobalPcMode = 1; memset(Flags.CurrentControler, 0 ,sizeof(Flags.CurrentControler)); strncpy(Flags.CurrentControler,MAJORPC,strlen(MAJORPC)); }

		if (Flags.RecvFreeMode) { Flags.GlobalPcMode = 1; Flags.GlobalFreeMode = 1; /*TO-DO 要找到主控agent的ip号*/} //后续的工作

		
		JudgeCharactor(status.CharacterID);//这里的参数要注意，初始化为0的情况。
		

		if(!isCallInit)	isCallInit = 1;	else Init();

		printf("\n");
		fflush(stdout);
		getLocalAddr();					//首先 获取 ip
		printf("\n>>>	Reset Success	>>>\n");
		fflush(stdout);

		break;


	}

}

char *CreatePreSentMsgJson()
{
	cJSON *root = NULL;
	char  *out  = NULL;

	root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "Type", "PreSentMsg");
	cJSON_AddStringToObject(root, "CmdID", getCommandID());
	cJSON_AddStringToObject(root, "Time", setLocalTime());
	cJSON_AddNumberToObject(root, "AgentID", status.AgentID);

	out = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	sResOutput("->->->StatusBuffer : ",out);
	return out;
}



int main(void) {

	sResOutput(">>>*		Program ","Startup		*>>>");
	pthread_t udp;
	int ret3;
	char *out;

	DestroyValue();				//清空全局变量
	Init();
	getLocalAddr();				//首先 获取 ip
	isCallInit = 1;



	pthread_mutex_init(&Flags.mutex,NULL);
	ret3 = pthread_create(&udp,NULL,(void*)RecvFromOthers,NULL);

//	ret3 = pthread_create(&udp,NULL,(void*)udp_with_web,NULL);


	if(ret3 != 0)
		printf(">>>	Create Pthread network Failed!\n");

	//////////////////////////////////////////////////////
	/*<!---------------Call Function Test--------------->*/


	/////////////////////////////////////////////////////

	while(1)
	{
		//发送ip给pc
		while(1)
		{
			sleep(2);

			out = CreatePreSentMsgJson();

			pthread_mutex_lock(&Flags.mutex);
			SendToOthers(MAJORPC,out);
			pthread_mutex_unlock(&Flags.mutex);

			free(out);
			out = NULL;
	//		printf("****^^^****\n********\n*******\n");
	//		fflush(stdout);
			if (Flags.isReset)	Flags.isReset = 0;
			if (Flags.isBreak)	break;
	//		printf("*****^****\n********\n*******\n");

		}

	//	sleep(1);
	//	printf("**@***\n****@****\n****@***\n");
	//	fflush(stdout);

		DefineCharactor();
	}

	pthread_mutex_destroy(&Flags.mutex);


	return 0;
}

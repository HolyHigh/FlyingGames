/*
 * Global.c
 *
 *  Created on: 2014年7月1日
 *      Author: zhich
 */


#include "Global.h"
//#include "Network.h"


int TimeAdjust = 0;
int year = 0, month = 0, day = 0, hour = 0, minute =0, second = 0;


void iNetOutput(char *string, int value)
{

	//RELEASE OUTPUT TEST
	#ifdef RELEASE
		printf("%s %d\n\n",string,value);
		fflush(stdout);
	#endif
	//RELEASE OUTPUT TEST
}

void iResOutput(char *string, int value)
{
	//DEBUG OUTPUT TEST
	#ifdef DEBUG
		printf("%s %d\n\n",string,value);
		fflush(stdout);
	#endif
	//DEBUG OUTPUT TEST
}

void sNetOutput(char *string, char *value)
{
	//RELEASE OUTPUT TEST
	#ifdef RELEASE
		printf("%s %s\n\n",string,value);
		fflush(stdout);
	#endif
	//RELEASE OUTPUT TEST

}

void sResOutput(char *string, char *value)
{
	//DEBUG OUTPUT TEST
	#ifdef DEBUG
		printf("%s %s\n\n",string,value);
		fflush(stdout);
	#endif
	//DEBUG OUTPUT TEST
}




//程序选择相应角色后，要对相应的全局变量做一下 初始化 的工作
void Init()
{
	//TO-DO
	status.Fule = 300;
	Flags.changeMissionf = 1;
	Flags.startMissionf = 1;
	memset(status.AliveStatus, 0 ,sizeof(status.AliveStatus));
	strncpy(status.AliveStatus, YES,strlen(YES));

}

void gAnswerJson()
{

}


char *setLocalTime()
{
	time_t time_input;
	struct tm *tmptime;
	static char buf[100];

	time(&time_input);
	tmptime = localtime(&time_input);
	sprintf(buf,"%04d-%02d-%02d %02d:%02d:%02d",
			tmptime -> tm_year+1900, tmptime -> tm_mon+1, tmptime ->tm_mday, tmptime -> tm_hour, tmptime -> tm_min, tmptime -> tm_sec);

	return buf;
}


char *getCommandID()
{
	static char buf[20];

	time_t time_input;
	time_input = time(&time_input);

	sprintf(buf,"%ld",time_input);

	return buf;


}


void DestroyValue()
{
	int ii;
	TeamCount	 = 0;
	tMemberCount = 0;
	AgentAnswer  = NULL;
	Flags.changeMissionf = 1;
	Flags.startMissionf = 1;

	memset(&status,0,sizeof(struct Status));
	memset(&Flags,0,sizeof(struct FlagsGroup));
//	memset(&ownformation,0,sizeof(struct ownFormation));
	memset(&mission,0,sizeof(struct Mission));

	for(ii = 0; ii < MAXARRAYCOUNT; ii++)
	{
		memset(&listItem[ii],0,sizeof(struct Formation));
		memset(&majorSaved[ii],0,sizeof(struct MajorSaved));
	}

}


void fun(char a[])
{
	 int i,length;

	 char t;

	 length = strlen(a);

	 for(i = 0; i < length/2; i++)
	 {
		 t = a[i];
		 a[i] = a[length-1-i];
		 a[length-1-i] = t;

	 }
}


void itoa (int n,char s[])
{
	int i,sign;

	if ( (sign=n) < 0 )
		n = -n;
	i = 0;

	do{
		s[i++] = n%10 + '0';
	}

	while ((n /= 10) > 0);

	if ( sign < 0 )
		s[i++] = '-';
	s[i] = '\0';

}



void IntToString(int value,char s[])
{

	memset(s,0,sizeof(s));

	itoa(value,s);

	fun(s);

}

void AdjustClockTime()
{
	struct tm mytm;
	time_t t;

	if (TimeAdjust)
		return;
	if (year==0)
		return;
	mytm.tm_year = year-1900;
	mytm.tm_mon = month-1;
	mytm.tm_mday = day;
	mytm.tm_hour = hour;
	mytm.tm_min = minute;
	mytm.tm_sec = second;
	printf("\n<<<<<<<********>>>>>>%04d-%02d-%02d %02d:%02d:%02d\n", mytm.tm_year + 1900,
		mytm.tm_mon + 1, mytm.tm_mday, mytm.tm_hour, mytm.tm_min, mytm.tm_sec);
	t = mktime(&mytm);
    stime(&t);
//    system("hwclock -w");
    TimeAdjust = 1;
}




//After finish called , remember free memery
char *CreateMultiplexJson(char *Type, char *cmdid)
{
	cJSON *root = NULL;
	char  *out  = NULL;

	root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "Type", Type);
	cJSON_AddStringToObject(root, "CmdID", cmdid);
	cJSON_AddStringToObject(root, "Time", setLocalTime());
	cJSON_AddStringToObject(root, "Broadcast", NONE);
	cJSON_AddStringToObject(root, "NeedAnswer", NO);
	cJSON_AddNumberToObject(root, "TeamID", status.TeamID);
	cJSON_AddNumberToObject(root, "AgentID", status.AgentID);
	cJSON_AddStringToObject(root, "Character", status.Character);
	cJSON_AddStringToObject(root, "TeamShape", status.TeamShape);
	cJSON_AddNumberToObject(root, "FuleStatus", status.Fule);
	cJSON_AddStringToObject(root, "AliveStatus", status.AliveStatus);



	out = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	sNetOutput("->->->StatusBuffer : ",out);
	return out;

}




int getLocalAddr()
{

    int inet_sock;
    struct ifreq ifr;
    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
    char addr[30];
    int ii;
    char *tmp;
    char ip[32];
    int  IP[4];

	for(ii = 0; ii < MAXCARD; ii++)
	{
		sprintf(addr, "%s%d", NETWORKCARD, ii);
		memset(ifr.ifr_name,0,sizeof(ifr.ifr_name));
		strcpy(ifr.ifr_name, addr);
	    if (ioctl(inet_sock, SIOCGIFADDR, &ifr) <  0)  perror("ioctl");
	    else 	break;

	}

    //SIOCGIFADDR标志代表获取接口地址

    sResOutput(">>> LocalIpAddr : ",inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr));

    //取出ip最后一个字段的值，并转换为int
    memset(ip,0,sizeof(ip));
    strncpy(ip,inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr),strlen(inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr)));


     tmp = strtok(ip,".");
     IP[0] = atoi(tmp);

     tmp = strtok(NULL,".");
     IP[1] = atoi(tmp);

     tmp = strtok(NULL,".");
     IP[2] = atoi(tmp);
     memset(IPPREFIX, 0 ,sizeof(IPPREFIX));
     sprintf(IPPREFIX,"%d.%d.%d.", IP[0], IP[1], IP[2]);

     tmp = strtok(NULL,".");

    status.AgentID = atoi(tmp);


//    struct sockaddr_in* sock_addr = (struct sockaddr_in*) & (ifr.ifr_addr );
//    unsigned char* uc  = &(sock_addr -> sin_addr.s_addr);
//    memset(IPPREFIX, 0 ,sizeof(IPPREFIX));
//    sprintf(IPPREFIX,"%d.%d.%d.", uc[0], uc[1], uc[2]);
//
//    status.AgentID = uc[3];
    return 0;
}


void AnalysisInitJson(char *buffer)
{
	cJSON *root, *item, *item2;
	cJSON *pJsonArray = NULL;
	cJSON *pArrayItem = NULL;
	char time[MinZone];
	int ii;

	if(!(root = cJSON_Parse(buffer)))
	{
		sNetOutput(">>>[ERR] Analisis Init json buffer failed in Global.c"," >>>");
		return;
	}


	if((item = cJSON_GetObjectItem(root, "Type")))
	{
		if (strcasecmp(item -> valuestring, "Init") == 0)
		{
			Flags.RecvPcMode = 1;


			if((item = cJSON_GetObjectItem(root, "MajorAgent")))
			{
				status.MajorAgent = item -> valueint;
			}

			item = cJSON_GetObjectItem(root, "TeamCount");
			TeamCount = item -> valueint;


			if((item = cJSON_GetObjectItem(root, "NeedAnswer")))
			{
				if(strcasecmp(item -> valuestring, YES) == 0)
				{
					//需要相应，则给对方发送相应。xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
					//这里是不是要改成验证CMDID的格式，那么打包的函数就不应该使用CreateMultiplexJson
					AgentAnswer = CreateMultiplexJson("AgentAnswer", getCommandID());
				}
			}


			//根据pc的时间，校准本地的时间。
			item = cJSON_GetObjectItem(root, "Time");
			memset(time, 0 ,sizeof(time));
			strncpy(time, item -> valuestring, strlen(item -> valuestring));

			year   = (time[2] - '0')*10 + (time[3] - '0') + 2000;
			month  = (time[5] - '0')*10 + (time[6] - '0');
			day    = (time[8] - '0')*10 + (time[9] - '0');

			hour   = (time[11] - '0')*10 + (time[12] - '0');
			minute = (time[14] - '0')*10 + (time[15] - '0');
			second = (time[17] - '0')*10 + (time[18] - '0');

			AdjustClockTime();


			item = cJSON_GetObjectItem(root, "SupplyAirToAir");
			status.Equipment.AirToAir = item -> valueint;

			item = cJSON_GetObjectItem(root, "SupplyAirToGround");
			status.Equipment.AirToGround = item -> valueint;

			item = cJSON_GetObjectItem(root, "SupplyLife");
			status.Equipment.SupplyLife = item ->valueint;



			pJsonArray = cJSON_GetObjectItem(root, "InitDetails");
			if (pJsonArray)
			{
				int size = cJSON_GetArraySize(pJsonArray);

				for(ii = 0; ii < size; ii++)
				{
					pArrayItem = cJSON_GetArrayItem(pJsonArray, ii);
					if (pArrayItem)
					{
						item2 = cJSON_GetObjectItem(pArrayItem, "AgentID");

						if( item2 -> valueint != status.AgentID )	continue;

						else{

							item2 = cJSON_GetObjectItem(pArrayItem, "GroupAgent");
							status.GroupAgent = item2 -> valueint;

							item2 = cJSON_GetObjectItem(pArrayItem, "TeamID");
							status.TeamID = item2 -> valueint;

							item2 = cJSON_GetObjectItem(pArrayItem, "MyTeamCount");
							status.MyTeamCount = item2 -> valueint;

							item2 = cJSON_GetObjectItem(pArrayItem, "Character");
							memset(status.Character, 0 ,sizeof(status.Character));
							strncpy(status.Character, item2 -> valuestring, strlen(item2->valuestring));


							if((strcasecmp(status.Character,GENERAL) == 0))	status.CharacterID = 12;
							if((strcasecmp(status.Character,GROUP)   == 0))	status.CharacterID = 11;
							if((strcasecmp(status.Character,MAJOR)   == 0))	status.CharacterID = 10;

							Flags.isBreak = 1;
							Flags.isSendRegister = 1;
							isWakeup = 1;
							sResOutput(">>> Init ownStatus is Okay "," >>>");

							break;
						}
					}
				}
			}
			else{
				sResOutput(">>>[Exc]: ","pJsonArray >>>");
			}
		}

	}

	sResOutput(">>> "," Init Finished >>>");
	cJSON_Delete(root);
	root = NULL;

}

void AnalysisChangeMissionJson(char *buffer)
{
	cJSON *root;
	cJSON *item;

	if(!(root = cJSON_Parse(buffer)))
	{
		sNetOutput(">>>[ERR] Analisis changemission json buffer failed in Global.c"," >>>");
		return;
	}


	if((item = cJSON_GetObjectItem(root, "Type")))
	{
		if (strcasecmp(item -> valuestring, "ChangeMission") == 0)
		{
			Flags.isStopMission = 1;
		}

		item = cJSON_GetObjectItem(root, "Distance");
		mission.Distance = item -> valueint;

		sleep(1);
		Flags.isStartMission = 1;

		printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
		printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
		printf("\n");
		printf("              Fighting      \n");
		printf("\n");
		printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
		printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");

	}
}


void AnalysisStartMissionJson(char *buffer)
{
	cJSON *root;
	cJSON *item;

	if(!(root = cJSON_Parse(buffer)))
	{
		sNetOutput(">>>[ERR] Analisis Formation json buffer failed in Global.c"," >>>");
		return;
	}


	if((item = cJSON_GetObjectItem(root, "Type")))
	{
		if (strcasecmp(item -> valuestring, "StartMission") == 0)
		{
			Flags.isStartMission = 1;
		}
	}
}


void AnalysisAdjustJson(char *buffer)
{
	cJSON *root;
	cJSON *item;

	if(!(root = cJSON_Parse(buffer)))
	{
		sNetOutput(">>>[ERR] Analisis Formation json buffer failed in Global.c"," >>>");
		return;
	}


	if((item = cJSON_GetObjectItem(root, "Type")))
	{
		if (strcasecmp(item -> valuestring, "Adjust") == 0)
		{
			item = cJSON_GetObjectItem(root, "IsMissionSuccess");
			if (strcasecmp(item->valuestring, YES) == 0)
			{
				Flags.isStopMission = 1;
			}
		}
	}
	cJSON_Delete(root);
	root = NULL;
}


void AnalysisCollectionJson(char *buffer)
{
	cJSON *root;
	cJSON *item;

	if(!(root = cJSON_Parse(buffer)))
	{
		return;
	}


	if((item = cJSON_GetObjectItem(root, "Type")))
	{
		if (strcasecmp(item -> valuestring, "Collection") == 0)
		{
			item = cJSON_GetObjectItem(root, "AliveStatus");
			if (strcasecmp(item->valuestring, status.AliveStatus) == 0)
			{
				item = cJSON_GetObjectItem(root, "CmdID");
				memset(status.AnswerCmdID,0,sizeof(status.AnswerCmdID));
				strncpy(status.AnswerCmdID, item->valuestring, strlen(item->valuestring));


				if((item = cJSON_GetObjectItem(root, "NeedAnswer")))
				{
					if(strcasecmp(item -> valuestring, YES) == 0)
					{
						//需要相应，则给对方发送相应。xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
						//这里是不是要改成验证CMDID的格式，那么打包的函数就不应该使用CreateMultiplexJson
						AgentAnswer = CreateMultiplexJson("ConfirmAlive", status.AnswerCmdID);

						pthread_mutex_lock(&Flags.mutex);
						SendToOthers(MAJORPC, AgentAnswer);
						pthread_mutex_unlock(&Flags.mutex);

						free(AgentAnswer);
						AgentAnswer = NULL;
					}
				}


				Flags.isRestart = 1;


			}
		}
	}
	cJSON_Delete(root);
	root = NULL;
}



void AnalysisReInitJson(char *buffer)
{
	cJSON *root, *item, *item2;
	cJSON *pJsonArray = NULL;
	cJSON *pArrayItem = NULL;
	int ii;

	if(!(root = cJSON_Parse(buffer)))
	{
		return;
	}


	if((item = cJSON_GetObjectItem(root, "Type")))
	{
		if (strcasecmp(item -> valuestring, "ReInit") == 0)
		{
			item = cJSON_GetObjectItem(root, "AliveStatus");
			if(strcasecmp(item -> valuestring, status.AliveStatus) == 0)
			{
				item = cJSON_GetObjectItem(root, "CmdID");
				memset(status.AnswerCmdID,0,sizeof(status.AnswerCmdID));
				strncpy(status.AnswerCmdID, item->valuestring, strlen(item->valuestring));

				if((item = cJSON_GetObjectItem(root, "NeedAnswer")))
				{
					if(strcasecmp(item -> valuestring, YES) == 0)
					{
						//需要相应，则给对方发送相应。xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
						//这里是不是要改成验证CMDID的格式，那么打包的函数就不应该使用CreateMultiplexJson
						AgentAnswer = CreateMultiplexJson("RecvReInit", status.AnswerCmdID);

						pthread_mutex_lock(&Flags.mutex);
						SendToOthers(MAJORPC, AgentAnswer);
						pthread_mutex_unlock(&Flags.mutex);

						free(AgentAnswer);
						AgentAnswer = NULL;
					}
				}

				Flags.RecvPcMode = 1;

				if((item = cJSON_GetObjectItem(root, "MajorAgent")))
				{
					status.MajorAgent = item -> valueint;
				}

				item = cJSON_GetObjectItem(root, "IsAttack");
				strncpy(mission.IsAttack, item->valuestring, strlen(item->valuestring));

				item = cJSON_GetObjectItem(root, "TeamCount");
				TeamCount = item -> valueint;

				item = cJSON_GetObjectItem(root, "Distance");
				mission.Distance = item -> valueint;

				pJsonArray = cJSON_GetObjectItem(root, "ReInitDetails");
				if (pJsonArray)
				{
					int size = cJSON_GetArraySize(pJsonArray);
//					TeamCount = size;
					for(ii = 0; ii < size; ii++)
					{
						pArrayItem = cJSON_GetArrayItem(pJsonArray, ii);
						if (pArrayItem)
						{
							item2 = cJSON_GetObjectItem(pArrayItem, "AgentID");

							if( item2 -> valueint != status.AgentID )	continue;

							else{

								item2 = cJSON_GetObjectItem(pArrayItem, "GroupAgent");
								status.GroupAgent = item2 -> valueint;

								item2 = cJSON_GetObjectItem(pArrayItem, "TeamID");
								status.TeamID = item2 -> valueint;

								item2 = cJSON_GetObjectItem(pArrayItem, "Character");
								strncpy(status.Character, item2 -> valuestring, strlen(item2->valuestring));

								item2 = cJSON_GetObjectItem(pArrayItem, "Shape");
								strncpy(status.TeamShape, item2 -> valuestring, strlen(item2->valuestring));

								item2 = cJSON_GetObjectItem(pArrayItem, "Mission");
								strncpy(status.TeamMission, item2->valuestring, strlen(item2->valuestring));

								if((strcasecmp(status.Character,GENERAL) == 0))	status.CharacterID = 12;
								if((strcasecmp(status.Character,GROUP)   == 0))	status.CharacterID = 11;
								if((strcasecmp(status.Character,MAJOR)   == 0))	status.CharacterID = 10;


								Flags.isStartMission = 1;
								sResOutput(">>> ReInit Status is Okay "," >>>");

								break;
							}
						}
					}
				}
				else{
					sResOutput(">>>[Exc]: ","pJsonArray >>>");
				}
			}
		}
	}
	cJSON_Delete(root);
	root = NULL;
}


void AnalysisResetJson(char *buffer)
{

	cJSON *root;
	cJSON *item;

	if(!(root = cJSON_Parse(buffer)))
	{
		sNetOutput(">>>[ERR] Analisis Formation json buffer failed in Reset Global.c"," >>>");
		return;
	}


	if((item = cJSON_GetObjectItem(root, "Type")))
	{
		if (strcasecmp(item -> valuestring, "Reset") == 0)
		{
			Flags.isStopMission = 1;
			sleep(1);				//关键！
			Flags.isReset = 1;
		}
	}
	cJSON_Delete(root);
	root = NULL;
}




//********************************************************
//********************************************************
//第三个线程函数。为接受pc机的侦查任务
void StartMission()
{
	//进入任务模式之后做三件事，1.发送任务状态信息。2.改变距离值。3.如果为攻击，那么要去做改变一些参数的方法。
	int distance;
	char *out;
	distance = mission.Distance;
	int value;
	int icount = 0;
	while(1)
	{

		//循环100毫秒进行一次，那么距离值的改变就会很快，如果距离值设为1000，则减少到0需要100秒
		usleep(SENDINTERVAL);

//		在第三个线程的每个while中都要添加一个判断是否有终止任务的指令，如果有的话，直接结束线程。

//		if() TO-DO
		if ( Flags.isStopMission == 1)
		{
			Flags.isStopMission = 0;
			return;
		}

		if ( ++Flags.isSendStatus == 50 )
		{
			status.Fule--; //5秒燃料减少1点

			Flags.isSendStatus = 0;
		}

		if ( ++Flags.isSendStatus2 == 25 )
		{

			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
			printf("\n");
			printf("              Navigate      \n");
			printf("\n");
			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");




			if(strcasecmp(status.TeamShape, "MasterSlave") == 0)
			{
				//改变distance值
				srand(time(NULL));
				value = rand()%4 +1;
				switch(value)
				{
				case 1:
					distance = distance - 1;
					break;
				default:
					distance = distance - 1;
					break;
				}
			}

			if(strcasecmp(status.TeamShape, "Parallel") == 0)
			{
				//改变distance值
				srand(time(NULL));
				value = rand()%4 +1;
				switch(value)
				{
				case 1:
					distance = distance - 1;
					break;
				default:
					distance = distance - 1;
					break;
				}
			}

			if(strcasecmp(status.TeamShape, "ChainStructure") == 0)
			{
				//改变distance值
				srand(time(NULL));
				value = rand()%4 +1;
				switch(value)
				{
				case 1:
					distance = distance - 1;
					break;
				default:
					distance = distance - 1;
					break;
				}
			}

			if( distance <= 0 )
			{
				//跳出此次循环，证明已经到了目标地点。
				Flags.isSendStatus2 = 0; 		//关键！！！
				break;
			}

			out = CreateMissionStatusJson(distance);

			pthread_mutex_lock(&Flags.mutex);
			SendToOthers(MAJORPC,out);
			pthread_mutex_unlock(&Flags.mutex);

			free(out);
			out = NULL;

			Flags.isSendStatus2 = 0;

		}


	}



	if(strcasecmp(mission.IsAttack,YES) == 0)
	{
		//攻击
		while(1)
		{
			usleep(SENDINTERVAL);

	//		在第三个线程的每个while中都要添加一个判断是否有终止任务的指令，如果有的话，直接结束线程。
	//		if() TO-DO
			if ( Flags.isStopMission == 1)
			{
				Flags.isStopMission = 0;
				printf("\n\n\n>>>  Finished Success! Please send [Collection] and [Reinit] to continue  >>> \n\n\n");
				fflush(stdout);

				return;
			}




			if ( ++Flags.isSendStatus == 50 )
			{
				status.Fule--; //5秒燃料减少1点
				out = CreateMissionStatusJson(distance);

				pthread_mutex_lock(&Flags.mutex);
				SendToOthers(MAJORPC,out);
				pthread_mutex_unlock(&Flags.mutex);

				free(out);
				out = NULL;

				Flags.isSendStatus = 0;
			}


			if ((strcasecmp(status.AliveStatus, YES) == 0))
			{
				if ((status.Fule <= 5)
						|| (status.Equipment.SupplyLife <= 1)
							|| (status.Equipment.AirToAir + status.Equipment.AirToGround <= 4))
				{
					memset(status.AliveStatus,0,sizeof(status.AliveStatus));
					strncpy(status.AliveStatus,NO,strlen(NO));

					isEndMission = 1;
					return;

				}else{

					if (++icount == 60) //6秒 对于 参数的 一次 变化
					{

						if (strcasecmp(status.TeamMission,BREAKOUT) == 0)
						{
							srand(time(NULL));
							value = rand()%4 +1;
							switch(value)
							{
							case 1:
								status.Equipment.AirToAir = status.Equipment.AirToAir - 2;
								status.Equipment.AirToGround = status.Equipment.AirToGround - 1;
								status.Equipment.SupplyLife = status.Equipment.SupplyLife - 12;
								break;
							case 2:
								status.Equipment.AirToAir = status.Equipment.AirToAir - 1;
								status.Equipment.AirToGround = status.Equipment.AirToGround - 1;
								status.Equipment.SupplyLife = status.Equipment.SupplyLife - 4;
								break;
							case 3:
								status.Equipment.AirToGround = status.Equipment.AirToGround - 1;
								status.Equipment.SupplyLife = status.Equipment.SupplyLife - 2;
								break;
							case 4:
								status.Equipment.AirToAir = status.Equipment.AirToAir - 1;
								status.Equipment.SupplyLife = status.Equipment.SupplyLife - 2;
								break;
							}

						}

						if (strcasecmp(status.TeamMission,ATTACK) == 0)
						{
							srand(time(NULL));
							value = rand()%4 +1;
							switch(value)
							{
							case 1:
								status.Equipment.AirToAir = status.Equipment.AirToAir - 1;
								status.Equipment.AirToGround = status.Equipment.AirToGround - 1;
								status.Equipment.SupplyLife = status.Equipment.SupplyLife - 5;
								break;
							case 2:
								status.Equipment.AirToAir = status.Equipment.AirToAir - 1;
								status.Equipment.AirToGround = status.Equipment.AirToGround - 2;
								status.Equipment.SupplyLife = status.Equipment.SupplyLife - 6;
								break;
							default :
								status.Equipment.AirToAir = status.Equipment.AirToAir - 2;
								status.Equipment.AirToGround = status.Equipment.AirToGround - 1;
								status.Equipment.SupplyLife = status.Equipment.SupplyLife - 2;
								break;
							}
						}

						if (strcasecmp(status.TeamMission,CIRCUITY) == 0)
						{
							srand(time(NULL));
							value = rand()%4 +1;
							switch(value)
							{
							case 1:
								status.Fule = status.Fule - 3;
								status.Equipment.AirToAir = status.Equipment.AirToAir - 1;
								status.Equipment.AirToGround = status.Equipment.AirToGround - 1;
								status.Equipment.SupplyLife = status.Equipment.SupplyLife - 11;
								break;
							case 2:
								status.Fule = status.Fule - 2;
								status.Equipment.AirToGround = status.Equipment.AirToGround - 1;
								status.Equipment.SupplyLife = status.Equipment.SupplyLife - 9;
								break;
							default :
								status.Fule = status.Fule - 2;
								status.Equipment.SupplyLife = status.Equipment.SupplyLife - 6;
								break;
							}

						}

						icount = 0;

					}
				}

			}

		}
	}



}


char *CreateMissionStatusJson(int distance)
{
	cJSON *root = NULL;
	char  *out  = NULL;

	root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "Type", "MissionStatusUpload");
	cJSON_AddStringToObject(root, "CmdID", getCommandID());
	cJSON_AddStringToObject(root, "Time", setLocalTime());
	cJSON_AddStringToObject(root, "Broadcast", NONE);
	cJSON_AddStringToObject(root, "NeedAnswer", NO);
	cJSON_AddNumberToObject(root, "TeamID", status.TeamID);
	cJSON_AddNumberToObject(root, "AgentID", status.AgentID);
	cJSON_AddNumberToObject(root, "AirToAir", status.Equipment.AirToAir);
	cJSON_AddNumberToObject(root, "AirToGround", status.Equipment.AirToGround);
	cJSON_AddNumberToObject(root, "SupplyLife", status.Equipment.SupplyLife);
	cJSON_AddNumberToObject(root, "Distance", distance);			//pc可根据此参数是否为0和下边的isAttack来判断是否开始攻击。
	cJSON_AddStringToObject(root, "IsAttack", mission.IsAttack);
	cJSON_AddStringToObject(root, "Mission", status.TeamMission);
	cJSON_AddStringToObject(root, "TeamShape", status.TeamShape);


	out = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	sResOutput("->->->StatusBuffer : ",out);
	return out;
}


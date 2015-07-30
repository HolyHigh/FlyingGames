/*
 * GeneralAgent.c
 *
 *  Created on: 2014年7月1日
 *      Author: zhich
 */


#include "GeneralAgent.h"


void GeneralAgent()
{
	char *out;
	pthread_t missions;
	int ret;


	while(1)
	{
		//首先，要定时给pc机 发送状态信息
		usleep(SENDINTERVAL);
		if ( ++Flags.isSend == 50 )
		{
			if ( Flags.GlobalPcMode )
			{
				out = CreateMultiplexJson("AgentStatus", getCommandID());

				pthread_mutex_lock(&Flags.mutex);
				SendToOthers(MAJORPC,out);
				pthread_mutex_unlock(&Flags.mutex);

				free(out);
				out = NULL;
			}

			if ( Flags.GlobalFreeMode )
			{
				//TO-DO 自由模式

			}


			Flags.isSend = 0;
		}

		//要向簇首登记，发送相应指令给簇首（如果失败的话，尝试三次，还失败，就往控制中心发送指令）
		if(Flags.isSendRegister)
		{
			char str[MinZone];
			int  j = 0;
			memset(str,0,sizeof(str));
			out = CreateRegisterJson();
			IntToString(status.GroupAgent, str);

			pthread_mutex_lock(&Flags.mutex);
			SendToOthers(str,out);
			pthread_mutex_unlock(&Flags.mutex);

			sResOutput(">>>this is first send",">>>");


			while(1)
			{
				sleep(2);
				if( (j < 3) && (!Flags.registers.GetAnswer))
				{

					pthread_mutex_lock(&Flags.mutex);
					SendToOthers(str,out);
					pthread_mutex_unlock(&Flags.mutex);

					printf(">>>> \nthis is %d send >>>",j+1);
					fflush(stdout);

					j++;
					continue;
				}
				else if((j < 3) && (Flags.registers.GetAnswer))
				{
					break;
				}
				else if((j >= 3) && (Flags.registers.GetAnswer))
				{
					break;
				}
				else{
					Flags.registers.isResend = 1;
					break;
				}
			}


			if(Flags.registers.isResend)
			{

				pthread_mutex_lock(&Flags.mutex);
				SendToOthers(MAJORPC,out);
				pthread_mutex_unlock(&Flags.mutex);

				Flags.registers.isResend = 0;
			}



			free(out);
			out = NULL;
			Flags.registers.GetAnswer = 0;
			Flags.isSendRegister = 0;

		}

		//接受簇首发过来的编队结构指令，使自己形成相应编队
		//done

		//接受由簇首发过来的任务指令（给予回应，这个过程由network模块考虑）
		//done

		//接受pc发过来的广播指令，并进入第三个线程（任务侦查模式）
		if (Flags.isStartMission)
		{
			ret = pthread_create(&missions,NULL,(void*)StartMission,NULL);

			if(ret != 0)
				printf(">>>	Create Pthread network Failed!\n");

			Flags.isStartMission = 0;
		}

		//接收了collection指令，就开始重新选择角色，但是自身的参数不变（载弹量、生命值等参数）
		if (Flags.isRestart)
		{
			status.MajorAgent = 0;
			status.GroupAgent = 0;
			status.MyTeamCount = 0;
			status.TeamID = 0;
			mission.Distance = 0;
			memset(status.TeamShape,0,sizeof(status.TeamShape));
			memset(status.TeamMission,0,sizeof(status.TeamMission));
			memset(status.Character,0,sizeof(status.Character));
			Flags.isRestart = 0;
			isCallInit = 0;

			return;
		}


		//待续：接受PC的广播指令，清空数据结构，并重新选择角色
		//PC重置指令，在任何情况下 都可以接收，从最初状态开始！
		if (Flags.isReset)
		{
			DestroyValue();
			return;
		}

		if( isEndMission )
		{
			DestroyValue();
			strncpy(status.AliveStatus, NO,strlen(NO));
			printf("\n\n >* Agent is Dead now, you can send [Init] to wake up it! *>\n");
			fflush(stdout);
			isCallInit = 0;

			isEndMission = 0;
			return;
		}

		//待续：自由编队
	}
}


char *CreateRegisterJson()
{
	cJSON *root = NULL;
	char  *out  = NULL;

	root = cJSON_CreateObject();
/*******/
	memset(status.SendCmdID, 0, sizeof(status.SendCmdID));
	strncpy(status.SendCmdID,getCommandID(),strlen(getCommandID()));
/*******/
	cJSON_AddStringToObject(root, "Type", "Register");
	cJSON_AddStringToObject(root, "CmdID", status.SendCmdID);
	cJSON_AddStringToObject(root, "Time", setLocalTime());
	cJSON_AddStringToObject(root, "Broadcast", GROUP);
	cJSON_AddStringToObject(root, "NeedAnswer", YES);
	cJSON_AddNumberToObject(root, "TeamID", status.TeamID);
	cJSON_AddNumberToObject(root, "AgentID", status.AgentID);
	cJSON_AddNumberToObject(root, "GroupAgent",status.GroupAgent);

	out = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	sResOutput("->->->StatusBuffer : ",out);
	return out;

}




//thread 2 analisis Json
//解析json的工作，放在了每个角色的模块下，虽然解析的工作属于第二个线程。
void AnalysisGroupFormationJson(char *buffer)
{
	cJSON *root;
	cJSON *item;
	char str[MinZone];


	if(!(root = cJSON_Parse(buffer)))
	{
		sNetOutput(">>>[ERR] Analisis Formation json buffer failed in Global.c"," >>>");
		return;
	}


	if((item = cJSON_GetObjectItem(root, "Type")))
	{
		if (strcasecmp(item -> valuestring, "GroupFormation") == 0)
		{
			if((item = cJSON_GetObjectItem(root, "TeamID")))
			{
				if( item -> valueint == status.TeamID)
				{
					if((item = cJSON_GetObjectItem(root, "Broadcast")))
					{
						if( (strcasecmp(status.Character,item->valuestring) == 0) )
						{
/******/
							item = cJSON_GetObjectItem(root, "CmdID");
							memset(status.AnswerCmdID,0,sizeof(status.AnswerCmdID));
							strncpy(status.AnswerCmdID, item->valuestring, strlen(item->valuestring));
/******/
							if((item = cJSON_GetObjectItem(root, "NeedAnswer")))
							{
								if(strcasecmp(item -> valuestring, YES) == 0)
								{
									//需要相应，则给对方发送相应。xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
									//这里是不是要改成验证CMDID的格式，那么打包的函数就不应该使用CreateMultiplexJson
									memset(str,0,sizeof(str));
									IntToString(status.GroupAgent, str);
									AgentAnswer = CreateMultiplexJson("GroupFormationAnswer",status.AnswerCmdID);

									pthread_mutex_lock(&Flags.mutex);
									SendToOthers(str, AgentAnswer);
									pthread_mutex_unlock(&Flags.mutex);

									printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
									printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
									printf("\n");
									printf("          Formation Finished\n");
									printf("\n");

									printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");

									printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");


									free(AgentAnswer);
									AgentAnswer = NULL;

								}
							}

							item = cJSON_GetObjectItem(root, "Shape");
							strncpy(status.TeamShape, item->valuestring, strlen(item->valuestring));

							item = cJSON_GetObjectItem(root, "Mission");
							strncpy(status.TeamMission, item->valuestring,strlen(item->valuestring));

							item = cJSON_GetObjectItem(root, "DefenseAbility");
							status.DefenseAbility = item -> valueint;

							item = cJSON_GetObjectItem(root, "AttackAbility");
							status.AttackAbility = item -> valueint;

						}
					}
				}
			}

		}
	}

	cJSON_Delete(root);
	root = NULL;

}



void AnalysisDecisionDownloadJson(char *buffer)
{
	cJSON *root;
	cJSON *item;
	char str[MinZone];

	if(!(root = cJSON_Parse(buffer)))
	{
		sNetOutput(">>>[ERR] Analisis Formation json buffer failed in Global.c"," >>>");
		return;
	}


	if((item = cJSON_GetObjectItem(root, "Type")))
	{
		if (strcasecmp(item -> valuestring, "DecisionDownload") == 0)
		{
			item = cJSON_GetObjectItem(root, "Broadcast");
			if( strcasecmp(item -> valuestring, status.Character) == 0)
			{
				item = cJSON_GetObjectItem(root, "TeamID");
				if(item->valueint == status.TeamID)
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
							memset(str,0,sizeof(str));
							IntToString(status.GroupAgent, str);
							AgentAnswer = CreateMultiplexJson("RecvMission",status.AnswerCmdID);

							pthread_mutex_lock(&Flags.mutex);
							SendToOthers(str, AgentAnswer);
							pthread_mutex_unlock(&Flags.mutex);

							free(AgentAnswer);
							AgentAnswer = NULL;
						}
					}


					item = cJSON_GetObjectItem(root, "TargetType");
					strncpy(mission.TargetType, item->valuestring, strlen(item->valuestring));

					item = cJSON_GetObjectItem(root, "Distance");
					mission.Distance = item->valueint;

					item = cJSON_GetObjectItem(root, "FirePower");
					mission.FirePower = item->valueint;

					item = cJSON_GetObjectItem(root, "ObjectCount");
					mission.ObjectCount = item->valueint;

					item = cJSON_GetObjectItem(root, "DecisionResult");
					memset(status.TeamMission,0,sizeof(status.TeamMission));
					strncpy(status.TeamMission, item->valuestring, strlen(item->valuestring));

					item = cJSON_GetObjectItem(root, "IsAttack");
					strncpy(mission.IsAttack, item->valuestring, strlen(item->valuestring));

				}
			}
		}
	}

	cJSON_Delete(root);
	root = NULL;
}







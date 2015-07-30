/*
 * GroupAgent.c
 *
 *  Created on: 2014年7月1日
 *      Author: zhich
 */


#include "GroupAgent.h"

void GroupAgent()
{
	char *out;
	pthread_t missions;
	int ret;


	while(1)
	{
		//首先，要定时给PC机发送 状态信息
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

		//接受组内的登记注册信息（这块由network模块完成），并把这些信息重新组成指令 发送给PC
		if( Flags.isSendGroupReport)
		{
			out = GroupReportInitJson("GroupReportInitialization");

			pthread_mutex_lock(&Flags.mutex);
			SendToOthers(MAJORPC,out);
			pthread_mutex_unlock(&Flags.mutex);

			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
			printf("\n");

			printf("        Initialization and Devided Into Groups Finished\n");
			printf("\n");

			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");


			free(out);
			out = NULL;

			Flags.isSendGroupReport = 0;
		}


		//接受来自PC的编队结构指令（network完成），存结构体，并发送指令 给组内agent
		if (Flags.isGroupFormation)
		{
			out = CreateGroupFormationJson();

			pthread_mutex_lock(&Flags.mutex);
			SendToOthers(BROADCAST,out);
			pthread_mutex_unlock(&Flags.mutex);


			free(out);
			out = NULL;

			Flags.isGroupFormation = 0;

		}

		//获取agent响应（network完成），并把编队完成的信息发送给pc。
		if (Flags.isSendGroupFormation)
		{
			out = CreateGroupReportFormationJson();

			pthread_mutex_lock(&Flags.mutex);
			SendToOthers(Flags.CurrentControler,out);
			pthread_mutex_unlock(&Flags.mutex);


			free(out);
			out = NULL;
			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
			printf("\n");
			printf("       Formation Finished\n");
			printf("\n");
			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");

			Flags.isSendGroupFormation = 0;
		}


		//接受主控agent的任务指令，并把这些指令下发给组内agent
		if (Flags.isSendDecisionDown)
		{
			out = CreateDecisionDownloadJson();

			pthread_mutex_lock(&Flags.mutex);
			SendToOthers(BROADCAST,out);
			pthread_mutex_unlock(&Flags.mutex);


			free(out);
			out = NULL;

			Flags.isSendDecisionDown = 0;
		}


		//任务已经下发完成，这时候簇首需要给PC一个指令，告诉pc下位机已经准备结束。
		if (Flags.isSendFinishedDownload)
		{
			out = CreateFinishedMissionJson();

			pthread_mutex_lock(&Flags.mutex);

			SendToOthers(Flags.CurrentControler,out);
			pthread_mutex_unlock(&Flags.mutex);

			free(out);
			out = NULL;

			Flags.isSendFinishedDownload = 0;
		}



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


char *GroupReportInitJson(char *Type)
{
	char  *out  = NULL;
	cJSON *root = NULL;

	root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "Type", Type);
	cJSON_AddStringToObject(root, "CmdID", getCommandID());
	cJSON_AddStringToObject(root, "Time", setLocalTime());
	cJSON_AddStringToObject(root, "Broadcast", NONE);
	cJSON_AddStringToObject(root, "NeedAnswer", NO);
	cJSON_AddNumberToObject(root, "TeamID", status.TeamID);
	cJSON_AddStringToObject(root, "RegisterResult", "Success");


	out = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	sResOutput("->->->StatusBuffer : ",out);
	return out;
}


char *CreateGroupFormationJson()
{
	char  *out  = NULL;
	cJSON *root = NULL;

	root = cJSON_CreateObject();

	memset(status.SendCmdID, 0, sizeof(status.SendCmdID));
	strncpy(status.SendCmdID,getCommandID(),strlen(getCommandID()));

	cJSON_AddStringToObject(root, "Type", "GroupFormation");
	cJSON_AddStringToObject(root, "CmdID", status.SendCmdID);
	cJSON_AddStringToObject(root, "Time", setLocalTime());
	cJSON_AddStringToObject(root, "Broadcast", GENERAL);
	cJSON_AddStringToObject(root, "NeedAnswer", YES);
	cJSON_AddNumberToObject(root, "TeamID", status.TeamID);
	cJSON_AddStringToObject(root, "Shape", status.TeamShape);
	cJSON_AddStringToObject(root, "Mission", status.TeamMission);
	cJSON_AddNumberToObject(root, "DefenseAbility", status.DefenseAbility);
	cJSON_AddNumberToObject(root, "AttackAbility", status.AttackAbility);


	out = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	sResOutput("->->->StatusBuffer : ",out);
	return out;
}


char *CreateGroupReportFormationJson()
{
	char  *out  = NULL;
	cJSON *root = NULL;

	root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "Type", "GroupReportFormation");
	cJSON_AddStringToObject(root, "CmdID", getCommandID());
	cJSON_AddStringToObject(root, "Time",  setLocalTime());
	cJSON_AddStringToObject(root, "Broadcast", NONE);
	cJSON_AddStringToObject(root, "NeedAnswer", NO);
	cJSON_AddNumberToObject(root, "TeamID", status.TeamID);
	cJSON_AddStringToObject(root, "ReadyFlag", YES);


	out = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	sResOutput("->->->StatusBuffer : ",out);

	return out;

}


char *CreateDecisionDownloadJson()
{
	char  *out  = NULL;
	cJSON *root = NULL;

	root = cJSON_CreateObject();


	memset(status.SendCmdID, 0, sizeof(status.SendCmdID));
	strncpy(status.SendCmdID,getCommandID(),strlen(getCommandID()));

	cJSON_AddStringToObject(root, "Type", "DecisionDownload");
	cJSON_AddStringToObject(root, "CmdID", status.SendCmdID);
	cJSON_AddStringToObject(root, "Time",  setLocalTime());
	cJSON_AddStringToObject(root, "Broadcast", GENERAL);
	cJSON_AddStringToObject(root, "NeedAnswer", YES);
	cJSON_AddNumberToObject(root, "TeamID", status.TeamID);
	cJSON_AddNumberToObject(root, "Distance", mission.Distance);
	cJSON_AddNumberToObject(root, "FirePower", mission.FirePower);
	cJSON_AddNumberToObject(root, "ObjectCount", mission.ObjectCount);
	cJSON_AddStringToObject(root, "DecisionResult", status.TeamMission);
	cJSON_AddStringToObject(root, "IsAttack", mission.IsAttack);
	cJSON_AddStringToObject(root, "TargetType", mission.TargetType);



	out = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	sResOutput("->->->StatusBuffer : ",out);
	return out;

}


char *CreateFinishedMissionJson()
{
	char  *out  = NULL;
	cJSON *root = NULL;

	root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "Type", "FinishedMission");
	cJSON_AddStringToObject(root, "CmdID", getCommandID());
	cJSON_AddStringToObject(root, "Time",  setLocalTime());
	cJSON_AddStringToObject(root, "Broadcast", NONE);
	cJSON_AddStringToObject(root, "NeedAnswer", NO);
	cJSON_AddNumberToObject(root, "TeamID", status.TeamID);


	out = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	sResOutput("->->->StatusBuffer : ",out);
	return out;
}



//thread 2 analisis Json
//解析json的工作，放在了每个角色的模块下，虽然解析的工作属于第二个线程。
void AnalysisFormationJson(char *buffer)
{
	/*
	 *
	cJSON *root;
	cJSON *item;

	if(!(root = cJSON_Parse(buffer)))
	{
		sNetOutput(">>>[ERR] Analisis Formation json buffer failed in Global.c"," >>>");
		return;
	}


	if((item = cJSON_GetObjectItem(root, "Type")))
	{
		if (strcasecmp(item -> valuestring, "Register") == 0)
		{

		}
	}

	cJSON_Delete(root);
	root = NULL;

	*/


	cJSON *root;
	cJSON *item, *item2;
	cJSON *pJsonArray = NULL;
	cJSON *pArrayItem = NULL;
	int ii;


	if(!(root = cJSON_Parse(buffer)))
	{
		sNetOutput(">>>[ERR] Analisis Formation json buffer failed in Global.c"," >>>");
		return;
	}


	if((item = cJSON_GetObjectItem(root, "Type")))
	{
		if (strcasecmp(item -> valuestring, "Formation") == 0)
		{
			//判断是普通簇首的工作
			item = cJSON_GetObjectItem(root, "Broadcast");
			if( (strcasecmp(status.Character, item -> valuestring) == 0))
			{

				item = cJSON_GetObjectItem(root, "CmdID");
				memset(status.AnswerCmdID, 0, sizeof(status.AnswerCmdID));
				strncpy(status.AnswerCmdID, item->valuestring, strlen(item->valuestring));



				if((item = cJSON_GetObjectItem(root, "NeedAnswer")))
				{
					if(strcasecmp(item -> valuestring, YES) == 0)
					{
						//需要相应，则给对方发送相应。xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
						//这里是不是要改成验证CMDID的格式，那么打包的函数就不应该使用CreateMultiplexJson//已改
						AgentAnswer = CreateMultiplexJson("FormationAnswer",status.AnswerCmdID);

						pthread_mutex_lock(&Flags.mutex);
						SendToOthers(MAJORPC, AgentAnswer);
						pthread_mutex_unlock(&Flags.mutex);

						free(AgentAnswer);
						AgentAnswer = NULL;
					}
				}

				pJsonArray = cJSON_GetObjectItem(root, "FormationDetails");
				if (pJsonArray)
				{

					int size = cJSON_GetArraySize(pJsonArray);
					for(ii = 0; ii < size; ii++)
					{
						pArrayItem = cJSON_GetArrayItem(pJsonArray, ii);
						if (pArrayItem)
						{
							item2 = cJSON_GetObjectItem(pArrayItem, "TeamID");

							if( item2 -> valueint != status.TeamID )	continue;

							else{

								item2 = cJSON_GetObjectItem(pArrayItem, "Shape");
								memset(status.TeamShape, 0, sizeof(status.TeamShape));
								strncpy(status.TeamShape, item2 -> valuestring, strlen(item2->valuestring));

								item2 = cJSON_GetObjectItem(pArrayItem, "Mission");
								memset(status.TeamMission, 0, sizeof(status.TeamMission));
								strncpy(status.TeamMission, item2 -> valuestring, strlen(item2->valuestring));

								item2 = cJSON_GetObjectItem(pArrayItem, "DefenseAbility");
								status.DefenseAbility = item2 -> valueint;

								item2 = cJSON_GetObjectItem(pArrayItem, "AttackAbility");
								status.AttackAbility = item2 -> valueint;

								Flags.isGroupFormation = 1;

								break;
							}
						}
					}
				}
				else{
					sResOutput(">>>[Exc]: ","pJsonArray in analisisformation 1 >>>");
				}
			}


			//判断是MajorAgent的情况，因为MajorAgent也是簇首，所以他也要做一些工作，并且有一些工作，普通的簇首是没有的
			if( (strcasecmp(status.Character, "MajorAgent") == 0))
			{
				item = cJSON_GetObjectItem(root, "CmdID");
				memset(status.AnswerCmdID, 0, sizeof(status.AnswerCmdID));
				strncpy(status.AnswerCmdID, item->valuestring, strlen(item->valuestring));

				if((item = cJSON_GetObjectItem(root, "NeedAnswer")))
				{

					if(strcasecmp(item -> valuestring, YES) == 0)
					{
						//需要相应，则给对方发送相应。xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
						//这里是不是要改成验证CMDID的格式，那么打包的函数就不应该使用CreateMultiplexJson
						AgentAnswer = CreateMultiplexJson("FormationAnswer",status.AnswerCmdID);

						pthread_mutex_lock(&Flags.mutex);
						SendToOthers(MAJORPC, AgentAnswer);
						pthread_mutex_unlock(&Flags.mutex);

						free(AgentAnswer);
						AgentAnswer = NULL;
					}
				}

				pJsonArray = cJSON_GetObjectItem(root, "FormationDetails");
				if (pJsonArray)
				{

					int size = cJSON_GetArraySize(pJsonArray);
					for(ii = 0; ii < size; ii++)
					{
						pArrayItem = cJSON_GetArrayItem(pJsonArray, ii);
						if (pArrayItem)
						{
							//这里做一下存储的操作，因为这里是majoragent，所以要存储下所有编队中的防御能力、攻击能力等信息
							//为之后的工作做准备！
							item2 = cJSON_GetObjectItem(pArrayItem, "TeamID");
							majorSaved[ii].TeamID = item2 -> valueint;

							item2 = cJSON_GetObjectItem(pArrayItem, "AttackAbility");
							majorSaved[ii].AttackAbility = item2 -> valueint;

							item2 = cJSON_GetObjectItem(pArrayItem, "DefenseAbility");
							majorSaved[ii].DefenseAbility = item2 -> valueint;



							//以下和上边的GroupAgent一样，存储在自己的变量中
							//与上边不同的是，下边没有了break，因为估计到上边的操作，所以不break。
							item2 = cJSON_GetObjectItem(pArrayItem, "TeamID");

							if( item2 -> valueint != status.TeamID )	continue;

							else{

								item2 = cJSON_GetObjectItem(pArrayItem, "Shape");
								memset(status.TeamShape, 0, sizeof(status.TeamShape));
								strncpy(status.TeamShape, item2 -> valuestring, strlen(item2->valuestring));

								item2 = cJSON_GetObjectItem(pArrayItem, "Mission");
								memset(status.TeamMission, 0, sizeof(status.TeamMission));
								strncpy(status.TeamMission, item2 -> valuestring, strlen(item2->valuestring));

								item2 = cJSON_GetObjectItem(pArrayItem, "DefenseAbility");
								status.DefenseAbility = item2 -> valueint;

								item2 = cJSON_GetObjectItem(pArrayItem, "AttackAbility");
								status.AttackAbility = item2 -> valueint;

								Flags.isGroupFormation = 1;
//								break;
							}
						}

					}

				}
				else{
					sResOutput(">>>[Exc]: ","pJsonArray >>>");
				}
//				printf("1.Attack:%d---2.Defense:%d---Mission:%s\n",majorSaved[0].AttackAbility,
//											majorSaved[1].DefenseAbility,status.TeamMission);
//									fflush(stdout);
			}

		}
	}

	cJSON_Delete(root);
	root = NULL;
}




void AnalysisRegisterJson(char *buffer)
{
	cJSON *root;
	cJSON *item;
	int tempvalue;
	char str[MinZone];


	if(!(root = cJSON_Parse(buffer)))
	{
		sNetOutput(">>>[ERR] Analisis Register json buffer failed in Global.c"," >>>");
		return;
	}


	if((item = cJSON_GetObjectItem(root, "Type")))
	{
		if (strcasecmp(item -> valuestring, "Register") == 0)
		{
			item = cJSON_GetObjectItem(root, "Broadcast");
			if ((strcasecmp(item -> valuestring, status.Character) ==0)||
					(strcasecmp(status.Character,MAJOR) == 0))
			{
				if((item = cJSON_GetObjectItem(root, "TeamID")))
				{
					if( item -> valueint == status.TeamID)
					{

						item = cJSON_GetObjectItem(root, "CmdID");
						memset(status.AnswerCmdID,0,sizeof(status.AnswerCmdID));
						strncpy(status.AnswerCmdID, item->valuestring, strlen(item->valuestring));

						item = cJSON_GetObjectItem(root, "AgentID");
						tempvalue = item -> valueint;
						memset(str,0,sizeof(str));
						IntToString(tempvalue, str);


//						printf(">>>~~~~Ready !!!!!!!!!!!\n\n");
//						fflush(stdout);
						if((item = cJSON_GetObjectItem(root, "NeedAnswer")))
						{
							if(strcasecmp(item -> valuestring, YES) == 0)
							{
//								printf(">>>~~~~COME IN !!!!!!!!!!!\n\n");
	//							fflush(stdout);

								AgentAnswer = thread2_creat_register_answer(status.AnswerCmdID);

								pthread_mutex_lock(&Flags.mutex);
								SendToOthers(str,AgentAnswer);
								pthread_mutex_unlock(&Flags.mutex);

								free(AgentAnswer);
								AgentAnswer = NULL;

//								printf(">>>~~~~AgentAnswer not finished !!!!!!\n\n");
//								fflush(stdout);
							}
						}

//						printf(">>>~~~~finished !!!!!!!!!!!\n\n");
//						fflush(stdout);

						tMemberCount++;

						if(tMemberCount == (status.MyTeamCount - 1))
						{
							tMemberCount = 0;
							Flags.isSendGroupReport = 1;
						}
					}
				}

			}

		}
	}

	cJSON_Delete(root);
	root = NULL;

}


char *thread2_creat_register_answer(char *cmdid)
{
	char  *out  = NULL;
	cJSON *root = NULL;

	root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "Type", "RegisterAnswer");
	cJSON_AddStringToObject(root, "CmdID", cmdid);
	cJSON_AddNumberToObject(root, "TeamID", status.TeamID);
	cJSON_AddStringToObject(root, "RegisterResult", "Success");

	out = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

//	sResOutput("-!-!-! answer : ",out);
	return out;
}


void AnalysisMissionDecisionJson(char *buffer)
{

	cJSON *root;
	cJSON *item, *item2;
	cJSON *pJsonArray = NULL;
	cJSON *pArrayItem = NULL;
	int ii;

	if(!(root = cJSON_Parse(buffer)))
	{
		sNetOutput(">>>[ERR] Analisis Formation json buffer failed in Global.c"," >>>");
		return;
	}


	if((item = cJSON_GetObjectItem(root, "Type")))
	{
		if (strcasecmp(item -> valuestring, "MissionDecision") == 0)
		{
			item = cJSON_GetObjectItem(root, "Broadcast");
			if( (strcasecmp(item -> valuestring, status.Character) == 0) ||
					(strcasecmp(status.Character, MAJOR) == 0))
			{

				if((item = cJSON_GetObjectItem(root, "NeedAnswer")))
				{
					if(strcasecmp(item -> valuestring, YES) == 0)
					{

						//需要相应，则给对方发送相应。xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
						//这里是不是要改成验证CMDID的格式，那么打包的函数就不应该使用CreateMultiplexJson
						AgentAnswer = CreateMultiplexJson("AgentAnswer", getCommandID());
					}
				}

				item = cJSON_GetObjectItem(root, "TargetType");
				memset(mission.TargetType, 0, sizeof(mission.TargetType));
				strncpy(mission.TargetType, item -> valuestring, strlen(item->valuestring));

				item = cJSON_GetObjectItem(root, "Distance");
				mission.Distance = item -> valueint;

				item = cJSON_GetObjectItem(root, "FirePower");
				mission.FirePower = item -> valueint;

				item = cJSON_GetObjectItem(root, "ObjectCount");
				mission.ObjectCount = item -> valueint;

				item = cJSON_GetObjectItem(root, "IsAttack");
				memset(mission.IsAttack, 0, sizeof(mission.IsAttack));
				strncpy(mission.IsAttack, item->valuestring, strlen(item->valuestring));

				pJsonArray = cJSON_GetObjectItem(root, "DecisionDetails");
				if (pJsonArray)
				{

					int size = cJSON_GetArraySize(pJsonArray);
					for(ii = 0; ii < size; ii++)
					{
						pArrayItem = cJSON_GetArrayItem(pJsonArray, ii);
						if (pArrayItem)
						{
							item2 = cJSON_GetObjectItem(pArrayItem, "TeamID");

							if( item2 -> valueint != status.TeamID )	continue;

							else{

								item2 = cJSON_GetObjectItem(pArrayItem, "DecisionResult");
								memset(status.TeamMission,0,sizeof(status.TeamMission));
								strncpy(status.TeamMission, item2 -> valuestring, strlen(item2->valuestring));

								Flags.isSendDecisionDown = 1;

								break;
							}
						}
					}
				}

			}
		}
	}

	cJSON_Delete(root);
	root = NULL;


}



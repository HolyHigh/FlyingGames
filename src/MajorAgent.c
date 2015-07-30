/*
 * MajorAgent.c
 *
 *  Created on: 2014年7月1日
 *      Author: zhich
 */


#include "MajorAgent.h"


void MajorAgent()
{
//	int iCount = 0;
	char *out;
	pthread_t missions;
	int ret;

	while(1)
	{
		//首先 要发送给pc机 当前自己的状态
//		sResOutput("> In MajorAgent "," >");
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


		/*
		  	接收PC的任务指令，转变为任务，下发到编队的簇首（而且，此主控agent也是簇首，找一个切入点进行身份的切换，通信）

			目前没有太好的办法，简单的办法：把major的工作范围放大，使之多做一些工作

			总的来说就是：接收到任务指令，存入结构体中，然后处理下边的 功能 ，处理完之后 启动 ＆＆ 攻击模式

		*/

		if (Flags.RecvMission)
		{
			//PC指令接受到了，首先要判断是敌人还是友方，其次再进行决策，选出哪个编队采取的是什么阵型
			memset(mission.IsAttack,0,sizeof(mission.IsAttack));

			if( strcasecmp(mission.TargetType, "Opponent") == 0)
			{

				strncpy(mission.IsAttack,YES,strlen(YES));
				DecisionMission(1);

			}
			else
			{
				strncpy(mission.IsAttack,NO,strlen(NO));
				DecisionMission(0);

			}

			out = MajorCreateMissionJson("MissionDecision");
//			out = CreateMultiplexJson("MissionDecision", getCommandID());

			pthread_mutex_lock(&Flags.mutex);
			SendToOthers(BROADCAST,out);
//			SendToOthers(MAJORPC,out);
			pthread_mutex_unlock(&Flags.mutex);

			free(out);
			out = NULL;
			Arrays = NULL; //注意这里！
			//sleep(2);
			Flags.RecvMission = 0;		//各个控制发送的地方 都要注意这个地方。
		}
			/*
			 * 	copy 一些 GroupAgent的功能
			 *
			 * 	接受组内的登记注册信息（这块由network模块完成），并把这些信息重新组成指令 发送给PC
			 *
			 *	接受来自PC的编队结构指令（network完成），存结构体，并发送指令 给组内agent
			 *
			 *	获取agent响应（network完成），并把编队完成的信息发送给pc。
			 *
			 *	接受主控agent的任务指令，并把这些指令下发给组内agent//////////kaolv
			 *
			 *	任务已经下发完成，这时候簇首需要给PC一个指令，告诉pc下位机已经准备结束。
			 *
			 *
			 *
			 */
		//finished
		if( Flags.isSendGroupReport)
		{
			out = GroupReportInitJson("GroupReportInitialization");

			pthread_mutex_lock(&Flags.mutex);
			SendToOthers(MAJORPC,out);
			pthread_mutex_unlock(&Flags.mutex);

			free(out);
			out = NULL;

			Flags.isSendGroupReport = 0;


			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
			printf("\n");

			printf("        Initialization and Devided Into Groups Finished\n");
			printf("\n");

			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");

		}

		//finished
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

		//finished
		if (Flags.isSendGroupFormation)
		{
			out = CreateGroupReportFormationJson();

			pthread_mutex_lock(&Flags.mutex);
			SendToOthers(Flags.CurrentControler,out);
			pthread_mutex_unlock(&Flags.mutex);

			free(out);
			out = NULL;

			Flags.isSendGroupFormation = 0;

			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
			printf("\n");
			printf("       Formation Finished\n");
			printf("\n");
			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
			printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
		}

		//finished
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


		//finished
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

		//待续：自由编队，计算agent数量，决策。
	}
}


//决策编队的任务
void DecisionMission(int iCount)
{

	int ii;
	int rander;
//	char str[2];
	if(iCount == 0)
	{
		//友方或者中立
		for(ii = 0; ii < TeamCount; ii ++)
		{
			//把没有弟请的侦查任务 都给 编队任务设为FREE，即：不攻击，直接往前飞。
			//这个过程是先赋值给Formation结构体中的各个变量，然后把赋值完成后的东西加载到链表中。（注意，赋值前先清空）
			memset(&listItem[ii],0,sizeof(struct Formation));
//			memset(str,0,sizeof(str));

			strncpy(listItem[ii].DecisionResult,"FREE",strlen("FREE"));
			listItem[ii].TeamID = ii+1;
			Object = CreateObjectsInArrayJson(ii);
			Arrays = AddObjectsToArray(Arrays, Object);

		}


	}else{
		//敌人

		//首先需要通过由PC发过来的编队结构信息结构体（即MajorSaved存储的信息），
		//来计算出防御能力最强的，和攻击能力最强的编队。（这里需要一个比较的函数）
		int id , max = 0;

		for(ii = 0; ii < TeamCount; ii++)
		{
			if( majorSaved[ii].AttackAbility > max)
			{
				max = majorSaved[ii].AttackAbility;
				id = majorSaved[ii].TeamID;
			}
		}
		memset(&listItem[id-1],0,sizeof(struct Formation));
		listItem[id-1].TeamID = id;
		listItem[id-1].flags = 1;
		strncpy(listItem[id-1].DecisionResult,"ATTACK",strlen("ATTACK"));


		max = 0;
		for(ii = 0; ii < TeamCount; ii++)
		{
			if( majorSaved[ii].DefenseAbility > max)
			{
				max = majorSaved[ii].DefenseAbility;
				id = majorSaved[ii].TeamID;
			}
		}
		memset(&listItem[id-1],0,sizeof(struct Formation));
		listItem[id-1].TeamID = id;
		listItem[id-1].flags = 1;
		strncpy(listItem[id-1].DecisionResult,"BREAKOUT",strlen("BREAKOUT"));


		//其他的编队无所谓，其他的编队采取随机选择编队的情况。（这里需要一个随机的函数）
		//选完之后把编队号和编队任务放入到listItem中。
		for(ii = 0; ii < TeamCount; ii++)
		{
			if(!listItem[ii].flags)
			{
				srand(time(NULL));
				rander = rand()%4+1;//有三种结构编队（这里取出来的是1-4的随机数），这里取4的余数是为了让 "攻击" 任务类型概率大一些
 				switch(rander)
				{
				case 1:
					strncpy(listItem[ii].DecisionResult, "BREAKOUT",strlen("BREAKOUT"));
					break;
				case 2:
					strncpy(listItem[ii].DecisionResult, "ATTACK",strlen("ATTACK"));
					break;
				case 3:
					strncpy(listItem[ii].DecisionResult, "CIRCULITY",strlen("CIRCULITY"));
					break;
				default:
					strncpy(listItem[ii].DecisionResult, "ATTACK",strlen("ATTACK"));
					break;

				}
				listItem[ii].TeamID = ii+1;

			}
		}
		//做一个循环，把listItem数组中的信息全部打包给json。

		for(ii = 0; ii < TeamCount; ii ++)
		{
			Object = CreateObjectsInArrayJson(ii);
			Arrays = AddObjectsToArray(Arrays, Object);
		}
		//结束
	}

}


cJSON * CreateObjectsInArrayJson(int iCount)
{
	cJSON * object = NULL;

	object = cJSON_CreateObject();

	cJSON_AddNumberToObject(object, "TeamID", listItem[iCount].TeamID );
	cJSON_AddStringToObject(object, "DecisionResult",listItem[iCount].DecisionResult);
//	cJSON_AddStringToObject(object, "Shape", listItem.Shape);

	return object;

}


cJSON * AddObjectsToArray( cJSON *Arrays, cJSON *Object)
{

	if (Arrays == NULL)
		Arrays = cJSON_CreateArray();
	cJSON_AddItemToArray(Arrays, Object);
	return Arrays;

}


char *MajorCreateMissionJson(char *Type)
{
	char  *out  = NULL;
	cJSON *root = NULL;

	root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "Type", Type);
	cJSON_AddStringToObject(root, "CmdID", getCommandID());
	cJSON_AddStringToObject(root, "Time", setLocalTime());
	cJSON_AddStringToObject(root, "Broadcast", GROUP);
	cJSON_AddStringToObject(root, "NeedAnswer", NO);
	cJSON_AddStringToObject(root, "TargetType", mission.TargetType);
	cJSON_AddNumberToObject(root, "Distance", mission.Distance);
	cJSON_AddNumberToObject(root, "FirePower", mission.FirePower);
	cJSON_AddNumberToObject(root, "ObjectCount", mission.ObjectCount);
	cJSON_AddStringToObject(root, "IsAttack", mission.IsAttack);
	cJSON_AddItemToObject(root,   "DecisionDetails", Arrays);				//检查下格式

	out = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	sResOutput("->->->StatusBuffer : ",out);
	return out;
}






//thread 2 analisis Json
//解析json的工作，放在了每个角色的模块下，虽然解析的工作属于第二个线程。

void  AnalysisMissionJson(char *buffer)
{
	cJSON *root;
	cJSON *item;

	if(!(root = cJSON_Parse(buffer)))
	{
		sNetOutput(">>>[ERR] Analisis Mission json buffer failed in MajorAgent.c"," >>>");
		return;
	}


	if((item = cJSON_GetObjectItem(root, "Type")))
	{
		if (strcasecmp(item -> valuestring, "Mission") == 0)
		{
			item = cJSON_GetObjectItem(root, "Broadcast");
			if((strcasecmp(item->valuestring, status.Character) == 0))
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
						AgentAnswer = CreateMultiplexJson("RecvMissionPC",status.AnswerCmdID);

						pthread_mutex_lock(&Flags.mutex);
						SendToOthers(MAJORPC, AgentAnswer);
						pthread_mutex_unlock(&Flags.mutex);

						free(AgentAnswer);
						AgentAnswer = NULL;
					}
				}

				item = cJSON_GetObjectItem(root, "TargetType");
				strncpy(mission.TargetType, item -> valuestring, strlen(item->valuestring));

				item = cJSON_GetObjectItem(root, "Distance");
				mission.Distance = item -> valueint;

				item = cJSON_GetObjectItem(root, "FirePower");
				mission.FirePower = item -> valueint;

				item = cJSON_GetObjectItem(root, "ObjectCount");
				mission.ObjectCount = item -> valueint;


				Flags.RecvMission = 1;

			}

		}
	}

	cJSON_Delete(root);
	root = NULL;


}

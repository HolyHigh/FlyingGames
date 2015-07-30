/*
 * Global.h
 *
 *  Created on: 2014年7月1日
 *      Author: zhich
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "Utils/cJSON.h"


//#include "MajorAgent.h"
//#include "GroupAgent.h"
//#include "GeneralAgent.h"

/*

pthread_mutex_lock(&mutex);
pthread_mutex_unlock(&mutex);
*/
#define MAXARRAYCOUNT	 10					    //数组最大值，即为编队的最大数量
#define NETWORKCARD		 "ra"					//网卡前缀
#define MAXCARD			 10						//网卡数量
//#define IPPREFIX		 "192.168.10."			//ip的前缀
#define SENDINTERVAL	 100000
#define MinZone 		 32
#define MidZone			 128
#define MaxZone			 256
#define DEBUG
#define RELEASE
#define BROADCAST		 "255"
#define	MAJORPC			 "14"  					//PC机ID号

#define NONE			 "NONE"
#define GROUP			 "GroupAgent"
#define GENERAL			 "GeneralAgent"
#define ALL				 "All"
#define MAJOR			 "MajorAgent"
#define YES				 "Yes"
#define NO				 "No"
#define BREAKOUT		 "BREAKOUT"
#define ATTACK			 "ATTACK"
#define CIRCUITY		 "CIRCUITY"


typedef struct Equipment
{
	int		   	  AirToAir;					//当前主机的空对空导弹
	int			  AirToGround;				//当前主机的空对地导弹
	int			  SupplyLife;				//当前主机的生命值
}Equipment;


typedef struct Status
{
	int			  MyTeamCount;				//这个myteamcout包含簇首，所以程序中判断应答信息的时候要注意-1
	int 		  MajorAgent;				//当前主机下，主控agent的ip号
	int			  GroupAgent;				//当前主机所在组的簇首agent的ip号
	int 		  AgentID;					//当前主机的ip号
	int			  TeamID;					//编队编号，从1开始
	char		  Character[MinZone];		//当前Agent对应的角色，如果属于普通，也属于簇首，那么身份为簇首；如果属于簇首，也是主控，那么为主控
	int			  CharacterID;				//角色的ID
	char		  TeamShape[MinZone];		//当前组的编队形状（主从、平行、链式）
	char		  TeamMission[MinZone];		//当前组的编队任务(自由，突防，攻击，迂回)
	char		  AliveStatus[MinZone];		//当前Agent的存活情况 yes or no
	Equipment	  Equipment;				//当前主机的装备信息
	char		  IsMissionSuccess[MinZone];//当前主机的任何是否成功
	int			  Fule;						//当前主机的燃料
//	char		  Broadcast[MinZone];
	int			  DefenseAbility;			//当前主机所在编队的防守能力
	int			  AttackAbility;			//当前主机所在编队的攻击能力
	char		  SendCmdID[MinZone];		//与AnswerCmdID一道，来为系统中需要响应的信息做一个临时变量中转的作用
	char		  AnswerCmdID[MinZone];		//同上
}Status;

typedef struct Register
{
	int			  isResend;					//是否重新发送注册信息
	int			  GetAnswer;				//是否接收到簇首的响应

}Register;


typedef struct FlagsGroup
{
//	int 		  gAnswerFlag;
	int 		  isSend;					//在每个角色中。是否给pc机发送心跳信息
	int 		  GlobalPcMode;				//代表pc 主控 全局模式
	int 		  GlobalFreeMode;			//代表自由编队模式
	int			  RecvPcMode;				//代表从pc机获得指令的不同，选择不同的模式
	int			  RecvFreeMode;				//同上
	char		  CurrentControler[10];		//当前主控的单元，区分pc和主控agent，为做自主决策做准备
	int			  RecvMission;				//主控标志。为是否接到pc的任务指令
	int			  isBreak;					//main主函数中，一直给pc发送指令，此标志表示是否跳出循环
	int			  isSendGroupReport;		//接收组内完整个数的注册信息后，是否给pc发送初始化下发完成的信息
	int			  isSendRegister;			//普通agent标志，是否发送注册信息给簇首
	int			  isSendGroupFormation;		//在groupagent中，是否发送编队完成的信息给pc
	int			  isGroupFormation;			//接受pc的编队指令，判断是否发送给组内编队信息
//	int			  isSendMissionDecision;
	int			  isSendDecisionDown;		//在簇首中，是否下发任务指令
	int			  isSendFinishedDownload;	//在簇首中，是否给pc发送任务下发完成的指令
	int			  isStartMission;				//是否开始任务的指令
	int			  isSendStatus;				//任务侦查模式中，是否给pc发送自身状态参数信息
	int			  isSendStatus2;			//同上
	int			  isStopMission;			//是否结束任务侦查模式指令
	int			  isRestart;				//在每个角色中，是否跳出当前角色的指令，（接受collection之后）
	int			  isReset;					//是否重置系统，重新开始的指令
	Register	  registers;				//注册的标志
	int			  temp;						//临时变量，做判断编队内成员的数量
	pthread_mutex_t 	mutex;
	int			  changeMissionf;
	int			  startMissionf;

}FlagsGroup;


typedef struct Mission
{
	char		 TargetType[MinZone];		//任务的目标类别
	int			 Distance;					//任务的距离
	int 		 FirePower;					//任务目标的火力
	int			 ObjectCount;				//目标的个数
	char		 IsAttack[MinZone];			//对目标是否采取攻击 Yes or No

}Mission;

/*
typedef struct ownFormation
{
	int 	   	 TeamID;
//	char		 Shape[MinZone];
//	char		 Mission[MinZone];

}ownFormation;
*/

//结构体为主控agent定义给各个簇首的编队任务信息。
typedef struct Formation
{
	int 		 TeamID;
	char		 DecisionResult[MinZone];
	int			 flags;
}Formation;

//结构体为主控agent使用，存储整个系统中各编队的攻防能力
typedef struct MajorSaved
{
	int 		TeamID;
	int			DefenseAbility;
	int			AttackAbility;
}MajorSaved;



Formation			listItem[MAXARRAYCOUNT];
MajorSaved			majorSaved[MAXARRAYCOUNT];
//ownFormation		ownformation;
Status				status;
FlagsGroup 			Flags;
Mission				mission;
int					TeamCount;  			//编队的个数，从init中可以获取到值并初始化
char			   *AgentAnswer;			//有一些指令需要对方的响应，对方接收到指令，分析完之后立刻给予响应的buffer
int					tMemberCount;			//组内编队成员数量的临时变量，用以判断是否等于组内成员的个数（组内成员-1），在groupagent中
int					isCallInit;				//重启任务的时候，设置了一个标记以便不调用Init()。
											//如果没有此变量，那么ReInit之后，燃料又重新定义为了初始值
int					isEndMission;			//为已经死亡的节点设置一个标记，这个标记的作用是当死亡时，推出线程，并回到DefineCharactor中
											//由于节点已经死亡，所以发送的Collection和ReInit都无效了。这个时候，只能发送Init重新开始。
int					isWakeup;				//当前死亡的节点，当收到INIT时，要先去改变一些标记信息的值，才能使之后的工作顺利进行
char				IPPREFIX[MinZone];


/***************************************/
//方便调试，iNetOutput用于发布的输出，而iResOutput用于调试的输出
void 		iNetOutput(char *string, int value);
void 		iResOutput(char *string, int value);

void 		sNetOutput(char *string, char *value);
void 		sResOutput(char *string, char *value);
/***************************************/

//清空全局变量的函数
void 		 DestroyValue();
void	 	 gAnswerJson();

//获取本地主机的ip，并返回ip最后一个字节（已转换成int型）
int 		 getLocalAddr();

//获取当前系统的时间
char 		*setLocalTime();

//获取当前系统的时间序列（长整形，以秒为单位），用于判断响应
char 		*getCommandID();



/*************************************/
//int 转 char*
void		 fun(char a[]);
void 		 itoa (int n,char s[]);
void	 	 IntToString(int value, char s[]);
/*************************************/


//创建一个多功能的json信息包，这个信息包有两个行参，一个为类型，另外一个为输入的时间序列。
char 		*CreateMultiplexJson(char *Type, char *cmdid);

//解析初始化json包，并判断是否满足一定的条件，如果满足，那么存入相应的变量中
void   		 AnalysisInitJson(char *buffer);

//编队、侦查、任务都已经分配完毕，这时候pc机开始发送广播，告诉所有agent进行侦查任务
void		 AnalysisStartMissionJson(char *buffer);

//分析判断pc是否给予终止任务模式的指令
void		 AnalysisAdjustJson(char *buffer);

//分析判断pc是否给予存活的agent返回重新选择角色的指令
void 		 AnalysisCollectionJson(char *buffer);

//接受pc的重新初始化的指令
void		 AnalysisReInitJson(char *buffer);

//接受pc重置系统，重新开始的指令
void 		 AnalysisResetJson(char *buffer);

//开始任务模式，这是第三个线程函数的声明
void		 StartMission();

//在任务模式中，创建一个给pc机发送自身状态参数的指令
char 		*CreateMissionStatusJson(int distance);

//外引函数，调用这个函数能够发送给其他节点信息
extern int 	   	  			SendToOthers(char *toAgentID, char *StatusBuffer);

void AdjustClockTime();

void AnalysisChangeMissionJson(char *buffer);

void Init();

#endif /* GLOBAL_H_ */

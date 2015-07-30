/*
 * GroupAgent.h
 *
 *  Created on: 2014年7月1日
 *      Author: zhich
 */

#ifndef GROUPAGENT_H_
#define GROUPAGENT_H_

#include "Global.h"


void 					GroupAgent();
char 	   			   *GroupReportInitJson(char *Type);
char				   *CreateGroupFormationJson();
char				   *CreateGroupReportFormationJson();
char 				   *CreateDecisionDownloadJson();
char				   *CreateFinishedMissionJson();


void			   	    AnalysisRegisterJson(char *buffer);
void					AnalysisFormationJson(char *buffer);
char				   *thread2_creat_register_answer(char *cmdid);
void					AnalysisMissionDecisionJson(char *buffer);


extern int  			SendToOthers(char *toAgentID, char *StatusBuffer);

extern int isEndMission;

extern FlagsGroup 	    Flags;


#endif /* GROUPAGENT_H_ */

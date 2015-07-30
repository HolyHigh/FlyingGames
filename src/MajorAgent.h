/*
 * MajorAgent.h
 *
 *  Created on: 2014年7月1日
 *      Author: zhich
 */

#ifndef MAJORAGENT_H_
#define MAJORAGENT_H_

#include "Global.h"
#include "GroupAgent.h"
//#include "LinkedList.h"



extern Mission			mission;
extern FlagsGroup  		Flags;
extern int				TeamCount;
cJSON		   			*Arrays;
cJSON		  			*Object;
extern Formation		listItem[10];
extern MajorSaved		majorSaved[10];
extern int isEndMission;


void					MajorAgent();
extern int 	   	  	    SendToOthers(char *toAgentID, char *StatusBuffer);
char		 		   *MajorCreateMissionJson(char *Type);
void		  			DecisionMission();
cJSON 				   *CreateObjectsInArrayJson(int iCount);
cJSON				   *AddObjectsToArray( cJSON *Arrays, cJSON *Object);
void				    AnalysisMissionJson(char *buffer);

#endif /* MAJORAGENT_H_ */

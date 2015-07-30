/*
 * GeneralAgent.h
 *
 *  Created on: 2014年7月1日
 *      Author: zhich
 */

#ifndef GENERALAGENT_H_
#define GENERALAGENT_H_


#include "Global.h"

extern int isEndMission;

char 				*CreateRegisterJson();



void				 AnalysisGroupFormationJson(char *buffer);
void				 AnalysisDecisionDownloadJson(char *buffer);


extern void			GeneralAgent();
extern int 	   	  	SendToOthers(char *toAgentID, char *StatusBuffer);


#endif /* GENERALAGENT_H_ */

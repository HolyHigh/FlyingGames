/*
 * Network.h
 *
 *  Created on: 2014年7月1日
 *      Author: zhich
 */

#ifndef NETWORK_H_
#define NETWORK_H_


#include "Global.h"
#include "MajorAgent.h"
#include "GroupAgent.h"
#include "GeneralAgent.h"

#define MAXDATASIZE 4096
#define SERVERPORT	"8000"


typedef struct LNode
{
	char buf[MAXDATASIZE];
	struct LNode* next;
}Link;


extern FlagsGroup	Flags;


void 		RecvFromOthers();
int 		SendToOthers(char *toAgentID, char *StatusBuffer);
void		AnalysisJson(char *buffer);
//void 		udp_with_web();

int 		myselect(int usec, int sockfd);




#endif /* NETWORK_H_ */

/*
 * Network.c
 *
 *  Created on: 2014年7月1日
 *      Author: zhich
 */

#include "Network.h"


int SendToOthers(char *toAgentID, char *StatusBuffer)
{
	int udpsock = 0;
	struct sockaddr_in s;
	short serv_port;
	char ip_addr[30];
//	int so_broadcast;


	memset(ip_addr,0,sizeof(ip_addr));

	udpsock = socket(AF_INET, SOCK_DGRAM, 0);
	if(udpsock < 0)
	{
		sResOutput(">>>[ERR] Create Udp Socket Failed "," >>>");
		return 0;
	}
	serv_port = atoi(SERVERPORT);


	const int opt = 1;
	int nb = 0;
//	nb = setsockopt(udpsock, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));//设置广播选项
	nb = setsockopt(udpsock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));//设置广播选项
	if( nb == -1)
	{
		sResOutput(">>>[ERR] , setsockopt! ",">>>")	;

	}


	memset(&s, 0, sizeof(struct sockaddr_in));
	sprintf(ip_addr, "%s%s", IPPREFIX,toAgentID);

	//设置对方端口信息
	s.sin_family = AF_INET;
	s.sin_port = htons(serv_port);
	s.sin_addr.s_addr = inet_addr(ip_addr);


//	connect(udpsock,(struct sockaddr *) &s,sizeof(s));

//	if((send(udpsock, StatusBuffer, strlen(StatusBuffer), 0)) < 0 )
	if((sendto(udpsock, StatusBuffer, strlen(StatusBuffer), 0,(struct sockaddr *)&s, sizeof(s))) < 0 )
	{
		sResOutput(">>>[ERR] Send Failed "," >>>");
		close(udpsock);
		return 0;
	}
	pthread_mutex_unlock(&Flags.mutex);


	close(udpsock);
	return 1;
}




void RecvFromOthers()
{
	short serv_port;
	int sockfd;
	int recvbytes;
	struct sockaddr_in my_addr;
//	struct sockaddr_in my_addr1;
//	Link* head = NULL;
//	Link* pt = NULL;
	Link* p = NULL;
	int ret = 0;
//	int number = 0;
//	int tempvalue = 0;

	socklen_t fromlen ;




	serv_port = atoi(SERVERPORT);

	//DEBUG OUTPUT TEST
	#ifdef DEBUG
		printf(">>>		Thread 2 Working	>>>\n");
		fflush(stdout);
	#endif
	//DEBUG OUTPUT TEST

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf(">>>	Socket Failed.\n");
		return ;
	}

//	{
//		int on = 1;
//		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
//	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(serv_port);
	my_addr.sin_addr.s_addr = INADDR_ANY;

//	my_addr1.sin_family = AF_INET;
//	my_addr1.sin_port = 0;
//	my_addr1.sin_addr.s_addr = 0;

	if(-1== bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)))
	{
		printf("Bind Failed/");
		return ;
	}

	fromlen = sizeof(my_addr);
	while(1)
	{
		ret = myselect(100000,sockfd);
		if(ret == -1)
		{
//			number++;
//			if(number >= 12)   //一分钟
//				break;
			continue;
		}

//		number = 0;
		p = malloc(sizeof(Link));
		p->buf[0] = '\0';
		p->next = NULL;

		if (0 >= (recvbytes = recvfrom(sockfd, p->buf, MAXDATASIZE, 0,
		   (struct sockaddr*)&my_addr, &fromlen)))
//		if(0 >= (recvbytes = recv(sockfd,p->buf,MAXDATASIZE,0)))
		{
			printf("recv failed.");
			break;
		}
		p->buf[recvbytes] = '\0';
//		printf("receive from port %d: %s\n", my_addr.sin_port, p->buf);
		sResOutput("-->   Receive From Others :",p -> buf );


//////////////////////////////////////////
/////////////////////////////////////////
		//remember free p
		//TO-DO something about answering the NEEDANSWER JSON ZIP

		AnalysisJson(p->buf);

///*
// * 		//如果需要应答，则直接在这里发送应答包
//		if(AgentAnswer != NULL)
//		{
//			tempvalue = sendto(sockfd, AgentAnswer, strlen(AgentAnswer), 0,(struct sockaddr *)&my_addr, fromlen);
//			//tempvalue = send(sockfd, AgentAnswer, strlen(AgentAnswer), 0);
//			printf("%s\n",AgentAnswer);
//			printf("\n***\n**%d\n",tempvalue);
//			free(AgentAnswer);
//			AgentAnswer = NULL;
//		}
//*/

		free(p);
		p = NULL;
/////////////////////////////////////////
/////////////////////////////////////////
//		if(my_addr1.sin_port == 0)
//		{
//			my_addr1.sin_port = my_addr.sin_port;
//			my_addr1.sin_addr.s_addr = my_addr.sin_addr.s_addr;
//			if(NULL == head) head = p;
//		}
//		else if(my_addr1.sin_port == my_addr.sin_port &&
//			my_addr1.sin_addr.s_addr == my_addr.sin_addr.s_addr)
//		{
//			if(NULL == head) head = p;
//			else
//			{
//				pt = head;
//				while(pt->next != NULL)
//					pt = pt->next;
//				pt->next = p;
//			}
//		}
//		else
//		{
//			while(head != NULL)
//			{
//				pt = head;
//				if (-1 == sendto(sockfd, pt->buf, strlen(pt->buf), 0,
//		   		(struct sockaddr *)&my_addr, fromlen))
//				{
//					perror("send failed1.");
//					break;
//				}
//				head = head->next;
//				free(pt);
//			}
//
//			if (-1 == sendto(sockfd, p->buf, strlen(p->buf), 0,
//		   	(struct sockaddr *)&my_addr1, fromlen))
//			{
//				perror("send failed2.");
//				break;
//			}
//			free(p);
//		}
	}
	if (-1 != sockfd) close(sockfd);

}


int myselect(int usec, int sockfd)
{
	struct timeval tv;
	fd_set readfds, writefds, exceptfds;
	int receive = 0;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);
	tv.tv_sec = 0;
	tv.tv_usec = usec;

	FD_SET(sockfd, &readfds);
	FD_SET(sockfd, &exceptfds);
	receive = select(sockfd+1, &readfds, NULL, &exceptfds, &tv);
	if(receive == -1)
	{
		sNetOutput(">>>  Select Error","   >>>");
		close(sockfd);
		exit(0);
	}
	else if(receive == 0)
	{
//		sNetOutput(">>>  Receive time out","  >>>");
		return -1;
	}
	else if(receive > 0 && FD_ISSET(sockfd, &readfds) > 0)
		return 0;
	else return -2;

	return 1;
}

//
//void udp_with_web()
//{
//    struct sockaddr_in s_addr;
//    struct sockaddr_in c_addr;
//    int sock;
//    socklen_t addr_len;
//    int len;
//    char buff[1024*5];
//    short serv_port ;
////    char buf[128];
//
////--------
//
//
//    serv_port = atoi(SERVERPORT);
//    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
//        perror("socket");
//
//    }
//    else
//        printf("create socket succeed!!!\n");
//
//    memset(&s_addr, 0, sizeof(struct sockaddr_in));
//
//
//    s_addr.sin_family = AF_INET;
//    s_addr.sin_port = htons(serv_port);
//    s_addr.sin_addr.s_addr = INADDR_ANY;
//
//
//    if ((bind(sock, (struct sockaddr *) &s_addr, sizeof(s_addr))) == -1) {
//        perror("bind");
//
//    }
//    else
//    {
//        printf("bind address to socket succeed.\n");
//        printf("please launch the client of udp to continue...\n");
//    }
//
//
//    addr_len = sizeof(c_addr);
//
// //   int i;
////    int n;
//    while(1)
//    {
//
//    	len = recvfrom(sock, buff, sizeof(buff) - 1, 0,(struct sockaddr *) &c_addr, &addr_len);
//
//        if (len < 0)
//        {
//            printf("receive data from web error!\n");
//
//        }
//
//        else
//        {
//        	buff[len] = '\0';
//
//            printf("UDP:received %s:%d     message:  %s\n",inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port), buff);
//            printf("> * strlen = %d\n",strlen(buff));
//
//
//        }
//
// //       printf("%s\n",buff_from_web);
////        pthread_mutex_unlock(&mutex);
//
//
//        usleep(10000);
//    }
//    close(sock);
//}


void AnalysisJson(char *buffer)
{
	cJSON *res, *item;

	if(!(res = cJSON_Parse(buffer)))
	{
		sNetOutput(">>>[ERR] Analisis json buffer failed in	 analysisjson Network.c "," >>>");
		return;
	}

	if((item = cJSON_GetObjectItem(res, "Type")))
	{

		if (strcasecmp(item -> valuestring, "Init") == 0)
		{
			AnalysisInitJson(buffer);

		}

		if (strcasecmp(item -> valuestring, "Register") == 0)
		{
			AnalysisRegisterJson(buffer);

		}

		if (strcasecmp(item -> valuestring, "Formation") == 0)
		{
			AnalysisFormationJson(buffer);

		}

		if (strcasecmp(item -> valuestring, "GroupFormation") == 0)
		{
			AnalysisGroupFormationJson(buffer);
		}

		if (strcasecmp(item -> valuestring, "RegisterAnswer") == 0)
		{
			item = cJSON_GetObjectItem(res, "CmdID");
			if( strcasecmp(status.SendCmdID, item->valuestring) == 0)
			{
				Flags.registers.GetAnswer = 1;
				printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
				printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
				printf("\n");

				printf("     Initialization and Devided Into Groups Finished\n");
				printf("\n");

				printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");
				printf("^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^ ^^^\n");

			}
			else	sResOutput(">>> [ERR] RegisterAnswer in network.c strcasecmp CmdID "," >>>");
		}


		if (strcasecmp(item -> valuestring, "GroupFormationAnswer") == 0)
		{
			item = cJSON_GetObjectItem(res, "CmdID");
			if( strcasecmp(status.SendCmdID, item->valuestring) == 0)
			{
				if ( ++Flags.temp == (status.MyTeamCount - 1) ){
					Flags.temp = 0;
					Flags.isSendGroupFormation = 1;
				}
			}
			else	sResOutput(">>> [ERR] GroupFormationAnswer in network.c strcasecmp CmdID "," >>>");


		}

		if (strcasecmp(item -> valuestring, "RecvMission") == 0)
		{
			item = cJSON_GetObjectItem(res, "CmdID");
			if( strcasecmp(status.SendCmdID, item->valuestring) == 0)
			{
				if ( ++Flags.temp == (status.MyTeamCount - 1) ){
					Flags.temp = 0;
					Flags.isSendFinishedDownload = 1;
				}
			}
			else	sResOutput(">>> [ERR] RecvMission in network.c strcasecmp CmdID "," >>>");

		}


		if (strcasecmp(item -> valuestring, "Mission") == 0)
		{
			AnalysisMissionJson(buffer);
		}

		if (strcasecmp(item -> valuestring, "MissionDecision") == 0)
		{
			AnalysisMissionDecisionJson(buffer);
		}

		if (strcasecmp(item -> valuestring, "DecisionDownload") == 0)
		{
			AnalysisDecisionDownloadJson(buffer);
		}


		if (strcasecmp(item -> valuestring, "Adjust") == 0)
		{
			AnalysisAdjustJson(buffer);
		}

		if (strcasecmp(item -> valuestring, "Collection") == 0)
		{
			AnalysisCollectionJson(buffer);
		}


		if (strcasecmp(item -> valuestring, "ReInit") == 0)
		{
			AnalysisReInitJson(buffer);
		}

		if (strcasecmp(item -> valuestring, "Reset") == 0)
		{
			AnalysisResetJson(buffer);
		}

		if (strcasecmp(item -> valuestring, "ChangeMission") == 0)
		{
			if ( Flags.changeMissionf ){
				AnalysisChangeMissionJson(buffer);
				Flags.changeMissionf = 0;
			}
		}

		if (strcasecmp(item -> valuestring, "PreAutoFormation") == 0)
		{

		}

		if (strcasecmp(item -> valuestring, "StartMission") == 0)
		{
			if ( Flags.startMissionf ){
				AnalysisStartMissionJson(buffer);
				Flags.startMissionf = 0;
			}
		}


	}

	cJSON_Delete(res);
	res = NULL;
}









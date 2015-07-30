/*
 * LinkedList.h
 *
 *  Created on: 2014年7月4日
 *      Author: zhich
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_


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
//#include "Global.h"
//#include "MajorAgent.h"


#define MinZone 64

//结构体为主控agent定义给各个簇首的编队任务信息。
typedef struct Formation
{
	char		 TeamID[MinZone];
//	char		 Shape[MinZone];
	char		 DecisionResult[MinZone];

}Formation;


typedef Formation 		Item;	//定义数据项类型	//Formation可更改成任何数据类型
typedef struct node    *PNode;	//定义节点指针

//节点的定义
typedef struct node
{
	Item  item;		//数据域
	PNode next;		//链域

}Node,* SList;




//Formation					listItem;	//节点中的一个数据字段。类似于一个对象或者实例，他存储了一个Formation(即：将要分配的任务信息)
										//这里的Formation每用完一次就往链表里插入一次，然后下次用mission之要清空，然后再插入
SList						varlist;	//链表定义的一个变量


int SL_Creat(SList *p_list,int size);
int SL_Insert(SList list,int pos,Item item);
int SL_GetItem(SList list,int pos,Item *p_item);
int SL_Delete(SList list,int pos,Item * p_item);
int SL_SetItem(SList list,int pos,Item item);
int SL_Find(SList list,int *pos,Item item);
int SL_Empty(SList list);
int SL_Size(SList list);
int SL_Clear(SList *p_list);

void ListInterface(int iCount);






#endif /* LINKEDLIST_H_ */

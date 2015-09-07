/*
 * queue.h
 *
 *  Created on: 2015��9��3��
 *      Author: liuhanchong
 */

#ifndef BASICCOMPONENT_QUEUE_H_
#define BASICCOMPONENT_QUEUE_H_

#include <pthread.h>
#include <memory.h>
#include <stdlib.h>
#include "../logmanage/error.h"

typedef struct QueueNode
{
	void *pData;
	int nPrio;
	struct QueueNode *pNext;
	struct QueueNode *pPre;
} QueueNode;

/*ѭ������*/
typedef struct List
{
	int nCurQueueLen;
	int nMaxQueueLen;
	QueueNode *pQueueHead;
	pthread_mutex_t thMutex;
	int nOpenPrio;
} List;

/*�����ڵ�*/
#define BeginTraveNode(list) \
		{\
			int nIndex = 0; \
			QueueNode *pQueueNode = NULL; \
			while (nIndex < GetCurQueueLen(list)) \
			{ \
				pQueueNode = GetNodeForIndex(list, nIndex); \
				if (pQueueNode) \
				{

#define EndTraveNode() \
				} \
				nIndex++; \
			} \
		}

/*��������*/
#define BeginTraveData(list) \
			BeginTraveNode(list) \
				void *pData = pQueueNode->pData; \
				if (pData) \
				{

#define EndTraveData() \
				} \
			EndTraveNode()

/*��ס����*/
#define LockQueue(list) \
{ \
	if (!(list)) \
	{ \
		ErrorInfor("LockQueue", ERROR_ARGNULL); \
	} \
	\
	pthread_testcancel(); \
	pthread_cleanup_push(ReleaseResource, (void *)&((list)->thMutex)); \
	pthread_testcancel(); \
	pthread_mutex_lock(&((list)->thMutex)); \

/*��������*/
#define UnlockQueue(list) \
	if (!(list)) \
	{ \
		ErrorInfor("UnlockQueue", ERROR_ARGNULL); \
	} \
	\
	pthread_mutex_unlock(&((list)->thMutex)); \
	pthread_cleanup_pop(0); \
} \

/*�ӿ�*/
int InitQueue(List *list, int nMaxLen, int nOpenPrio);
int ReleaseQueue(List *list);
int GetCurQueueLen(List *list);
void SetMaxQueueLen(List *list, int nMaxLen);
int GetMaxQueueLen(List *list);
int Empty(List *list);
int Insert(List *list, void *pData, int nPrio);
int DeleteForNode(List *list, QueueNode *pData);
int DeleteForIndex(List *list, int nIndex);
QueueNode *GetHead(List *list);
void *GetDataForIndex(List *list, int nIndex);
void *GetDataForNode(QueueNode *pNode);
QueueNode *GetNodeForIndex(List *list, int nIndex);
int FindNodeIndex(List *list, const QueueNode *pData);
int FindDataIndex(List *list, const void *pData);
int ModifyData(List *list, int nIndex, void *pData);

#endif /* BASICCOMPONENT_QUEUE_H_ */

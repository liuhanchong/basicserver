/*
 * data.h
 *
 *  Created on: 2015��9��4��
 *      Author: liuhanchong
 */

#ifndef COMMUNICATION_DATA_H_
#define COMMUNICATION_DATA_H_

#include "../poolmanage/threadpool.h"
#include "../poolmanage/dbconnpool.h"

typedef struct DataNode
{
	int nSocket;
	void *pData;
	int nDataSize;
} DataNode;

typedef struct Data
{
	List recvDataList;
	List sendDataList;

	ThreadPool recvThreadPool;
	ThreadPool sendThreadPool;

	DBConnPool dbConnPool;

	int nMaxRecvListLen;
	int nMaxSendListLen;

	Thread *pProRecvThread;
	int nProRecvDataLoopSpace;

	Thread *pProSendThread;
	int nProSendDataLoopSpace;
} Data;

static Data data;

/*�ӿ�*/
int InitData();
int ReleaseData();
void ReleaseDataNode(DataNode *pNode);
int InsertRecvDataNode(int nSocket, void *pData, int nDataSize);
int InsertSendDataNode(int nSocket, void *pData, int nDataSize);

/*˽��*/
void *ProcessRecvData(void *pData);
void *ProcessSendData(void *pData);
int InsertDataNode(int nSocket, void *pData, int nDataSize, int nType);

/*����*/
void *TestData(void *pData);

#endif /* COMMUNICATION_DATA_H_ */

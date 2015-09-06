/*
 * data.c
 *
 *  Created on: 2015骞�鏈�鏃�
 *      Author: liuhanchong
 */

#include "data.h"

int InitData()
{
	Ini ini;
	if (InitIni(&ini, "../ini/data.ini", 200) != 1)
	{
		ErrorInfor("InitData", ERROR_READDINI);
		return 0;
	}

	data.nMaxRecvListLen = GetInt(&ini, "DATANUMBER", "MaxRecvListLen", 999); /*鏈�ぇ澶勭悊鏁版嵁鏁伴噺*/
	data.nMaxSendListLen = GetInt(&ini, "DATANUMBER", "MaxSendListLen", 999); /*鏈�ぇ澶勭悊鏁版嵁鏁伴噺*/
	data.nProRecvDataLoopSpace = GetInt(&ini, "DATANUMBER", "ProRecvDataLoopSpace", 1); /*澶勭悊鏁版嵁鏃堕棿闂撮殧*/
	data.nProSendDataLoopSpace = GetInt(&ini, "DATANUMBER", "ProSendDataLoopSpace", 1); /*澶勭悊鏁版嵁鏃堕棿闂撮殧*/

	ReleaseIni(&ini);

	if (InitQueue(&data.recvDataList, data.nMaxRecvListLen, 0) == 0)
	{
		ErrorInfor("InitData-1", ERROR_INITQUEUE);
		return 0;
	}

	// if (InitQueue(&data.sendDataList, data.nMaxSendListLen, 0) == 0)
	// {
	// 	ErrorInfor("InitData-2", ERROR_INITQUEUE);

	// 	ReleaseData();
	// 	return 0;
	// }

	if (CreateThreadPool(&data.recvThreadPool, &data.recvDataList.nCurQueueLen) == 0)
	{
		ErrorInfor("InitData-1", ERROR_CREPOOL);

		ReleaseData();
		return 0;
	}

	// if (CreateThreadPool(&data.sendThreadPool, &data.sendDataList.nCurQueueLen) == 0)
	// {
	// 	ErrorInfor("InitData-2", ERROR_CREPOOL);

	// 	ReleaseData();
	// 	return 0;
	// }

	if (CreateDBConnPool(&data.dbConnPool) == 0)
	{
		ErrorInfor("InitData-3", ERROR_CREPOOL);

		ReleaseData();
		return 0;
	}

	data.pProRecvThread = CreateLoopThread(ProcessRecvData, NULL, data.nProRecvDataLoopSpace);
	if (!data.pProRecvThread)
	{
		ErrorInfor("InitData-1", ERROR_CRETHREAD);

		ReleaseData();
		return 0;
	}

	// data.pProSendThread = CreateLoopThread(ProcessSendData, NULL, data.nProSendDataLoopSpace);
	// if (!data.pProSendThread)
	// {
	// 	ErrorInfor("InitData-2", ERROR_CRETHREAD);

	// 	ReleaseData();
	// 	return 0;
	// }

	return 1;
}

int ReleaseData()
{
<<<<<<< HEAD
	printf("10\n");
=======
	if (data.pProRecvThread)
	{
		ReleaseThread(data.pProRecvThread);
	}

	if (data.pProSendThread)
	{
		ReleaseThread(data.pProSendThread);
	}

	if (ReleaseThreadPool(&data.recvThreadPool) == 0)
	{
		ErrorInfor("ReleaseData-1", ERROR_RELPOOL);
	}

	if (ReleaseThreadPool(&data.sendThreadPool) == 0)
	{
		ErrorInfor("ReleaseData-2", ERROR_RELPOOL);
	}

	if (ReleaseDBConnPool(&data.dbConnPool) == 0)
	{
		ErrorInfor("ReleaseData-3", ERROR_RELPOOL);
	}
>>>>>>> parent of b5f84a3... 添加调试信息

	/*閬嶅巻闃熷垪鍒楄〃*/
	BeginTraveData(&data.recvDataList);
		ReleaseDataNode((DataNode *)pData);
	EndTraveData();

	if (ReleaseQueue(&data.recvDataList) == 0)
	{
		ErrorInfor("ReleaseData-1", ERROR_RELQUEUE);
	}

	// BeginTraveData(&data.sendDataList);
	// 	ReleaseDataNode((DataNode *)pData);
	// EndTraveData();

<<<<<<< HEAD
	// printf("12\n");

	// if (ReleaseQueue(&data.sendDataList) == 0)
	// {
	// 	ErrorInfor("ReleaseData-2", ERROR_RELQUEUE);
	// }

	printf("7\n");

	if (ReleaseThreadPool(&data.recvThreadPool) == 0)
	{
		ErrorInfor("ReleaseData-1", ERROR_RELPOOL);
	}

	printf("8\n");

	// if (ReleaseThreadPool(&data.sendThreadPool) == 0)
	// {
	// 	ErrorInfor("ReleaseData-2", ERROR_RELPOOL);
	// }

	printf("9\n");

	if (ReleaseDBConnPool(&data.dbConnPool) == 0)
	{
		ErrorInfor("ReleaseData-3", ERROR_RELPOOL);
	}

	if (data.pProRecvThread)
	{
		ReleaseThread(data.pProRecvThread);
	}

	printf("6\n");

	// if (data.pProSendThread)
	// {
	// 	ReleaseThread(data.pProSendThread);
	// }

=======
	if (ReleaseQueue(&data.sendDataList) == 0)
	{
		ErrorInfor("ReleaseData-2", ERROR_RELQUEUE);
	}

>>>>>>> parent of b5f84a3... 添加调试信息
	return 1;
}

void ReleaseDataNode(DataNode *pNode)
{
	if (pNode)
	{
		if (pNode->pData)
		{
			free(pNode->pData);
			pNode->pData = NULL;
		}

		free(pNode);
		pNode = NULL;
	}
}

int InsertRecvDataNode(int nSocket, void *pData, int nDataSize)
{
	return InsertDataNode(nSocket, pData, nDataSize, 1);
}

int InsertSendDataNode(int nSocket, void *pData, int nDataSize)
{
	return InsertDataNode(nSocket, pData, nDataSize, 2);
}

void *ProcessRecvData(void *pData)
{
	LockQueue(&data.recvDataList);

	QueueNode *pQueueNode = (QueueNode *)GetNodeForIndex(&data.recvDataList, 0);
	if (pQueueNode)
	{
		/*姝ゅ鍒嗛厤鐨刣atanode鍐呭瓨绌洪棿闇�鎵ц鐨勭嚎绋嬪嚱鏁拌繘琛岄攢姣�/
		if (ExecuteTask(&data.recvThreadPool, TestData, pQueueNode->pData) == 1)
		{
			DeleteForNode(&data.recvDataList, pQueueNode);
		}
	}

	UnlockQueue(&data.recvDataList);

	return NULL;
}

void *ProcessSendData(void *pData)
{
	LockQueue(&data.sendDataList);

	QueueNode *pQueueNode = (QueueNode *)GetNodeForIndex(&data.sendDataList, 0);
	if (pQueueNode)
	{
		/*姝ゅ鍒嗛厤鐨刣atanode鍐呭瓨绌洪棿闇�鎵ц鐨勭嚎绋嬪嚱鏁拌繘琛岄攢姣�/
		if (ExecuteTask(&data.sendThreadPool, TestData, pQueueNode->pData) == 1)
		{
			DeleteForNode(&data.sendDataList, pQueueNode);
		}
	}

	UnlockQueue(&data.sendDataList);

	return NULL;
}

int InsertDataNode(int nSocket, void *pData, int nDataSize, int nType)
{
	DataNode *pDataNode = (DataNode *)malloc(sizeof(DataNode));
	if (!pDataNode)
	{
		SystemErrorInfor("InsertDataNode");
		return 0;
	}

	pDataNode->pData = pData;
	pDataNode->nSocket = nSocket;
	pDataNode->nDataSize = nDataSize;

	/*鎻掑叆鏁版嵁澶勭悊闃熷垪*/
	if (nType == 1)
	{
		LockQueue(&data.recvDataList);
		Insert(&data.recvDataList, pDataNode, 0);
		UnlockQueue(&data.recvDataList);
	}
	else
	{
		LockQueue(&data.sendDataList);
		Insert(&data.sendDataList, pDataNode, 0);
		UnlockQueue(&data.sendDataList);
	}
	return 1;
}

void *TestData(void *pData)
{
	DataNode *pDataNode = (DataNode *)pData;
	if (pDataNode)
	{
		if(pDataNode->pData)
		{
//			char *pData = (char *)pDataNode->pData;
//			pData[pDataNode->nDataSize - 1] = '\0';
//			printf("INFOR-socket:%d data:%s\n", pDataNode->nSocket, pData);
			DBConnNode *pDBConnNode = GetFreeDBConn(&data.dbConnPool);
			if (pDBConnNode)
			{
				if (ExecuteModify(pDBConnNode->pMySql, "insert into test.message(id, message) values(1, '123')") == 0)
				{
//					printf("1\n");
					ReleaseAccessDBConn(pDBConnNode);
					exit(0);
				}
				else
				{
//					printf("2\n");
				}
				ReleaseAccessDBConn(pDBConnNode);
			}
			else
			{
				printf("娌℃湁绌洪棽鐨勮繛鎺n");
			}
		}

		ReleaseDataNode(pDataNode);
		pDataNode = NULL;
	}

	return NULL;
}

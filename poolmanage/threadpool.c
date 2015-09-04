/*
 * threadpool.c
 *
 *  Created on: 2015年9月4日
 *      Author: liuhanchong
 */

#include "threadpool.h"

int CreateThreadPool(ThreadPool *pThreadQueue)
{
	if (!pThreadQueue)
	{
		ErrorInfor("CreateThreadPool", ERROR_ARGNULL);
		return 0;
	}

	Ini ini;
	if (InitIni(&ini, "/Users/liuhanchong/Documents/workspace/comm_server/ini/threadpool.ini", 200) != 1)
	{
		ErrorInfor("CreateThreadPool", ERROR_READDINI);
		return 0;
	}

	pThreadQueue->nMaxThreadNumber = GetInt(&ini, "POOLNUMBER", "MaxThreadNumber", 99);
	pThreadQueue->nCoreThreadNumber = GetInt(&ini, "POOLNUMBER", "CoreThreadNumber", 29);

	pThreadQueue->nAccOverTime = GetInt(&ini, "ACCOVERTIME", "AccOverTime", 1700);/*线程未使用时间超时*/
	pThreadQueue->nAccThreadLoopSpace = GetInt(&ini, "ACCOVERTIME", "AccThreadLoopSpace", 500);/*超时访问线程时候的判断间隔*/

	pThreadQueue->nAddThreadNumber = GetInt(&ini, "ADDTHREAD", "AddThreadNumber", 4);/*增加线程时候增加的个数*/
	pThreadQueue->nAddThreadLoopSpace = GetInt(&ini, "ADDTHREAD", "AddThreadLoopSpace", 50);/*增加线程时候的判断间隔*/

	pThreadQueue->nExeThreadOverTime = GetInt(&ini, "EXEOVERTIME", "ExeThreadOverTime", 170);/*执行线程的时间超时*/
	pThreadQueue->nExeThreadLoopSpace = GetInt(&ini, "EXEOVERTIME", "ExeThreadLoopSpace", 9);/*执行线程的判断间隔*/

	ReleaseIni(&ini);

	pThreadQueue->pTaskQueueLength = NULL;

	if (InitQueue(&pThreadQueue->threadList, pThreadQueue->nMaxThreadNumber, 0) == 0)
	{
		ErrorInfor("CreateThreadPool", ERROR_INITQUEUE);
		return 0;
	}

	if (CreateMulThread(pThreadQueue, pThreadQueue->nCoreThreadNumber) == 0)
	{
		ReleaseThreadPool(pThreadQueue);

		ErrorInfor("CreateThreadPool", ERROR_CREPOOL);
		return 0;
	}

	pThreadQueue->pDynAddThread = CreateLoopThread(AddThreadDynamic, pThreadQueue, pThreadQueue->nAddThreadLoopSpace);
	if (!pThreadQueue->pDynAddThread)
	{
		ReleaseThreadPool(pThreadQueue);

		ErrorInfor("CreateThreadPool-1", ERROR_CRETHREAD);
		return 0;
	}

	pThreadQueue->pFreeOvertimeThread = CreateLoopThread(FreeThreadAccess, pThreadQueue, pThreadQueue->nAccThreadLoopSpace);
	if (!pThreadQueue->pFreeOvertimeThread)
	{
		ReleaseThreadPool(pThreadQueue);

		ErrorInfor("CreateThreadPool-2", ERROR_CRETHREAD);
		return 0;
	}

	pThreadQueue->pExecuteOvertimeThread = CreateLoopThread(FreeThreadExecute, pThreadQueue, pThreadQueue->nExeThreadLoopSpace);
	if (!pThreadQueue->pExecuteOvertimeThread)
	{
		ReleaseThreadPool(pThreadQueue);

		ErrorInfor("CreateThreadPool-3", ERROR_CRETHREAD);
		return 0;
	}

	return 1;
}

int ReleaseThreadPool(ThreadPool *pThreadQueue)
{
	if (!pThreadQueue)
	{
		ErrorInfor("ReleaseThreadPool", ERROR_ARGNULL);
		return 0;
	}

	if (pThreadQueue->pDynAddThread)
	{
		ReleaseThread(pThreadQueue->pDynAddThread);
	}

	if (pThreadQueue->pFreeOvertimeThread)
	{
		ReleaseThread(pThreadQueue->pFreeOvertimeThread);
	}

	if (pThreadQueue->pExecuteOvertimeThread)
	{
		ReleaseThread(pThreadQueue->pExecuteOvertimeThread);
	}

	LockQueue(&pThreadQueue->threadList);

	/*遍历队列列表*/
	BeginTraveData(&pThreadQueue->threadList);
		ReleaseThreadNode((ThreadNode *)pData);
	EndTraveData();

	UnlockQueue(&pThreadQueue->threadList);

	ReleaseQueue(&pThreadQueue->threadList);

	return 1;
}

int GetFreeThreadNumber(ThreadPool *pThreadQueue)
{
	if (!pThreadQueue)
	{
		ErrorInfor("GetFreeThreadNumber", ERROR_ARGNULL);
		return 0;
	}

	int nCount = 0;
	/*遍历队列列表*/
	LockQueue(&pThreadQueue->threadList);

	BeginTraveData(&pThreadQueue->threadList);
		if (IsResume(((ThreadNode *)pData)->pThread) == 0)
		{
			nCount++;
		}
	EndTraveData();

	UnlockQueue(&pThreadQueue->threadList);

	return nCount;
}

ThreadNode *GetFreeThread(ThreadPool *pThreadQueue)
{
	if (!pThreadQueue)
	{
		ErrorInfor("GetFreeThread", ERROR_ARGNULL);
		return 0;
	}

	ThreadNode *pThreadNode = NULL;
	/*遍历队列列表*/
	LockQueue(&pThreadQueue->threadList);

	BeginTraveData(&pThreadQueue->threadList);
		pThreadNode = (ThreadNode *)pData;
		if (IsResume(pThreadNode->pThread) == 0)
		{
			pThreadNode->tmAccessTime = time(NULL);
			break;
		}
		pThreadNode = NULL;
	EndTraveData();

	UnlockQueue(&pThreadQueue->threadList);

	return pThreadNode;
}

void ReleaseThreadNode(ThreadNode *pThreadNode)
{
	if (pThreadNode)
	{
		if (pThreadNode->pThread)
		{
			ReleaseThread(pThreadNode->pThread);
		}

		free(pThreadNode);
		pThreadNode = NULL;
	}
}

int ExecuteTask(ThreadPool *pThreadQueue, void *(*Fun)(void *), void *pData)
{
	if (!pThreadQueue)
	{
		ErrorInfor("ExecuteTask", ERROR_ARGNULL);
		return 0;
	}

	ThreadNode *pThreadNode = GetFreeThread(pThreadQueue);
	if (pThreadNode)
	{
		/*设置线程执行的函数和所需要的数据*/
		SetThreadExecute(pThreadNode->pThread, Fun, pData);

		if (ResumeThread(pThreadNode->pThread) == 1)
		{
			pThreadNode->tmExeTime = time(NULL);
			return 1;
		}
	}

	return 0;
}

void SetTaskQueueLength(ThreadPool *pThreadQueue, int *pTaskQueueLength)
{
	pThreadQueue->pTaskQueueLength = pTaskQueueLength;
}

void *AddThreadDynamic(void *pData)
{
	ThreadPool *pThreadQueue = (ThreadPool *)pData;
	if (!pThreadQueue)
	{
		ErrorInfor("AddThreadDynamic", ERROR_TRANTYPE);
		return NULL;
	}

	LockQueue(&pThreadQueue->threadList);

	if (GetFreeThreadNumber(pThreadQueue) == 0)
	{
		if (pThreadQueue->pTaskQueueLength != NULL)
		{
			if (*pThreadQueue->pTaskQueueLength > 10)//目前设定为10个
			{
				CreateMulThread(pThreadQueue, pThreadQueue->nAddThreadNumber);
			}
		}
	}

	UnlockQueue(&pThreadQueue->threadList);

	return NULL;
}

void *FreeThreadAccess(void *pData)
{
	ThreadPool *pThreadQueue = (ThreadPool *)pData;
	if (!pThreadQueue)
	{
		ErrorInfor("FreeThreadAccess", ERROR_TRANTYPE);
		return NULL;
	}

	/*遍历队列列表*/
	time_t tmCurTime = 0;
	ThreadNode *pThreadNode = NULL;

	LockQueue(&pThreadQueue->threadList);

	/*当前线程超过核心线程数删除*/
	if (GetCurQueueLen(&pThreadQueue->threadList) > pThreadQueue->nCoreThreadNumber)
	{
		BeginTraveData(&pThreadQueue->threadList);
			pThreadNode = (ThreadNode *)pData;
			tmCurTime = time(NULL);
			if ((IsResume(pThreadNode->pThread) == 0) && ((tmCurTime - pThreadNode->tmAccessTime) >= pThreadQueue->nAccOverTime))
			{
				ReleaseThreadNode(pThreadNode);
				DeleteForNode(&pThreadQueue->threadList, pQueueNode);
			}
		EndTraveData();
	}

	UnlockQueue(&pThreadQueue->threadList);

	return NULL;
}

void *FreeThreadExecute(void *pData)
{
	ThreadPool *pThreadQueue = (ThreadPool *)pData;
	if (!pThreadQueue)
	{
		ErrorInfor("FreeThreadExecute", ERROR_TRANTYPE);
		return NULL;
	}

	/*遍历队列列表*/
	time_t tmCurTime = 0;
	ThreadNode *pThreadNode = NULL;

	LockQueue(&pThreadQueue->threadList);

	BeginTraveData(&pThreadQueue->threadList);
	pThreadNode = (ThreadNode *)pData;
		tmCurTime = time(NULL);
		if ((IsResume(pThreadNode->pThread) == 0) && ((tmCurTime - pThreadNode->tmExeTime) >= pThreadQueue->nExeThreadOverTime))
		{
			ReleaseThreadNode(pThreadNode);
			DeleteForNode(&pThreadQueue->threadList, pQueueNode);
			InsertThread(pThreadQueue);
		}
	EndTraveData();

	UnlockQueue(&pThreadQueue->threadList);

	return NULL;
}

int CreateMulThread(ThreadPool *pThreadQueue, int nNumber)
{
	if (!pThreadQueue)
	{
		ErrorInfor("CreateMulThread", ERROR_ARGNULL);
		return 0;
	}

	while ((nNumber--) > 0)
	{
		if (InsertThread(pThreadQueue) == 0)
		{
			ErrorInfor("CreateMulThread", ERROR_CRETHREAD);
		}
	}
	return 1;
}

int InsertThread(ThreadPool *pThreadQueue)
{
	if (!pThreadQueue)
	{
		ErrorInfor("InsertThread", ERROR_ARGNULL);
		return 0;
	}

	if (GetCurQueueLen(&pThreadQueue->threadList) >= GetMaxQueueLen(&pThreadQueue->threadList))
	{
		ErrorInfor("InsertThread", ERROR_OUTQUEUE);
		return 0;
	}

	ThreadNode *pThreadNode = (ThreadNode *)malloc(sizeof(ThreadNode));
	if (!pThreadNode)
	{
		SystemErrorInfor("InsertThread");
		return 0;
	}

	Thread *pThread = CreateSingleThread(DefaultThreadFun, pThreadNode);
	if (!pThread)
	{
		free(pThreadNode);
		pThreadNode = NULL;

		ErrorInfor("InsertThread", ERROR_CRETHREAD);
		return 0;
	}

	pThreadNode->pThread = pThread;
	pThreadNode->tmAccessTime = time(NULL);
	pThreadNode->tmExeTime = time(NULL);

	int nRet = Insert(&pThreadQueue->threadList, (void *)pThreadNode, 0);
	if (!nRet)
	{
		ReleaseThreadNode(pThreadNode);

		ErrorInfor("InsertThread", ERROR_INSEQUEUE);
		return 0;
	}

	return 1;
}

void *DefaultThreadFun(void *pData)
{
	return NULL;
}

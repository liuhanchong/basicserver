/*
 * threadpool.c
 *
 *  Created on: 2015年9月4日
 *      Author: liuhanchong
 */

#include "threadpool.h"

int CreateThreadPool()
{
	Ini ini;
	if (InitIni(&ini, "/Users/liuhanchong/Documents/workspace/comm_server/ini/threadpool.ini", 200) != 1)
	{
		ErrorInfor("CreateThreadPool", ERROR_READDINI);
		return 0;
	}

	threadQueue.nMaxThreadNumber = GetInt(&ini, "POOLNUMBER", "MaxThreadNumber", 99);
	threadQueue.nCoreThreadNumber = GetInt(&ini, "POOLNUMBER", "CoreThreadNumber", 29);

	threadQueue.nAccOverTime = GetInt(&ini, "ACCOVERTIME", "AccOverTime", 1700);/*线程未使用时间超时*/
	threadQueue.nAccThreadLoopSpace = GetInt(&ini, "ACCOVERTIME", "AccThreadLoopSpace", 500);/*超时访问线程时候的判断间隔*/

	threadQueue.nAddThreadNumber = GetInt(&ini, "ADDTHREAD", "AddThreadNumber", 4);/*增加线程时候增加的个数*/
	threadQueue.nAddThreadLoopSpace = GetInt(&ini, "ADDTHREAD", "AddThreadLoopSpace", 50);/*增加线程时候的判断间隔*/

	threadQueue.nExeThreadOverTime = GetInt(&ini, "EXEOVERTIME", "ExeThreadOverTime", 170);/*执行线程的时间超时*/
	threadQueue.nExeThreadLoopSpace = GetInt(&ini, "EXEOVERTIME", "ExeThreadLoopSpace", 9);/*执行线程的判断间隔*/

	ReleaseIni(&ini);

	threadQueue.pTaskQueueLength = NULL;

	if (InitQueue(&threadQueue.threadList, threadQueue.nMaxThreadNumber, 0) == 0)
	{
		ErrorInfor("CreateThreadPool", ERROR_INITQUEUE);
		return 0;
	}

	if (CreateMulThread(threadQueue.nCoreThreadNumber) == 0)
	{
		ErrorInfor("CreateThreadPool", ERROR_CREPOOL);
		return 0;
	}

	threadQueue.pDynAddThread = CreateLoopThread(AddThreadDynamic, NULL, threadQueue.nAddThreadLoopSpace);
	if (!threadQueue.pDynAddThread)
	{
		ErrorInfor("CreateThreadPool-1", ERROR_CRETHREAD);
		return 0;
	}

	threadQueue.pFreeOvertimeThread = CreateLoopThread(FreeThreadAccess, NULL, threadQueue.nAccThreadLoopSpace);
	if (!threadQueue.pFreeOvertimeThread)
	{
		ErrorInfor("CreateThreadPool-2", ERROR_CRETHREAD);
		return 0;
	}

	threadQueue.pExecuteOvertimeThread = CreateLoopThread(FreeThreadExecute, NULL, threadQueue.nExeThreadLoopSpace);
	if (!threadQueue.pExecuteOvertimeThread)
	{
		ErrorInfor("CreateThreadPool-3", ERROR_CRETHREAD);
		return 0;
	}

	return 1;
}

int ReleaseThreadPool()
{
	if (threadQueue.pDynAddThread)
	{
		ReleaseThread(threadQueue.pDynAddThread);
	}

	if (threadQueue.pFreeOvertimeThread)
	{
		ReleaseThread(threadQueue.pFreeOvertimeThread);
	}

	if (threadQueue.pExecuteOvertimeThread)
	{
		ReleaseThread(threadQueue.pExecuteOvertimeThread);
	}

	LockQueue(&threadQueue.threadList);

	/*遍历队列列表*/
	BeginTraveData(&threadQueue.threadList);
		ReleaseThreadNode((ThreadNode *)pData);
	EndTraveData();

	UnlockQueue(&threadQueue.threadList);

	ReleaseQueue(&threadQueue.threadList);

	return 1;
}

int GetFreeThreadNumber()
{
	int nCount = 0;
	/*遍历队列列表*/
	LockQueue(&threadQueue.threadList);

	BeginTraveData(&threadQueue.threadList);
		if (IsResume(((ThreadNode *)pData)->pThread) == 0)
		{
			nCount++;
		}
	EndTraveData();

	UnlockQueue(&threadQueue.threadList);

	return nCount;
}

ThreadNode *GetFreeThread()
{
	ThreadNode *pThreadNode = NULL;
	/*遍历队列列表*/
	LockQueue(&threadQueue.threadList);

	BeginTraveData(&threadQueue.threadList);
		pThreadNode = (ThreadNode *)pData;
		if (IsResume(pThreadNode->pThread) == 0)
		{
			pThreadNode->tmAccessTime = time(NULL);
			break;
		}
		pThreadNode = NULL;
	EndTraveData();

	UnlockQueue(&threadQueue.threadList);

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

int ExecuteTask(void *(*Fun)(void *), void *pData)
{
	ThreadNode *pThreadNode = GetFreeThread();
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

void SetTaskQueueLength(int *pTaskQueueLength)
{
	threadQueue.pTaskQueueLength = pTaskQueueLength;
}

void *AddThreadDynamic(void *pData)
{
	LockQueue(&threadQueue.threadList);

	if (GetFreeThreadNumber() == 0)
	{
		if (threadQueue.pTaskQueueLength != NULL)
		{
			if (*threadQueue.pTaskQueueLength > 10)//目前设定为10个
			{
				CreateMulThread(threadQueue.nAddThreadNumber);
			}
		}
	}

	UnlockQueue(&threadQueue.threadList);

	return NULL;
}

void *FreeThreadAccess(void *pData)
{
	/*遍历队列列表*/
	time_t tmCurTime = 0;
	ThreadNode *pThreadNode = NULL;

	LockQueue(&threadQueue.threadList);

	/*当前线程超过核心线程数删除*/
	if (GetCurQueueLen(&threadQueue.threadList) > threadQueue.nCoreThreadNumber)
	{
		BeginTraveData(&threadQueue.threadList);
			pThreadNode = (ThreadNode *)pData;
			tmCurTime = time(NULL);
			if ((IsResume(pThreadNode->pThread) == 0) && ((tmCurTime - pThreadNode->tmAccessTime) >= threadQueue.nAccOverTime))
			{
				ReleaseThreadNode(pThreadNode);
				DeleteForNode(&threadQueue.threadList, pQueueNode);
			}
		EndTraveData();
	}

	UnlockQueue(&threadQueue.threadList);

	return NULL;
}

void *FreeThreadExecute(void *pData)
{
	/*遍历队列列表*/
	time_t tmCurTime = 0;
	ThreadNode *pThreadNode = NULL;

	LockQueue(&threadQueue.threadList);

	BeginTraveData(&threadQueue.threadList);
	pThreadNode = (ThreadNode *)pData;
		tmCurTime = time(NULL);
		if ((IsResume(pThreadNode->pThread) == 0) && ((tmCurTime - pThreadNode->tmExeTime) >= threadQueue.nExeThreadOverTime))
		{
			ReleaseThreadNode(pThreadNode);
			DeleteForNode(&threadQueue.threadList, pQueueNode);
			InsertThread();
		}
	EndTraveData();

	UnlockQueue(&threadQueue.threadList);

	return NULL;
}

int CreateMulThread(int nNumber)
{
	while ((nNumber--) > 0)
	{
		if (InsertThread() == 0)
		{
			ErrorInfor("CreateMulThread", ERROR_CRETHREAD);
		}
	}
	return 1;
}

int InsertThread()
{
	if (GetCurQueueLen(&threadQueue.threadList) >= GetMaxQueueLen(&threadQueue.threadList))
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

	int nRet = Insert(&threadQueue.threadList, (void *)pThreadNode, 0);
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

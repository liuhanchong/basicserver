/*
 * log.c
 *
 *  Created on: 2015年9月6日
 *      Author: liuhanchong
 */

int InitLog(Log *pLog)
{
	if (!pLog)
	{
		ErrorInfor("InitLog", ERROR_ARGNULL);
		return 0;
	}

	Ini ini;
	if (InitIni(&ini, "../ini/log.ini", 200) != 1)
	{
		ErrorInfor("InitLog", ERROR_READDINI);
		return 0;
	}

	pLog->nMaxLogListLength = GetInt(&ini, "LOG", "MaxLogListLength", 9999);
	pLog->nProLogLoopSpace = GetInt(&ini, "LOG", "ProLogLoopSpace", 0);

	ReleaseIni(&ini);

	if (InitQueue(&pLog->logList, pLog->nMaxLogListLength, 0) == 0)
	{
		ErrorInfor("InitLog", ERROR_INITQUEUE);
		return 0;
	}


	if (CreateThreadPool(&pLog->logThreadPool, &pLog->logList.nCurQueueLen) == 0)
	{
		ErrorInfor("InitLog", ERROR_CREPOOL);

		ReleaseLog(pLog);
		return 0;
	}

	pLog->pProLogThread = CreateLoopThread(ProcessLog, pLog, pLog->nProLogLoopSpace);
	if (!pLog->pProLogThread)
	{
		ErrorInfor("InitLog", ERROR_CRETHREAD);

		ReleaseLog(pLog);
		return 0;
	}
}

int ReleaseLog(Log *pLog)
{
	if (!pLog)
	{
		ErrorInfor("ReleaseLog", ERROR_ARGNULL);
		return 0;
	}

	BeginTraveData(&pLog->logList);
		ReleaseDataNode((LogNode *)pData);
	EndTraveData();

	if (ReleaseQueue(&pLog->logList) == 0)
	{
		ErrorInfor("ReleaseLog", ERROR_RELQUEUE);
	}

	if (ReleaseThreadPool(&pLog->logThreadPool) == 0)
	{
		ErrorInfor("ReleaseLog", ERROR_RELPOOL);
	}

	if (pLog->pProLogThread)
	{
		ReleaseThread(pLog->pProLogThread);
	}
}

int WriteLog(Log *pLog, char *pText, int nType)
{
	if (!pText || nType < 0 || !pLog)
	{
		ErrorInfor("WriteLog", ERROR_ARGNULL);
		return 0;
	}

	LogNode *pLogNode = (LogNode *)malloc(sizeof(LogNode));
	if (!pLogNode)
	{
		SystemErrorInfor("WriteLog-1");
		return 0;
	}

	pLogNode->nType = nType;
	pLogNode->nDataSize = strlen(pText);
	pLogNode->pLog = (char *)malloc(pLogNode->nDataSize);
	if (!pLogNode->pLog)
	{
		free(pLogNode);
		pLogNode = NULL;
		SystemErrorInfor("WriteLog-2");
		return 0;
	}

	memcpy(pLogNode->pLog, pText, pLogNode->nDataSize);

	LockQueue(&pLog->logList);
	Insert(&pLog->logList, pLogNode, 0);
	UnlockQueue(&pLog->logList);

}

void *ProcessLog(void *pData)
{
	return NULL;
}

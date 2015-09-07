/*
 * log.c
 *
 *  Created on: 2015年9月6日
 *      Author: liuhanchong
 */

int InitLog()
{
	Ini ini;
	if (InitIni(&ini, "../ini/log.ini", 200) != 1)
	{
		ErrorInfor("InitLog", ERROR_READDINI);
		return 0;
	}

	log.nMaxLogListLength = GetInt(&ini, "LOG", "MaxLogListLength", 9999);
	log.nProLogLoopSpace = GetInt(&ini, "LOG", "ProLogLoopSpace", 0);

	ReleaseIni(&ini);

	if (InitQueue(&log.logList, log.nMaxLogListLength, 0) == 0)
	{
		ErrorInfor("InitLog", ERROR_INITQUEUE);
		return 0;
	}


	if (CreateThreadPool(&log.logThreadPool, &log.logList.nCurQueueLen) == 0)
	{
		ErrorInfor("InitLog", ERROR_CREPOOL);

		ReleaseLog();
		return 0;
	}

	log.pProLogThread = CreateLoopThread(ProcessLog, NULL, log.nProLogLoopSpace);
	if (!log.pProLogThread)
	{
		ErrorInfor("InitLog", ERROR_CRETHREAD);

		ReleaseLog();
		return 0;
	}

	//开启日志文件
	char pPath[PATH_MAX];
	for (int i = 0; i < LOG_LEVEL; i++)
	{
		sprintf(pPath, "%s%s.log", "./log/debug/", pLevelName[i]);
		log.pFileArray[i] = fopen(pPath, "a+");
		if (!log.pFileArray[i])
		{
			SystemErrorInfor("InitLog");
		}
	}
}

int ReleaseLog()
{
	BeginTraveData(&log.logList);
		ReleaseDataNode((LogNode *)pData);
	EndTraveData();

	if (ReleaseQueue(&log.logList) == 0)
	{
		ErrorInfor("ReleaseLog", ERROR_RELQUEUE);
	}

	if (ReleaseThreadPool(&log.logThreadPool) == 0)
	{
		ErrorInfor("ReleaseLog", ERROR_RELPOOL);
	}

	if (log.pProLogThread)
	{
		ReleaseThread(log.pProLogThread);
	}

	//关闭文件
	for (int i = 0; i < LOG_LEVEL; i++)
	{
		if (log.pFileArray[i])
		{
			fclose(log.pFileArray[i]);
		}
	}
}

int WriteLog(char *pText, int nType)
{
	if (!pText || nType < 0)
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

	LockQueue(&log.logList);
	Insert(&log.logList, pLogNode, 0);
	UnlockQueue(&log.logList);

}

void *ProcessLog(void *pData)
{
	LockQueue(&data.recvDataList);

	QueueNode *pQueueNode = (QueueNode *)GetNodeForIndex(&log.logList, 0);
	if (pQueueNode)
	{
		/*此处分配的node内存空间需要执行的线程函数进行销毁*/
		if (ExecuteTask(&log.logThreadPool, Write, pQueueNode->pData) == 1)
		{
			DeleteForNode(&log.logList, pQueueNode);
		}
	}

	UnlockQueue(&log.logList);

	return NULL;
}

void *Write(void *pData)
{
	LogNode *pLogNode = (LogNode *)pData;
	if (!pLogNode)
	{
		ErrorInfor("Write", ERROR_ARGNULL);
		return NULL;
	}

	FILE *pFile = GetFile(pLogNode->nType);
	if (!pFile)
	{
		ErrorInfor("Write", ERROR_GETFILE);
		return NULL;
	}

	if (pLogNode->pLog)
	{
		fwrite(pLogNode->pLog, pLogNode->nDataSize, 1, pFile);
		fwrite("\r\n", 2, 1, pFile);
	}
}

FILE *GetFile(int nType)
{
	if (nType > 0 && nType < (LOG_LEVEL + 1))
	{
		return log.pFileArray[nType - 1];
	}

	return log.pFileArray[0];
}

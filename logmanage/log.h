/*
 * log.h
 *
 *  Created on: 2015��9��6��
 *      Author: liuhanchong
 */

#ifndef LOGMANAGE_LOG_H_
#define LOGMANAGE_LOG_H_

#include <stdlib.h>
#include "../basiccomponent/queue.h"
#include "../poolmanage/threadpool.h"
#include "error.h"

/*��־����
 * 1 debug		  ������Ϣ
 * 2 log 		  �������־
 * 3 error 		  ������Ϣ
 * 4 system error ϵͳ��Ϣ
 * */

#define LOG_LEVEL 4

typedef struct LogNode
{
	char *pLog;
	int nType;
	int nDataSize;
} LogNode;

typedef struct Log
{
	List logList;
	ThreadPool logThreadPool;
	int nMaxLogListLength;
	Thread *pProLogThread;
	int nProLogLoopSpace;
	FILE *pFileArray[LOG_LEVEL] = {0};
	char *pLevelName[LOG_LEVEL] = {"debug", "log", "error", "syserror"};
} Log;

struct Log log;

int InitLog();
int ReleaseLog();
int WriteLog(char *pText, int nType);

void *ProcessLog(void *pData);
void *Write(void *pData);
FILE *GetFile(int nType);

#endif /* LOGMANAGE_LOG_H_ */

/*
 * log.h
 *
 *  Created on: 2015Äê9ÔÂ6ÈÕ
 *      Author: liuhanchong
 */

#ifndef LOGMANAGE_LOG_H_
#define LOGMANAGE_LOG_H_

#include "../basiccomponent/queue.h"
#include "../poolmanage/threadpool.h"
#include "error.h"

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
} Log;

int InitLog(Log *pLog);
int ReleaseLog(Log *pLog);

int WriteLog(Log *pLog, char *pText, int nType);

void *ProcessLog(void *pData);

#endif /* LOGMANAGE_LOG_H_ */

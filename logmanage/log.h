/*
 * log.h
 *
 *  Created on: 2015年9月6日
 *      Author: liuhanchong
 */

#ifndef LOGMANAGE_LOG_H_
#define LOGMANAGE_LOG_H_

#include <stdlib.h>
#include "../basiccomponent/queue.h"
#include "../poolmanage/threadpool.h"
#include "error.h"

/*日志级别
 * 1 debug		  调试信息
 * 2 log 		  输出的日志
 * 3 error 		  错误信息
 * 4 system error 系统信息
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

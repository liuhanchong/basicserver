/*
 * error.c
 *
 *  Created on: 2015��9��3��
 *      Author: liuhanchong
 */

#include "error.h"

void DebugInfor(char *pDebug)
{
#ifdef DEBUG_SERVER
	if (pDebug)
	{
		printf("Debug: %s!\n", pDebug);
	}
#endif
}

void LogInfor(char *pLog)
{
	if (pLog)
	{
		printf("Log: %s!\n", pLog);
	}
}

void ErrorInfor(char *pFunctionName, char *pError)
{
	if (pFunctionName && pError)
	{
		printf("Error: %s-%s!\n", pFunctionName, pError);
	}
}

void SystemErrorInfor(char *pFunctionName)
{
	if (pFunctionName)
	{
		printf("SystemError: %s-%s!\n", pFunctionName, strerror(errno));
	}
}

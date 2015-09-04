/*
 * servermanage.c
 *
 *  Created on: 2015年8月10日
 *      Author: liuhanchong
 */

#include "servermanage.h"

int Start(int nSize, char **aArgArray)
{
	if (nSize < 1 && nSize > 2)
	{
		ErrorInfor("Start", ERROR_ARGNUMBER);
		return 0;
	}

	//启动服务器
	if (nSize == 1 || strcmp(aArgArray[1], "start") == 0)
	{
		if (CreateShareMemory() == 1)
		{
			if (pShareMemory->nStart != 1)
			{
				if (StartServer() == 1)
				{
					return 1;
				}
			}
		}

		ErrorInfor("Start", ERROR_START);
		return 0;
	}
	//重启服务器
	else if (strcmp(aArgArray[1], "restart") == 0)
	{
		if (CreateShareMemory() == 1)
		{
			if (pShareMemory->nStart == 1)
			{
				if (RestartServer() == 1)
				{
					return 1;
				}
			}
			else
			{
				if (StartServer() == 1)
				{
					return 1;
				}
			}
		}
	}
	//停止服务器
	else if (strcmp(aArgArray[1], "stop") == 0)
	{
		if (CreateShareMemory() == 1)
		{
			if (pShareMemory->nStart == 1)
			{
				kill(pShareMemory->proId, SIGTERM);//关闭服务器

				if (ReleaseShareMemory() == 1)
				{
					return 1;
				}

				ErrorInfor("Start", ERROR_RELSHAMEM);
			}
		}

		ErrorInfor("Start", ERROR_STOP);
		return 0;
	}

	ErrorInfor("Start", ERROR_ARGVALUE);
	return 0;
}

void ProcessMessage()
{
	while (1);
}

int StartServer()
{
	Ini ini;
	if (InitIni(&ini, "/Users/liuhanchong/Documents/workspace/comm_server/ini/servermanage.ini", 200) != 1)
	{
		ErrorInfor("StartServer", ERROR_READDINI);
		return 0;
	}

	char *pIp = GetStringEx(&ini, "SERVER", "IP", "127.0.0.1");
	int nPort = GetInt(&ini, "SERVER", "PORT", 6001);
	int nListenNumber = GetInt(&ini, "SERVER", "LISNUMBER", 10);

	ReleaseIni(&ini);

	nServerSocket = Create(AF_INET, SOCK_STREAM, 0, nPort, pIp);

	free(pIp);
	pIp = NULL;

	if (nServerSocket != -1)
	{
		if (Listen(nServerSocket, nListenNumber) != 0)
		{
			if (Accept(nServerSocket) != 0)
			{
				pShareMemory->proId = getpid();
				pShareMemory->nStart = 1;
				if (signal(SIGTERM, StopServerSignal) != (void *)-1)//激活终止信号
				{
					ProcessMessage();
				}
				return 1;
			}
		}
	}

	ErrorInfor("StartServer", ERROR_START);
	return 0;
}

int StopServer()
{
	if (nServerSocket >= 0)
	{
		if (Close(nServerSocket) == 1)
		{
			return 1;
		}
	}

	ErrorInfor("StopServer", ERROR_STOP);
	return 0;
}

int RestartServer()
{
	system("./comm_server stop");
	sleep(2);
	system("./comm_server start");
	exit(0);
	return 1;
}

int CreateShareMemory()
{
	key_t key = 1905;
	size_t size = sizeof(ShareMemory);
	int nFlag = 0644 | IPC_CREAT;

	nShareMemoryId = shmget(key, size, nFlag);
	if (nShareMemoryId != -1)
	{
		if ((pShareMemory = shmat(nShareMemoryId, NULL, 0)) != (void *)-1)
		{
			return 1;
		}
	}

	ErrorInfor("CreateShareMemory", ERROR_CRESHAMEM);
	return 0;
}

int ReleaseShareMemory()
{
	if (shmdt(pShareMemory) == 0)
	{
		if (shmctl(nShareMemoryId, IPC_RMID, 0) == 0)
		{
			return 1;
		}
	}

	ErrorInfor("ReleaseShareMemory", ERROR_RELSHAMEM);
	return 0;
}

void StopServerSignal(int nSignalType)
{
	StopServer();
	exit(0);
}



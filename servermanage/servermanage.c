/*
 * servermanage.c
 *
 *  Created on: 2015年8月10日
 *      Author: liuhanchong
 */

#include "servermanage.h"

int Start(int nSize, char **aArgArray)
{
	if (nSize < 1 || nSize > 2)
	{
		ErrorInfor("Start", ERROR_ARGNUMBER);
		return 0;
	}

	//启动服务器
	if (nSize == 1 || strcmp(aArgArray[1], "start") == 0)
	{
		if (CreateShareMemory() != 1)
		{
			ErrorInfor("Start", ERROR_CRESHAMEM);
			return 0;
		}

		if (server.pShareMemory->nRun == 1)
		{
			ErrorInfor("Start", ERROR_SERISRUN);
			return 0;
		}

		if (StartServer() != 1)
		{
			ErrorInfor("Start", ERROR_START);
			return 0;
		}

		return 1;
	}

	//重启服务器
	if (strcmp(aArgArray[1], "restart") == 0)
	{
		if (CreateShareMemory() != 1)
		{
			ErrorInfor("Start", ERROR_CRESHAMEM);
			return 0;
		}

		return (server.pShareMemory->nRun == 1) ? RestartServer() : StartServer();
	}

	//停止服务器
	if (strcmp(aArgArray[1], "stop") == 0)
	{
		if (CreateShareMemory() != 1)
		{
			ErrorInfor("Start", ERROR_CRESHAMEM);
			return 0;
		}

		if (server.pShareMemory->nRun != 1)
		{
			ErrorInfor("Start", ERROR_SERNORUN);
			return 0;
		}

		kill(server.pShareMemory->proId, SIGTERM);//关闭服务器
		if (ReleaseShareMemory() != 1)
		{
			ErrorInfor("Start", ERROR_RELSHAMEM);
			return 0;
		}

		return 1;
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
	if (InitLog() != 1)
	{
		ErrorInfor("StartServer", ERROR_INITLOG);
		return 0;
	}

	Ini ini;
	if (InitIni(&ini, "../ini/servermanage.ini", 200) != 1)
	{
		ErrorInfor("StartServer", ERROR_READDINI);
		return 0;
	}

	char *pIp = GetStringEx(&ini, "SERVER", "IP", "127.0.0.1");
	int nPort = GetInt(&ini, "SERVER", "PORT", 6001);
	int nListenNumber = GetInt(&ini, "SERVER", "LISNUMBER", 10);

	ReleaseIni(&ini);

	server.nServerSocket = Create(AF_INET, SOCK_STREAM, 0, nPort, pIp);

	free(pIp);
	pIp = NULL;

	if (server.nServerSocket == -1)
	{
		ErrorInfor("StartServer", ERROR_CRESOCKET);
		return 0;
	}

	if (Listen(server.nServerSocket, nListenNumber) != 1)
	{
		ErrorInfor("StartServer", ERROR_CRELISTEN);
		return 0;
	}

	if (Accept(server.nServerSocket) == 0)
	{
		ErrorInfor("StartServer", ERROR_CREACCEPT);
		return 0;
	}

	server.pShareMemory->proId = getpid();
	server.pShareMemory->nRun = 1;
	if (signal(SIGTERM, StopServerSignal) == (void *)-1)//激活终止信号
	{
		ErrorInfor("StartServer", ERROR_CRESIGNAL);
		return 0;
	}

	ProcessMessage();

	return 1;
}

int StopServer()
{
	if (Close(server.nServerSocket) != 1)
	{
		ErrorInfor("StopServer", ERROR_STOP);
	}

	if (ReleaseLog() != 1)
	{
		ErrorInfor("StartServer", ERROR_RELLOG);
	}

	return 1;
}

int RestartServer()
{
	system("./basicserver stop");

	sleep(2);

	system("./basicserver start");

	exit(0);

	return 1;
}

int CreateShareMemory()
{
	key_t key = KEY;
	size_t size = sizeof(Server);
	int nFlag = 0644 | IPC_CREAT;

	server.nShareMemoryId = shmget(key, size, nFlag);
	if (server.nShareMemoryId == -1)
	{
		ErrorInfor("CreateShareMemory", ERROR_CRESHAMEM);
		return 0;
	}

	if ((server.pShareMemory = (ShareMemory *)shmat(server.nShareMemoryId, NULL, 0)) == (void *)-1)
	{
		ErrorInfor("CreateShareMemory", ERROR_GETSHAMEM);
		return 0;
	}

	return 1;
}

int ReleaseShareMemory()
{
	if (shmdt(server.pShareMemory) != 0)
	{
		ErrorInfor("ReleaseShareMemory", ERROR_CUTSHAMEM);
		return 0;
	}

	if (shmctl(server.nShareMemoryId, IPC_RMID, 0) != 0)
	{
		ErrorInfor("ReleaseShareMemory", ERROR_RELSHAMEM);
		return 0;
	}

	return 1;
}

void StopServerSignal(int nSignalType)
{
	StopServer();
	exit(0);
}



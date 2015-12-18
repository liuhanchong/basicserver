/*
 * servermanage.h
 *
 *  Created on: 2015��8��10��
 *      Author: liuhanchong
 */

#ifndef SERVERMANAGE_SERVERMANAGE_H_
#define SERVERMANAGE_SERVERMANAGE_H_

#include <sys/shm.h>
#include "../logmanage/error.h"
#include "../communication/socket.h"
#include "../logmanage/log.h"

#define KEY 0x19900510

typedef struct ShareMemory
{
	pid_t proId;
	int nRun;
} ShareMemory;

typedef struct Server
{
	ShareMemory *pShareMemory;
	int nServerSocket;
    int nShareMemoryId;
} Server;

static Server server;

/*�ӿ�*/
int Start(int nSize, char **aArgArray);
void ProcessMessage();

/*˽��*/
int StartServer();
int StopServer();
int RestartServer();
int CreateShareMemory();
int ReleaseShareMemory();
void StopServerSignal(int nSignalType);

#endif /* HEADER_SERVERMANAGE_H_ */

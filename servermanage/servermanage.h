/*
 * servermanage.h
 *
 *  Created on: 2015年8月10日
 *      Author: liuhanchong
 */

#ifndef HEADER_SERVERMANAGE_H_
#define HEADER_SERVERMANAGE_H_

#include <sys/shm.h>
#include "../logmanage/error.h"
#include "../communication/socket.h"

typedef struct ShareMemory
{
	pid_t proId;
	int nStart;
} ShareMemory;

static int nServerSocket = -1;
static int nShareMemoryId = -1;
static ShareMemory *pShareMemory = NULL;

/*接口*/
int Start(int nSize, char **aArgArray);
void ProcessMessage();

/*私有*/
int StartServer();
int StopServer();
int RestartServer();
int CreateShareMemory();
int ReleaseShareMemory();
void StopServerSignal(int nSignalType);

#endif /* HEADER_SERVERMANAGE_H_ */

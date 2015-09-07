/*
 * socket.h
 *
 *  Created on: 2015��9��4��
 *      Author: liuhanchong
 */

#ifndef COMMUNICATION_SOCKET_H_
#define COMMUNICATION_SOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include "../basiccomponent/thread.h"
#include "../basiccomponent/queue.h"
#include "../logmanage/error.h"
#include "../basiccomponent/ini.h"
#include "aiox.h"

typedef struct SocketNode
{
	struct sockaddr_in sock_addr;
	int nClientSocket;
	time_t tmAccDateTime;
} SocketNode;

typedef struct Socket
{
	List socketList;
	int nAccThreadLoopSpace;
	int nAccOutTime;
	int nAccOutTimeThreadLoopSpace;/*��ʼ����buffer����*/
	int nServerSocket;
	Thread *pAccThread;
	Thread *pOutTimeThread;
	int nMaxAcceptSocketNumber;
	AioX aiox;
} Socket;

static Socket serverSocket;

/*�ӿ�*/
int Create(int nDomain, int nType, int nProtocol, int nPort, const char *pIp);
int Listen(int nSocket, int nQueSize);
int Accept(int nSocket);
int Close(int nSocket);
void ReleaseSocketNode(SocketNode *pNode);

/*˽��*/
void *AcceptSocket(void *pData);
void *FreeOutTimeSocket(void *pData);

#endif /* COMMUNICATION_SOCKET_H_ */

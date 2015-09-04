/*
 * aiox.c
 *
 *  Created on: 2015年9月4日
 *      Author: liuhanchong
 */

#include "aiox.h"
#include "socket.h"

int CreateAio(AioX *pAio, int nMaxAioQueueLength, int nLoopSpace)
{
	if (!pAio || nMaxAioQueueLength < 1)
	{
		ErrorInfor("CreateAio", ERROR_ARGNULL);
		return 0;
	}

	pAio->nAioId = kqueue();
	if (pAio->nAioId == -1)
	{
		SystemErrorInfor("CreateAio");
		return 0;
	}

	pAio->nMaxAioQueueLength = nMaxAioQueueLength;
	pAio->pEvnetQueue = malloc(nMaxAioQueueLength * sizeof(struct kevent));
	if (!pAio->pEvnetQueue)
	{
		SystemErrorInfor("CreateAio");

		ReleaseAio(pAio);
		return 0;
	}

	pAio->pProAioThread = CreateLoopThread(ProcessAio, pAio, nLoopSpace);
	if (!pAio->pProAioThread)
	{
		ReleaseAio(pAio);

		ErrorInfor("CreateAio", ERROR_CRETHREAD);
		return 0;
	}

	return 1;
}

int ControlAio(int nQueueId, struct kevent *event)
{
	if (kevent(nQueueId, event, 1, NULL, 0, NULL) == -1)
	{
		SystemErrorInfor("ControlAio");
		return 0;
	}
	return 1;
}

int RemoveEvent(int nQueueId, int nFd, int nFilter)
{
	struct kevent event = GetEvent(nFd, nFilter, EV_DELETE, NULL);
	return ControlAio(nQueueId, &event);
}

int AdditionEvent(int nQueueId, int nFd, int nFilter, void *pData)
{
	struct kevent event = GetEvent(nFd, nFilter, EV_ADD, pData);
	return ControlAio(nQueueId, &event);
}

int ModifyEvent(int nQueueId, int nFd, int nFilter, void *pData)
{
	return AdditionEvent(nQueueId, nFd, nFilter, pData);
}

struct kevent GetEvent(int fd, int nFilter, int nFlags, void *pData)
{
	struct kevent event;
	EV_SET(&event, fd, nFilter, nFlags, 0, 0, pData);
	return event;
}

int ReleaseAio(AioX *pAio)
{
	if (!pAio)
	{
		ErrorInfor("ReleaseAio", ERROR_ARGNULL);
		return 0;
	}

	if (pAio->pProAioThread)
	{
		ReleaseThread(pAio->pProAioThread);
	}

	if (pAio->pEvnetQueue)
	{
		free(pAio->pEvnetQueue);
		pAio->pEvnetQueue = NULL;
	}

	if (close(pAio->nAioId) != 0)
	{
		SystemErrorInfor("ReleaseAio");
		return 0;
	}
	return 1;
}

void *ProcessAio(void *pData)
{
	AioX *pAioX = (AioX *)pData;
	if (!pAioX || !pAioX->pEvnetQueue || pAioX->nMaxAioQueueLength < 1)
	{
		ErrorInfor("ProcessAio", ERROR_ARGNULL);
		return NULL;
	}

	/*五秒超时*/
	struct timespec time = {.tv_sec = 5, .tv_nsec = 0};
	int nQueLen = kevent(pAioX->nAioId, NULL, 0, pAioX->pEvnetQueue, pAioX->nMaxAioQueueLength, &time);
	if (nQueLen == -1)
	{
		SystemErrorInfor("ProcessAio");
	}

	struct kevent event;
	for (int i = 0; i < nQueLen; i++)
	{
		event = pAioX->pEvnetQueue[i];

		//读取数据
		if (event.flags & EVFILT_READ)
		{
			Read(&event);
		}
		//写数据
		else if (event.flags & EVFILT_WRITE)
		{
			Write(&event);
		}
		//错误数据
		else if (event.flags & EV_ERROR)
		{
			if (RemoveEvent(pAioX->nAioId, event.ident, EV_ERROR) == 0)
			{
				ErrorInfor("ProcessAio", ERROR_REMEVENT);
			}
			ErrorInfor("ProcessAio", (char *)event.data);
		}
		else
		{
		}
	}

	return NULL;
}

int Read(struct kevent *event)
{
	SocketNode *pSocketNode = (SocketNode *)event->udata;
	if (!pSocketNode)
	{
		return 0;
	}

	int nFd = 0;
	int nReturn = 0;
	nFd = event->ident;

	/*获取节点信息*/
	LockQueue(&serverSocket.socketList);

	if (FindDataIndex(&serverSocket.socketList, pSocketNode) >= 0)
	{
		memset(pSocketNode->data.pData, 0, pSocketNode->nBuffLength);
		pSocketNode->data.nDataSize = recv(nFd, pSocketNode->data.pData, pSocketNode->nBuffLength, 0);

		//读取信息错误
		if (pSocketNode->data.nDataSize == -1)
		{
			SystemErrorInfor("Read");

			if (RemoveEvent(serverSocket.aio.nAioId, nFd, EVFILT_READ) == 0)
			{
				ErrorInfor("Read", ERROR_REMEVENT);
			}
		}
		//对方关闭了套接字
		else if (pSocketNode->data.nDataSize == 0)
		{
			if (RemoveEvent(serverSocket.aio.nAioId, nFd, EVFILT_READ) == 0)
			{
				ErrorInfor("Read", ERROR_DISCONN);
			}
		}
		else
		{
			pSocketNode->tmAccDateTime = time(NULL);

			//将获取到的数据保存
			char *pData = (char *)malloc(pSocketNode->data.nDataSize + 1);
			if (pData)
			{
				/*保存数据*/
				memcpy(pData, pSocketNode->data.pData, pSocketNode->data.nDataSize);
				pData[pSocketNode->data.nDataSize] = '\0';

				InsertRecvDataNode(pSocketNode->nClientSocket, pData, pSocketNode->data.nDataSize);
				nReturn = 1;
			}
			else
			{
				SystemErrorInfor("Read");
			}
		}
	}

	UnlockQueue(&serverSocket.socketList);

	return nReturn;
}

int Write(struct kevent *event)
{
	return 1;
}


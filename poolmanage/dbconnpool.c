/*
 * dbconnpool.c
 *
 *  Created on: 2015年9月4日
 *      Author: liuhanchong
 */

#include "dbconnpool.h"

int CreateDBConnPool()
{
	Ini ini;
	if (InitIni(&ini, "/Users/liuhanchong/Documents/workspace/comm_server/ini/dbconnpool.ini", 200) != 1)
	{
		ErrorInfor("CreateDBConnPool", ERROR_READDINI);
		return 0;
	}

	connPool.nMaxConnNumber = GetInt(&ini, "POOLNUMBER", "MaxConnNumber", 99);
	connPool.nCoreConnNumber = GetInt(&ini, "POOLNUMBER", "CoreConnNumber", 29);

	connPool.nAccOverTime = GetInt(&ini, "ACCOVERTIME", "AccOverTime", 1700);/*连接未使用时间超时*/
	connPool.nAccConnLoopSpace = GetInt(&ini, "ACCOVERTIME", "AccConnLoopSpace", 500);/*超时访问连接时候的判断间隔*/

	connPool.nAddConnNumber = GetInt(&ini, "ADDTHREAD", "AddConnNumber", 4);

	ReleaseIni(&ini);

	if (InitQueue(&connPool.dbConnList, connPool.nMaxConnNumber, 0) == 0)
	{
		ErrorInfor("CreateDBConnPool", ERROR_INITQUEUE);
		return 0;
	}

	if (CreateMulDBConn(connPool.nCoreConnNumber) == 0)
	{
		ErrorInfor("CreateDBConnPool", ERROR_CREPOOL);
		return 0;
	}

	connPool.pFreeOvertimeConn = CreateLoopThread(FreeDBConnAccess, NULL, connPool.nAccConnLoopSpace);
	if (!connPool.pFreeOvertimeConn)
	{
		ErrorInfor("CreateDBConnPool", ERROR_CRETHREAD);
		return 0;
	}

	return 1;
}

int ReleaseDBConnPool()
{
	if (connPool.pFreeOvertimeConn)
	{
		ReleaseThread(connPool.pFreeOvertimeConn);
	}

	LockQueue(&connPool.dbConnList);

	/*遍历队列列表*/
	BeginTraveData(&connPool.dbConnList);
		ReleaseDBConnNode((DBConnNode *)pData);
	EndTraveData();

	UnlockQueue(&connPool.dbConnList);

	ReleaseQueue(&connPool.dbConnList);

	return 1;
}

DBConnNode *GetFreeDBConn()
{
	/*遍历队列列表*/
	DBConnNode *pDBConnNode = NULL;

	LockQueue(&connPool.dbConnList);

	BeginTraveData(&connPool.dbConnList);
		pDBConnNode = (DBConnNode *)pData;
		if (pDBConnNode->nConnection == 0)
		{
			pDBConnNode->nConnection = 1;//状态为已连接
			pDBConnNode->tmAccessTime = time(NULL);
			break;
		}
		pDBConnNode = NULL;
	EndTraveData();

	/*当没有空闲连接，创建新的连接*/
	if (!pDBConnNode)
	{
		if (CreateMulDBConn(connPool.nAddConnNumber) == 0)
		{
			ErrorInfor("GetFreeDBConn", ERROR_CREPOOL);
		}
	}

	UnlockQueue(&connPool.dbConnList);

	return pDBConnNode;
}

int ReleaseAccessDBConn(DBConnNode *pDBConnNode)
{
	pDBConnNode->nConnection = 0;//状态为已连接
	pDBConnNode->tmAccessTime = time(NULL);
	return 1;
}

void ReleaseDBConnNode(DBConnNode *pNode)
{
	if (pNode)
	{
		if (pNode->pMySql)
		{
			CloseDB(pNode->pMySql);
		}

		free(pNode);
		pNode = NULL;
	}
}

void *FreeDBConnAccess(void *pData)
{
	/*遍历队列列表*/
	time_t tmCurTime = 0;
	DBConnNode *pDBConnNode = NULL;

	LockQueue(&connPool.dbConnList);

	//当前线程超过核心线程数删除
	if (GetCurQueueLen(&connPool.dbConnList) > connPool.nCoreConnNumber)
	{
		BeginTraveData(&connPool.dbConnList);
			pDBConnNode = (DBConnNode *)pData;
			tmCurTime = time(NULL);
			if ((pDBConnNode->nConnection == 0) && ((tmCurTime - pDBConnNode->tmAccessTime) >= connPool.nAccOverTime))
			{
				ReleaseDBConnNode(pDBConnNode);
				DeleteForNode(&connPool.dbConnList, pQueueNode);
			}
		EndTraveData();
	}

	UnlockQueue(&connPool.dbConnList);

	return NULL;
}

int CreateMulDBConn(int nNumber)
{
	Ini ini;
	if (InitIni(&ini, "/Users/liuhanchong/Documents/workspace/comm_server/ini/db.ini", 200) != 1)
	{
		ErrorInfor("CreateMulDBConn", ERROR_READDINI);
		return 0;
	}

	char *pHost = GetStringEx(&ini, "MYSQLDB", "Host", "127.0.0.1");
	char *pUser = GetStringEx(&ini, "MYSQLDB", "User", "root");
	char *pPasswd = GetStringEx(&ini, "MYSQLDB", "Passwd", "");
	char *pDB = GetStringEx(&ini, "MYSQLDB", "DB", "test");
	char *pUnixSocket = GetStringEx(&ini, "MYSQLDB", "UnixSocket", "");
	unsigned long lClientFlag = GetInt(&ini, "MYSQLDB", "ClientFlag", 0);
	unsigned int nPort = GetInt(&ini, "MYSQLDB", "Port", 3306);

	ReleaseIni(&ini);

	while ((nNumber--) > 0)
	{
		if (InsertDBConn(pHost, pUser, pPasswd, pDB, pUnixSocket, lClientFlag, nPort) == 0)
		{
			ErrorInfor("CreateMulDBConn", ERROR_CRETHREAD);
		}
	}

	free(pHost);
	pHost = NULL;

	free(pUser);
	pUser = NULL;

	free(pPasswd);
	pPasswd = NULL;

	free(pDB);
	pDB = NULL;

	free(pUnixSocket);
	pUnixSocket = NULL;

	return 1;
}

int InsertDBConn(char *pHost, char *pUser, char *pPasswd, char *pDB, char *pUnixSocket, unsigned long lClientFlag, unsigned int nPort)
{
	if (GetCurQueueLen(&connPool.dbConnList) >= GetMaxQueueLen(&connPool.dbConnList))
	{
		ErrorInfor("InsertDBConn", ERROR_OUTQUEUE);
		return 0;
	}

	DBConnNode *pTmp = (DBConnNode *)malloc(sizeof(DBConnNode));
	if (!pTmp)
	{
		SystemErrorInfor("InsertDBConn");
		return 0;
	}

	MYSQL *pMySql = OpenDB(pHost, pUser, pPasswd, pDB, pUnixSocket, lClientFlag, nPort);
	if (!pMySql)
	{
		free(pTmp);
		pTmp = NULL;

		ErrorInfor("InsertDBConn", ERROR_DBOPEN);
		return 0;
	}

	pTmp->pMySql = pMySql;
	pTmp->tmAccessTime = time(NULL);
	pTmp->nConnection = 0;

	int nRet = Insert(&connPool.dbConnList, (void *)pTmp, 0);
	if (!nRet)
	{
		ReleaseDBConnNode(pTmp);
		pTmp = NULL;

		ErrorInfor("InsertDBConn", ERROR_INSEQUEUE);
		return 0;
	}

	return 1;
}

/*
 * threadpool.h
 *
 *  Created on: 2015年9月4日
 *      Author: liuhanchong
 */

#ifndef POOLMANAGE_THREADPOOL_H_
#define POOLMANAGE_THREADPOOL_H_

#include "../basiccomponent/thread.h"
#include "../basiccomponent/queue.h"
#include "../basiccomponent/ini.h"
#include "../logmanage/error.h"

typedef struct ThreadNode
{
	Thread *pThread;
	time_t tmAccessTime;
	time_t tmExeTime;
} ThreadNode;

typedef struct ThreadPool
{
	List threadList;
	Thread *pDynAddThread;
	Thread *pFreeOvertimeThread;
	Thread *pExecuteOvertimeThread;
	int *pTaskQueueLength;

	int nMaxThreadNumber;
	int nCoreThreadNumber;

	int nAccOverTime;/*线程未使用时间超时*/
	int nAccThreadLoopSpace;/*线程未使用的判断间隔*/

	int nAddThreadNumber;/*增加线程时候增加的个数*/
	int nAddThreadLoopSpace;/*增加线程时候的判断间隔*/

	int nExeThreadOverTime;/*执行线程的时间超时*/
	int nExeThreadLoopSpace;/*执行线程的判断间隔*/
} ThreadPool;

static ThreadPool threadQueue;

/*接口*/
int CreateThreadPool();
int ReleaseThreadPool();
int GetFreeThreadNumber();/*获取空闲线程个数*/
ThreadNode *GetFreeThread();/*获取一个空闲线程*/
void ReleaseThreadNode(ThreadNode *pThreadNode);/*释放线程节点*/
int ExecuteTask(void *(*Fun)(void *), void *pData);/*执行线程*/
void SetTaskQueueLength(int *pTaskQueueLength);/*设置任务队列的长度，其是动态变化的*/

/*私有*/
void *AddThreadDynamic(void *pData);/*动态的添加线程*/
void *FreeThreadAccess(void *pData);/*未访问超时线程*/
void *FreeThreadExecute(void *pData);/*执行超时线程*/
int CreateMulThread(int nNumber);
int InsertThread();
void *DefaultThreadFun(void *pData);

#endif /* POOLMANAGE_THREADPOOL_H_ */

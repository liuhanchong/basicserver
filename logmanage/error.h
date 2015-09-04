/*
 * error.h
 *
 *  Created on: 2015年9月3日
 *      Author: liuhanchong
 */

#ifndef LOGMANAGE_ERROR_H_
#define LOGMANAGE_ERROR_H_

#include <errno.h>
#include <string.h>
#include <stdio.h>

/*错误级别
 * 1 debug		  调试信息
 * 2 log 		  输出的日志
 * 3 error 		  错误信息
 * 4 system error 系统信息
 * */

#define DEBUG_SERVER

/*当前服务器定义的运行错误信息*/
#define ERROR_ARGNULL    ("传递的参数不能为空")
#define ERROR_CRETHREAD  ("创建线程失败")
#define ERROR_CLOTHREAD  ("关闭线程失败")
#define ERROR_DETTHREAD  ("分离线程失败")
#define ERROR_TRANTYPE   ("强制转换数据失败")
#define ERROR_SETMODE    ("设置模式失败")
#define ERROR_PROFUNNULL ("线程的处理函数为空")
#define ERROR_OUTQUEUE   ("超出队列的最大长度")
#define ERROR_INSEQUEUE  ("插入元素失败")
#define ERROR_DELQUEUE   ("删除元素失败")
#define ERROR_EDITQUEUE  ("修改数据失败")
#define ERROR_FILEDDB    ("获取结果集字段失败")
#define ERROR_READDINI 	 ("读取配置文件失败")
#define ERROR_INITQUEUE  ("初始化队列失败")
#define ERROR_CREPOOL 	 ("创建线程池失败")
#define ERROR_DBOPEN 	 ("数据库打开失败")

#define ERR_TYPETRAN ("强制转换数据失败") //数据转换失败
#define ERR_UNLOCK ("释放互斥锁失败") //释放互斥锁失败
#define ERR_CANCELTHREAD ("设置取消线程失败") //取消线程失败
#define ERR_ARGNULL ("传递的参数为空") //参数传递为空
#define ERR_MALLOC ("内存分配失败") //内存分配失败
#define ERR_CLOTHREAD ("线程关闭失败") //线程关闭失败
#define ERR_OUTMAXLEN ("超出队列的最大长度") //队列溢出失败
#define ERR_INSELE ("插入元素失败") //插入元素失败
#define ERR_SOCKET ("Socket初始化失败") //初始化socket失败
#define ERR_BIND ("Socket绑定失败") //绑定socket失败
#define ERR_LISTEN ("Socket监听失败") //监听socket失败
#define ERR_DATA ("数据处理初始化失败") //数据处理模块的初始化失败
#define ERR_DATARE ("数据处理释放失败") //数据处理模块的释放失败
#define ERR_CLOSE_S ("关闭Socket失败") //关闭socket失败
#define ERR_CRESOCKET ("创建Socket节点失败") //创建socket节点失败
#define ERR_ACCSOCKET ("接收Socket节点失败") //接收socket节点失败
#define ERR_CREAIOCB ("创建Aiocb结构失败") //创建结构失败
#define ERR_CREPOOL ("创建线程池失败") //创建线程池失败
#define ERR_INITQUEUE ("初始化队列失败") //初始化队列失败
#define ERR_AIOREAD ("异步读取失败") //异步读取失败
#define ERR_BUFF ("创建BUFFER失败") //创建BUFFER失败
#define ERR_PRONULL ("线程执行函数为空") //线程无法执行
#define ERR_OPFILE ("打开文件失败") //打开文件失败
#define ERR_DESLOCK ("销毁互斥锁失败") //销毁互斥锁失败
#define ERR_DELNODE ("删除节点失败") //删除节点失败
#define ERR_EDNODE ("修改节点失败") //修改节点失败
#define ERR_EDDATA ("修改数据失败") //修改数据失败
#define ERR_RDINI ("读取配置文件失败") //配置文件失败
#define ERR_REPOOL ("销毁线程池失败") //销毁线程池失败
#define ERR_REQUEUE ("销毁队列失败") //销毁队列失败
#define ERR_SERARGNUMBER ("服务器参数个数错误") //服务器参数个数错误
#define ERR_SERARG ("服务器参数错误") //服务器参数错误
#define ERR_STARTSER ("启动服务器失败") //服务器启动错误
#define ERR_STOPSER ("停止服务器失败") //服务器停止错误
#define ERR_RESTARTSER ("重启服务器失败") //服务器重启错误
#define ERR_CRESHME ("创建共享内存失败") //创建共享内存失败
#define ERR_RESHME ("释放共享内存失败") //释放共享内存失败
#define ERR_SERWISTA ("服务器没有启动") //服务器没有启动
#define ERR_SERSTA ("服务器已经启动") //服务器已经启动
#define ERR_OPESER ("操作服务器失败") //操作服务器失败
#define ERR_SIGENA ("激活信号失败") //激活信号失败
#define ERR_LISEPOLL ("删除epoll监听事件失败") //删除epoll监听事件失败
#define ERR_CREEPOLL ("创建epoll失败") //创建epoll失败
#define ERR_CLOEPOLL ("关闭epoll失败") //关闭epoll失败
#define ERR_MODSOCK ("修改套接字属性失败") //修改套接字属性失败
#define ERR_DBINIT ("数据库初始化失败")
#define ERR_DBCONN ("数据库连接失败")
#define ERR_DBOPEN ("数据库打开失败")
#define ERR_DBCLOSE ("数据库关闭失败")
#define ERR_DBAUTOCOMM ("设置数据库自动提交失败")
#define ERR_CRECONNPOOL ("创建连接池失败") //创建连接池失败
#define ERR_RECONNPOOL ("销毁连接池失败") //销毁连接池失败

void DebugInfor(char *pDebug); /*在调试时，打印的一些运行时信息，当版本发布时候，此函数输出的信息会被屏蔽*/
void LogInfor(char *pLog); /*日常运行情况的记录*/
void ErrorInfor(char *pFunctionName, char *pError); /*根据服务器定义的错误信息，输出相应的信息*/
void SystemErrorInfor(char *pFunctionName);	/*针对于Linux系统直接输出errno码所对应的信息*/

#endif /* LOGMANAGE_ERROR_H_ */

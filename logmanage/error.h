/*
 * error.h
 *
 *  Created on: 2015骞�鏈�鏃�
 *      Author: liuhanchong
 */

#ifndef LOGMANAGE_ERROR_H_
#define LOGMANAGE_ERROR_H_

#include <errno.h>
#include <string.h>
#include <stdio.h>

/*閿欒绾у埆
 * 1 debug		  璋冭瘯淇℃伅
 * 2 log 		  杈撳嚭鐨勬棩蹇�
 * 3 error 		  閿欒淇℃伅
 * 4 system error 绯荤粺淇℃伅
 * */

#define DEBUG_SERVER /*瀹氫箟姝ら」涓鸿皟璇曟ā寮�/

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
#define ERROR_CREPOOL 	 ("创建池失败")
#define ERROR_DBOPEN 	 ("数据库打开失败")
#define ERROR_CREAIOX 	 ("创建AIO失败")
#define ERROR_RELPOOL    ("释放池失败")
#define ERROR_RELQUEUE   ("释放队列失败")
#define ERROR_REMEVENT   ("删除事件失败")
#define ERROR_DISCONN  	 ("SOCKET连接断开")
#define ERROR_CLOSOCKET  ("关闭SOCKET失败")
#define ERROR_RELAIOX    ("释放异步IO失败")
#define ERROR_ADDEVENT 	 ("增加监听事件失败")
#define ERROR_INITDATA   ("初始化数据处理失败")
#define ERROR_RELEDATA   ("释放数据处理失败")
#define ERROR_MAIN       ("MAIN运行失败")
#define ERROR_ARGNUMBER  ("参数个数不符合规定")
#define ERROR_START      ("启动服务器失败")
#define ERROR_RELSHAMEM  ("释放共享内存失败")
#define ERROR_STOP       ("停止服务器失败")
#define ERROR_ARGVALUE   ("不匹配的参数")
#define ERROR_CRESHAMEM  ("创建共享内存失败")
#define ERROR_SERISRUN   ("服务器已启动")
#define ERROR_SERNORUN   ("服务器未启动")

void DebugInfor(char *pDebug); /*鍦ㄨ皟璇曟椂锛屾墦鍗扮殑涓�簺杩愯鏃朵俊鎭紝褰撶増鏈彂甯冩椂鍊欙紝姝ゅ嚱鏁拌緭鍑虹殑淇℃伅浼氳灞忚斀*/
void LogInfor(char *pLog); /*鏃ュ父杩愯鎯呭喌鐨勮褰�*/
void ErrorInfor(char *pFunctionName, char *pError); /* 鏍规嵁鏈嶅姟鍣ㄥ畾涔夌殑閿欒淇℃伅锛岃緭鍑虹浉搴旂殑淇℃伅 */
void SystemErrorInfor(char *pFunctionName);	/*閽堝浜嶭inux绯荤粺鐩存帴杈撳嚭errno鐮佹墍瀵瑰簲鐨勪俊鎭�*/

#endif /* LOGMANAGE_ERROR_H_ */

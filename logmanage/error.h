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

/*褰撳墠鏈嶅姟鍣ㄥ畾涔夌殑杩愯閿欒淇℃伅*/
#define ERROR_ARGNULL    ("浼犻�鐨勫弬鏁颁笉鑳戒负绌�)
#define ERROR_CRETHREAD  ("鍒涘缓绾跨▼澶辫触")
#define ERROR_CLOTHREAD  ("鍏抽棴绾跨▼澶辫触")
#define ERROR_DETTHREAD  ("鍒嗙绾跨▼澶辫触")
#define ERROR_TRANTYPE   ("寮哄埗杞崲鏁版嵁澶辫触")
#define ERROR_SETMODE    ("璁剧疆妯″紡澶辫触")
#define ERROR_PROFUNNULL ("绾跨▼鐨勫鐞嗗嚱鏁颁负绌�)
#define ERROR_OUTQUEUE   ("瓒呭嚭闃熷垪鐨勬渶澶ч暱搴�)
#define ERROR_INSEQUEUE  ("鎻掑叆鍏冪礌澶辫触")
#define ERROR_DELQUEUE   ("鍒犻櫎鍏冪礌澶辫触")
#define ERROR_EDITQUEUE  ("淇敼鏁版嵁澶辫触")
#define ERROR_FILEDDB    ("鑾峰彇缁撴灉闆嗗瓧娈靛け璐�)
#define ERROR_READDINI 	 ("璇诲彇閰嶇疆鏂囦欢澶辫触")
#define ERROR_INITQUEUE  ("鍒濆鍖栭槦鍒楀け璐�)
#define ERROR_CREPOOL 	 ("鍒涘缓姹犲け璐�)
#define ERROR_DBOPEN 	 ("鏁版嵁搴撴墦寮�け璐�)
#define ERROR_CREAIOX 	 ("鍒涘缓AIO澶辫触")
#define ERROR_RELPOOL    ("閲婃斁姹犲け璐�)
#define ERROR_RELQUEUE   ("閲婃斁闃熷垪澶辫触")
#define ERROR_REMEVENT   ("鍒犻櫎浜嬩欢澶辫触")
#define ERROR_DISCONN  	 ("SOCKET杩炴帴鏂紑")
#define ERROR_CLOSOCKET  ("鍏抽棴SOCKET澶辫触")
#define ERROR_RELAIOX    ("閲婃斁寮傛IO澶辫触")
#define ERROR_ADDEVENT 	 ("澧炲姞鐩戝惉浜嬩欢澶辫触")
#define ERROR_INITDATA   ("鍒濆鍖栨暟鎹鐞嗗け璐�)
#define ERROR_RELEDATA   ("閲婃斁鏁版嵁澶勭悊澶辫触")
#define ERROR_MAIN       ("MAIN杩愯澶辫触")
#define ERROR_ARGNUMBER  ("鍙傛暟涓暟涓嶇鍚堣瀹�)
#define ERROR_START      ("鍚姩鏈嶅姟鍣ㄥけ璐�)
#define ERROR_RELSHAMEM  ("閲婃斁鍏变韩鍐呭瓨澶辫触")
#define ERROR_STOP       ("鍋滄鏈嶅姟鍣ㄥけ璐�)
#define ERROR_ARGVALUE   ("涓嶅尮閰嶇殑鍙傛暟")
#define ERROR_CRESHAMEM  ("鍒涘缓鍏变韩鍐呭瓨澶辫触")
#define ERROR_SERISRUN   ("鏈嶅姟鍣ㄥ凡鍚姩")
#define ERROR_SERNORUN   ("鏈嶅姟鍣ㄦ湭鍚姩")
#define ERROR_GETSHAMEM  ("鑾峰彇鍏变韩鍐呭瓨澶辫触")
#define ERROR_CUTSHAMEM  ("鏂紑鍏变韩鍐呭瓨澶辫触")
#define ERROR_CRESOCKET  ("鍒涘缓SOCKET澶辫触")
#define ERROR_CRELISTEN  ("鍒涘缓鐩戝惉澶辫触")
#define ERROR_CREACCEPT  ("鍒涘缓鎺ユ敹SOCKET澶辫触")
#define ERROR_CRESIGNAL  ("鍒涘缓淇″彿澶辫触")
#define ERROR_INITLOG    ("初始化LOG失败")
#define ERROR_RELLOG     ("释放LOG失败")

void DebugInfor(char *pDebug); /*鍦ㄨ皟璇曟椂锛屾墦鍗扮殑涓�簺杩愯鏃朵俊鎭紝褰撶増鏈彂甯冩椂鍊欙紝姝ゅ嚱鏁拌緭鍑虹殑淇℃伅浼氳灞忚斀*/
void LogInfor(char *pLog); /*鏃ュ父杩愯鎯呭喌鐨勮褰�/
void ErrorInfor(char *pFunctionName, char *pError); /*鏍规嵁鏈嶅姟鍣ㄥ畾涔夌殑閿欒淇℃伅锛岃緭鍑虹浉搴旂殑淇℃伅*/
void SystemErrorInfor(char *pFunctionName);	/*閽堝浜嶭inux绯荤粺鐩存帴杈撳嚭errno鐮佹墍瀵瑰簲鐨勪俊鎭�/

#endif /* LOGMANAGE_ERROR_H_ */

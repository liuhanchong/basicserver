/*
 * serverconfig.h
 *
 *  Created on: 2015年9月8日
 *      Author: liuhanchong
 */

#ifndef BASICCOMPONENT_SERVERCONFIG_H_
#define BASICCOMPONENT_SERVERCONFIG_H_

#define MAXROWLENGTH 512 /*此处为配置文件每行的最大长度*/

typedef struct ServerConfig
{
	Ini ini;
} ServerConfig;

#endif /* BASICCOMPONENT_SERVERCONFIG_H_ */

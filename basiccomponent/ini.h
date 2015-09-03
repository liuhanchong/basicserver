/*
 * ini.h
 *
 *  Created on: 2015年9月3日
 *      Author: liuhanchong
 */

#ifndef BASICCOMPONENT_INI_H_
#define BASICCOMPONENT_INI_H_

#include <stdlib.h>
#include <memory.h>
#include "../logmanage/error.h"

typedef struct Ini
{
	FILE *pFile;
	char *pPath;
	char *pText;
	int nRowMaxLength;
} Ini;

/*接口*/
int InitIni(Ini *pIni, char *chPath, int nRowMaxLength);
const char *GetPathName(Ini *pIni);
char *GetStringEx(Ini *pIni, char *pSection, char *pKey, char *pDef);
char *GetString(Ini *pIni, char *pSection, char *pKey, char *pDef);
int GetInt(Ini *pIni, char *pSection, char *pKey, int nDefault);
int GetUInt(Ini *pIni, char *pSection, char *pKey, int nDefault);
double GetDouble(Ini *pIni, char *pSection, char *pKey, double dDefault);
void GetArray(Ini *pIni, char *pSection, char *pKey, char **pArray, int nLen);
int GetBool(Ini *pIni, char *pSection, char *pKey, int nDefault);
void ReleaseIni(Ini *pIni);

/*私有*/
void FindValue(Ini *pIni, char *pSection, char *pKey, char *pValue, int nSize);
int FindSection(char *pSection,  char *pValue);
int FindKey(char *pKey, char *pValue);
char *GetLine(char *pText, int nSize, FILE *pFile);
void ClearSpace(char *pText, int nSize);
int FindChar(char ch, char *pText);

#endif /* BASICCOMPONENT_INI_H_ */

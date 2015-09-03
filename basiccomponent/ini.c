/*
 * ini.c
 *
 *  Created on: 2015年9月3日
 *      Author: liuhanchong
 */

#include "ini.h"

int InitIni(Ini *pIni, char *chPath, int nRowMaxLength)
{
	if (!chPath || !pIni || nRowMaxLength < 1)
	{
		ErrorInfor("InitIni", ERROR_ARGNULL);
		return 0;
	}

	FILE *pFile = fopen(chPath, "r+");
	if (!pFile)
	{
		SystemErrorInfor("InitIni-1");
		return 0;
	}

	pIni->pPath = (char *)malloc(strlen(chPath));
	if (!pIni->pPath)
	{
		SystemErrorInfor("InitIni-2");
		fclose(pFile);
		return 0;
	}

	pIni->pText = (char *)malloc(nRowMaxLength);
	if (!pIni->pText)
	{
		SystemErrorInfor("InitIni-3");
		fclose(pFile);
		free(pIni->pPath);
		return 0;
	}

	pIni->pFile = pFile;
	memcpy(pIni->pPath, chPath, strlen(chPath));
	pIni->nRowMaxLength = nRowMaxLength;

	return 1;
}

const char *GetPathName(Ini *pIni)
{
	if (!pIni)
	{
		return NULL;
	}

	return pIni->pPath;
}

char *GetStringEx(Ini *pIni, char *pSection, char *pKey, char *pDef)
{
	char *pValue = (char *)malloc(pIni->nRowMaxLength);
	if (!pValue)
	{
		SystemErrorInfor("GetStringEx");
		return NULL;
	}

	memcpy(pValue, GetString(pIni, pSection, pKey, pDef), pIni->nRowMaxLength);
	return pValue;
}

char *GetString(Ini *pIni, char *pSection, char *pKey, char *pDef)
{
	if (!pIni || !pSection || !pKey || !pDef)
	{
		ErrorInfor("GetString", ERROR_ARGNULL);
		return NULL;
	}

	char *pValue = (char *)malloc(pIni->nRowMaxLength);
	if (!pValue)
	{
		SystemErrorInfor("GetString");
		return NULL;
	}

	FindValue(pIni, pSection, pKey, pValue, pIni->nRowMaxLength);

	if (strlen(pValue) > 0)
	{
		int nSize = strlen(pValue);
		memcpy(pIni->pText, pValue, nSize);
		pIni->pText[nSize] = '\0';
	}
	else
	{
		memset(pIni->pText, 0, pIni->nRowMaxLength);
		memcpy(pIni->pText, pDef, strlen(pDef));
	}

	free(pValue);
	pValue = NULL;

	return pIni->pText;
}

int GetInt(Ini *pIni, char *pSection, char *pKey, int nDefault)
{
	if (!pIni || !pSection || !pKey)
	{
		ErrorInfor("GetInt", ERROR_ARGNULL);
		return nDefault;
	}

	if (strlen(GetString(pIni, pSection, pKey, "")) == 0)
	{
		return nDefault;
	}

	return atoi(pIni->pText);
}

int GetUInt(Ini *pIni, char *pSection, char *pKey, int nDefault)
{
	if (!pIni || !pSection || !pKey)
	{
		ErrorInfor("GetUInt", ERROR_ARGNULL);
		return 0;
	}

	int nValue = GetInt(pIni, pSection, pKey, nDefault);
	return (nValue > 0) ? nValue : 0;
}

double GetDouble(Ini *pIni, char *pSection, char *pKey, double dDefault)
{
	if (!pIni || !pSection || !pKey)
	{
		ErrorInfor("GetInt", ERROR_ARGNULL);
		return 0;
	}

	return (strlen(GetString(pIni, pSection, pKey, "")) == 0) ? dDefault : atof(pIni->pText);
}

void GetArray(Ini *pIni, char *pSection, char *pKey, char **pArray, int nLen)
{
}

int GetBool(Ini *pIni, char *pSection, char *pKey, int nDefault)
{
	if (!pIni || !pSection || !pKey)
	{
		ErrorInfor("GetBool", ERROR_ARGNULL);
		return 0;
	}
	return ((GetInt(pIni, pSection, pKey, ((nDefault > 0) ? 1 : 0)) > 0) ? 1 : 0);
}

void ReleaseIni(Ini *pIni)
{
	if (!pIni)
	{
		ErrorInfor("ReleaseIni", ERROR_ARGNULL);
		return;
	}

	fclose(pIni->pFile);
	free(pIni->pPath);
	free(pIni->pText);
}

void FindValue(Ini *pIni, char *pSection, char *pKey, char *pValue, int nSize)
{
	if (!pIni || !pSection || !pKey || !pValue || nSize <= 0 || pIni->pFile)
	{
		ErrorInfor("FindValue", ERROR_ARGNULL);
		return;
	}

	FILE *pFile = pIni->pFile;
	if (fseek(pFile, 0, SEEK_SET) != 0)
	{
		SystemErrorInfor("FindValue-1");
		return;
	}

	char *pTmp = (char *)malloc(pIni->nRowMaxLength);
	if (!pTmp)
	{
		SystemErrorInfor("FindValue-2");
		return;
	}

	while (GetLine(pTmp, pIni->nRowMaxLength, pFile))
	{
		if (FindSection(pSection, pTmp))
		{
			while (GetLine(pTmp, pIni->nRowMaxLength, pFile))
			{
				/*找到下一个段，证明在当前段没找到*/
				if ((FindChar('[', pTmp) >= 0) && (FindChar(']', pTmp) >= 0))
				{
					break;
				}

				if (FindKey(pKey, pTmp))
				{
					/*分析数值*/
					int nIndex = FindChar('=', pTmp) + 1;
					memcpy(pValue, (pTmp + nIndex), (strlen(pTmp) - nIndex));
					pValue[strlen(pTmp) - nIndex] = '\0';
					break;
				}
			}
		}
	}

	free(pTmp);
	pTmp = NULL;
}

int FindSection(char *pSection, char *pValue)
{
	if (!pSection || !pValue)
	{
		ErrorInfor("FindSection", ERROR_ARGNULL);
		return 0;
	}

	int nBegin = -1;
	int nEnd = -1;
	nBegin = FindChar('[', pValue) + 1;
	nEnd = FindChar(']', pValue);

	int nSize = nEnd - nBegin;
	if (nBegin >= 0 && nEnd >= nBegin && (nSize == strlen(pSection)))
	{
		for (int i = 0; i < nSize; i++)
		{
			if (pValue[i + nBegin] != pSection[i])
			{
				return 0;
			}
		}

		return 1;
	}

	return 0;
}

int FindKey(char *pKey, char *pValue)
{
	if (!pKey || !pValue)
	{
		ErrorInfor("FindKey", ERROR_ARGNULL);
		return 0;
	}

	int nIndex = FindChar('=', pValue);
	if (nIndex == strlen(pKey))
	{
		for (int i = 0; i < nIndex; i++)
		{
			if (pValue[i] != pKey[i])
			{
				return 0;
			}
		}

		return 1;
	}

	return 0;
}

char *GetLine(char *pText, int nSize, FILE *pFile)
{
	if (!pText || nSize <= 0 || !pFile)
	{
		ErrorInfor("GetLine", ERROR_ARGNULL);
		return 0;
	}

	char *pRet = fgets(pText, nSize, pFile);
	if (!pRet)
	{
		SystemErrorInfor("GetLine");
		return 0;
	}

	//删除\r\n
	int nIndex = FindChar('\r', pText);
	if (nIndex >= 0)
	{
		pText[nIndex] = '\0';
	}

	nIndex = FindChar('\n', pText);
	if (nIndex >= 0)
	{
		pText[nIndex] = '\0';
	}

	//找到;号标志
	nIndex = FindChar(';', pText);
	if (nIndex >= 0)
	{
		pText[nIndex] = '\0';
	}

	//清楚左右的空格
	int nLen = strlen(pText);
	ClearSpace(pText, nLen);

	//清除等号左右的空格
	nIndex = FindChar('=', pText);
	if (nIndex >= 0 && nIndex < nLen)
	{
		pText[nIndex] = ' ';

		ClearSpace(pText, nIndex + 1);

		nLen = strlen(pText);
		pText[nLen] = '=';

		ClearSpace(pText + nLen + 1, nSize - nLen);
	}

	return pRet;
}

void ClearSpace(char *pText, int nSize)
{
	if (!pText)
	{
		ErrorInfor("ClearSpace", ERROR_ARGNULL);
		return;
	}

	int nBegin = -1;
	int nEnd = -1;
	for (int i = 0; i < nSize; i++)
	{
		if (pText[i] != ' ')
		{
			if (nBegin == -1)
			{
				nBegin = i;
			}

			nEnd = i;
		}
	}

	if (nBegin >= 0 && nEnd >= nBegin)
	{
		int nSize = nEnd - nBegin + 1;
		for (int i = 0; i < nSize; i++)
		{
			pText[i] = pText[i + nBegin];
		}

		pText[nEnd - nBegin + 1] = '\0';
	}
	else
	{
		pText[0] = '\0';
	}
}

int FindChar(char ch, char *pText)
{
	if (!pText)
	{
		ErrorInfor("FindChar", ERROR_ARGNULL);
		return -1;
	}

	char *pIndex = pText;
	while (*pIndex != '\0' && *(pIndex) != ch)
	{
		pIndex++;
	}

	return (*pIndex == ch) ? (pIndex - pText) : -1;
}


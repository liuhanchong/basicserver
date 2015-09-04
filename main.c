/*
 * main.c
 *
 *  Created on: 2015年9月3日
 *      Author: liuhanchong
 */

#include "./servermanage/servermanage.h"

int main(int argc, char *argv[])
{
	if (Start(argc, argv) == 0)
	{
		ErrorInfor("main", ERROR_MAIN);
		return 1;
	}

	return 0;
}

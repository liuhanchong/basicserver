/*
 * main.c
 *
 *  Created on: 2015��9��3��
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

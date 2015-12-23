// DropListenerExeChild.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <Windows.h>

/*
See DropListenerExe for description.
*/

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 1)
		return -1;

	int num = atoi(argv[0]);
	if (num > 0)
	{
		unsigned int uNum = (unsigned int)num;
		for (int i = 0; i < 243; i++)
			uNum = (uNum * 1103515245 + 12345) % 2147483648;
		uNum = (uNum % 25500) + 40000;
		num = (int)uNum;

	}
	else
		num = 0;
	
	ExitProcess(num);
}



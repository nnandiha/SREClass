// HMACVerifier.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Common.h"
#include <tchar.h>

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 4)
	{
		printf("Usage: %s <studentID> <challengeID> <HMAC>", argv[0]);
		return 0;
	}

	if (verifyHMAC(	argv[1], strlen(argv[1])+1, //Student ID
					argv[2], strlen(argv[2])+1, //Challenge ID
					argv[3], strlen(argv[3])+1) //HMAC
					== 1)
		printf("Successfully validated HMAC.\n");
	else
		printf("ERROR - HMAC does not validate.\n");
	return 0;
}


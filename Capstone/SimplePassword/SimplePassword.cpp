// SimplePassword.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Common.h"

static const TCHAR CHALLENGE_NAME[] = "Challenge 1";
static const TCHAR PORT[] = "5001";
static const TCHAR DESCRIPTION[] = "";
static const TCHAR CATEGORY[] = "General";
static const TCHAR DIFFICULTY[] = "Easy";

void __declspec (dllexport) NTAPI  challenge(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WORK work)
{
	SOCKET s = (SOCKET)context;
}

int _tmain(int argc, _TCHAR* argv[])
{
	_challengeInfo cInfo;
	cInfo.challengeName = CHALLENGE_NAME;
	cInfo.port = PORT;
	cInfo.description = DESCRIPTION;
	cInfo.category = CATEGORY;
	cInfo.difficulty = DIFFICULTY;
	cInfo.fCB = challenge;
	
	startServer(cInfo);
	return 0;
}


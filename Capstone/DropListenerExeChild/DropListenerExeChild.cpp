// DropListenerExeChild.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Common.h"

/*
This challenge 
*/

static const TCHAR CHALLENGE_NAME[] = "ARE1";
static const TCHAR DIFFICULTY[] = "5/5";

void __declspec (dllexport) NTAPI challenge(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WORK work)
{
	TCHAR username[32];
	SOCKET s = (SOCKET)context;

	int usernameLen = getData(s, username, sizeof(username) - 1);
	if (usernameLen <= 0){
		endComms(s);
		return;
	}
	username[usernameLen - 1] = '\0'; //Clobber the \n that netcat adds

	int result = submitFlag(username, CHALLENGE_NAME, DIFFICULTY);
	if (result == 0){
		TCHAR chal2[] = "Congratulations! Your flag has been submitted.\n";
		sendData(s, chal2, sizeof(chal2));
	}
	else if (result == -2){
		TCHAR chal2[] = "You have solved this challenge, but the flag submission script was not found. This probably means you were running it locally and not on the instructor's server.\n";
		sendData(s, chal2, sizeof(chal2));
	}
	else{
		TCHAR chal2[] = "You solved this challenge, but there was a problem submitting your flag. Try again or ask for assistance.\n";
		sendData(s, chal2, sizeof(chal2));
	}

	endComms(s);
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 2)
		return 0;

	_challengeInfo cInfo;
	cInfo.challengeName = "";
	cInfo.port = argv[1];
	cInfo.description = "";
	cInfo.category = "";
	cInfo.difficulty = "";
	cInfo.fCB = challenge;

	startServer(cInfo);

	while (1)
		Sleep(30000);
	return 0;
}



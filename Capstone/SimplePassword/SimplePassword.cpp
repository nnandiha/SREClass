// SimplePassword.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Common.h"

static const TCHAR CHALLENGE_NAME[] = "GEN1";
static const TCHAR PORT[] = "39001";
static const TCHAR DESCRIPTION[] = "As straightforward as they come!";
static const TCHAR CATEGORY[] = "General";
static const TCHAR DIFFICULTY[] = "1/5";

/*
This challenge requires the student to type in the correct password. The
password is clearly available in the code, so it should be fairly
straightforward to locate by the student.
*/

static const TCHAR PASSWORD[] = "TrustNo1";

void __declspec (dllexport) NTAPI challenge(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WORK work)
{
	TCHAR username[32];
	TCHAR buf[32];
	SOCKET s = (SOCKET)context;

	int usernameLen = getData(s, username, sizeof(username) - 1);
	if (usernameLen <= 0){
		endComms(s);
		return;
	}
	username[usernameLen-1] = '\0'; //Clobber the \n that netcat adds
	printf("User connected: %s\n", username);

	TCHAR chal1[] = "Enter the password for access: ";
	if (sendData(s, chal1, sizeof(chal1)) < 0){
		endComms(s);
		return;
	}

	int bufLen = getData(s, buf, sizeof(buf) - 1);
	if (bufLen <= 0){
		endComms(s);
		return;
	}
	buf[bufLen - 1] = '\0'; //Clobber the \n that netcat adds

	if (strncmp(buf, PASSWORD, sizeof(PASSWORD)) == 0){
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
	}
	else{
		TCHAR chal2[] = "Sorry, that's not the correct password. Please try again later.\n";
		sendData(s, chal2, sizeof(chal2));
	}

	endComms(s);
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

	while (1)
		Sleep(30000);
	return 0;
}


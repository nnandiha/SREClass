// SimplePickNumber.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include "Common.h"

static const TCHAR CHALLENGE_NAME[] = "GEN4";
static const TCHAR PORT[] = "39002";
static const TCHAR DESCRIPTION[] = "Try to pick the right number.";
static const TCHAR CATEGORY[] = "General";
static const TCHAR DIFFICULTY[] = "1/5";

/*
This challenge requires the student to type in the correct number. This magic
number is contained within a switch statement, and requires the student to
understand how switch statements work in assembly. The magic number is 73882.
*/

void __declspec (dllexport) NTAPI challenge(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WORK work)
{
	TCHAR username[32];
	TCHAR buf[32];
	SOCKET s = (SOCKET)context;

	unsigned long peerIP = getPeerIP(s);
	if (peerIP == 0){
		endComms(s);
		return;
	}

	int usernameLen = getData(s, username, sizeof(username) - 1);
	if (usernameLen <= 0){
		endComms(s);
		return;
	}
	username[usernameLen - 1] = '\0'; //Clobber the \n that netcat adds
	if (validateUsername(username, usernameLen) == FALSE){
		endComms(s);
		return;
	}
	printf("User connected: %s\n", username);

	TCHAR chal1[] = "Enter the correct number: ";
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

	int guess = 0;
	_sntscanf_s(buf, 8, "%d", &guess);

	switch (guess){
	case 8398:
	case 67210:
	case 62:
	case 8724:
		guess = 0;
		break;
	case 73882:
		guess = 1;
		break;
	}

	if (guess == 1){
		int result = submitFlag(username, CHALLENGE_NAME, DIFFICULTY, peerIP);
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
		TCHAR chal2[] = "Sorry, that number is not correct. Please try again later.\n";
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


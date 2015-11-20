// LCG.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Common.h"
#include <tchar.h>

static const TCHAR CHALLENGE_NAME[] = "CR1";
static const TCHAR PORT[] = "39004";
static const TCHAR DESCRIPTION[] = "Figure out the next number in the sequence.";
static const TCHAR CATEGORY[] = "Crypto";
static const TCHAR DIFFICULTY[] = "2/5";

/*
This challenge requires the student to simulate a Linear Congruential Generator. The LCG is
defined by the constants below. To obtain the correct answer, take the number presented in
the challenge, multiply it by A, then add C, then take its modulus of M. The result
is then ANDed by 0x7FFFFFF. This is the number that should be submitted (in decimal format)
*/

const unsigned long M = 2147483648L; //2^31
const unsigned long A = 1103515245L;
const unsigned long C = 12345L;

int stepLCG(unsigned long long &state)
{
	state *= A;
	state += C;
	state %= M;

	return (int)(state & 0x7FFFFFFF);
}


void __declspec (dllexport) NTAPI challenge(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WORK work)
{
	TCHAR username[32];
	TCHAR buf[32];
	TCHAR chal1[128];
	SOCKET s = (SOCKET)context;
	unsigned long long state = getPseudoRand();
	int match = stepLCG(state);

	int usernameLen = getData(s, username, sizeof(username) - 1);
	if (usernameLen <= 0){
		endComms(s);
		return;
	}
	username[usernameLen - 1] = '\0'; //Clobber the \n that netcat adds
	printf("User connected: %s\n", username);

	int chal1Len = sprintf_s(chal1, sizeof(chal1), "Enter the number that follows %d in the sequence: ", match);

	if (sendData(s, chal1, chal1Len) < 0){
		endComms(s);
		return;
	}

	int bufLen = getData(s, buf, sizeof(buf) - 1);
	if (bufLen <= 0){
		endComms(s);
		return;
	}
	buf[bufLen - 1] = '\0'; //Clobber the \n that netcat adds

	match = stepLCG(state);
	int guess = _tstoi(buf);

	if (match == guess){
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
		TCHAR chal2[] = "That is not the next number in the sequence. Please try again. Or not.\n";
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




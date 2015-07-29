// XorGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <intrin.h>
#include <time.h>

#include "Common.h"

static const TCHAR CHALLENGE_NAME[] = "GEN4";
static const TCHAR PORT[] = "39007";
static const TCHAR DESCRIPTION[] = "A race to zero";
static const TCHAR CATEGORY[] = "General";
static const TCHAR DIFFICULTY[] = "Medium";

/*
This challenge starts by printing a random number N to the student. The goal is for the student
to xor one bit at a time until this number reaches zero. The student may only flip one bit
at a time by entering decimal numbers. The student is given popcnt(N) entries, and
each entry must satisfy popcnt(entry) == 1, meaning only a single bit is set. The number of
entries given is exactly the number required to reach zero if only the correct bits are selected.
An example run through is as follows:

Random number: 69, popcnt(69) == 3
Student gets 3 entries
N=69
Entry 1: 4 -> N==65
Entry 2: 1 -> N==64
Entry 3: 64 -> N==0
*/

void __declspec (dllexport) NTAPI challenge(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WORK work)
{
	TCHAR username[32];
	TCHAR buf[64];
	SOCKET s = (SOCKET)context;

	int usernameLen = getData(s, username, sizeof(username) - 1);
	if (usernameLen <= 0){
		endComms(s);
		return;
	}
	username[usernameLen - 1] = '\0'; //Clobber the \n that netcat adds
	printf("User connected: %s\n", username);

	srand(time(NULL));
	int num = rand();
	int popcnt = _mm_popcnt_u32(num);

	int bufLen = sprintf_s(buf, sizeof(buf), "Your target is: %d\n", num);
	if (sendData(s, buf, bufLen) < 0){
		endComms(s);
		return;
	}

	for (int i = 0; i < popcnt; i++)
	{
		int bufLen = sprintf_s(buf, sizeof(buf), "Please enter a number. You have %d entries left:  ", (popcnt-i));
		if (sendData(s, buf, bufLen) < 0){
			endComms(s);
			return;
		}

		bufLen = getData(s, buf, sizeof(buf) - 1);
		if (bufLen <= 0){
			endComms(s);
			return;
		}
		buf[bufLen - 1] = '\0'; //Clobber the \n that netcat adds

		int xorVal = _tstoi(buf);
		if (_mm_popcnt_u32(xorVal) != 1)
		{
			endComms(s);
			return;
		}

		num ^= xorVal;
	}

	TCHAR hmac[HMAC_LENGTH];
	if (num == 0){
		int hmacLen = generateHMAC(username, usernameLen, CHALLENGE_NAME, sizeof(CHALLENGE_NAME), hmac, sizeof(hmac));
		if (hmacLen < 0){
			printf("Error generating HMAC.\n");
			endComms(s);
			return;
		}

		TCHAR chal2[] = "Access granted! Your key is:\n";
		sendData(s, chal2, sizeof(chal2));
		sendData(s, hmac, hmacLen);
		sendData(s, "\n", 1);
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



// WhackAMole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <time.h>

#include "Common.h"

static const TCHAR CHALLENGE_NAME[] = "GEN7";
static const TCHAR PORT[] = "39009";
static const TCHAR DESCRIPTION[] = "Can you whack all of the moles?";
static const TCHAR CATEGORY[] = "General";
static const TCHAR DIFFICULTY[] = "Medium";

/*
This challenge consists of 10 "moles" that the student needs to whack. Each whack has a side effect
on the other moles, so the student needs to analyze these effects to knock all of the moles out.

Whacking a mole will always cause that mole to drop. i.e. the side effect on that mole is ignored
Side effects:
1- Causes 1 random mole that isn't up already up to pop up
2- Pops up mole 5 and 1
3- No effect, but this mole pops up on every 3rd whack
4- Pops up mole 8
5- Causes 2 random moles to pop up. Effect ignored if mole is already up.
6- If the previous whack was an even mole, no effect. Otherwise pops up mole 9 and 3
7- Causes one random mole to drop. Effect ignored if mole is already down.
8- Causes all moles to invert
9- Inverts the mole corresponding to the total number of whacks so far (mod 10)
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

/*
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
		int bufLen = sprintf_s(buf, sizeof(buf), "Please enter a number. You have %d entries left:  ", (popcnt - i));
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
	}*/

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



// WhackAMole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <time.h>

#include "Common.h"

static const TCHAR CHALLENGE_NAME[] = "GEN7";
static const TCHAR PORT[] = "39009";
static const TCHAR DESCRIPTION[] = "Can you whack all of the moles?";
static const TCHAR CATEGORY[] = "General";
static const TCHAR DIFFICULTY[] = "4/5";

/*
This challenge consists of 10 "moles" that the student needs to whack. Each whack has a side effect
on the other moles, so the student needs to analyze these effects to knock all of the moles out.

Whacking a mole will always cause that mole to drop. i.e. the side effect on that mole is ignored
Side effects:
1- Causes 1 random mole that isn't already up to pop up
2- Pops up mole 5 and 1
3- No effect, but this mole pops up on every 3rd whack
4- Pops up mole 8
5- Causes 2 random moles to pop up. Effect ignored if mole is already up.
6- If the previous whack was an even mole, no effect. Otherwise pops up mole 9 and 3
7- Causes one random mole to drop or pop up alternatingly. Effect ignored if mole is already in desired state.
8- Causes all moles to invert
9- Inverts the mole corresponding to the total number of whacks so far (mod 9)
*/

static const int NUMMOLES = 9;
static const int MOLE1 = 0;
static const int MOLE2 = 1;
static const int MOLE3 = 2;
static const int MOLE4 = 3;
static const int MOLE5 = 4;
static const int MOLE6 = 5;
static const int MOLE7 = 6;
static const int MOLE8 = 7;
static const int MOLE9 = 8;
int molehill[9];

void printMoles(SOCKET s)
{
	char buf[128];
	int bufLen = sprintf_s(	buf,
							sizeof(buf),
							"1 2 3 4 5 6 7 8 9\n%c %c %c %c %c %c %c %c %c\nWhich mole would you like to whack? ",
							(char)(0x2D + molehill[MOLE1] * 0x13),
							(char)(0x2D + molehill[MOLE2] * 0x13),
							(char)(0x2D + molehill[MOLE3] * 0x13),
							(char)(0x2D + molehill[MOLE4] * 0x13),
							(char)(0x2D + molehill[MOLE5] * 0x13),
							(char)(0x2D + molehill[MOLE6] * 0x13),
							(char)(0x2D + molehill[MOLE7] * 0x13),
							(char)(0x2D + molehill[MOLE8] * 0x13),
							(char)(0x2D + molehill[MOLE9] * 0x13)
							);
	if (sendData(s, buf, bufLen) < 0){
		endComms(s);
		return;
	}
}

bool checkMoles()
{
	bool allWhacked = true;
	for (int i = 0; i < NUMMOLES; i++){
		if (molehill[i] == 1)
			allWhacked = false;
	}
	return allWhacked;
}

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
	if (validateUsername(username, usernameLen) == FALSE){
		endComms(s);
		return;
	}
	printf("User connected: %s\n", username);
	
	srand(time(NULL));
	molehill[MOLE1] = rand() % 2;
	molehill[MOLE2] = rand() % 2;
	molehill[MOLE3] = rand() % 2;
	molehill[MOLE4] = rand() % 2;
	molehill[MOLE5] = rand() % 2;
	molehill[MOLE6] = rand() % 2;
	molehill[MOLE7] = rand() % 2;
	molehill[MOLE8] = rand() % 2;
	molehill[MOLE9] = rand() % 2;

	int numWhacks = 0;
	char lastWhack = '0';
	int state7 = 0;

	printMoles(s);

	while (numWhacks < 20) {
		int bufLen = getData(s, buf, sizeof(buf) - 1);
		if (bufLen <= 0){
			endComms(s);
			return;
		}
		buf[bufLen - 1] = '\0'; //Clobber the \n that netcat adds

		if (buf[0] < '1' || buf[0] > '9'){
			char noWhack[] = "That mole is not whackable!\n";
			if (sendData(s, noWhack, sizeof(noWhack)) < 0){
				endComms(s);
				return;
			}
			continue;
		}

		numWhacks++;

		if (numWhacks % 2 == 0){
			molehill[numWhacks % NUMMOLES] = 1;
		}

		char sel = buf[0];
		if (sel == '1'){
			

			molehill[MOLE1] = 0;
		}
		else if (sel == '2'){
			molehill[MOLE1] = 1;
			molehill[MOLE5] = 1;

			molehill[MOLE2] = 0;
		}
		else if (sel == '3'){
			molehill[MOLE3] = 0;
		}
		else if (sel == '4'){
			molehill[MOLE8] = 1;

			molehill[MOLE4] = 0;
		}
		else if (sel == '5'){
			molehill[rand() % NUMMOLES] = 1;
			molehill[rand() % NUMMOLES] = 1;

			molehill[MOLE5] = 0;
		}
		else if (sel == '6'){
			if (lastWhack % 2 == 0){
				molehill[MOLE3] = 1;
				molehill[MOLE9] = 1;
			}

			molehill[MOLE6] = 0;
		}
		else if (sel == '7'){
			molehill[rand() % NUMMOLES] = state7;
			state7 ^= 1;

			molehill[MOLE7] = 0;
		}
		else if (sel == '8'){
			for (int i = 0; i < NUMMOLES; i++){
				molehill[i] ^= 1;
			}

			molehill[MOLE8] = 0;
		}
		else if (sel == '9'){
			molehill[numWhacks % NUMMOLES] ^= 1;

			molehill[MOLE9] = 0;
		}
		lastWhack = sel;

		printMoles(s);
		if (checkMoles())
			break;
	}
	
	if (checkMoles()){
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



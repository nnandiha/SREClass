// MungedPassword.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Common.h"

static const TCHAR CHALLENGE_NAME[] = "GEN2";
static const TCHAR PORT[] = "39005";
static const TCHAR DESCRIPTION[] = "Enter the password for access.";
static const TCHAR CATEGORY[] = "General";
static const TCHAR DIFFICULTY[] = "2/5";

/*
This challenge requires the student to type in the correct password. This 
version is a little harder than the SimplePassword version because the
plaintext string is not kicking around in code or memory to be located
easily. The student must figure out how it is munged to work out what
the string is.
*/

//Munged ASCII String: "Intel Inside"
const TCHAR MUNGED[] = "\x97\x38\x18\xc2\xbd\x18\xcd\x85\x6c\x13\x1e\xc4";

//Random bytes
const TCHAR RANDOM[] = "\xde\x56\x6c\xa7\xd1\x38\x84\xeb\x1f\x7a\x7a\xa1";

void __declspec (dllexport) NTAPI challenge(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WORK work)
{
	TCHAR username[32];
	TCHAR buf[32];
	SOCKET s = (SOCKET)context;
	TCHAR guess[sizeof(MUNGED)];

	int usernameLen = getData(s, username, sizeof(username) - 1);
	if (usernameLen <= 0){
		endComms(s);
		return;
	}
	username[usernameLen - 1] = '\0'; //Clobber the \n that netcat adds
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

	if (bufLen != sizeof(MUNGED))
	{
		TCHAR chal2[] = "You're not even close!\n";
		sendData(s, chal2, sizeof(chal2));
		endComms(s);
		return;
	}
	
	for (int i = 0; i < bufLen; i++)
	{
		guess[i] = buf[i] ^ RANDOM[i];
	}

	if (strncmp(guess, MUNGED, sizeof(MUNGED)) == 0){
		if (submitFlag(username, usernameLen, CHALLENGE_NAME, sizeof(CHALLENGE_NAME)) == 0){
			TCHAR chal2[] = "Congratulations! Your flag has been submitted.\n";
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



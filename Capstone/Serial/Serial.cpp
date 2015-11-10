// Serial.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Common.h"

static const TCHAR CHALLENGE_NAME[] = "GEN3";
static const TCHAR PORT[] = "39003";
static const TCHAR DESCRIPTION[] = "Reverse engineer the serial number verification algorithm.";
static const TCHAR CATEGORY[] = "General";
static const TCHAR DIFFICULTY[] = "3/5";

/*
This challenge requires a valid serial number to be entered. A serial must be of the form:
XXXX-XXXX-XXXX-XXXX
0000000000111111111
0123456789012345678

and adhere to the following requirements:
0. All characters must be ASCII printable
1. buf[1] == 'A'
2. buf[3] == '.'
3. buf[0] == buf[2]
4. buf[5] > buf[6]
5. buf[7] and buf[8] are numeric
6. buf[10] - buf[13] == 6
7. buf[11] ^ buf[12] == 2
8. buf[15] + buf[16] + buf[18] == 0xA3
9. buf[17] == buf[16] + buf[18]
10. Parity of buf[15] through buf[18] is 1

Example valid serial: AAA.-BA00-6AC0-A1b1
*/

bool verifySerial(TCHAR *buf, int bufLen)
{
	if (bufLen != 19)
		return false;

	for (int i = 0; i < bufLen; i++)
	{
		if (buf[i] < 0x20 || buf[i] > 0x7f)
			return false;
	}

	if (buf[4] != '-' || buf[9] != '-' || buf[14] != '-')
		return false;

	if (buf[1] != 'A')
		return false;

	if (buf[3] != '.')
		return false;

	if (buf[0] != buf[2])
		return false;

	if (buf[5] <= buf[6])
		return false;

	if (buf[7] < '0' || buf[7] > '9' || buf[8] < '0' || buf[8] > '9')
		return false;

	if ((buf[10] - buf[13]) != 6)
		return false;

	if ((buf[11] ^ buf[12]) != 2)
		return false;

	if (buf[15] + buf[16] + buf[18] != 0xA3)
		return false;

	if (buf[17] != buf[16] + buf[18])
		return false;

	if (((int)buf[15] + buf[16] + buf[17] + buf[18]) % 2 != 1)
		return false;

	return true;
}

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
	username[usernameLen - 1] = '\0'; //Clobber the \n that netcat adds
	printf("User connected: %s\n", username);

	TCHAR chal1[] = "Enter a valid serial number: ";
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

	TCHAR hmac[HMAC_LENGTH];


	if (verifySerial(buf, bufLen-1) == true){
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
		TCHAR chal2[] = "That serial number is incorrect. Please stop trying to hack this application.\n";
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



// Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include "../Common/Common.h"

#define DEFAULT_BUFLEN 512

void NTAPI func(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WORK work)
{
	SOCKET s = (SOCKET)context;

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
//	int iResult;

	getData(s, recvbuf, DEFAULT_BUFLEN, 4);
	sendData(s, recvbuf, DEFAULT_BUFLEN, 4);
}

int _tmain(int argc, _TCHAR* argv[])
{
	TCHAR oHash[64];
	generateHMAC("test", 5, "test", 5, oHash);

//	startServer("1234", "test", 5, func);
	return 0;
}
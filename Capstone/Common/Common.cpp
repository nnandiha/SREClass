// Common.cpp : Defines the exported functions for the DLL application.
//

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include "stdafx.h"
#include "Common.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include "sha1.h"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

// This is an example of an exported variable
COMMON_API int nCommon=0;

// This is an example of an exported function.
COMMON_API int fnCommon(void)
{
	return 42;
}


static const TCHAR DEFAULT_KEY[] = "PASSWORD";
static const int KEY_LENGTH = 32;

void NTAPI MyWorkCallback(PTP_CALLBACK_INSTANCE instance, void * context, PTP_WORK work)
{
	printf("Hello %s", context);
}

COMMON_API int startServer(int port, TCHAR *challengeName, int challengeNameLen, PTP_WORK_CALLBACK fCB)
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	while (1)
	{
		iResult = listen(ListenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		TP_WORK *work = CreateThreadpoolWork(fCB, (PVOID)ClientSocket, NULL);
		if (work == NULL)
			printf("Error %d in CreateThreadpoolWork", GetLastError());
		else {
			SubmitThreadpoolWork(work);
			WaitForThreadpoolWorkCallbacks(work, FALSE);
			CloseThreadpoolWork(work);
		}
	}
	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}

COMMON_API int sendData(SOCKET s, TCHAR *buf, int bufLen, int bytesToSend)
{
	if (buf == NULL || bufLen <= 0 || bytesToSend <= 0)
		return -1;

	if (bytesToSend > bufLen)
		return -1;

	ZeroMemory(buf, bufLen);

	// Echo the buffer back to the sender
	int iSendResult = send(s, buf, bytesToSend, 0);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return;
	}
	printf("Bytes sent: %d\n", iSendResult);

	return 0;
}

COMMON_API int getData(SOCKET s, TCHAR *buf, int bufLen, int bytesToGet)
{
	if (buf == NULL || bufLen <= 0 || bytesToGet <= 0)
		return -1;
	
	if (bytesToGet > bufLen)
		return -1;

	ZeroMemory(buf, bufLen);

	int iResult = recv(s, buf, bytesToGet, 0);
	if (iResult > 0) {
		printf("Bytes received: %d\n", iResult);
	}
	else if (iResult == 0)
		printf("Connection closing...\n");
		// shutdown the connection since we're done
		iResult = shutdown(s, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(s);
			WSACleanup();
			return -1;
		}
	else  {
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return -1;
	}
	// cleanup
	closesocket(s);
}

COMMON_API int generateHMAC(TCHAR *studentID, int studentIDLen, TCHAR *challengeID, int challengeIDLen, TCHAR *oHash)
{
	sha1nfo s;
	uint8_t key[KEY_LENGTH];
	int keyLen;

	HANDLE hFile = CreateFile("keyFile.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		printf("Error opening keyfile.txt. Using default HMAC key.");
		
		if (sizeof(DEFAULT_KEY) >= KEY_LENGTH)
			keyLen = KEY_LENGTH;
		else
			keyLen = sizeof(DEFAULT_KEY);

		CopyMemory(key, DEFAULT_KEY, keyLen);
	}

	sha1_initHmac(&s, key, keyLen);
	sha1_write(&s, studentID, studentIDLen);
	sha1_write(&s, challengeID, challengeIDLen);
	uint8_t* hash = sha1_resultHmac(&s);

	//20 bytes in a SHA-1 HMAC
	wsprintf(oHash, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
		hash[0], hash[1], hash[2], hash[3],
		hash[4], hash[5], hash[6], hash[7],
		hash[8], hash[9], hash[10], hash[11],
		hash[12], hash[13], hash[14], hash[15],
		hash[16], hash[17], hash[18], hash[19]);
	
	return 0;
}
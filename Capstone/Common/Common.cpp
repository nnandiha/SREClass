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
#include <tchar.h>
#include <time.h>
#include <Shlwapi.h>

#include "sha1.h"

#define DEFAULT_BUFLEN 512

//// This is an example of an exported variable
//COMMON_API int nCommon=0;
//
//// This is an example of an exported function.
//COMMON_API int fnCommon(void)
//{
//	return 42;
//}

static const TCHAR DEFAULT_KEY[] = "PASSWORD";
static const int KEY_LENGTH = 32;
static const TCHAR PYTHON_PATH[] = "C:\\Python27\\python.exe";
static const TCHAR PYTHON_SCRIPT[] = "SubmitFlag.py";
static const DWORD RATE_LIMIT_DELAY = 5000;

DWORD WINAPI timeoutThread(LPVOID socket)
{
	Sleep(10000);
	closesocket((SOCKET)socket);
	return 0;
}

COMMON_API int startServerCore(_challengeInfo cInfo, BOOL suppressOutput, BOOL timeout)
{
	WSADATA wsaData;
	int iResult;
	unsigned long lastPeer = 0;
	DWORD lastTime = 0;

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
	iResult = getaddrinfo(NULL, cInfo.port, &hints, &result);
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
		printf("bind failed with error: %d. Are you already running another copy of this challenge?\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	if (suppressOutput == FALSE){
		printf("Challenge ID:\t%s\nDifficulty:\t%s\nCategory:\t%s\nDescription:\t%s\n", cInfo.challengeName, cInfo.difficulty, cInfo.category, cInfo.description);
		printf("Listening on port %s\n", cInfo.port);
	}

	if (timeout == TRUE){
		CreateThread(NULL, 0, timeoutThread, (LPVOID)ListenSocket, 0, NULL);
	}

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
			//WSACleanup();
			return 1;
		}

		//Rate limit connections - only based on last client and time
		unsigned long currentPeer = getPeerIP(ClientSocket);
		if (currentPeer == lastPeer) {
			DWORD currentTime = GetTickCount();
			if (currentTime - lastTime < RATE_LIMIT_DELAY){
				closesocket(ClientSocket);
				continue;
			}
			lastTime = currentTime;
		}
		lastPeer = currentPeer;

		TCHAR intro[512];
		iResult = _stprintf_s(intro, sizeof(intro), "Challenge Name:\t%s\nDifficulty:\t%s\nCategory:\t%s\nDescription:\t%s\n\nPlease enter your student ID: ", cInfo.challengeName, cInfo.difficulty, cInfo.category, cInfo.description);
		if (iResult < 0){
			printf("Error in _stprintf\n");
			closesocket(ClientSocket);
			continue;
		}

		iResult = sendData(ClientSocket, intro, iResult);
		if (iResult < 0){
			printf("Error sending intro\n");
			closesocket(ClientSocket);
			continue;
		}

		TP_WORK *work = CreateThreadpoolWork(cInfo.fCB, (PVOID)ClientSocket, NULL);
		if (work == NULL)
			printf("Error %d in CreateThreadpoolWork", GetLastError());
		else {
			SubmitThreadpoolWork(work);
			//WaitForThreadpoolWorkCallbacks(work, FALSE);
			CloseThreadpoolWork(work);
		}
	}
	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}

COMMON_API int startServer(_challengeInfo cInfo)
{
	return startServerCore(cInfo, FALSE, 0);
}

COMMON_API int sendData(SOCKET s, TCHAR *buf, int bytesToSend)
{
	if (buf == NULL || bytesToSend <= 0)
		return -1;

	if (buf[bytesToSend - 1] == '\0')
		bytesToSend--; //NULL bytes commonly added to C-strings. Not necessary to send.

	int iSendResult = send(s, buf, bytesToSend, 0);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(s);
		return -1;
	}
//	printf("Bytes sent: %d\n", iSendResult);

	return iSendResult;
}

COMMON_API int getData(SOCKET s, TCHAR *buf, int bytesToGet)
{
	if (buf == NULL || bytesToGet <= 0)
		return -1;
	
	int iResult = recv(s, buf, bytesToGet, 0);
	if (iResult > 0) {
//		printf("Bytes received: %d\n", iResult);
	}
	else if (iResult == 0){
		printf("Connection closing...\n");
		// shutdown the connection since we're done
		iResult = shutdown(s, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(s);
			return -1;
		}
	}
	else  {
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(s);
		return -1;
	}

	return iResult;
}

COMMON_API unsigned long getPeerIP(SOCKET s)
{
	SOCKADDR_IN peername;

	int size = sizeof(SOCKADDR_IN);
	if (getpeername(s, (SOCKADDR *)&peername, &size) != 0)
		return 0;

	return peername.sin_addr.S_un.S_addr;
}

COMMON_API void endComms(SOCKET s)
{
	closesocket(s);
}

COMMON_API int getPseudoRand()
{
	srand((unsigned int)time(NULL));
	return rand();
}

COMMON_API unsigned int popcnt32(unsigned int v)
{
	//Algorithm from http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel

	unsigned int c = 0;
	v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
	c = ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count

	return c;
}

/*
COMMON_API int generateHMAC(const TCHAR *studentID, int studentIDLen, const TCHAR *challengeID, int challengeIDLen, TCHAR *oHash, int oHashLen)
{
	sha1nfo s;
	uint8_t key[KEY_LENGTH];
	DWORD keyLen;

	if (oHashLen < HMAC_LENGTH)
		return -1;

	if (sizeof(DEFAULT_KEY) >= KEY_LENGTH)
		keyLen = KEY_LENGTH;
	else
		keyLen = sizeof(DEFAULT_KEY);

	CopyMemory(key, DEFAULT_KEY, keyLen);

	HANDLE hFile = CreateFile("keyFile.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		printf("Can't open keyfile.txt. Using default HMAC key.");
	}
	else{
		if (ReadFile(hFile, key, KEY_LENGTH, &keyLen, NULL) == FALSE){
			printf("Error reading keyfile.txt. Using default HMAC key.");
		}
		CloseHandle(hFile);
	}

	sha1_initHmac(&s, key, keyLen);
	sha1_write(&s, studentID, studentIDLen);
	sha1_write(&s, challengeID, challengeIDLen);
	uint8_t* hash = sha1_resultHmac(&s);

	//Convert the HMAC into an ASCII-printable version
	oHashLen = _stprintf_s(oHash, oHashLen, "%02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x%02x%02x",
		hash[0], hash[1], hash[2], hash[3],
		hash[4], hash[5], hash[6], hash[7],
		hash[8], hash[9], hash[10], hash[11],
		hash[12], hash[13], hash[14], hash[15],
		hash[16], hash[17], hash[18], hash[19]);
	
	return oHashLen;
}

COMMON_API int verifyHMAC(const TCHAR *studentID, int studentIDLen, const TCHAR *challengeID, int challengeIDLen, const TCHAR *userHash, int userHashLen)
{
	sha1nfo s;
	uint8_t key[KEY_LENGTH];
	DWORD keyLen;

	TCHAR checkHash[HMAC_LENGTH];

	if (userHashLen != HMAC_LENGTH)
		return -1;

	HANDLE hFile = CreateFile("keyFile.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		printf("Can't open keyfile.txt. Unable to verify HMAC.");
		return -2;
	}
	else{
		if (ReadFile(hFile, key, KEY_LENGTH, &keyLen, NULL) == FALSE){
			printf("Error reading keyfile.txt. Unable to verify HMAC.");
			CloseHandle(hFile);
			return -3;
		}
		CloseHandle(hFile);
	}

	sha1_initHmac(&s, key, keyLen);
	sha1_write(&s, studentID, studentIDLen);
	sha1_write(&s, challengeID, challengeIDLen);
	uint8_t* hash = sha1_resultHmac(&s);

	//Convert the HMAC into an ASCII-printable version
	_stprintf_s(checkHash, sizeof(checkHash), "%02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x%02x%02x",
		hash[0], hash[1], hash[2], hash[3],
		hash[4], hash[5], hash[6], hash[7],
		hash[8], hash[9], hash[10], hash[11],
		hash[12], hash[13], hash[14], hash[15],
		hash[16], hash[17], hash[18], hash[19]);

	//return oHashLen;
	if (strncmp(checkHash, userHash, HMAC_LENGTH) == 0)
		return 1;
	else
		return 0;
}*/

COMMON_API int submitFlag(const TCHAR *studentID, const TCHAR *challengeID, const TCHAR *difficulty, unsigned long peerIP)
{
	CHAR cmdLine[MAX_PATH];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	DWORD dwExitCode;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	STARTUPINFO *lpStartupInfo = &si;
	PROCESS_INFORMATION *lpProcessInformation = &pi;

	_snprintf_s(cmdLine, MAX_PATH * sizeof(cmdLine[0]), MAX_PATH, "%s %s %s %s %c %lu", PYTHON_PATH, PYTHON_SCRIPT, studentID, challengeID, difficulty[0], peerIP);

	if (PathFileExists(PYTHON_SCRIPT) == FALSE)
	{
		printf("Flag submission script not found.\n");
		return -2;
	}

	if (CreateProcess(NULL, cmdLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, lpStartupInfo, lpProcessInformation) == FALSE)
	{
		printf("Error creating process\n");
		return -1;
	}
	Sleep(3000);
	if(GetExitCodeProcess(pi.hProcess, &dwExitCode) == FALSE)
	{
		printf("Error getting return code\n");
		return -1;
	}

	printf("Child return code: %d\n", dwExitCode);

	if (dwExitCode == 0)
	{
		printf("Flag submitted successfully!\n");
		return 0;
	}
	else
	{
		printf("Error submitting flag.\n");
		return -1;
	}
}

COMMON_API BOOL validateUsername(const TCHAR *studentID, int studentIDLen)
{
	for (int i = 0; i < studentIDLen; i++){
		if (!_istalnum(studentID[i]) && studentID[i] != _T('\0'))
			return FALSE;
	}

	return TRUE;
}
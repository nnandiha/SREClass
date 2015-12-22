// DropListenerExe.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <time.h>

#include "Common.h"

static const TCHAR CHALLENGE_NAME[] = "ARE1";
static const TCHAR PORT[] = "39010";
static const TCHAR DESCRIPTION[] = "Can you beat the clock?";
static const TCHAR CATEGORY[] = "Anti-SRE";
static const TCHAR DIFFICULTY[] = "5/5";

/*
This challenge
*/

void __declspec (dllexport) NTAPI challenge2(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WORK work)
{
	TCHAR username[32];
	SOCKET s = (SOCKET)context;

	int usernameLen = getData(s, username, sizeof(username) - 1);
	if (usernameLen <= 0){
		endComms(s);
		return;
	}
	username[usernameLen - 1] = '\0'; //Clobber the \n that netcat adds
	printf("User connected: %s\n", username);

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
	endComms(s);
}

void __declspec (dllexport) NTAPI challenge(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WORK work)
{
	TCHAR tempPath[MAX_PATH];
	TCHAR tempFilename[MAX_PATH];
	TCHAR strSeed[16];
	TCHAR username[32];
	SOCKET s = (SOCKET)context;

	srand(time(NULL));

	unsigned int seed = (rand() % 25500) + 40000; //Get random number between 40000-65500
	_snprintf_s(strSeed, 16, 16, "%d", seed);

	//Drop child exe to disk
	HRSRC hRsrc = FindResource(GetModuleHandle(NULL), "IDR_CHILD", RT_RCDATA);
	if (hRsrc == NULL)
		return;

	HGLOBAL hExePtr = LoadResource(GetModuleHandle(NULL), hRsrc);
	if (hExePtr == NULL)
		return;

	DWORD exeSize = SizeofResource(GetModuleHandle(NULL), hRsrc);
	if (exeSize == 0)
		return;

	if (GetTempPath(MAX_PATH, tempPath) == 0)
		return;

	if (GetTempFileName(tempPath, "_da", 0, tempFilename) == 0)
		return;

	HANDLE hFile = CreateFile(tempFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile == NULL)
		return;

	DWORD bytesWritten;
	if (WriteFile(hFile, hExePtr, exeSize, &bytesWritten, NULL) == FALSE)
		return;

	if (CloseHandle(hFile) == FALSE)
		return;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (CreateProcess(tempFilename, strSeed, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi) == FALSE)
		return;

	DWORD waitResult = WaitForSingleObject(pi.hProcess, 10000);
	if (waitResult == WAIT_TIMEOUT){
		return;
	}

	DeleteFile(tempFilename);

	DWORD dwExitCode;
	if (GetExitCodeProcess(pi.hProcess, &dwExitCode) == FALSE)
		return;

	if (dwExitCode < 40000 || dwExitCode >= 65500)
		return;

	TCHAR newPort[8];
	int iResult = _stprintf_s(newPort, sizeof(newPort), "%d", dwExitCode);
	if (iResult < 0)
		return;
	
	int usernameLen = getData(s, username, sizeof(username) - 1);
	if (usernameLen <= 0){
		endComms(s);
		return;
	}
	username[usernameLen - 1] = '\0'; //Clobber the \n that netcat adds
	printf("User connected: %s\n", username);

	TCHAR chal1[32];
	iResult = _stprintf_s(chal1, sizeof(chal1), "Here is your clue: %d\n", seed);
	if (iResult < 0)
		return;
	if (sendData(s, chal1, iResult) < 0){
		endComms(s);
		return;
	}

	_challengeInfo cInfo;
	cInfo.challengeName = CHALLENGE_NAME;
	cInfo.port = newPort;
	cInfo.description = "";
	cInfo.category = "";
	cInfo.difficulty = "";
	cInfo.fCB = challenge2;

	startServerCore(cInfo, TRUE, TRUE);

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



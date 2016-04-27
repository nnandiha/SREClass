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

typedef int(__stdcall *tSubmitFlag)(const TCHAR *studentID, const TCHAR *challengeID, const TCHAR *difficulty);
typedef int(__stdcall *tGetData)(SOCKET s, TCHAR *buf, int bytesToGet);
typedef int(__stdcall *tSendData)(SOCKET s, TCHAR *buf, int bytesToSend);
typedef int(__stdcall *tEndComms)(SOCKET s);

static tSubmitFlag pSubmitFlag;
static tGetData pGetData;
static tSendData pSendData;
static tEndComms pEndComms;

static BYTE xorKey = 0x85;

/*
This challenge extracts another exe from its resource section and executes it, passing
a random number on the command line. The child process takes this number and steps an
internal LCG 243 times, then takes the final result and converts it into a high port
number between 40000-65500. It exits with this port number as a return code. The parent
process gets this return code and starts listening on that port. The student must
connect to this port within 10 seconds to solve this challenge.

LCG parameters: A=1103515245, C=12345, M=2^31

Anti-SRE techniques used: Disassembly desynchronization, Random exceptions thrown,
Loading dll functions by ordinal and calling via function pointers, important
string munged with a static 1-byte xor key.
*/

__forceinline void MEAN(){
	int r = rand() % 3;
	if (r == 0)
		RaiseException(rand(), 0, NULL, NULL);
	else if (r == 1)
		RaiseException(5 / (r - 1), 0, NULL, NULL);
	__asm {
		xor eax, eax
		_emit 0x74 //74 03 -> jz $+1
		_emit 0x01
		_emit 0xB9 //B9 XX XX XX XX -> mov ????
	}
}

void xorWithKey(TCHAR *buf, DWORD len, BYTE key)
{
	for (int i = 0; i < len; i++)
		buf[i] ^= key;
}

void NTAPI challenge2(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WORK work)
{
	TCHAR username[32];
	SOCKET s = (SOCKET)context;

	int usernameLen = pGetData(s, username, sizeof(username) - 1);
	if (usernameLen <= 0){
		pEndComms(s);
		return;
	}
	username[usernameLen - 1] = '\0'; //Clobber the \n that netcat adds
	if (validateUsername(username, usernameLen) == FALSE){
		pEndComms(s);
		return;
	}

	try{ MEAN(); }
	catch (...){}
	int result = pSubmitFlag(username, CHALLENGE_NAME, DIFFICULTY);
	if (result == 0){
		try{ MEAN(); }
		catch (...){}
		//TCHAR chal2[] = "Congratulations! Your flag has been submitted.\n";
		TCHAR chal2[] = "\xc6\xea\xeb\xe2\xf7\xe4\xf1\xf0\xe9\xe4\xf1\xec\xea\xeb\xf6\xa4\xa5\xdc\xea\xf0\xf7\xa5\xe3\xe9\xe4\xe2\xa5\xed\xe4\xf6\xa5\xe7\xe0\xe0\xeb\xa5\xf6\xf0\xe7\xe8\xec\xf1\xf1\xe0\xe1\xab\n";
		xorWithKey(chal2, sizeof(chal2) - 2, 0x85);
		pSendData(s, chal2, sizeof(chal2));
	}
	else if (result == -2){
		try{ MEAN(); }
		catch (...){}
		//TCHAR chal2[] = "You have solved this challenge, but the flag submission script was not found.\n";
		TCHAR chal2[] = "\xdc\xea\xf0\xa5\xed\xe4\xf3\xe0\xa5\xf6\xea\xe9\xf3\xe0\xe1\xa5\xf1\xed\xec\xf6\xa5\xe6\xed\xe4\xe9\xe9\xe0\xeb\xe2\xe0\xa9\xa5\xe7\xf0\xf1\xa5\xf1\xed\xe0\xa5\xe3\xe9\xe4\xe2\xa5\xf6\xf0\xe7\xe8\xec\xf6\xf6\xec\xea\xeb\xa5\xf6\xe6\xf7\xec\xf5\xf1\xa5\xf2\xe4\xf6\xa5\xeb\xea\xf1\xa5\xe3\xea\xf0\xeb\xe1\xab\n";
		xorWithKey(chal2, sizeof(chal2) - 2, 0x85);
		pSendData(s, chal2, sizeof(chal2));
	}
	else{
		try{ MEAN(); }
		catch (...){}
		//TCHAR chal2[] = "You solved this challenge, but there was a problem submitting your flag.\n";
		TCHAR chal2[] = "\xdc\xea\xf0\xa5\xf6\xea\xe9\xf3\xe0\xe1\xa5\xf1\xed\xec\xf6\xa5\xe6\xed\xe4\xe9\xe9\xe0\xeb\xe2\xe0\xa9\xa5\xe7\xf0\xf1\xa5\xf1\xed\xe0\xf7\xe0\xa5\xf2\xe4\xf6\xa5\xe4\xa5\xf5\xf7\xea\xe7\xe9\xe0\xe8\xa5\xf6\xf0\xe7\xe8\xec\xf1\xf1\xec\xeb\xe2\xa5\xfc\xea\xf0\xf7\xa5\xe3\xe9\xe4\xe2\xab\n";
		xorWithKey(chal2, sizeof(chal2) - 2, 0x85);
		pSendData(s, chal2, sizeof(chal2));
	}
	pEndComms(s);
}

void __declspec (dllexport) NTAPI challenge(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WORK work)
{
	TCHAR tempPath[MAX_PATH];
	TCHAR tempFilename[MAX_PATH];
	TCHAR strSeed[16];
	TCHAR username[32];
	SOCKET s = (SOCKET)context;

	unsigned long peerIP = getPeerIP(s);
	if (peerIP == 0){
		endComms(s);
		return;
	}

	srand(time(NULL));

	try{ MEAN(); }
	catch (...){}

	unsigned int seed = (rand() % 25500) + 40000; //Get random number between 40000-65500
	_snprintf_s(strSeed, 16, 16, "%d", seed);

	//Drop child exe to disk
	try{ MEAN(); }
	catch (...){}
	HRSRC hRsrc = FindResource(GetModuleHandle(NULL), "IDR_CHILD", RT_RCDATA);
	if (hRsrc == NULL)
		return;

	try{ MEAN(); }
	catch (...){}
	HGLOBAL hExePtr = LoadResource(GetModuleHandle(NULL), hRsrc);
	if (hExePtr == NULL)
		return;

	try{ MEAN(); }
	catch (...){}
	DWORD exeSize = SizeofResource(GetModuleHandle(NULL), hRsrc);
	if (exeSize == 0)
		return;

	try{ MEAN(); }
	catch (...){}
	if (GetTempPath(MAX_PATH, tempPath) == 0)
		return;

	try{ MEAN(); }
	catch (...){}
	if (GetTempFileName(tempPath, "_da", 0, tempFilename) == 0)
		return;

	try{ MEAN(); }
	catch (...){}
	HANDLE hFile = CreateFile(tempFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile == NULL)
		return;

	try{ MEAN(); }
	catch (...){}
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

	try{ MEAN(); }
	catch (...){}
	if (CreateProcess(tempFilename, strSeed, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi) == FALSE)
		return;

	DWORD waitResult = WaitForSingleObject(pi.hProcess, 10000);
	if (waitResult == WAIT_TIMEOUT){
		return;
	}

	try{ MEAN(); }
	catch (...){}
	DeleteFile(tempFilename);

	DWORD dwExitCode;
	if (GetExitCodeProcess(pi.hProcess, &dwExitCode) == FALSE)
		return;

	if (dwExitCode < 40000 || dwExitCode >= 65500)
		return;

	try{ MEAN(); }
	catch (...){}
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

	try{ MEAN(); }
	catch (...){}
	_challengeInfo cInfo;
	cInfo.challengeName = CHALLENGE_NAME;
	try{ MEAN(); }
	catch (...){}
	cInfo.port = newPort;
	cInfo.description = "";
	cInfo.category = "";
	cInfo.difficulty = "5/5";
	try{ MEAN(); }
	catch (...){}
	cInfo.fCB = challenge2;

	try{ MEAN(); }
	catch (...){}
	startServerCore(cInfo, FALSE, TRUE);

	if (strlen(username) == 42){
		int result = submitFlag(username, CHALLENGE_NAME, DIFFICULTY, peerIP);
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

	endComms(s);
}

int _tmain(int argc, _TCHAR* argv[])
{
	HMODULE hDll = LoadLibrary("Common.dll");

	pSubmitFlag = (tSubmitFlag)GetProcAddress(hDll, MAKEINTRESOURCE(8));
	pGetData = (tGetData)GetProcAddress(hDll, MAKEINTRESOURCE(3));
	pSendData = (tSendData)GetProcAddress(hDll, MAKEINTRESOURCE(5));
	pEndComms = (tEndComms)GetProcAddress(hDll, MAKEINTRESOURCE(1));

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



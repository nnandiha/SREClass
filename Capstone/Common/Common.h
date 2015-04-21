// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the COMMON_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// COMMON_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef COMMON_EXPORTS
#define COMMON_API __declspec(dllexport)
#else
#define COMMON_API __declspec(dllimport)
#endif

#include <WinSock2.h>

//extern COMMON_API int nCommon;

//COMMON_API int fnCommon(void);

struct _challengeInfo{
	const TCHAR *challengeName;
//	DWORD challengeNameLen;
	const TCHAR *port;
//	DWORD portLen;
	const TCHAR *category;
//	DWORD categoryLen;
	const TCHAR *description;
//	DWORD descriptionLen;
	const TCHAR *difficulty;
//	DWORD difficultyLen;
	PTP_WORK_CALLBACK fCB;
};

//Communications functions
COMMON_API int startServer(_challengeInfo cInfo);
COMMON_API int sendData(SOCKET s, TCHAR *buf, int bufLen, int bytesToSend);
COMMON_API int getData(SOCKET s, TCHAR *buf, int bufLen, int bytesToGet);

COMMON_API int generateHMAC(TCHAR *studentID, int studentIDLen, TCHAR *challengeID, int challengeIDLen, TCHAR *oHash);
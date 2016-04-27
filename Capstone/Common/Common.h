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
	const TCHAR *port;
	const TCHAR *category;
	const TCHAR *description;
	const TCHAR *difficulty;
	PTP_WORK_CALLBACK fCB;
};

//Communications functions
COMMON_API int startServer(_challengeInfo cInfo);
COMMON_API int startServerCore(_challengeInfo cInfo, BOOL suppressOutput, BOOL timeout);
COMMON_API int sendData(SOCKET s, TCHAR *buf, int bytesToSend);
COMMON_API int getData(SOCKET s, TCHAR *buf, int bytesToGet);
COMMON_API unsigned long getPeerIP(SOCKET s);
COMMON_API void endComms(SOCKET s);

//Math Functions
COMMON_API int getPseudoRand();
//COMMON_API int generateHMAC(const TCHAR *studentID, int studentIDLen, const TCHAR *challengeID, int challengeIDLen, TCHAR *oHash, int oHashLen);
//COMMON_API int verifyHMAC(const TCHAR *studentID, int studentIDLen, const TCHAR *challengeID, int challengeIDLen, const TCHAR *hash, int hashLen);

//Flag functions
COMMON_API int submitFlag(const TCHAR *studentID, const TCHAR *challengeID, const TCHAR *difficulty, unsigned long peerIP);

//Security Functions
COMMON_API int validateUsername(const TCHAR *studentID, int studentIDLen);
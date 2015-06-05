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

//"XXXXXXXX-XXXXXXXX-XXXXXXXX-XXXXXXXX-XXXXXXXX\0"
static const int HMAC_LENGTH = 45; //Number of bytes needed to store our formatted HMAC

//Communications functions
COMMON_API int startServer(_challengeInfo cInfo);
COMMON_API int sendData(SOCKET s, TCHAR *buf, int bytesToSend);
COMMON_API int getData(SOCKET s, TCHAR *buf, int bytesToGet);
COMMON_API void endComms(SOCKET s);

//Math Functions
COMMON_API int getPseudoRand();
COMMON_API int generateHMAC(const TCHAR *studentID, int studentIDLen, const TCHAR *challengeID, int challengeIDLen, TCHAR *oHash, int oHashLen);
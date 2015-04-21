// Decompiling.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// ToUpper
int __cdecl block1(char *str, unsigned int len)
{
	int cnt = 0;
	for (int i = 0; i < len; i++){
		if (str[i] >= 'a' && str[i] <= 'z'){
			str[i] -= 0x20;
			cnt++;
		}
	}

	return cnt;
}

//Compute average
int __fastcall block2(int intArray[], int arrayLen, int &avg)
{
	int tmp = 0;

	if (arrayLen > 100)
		return -1;

	for (int i = 0; i < arrayLen; i++){
		tmp += intArray[i];
	}

	avg = tmp / arrayLen;
	return 0;
}

//Find a character in a string
char * __stdcall block3(char *str, unsigned int len, char c)
{
	char *endPtr = str + len;
	if (endPtr < str)
		return NULL;

	while (str < endPtr)
	{
		if (*str == c){
			return str;
		}
		str++;
	}

	return NULL;
}

int _tmain(int argc, _TCHAR* argv[])
{
	char str1[] = "AaAaAaAa";
	block1(str1, sizeof(str1)/sizeof(str1[0]));

	int a[] = { 15, 432, 32, 123, 243598, 8432 };
	int avg;
	block2(a, sizeof(a)/sizeof(a[0]), avg);

	char str2[] = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";
	block3(str2, sizeof(str2)/sizeof(str2[0]), 'g');

	return 0;
}


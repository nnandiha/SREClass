// Lab1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string.h>

struct _example{
	int int1;
	char char1;
	short short1;
	int unused;
	short short2;
	double double1;
	char buf[16];
};

int initStruct(_example &ex, int int1, char char1, short short1, unsigned short short2, char *buf)
{
	ex.int1 = int1;
	ex.char1 = char1;
	ex.short1 = short1;
	ex.short2 = short2;
	strcpy_s(ex.buf, sizeof(ex.buf), buf);

	return 0;
}

int copyStruct(_example &ex1, const _example ex2)
{
	ex1.int1 = ex2.int1;
	ex1.char1 = ex2.char1;
	ex1.short1 = ex2.short1;
	ex1.short2 = ex2.short2;

	strcpy_s(ex1.buf, sizeof(ex1.buf), ex2.buf);

	return 0;
}

int addStructs(_example ex1, _example ex2, _example &ex3)
{
	ex3.int1 = ex1.int1 + ex2.int1;
	ex3.char1 = ex1.char1 + ex2.char1;
	ex3.short1 = ex1.short1 + ex2.short1;
	ex3.short2 = ex1.short2 + ex2.short2;

	size_t len1 = strlen(ex1.buf);

	
	strcpy_s(ex3.buf, sizeof(ex3.buf), ex1.buf);
	strcat_s(ex3.buf, sizeof(ex3.buf) - len1, ex2.buf);

	return 0;
}

int printStruct(const _example ex)
{
	printf("%d, %c, %d, %d, %s\n", ex.int1, ex.char1, ex.short1, ex.short2, ex.buf);
	return 0;
}

__declspec(dllexport) int main(int argc, char **argv)
{
	_example ex1;
	_example ex2;
	_example ex3;
	
	initStruct(ex1, 4, 'B', 544, 239, "Hello");
	initStruct(ex2, 654, 'H', -4352, 12, "World");

	copyStruct(ex3, ex1);
	printStruct(ex3);

	addStructs(ex1, ex2, ex3);
	printStruct(ex3);

	return 0;
}


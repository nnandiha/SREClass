// Structures.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

struct example{
	int a;
	int b;
};


int func(example &ex)
{
	ex.a = ex.b;
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	example ex;
	ex.a = 1;
	ex.b = 2;

	func(ex);

	return 0;
}


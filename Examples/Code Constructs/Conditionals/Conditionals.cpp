// Conditionals.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int x = 0;
	scanf("Enter a number: %d", &x);

	if (x < 3){
		printf("%d is too small!\n", x);
	}
	else{
		printf("%d is big enough!\n", x);
	}
	return 0;
}


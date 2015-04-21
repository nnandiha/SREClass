// Switches.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int x = 0;

	scanf("Enter a number: %d", &x);

	switch (x){
	case 1:
		printf("%d is too small!\n", x);
		break;
	case 2:
		printf("%d is a little small!\n", x);
		break;
	case 3:
		printf("%d is just right!\n", x);
		break;
	case 4:
		printf("%d is a little big!\n", x);
		break;
	default:
		printf("%d is too big... or is it?!\n", x);
	}
	return 0;
}


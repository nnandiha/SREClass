// Loops.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int i;
	int j;
	int k;

	for (i = 0; i < 10; i++) {
		printf("i = %d\n", i);
	}

	j = 0;
	while (j < 10){
		j++;
		printf("j = %d\n", j);
	}

	k = 0;
	do{
		k++;
		printf("k = %d\n", k);
	} while (k < 10);

	return 0;
}


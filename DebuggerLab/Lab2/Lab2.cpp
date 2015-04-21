// Lab2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string.h>
#include <stdlib.h>

const char alphabet[] = " ABCDEFGHIJKLMNOPQRSTUWVXYZ";
const char password[] = "DROWSSAP";


/*
Each argument on the command line is treated as a numeric index into
the "alphabet" array. This challenge requires that the command line input
spells out "PASSWORD" backwards. The correct sequence is:
4 18 15 22 19 19 1 16

Note that V and W are swapped in the array. (on purpose) Tricky! :-P
*/
__declspec (dllexport) int main(int argc, char **argv)
{
	bool matched = false;
	char input[64];
	memset(input, 0, sizeof(input));

	for (int i = 1; i < 63; i++){
		
		if (argc <= i)
			break;
		
		int num = atoi(argv[i]);
		if (num == 0)
			break;

		input[i - 1] = alphabet[num % sizeof(alphabet)];
	}
	if (strcmp(password, input) == 0)
		matched = true;

	if (matched == true)
		printf("Congratulations! You have successfully completed this challenge.\n");

	return 0;
}


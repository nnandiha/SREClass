// Lab1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string.h>

//const char MATCH_STRING[] = "Reverse Engineering Is Fun!";
const char RAND_STRING[] = "CPC9DK76H7QBAUSTUY659D131QJ";
const char XOR_STRING[] = { '\x11', '5', '5', '\\', '6', '8', 'R', '\x16', '\r', 'Y', '6', '+', '/', '0', '6', '&', '<', '7', 'Q', '\x15', 'p', '7', '\x11', 'u', 'D', '?', 'k'};

__declspec (dllexport) int main(int argc, char **argv)
{
	if (argc != 2){
		printf("Sorry!");
		return 0;
	}

	size_t len = strlen(argv[1]);
	if (len != sizeof(XOR_STRING)){
		printf("Sorry!"); 
		return 0;
	}

	char match_string[sizeof(XOR_STRING)];
	for (int i = 0; i < sizeof(XOR_STRING); i++){
		match_string[i] = RAND_STRING[i] ^ XOR_STRING[i];
	}

	if (memcmp(match_string, argv[1], sizeof(match_string)) != 0){
		printf("Sorry!");
		return 0;
	}

	printf("Congratulations! You have solved this exercise.");

	return 0;
}


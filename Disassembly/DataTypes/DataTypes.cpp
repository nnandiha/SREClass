// DataTypes.cpp : Defines the entry point for the console application.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char stringToMatch[] = "BESTCLASSEVER";
int intToMatch = 9998;
unsigned int hexToMatch = 0xFEEDFACE;
float floatToMatch = 423.72;

int matchString(){
	char buf[128];
	if (scanf("%64s", buf) != 1){
		return -1;
	}

	printf("Please enter the correct string: ");
	if (strncmp(stringToMatch, buf, sizeof(stringToMatch)) == 0){
		return 0;
	}
	else{
		return -1;
	}
}

int matchInt(){
	unsigned int num = 0;

	printf("Please enter the correct number: ");
	if (scanf("%u", &num) != 1){
		return -1;
	}

	if (num == intToMatch){
		return 0;
	}
	else{
		return -1;
	}
}

int matchHex(){
	unsigned int num = 0;

	printf("Please enter the correct number: ");
	if (scanf("%x", &num) != 1){
		return -1;
	}

	if (num == hexToMatch){
		return 0;
	}
	else{
		return -1;
	}
}

int matchFloat(){
	float num = 0.0;

	printf("Please enter the correct number: ");
	if (scanf("%f", &num) != 1){
		return -1;
	}

	if (num == floatToMatch){
		return 0;
	}
	else{
		return -1;
	}
}

__declspec (dllexport) int main(int argc, char ** argv)
{
	unsigned int sel = 0;
	printf("Please enter a number: ");
	if (scanf("%u", &sel) != 1){
		printf("Error!");
		return -1;
	}

	sel %= 4;
	int result = -1;

	switch (sel){
	case 0:
		result = matchString();
		break;
	case 1:
		result = matchInt();
		break;
	case 2:
		result = matchHex();
		break;
	case 3:
		result = matchFloat();
		break;
	default:
		//should never get here
		printf("Hmm, that shouldn't have happened...\n");
		break;
	}

	if (result == 0){
		printf("Success!\n");
	}
	else{
		printf("Sorry, please try again.\n");
	}

	return 0;
}


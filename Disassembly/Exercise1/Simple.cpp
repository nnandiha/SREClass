// Exercise1.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

__declspec (dllexport) int main(int argc, char ** argv)
{
	srand(time(NULL));

	unsigned int a = rand();
	unsigned int b = rand();

	unsigned int c = a^b;

	printf("Here are your clues: %u %u\n", a, b);
	printf("Please enter the correct response: ");

	unsigned int usr = 0;
	scanf("%u", &usr);

	if (usr == c){
		printf("That's correct!\n");
	}
	else{
		printf("Nope. Try again.\n");
	}

	return 0;
}


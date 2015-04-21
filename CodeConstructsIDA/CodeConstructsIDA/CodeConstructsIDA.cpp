// CodeConstructsIDA.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string.h>

bool check1(char *buf)
{
	if (strncmp(buf, "EASY", 5) == 0)
		return true;
	else
		return false;
}


/*
Requirements:
First character, minus 0x30, must be the length of the argument
Length must be 2 or greater
Second character must be A, B, or H
*/
bool check2(char *buf)
{
	int test1 = 0;

	size_t len = strlen(buf);

	if (len != (buf[0] - 0x30))
		return false;

	if (len <= 2)
		return false;

	switch (buf[1])
	{
	case 0x41:
	case 0x42:
	case 0x48:
		test1 = 1;
		break;
	case 0x61:
		test1 = 2;
		break;
	default:
		test1 = 3;
	}

	if (test1 == 1)
		return true;
	else
		return false;
}

/*
Requirements:
Length is 5 or 6
Character 0 is 'X'
Character 1 is a number
Character 2 is 'a' or 'A'
Character 3 is not alphanumeric
If buf len is 4, character 4 must equal character 3
If buf len is 5, character 5 must equal character 2
*/
bool check3(char *buf)
{
	size_t len = strlen(buf);
	if (len < 5)
		return false;

	if (len > 6)
		return false;

	if (buf[0] != 'X')
		return false;

	if (buf[1] < '0' || buf[1] > '9')
		return false;

	if (buf[2] != 'a' && buf[2] != 'A')
		return false;

	if ((buf[3] >= 'A' && buf[3] <= 'z') || (buf[3] >= '0' && buf[3] <= '9'))
		return false;

	if (len == 5)
	{
		if (buf[4] != buf[3])
			return false;
	}
	else
	{
		if (buf[5] != buf[2])
			return false;
	}

	return true;
}

__declspec (dllexport) int main(int argc, char ** argv)
{
	if (argc != 4)
	{
		printf("Not even close!\n");
		return 0;
	}

	if (!check1(argv[1]))
	{
		printf("You're off to a good start!");
		return 0;
	}

	if (!check2(argv[3]))
	{
		printf("Almost there!");
		return 0;
	}

	if (check3(argv[2]))
	{
		printf("Congratulations! You have solved this exercise.");
		return 0;
	}

	return 0;
}


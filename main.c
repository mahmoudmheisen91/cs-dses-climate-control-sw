// Climate Control Software:

// Including Libraries:
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ccslib.h"

// Global Variables:
double desired;

// main function:
int main(int argc, char **argv) {

	extern_argc = argc;

	// Initialization:
	init();

	// Reading desired value from command line:
	int strsize = strlen(argv[1]);
	char* desiredChar = malloc(strsize);
	strcat(desiredChar, argv[1]);
	desiredChar[strsize] = '\0';
	desired = (double) atof(desiredChar)/ 1000;

	// Infinite Loop:
	while (1) {
		// TASK 1: The Local Controller:
		localController(desired);
	}

	return SUCCESS;
}

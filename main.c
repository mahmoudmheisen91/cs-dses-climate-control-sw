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

	// TODO: MESSY
	// Reading desired value from command line:
	int strsize = strlen(argv[1]);
	char* desiredChar = malloc(strsize);
	strcat(desiredChar, argv[1]);
	desiredChar[strsize] = '\0';
	desired = (double) atof(desiredChar)/ 1000;

	// Infinite Loop:
	while (1) {
		// TODO: MESSY
		// TODO: sleep()
		// TASK 1: The Local Controller:
		double actual;
		int command;
		char sensorTempData[5];
		char commandStr[15];

		// Read Sensor Data:
		readData("/dev/temp_sensor", sensorTempData);
		actual = (double) atof(sensorTempData)/ 1000;

		// command (set level) to knob:
		command = PIDcontroller(desired, actual);
		sprintf(commandStr, "%d", command);

		// Write data to knob:
		writeData("/dev/temp_knob", commandStr);

		// Print to screen:
		printf("Sensor Reading = %.2f°C  ==>  Knob Level = %s\n", actual, commandStr);



		// Important for RT:
		sleep(1);
	}

	return SUCCESS;
}

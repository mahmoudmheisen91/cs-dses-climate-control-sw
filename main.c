// Climate Control Software:

// Including Libraries:
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ccslib.h"

// Global Variables:
double actual;
double desired;
int command;
char sensorTempData[5];

// main function:
int main(int argc, char **argv) {

	if (argc != 2) {
		fprintf(stderr, "Error running software!\n");
		fprintf(stderr, "Mismatch number of argument\n");
		fprintf(stderr, "E.g run as follow..........\n");
		fprintf(stderr, "    ./climateControlSoftware 22500\n");
		fprintf(stderr, "Terminating program..........\n");
		fprintf(stderr, "Terminating program..........Done\n");
		FAIL;
	}

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

		// Read Sensor Data:
		readData("/dev/temp_sensor", sensorTempData);
		actual = (double) atof(sensorTempData)/ 1000;

		// command (set level) to knob:
		command = PIDcontroller(desired, actual);
		char commandStr[15];
		sprintf(commandStr, "%d", command);

		// Write data to knob:
		writeData("/dev/temp_knob", commandStr);
		// Print to log file:
		printf("Sensor Reading = %.2f°C  ==>  Knob Level = %s\n", actual, commandStr);

		// Important for RT:
		sleep(1);
	}

	return SUCCESS;
}
